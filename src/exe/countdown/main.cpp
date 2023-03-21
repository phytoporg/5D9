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

    while (true)
    {
        const Window::EventType EventType = window.PollEvents();
        if (EventType == Window::EventType::Quit)
        {
            break;
        }

        window.Clear(ColorRGB::WHITE);
        window.Update();
    }
}
