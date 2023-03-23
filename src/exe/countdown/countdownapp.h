#pragma once

#include "selectable.h"

// ****
#include "gamecard.h" // TODO: forward decl?
#include <memory>
// ****

#include <string>
#include <cstdint>

#include <glm/glm.hpp>

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

        glm::mat4 m_projectionMatrix;
        glm::mat4 m_viewMatrix; // TODO: Replace with camera

        std::unique_ptr<GameCard> m_spGameCard; // Just testing

        // TODO: factor app state into common structure
        // TODO: static array container
        static constexpr uint8_t kMaxSelectableEntries = 255; 
        Selectable m_selectables[kMaxSelectableEntries];
        uint8_t m_currentSelectableIndex = 0;
        uint8_t m_numSelectables = 0;
};