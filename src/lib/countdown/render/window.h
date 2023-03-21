// Basic window class, wraps SDL bootstrapping

#pragma once

#include <cstdint>

// Forward declarations
struct SDL_Window;

namespace countdown { namespace render {
    struct ColorRGB;

    class Window
    {
    public:
        enum class EventType
        {
            Invalid = 0,
            KeyDown,
            KeyUp,
            Quit
        };

        // TODO: a real input abstraction layer
        enum class KeyType
        {
            Invalid = 0,
            Q
        };

        static constexpr uint32_t DEFAULT_WINDOW_WIDTH = 1024;
        static constexpr uint32_t DEFAULT_WINDDOW_HEIGHT = 768;
        Window(
            const char* pWindowName = nullptr,
            uint32_t width = DEFAULT_WINDOW_WIDTH,
            uint32_t height = DEFAULT_WINDDOW_HEIGHT);
        ~Window();

        EventType PollEvents() const;
        void Clear(const ColorRGB& clearColor) const;
        void Update() const;
        void Quit() const;

        typedef void(*FnKeyStateChangedHandler)(EventType, KeyType, void*);
        void SetKeyStateChangedHandler(FnKeyStateChangedHandler pfnHandler);

        void SetUserPointer(void* pUserPointer);

    private:
        FnKeyStateChangedHandler m_pfnKeyStateChanged = nullptr;
        void*                    m_pUserPointer = nullptr;

        SDL_Window* m_pWindow = nullptr;
    };
}}
