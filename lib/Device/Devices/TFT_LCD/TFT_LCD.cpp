#include "./TFT_LCD.h"

void TFT_LCD_Device::setup()
{
  // SPI.begin(LCD_SCLK, -1, LCD_MOSI, LCD_CS);
  spilcdInit(&_lcd, LCD_ST7789, FLAGS_INVERT, SPI_FREQUENCY, LCD_CS, LCD_DC, LCD_RST, LCD_BL, -1, LCD_MOSI, LCD_SCLK, 0);
  spilcdSetOrientation(&_lcd, LCD_ORIENTATION_90);
  spilcdFill(&_lcd, 0, DRAW_TO_LCD);
  spilcdWriteString(&_lcd, 0, 72, "Hello, World", 0xffff, 0x0000, FONT_8x8, DRAW_TO_LCD);
}

void TFT_LCD_Device::_draw_image(const char *filename, int x, int y)
{
}

void TFT_LCD_Device::task()
{
  if (_current_state != _last_state)
  {
    _draw_image(_get_face_from_state(_current_state), 0, 0);
    _last_state = _current_state;
  }
}
