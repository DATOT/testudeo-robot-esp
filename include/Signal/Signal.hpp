#ifndef __SIGNAL_HPP__
#define __SIGNAL_HPP__

#include <Arduino.h>
#include <functional>
#include <vector>
#include <utility>

#define signals public
/**-----------------------
 * !      WARNING
 * ! Không được thay đổi code ở đây
 *------------------------**/
template<typename... Args>
class Signal
{
public:
    using Slot = std::function<void(Args...)>;

    Signal() = default;

    Signal(const Signal &) = delete;
    Signal &operator=(const Signal &) = delete;

    ~Signal() { stop(); }

    bool start(UBaseType_t      priority    = 1,
               uint32_t         stack_words = 4096,
               UBaseType_t      queue_len   = 8)
    {
        if (_task != nullptr) return true;

        _queue = xQueueCreate(queue_len, sizeof(Job *));
        if (_queue == nullptr) return false;

        _slots_mutex = xSemaphoreCreateMutex();
        if (_slots_mutex == nullptr)
        {
            vQueueDelete(_queue);
            _queue = nullptr;
            return false;
        }

        _running = true;

        BaseType_t ok = xTaskCreate(
            worker_entry, "signal", stack_words, this, priority, &_task);

        if (ok != pdPASS)
        {
            _running = false;
            vSemaphoreDelete(_slots_mutex); _slots_mutex = nullptr;
            vQueueDelete(_queue);           _queue       = nullptr;
            _task = nullptr;
            return false;
        }
        return true;
    }

    void stop(TickType_t timeout = portMAX_DELAY)
    {
        if (_task == nullptr) return;

        _running = false;

        Job *sentinel = nullptr;
        xQueueSend(_queue, &sentinel, 0);

        TickType_t start = xTaskGetTickCount();
        while (eTaskGetState(_task) != eDeleted)
        {
            if (timeout != portMAX_DELAY &&
                (xTaskGetTickCount() - start) >= timeout)
                break;
            vTaskDelay(pdMS_TO_TICKS(1));
        }

        Job *job = nullptr;
        while (xQueueReceive(_queue, &job, 0) == pdTRUE)
            delete job;

        vQueueDelete(_queue);           _queue       = nullptr;
        vSemaphoreDelete(_slots_mutex); _slots_mutex = nullptr;
        _task = nullptr;
    }

    void connect(Slot slot)
    {
        if (_task == nullptr) start();

        if (_slots_mutex &&
            xSemaphoreTake(_slots_mutex, portMAX_DELAY) == pdTRUE)
        {
            _slots.push_back(std::move(slot));
            xSemaphoreGive(_slots_mutex);
        }
    }

    bool emit(Args... args)
    {
        if (_task == nullptr && !start()) return false;

        Job *job = new (std::nothrow) Job{ std::make_tuple(std::move(args)...) };
        if (job == nullptr) return false;

        if (xQueueSend(_queue, &job, 0) != pdTRUE)
        {
            delete job;
            return false;
        }
        return true;
    }

    bool running() const { return _running; }

private:
    struct Job { std::tuple<Args...> args; };

    static void worker_entry(void *arg)
    {
        auto *self = static_cast<Signal *>(arg);

        while (self->_running)
        {
            Job *job = nullptr;
            if (xQueueReceive(self->_queue, &job,
                              pdMS_TO_TICKS(50)) != pdTRUE)
                continue;
            if (job == nullptr) continue;

            self->invoke(job->args);
            delete job;
        }

        self->_task = nullptr;
        vTaskDelete(nullptr);
    }

    template<std::size_t... I>
    void invoke_impl(std::tuple<Args...> &t, std::index_sequence<I...>)
    {
        invoke_slots(std::get<I>(t)...);
    }

    void invoke(std::tuple<Args...> &t)
    {
        invoke_impl(t, std::index_sequence_for<Args...>{});
    }

    void invoke_slots(Args... args)
    {
        std::vector<Slot> snapshot;
        if (_slots_mutex &&
            xSemaphoreTake(_slots_mutex, portMAX_DELAY) == pdTRUE)
        {
            snapshot = _slots;
            xSemaphoreGive(_slots_mutex);
        }
        for (auto &s : snapshot) s(args...);
    }

    std::vector<Slot>  _slots;
    SemaphoreHandle_t  _slots_mutex = nullptr;
    QueueHandle_t      _queue       = nullptr;
    TaskHandle_t       _task        = nullptr;
    volatile bool      _running     = false;
};

#endif // __SIGNAL_HPP__