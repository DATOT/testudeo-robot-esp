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
 * !  SERVO_PIN_NECK_H : ...             - Servo cổ qua trái phải
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
#define BASE_AMPLITUDE_NECK 90
#define BASE_AMPLITUDE_ARM 0
#define MAX_AMPLITUDE_V 160
#define MIN_AMPLITUDE_V 40
#define MAX_AMPLITUDE_H 160
#define MIN_AMPLITUDE_H 20
#define MAX_AMPLITUDE_ARM 160
#define MIN_AMPLITUDE_ARM 115

//#define ANGLE(A) (((A) < 0) ? max(MIN_AMPLITUDE_V, BASE_AMPLITUDE_V + (A) : min(MAX_AMPLITUDE_V, BASE_AMPLITUDE_V + (A))))

//?-------------------------------- END OF PINS & FREQ ------------------------------*/

class SERVO_MANAGER_Device : public BaseDevice
{
private:
  Servo _servo_neck_v;
  Servo _servo_neck_h;
  Servo _servo_arm_l;
  Servo _servo_arm_r;
  int angle = 0;  

  void _shake_head(int times);
  void _nod_head(int times);
  void _wave_r_arm(int time);
  void _wave_l_arm(int time);

  friend class SERVO_MANAGER_Device_TESTER;

public:
  void setup() override;
  void task() override;
  inline bool has_task() const override {
    return true;
  }
};

#endif // !__SERVO_MANAGER_H__