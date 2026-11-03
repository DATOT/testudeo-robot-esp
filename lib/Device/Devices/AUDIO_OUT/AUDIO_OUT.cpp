#include "./AUDIO_OUT.h"
// #define SKIP_SETUP
void AUDIO_OUT_Device::setup()
{
#ifndef SKIP_SETUP
  Serial.println("[AUDIO_OUT] Starting I2S via ESP_I2S...");

  if (auto device_opt = manager->get_first_device<SD_CARD_READER_Device>())
  {
    _sd_card_reader = *device_opt; // Dereference optional to get the pointer
  }
  else
  {
    Serial.println("[Error] SD_CARD_READER_Device not found in manager!");
  }
  // MAX98357 only needs TX (DataOut), so DataIn is set to -1 (unused)
  _i2s.setPins(A_I2S_BCLK, A_I2S_LRCLK, A_I2S_DIN, -1);

  bool ok = _i2s.begin(
      I2S_MODE_STD,
      _SAMPLE_RATE,
      I2S_DATA_BIT_WIDTH_16BIT,
      I2S_SLOT_MODE_STEREO);

  if (!ok)
  {
    Serial.println("[AUDIO_OUT] Failed to initialize I2S!");
    return;
  }

  ok = _i2s.configureTX(_SAMPLE_RATE, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO, I2S_STD_SLOT_LEFT);

  if (!ok)
  {
    Serial.println("[AUDIO_OUT] configureTX() failed!");
    _i2s.end();
    return;
  }

  _writing = true;
  Serial.println("[AUDIO_OUT] I2S Ready (16-bit mono TX).");
#endif
}

void AUDIO_OUT_Device::play_fallback(const char *fallback)
{
  if (!_sd_card_reader)
  {
    Serial.println("[Error] SD card reader not initialized!");
    return;
  }

  char path[64];
  snprintf(path, sizeof(path), "/audio/fallback/%s.wav", fallback);

  File f = _sd_card_reader->read_file(path);
  if (!f)
  {
    Serial.println("[AUDIO_OUT] Failed to open fallback file!");
    return;
  }

  Serial.printf("[AUDIO_OUT] Playing fallback: %s\n", path);

  if (f.size() > 44)
  {
    f.seek(44);
  }

  while (f.available())
  {
    size_t bytesRead = f.read(_sBuffer, sizeof(_sBuffer));
    if (bytesRead > 0)
    {
      size_t bytesWritten = _i2s.write(_sBuffer, bytesRead);

      if (bytesWritten == 0)
      {
        delay(1);
      }
    }
    vTaskDelay(1);
  }

  f.close();
}

void AUDIO_OUT_Device::task()
{
  if (!_writing)
  {
    return;
  }
}