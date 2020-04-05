#pragma once

struct ButtonState
{
  uint8_t pressed : 1;
  uint8_t repeat : 1; 
  uint8_t latched : 1;
  uint8_t changed : 1;
  uint8_t current : 1;
  uint8_t active_low : 1;
  uint8_t latchable : 1;
};

#define BUTTON_LATCHABLE 1
#define BUTTON_ACTIVE_LOW 2

//
// Class Declarations
//

class Button
{
public:
  Button(uint8_t pin, uint8_t flags);
  ButtonState read();
  void reset();
  void zero();

private:
  uint8_t _pin;
  unsigned long _debounce;
  ButtonState _state;
};

// DPad as an array of buttons
#define Button_Up 0
#define Button_Down (Button_Up + 1)
#define Button_Left (Button_Down + 1)
#define Button_Right (Button_Left + 1)
#define Button_Select (Button_Right + 1)
#define Button_Count (Button_Select + 1)

class DPad
{
public:
  struct Action
  {
    uint8_t button;
    ButtonState state;
  };

  DPad(Button buttons[Button_Count]);
  Action read();

private:
  Action _active;
  Button *_buttons;
};