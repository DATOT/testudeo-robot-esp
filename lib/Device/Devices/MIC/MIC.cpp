#include "MIC.h"
#include "SERIAL_READ/SERIAL_READ.h"
void MIC_Device::i2s_install()
{
  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = _SAMPLE_RATE,
      .bits_per_sample = i2s_bits_per_sample_t(16),
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
      .intr_alloc_flags = 0,
      .dma_buf_count = 8,
      .dma_buf_len = M_BUFFER_LEN,
      .use_apll = false};

  i2s_driver_install(M_I2S_PORT, &i2s_config, 0, NULL);
}

void MIC_Device::i2s_setpin()
{
  // Set I2S pin configuration
  const i2s_pin_config_t pin_config = {
      .bck_io_num = M_I2S_SCK,
      .ws_io_num = M_I2S_WS,
      .data_out_num = -1,
      .data_in_num = M_I2S_SD};

  i2s_set_pin(M_I2S_PORT, &pin_config);
}

void MIC_Device::setup()
{
  pinMode(L_R, OUTPUT);
  digitalWrite(L_R, LOW);

  i2s_install();
  i2s_setpin();
  i2s_start(M_I2S_PORT);
}

void MIC_Device::loop()
{
  size_t bytesIn = 0;
  esp_err_t result = i2s_read(M_I2S_PORT, &_sBuffer, M_BUFFER_LEN, &bytesIn, portMAX_DELAY);
  if (result == ESP_OK)
  {
    size_t sample_count = bytesIn / sizeof(int16_t);
    if (sample_count > 0)
    {
      AudioPackage audio_pkg((uint16_t*)_sBuffer, (uint8_t)(sample_count*2));
      audio_pkg.send();
    }
  }
}
