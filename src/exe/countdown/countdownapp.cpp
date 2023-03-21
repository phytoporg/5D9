#include "countdownapp.h"
#include "appconfig.h"

#include <countdown/render/window.h>
#include <countdown/log/check.h>

using namespace countdown;
using namespace countdown::render;

CountdownApp::CountdownApp(Window* pWindow, const AppConfig& configuration)
    : m_pWindow(pWindow)
{
    RELEASE_CHECK(m_pWindow != nullptr, "Null window pointer in CountdownApp");

    // TODO: process configuration
}

bool CountdownApp::Initialize()
{
    // TODO
    return false;
}

void CountdownApp::Tick(float dtSeconds)
{
    RELEASE_CHECK(m_isInitialized, "Attempting to tick app without having initialized");
    // TODO
}

void CountdownApp::Draw()
{
    RELEASE_CHECK(m_isInitialized, "Attempting to draw app without having initialized");
    // TODO
}