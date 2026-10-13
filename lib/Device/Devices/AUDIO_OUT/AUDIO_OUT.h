#ifndef __AUDIO_OUT_H__
#define __AUDIO_OUT_H__

#include <Arduino.h>
#include <driver/i2s.h>

#include "Device/BaseDevice.h"

/**----------------------------------------------
 * *                   INFO
 * *  AUDIO_OUT_Device: Phần điều khiển âm thanh ra của robot
 * ?  Dùng module: MAX98357-QFN
 * !  Code dùng: driver/i2s.h
 *
 *---------------------------------------------**/

//?------------------ PINS & SAMPLES -----------------*/
#define A_I2S_BCLK_PIN   18
#define A_I2S_LRCLK_PIN  19
#define A_I2S_DATA_PIN   23

#define A_I2S_PORT       I2S_NUM_1
#define A_BUFFER_SAMPLES 256
//?--------------- END OF PINS & SAMPLES --------------*/

class AUDIO_OUT_Device : public BaseDevice
{
private:
  const uint32_t _SAMPLE_RATE = 16000;
  int16_t _audioBuffer[A_BUFFER_SAMPLES];

  void _i2s_install(); // tải i2s
  void _i2s_setpin(); // lựa pin cho i2s

public:
  void task() override; // chạy theo kiểu task song song
  void setup() override;
  inline bool has_task() const override
  {
    return true;
  }
};

#endif // !__AUDIO_OUT_H__