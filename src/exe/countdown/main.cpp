#include "countdownapp.h"
#include "appconfig.h"

#include <countdown/cli/cliargumentparser.h>
#include <countdown/log/log.h>
#include <countdown/render/window.h>
#include <countdown/render/color.h>
#include <countdown/system/time.h>

using namespace countdown;
using namespace countdown::cli;
using namespace countdown::render;
using namespace countdown::system;

constexpr uint32_t kTicksPerFrame = 1000 / 60; // 60 FPS

int main(int argc, char** argv)
{
    cli::CommandLineArgumentParser argumentParser(argc, argv);
    const cli::CommandLineArgument* pConfigPathArgument = argumentParser.FindArgument("config");
    if (!pConfigPathArgument)
    {
        // TODO: fall back to a reasonable default
        RELEASE_LOG_ERROR(LOG_DEFAULT, "Required argument: --config");
        return -1;
    }

    AppConfig appConfig;
    if (!appConfig.Parse(pConfigPathArgument->AsString()))
    {
        RELEASE_LOG_ERROR(LOG_DEFAULT, "Failed to parse configuration file");
        return -1;
    }

    Window window("shotOS game selection carousel");
    window.SetKeyStateChangedHandler(
        [](Window::EventType eventType, Window::KeyType keyType, void* pUserPointer)
        {
            if (eventType == Window::EventType::KeyDown &&
                keyType == Window::KeyType::Q &&
                pUserPointer)
            {
                Window* pWindow = static_cast<Window*>(pUserPointer);
                pWindow->Quit();
            }
        });
    window.SetUserPointer(&window);

    // TODO: initialize with reference to window after loading all necessary assets
    // - Shaders
    // - Textures
    // - Selectables
    CountdownApp app(&window, appConfig);

    uint32_t lastFrameMs = time::GetTicksMs();
    bool looping = true;
    while (looping)
    {
        Window::EventType eventType = window.PollEvents();
        while (eventType != Window::EventType::None)
        {
            if (eventType == Window::EventType::Quit)
            {
                looping = false;
                break;
            }

            eventType = window.PollEvents();
        }

        const uint32_t nowMs = time::GetTicksMs();
        const uint32_t dtTicks = nowMs - lastFrameMs;
        const float dtSeconds = nowMs / 1000.f;
        app.Tick(dtSeconds);
        lastFrameMs = nowMs;

        if (dtTicks < kTicksPerFrame)
        {
            time::SleepMs(kTicksPerFrame - dtTicks);
        }
        

        window.Clear(ColorRGB::WHITE);
        app.Draw();
        window.Update();
    }
}
