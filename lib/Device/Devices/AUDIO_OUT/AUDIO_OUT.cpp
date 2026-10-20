#include "./AUDIO_OUT.h"

void AUDIO_OUT_Device::setup()
{
  auto cfg = _i2s.defaultConfig(TX_MODE); // TX_MODE = speaker output
  cfg.pin_bck = A_I2S_BCLK_PIN;           // GPIO 18
  cfg.pin_ws = A_I2S_LRCLK_PIN;           // GPIO 19
  cfg.pin_data = A_I2S_DATA_PIN;          // GPIO 23
  cfg.sample_rate = _SAMPLE_RATE;         // 16000 Hz
  cfg.channels = 1;                       // MAX98357 mono; send 1ch
  cfg.bits_per_sample = 16;
  cfg.buffer_size = A_BUFFER_SAMPLES; // 256 frames per DMA buffer
  cfg.buffer_count = 8;               // 8 DMA buffers

  _info = cfg;

  if (!_i2s.begin(cfg))
  {
    Serial.println("[AUDIO_OUT] I2S begin() failed");
    return;
  }
}

void AUDIO_OUT_Device::task()
{
}
