#include "countdownapp.h"

#include <countdown/render/window.h>
#include <countdown/log/check.h>

using namespace countdown;
using namespace countdown::render;

CountdownApp::CountdownApp(Window* pWindow)
    : m_pWindow(pWindow)
{
    RELEASE_CHECK(m_pWindow != nullptr, "Null window pointer in CountdownApp");
}

void CountdownApp::Tick(float dtSeconds)
{
    // TODO
}

void CountdownApp::Draw()
{
    // TODO
}
