
#pragma once

#include <view.h>
#include <views/menu.h>

class DefaultView : public View
{
public:
    void init(View *previousView) override;
    bool processDpad(const DPad::Action &action, View **newView) override;
    bool update(bool newReading) override;
    void render() override;

private:
    MenuView _menu;
    uint8_t _count = 0;
    int32_t _max = 0;
    bool _seenSelect = false;
};
