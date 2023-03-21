#include <countdown/render/window.h>

using namespace countdown;
using namespace countdown::render;

int main(int argc, char** argv)
{
    Window window("shotOS game selection carousel");

    while (window.PollEvents() != Window::EventType::Quit)
    {
        // Do something interesting?
    }
}
