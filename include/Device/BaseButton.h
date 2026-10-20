#ifndef __BASE_BUTTON_H__
#define __BASE_BUTTON_H__

#include "./BaseDevice.h"

class BaseButton : public BaseDevice
{
private:
  const int _pin;
  bool _down = false;

public:
  BaseButton(int pin)
      : _pin(pin)
  {
  }
  void setup() override
  {
#ifdef BUTTON_PULLUP
    pinMode(_pin, INPUT_PULLUP);
#else
    pinMode(_pin, INPUT);
#endif
  }
  void task() override
  {
    int state = digitalRead(_pin);

#ifdef BUTTON_PULLUP
    bool isPressed = (state == LOW);
#else
    bool isPressed = (state == HIGH);
#endif
    if (isPressed && !_down)
    {
      _down = true;
      buttonDown.emit();
    }
    else if (!isPressed && _down)
    {
      _down = false;
      buttonUp.emit();
    }
  }
  bool has_task() const override
  {
    return true;
  }
signals:
  Signal<> buttonDown;
  Signal<> buttonUp;
};
#endif // !__BASE_BUTTON_H__