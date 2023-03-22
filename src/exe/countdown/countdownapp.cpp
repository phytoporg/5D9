#include "countdownapp.h"
#include "appconfig.h"

#include <filesystem>

#include <countdown/render/window.h>
#include <countdown/log/check.h>

using namespace countdown;
using namespace countdown::render;

bool CountdownApp::Initialize(const AppConfig& configuration)
{
    if (!configuration.GetIsParsed())
    {
        RELEASE_LOG_ERROR(LOG_DEFAULT, "Configuration has not been parsed.");
        return false;
    }

    // Load textures
    const std::string& TexturesPath = configuration.GetTexturesPath();
    if (!std::filesystem::exists(TexturesPath))
    {
        RELEASE_LOG_ERROR(
            LOG_DEFAULT,
            "Textures path does not exist: %s",
            TexturesPath.c_str());
        return false;
    }

    for (const auto& directoryEntry : std::filesystem::directory_iterator(TexturesPath))
    {
        // Only support pngs for now
        const std::filesystem::path FilePath = directoryEntry.path();
        if (directoryEntry.is_regular_file() && FilePath.extension() == ".png")
        {
            if (!m_textureStorage.AddTextureFromImagePath(FilePath.c_str(), FilePath.stem().c_str()))
            {
                RELEASE_LOG_WARNING(
                    LOG_DEFAULT,
                    "Failed to add texture from file %s",
                    FilePath.c_str());
            }
        }
    }

    // TODO: Load shaders
    m_isInitialized = true;
    return true;
}

void CountdownApp::SetWindow(countdown::render::Window* pWindow)
{
    m_pWindow = pWindow;
}

void CountdownApp::Tick(float dtSeconds)
{
    RELEASE_CHECK(m_isInitialized, "Attempting to tick app without having initialized");
}

void CountdownApp::Draw()
{
    RELEASE_CHECK(m_isInitialized, "Attempting to draw app without having initialized");
}

bool CountdownApp::LoadTexturesFromPath(const std::string& texturesPath)
{
    return false;
}

bool CountdownApp::LoadShadersFromPath(const std::string& shadersPath)
{
    return false;
}