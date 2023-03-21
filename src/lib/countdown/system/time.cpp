#include "time.h"
#include <SDL.h>

uint32_t countdown::system::time::GetTicksMs()
{
    return static_cast<uint32_t>(SDL_GetTicks());
}

void countdown::system::time::SleepMs(uint32_t milliseconds)
{
    SDL_Delay(milliseconds);
}