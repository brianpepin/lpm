#pragma once
#include <view.h>

class CalibrateView : public View
{
public:
    void init(View *previousView) override;
    bool processDpad(const DPad::Action &action, View **newView) override;
    bool update(bool newReading) override;
    void render() override;

private:
    View* _previousView = nullptr;
    int8_t _digits[6];
    int8_t _select = 0;
    int32_t _initialCalibration;
    int32_t _initialReading;
    int32_t _currentReading;
    int32_t _saveMillis = 0;
};