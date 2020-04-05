#pragma once
#include <view.h>

class CadenceView : public View
{
public:
    void init(View *previousView) override;
    bool processDpad(const DPad::Action &action, View **newView) override;
    void render() override;

private:
    View* _previousView = nullptr;
    int8_t _cadence = 0;

    void updateCadence(int8_t delta);
};