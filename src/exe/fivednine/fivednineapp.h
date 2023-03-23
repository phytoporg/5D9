#pragma once

#include "selectable.h"

// ****
#include "gamecard.h" // TODO: forward decl?
#include <memory>
// ****

#include <string>
#include <cstdint>

#include <glm/glm.hpp>

#include <fivednine/render/texturestorage.h>
#include <fivednine/render/shaderstorage.h>

namespace fivednine 
{
    namespace render 
    {
        class Window;
    }
}

class AppConfig;
class fivednineApp
{
    public:
        bool Initialize(const AppConfig& configuration);
        void SetWindow(fivednine::render::Window* pWindow);

        void Tick(float dtSeconds);
        void Draw();

    private:
        bool LoadTexturesFromPath(const std::string& texturesPath);
        bool LoadShadersFromPath(const std::string& shadersPath);

    private:
        bool                       m_isInitialized = false;
        fivednine::render::Window* m_pWindow = nullptr;
        fivednine::render::TextureStorage m_textureStorage;
        fivednine::render::ShaderStorage  m_shaderStorage;

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