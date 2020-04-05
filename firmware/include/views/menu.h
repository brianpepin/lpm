#pragma once

#include <views/menubase.h>
#include <views/calibrate.h>
#include <views/log.h>

class MenuView : public MenuBaseView
{
public:
    void init(View* previousView) override;
    bool update(bool newReading) override;
    
protected:
    void onSelect(int8_t row, View** newView) override;

private:
    CalibrateView _calibrate;
    LogView _log;
};
