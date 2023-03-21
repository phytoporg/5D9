#include "countdownapp.h"
#include <countdown/render/window.h>
#include <countdown/render/color.h>

using namespace countdown;
using namespace countdown::render;

int main(int argc, char** argv)
{
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

    CountdownApp app(&window);

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

        // TODO: proper outer loop with dt bookkeeping
        const float dtSeconds = 0.f;
        app.Tick(dtSeconds);

        window.Clear(ColorRGB::WHITE);
        app.Draw();
        window.Update();
    }
}
