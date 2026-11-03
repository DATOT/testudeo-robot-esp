#include "MIC.h"
#include "Global.hpp"
#include "WEBSOCKET/WEBSOCKET.h"

void MIC_Device::setup() {
  Serial.println("[MIC] Starting I2S microphone");
  _i2s.setPins(M_I2S_SCK, M_I2S_WS, -1, M_I2S_SD);
  bool ok = _i2s.begin(I2S_MODE_STD, _SAMPLE_RATE, I2S_DATA_BIT_WIDTH_32BIT, I2S_SLOT_MODE_STEREO);
  if (!ok) {
    Serial.println("[MIC] Failed to initialize I2S");
    return;
  }
  _reading = _i2s.configureRX(_SAMPLE_RATE, I2S_DATA_BIT_WIDTH_32BIT,
                               I2S_SLOT_MODE_MONO, I2S_RX_TRANSFORM_32_TO_16,
                               I2S_STD_SLOT_LEFT);
  if (_reading) Serial.println("[MIC] I2S ready (16 kHz, PCM16 mono)");
}

void MIC_Device::loop() {
  if (!_reading || !Global::mic_enabled || !manager) return;
  auto websocket = manager->get_first_device<WEBSOCKET_Device>();
  if (!websocket || !(*websocket)->connected()) return;

  const size_t bytesRead = _i2s.readBytes(reinterpret_cast<char *>(_sBuffer), sizeof(_sBuffer));
  if (bytesRead > 0) (*websocket)->send_audio(reinterpret_cast<uint8_t *>(_sBuffer), bytesRead);
}
