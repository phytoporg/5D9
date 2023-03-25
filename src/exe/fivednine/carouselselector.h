#pragma once

#include "selectorevent.h"
#include <cstddef>
#include <cstdint>

struct GameInfo;
class  EventPump;
class  fivednineApp;

class CarouselSelector
{
public:
    CarouselSelector(fivednineApp* pApp, EventPump* pEventPump);
    bool Initialize();

    void Tick(float dtSeconds);

private:
    void HandleInputEvent(const SelectorInputEventPayload& inputEventPayload);
    void MoveCameraToCard(uint32_t cardIndex);
    void SnapCameraToCard(uint32_t cardIndex);

    fivednineApp* m_pApp = nullptr;
    EventPump*    m_pEventPump = nullptr;
};