// Basic window class, wraps SDL bootstrapping

#pragma once

#include <cstdint>

// Forward declarations
struct SDL_Window;

namespace countdown { namespace render {
    class Window
    {
    public:
        enum class EventType
        {
            Invalid = 0,
            Quit
        };

        static constexpr uint32_t DEFAULT_WINDOW_WIDTH = 1024;
        static constexpr uint32_t DEFAULT_WINDDOW_HEIGHT = 768;
        Window(
            const char* pWindowName = nullptr,
            uint32_t width = DEFAULT_WINDOW_WIDTH,
            uint32_t height = DEFAULT_WINDDOW_HEIGHT);
        ~Window();

        EventType PollEvents() const;

    private:
        SDL_Window* m_pWindow = nullptr;
    };
}}
