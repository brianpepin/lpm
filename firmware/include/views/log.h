#pragma once

#include <views/menubase.h>
#include <views/cadence.h>
#include <views/logviewer.h>

class LogView : public MenuBaseView
{
public:
    void init(View *previousView) override;
    bool processDpad(const DPad::Action &action, View **newView) override;
    bool update(bool newReading) override;
    void render() override;

protected:
    void onSelect(int8_t row, View** newView) override;

private:
    CadenceView _cadence;
    LogViewer _viewer;
    int8_t _page = 0;
    uint8_t _export = 0;
};
