#ifndef __MIC_H__
#define __MIC_H__

#include <Arduino.h>
#include <AudioTools.h>
#include <AudioTools/CoreAudio/AudioStreams.h>

#include "Device/BaseDevice.h"
#include "Package/AudioPackage.h"

//?------------------ PINS & SAMPLES -----------------*/
// Connections to INMP441 I2S microphone
#define M_I2S_WS 22
#define M_I2S_SD 21
#define M_I2S_SCK 26
//?------------------ END OF PINS & SAMPLES -----------------*/

class MIC_Device : public BaseDevice
{
private:
  static constexpr uint32_t _SAMPLE_RATE = 16000;
  static constexpr uint16_t _BUFFER_SIZE = 64;

  audio_tools::I2SStream _i2s;
  audio_tools::AudioInfo _info;

  int16_t _sBuffer[_BUFFER_SIZE * 2];

  bool _reading = false;

  friend class MIC_Device_TESTER;

public:
  void setup() override;
  void loop() override;
  inline bool has_task() const override
  {
    return true;
  }
};

#endif // !__MIC_H__