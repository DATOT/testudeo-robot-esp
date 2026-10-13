#include "./TFT_LCD.h"

void TFT_LCD_Device::setup()
{
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
}

void TFT_LCD_Device::task()
{
}
