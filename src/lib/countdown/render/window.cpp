#include "window.h"
#include "color.h"
#include <countdown/log/log.h>
#include <countdown/log/check.h>

#include <SDL.h>

using namespace countdown;
using namespace countdown::render;

namespace 
{
    Window::EventType SDLToCountdownEvent(Uint32 sdlType)
    {
        switch (sdlType)
        {
            case SDL_QUIT:
                return Window::EventType::Quit;
            case SDL_KEYDOWN:
                return Window::EventType::KeyDown;
            case SDL_KEYUP:
                return Window::EventType::KeyUp;
            default:
                return Window::EventType::Invalid;
        }
    }

    Window::KeyType SDLKeySymToKeyType(SDL_Keycode sdlKeyType)
    {
        switch (sdlKeyType)
        {
            case SDLK_q:
                return Window::KeyType::Q;
            default:
                return Window::KeyType::Invalid;
        }
    }
}

Window::Window(const char* pWindowName, uint32_t width, uint32_t height)
{
    RELEASE_CHECK(pWindowName != nullptr, "Window requires a name");

    if (!SDL_WasInit(SDL_INIT_VIDEO))
    {
        // Initialize video subsystem if it's not already initialized
        if (SDL_Init(SDL_INIT_VIDEO) < 0)
        {
            RELEASE_LOGLINE_FATAL(
                LOG_RENDER,
                "Window %s failed to initialize SDL video subsystem",
                pWindowName);
        }
    }

    m_pWindow = SDL_CreateWindow(
            pWindowName,
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            width,
            height,
            SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
    RELEASE_CHECK(
        m_pWindow != nullptr,
        "Failed to create SDL window %s: %s", pWindowName, SDL_GetError());
}

Window::~Window()
{
    SDL_DestroyWindow(m_pWindow);
    m_pWindow = nullptr;

    // TODO: Does this really belong here?
    SDL_Quit();
}

Window::EventType Window::PollEvents() const
{
    SDL_Event e;
    if (SDL_PollEvent(&e))
    {
        const Window::EventType EventType = SDLToCountdownEvent(e.type);
        switch (EventType)
        {
            case Window::EventType::KeyUp:
                // Fallthrough
            case Window::EventType::KeyDown:
                if (m_pfnKeyStateChanged)
                {
                    m_pfnKeyStateChanged(
                        EventType,
                        SDLKeySymToKeyType(e.key.keysym.sym),
                        m_pUserPointer);
                }
        }

        return EventType;
    }

    return Window::EventType::Invalid;
}

void Window::Clear(const ColorRGB& clearColor) const
{
    SDL_Surface* pScreenSurface = SDL_GetWindowSurface(m_pWindow);
    SDL_FillRect(
        pScreenSurface,
        nullptr,
        SDL_MapRGB(pScreenSurface->format, clearColor.R, clearColor.G, clearColor.B));
}

void Window::Update() const
{
    SDL_UpdateWindowSurface(m_pWindow);
}

void Window::Quit() const
{
    SDL_Event quitEvent;
    quitEvent.type = SDL_QUIT;
    SDL_PushEvent(&quitEvent);
}

void Window::SetKeyStateChangedHandler(FnKeyStateChangedHandler pfnHandler)
{
    m_pfnKeyStateChanged = pfnHandler;
}

void Window::SetUserPointer(void* pUserPointer)
{
    m_pUserPointer = pUserPointer;
}
