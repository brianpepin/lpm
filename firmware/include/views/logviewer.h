#pragma once

#include <view.h>

class LogViewer : public View
{
public:
    void init(View *previousView) override;
    bool processDpad(const DPad::Action &action, View **newView) override;
    void render() override;

private:
    View* _previousView;
    uint8_t _readingCount;
    int32_t _avg = 0;
    int32_t _max = 0;
    int16_t _cursor = -1;
    uint32_t _readingMs = 0;

    void drawGraph();
    void drawLegend();
    void drawCursorBox(uint8_t x, uint8_t y, uint8_t idx);
};