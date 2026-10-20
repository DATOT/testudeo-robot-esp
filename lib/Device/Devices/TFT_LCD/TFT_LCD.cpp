#include "./TFT_LCD.h"

void TFT_LCD_Device::setup()
{
  // Initialize display: 240 width, 320 height
  _lcd.init(240, 320);
  _lcd.setRotation(1); // Adjust if your orientation is sideways/upside down

  _lcd.fillScreen(ST77XX_BLACK);
  rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
  delay(500);

  _lcd.fillScreen(ST77XX_RED);
  rgbLedWrite(RGB_BUILTIN, 255, 0, 0);
  delay(500);

  _lcd.fillScreen(ST77XX_BLUE);
  rgbLedWrite(RGB_BUILTIN, 0, 0, 255);
  delay(500);

  rgbLedWrite(RGB_BUILTIN, 0, 0, 0);
  _lcd.fillScreen(ST77XX_BLACK);

  // Draw confirmation text
  _lcd.setCursor(20, 50);
  _lcd.setTextColor(ST77XX_WHITE);
  _lcd.setTextSize(2);
  _lcd.print("TEST");
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
