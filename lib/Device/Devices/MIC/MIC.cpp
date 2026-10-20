#include "MIC.h"
#include "SERIAL_READ/SERIAL_READ.h"

void MIC_Device::setup()
{
  auto cfg = _i2s.defaultConfig(RX_MODE);
  cfg.pin_bck = M_I2S_SCK;
  cfg.pin_ws = M_I2S_WS;
  cfg.pin_data = M_I2S_SD;
  cfg.sample_rate = _SAMPLE_RATE;
  cfg.channels = 2;
  cfg.bits_per_sample = 16;
  cfg.buffer_size = _BUFFER_SIZE;
  cfg.buffer_count = 4;

  _info = cfg;

  if (!_i2s.begin(cfg))
  {
    Serial.println("[MIC] I2S begin failed");
    return;
  }

  _reading = true;
}

void MIC_Device::loop()
{
  if (!_reading)
  {
    return;
  }

  size_t bytesIn = _i2s.readBytes((uint8_t *)_sBuffer, sizeof(_sBuffer));
  if (bytesIn == 0)
    return;

  size_t frames = bytesIn / (2 * sizeof(int16_t));
  if (frames == 0)
    return;

  int16_t *mono = _sBuffer;
  for (size_t i = 0; i < frames; ++i)
  {
    mono[i] = _sBuffer[2 * i];
  }
  AudioPackage audio_pkg(
      reinterpret_cast<uint16_t *>(mono),
      static_cast<uint8_t>(frames * sizeof(int16_t)));
  audio_pkg.send();
}
