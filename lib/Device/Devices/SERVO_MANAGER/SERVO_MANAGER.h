#ifndef __SERVO_MANAGER_H__
#define __SERVO_MANAGER_H__

#include "Device/BaseDevice.h"
#include <Arduino.h>
#include <ESP32Servo.h>

/**----------------------------------------------
 * *                   INFO
 * *  SERVO_MANAGER_Device: điều khiển Servo
 * ?  Code dùng: ESP32Servo.h
 * 
 * !  SERVO_PIN_NECK_V : ...             - Servo cổ lên xuống
 * !  SERVO_PIN_NECK_H : ...             - Servo cổ lên qua trái phải
 * !  SERVO_PIN_ARM_L  : ...             - Servo tay trái
 * !  SERVO_PIN_ARM_R  : ...             - Servo tay phải
 *
 * !  SERVO_FREQUENCY  : 50   (Hz)
 *---------------------------------------------**/

//?-------------------------------- PINS & FREQ ------------------------------*/
#define SERVO_PIN_NECK_V 0
#define SERVO_PIN_NECK_H 0
#define SERVO_PIN_ARM_L 0
#define SERVO_PIN_ARM_R 0

#define SERVO_FREQUENCY 50
//?-------------------------------- END OF PINS & FREQ ------------------------------*/

class SERVO_MANAGER_Device : public BaseDevice
{
private:
  Servo _servo_neck_v;
  Servo _servo_neck_h;
  Servo _servo_arm_l;
  Servo _servo_arm_r;

  void _shake_head();
  void _nod_head();
  void _wave_r_arm();
  void _wave_l_arm();

  friend class SERVO_MANAGER_Device_TESTER;

public:
  void setup() override;
  void task() override;
  inline bool has_task() const override {
    return true;
  }
};

#endif // !__SERVO_MANAGER_H__