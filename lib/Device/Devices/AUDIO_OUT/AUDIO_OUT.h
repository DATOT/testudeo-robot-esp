#ifndef __AUDIO_OUT_DEVICE_H__
#define __AUDIO_OUT_DEVICE_H__

#include <Arduino.h>
#include <ESP_I2S.h>
#include "Device/BaseDevice.h"
#include "SD_CARD_READER/SD_CARD_READER.h"

#define A_I2S_BCLK GPIO_NUM_39
#define A_I2S_LRCLK GPIO_NUM_4
#define A_I2S_DIN GPIO_NUM_10
#define A_BUFFER_SAMPLES 512

class AUDIO_OUT_Device : public BaseDevice
{
private:
  static constexpr uint32_t _SAMPLE_RATE = 16000;
  SD_CARD_READER_Device *_sd_card_reader = nullptr;
  I2SClass _i2s;
  uint8_t _sBuffer[A_BUFFER_SAMPLES];
  bool _writing = false;

public:
  void task() override;
  void setup() override;
  void play_fallback(const char *fallback);
  void play_pcm16(const int16_t *samples, size_t sampleCount);
  inline bool has_task() const override { return true; }
};

#endif
