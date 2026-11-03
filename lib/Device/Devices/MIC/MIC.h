#ifndef __MIC_H__
#define __MIC_H__

#include <Arduino.h>
#include <ESP_I2S.h>
#include "Device/BaseDevice.h"

#define M_I2S_WS GPIO_NUM_4
#define M_I2S_SD GPIO_NUM_6
#define M_I2S_SCK GPIO_NUM_39

class MIC_Device : public BaseDevice {
private:
  static constexpr uint32_t _SAMPLE_RATE = 16000;
  static constexpr uint16_t _BUFFER_SIZE = 64;
  I2SClass _i2s;
  int16_t _sBuffer[_BUFFER_SIZE];
  bool _reading = false;

public:
  void setup() override;
  void loop() override;
  inline bool has_loop() const override { return true; }
};

#endif
