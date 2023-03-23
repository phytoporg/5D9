#pragma once

#include "selectorevent.h"
#include <deque>

class EventPump
{
public:
    void PostEvent(const SelectorEvent& event);
    bool GetNextEvent(SelectorEvent* pEventOut);
    bool PeekNextEvent(SelectorEvent* pEventOut) const;

private:
    std::deque<SelectorEvent> m_eventQueue;
};