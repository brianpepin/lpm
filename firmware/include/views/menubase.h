#pragma once

#include <view.h>

class MenuBaseView : public View
{
public:
    bool processDpad(const DPad::Action &action, View **newView) override;
    void render() override;

protected:
    void init(View *previousView, const char* title);
    virtual void onSelect(int8_t row, View** newView) = 0;
    void setItems(const char** menuItems);
    const char** getItems() { return _menuItems; }

private:
    View *_previousView;
    int8_t _row = 0;
    const char* _title;
    const char** _menuItems = nullptr;
    int8_t _rowCount = 0;
};
