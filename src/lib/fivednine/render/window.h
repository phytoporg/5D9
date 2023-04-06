// window.h
//
// Basic window class, wraps SDL bootstrapping

#pragma once

#include <cstdint>

struct SDL_Window;

namespace fivednine { namespace render {
    struct ColorRGB;

    class Window
    {
    public:
        enum class EventType
        {
            None = 0,
            Invalid,
            KeyDown,
            KeyUp,
            WindowEvent,
            Quit
        };

        // TODO: a real input abstraction layer
        enum class KeyType
        {
            Invalid = 0,
            Left,
            Right,
            A,
            D,
            Q,
            Spacebar
        };

        enum class WindowEvent
        {
            Invalid = 0,
            Shown,
            Hidden,
            Exposed,
            Moved,
            Resized,
            SizeChanged,
            Minimized,
            Maximized,
            Restored,
            Enter,
            Leave,
            FocusGained,
            FocusLost,
            Close,
        };

        static constexpr uint32_t kDefaultWindowWidth = 1024;
        static constexpr uint32_t kDefaultWindowHeight = 768;
        Window(
            const char* pWindowName = nullptr,
            uint32_t width = kDefaultWindowWidth,
            uint32_t height = kDefaultWindowHeight,
            bool fullScreen = false); // Temporarily disabling for development & iteration
        ~Window();

        EventType PollEvents() const;
        void Clear(const ColorRGB& clearColor) const;
        void Update() const;
        void Quit() const;

        void GetWindowDimensions(uint32_t* pWidthOut, uint32_t* pHeightOut) const;

        typedef void(*FnKeyStateChangedHandler)(EventType, KeyType, void*);
        void SetKeyStateChangedHandler(FnKeyStateChangedHandler pfnHandler);

        typedef void(*FnWindowChangedHandler)(WindowEvent, void*);
        void SetWindowStateChangedHandler(FnWindowChangedHandler pfnHandler);

        void SetUserPointer(void* pUserPointer);

    private:
        FnKeyStateChangedHandler m_pfnKeyStateChanged = nullptr;
        FnWindowChangedHandler   m_pfnWindowChanged = nullptr;
        void*                    m_pUserPointer = nullptr;

        SDL_Window* m_pWindow = nullptr;
    };
}}
