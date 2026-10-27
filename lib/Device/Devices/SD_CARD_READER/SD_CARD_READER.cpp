#include "SD_CARD_READER.h"

void SD_CARD_READER_Device::setup()
{
  SPI.begin(SPI_SCK, SPI_MISO, -1, SD_CS);
  if (!SD.begin(SD_CS))
  {
    Serial.println("[SD] SD Failed to mount");
    return;
  }
}

File &SD_CARD_READER_Device::read_file(const char *filePath) const
{
  File readFile = SD.open(filePath, FILE_READ);
  if (!readFile)
  {
    Serial.printf("[SD] failed to open file for reading: \"%s\"\n", filePath);
  }
  return readFile;
}

char *SD_CARD_READER_Device::read_file_flush(const char *filePath, int *outLen) const
{
  File readFile;
  if (!(readFile = read_file(filePath)))
  {
    return nullptr;
  }

  size_t size = readFile.size();
  char *buf = (char *)malloc(size + 1);
  if (!buf)
  {
    Serial.println("[SD] malloc failed");
    readFile.close();
    return nullptr;
  }

  size_t bytesRead = readFile.read((uint8_t *)buf, size);
  readFile.close();

  if (bytesRead != size)
  {
    Serial.printf("[SD] read mismatch: expected %u(file size), got %u(read)\n",
                  (unsigned)size, (unsigned)bytesRead);
    free(buf);
    return nullptr;
  }
  buf[size] = '\0';
  if (outLen)
    *outLen = size;
  return nullptr;
}
