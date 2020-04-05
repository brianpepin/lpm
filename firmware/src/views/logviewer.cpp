#include <globals.h>
#include <views/logviewer.h>
#include <logger.h>
#include <adc.h>
#include <format.h>

#define GRAPH_WIDTH Graphics::GraphBuffer::max
#define GRAPH_HEIGHT 55
#define GRAPH_FOOTER 64
#define GRAPH_LEGEND 10
#define GRAPH_LEGEND_MARGIN 2
#define GRAPH_CURSOR_BOX_MARGIN 2
#define GRAPH_CURSOR_BOX_ARROW 5

void LogViewer::init(View *previousView)
{
    Graphics::GraphBuffer::owner = this;
    _previousView = previousView;
    _readingCount = 0;
    _avg = _max = 0;
    _cursor = -1;
    _readingMs = 0;

    uint16_t logSize = 0;
    uint32_t timeStamp;
    int32_t reading;

    while(Logger::fetch(logSize, &timeStamp, &reading))
    {
        logSize++;
        if (reading > _max) _max = reading;
    }

    uint16_t rpe = (logSize * 15) / (GRAPH_WIDTH * 10);
    if (rpe)
    {
        _readingCount = GRAPH_WIDTH;
    }
    else
    {
        _readingCount = logSize;
        rpe = 1;
    }

    _readingMs = (timeStamp * 1000) / _readingCount;
    
    uint16_t index = 0;
    uint16_t readings = 0;
    _readingCount = 0;

    while(Logger::fetch(index++, &timeStamp, &reading))
    {
        _avg += reading;

        if (readings == 0)
        {
            Graphics::GraphBuffer::buffer[_readingCount] = 0;
        }

        Graphics::GraphBuffer::buffer[_readingCount] += reading;

        // If we overshoot our last reading slot we will
        // just average the values into itl
        if (++readings == rpe && _readingCount < GRAPH_WIDTH)
        {
            Graphics::GraphBuffer::buffer[_readingCount] /= readings;
            _readingCount++;
            readings = 0;
        }
    }

    if (readings)
    {
        Graphics::GraphBuffer::buffer[_readingCount] /= readings;
    }

    _avg /= logSize;
}

bool LogViewer::processDpad(const DPad::Action &action, View **newView)
{
    bool update = false;

    if ((action.state.changed && action.state.pressed) || action.state.repeat)
    {
        switch(action.button)
        {
            case Button_Left:
                _cursor--;
                update = true;
                if (_cursor < -1)
                {
                    *newView = _previousView;
                }
                break;

            case Button_Right:
                if (_cursor < _readingCount - 1)
                {
                    _cursor++;
                    update = true;
                }
                break;

            default:
                *newView = _previousView;
                update = true;
                break;
        }
    }

    return update;
}

void LogViewer::drawCursorBox(uint8_t x, uint8_t y, uint8_t idx)
{
    display.setFont(GRAPH_READING_FONT);

    char power[20], time[20];
    int32_t sec = (_readingMs * idx) / 1000;
    Adc::toPower(Graphics::GraphBuffer::buffer[idx], 3, true, power);
    Format::toTime(sec, time);

    uint8_t boxH = display.getAscent() * 2 + GRAPH_CURSOR_BOX_MARGIN * 3;
    uint8_t powerW = display.getStrWidth(power);
    uint8_t timeW = display.getStrWidth(time);
    uint8_t boxW = powerW;

    if (timeW > powerW) boxW = timeW;
    boxW += 2 * GRAPH_CURSOR_BOX_MARGIN;

    // Keep the box on screen
    if (y + boxH > GRAPH_HEIGHT)
    {
        y = GRAPH_HEIGHT - boxH;
    }

    if (x + 1 + boxW > GRAPH_WIDTH)
    {
        x -= boxW;
    } 
    else
    {
        x++;
    }

    display.drawBox(x ,y, boxW, boxH);
    display.setDrawColor(0);
    display.setCursor(x + GRAPH_CURSOR_BOX_MARGIN, y + GRAPH_CURSOR_BOX_MARGIN + display.getAscent());
    display.print(time);
    display.setCursor(x + GRAPH_CURSOR_BOX_MARGIN, y + 2 * (GRAPH_CURSOR_BOX_MARGIN + display.getAscent()));
    display.print(power);
    display.setDrawColor(1);
}

void LogViewer::drawGraph()
{
    int8_t dx = display.getDisplayWidth() / _readingCount;
    int32_t dy = _max / GRAPH_HEIGHT;

    for (uint8_t x = 1; x < _readingCount; x++)
    {
        uint8_t x1 = (x - 1) * dx;
        uint8_t x2 = x * dx;
        uint8_t y1 = GRAPH_HEIGHT - (uint8_t)(Graphics::GraphBuffer::buffer[x - 1] / dy);
        uint8_t y2 = GRAPH_HEIGHT - (uint8_t)(Graphics::GraphBuffer::buffer[x] / dy);
        display.drawLine(x1, y1, x2, y2);
    }

    // Max, avg lines
    uint8_t yAvg = GRAPH_HEIGHT - (uint8_t)(_avg / dy);
    Graphics::drawDottedHLine(0, yAvg, display.getDisplayWidth());
    display.drawHLine(0, GRAPH_HEIGHT, display.getDisplayWidth());

    // Vertical cursor
    if (_cursor != -1)
    {
        uint8_t cx = (uint8_t)(_cursor * dx);
        uint8_t cy = GRAPH_HEIGHT - (uint8_t)(Graphics::GraphBuffer::buffer[_cursor] / dy);
        Graphics::drawDottedVLine(cx, 0, GRAPH_HEIGHT);
        drawCursorBox(cx, cy, (uint8_t)_cursor);
    }
}

void LogViewer::drawLegend()
{
    display.setFont(GRAPH_READING_FONT);
    Graphics::drawDottedHLine(GRAPH_LEGEND_MARGIN * 2, GRAPH_FOOTER - (display.getAscent() / 2), GRAPH_LEGEND);
    display.setCursor(GRAPH_LEGEND_MARGIN * 3 + GRAPH_LEGEND, GRAPH_FOOTER);
    
    char text[20];
    Adc::toPower(_avg, 3, true, text);
    display.print(text);

    display.drawHLine(display.getDisplayWidth() / 2, GRAPH_FOOTER - (display.getAscent() / 2), GRAPH_LEGEND);
    display.setCursor(display.getDisplayWidth() / 2 + GRAPH_LEGEND_MARGIN + GRAPH_LEGEND, GRAPH_FOOTER);
    Adc::toPower(_max, 3, true, text);
    display.print(text);
}

void LogViewer::render()
{
    drawGraph();
    drawLegend();
}
