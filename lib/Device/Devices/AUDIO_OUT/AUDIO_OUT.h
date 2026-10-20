#ifndef __AUDIO_OUT_H__
#define __AUDIO_OUT_H__

#include <Arduino.h>
#include <AudioTools.h>

#include "Device/BaseDevice.h"

/**----------------------------------------------
 * *                   INFO
 * *  AUDIO_OUT_Device: Phần điều khiển âm thanh ra của robot
 * ?  Dùng module: MAX98357-QFN
 * !  Code dùng: driver/i2s.h
 *
 *---------------------------------------------**/

//?------------------ PINS & SAMPLES -----------------*/
#define A_I2S_BCLK_PIN 18
#define A_I2S_LRCLK_PIN 19
#define A_I2S_DATA_PIN 23

#define A_BUFFER_SAMPLES 256
//?--------------- END OF PINS & SAMPLES --------------*/

class AUDIO_OUT_Device : public BaseDevice
{
private:
  static constexpr uint32_t _SAMPLE_RATE = 16000;

  audio_tools::I2SStream _i2s;
  audio_tools::AudioInfo _info;

  int16_t _audioBuffer[A_BUFFER_SAMPLES];

public:
  void task() override; // chạy theo kiểu task song song
  void setup() override;
  Stream &stream() { return _i2s; }
  inline bool has_task() const override
  {
    return true;
  }
};

#endif // !__AUDIO_OUT_H__