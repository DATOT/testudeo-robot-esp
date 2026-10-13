#include "./DeviceManager.hpp"
/**-----------------------
 * !      WARNING
 * ! Không được thay đổi code ở đây
 *------------------------**/
DeviceManager::DeviceManager()
    : _started(false),
      _stopping(false),
      _setup_done(false)
{
    _manager_mutex = xSemaphoreCreateMutex();
}
DeviceManager::~DeviceManager()
{
    stop(portMAX_DELAY);

    if (_manager_mutex != nullptr)
    {
        for (DeviceSlot *slot : _devices)
        {
            if (slot->task_mutex) vSemaphoreDelete(slot->task_mutex);
            if (slot->loop_mutex) vSemaphoreDelete(slot->loop_mutex);
            if (slot->stopped)    vSemaphoreDelete(slot->stopped);
            delete slot;
        }

        _devices.clear();

        vSemaphoreDelete(_manager_mutex);
        _manager_mutex = nullptr;
    }
}

bool DeviceManager::_lock_mutex(
    SemaphoreHandle_t mutex,
    TickType_t timeout)
{
    return mutex != nullptr &&
           xSemaphoreTake(mutex, timeout) == pdTRUE;
}

void DeviceManager::_unlock_mutex(SemaphoreHandle_t mutex)
{
    if (mutex != nullptr)
    {
        xSemaphoreGive(mutex);
    }
}
bool DeviceManager::add(BaseDevice &device)
{
    if (!_lock_mutex(_manager_mutex))
        return false;

    if (_started || _stopping)
    {
        _unlock_mutex(_manager_mutex);
        return false;
    }

    DeviceSlot *slot = new (std::nothrow) DeviceSlot(&device);
    if (slot == nullptr)
    {
        _unlock_mutex(_manager_mutex);
        return false;
    }

    slot->task_mutex = xSemaphoreCreateMutex();
    slot->loop_mutex = xSemaphoreCreateMutex();
    slot->stopped    = xSemaphoreCreateBinary();

    if (slot->task_mutex == nullptr ||
        slot->loop_mutex == nullptr ||
        slot->stopped    == nullptr)
    {
        if (slot->task_mutex) vSemaphoreDelete(slot->task_mutex);
        if (slot->loop_mutex) vSemaphoreDelete(slot->loop_mutex);
        if (slot->stopped)    vSemaphoreDelete(slot->stopped);
        delete slot;
        _unlock_mutex(_manager_mutex);
        return false;
    }

    device.set_manager(this);
    _devices.push_back(slot);

    _unlock_mutex(_manager_mutex);
    return true;
}

template <typename T>
inline std::optional<T *> DeviceManager::get_first_device()
{
    for (const auto* slot : _devices)
    {
        if (slot == nullptr || slot->device == nullptr)
            continue;

        if (auto* device = dynamic_cast<T*>(slot->device))
            return device;
    }

    return std::nullopt;
}

void DeviceManager::setup()
{
    if (!_lock_mutex(_manager_mutex))
        return;

    if (_started || _stopping || _setup_done)
    {
        _unlock_mutex(_manager_mutex);
        return;
    }

    for (DeviceSlot *slot : _devices)
    {
        if (_lock_mutex(slot->task_mutex))
        {
            slot->device->setup();
            _unlock_mutex(slot->task_mutex);
        }
    }

    _setup_done = true;
    _unlock_mutex(_manager_mutex);
}

bool DeviceManager::start()
{
    if (!_lock_mutex(_manager_mutex))
        return false;

    if (_started || _stopping || !_setup_done)
    {
        _unlock_mutex(_manager_mutex);
        return false;
    }

    _started = true;

    for (DeviceSlot *slot : _devices)
    {
        slot->task = nullptr;

        // loop() sẽ được gọi tuần hoàn trong DeviceManager::loop()
        slot->loop_enabled = slot->device->has_loop();

        if (slot->device->has_task())
        {
            slot->running = true;

            // Xóa sạch tín hiệu stopped cũ (nếu có)
            while (xSemaphoreTake(slot->stopped, 0) == pdTRUE) {}

            BaseType_t result = xTaskCreate(
                _task_entry,
                "device_task",
                4096,
                slot,
                1,
                &slot->task);

            if (result != pdPASS)
            {
                // Rollback: dừng tất cả các task đã tạo
                for (DeviceSlot *s : _devices)
                    s->running = false;

                _unlock_mutex(_manager_mutex);
                stop(portMAX_DELAY);
                return false;
            }
        }
        else
        {
            slot->running = false;
        }
    }

    _unlock_mutex(_manager_mutex);
    return true;
}

void DeviceManager::_task_entry(void *arg)
{
    DeviceSlot *slot = static_cast<DeviceSlot *>(arg);

    while (slot->running)
    {
        if (xSemaphoreTake(slot->task_mutex, portMAX_DELAY) == pdTRUE)
        {
            if (slot->running)
            {
                slot->device->task();
            }
            xSemaphoreGive(slot->task_mutex);
        }

        vTaskDelay(pdMS_TO_TICKS(1));
    }

    xSemaphoreGive(slot->stopped);
    vTaskDelete(nullptr);
}

bool DeviceManager::stop(TickType_t timeout)
{
    if (!_lock_mutex(_manager_mutex))
        return false;

    if (!_started)
    {
        _unlock_mutex(_manager_mutex);
        return true;
    }

    _stopping = true;

    // Yêu cầu tất cả task dừng
    for (DeviceSlot *slot : _devices)
    {
        if (slot->task != nullptr)
            slot->running = false;
    }

    _unlock_mutex(_manager_mutex);

    TickType_t started_at = xTaskGetTickCount();

    // Chờ từng task kết thúc
    for (DeviceSlot *slot : _devices)
    {
        if (slot->task == nullptr)
            continue;

        TickType_t wait_ticks = timeout;

        if (timeout != portMAX_DELAY)
        {
            TickType_t elapsed = xTaskGetTickCount() - started_at;

            if (elapsed >= timeout)
                wait_ticks = 0;
            else
                wait_ticks = timeout - elapsed;
        }

        if (xSemaphoreTake(slot->stopped, wait_ticks) != pdTRUE)
            return false;
    }

    // Đảm bảo không còn ai đang giữ mutex (task/loop/on_command đã xong)
    for (DeviceSlot *slot : _devices)
    {
        if (!_lock_mutex(slot->task_mutex, timeout)) return false;
        _unlock_mutex(slot->task_mutex);

        if (!_lock_mutex(slot->loop_mutex, timeout)) return false;
        _unlock_mutex(slot->loop_mutex);
    }

    if (!_lock_mutex(_manager_mutex))
        return false;

    for (DeviceSlot *slot : _devices)
        slot->task = nullptr;

    _started  = false;
    _stopping = false;

    _unlock_mutex(_manager_mutex);
    return true;
}

void DeviceManager::loop()
{
    // Chạy loop() của từng device tuần tự (không song song với nhau)
    // nhưng song song với task() của chính device đó.
    for (DeviceSlot *slot : _devices)
    {
        if (!slot->loop_enabled)
            continue;

        if (xSemaphoreTake(slot->loop_mutex, portMAX_DELAY) == pdTRUE)
        {
            slot->device->loop();
            xSemaphoreGive(slot->loop_mutex);
        }
    }

    vTaskDelay(pdMS_TO_TICKS(1));
}

bool DeviceManager::is_running() const
{
    DeviceManager *self = const_cast<DeviceManager *>(this);

    if (!_lock_mutex(self->_manager_mutex))
        return false;

    bool result = self->_started && !self->_stopping;

    _unlock_mutex(self->_manager_mutex);
    return result;
}
