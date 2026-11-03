#include "MIC.h"
#include "SERIAL_READ/SERIAL_READ.h"

#define SKIP_SETUP

void MIC_Device::setup()
{
#ifndef SKIP_SETUP
  Serial.println("[MIC] Starting I2S via ESP_I2S...");

  // Set the pins for I2S (SCK, WS, DataOut, DataIn)
  // We only need RX (DataIn), so DataOut is set to -1 (unused)
  _i2s.setPins(M_I2S_SCK, M_I2S_WS, -1, M_I2S_SD);
  // Initialize I2S in standard mode
  bool ok = _i2s.begin(
      I2S_MODE_STD,
      _SAMPLE_RATE,
      I2S_DATA_BIT_WIDTH_32BIT,
      I2S_SLOT_MODE_STEREO);
  // Parameters: Mode, Sample Rate, Data Width, Channels, Slot Mode, Transform

  if (!ok)
  {
    Serial.println("[MIC] Failed to initialize I2S!");
    return;
  }
  ok = _i2s.configureRX(_SAMPLE_RATE, I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_MONO, I2S_RX_TRANSFORM_32_TO_16, I2S_STD_SLOT_LEFT);

  if (!ok)
  {
    Serial.println("[MIC] configureRX() failed!");
    _i2s.end();
    return;
  }

  _reading = true;
  Serial.println("[MIC] I2S Ready (32->16 transform enabled).");
#endif
}

uint32_t lastPrint = 0;

void MIC_Device::loop()
{
  if (!_reading)
    return;

  // Read 16-bit mono data directly
  size_t bytesRead = _i2s.readBytes((char *)_sBuffer, sizeof(_sBuffer));

  if (bytesRead == 0)
  {
    lastPrint = millis();
    Serial.println("[MIC] No data received.");
    return;
  }

  // Calculate number of samples (16-bit = 2 bytes)
  size_t samplesRead = bytesRead / sizeof(int16_t);

  if (samplesRead == 0)
  {
    Serial.printf("[MIC] Too few bytes: %u\n", (unsigned int)bytesRead);
    return;
  }

  // Debug print (min/max) - optional, can be removed for performance\
    lastPrint = millis();
  int16_t minSample = INT16_MAX;
  int16_t maxSample = INT16_MIN;
  for (size_t i = 0; i < samplesRead; ++i)
  {
    if (_sBuffer[i] < minSample)
      minSample = _sBuffer[i];
    if (_sBuffer[i] > maxSample)
      maxSample = _sBuffer[i];
  }
  // Serial.printf("[MIC] bytes=%u samples=%u min=%d max=%d:\n",
  //               (unsigned int)bytesRead,
  //               (unsigned int)samplesRead,
  //               minSample,
  //               maxSample);
  for (auto &s : _sBuffer)
  {
    Serial.printf("%d,", s);
  }
  Serial.println();

  // Send the audio package
  uint16_t outBytes = (uint16_t)(samplesRead * sizeof(int16_t));
  AudioPackage audio_pkg(reinterpret_cast<uint16_t *>(_sBuffer), outBytes);
  audio_pkg.send();
}