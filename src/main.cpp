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

#define USE_AUDIO
// #define USE_LCD
// #define NO_MANAGER

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
#ifdef USE_LCD
TFT_LCD_Device lcd = TFT_LCD_Device();
#endif
#ifdef USE_SERVO
SERVO_MANAGER_Device smd = SERVO_MANAGER_Device();
#endif
#ifdef USE_BUTTONS
LISTEN_BUTTON_Device listenButton = LISTEN_BUTTON_Device();
DISCONNECT_BUTTON_Device disconnectButton = DISCONNECT_BUTTON_Device();
#endif
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
  Serial.begin(115200);
  while (!Serial)
    delay(10);
  pinMode(RGB_BUILTIN, OUTPUT);
  rgbTest();

#ifndef NO_MANAGER
  if (!manager.add(srd))
  {
    // ...
  }
#ifdef USE_LCD
  if (!manager.add(lcd))
  {
    // ...
  }
#endif
#ifdef USE_SERVO
  if (!manager.add(smd))
  {
    // ...
  }
#endif
#ifdef USE_BUTTONS
  if (!manager.add(listenButton))
  {
    // ...
  }
  if (!manager.add(disconnectButton))
  {
    // ...
  }
#endif
#ifdef USE_AUDIO
  if (!manager.add(mic))
  {
    //...
  }

  if (!manager.add(aod))
  {
    //...
  }
#endif

  manager.setup();
  Serial.println("SETUP OK");
  manager.start();
#endif
  delay(2000);
  Serial.println("BOOT OK");
}

void loop()
{

#ifndef NO_MANAGER
  manager.loop();
#endif
}