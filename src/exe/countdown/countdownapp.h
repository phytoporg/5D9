#pragma once

#include "selectable.h"
#include <string>
#include <cstdint>

#include <countdown/render/texturestorage.h>
#include <countdown/render/shaderstorage.h>

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
        bool Initialize(const AppConfig& configuration);
        void SetWindow(countdown::render::Window* pWindow);

        void Tick(float dtSeconds);
        void Draw();

    private:
        bool LoadTexturesFromPath(const std::string& texturesPath);
        bool LoadShadersFromPath(const std::string& shadersPath);

    private:
        bool                       m_isInitialized = false;
        countdown::render::Window* m_pWindow = nullptr;
        countdown::render::TextureStorage m_textureStorage;
        countdown::render::ShaderStorage  m_shaderStorage;

        // TODO: factor app state into common structure
        // TODO: static array container
        static constexpr uint8_t kMaxSelectableEntries = 255; 
        Selectable m_selectables[kMaxSelectableEntries];
        uint8_t m_currentSelectableIndex = 0;
        uint8_t m_numSelectables = 0;
};