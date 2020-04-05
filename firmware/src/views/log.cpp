#include <globals.h>
#include <views/log.h>
#include <logger.h>
#include <settings.h>
#include <exporter.h>

#define MI_START    0x00
#define MI_CADENCE  0x01
#define MI_VIEW     0x10
#define MI_EXPORT   0x11
#define MI_CLEAR    0x12

#define PG_NO_LOG   0
#define PG_LOG      1

#define MARGIN_TOP  35
#define MARGIN_LEFT 40

const char PROGMEM _menuTitle[] = "Logging";
const char PROGMEM _miStart[]   = "Start";
const char PROGMEM _miCadence[] = "Cadence";
const char PROGMEM _miView[]    = "View";
const char PROGMEM _miExport[]  = "Export";
const char PROGMEM _miClear[]   = "Clear";

const char * _noLogMenuText[] =
{
    _miStart,
    _miCadence,
    nullptr
};

const char * _logMenuText[] =
{
    _miView,
    _miExport,
    _miClear,
    nullptr
};

const char ** _pages[]
{
    _noLogMenuText,
    _logMenuText
};

int8_t computePage()
{
    if (Logger::isEmpty()) return PG_NO_LOG;
    return PG_LOG;
}

void LogView::init(View *previousView)
{
    MenuBaseView::init(previousView, _menuTitle);
    _page = computePage();
    _export = 0;
    setItems(_pages[_page]);
}

bool LogView::processDpad(const DPad::Action &action, View **newView)
{
    return MenuBaseView::processDpad(action, newView);
}

void LogView::onSelect(int8_t row, View** newView)
{
    int8_t item = _page << 4 | row;

    switch(item)
    {
        case MI_START:
            Logger::start(Settings::get()->logCadence);
            break;

        case MI_CADENCE:
            _cadence.init(this);
            *newView = &_cadence;
            break;

        case MI_CLEAR:
            Logger::clear();
            break;

        case MI_VIEW:
            _viewer.init(this);
            *newView = &_viewer;
            break;

        case MI_EXPORT:
            _export = 1;
            *newView = this;
            break;
    }
}

bool LogView::update(bool newReading)
{
    int8_t page = computePage();
    if (page != _page)
    {
        _page = page;
        setItems(_pages[_page]);
        return true;
    }

    if (_export == 2)
    {
        Exporter::exportLog();
        _export = 0;
        return true;
    }

    return false;
}

void LogView::render()
{
    if (_export)
    {
        display.setFont(NOTICE_FONT);
        display.setCursor(MARGIN_LEFT, MARGIN_TOP);
        display.print(F("Exporting..."));
        _export = 2;
    }
    else
    {
        MenuBaseView::render();
    }
}
