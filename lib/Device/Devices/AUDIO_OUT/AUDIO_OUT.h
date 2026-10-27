#ifndef __AUDIO_OUT_H__
#define __AUDIO_OUT_H__

#include <Arduino.h>
#include <ESP_I2S.h>

#include "Device/BaseDevice.h"

/**----------------------------------------------
 * *                   INFO
 * *  AUDIO_OUT_Device: Phần điều khiển âm thanh ra của robot
 * ?  Dùng module: MAX98357-QFN
 * !  Code dùng: driver/i2s.h
 *
 *---------------------------------------------**/

//?------------------ PINS & SAMPLES -----------------*/
#define A_I2S_BCLK GPIO_NUM_7
#define A_I2S_LRCLK GPIO_NUM_4
#define A_I2S_DIN GPIO_NUM_10

#define A_BUFFER_SAMPLES 256
//?--------------- END OF PINS & SAMPLES --------------*/

class AUDIO_OUT_Device : public BaseDevice
{
private:
  static constexpr uint32_t _SAMPLE_RATE = 16000;
  static constexpr uint16_t _BUFFER_SIZE = A_BUFFER_SAMPLES;

  I2SClass _i2s;

  int16_t _sBuffer[A_BUFFER_SAMPLES];

  bool _writing = false;

public:
  void task() override; // chạy theo kiểu task song song
  void setup() override;
  inline bool has_task() const override
  {
    return true;
  }
};

#endif // !__AUDIO_OUT_H__