#include <globals.h>
#include <views/default.h>
#include <adc.h>
#include <logger.h>
#include <settings.h>

#define PRECISION 4
#define GRAPH_HEIGHT 30
#define GRAPH_BOTTOM 60
#define GRAPH_COUNT  64

void extractSuffix(char *text, char suffix[6])
{
  char *ps = suffix;
  char *pt = text;

  while (*pt)
  {
    if (!isDigit(*pt) && *pt != '.')
    {
      *ps = *pt;
      *pt = '\0';
      ps++;
    }

    pt++;
  }

  *ps = '\0';
}

void limitText(char *text, int count)
{
  char *pch = text;
  while(*pch && count)
  {
    pch++;
    count--;
  }
  *pch = '\0';
}

void DefaultView::init(View *previousView)
{
  Graphics::GraphBuffer::owner = this;
  for (size_t i = 0; i < GRAPH_COUNT; i++)
  {
    Graphics::GraphBuffer::buffer[i] = 0;
  }
}

bool DefaultView::processDpad(const DPad::Action &action, View **newView)
{
  if (action.state.changed)
  {
    if (action.state.pressed)
    {
      switch (action.button)
      {
      case Button_Down:
        Adc::zero();
        break;

      case Button_Up:
        if (Logger::isLogging())
        {
          Logger::stop();
        }
        else
        {
          Logger::start(Settings::get()->logCadence);
        }
        break;
        
      case Button_Left:
      case Button_Right:
        _menu.init(this);
        *newView = &_menu;
        return true;
        break;

      case Button_Select:
        _seenSelect = true;
        break;
      }
    }
    else if (action.button == Button_Select && _seenSelect)
    {
        _menu.init(this);
        *newView = &_menu;
        return true;
    }
  }

  *newView = this;
  return false;
}

bool DefaultView::update(bool newReading)
{
  if (newReading || Graphics::GraphBuffer::owner != this)
  {
    int32_t reading, max;
    Adc::read(&reading, nullptr, &max);
    if (reading < 0)
    {
      reading = 0;
    }

    if (max < _max || Graphics::GraphBuffer::owner != this)
    {
      Graphics::GraphBuffer::owner = this;

      // ADC has been reset; reset the graph
      for (size_t i = 0; i < GRAPH_COUNT; i++)
      {
        Graphics::GraphBuffer::buffer[i] = 0;
      }
    }

    _max = max;
    Graphics::GraphBuffer::buffer[_count] = reading;
    _count = (_count + 1) % GRAPH_COUNT;
    return true;
  }
  return false;
}

void DefaultView::render()
{
  int32_t adc, adcAverage, adcMax;
  Adc::read(&adc, &adcAverage, &adcMax);

  char text[16];
  char suffix[4];

  Adc::toPower(adc, PRECISION, true, text);
  extractSuffix(text, suffix);

  if (Logger::isLogging())
  {
    display.setFont(MAIN_READING_FONT_SM);
  }
  else
  {
    // big font we're using only has room for 6 chars
    limitText(text, 6);
    display.setFont(MAIN_READING_FONT);
  }

  display.setCursor(0, 22);
  display.print(text);
  display.setFont(MAIN_UNITS_FONT);
  display.print(suffix);

  if (adc < 0) adc = 0;
  if (adcAverage < 0) adcAverage = 0;
  if (adcMax < 1) adcMax = 1;

  for (uint8_t idx = 0; idx < GRAPH_COUNT; idx++)
  {
    int i = (idx + _count) % GRAPH_COUNT;
    int h = (int)(Graphics::GraphBuffer::buffer[i] * GRAPH_HEIGHT / adcMax);

    display.drawVLine(idx * 2, GRAPH_BOTTOM - h, h);
  }

  int aveY = 30 + GRAPH_HEIGHT - (int)(adcAverage * GRAPH_HEIGHT / adcMax);
  display.drawHLine(0, aveY, 128);
  
  display.setFont(GRAPH_READING_FONT);

  display.setCursor(0, 30);
  Adc::toPower(adcMax, PRECISION, true, text);
  unsigned int textWidth = display.getStrWidth(text);
  display.setDrawColor(0);
  display.drawBox(0, GRAPH_HEIGHT - display.getAscent() - 1, textWidth + 1, display.getAscent() + 2);
  display.setDrawColor(1);
  display.print(text);

  aveY--;
  if (aveY < 36)
  {
    aveY = 36;
  }

  display.setCursor(0, aveY);
  Adc::toPower(adcAverage, PRECISION, true, text);
  textWidth = display.getStrWidth(text);
  display.setDrawColor(0);
  display.drawBox(0, aveY - display.getAscent() - 1, textWidth + 1, display.getAscent() + 2);
  display.setDrawColor(1);
  display.print(text);
}