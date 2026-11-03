#include <Arduino.h>
#define USE_HSPI_PORT

#include "DeviceManager/DeviceManager.hpp"
#include "TFT_LCD/TFT_LCD.h"
#include "AUDIO_OUT/AUDIO_OUT.h"
#include "MIC/MIC.h"
#include "WEBSOCKET/WEBSOCKET.h"
#include "SD_CARD_READER/SD_CARD_READER.h"
#include "SERVO_MANAGER/SERVO_MANAGER.h"

#ifndef RGB_BUILTIN
#define RGB_BUILTIN 38
#endif
#if ESP_ARDUINO_VERSION_MAJOR < 3
#define rgbLedWrite neopixelWrite
#endif

DeviceManager manager;
WEBSOCKET_Device websocketDevice;
SD_CARD_READER_Device sdcrd;
MIC_Device mic;
AUDIO_OUT_Device aod;

void setup()
{
  Serial.begin(115200);
  pinMode(RGB_BUILTIN, OUTPUT);
  rgbLedWrite(RGB_BUILTIN, 0, 0, 0);

  manager.add(websocketDevice);
  manager.add(sdcrd);
  manager.add(mic);
  manager.add(aod);

  manager.setup();
  manager.start();
  Serial.println("[BOOT] Device manager started");
}

void loop()
{
  manager.loop();
}
