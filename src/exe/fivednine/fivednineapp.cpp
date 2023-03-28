#include "fivednineapp.h"
#include "appconfig.h"

#include <algorithm>
#include <filesystem>
#include <vector>
#include <fstream>
#include <sstream>

#include <json/json.hpp>

#include <glm/gtc/matrix_transform.hpp>

#include <common/log/log.h>
#include <common/log/check.h>

#include <fivednine/render/window.h>

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
                filePath.c_str());
                return false;
        }

        *pProgramNameOut = StemString.substr(0, LastUnderscoreIndex);
        *pShaderTypeOut = StemString.substr(LastUnderscoreIndex + 1);
        return true;
    }
}

bool fivednineApp::Initialize(const AppConfig& configuration, Window* pWindow)
{
    RELEASE_CHECK(pWindow, "pWindow cannot be null");
    m_pWindow = pWindow;

    if (!configuration.GetIsParsed())
    {
        RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "Configuration has not been parsed.");
        return false;
    }

    if (!LoadTextures(configuration))
    {
        return false;
    }

    if (!LoadShaders(configuration))
    {
        return false;
    }

    if (!LoadGamesInfo(configuration))
    {
        return false;
    }

    // Initialize projection matrix
    // TODO: Decouple from window size
    uint32_t windowWidth, windowHeight;
    m_pWindow->GetWindowDimensions(&windowWidth, &windowHeight);
    m_projectionMatrix = glm::ortho(
        0.f, static_cast<float>(windowWidth),
        static_cast<float>(windowHeight), 0.f,
        0.1f, 1000.f
    );

    // Initialize camera
    m_camera.SetTranslation(glm::vec3(windowWidth / -2.f, windowHeight / -2.f, 1.f));

    // Initialize game cards
    ShaderPtr spGameCardShader = m_shaderStorage.FindShaderByName("gamecard");
    for (uint32_t i = 0; i < m_numGameInfos; ++i)
    {
        m_gameCards.emplace_back(new GameCard(spGameCardShader));
        RELEASE_CHECK(m_gameCards.back() != nullptr, "Failed to allocate game card");
    }
    m_spSelector.reset(new CarouselSelector(this, &m_selectorEventPump));
    RELEASE_CHECK(m_spSelector != nullptr, "Failed to allocate selector");

    if (!m_spSelector->Initialize())
    {
        RELEASE_LOGLINE_ERROR(LOG_DEFAULT, "Failed to initialize selector");
        return false;
    }

    // TODO: Factor out all of the input goo
    m_pWindow->SetKeyStateChangedHandler(HandleKeypress);
    m_pWindow->SetUserPointer(this);

    m_isInitialized = true;
    return true;
}

void fivednineApp::Tick(float dtSeconds)
{
    RELEASE_CHECK(m_isInitialized, "Attempting to tick app without having initialized");
    m_spSelector->Tick(dtSeconds);
    m_camera.Tick(dtSeconds);
}

void fivednineApp::Draw()
{
    RELEASE_CHECK(m_isInitialized, "Attempting to draw app without having initialized");
    for (auto spGameCard : m_gameCards)
    {
        spGameCard->Draw(m_projectionMatrix, m_camera.ViewMatrix4());
    }
}

bool fivednineApp::LoadTextures(const AppConfig& configuration)
{
    // Load textures
    const std::string& TexturesPath = configuration.GetTexturesPath();
    if (!std::filesystem::exists(TexturesPath))
    {
        RELEASE_LOGLINE_ERROR(
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

    return true;
}

bool fivednineApp::LoadShaders(const AppConfig& configuration)
{
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

    return true;
}

bool fivednineApp::LoadGamesInfo(const AppConfig& configuration)
{
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
        if (m_numGameInfos >= kMaxGameInfoEntries)
        {
            // Don't fail here, but do log a warning.
            RELEASE_LOGLINE_WARNING(
                LOG_DEFAULT,
                "Reached maximum number of supported selectable games. Stopping.");
            return true;
        }

        GameInfo gameInfo;
        if (!gameEntry.contains("title"))
        {
            RELEASE_LOGLINE_ERROR(
                LOG_DEFAULT,
                "Game DB entry is missing required field: 'title'");
            failedParseGames = true;
            continue;
        }
        gameInfo.Title = gameEntry["title"].get<std::string>();

        if (!gameEntry.contains("alias"))
        {
            RELEASE_LOGLINE_ERROR(
                LOG_DEFAULT,
                "Game DB entry %s is missing required field: 'alias'",
                gameInfo.Title.c_str());
            failedParseGames = true;
            continue;
        }
        gameInfo.Alias = gameEntry["alias"].get<std::string>();

        if (!gameEntry.contains("texture_prefix"))
        {
            RELEASE_LOGLINE_ERROR(
                LOG_DEFAULT,
                "Game DB entry %s is missing required field: 'texture_prefix'",
                gameInfo.Title.c_str());
            failedParseGames = true;
            continue;
        }
        gameInfo.TexturePrefix = gameEntry["texture_prefix"].get<std::string>();

        m_gameInfoArray[m_numGameInfos] = gameInfo;
        ++m_numGameInfos;
    }

    if (failedParseGames)
    {
        if (m_numGameInfos == 0)
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

    return true;
}

// API METHODS
uint32_t fivednineApp::Selector_GetNumCards()
{
    return m_numGameInfos;
}

void fivednineApp::Selector_SelectIndex(uint32_t index)
{
    RELEASE_CHECK(index >= 0 && index < m_numGameInfos, "Invalid card index: %u", index);
    m_currentSelectedCardIndex = index;
}

uint32_t fivednineApp::Selector_GetSelectedIndex()
{
    return m_currentSelectedCardIndex;
}

void fivednineApp::Selector_ConfirmCurrentSelection()
{
    // TODO
}

void fivednineApp::Selector_GetDisplayDimensions(uint32_t* pWidthOut, uint32_t* pHeightOut)
{
    m_pWindow->GetWindowDimensions(pWidthOut, pHeightOut);
}

bool fivednineApp::Selector_GetCardGameInfo(uint32_t index, GameInfo* pGameInfoOut)
{
    if (!pGameInfoOut)
    {
        RELEASE_LOGLINE_ERROR(LOG_API, "pGameInfoOut cannot be null");
        return false;
    }

    if (index > m_numGameInfos)
    {
        RELEASE_LOGLINE_ERROR(LOG_API, "Game card index out of bounds: %u", index);
        return false;
    }

    *pGameInfoOut = m_gameInfoArray[index];
    return true;
}

bool fivednineApp::Selector_SetCardAppearanceParam1f(uint32_t index, const char* pParameterName, float *pValue)
{
    if (index > m_numGameInfos)
    {
        RELEASE_LOGLINE_ERROR(LOG_API, "Game card index out of bounds: %u", index);
        return false;
    }

    m_gameCards[index]->SetUniformValue1f(pParameterName, pValue);
    return false;
}

bool fivednineApp::Selector_GetCardPosition(uint32_t index, glm::vec3* pCardPositionOut)
{
    if (index > m_numGameInfos)
    {
        RELEASE_LOGLINE_ERROR(LOG_API, "Game card index out of bounds: %u", index);
        return false;
    }
    
    if (!pCardPositionOut)
    {
        RELEASE_LOGLINE_ERROR(LOG_API, "pCardPosition cannot be null");
        return false;
    }

    *pCardPositionOut = m_gameCards[index]->GetPosition();
    return true;
}

bool fivednineApp::Selector_SetCardPosition(uint32_t index, float x, float y, float z)
{
    if (index > m_numGameInfos)
    {
        RELEASE_LOGLINE_ERROR(LOG_API, "Game card index out of bounds: %u", index);
        return false;
    }

    m_gameCards[index]->SetPosition(x, y, z);
    return true;
}

bool fivednineApp::Selector_SetCardDimensions(uint32_t index, float width, float height)
{
    if (index > m_numGameInfos)
    {
        RELEASE_LOGLINE_ERROR(LOG_API, "Game card index out of bounds: %u", index);
        return false;
    }

    m_gameCards[index]->SetDimensions(width, height);
    return false;
}

bool fivednineApp::Selector_SetCardTexture(uint32_t index, const char* pTextureName)
{
    if (index > m_numGameInfos)
    {
        RELEASE_LOGLINE_ERROR(LOG_API, "Game card index out of bounds: %u", index);
        return false;
    }

    TexturePtr spTexture = m_textureStorage.FindTextureByName(pTextureName);
    if (!spTexture)
    {
        RELEASE_LOGLINE_WARNING(LOG_API, "Failed to find texture %s", pTextureName);
        return false;
    }

    m_gameCards[index]->SetTexture(spTexture);
    return true;
}

void fivednineApp::Selector_SetCameraTarget(const glm::vec3& target)
{
    m_camera.SetTranslationTarget(target);
}

void fivednineApp::Selector_SetCameraPosition(const glm::vec3& position)
{
    m_camera.SetTranslation(position);
}

void 
fivednineApp::HandleKeypress(
    Window::EventType eventType,
    Window::KeyType keyType,
    void* pUserPointer)
{
    if (eventType == Window::EventType::KeyDown && pUserPointer)
    {
        switch (keyType)
        {
            case Window::KeyType::Q:
            {
                Window* pWindow = static_cast<fivednineApp*>(pUserPointer)->m_pWindow;
                pWindow->Quit();
                break;
            }
            case Window::KeyType::Left:
                // fallthrough
            case Window::KeyType::A:
            {
                EventPump& eventPump = static_cast<fivednineApp*>(pUserPointer)->m_selectorEventPump;

                SelectorEvent event;
                event.EventType = SelectorEventType::Input;
                event.EventPayload.InputEventPayload.InputEventType = SelectorInputEventType::PreviousSelection;
                eventPump.PostEvent(event);
                break;
            }
            case Window::KeyType::Right:
                // fallthrough
            case Window::KeyType::D:
            {
                EventPump& eventPump = static_cast<fivednineApp*>(pUserPointer)->m_selectorEventPump;

                SelectorEvent event;
                event.EventType = SelectorEventType::Input;
                event.EventPayload.InputEventPayload.InputEventType = SelectorInputEventType::NextSelection;
                eventPump.PostEvent(event);
                break;
            }
            default:
                break;
        }
    }
}
