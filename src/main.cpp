/**------------------------------------------------------------------------
 * ?                                ABOUT
 * @author         :  Lâm Tiến Đạt & Phạm Nguyễn Đăng Khôi
 * @repo           :  https://github.com/DATOT/testudeo-robot-esp
 * @createdOn      :  11/10/2026
 * @description    :  Code cho robot feynman
 *------------------------------------------------------------------------**/
#include <Arduino.h>

#include "DeviceManager/DeviceManager.hpp"
#include "TFT_LCD/TFT_LCD.h"
#include "AUDIO_OUT/AUDIO_OUT.h"
#include "MIC/MIC.h"
#include "SERIAL_READ/SERIAL_READ.h"
#include "SERVO_MANAGER/SERVO_MANAGER.h"

DeviceManager manager;
SERIAL_READ_Device srd = SERIAL_READ_Device();
MIC_Device mic = MIC_Device();
TFT_LCD_Device lcd = TFT_LCD_Device();
AUDIO_OUT_Device aod = AUDIO_OUT_Device();
SERVO_MANAGER_Device smd = SERVO_MANAGER_Device();

void setup()
{
  Serial.begin(115200);
  while (!Serial) delay(10);
  if (!manager.add(srd))
  {
    // ...
  }
  if (!manager.add(mic))
  {
    // ...
  }
  if (!manager.add(lcd))
  {
    // ...
  }
  if (!manager.add(smd))
  {
    // ...
  }
  if (!manager.add(aod))
  {
    // ...
  }

  manager.setup();
  manager.start();
}

void loop()
{
  manager.loop();
}