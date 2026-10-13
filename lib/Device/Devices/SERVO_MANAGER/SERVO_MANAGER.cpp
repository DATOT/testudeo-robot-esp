#include "./SERVO_MANAGER.h"

void SERVO_MANAGER_Device::setup()
{
  _servo_neck_h.setPeriodHertz(SERVO_FREQUENCY);
  _servo_neck_h.attach(SERVO_PIN_NECK_H, 500, 2400);

  _servo_neck_v.setPeriodHertz(SERVO_FREQUENCY);
  _servo_neck_v.attach(SERVO_PIN_NECK_V, 500, 2400);

  _servo_arm_l.setPeriodHertz(SERVO_FREQUENCY);
  _servo_arm_l.attach(SERVO_PIN_ARM_L, 500, 2400);

  _servo_arm_r.setPeriodHertz(SERVO_FREQUENCY);
  _servo_arm_r.attach(SERVO_PIN_ARM_R, 500, 2400);
}

void SERVO_MANAGER_Device::task()
{
}
