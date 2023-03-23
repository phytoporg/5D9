#pragma once

#include <cstddef>

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
    void DoSelectIndex(uint32_t selectedIndex);

    fivednineApp* m_pApp = nullptr;
    EventPump*    m_pEventPump = nullptr;
};