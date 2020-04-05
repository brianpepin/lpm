#include <SPI.h>
#include <Wire.h>
#include <button.h>

#define DEBOUNCE_THRESHOLD 50
#define REPEAT_THRESHOLD   700

Button::Button(uint8_t pin, uint8_t flags) : _pin(pin),
                                             _debounce(0)
{
  _state.pressed = 0;
  _state.latched = 0;
  _state.changed = 0;
  _state.current = 0;
  _state.latchable = !!(flags & BUTTON_LATCHABLE);
  _state.active_low = !!(flags & BUTTON_ACTIVE_LOW);
}

ButtonState Button::read()
{
  uint8_t value = digitalRead(_pin);
  if (_state.active_low)
  {
    value = !value;
  }

  if (value != _state.current)
  {
    _debounce = millis();
    _state.current = value;
  }

  if (_state.current != _state.pressed && millis() - _debounce > DEBOUNCE_THRESHOLD)
  {
    if (_state.current && _state.latchable)
    {
      _state.latched = !_state.latched;
    }
    _state.pressed = _state.current;
    _state.repeat = false;

    ButtonState result = _state;
    result.changed = true;
    return result;
  }

  if (_state.pressed && millis() - _debounce > REPEAT_THRESHOLD)
  {
    _state.repeat = true;
  }

  return _state;
}

void Button::zero()
{
  uint8_t value = digitalRead(_pin);
  if (_state.active_low)
  {
    value = !value;
  }

  reset();
  _state.current = _state.pressed = value;

}

void Button::reset()
{
  _state.latched = false;
}

DPad::DPad(Button buttons[Button_Count])
    : _buttons(buttons)
{
  _active.button = Button_Count;
}

DPad::Action DPad::read()
{
  if (_active.button != Button_Count)
  {
    // We only care about the active button
    _active.state = _buttons[_active.button].read();
  }
  else
  {
    for (int btn = 0; btn < Button_Count; btn++)
    {
      ButtonState state = _buttons[btn].read();
      if (state.changed && state.pressed)
      {
        _active.button = btn;
        _active.state = state;
      }    
    }
  }

  Action action = _active;

  if (_active.button != Button_Count &&
      _active.state.changed && 
      !_active.state.pressed)
  {
    _active.button = Button_Count;
  }
  
    return action;
}
