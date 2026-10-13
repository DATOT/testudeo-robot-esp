#ifndef __MIC_H__
#define __MIC_H__

#include "Device/BaseDevice.h"
#include "Package/AudioPackage.h"
#include <Arduino.h>
#include <driver/i2s.h>

//?------------------ PINS & SAMPLES -----------------*/
// Connections to INMP441 I2S microphone
#define M_I2S_WS 22
#define M_I2S_SD 21
#define M_I2S_SCK 26
#define L_R 2

// Use I2S Processor 0
#define M_I2S_PORT I2S_NUM_0
#define M_BUFFER_LEN 64
//?------------------ END OF PINS & SAMPLES -----------------*/

class MIC_Device : public BaseDevice
{
private:
  const uint32_t _SAMPLE_RATE = 16000;

  int16_t _sBuffer[M_BUFFER_LEN];
  void i2s_install();
  void i2s_setpin();

public:
  void setup() override;
  void task() override;
  inline bool has_task() const override {
    return true;
  }
};

#endif // !__MIC_H__