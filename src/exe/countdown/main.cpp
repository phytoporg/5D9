#include <countdown/render/window.h>
#include <countdown/render/color.h>

using namespace countdown;
using namespace countdown::render;

int main(int argc, char** argv)
{
    Window window("shotOS game selection carousel");

    while (window.PollEvents() != Window::EventType::Quit)
    {
        window.Clear(ColorRGB::WHITE);
        window.Update();
    }
}
