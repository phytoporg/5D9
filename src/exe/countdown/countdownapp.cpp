#include "countdownapp.h"
#include "appconfig.h"

#include <algorithm>
#include <filesystem>
#include <vector>
#include <fstream>
#include <sstream>

#include <countdown/render/window.h>
#include <countdown/log/log.h>
#include <countdown/log/check.h>

using namespace countdown;
using namespace countdown::render;

namespace 
{
    bool IsTextureAssetPath(const std::filesystem::path& filePath)
    {
        // Just supports png files for the moment
        return filePath.extension() == ".png";
    }

    bool IsShaderAssetPath(const std::filesystem::path& filePath)
    {
        // glsl only
        return filePath.extension() == ".glsl";
    }

    bool 
    ShaderProgramNameAndShaderTypeFromFilePath(
        const std::filesystem::path& filePath,
        std::string* pProgramNameOut,
        std::string* pShaderTypeOut)
    {
        RELEASE_CHECK(pProgramNameOut != nullptr, "pProgramNameOut cannot be null");
        *pProgramNameOut = "";

        RELEASE_CHECK(pShaderTypeOut != nullptr, "pShaderTypeOut cannot be null");
        *pShaderTypeOut = "";

        const std::string StemString = filePath.stem().string();

        // The expected format is <programname>_<shadertype>.<extension>
        const size_t LastUnderscoreIndex = StemString.find_last_of('_');
        if (LastUnderscoreIndex == std::string::npos)
        {
            RELEASE_LOG_WARNING(
                LOG_DEFAULT,
                "Improperly named shader: %s",
                filePath.string());
                return false;
        }

        *pProgramNameOut = StemString.substr(0, LastUnderscoreIndex);
        *pShaderTypeOut = StemString.substr(LastUnderscoreIndex + 1);
        return true;
    }
}

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
        const std::filesystem::path FilePath = directoryEntry.path();
        if (directoryEntry.is_regular_file() && IsTextureAssetPath(FilePath))
        {
            const std::string& textureName = FilePath.stem().string();
            if (!m_textureStorage.AddTextureFromImagePath(FilePath.c_str(), textureName))
            {
                RELEASE_LOGLINE_WARNING(
                    LOG_DEFAULT,
                    "Failed to add texture from file %s",
                    FilePath.c_str());
            }
            else
            {
                RELEASE_LOGLINE_INFO(
                    LOG_DEFAULT,
                    "Successfully added texture %s",
                    textureName.c_str()
                );
            }
        }
    }

    // Load shaders
    const std::string& ShadersPath = configuration.GetShadersPath();
    if (!std::filesystem::exists(ShadersPath))
    {
        RELEASE_LOGLINE_ERROR(
            LOG_DEFAULT,
            "Shaders path does not exist: %s",
            ShadersPath.c_str());
        return false;
    }

    // Gather up shader file lookups
    struct ShaderProgramLookup
    {
        ShaderProgramLookup(const std::string& programName)
            : ProgramName(programName) {}

        std::string ProgramName;
        std::string VertexShaderPath;
        std::string FragmentShaderPath;
    };
    std::vector<ShaderProgramLookup> shaderLookupStates;

    // Find shader files in the target directory
    for (const auto& directoryEntry : std::filesystem::directory_iterator(ShadersPath))
    {
        const std::filesystem::path FilePath = directoryEntry.path();
        if (directoryEntry.is_regular_file() && IsShaderAssetPath(FilePath))
        {
            std::string programName;
            std::string shaderType;
            if (!ShaderProgramNameAndShaderTypeFromFilePath(FilePath, &programName, &shaderType))
            {
                continue;
            }

            auto it = std::find_if(std::begin(shaderLookupStates), std::end(shaderLookupStates),
                [programName](const ShaderProgramLookup& lookup) -> bool
                {
                    return lookup.ProgramName == programName;
                }
            );
            if (it == std::end(shaderLookupStates))
            {
                shaderLookupStates.emplace_back(programName);
                it = std::end(shaderLookupStates) - 1;
            }

            if (shaderType == "vert")
            {
                it->VertexShaderPath = FilePath.string();
            }
            else if (shaderType == "frag")
            {
                it->FragmentShaderPath = FilePath.string();
            }
            else
            {
                RELEASE_LOGLINE_WARNING(
                    LOG_DEFAULT,
                    "Unexpected shader type for file %s",
                    FilePath.string().c_str());
            }
        }
    }

    // Read, compile, link and store the shader programs

    // Defer failure for comprehensive logging
    bool failedShaderLoad = false;
    for (const ShaderProgramLookup& shaderProgramLookup : shaderLookupStates)
    {
        std::ifstream vertexShaderIn(shaderProgramLookup.VertexShaderPath);
        if (!vertexShaderIn.good())
        {
            RELEASE_LOGLINE_ERROR(
                LOG_DEFAULT,
                "Failed to open vertex shader %s for shader program %s",
                shaderProgramLookup.VertexShaderPath.c_str(),
                shaderProgramLookup.ProgramName.c_str()
            );
            failedShaderLoad = true;
            continue;
        }

        std::ifstream fragmentShaderIn(shaderProgramLookup.FragmentShaderPath);
        if (!fragmentShaderIn.good())
        {
            RELEASE_LOGLINE_ERROR(
                LOG_DEFAULT,
                "Failed to open fragment shader %s for shader program %s",
                shaderProgramLookup.FragmentShaderPath.c_str(),
                shaderProgramLookup.ProgramName.c_str()
            );
            failedShaderLoad = true;
            continue;
        }

        std::stringstream vertexStringStream;
        vertexStringStream << vertexShaderIn.rdbuf();

        std::stringstream fragmentStringStream;
        fragmentStringStream << fragmentShaderIn.rdbuf();

        if (!m_shaderStorage.AddShader(
            vertexStringStream.str(),
            fragmentStringStream.str(),
            shaderProgramLookup.ProgramName))
        {
            RELEASE_LOGLINE_ERROR(
                LOG_DEFAULT,
                "Failed to add shader program %s",
                shaderProgramLookup.ProgramName.c_str());
            failedShaderLoad = true;
        }
        else
        {
            RELEASE_LOGLINE_INFO(
                LOG_DEFAULT,
                "Successfully added shader program %s",
                shaderProgramLookup.ProgramName.c_str()
            );
        }
    }

    if (failedShaderLoad)
    {
        RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "Failed to load one or more shaders.");
        return false;
    }

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