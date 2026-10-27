#ifndef __DISCONNECT_BUTTON_HPP__
#define __DISCONNECT_BUTTON_HPP__

#include "Device/BaseDevice.h"
#include "SD.h"
#include "SPI.h"

/*================== PINS =================*/
#define SD_CS 0
#define SPI_MISO 13
#define SPI_SCK 12

/*=============== END OF PINS ==============*/

class SD_CARD_READER_Device : public BaseDevice
{
public:
  void setup() override;

  File &read_file(const char *filePath) const;
  char *read_file_flush(const char *filePath, int *outLen) const;
};

#endif // !__DISCONNECT_BUTTON_HPP__