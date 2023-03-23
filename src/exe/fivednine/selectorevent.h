#pragma once

// Input events
enum class SelectorInputEventType
{
    None = 0,
    NextSelection,
    PreviousSelection,
    ConfirmCurrent,
    Max
};

struct SelectorInputEventPayload
{
    SelectorInputEventType InputEventType;
};

// General selector event
enum class SelectorEventType
{
    None = 0,
    Input,
    Max
};

struct SelectorEvent
{
    SelectorEventType EventType;
    union 
    {
        SelectorInputEventPayload InputEventPayload;
    } EventPayload;

    SelectorEvent& operator=(const SelectorEvent& other)
    {
        EventType = other.EventType;
        if (EventType == SelectorEventType::Input)
        {
            EventPayload.InputEventPayload = other.EventPayload.InputEventPayload;
        }

        return *this;
    }
};