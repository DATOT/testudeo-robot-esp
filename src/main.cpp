/**------------------------------------------------------------------------
 * ?                                ABOUT
 * @author         :  Lâm Tiến Đạt & Phạm Nguyễn Đăng Khôi
 * @repo           :  https://github.com/DATOT/testudeo-robot-esp
 * @createdOn      :  11/10/2026
 * @description    :  Code cho robot feynman
 *------------------------------------------------------------------------**/
#include <Arduino.h>
#define USE_HSPI_PORT

#include "DeviceManager/DeviceManager.hpp"
#include "TFT_LCD/TFT_LCD.h"
#include "AUDIO_OUT/AUDIO_OUT.h"
#include "MIC/MIC.h"
#include "SERIAL_READ/SERIAL_READ.h"
#include "SERVO_MANAGER/SERVO_MANAGER.h"
#include "LISTEN_BUTTON/LISTEN_BUTTON.hpp"
#include "DISCONNECT_BUTTON/DISCONNECT_BUTTON.hpp"

#ifndef RGB_BUILTIN
#define RGB_BUILTIN 38 // Change to 48 if 38 doesn't work
#endif

// Fallback for older Arduino ESP32 cores where rgbLedWrite didn't exist
#if ESP_ARDUINO_VERSION_MAJOR < 3
#define rgbLedWrite neopixelWrite
#endif

#ifndef NO_MANAGER
DeviceManager manager;
SERIAL_READ_Device srd = SERIAL_READ_Device();
TFT_LCD_Device lcd = TFT_LCD_Device();
SERVO_MANAGER_Device smd = SERVO_MANAGER_Device();
LISTEN_BUTTON_Device listenButton = LISTEN_BUTTON_Device();
DISCONNECT_BUTTON_Device disconnectButton = DISCONNECT_BUTTON_Device();
#ifdef USE_AUDIO
MIC_Device mic = MIC_Device();
AUDIO_OUT_Device aod = AUDIO_OUT_Device();
#endif
#endif

void rgbTest()
{
  rgbLedWrite(RGB_BUILTIN, 255, 0, 0);
  delay(100);
  rgbLedWrite(RGB_BUILTIN, 0, 255, 0);
  delay(100);
  rgbLedWrite(RGB_BUILTIN, 255, 255, 0);
  delay(100);
  rgbLedWrite(RGB_BUILTIN, 0, 0, 255);
  delay(100);
  rgbLedWrite(RGB_BUILTIN, 255, 0, 255);
  delay(100);
  rgbLedWrite(RGB_BUILTIN, 0, 255, 255);
  delay(100);
  rgbLedWrite(RGB_BUILTIN, 255, 255, 255);
  delay(100);
  rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
}
void setup()
{
  Serial.begin(9600);
  pinMode(RGB_BUILTIN, OUTPUT);
  rgbTest();

  while (!Serial)
    delay(10);
#ifndef NO_MANAGER
  if (!manager.add(srd))
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
  if (!manager.add(listenButton))
  {
    // ...
  }
  if (!manager.add(disconnectButton))
  {
    // ...
  }
#ifdef USE_AUDIO
  if (!manager.add(mic))
  {
    ...
  }

  if (!manager.add(aod))
  {
    ...
  }
#endif

  manager.setup();
  manager.start();
#endif
}

void loop()
{

#ifndef NO_MANAGER
  manager.loop();
#endif
}