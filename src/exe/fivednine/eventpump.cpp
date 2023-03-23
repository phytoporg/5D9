#include "eventpump.h"

void EventPump::PostEvent(const SelectorEvent& event)
{
    m_eventQueue.push_back(event);
}

bool EventPump::GetNextEvent(SelectorEvent* pEventOut)
{
    if (m_eventQueue.empty())
    {
        return false;
    }

    *pEventOut = m_eventQueue.front();
    m_eventQueue.pop_front();
    return true;
}

bool EventPump::PeekNextEvent(SelectorEvent* pEventOut) const
{
    if (m_eventQueue.empty())
    {
        return false;
    }

    *pEventOut = m_eventQueue.front();
    return true;
}