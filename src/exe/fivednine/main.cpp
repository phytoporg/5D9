#include "fivednineapp.h"
#include "appconfig.h"

#include <common/cli/cliargumentparser.h>
#include <common/log/log.h>

#include <fivednine/render/window.h>
#include <fivednine/render/color.h>
#include <fivednine/system/time.h>

using namespace common;
using namespace common::cli;

using namespace fivednine;
using namespace fivednine::render;
using namespace fivednine::system;

constexpr uint32_t kTicksPerFrame = 1000 / 60; // 60 FPS

int main(int argc, char** argv)
{
    // Initialize logging
    log::SetLogVerbosity(log::LogVerbosity::Warning);

    log::RegisterLogZone(LOG_DEFAULT, true, "Default");
    log::RegisterLogZone(LOG_RENDER, true, "Render");
    log::RegisterLogZone(LOG_API, true, "API");

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
    fivednineApp app;
    if (!app.Initialize(appConfig, &window))
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
