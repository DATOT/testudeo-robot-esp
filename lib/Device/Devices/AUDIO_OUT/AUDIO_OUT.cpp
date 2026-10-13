#include "./AUDIO_OUT.h"

void AUDIO_OUT_Device::_i2s_install()
{
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = _SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT, // Mono output
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = 0,
      .dma_buf_count = 8,
      .dma_buf_len = 64,
      .use_apll = false};
  i2s_driver_install(A_I2S_PORT, &i2s_config, 0, NULL);
}

void AUDIO_OUT_Device::_i2s_setpin() {
  i2s_pin_config_t pin_config = {
      .bck_io_num = A_I2S_BCLK_PIN,
      .ws_io_num = A_I2S_LRCLK_PIN,
      .data_out_num = A_I2S_DATA_PIN,
      .data_in_num = I2S_PIN_NO_CHANGE};

  i2s_set_pin(A_I2S_PORT, &pin_config);
}

void AUDIO_OUT_Device::setup()
{
  _i2s_install();
  _i2s_setpin();
  i2s_zero_dma_buffer(A_I2S_PORT);
}

void AUDIO_OUT_Device::task()
{
}

