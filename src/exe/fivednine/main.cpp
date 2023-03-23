#include "fivednineapp.h"
#include "appconfig.h"

#include <fivednine/cli/cliargumentparser.h>
#include <fivednine/log/log.h>
#include <fivednine/render/window.h>
#include <fivednine/render/color.h>
#include <fivednine/system/time.h>

using namespace fivednine;
using namespace fivednine::cli;
using namespace fivednine::render;
using namespace fivednine::system;

constexpr uint32_t kTicksPerFrame = 1000 / 60; // 60 FPS

int main(int argc, char** argv)
{
    // Initialize logging
    log::SetLogVerbosity(log::LogVerbosity::Warning);
    log::EnableZone(LOG_DEFAULT);
    log::EnableZone(LOG_RENDER);
    log::EnableZone(LOG_API);

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

    // boo, gross
    fivednineApp app;
    app.SetWindow(&window);
    if (!app.Initialize(appConfig))
    {
        RELEASE_LOG_ERROR(LOG_DEFAULT, "Failed to initialize app");
        return -1;
    }

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

        window.Clear(ColorRGB::BLACK);
        app.Draw();
        window.Update();
    }
}
