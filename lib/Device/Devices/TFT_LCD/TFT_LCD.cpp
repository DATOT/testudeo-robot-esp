#include "./TFT_LCD.h"

void TFT_LCD_Device::setup()
{
  tft.init();
  tft.setRotation(1);

  tft.fillScreen(TFT_BLACK);

  _draw_image(_get_face_from_state(Neutral), 0, 0);
}

void TFT_LCD_Device::_draw_image(const char *filename, int x, int y) {
    File bmpFile = SD.open(filename, FILE_READ);
    if (!bmpFile) {
        Serial.print("Không thể mở tệp ảnh: ");
        Serial.println(filename);
        return;
    }

    bmpFile.close();
}

void TFT_LCD_Device::task()
{
  if (_current_state != _last_state) {
        _draw_image(_get_face_from_state(_current_state), 0, 0);
        _last_state = _current_state;
  }
}

