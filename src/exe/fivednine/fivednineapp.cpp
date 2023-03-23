#include "fivednineapp.h"
#include "appconfig.h"

#include <algorithm>
#include <filesystem>
#include <vector>
#include <fstream>
#include <sstream>

#include <json/json.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <fivednine/render/window.h>
#include <fivednine/log/log.h>
#include <fivednine/log/check.h>

using json = nlohmann::json;
using namespace fivednine;
using namespace fivednine::render;

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

bool fivednineApp::Initialize(const AppConfig& configuration)
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

    // Load games info to populate selectables
    const std::string& GamesDBPath = configuration.GetGamesDbPath();
    if (!std::filesystem::exists(GamesDBPath))
    {
        RELEASE_LOGLINE_ERROR(
            LOG_DEFAULT,
            "Games database configuration path does not exist: %s",
            GamesDBPath.c_str());
        return false;
    }

    std::ifstream gamesDbIn(GamesDBPath);
    json gamesDbData = json::parse(gamesDbIn);
    if (!gamesDbData.contains("games"))
    {
        RELEASE_LOGLINE_ERROR(
            LOG_DEFAULT,
            "Games database missing required field 'games': %s",
            GamesDBPath.c_str());
        return false;
    }

    // Again, defer failure so we can log as much info as possible
    bool failedParseGames = false;
    json gamesArray = gamesDbData["games"];
    for (const auto& gameEntry : gamesArray)
    {
        if (m_numSelectables >= kMaxSelectableEntries)
        {
            // Don't fail here, but do log a warning.
            RELEASE_LOGLINE_WARNING(
                LOG_DEFAULT,
                "Reached maximum number of supported selectable games. Stopping.");
            return true;
        }

        Selectable selectableGame;;
        if (!gameEntry.contains("title"))
        {
            RELEASE_LOGLINE_ERROR(
                LOG_DEFAULT,
                "Game DB entry is missing required field: 'title'");
            failedParseGames = true;
            continue;
        }
        selectableGame.Title = gameEntry["title"].get<std::string>();

        if (!gameEntry.contains("alias"))
        {
            RELEASE_LOGLINE_ERROR(
                LOG_DEFAULT,
                "Game DB entry %s is missing required field: 'alias'",
                selectableGame.Title.c_str());
            failedParseGames = true;
            continue;
        }
        selectableGame.Alias = gameEntry["alias"].get<std::string>();

        if (!gameEntry.contains("texture_prefix"))
        {
            RELEASE_LOGLINE_ERROR(
                LOG_DEFAULT,
                "Game DB entry %s is missing required field: 'texture_prefix'",
                selectableGame.Title.c_str());
            failedParseGames = true;
            continue;
        }
        selectableGame.TexturePrefix = gameEntry["texture_prefix"].get<std::string>();

        m_selectables[m_numSelectables] = selectableGame;
        ++m_numSelectables;
    }

    if (failedParseGames)
    {
        if (m_numSelectables == 0)
        {
            RELEASE_LOGLINE_ERROR(
                LOG_DEFAULT,
                "Failed to load any selectable games. Exiting."
            );
            return false;
        }
        else
        {
            RELEASE_LOGLINE_WARNING(
                LOG_DEFAULT,
                "Failed to load some selectable games. Continuing initialization."
            );
        }
    }

    // Initialize view & projection matrices
    // TODO: Decouple from window size
    uint32_t windowWidth, windowHeight;
    m_pWindow->GetWindowDimensions(&windowWidth, &windowHeight);
    m_projectionMatrix = glm::ortho(
        0.f, static_cast<float>(windowWidth),
        static_cast<float>(windowHeight), 0.f,
        0.1f, 1000.f
    );

    // TODO: Fix this wacked-out coordinat system
    glm::vec3 cameraPosition(windowWidth / -2.f, windowHeight / -2.f, 1.f);
    glm::vec3 cameraForward(0.f, 0.f, -1.f);
    glm::vec3 cameraUp(0.f, 1.f, 0.f);
    m_viewMatrix = glm::lookAt(
        cameraPosition,
        cameraPosition + cameraForward,
        cameraUp
    );

    // PLACEHOLDER
    ShaderPtr spGameCardShader = m_shaderStorage.FindShaderByName("selectable");
    TexturePtr spGameCardTexture = m_textureStorage.FindTextureByName("plusr_600x900");
    const float cardWidth = 200.0f;
    const float cardHeight = 360.0f;
    const glm::vec3 upperLeft(cardWidth / -2.f, cardHeight / -2.f, 0.f);
    m_spGameCard.reset(
        new GameCard(spGameCardShader, spGameCardTexture, upperLeft, cardWidth, cardHeight)
    );

    m_isInitialized = true;
    return true;
}

void fivednineApp::SetWindow(fivednine::render::Window* pWindow)
{
    m_pWindow = pWindow;
}

void fivednineApp::Tick(float dtSeconds)
{
    RELEASE_CHECK(m_isInitialized, "Attempting to tick app without having initialized");
}

void fivednineApp::Draw()
{
    RELEASE_CHECK(m_isInitialized, "Attempting to draw app without having initialized");
    m_spGameCard->Draw(m_projectionMatrix, m_viewMatrix);
}

bool fivednineApp::LoadTexturesFromPath(const std::string& texturesPath)
{
    return false;
}

bool fivednineApp::LoadShadersFromPath(const std::string& shadersPath)
{
    return false;
}