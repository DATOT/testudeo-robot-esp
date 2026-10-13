#ifndef __TFT_LCD_H__
#define __TFT_LCD_H__

#include "Device/BaseDevice.h"
#include <Arduino.h>

/**----------------------------------------------
 * *                   INFO
 * * TFT_LCD_Device: Điều khiển màn hình TFT
 * ? Dùng module: TFT LCD ST7789V
 * ! Dùng để code: TFT_eSPI
 * ? Độ phân giải  : 240 x 320 pixel
 *
 * ! TFT_MOSI       : ...             - Truyền dữ liệu SPI
 * ! TFT_SCLK       : ...             - Xung nhịp SPI
 * ! TFT_CS         : ...             - Chọn thiết bị SPI
 * ! TFT_DC         : ...             - Phân biệt dữ liệu và lệnh
 * ! TFT_RST        : ...             - Khởi động lại màn hình
 *
 * ! Tần số SPI   : 27 MHz
 *---------------------------------------------**/

//?-------------------------------- NAME, SIZE, PINs, FREQ ------------------------------*/
#define ST7789_DRIVER

#define TFT_WIDTH 240
#define TFT_HEIGHT 320

#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS 5
#define TFT_DC 2
#define TFT_RST 4

#define SPI_FREQUENCY 27000000
//?-------------------------------- END OF NAME, SIZE, PINs, FREQ -----------------------------*/

#include <TFT_eSPI.h>

class TFT_LCD_Device : public BaseDevice
{
private:
  TFT_eSPI tft = TFT_eSPI();

public:
  void setup() override;
  void task() override;
  inline bool has_task() const override {
    return true;
  }
};

#endif // !__TFT_LCD_H__