#include <globals.h>
#include <adc.h>
#include <battery.h>
#include <button.h>
#include <power.h>
#include <flash.h>
#include <logger.h>
#include <views/default.h>

Button buttons[] =
    {
        {PIN_UP, BUTTON_ACTIVE_LOW},
        {PIN_DOWN, BUTTON_ACTIVE_LOW},
        {PIN_LEFT, BUTTON_ACTIVE_LOW},
        {PIN_RIGHT, BUTTON_ACTIVE_LOW},
        {PIN_SELECT, BUTTON_ACTIVE_LOW}
    };

DPad dpad(buttons);
DefaultView defaultView;
View *currentView;

void setup()
{
  Serial.begin(9600);
  
  Power::init();

  display.begin();
  
  Flash::init();
  Adc::init();
  Battery::init();
  Logger::init();

  pinMode(PIN_UP, INPUT_PULLUP);
  pinMode(PIN_DOWN, INPUT_PULLUP);
  pinMode(PIN_LEFT, INPUT_PULLUP);
  pinMode(PIN_RIGHT, INPUT_PULLUP);
  pinMode(PIN_SELECT, INPUT_PULLUP);

  // Start reading all button state
  // so buttons held down during
  // power up are ignored.

  for (unsigned int i = 0; i < COUNT_OF(buttons); i++)
  {
    buttons[i].zero();
  }

  defaultView.init(nullptr);
  currentView = &defaultView;
}

void loop()
{
  DPad::Action action = dpad.read();

  if (action.button == Button_Select)
  {
    Power::processPowerButton(action.state);
  }

  Power::tick();

  if (action.state.changed || Battery::isCharging() || Logger::isLogging())
  {
    Power::resetSleep();
  }

  bool newReading = Adc::update();
  bool updated = currentView->processDpad(action, &currentView);

  updated |= currentView->update(newReading);
  updated |= Battery::update();
  
  if (newReading && Logger::isLogging())
  {
    int32_t reading;
    Adc::read(&reading);

    if (reading < 0)
    {
      reading = 0;
    }

    updated |= Logger::update(reading);
  }

  if (updated)
  {
    display.firstPage();
    display.setFontDirection(0);

    do
    {
      currentView->render();
      Battery::render();
      Logger::render();
    } while (display.nextPage());
  }

  if (Battery::isDepleted())
  {
    Power::turnOff();
  }
}