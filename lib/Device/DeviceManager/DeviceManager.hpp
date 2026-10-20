#ifndef __DEVICE_MANAGER_HPP__
#define __DEVICE_MANAGER_HPP__

#include <Arduino.h>
#include <vector>
#include <optional>

class BaseDevice;
#include "Device/BaseDevice.h"
#include "Signal/Signal.hpp"

/**-----------------------
 * !      WARNING
 * * Chỉ được thêm global signal ở phần signals ở dưới
 * ! Không được thay đổi code ở đây
 *------------------------**/
class DeviceManager
{
//*-------------------------------- PUBLIC ------------------------------*/
public:
    DeviceManager();
    ~DeviceManager();

    DeviceManager(const DeviceManager &) = delete;
    DeviceManager &operator=(const DeviceManager &) = delete;

    bool add(BaseDevice &device); // thêm device
    void setup();
    void loop();
    bool start(); // Bắt đầu chạy tất cả mọi thứ
    bool stop(TickType_t timeout = portMAX_DELAY); // Dừng tất cả mọi thứ đang chạy

    template <typename T>
    std::optional<T> get_first_device(); // lấy thiết bị

    bool is_running() const;

//?-------------------------------- GLOBAL SIGNAL ------------------------------//
signals:
  Signal <int> connecting;
  Signal <int> connected;
//?-------------------------------- END OF GLOBAL SIGNAL ------------------------------//

//*-------------------------------- PRIVATE ------------------------------*/
private:
    struct DeviceSlot
    {
        explicit DeviceSlot(BaseDevice *dev)
            : device(dev),
              task(nullptr),
              task_mutex(nullptr),
              loop_mutex(nullptr),
              stopped(nullptr),
              running(false),
              loop_enabled(false)
        {}

        BaseDevice        *device;

        TaskHandle_t       task;
        SemaphoreHandle_t  task_mutex;  // protects task() only
        SemaphoreHandle_t  loop_mutex;  // protects loop()
        SemaphoreHandle_t  stopped;

        volatile bool running;
        bool          loop_enabled;
    };

    std::vector<DeviceSlot *> _devices;
    SemaphoreHandle_t         _manager_mutex;

    bool _started;
    bool _stopping;
    bool _setup_done;

    static void _task_entry(void *arg);

    static bool _lock_mutex(SemaphoreHandle_t mutex,
                           TickType_t timeout = portMAX_DELAY);
    static void _unlock_mutex(SemaphoreHandle_t mutex);
};

#endif // !__DEVICE_MANAGER_HPP__
