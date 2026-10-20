#ifndef __BASE_DEVICE_H__
#define __BASE_DEVICE_H__

#include <Arduino.h>
#include "Signal/Signal.hpp"

class DeviceManager;
#include "DeviceManager/DeviceManager.hpp"
/**-----------------------
 * !      WARNING
 * ! Không được thay đổi code ở đây
 *------------------------**/	 
class BaseDevice
{
protected:
	DeviceManager *manager; // ptr cho DeviceManager quản lý Device

	// Gán manager
	void set_manager(DeviceManager *newManager)
	{
		manager = newManager;
	}

	friend class DeviceManager;

public:
	virtual void setup() {}; // setup, tương ứng với setup ở code arduino bth
	virtual void task() {};  // giống loop nhưng cho mấy cái code chạy song song
	virtual void loop() {}; // loop, tương ứng với loop ở code arduino bth

	virtual bool has_task() const { return false; }
	virtual bool has_loop() const { return false; }

	virtual ~BaseDevice() = default;
};

#endif // !__BASE_DEVICE_H__