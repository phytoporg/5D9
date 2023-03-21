#pragma once

#include "selectable.h"
#include <string>
#include <cstdint>

namespace countdown 
{
    namespace render 
    {
        class Window;
    }
}

class AppConfig;
class CountdownApp
{
    public:
        CountdownApp(countdown::render::Window* pWindow, const AppConfig& configuration);

        bool Initialize();

        void Tick(float dtSeconds);
        void Draw();

    private:
        bool LoadShadersFromPath(const std::string& shadersPath);

    private:
        countdown::render::Window* m_pWindow = nullptr;
        bool                       m_isInitialized = false;

        // TODO: factor app state into common structure
        // TODO: static array container
        static constexpr uint8_t kMaxSelectableEntries = 255; 
        Selectable m_selectables[kMaxSelectableEntries];
        uint8_t m_currentSelectableIndex = 0;
        uint8_t m_numSelectables = 0;
};