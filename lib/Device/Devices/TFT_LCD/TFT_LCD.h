#ifndef __TFT_LCD_H__
#define __TFT_LCD_H__

#include "Device/BaseDevice.h"

#include <Arduino.h>
#include <bb_spi_lcd.h>
#include <SPI.h>

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
#define LCD_MOSI 19
#define LCD_SCLK 18
#define LCD_CS 17
#define LCD_DC 16
#define LCD_RST 5
#define LCD_BL 10

#define SPI_FREQUENCY 20000000
//?-------------------------------- END OF NAME, SIZE, PINs, FREQ -----------------------------*/

class TFT_LCD_Device : public BaseDevice
{
private:
  SPILCD _lcd;

  enum _FaceState
  {
    None,
    Neutral,
    Confused,
    Cool,
    Funny,
    Laughing,
    Shocked,
    Silly,
    Sleepy,
    Suprised,
    Thinking,
  };
  _FaceState _current_state;
  _FaceState _last_state = None;

  const char *_FACE_TO_FILE_ARR[11] =
      {
          "",
          "Neutral.bmp",
          "Confused.bmp",
          "Cool.bmp",
          "Funny.bmp",
          "Laughing.bmp",
          "Shocked.bmp",
          "Silly.bmp",
          "Sleepy.bmp",
          "Suprised.bmp",
          "Thinking.bmp",
  };

  inline const char *_get_face_from_state(_FaceState state) { return _FACE_TO_FILE_ARR[state]; }

  void _draw_image(const char *filename, int x, int y);

public:
  void setup() override;
  void task() override;
  inline bool has_task() const override
  {
    return true;
  }
};

#endif // !__TFT_LCD_H__