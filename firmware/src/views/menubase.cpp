#include <globals.h>
#include <views/menubase.h>

#define MARGIN_ARROW 2
#define ARROW_OFFSET 0
#define MARGIN_TEXT 12
#define MARGIN_TOP 23
#define LINE_HEIGHT 12

void MenuBaseView::init(View *previousView, const char* title)
{
    _title = title;
    _previousView = previousView;
    _row = 0;
}

bool MenuBaseView::processDpad(const DPad::Action &action, View **newView)
{
    *newView = this;

    if (action.state.changed && action.state.pressed)
    {
        switch (action.button)
        {
        case Button_Left:
            *newView = _previousView;
            _row = 0;
            return true;
            break;

        case Button_Down:
            _row = (_row + 1) % _rowCount;
            return true;
            break;

        case Button_Up:
            if (_row > 0)
            {
                _row--;
            }
            else
            {
                _row = _rowCount - 1;
            }
            return true;
            break;

        case Button_Right:
        case Button_Select:
            *newView = _previousView;
            onSelect(_row, newView);
            return true;
            break;
        }
    }

    return false;
}

void MenuBaseView::render()
{
    display.setFont(MENU_HEADER_FONT);
    display.setCursor(MARGIN_TEXT, display.getAscent());
    Graphics::printPROGMEM(_title);
    display.drawHLine(MARGIN_TEXT, display.getAscent() + 2, display.getDisplayWidth() - MARGIN_TEXT);

    display.setFont(MENU_ITEM_FONT);

    if (_rowCount)
    {
        for (uint8_t i = 0; i < _rowCount; i++)
        {
            display.setCursor(MARGIN_TEXT, LINE_HEIGHT * i + MARGIN_TOP);
            Graphics::printPROGMEM(_menuItems[i]);
        }

        display.setFont(MENU_ARROW_FONT);
        display.setCursor(MARGIN_ARROW, LINE_HEIGHT * _row + MARGIN_TOP + ARROW_OFFSET);
        display.print((char)78);
    }
}

void MenuBaseView::setItems(const char** menuItems)
{
    _row = 0;
    _rowCount = 0;
    _menuItems = menuItems;

    auto item = menuItems;

    while(*item)
    {
        _rowCount++;
        item++;
    }
}
