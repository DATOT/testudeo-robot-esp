#ifndef __SD_CARD_READER_HPP__
#define __SD_CARD_READER_HPP__

#include "Device/BaseDevice.h"
#include "SD.h"
#include "SPI.h"

/*================== PINS =================*/
#define SD_CS GPIO_NUM_14
#define SPI_MOSI GPIO_NUM_15
#define SPI_MISO GPIO_NUM_8
#define SPI_SCK GPIO_NUM_40
/*=============== END OF PINS ==============*/

class SD_CARD_READER_Device : public BaseDevice
{
public:
  void setup() override;

  File read_file(const char *filePath) const;
  char *read_file_flush(const char *filePath, int *outLen = nullptr) const;
};

#endif // !__SD_CARD_READER_HPP__