
#pragma once

#include <button.h>
#include <graphics.h>

class View
{
public:
    virtual void init(View *previousView) = 0;
    virtual bool processDpad(const DPad::Action &action, View **newView) = 0;
    virtual bool update(bool newReading) { return false; }
    virtual void render() = 0;
};
