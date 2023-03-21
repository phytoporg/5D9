#pragma once

#include "selectable.h"
#include <cstdint>

namespace countdown 
{
    namespace render 
    {
        class Window;
    }
}

class CountdownApp
{
    public:
        CountdownApp(countdown::render::Window* pWindow);

        void Tick(float dtSeconds);
        void Draw();

    private:
        countdown::render::Window* m_pWindow = nullptr;

        // TODO: factor app state into common structure
        // TODO: static array container
        static constexpr uint8_t kMaxSelectableEntries = 255; 
        Selectable m_selectables[kMaxSelectableEntries];
        uint8_t m_currentSelectableIndex = 0;
        uint8_t m_numSelectables = 0;
};