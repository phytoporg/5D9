#include "window.h"
#include "color.h"
#include "rendercommon.h"

#include <common/log/log.h>
#include <common/log/check.h>

#include <SDL.h>

using namespace fivednine;
using namespace fivednine::render;

namespace 
{
    Window::EventType SDLTofivednineEvent(Uint32 sdlType)
    {
        switch (sdlType)
        {
            case SDL_QUIT:
                return Window::EventType::Quit;
            case SDL_KEYDOWN:
                return Window::EventType::KeyDown;
            case SDL_KEYUP:
                return Window::EventType::KeyUp;
            case SDL_WINDOWEVENT:
                return Window::EventType::WindowEvent;
            default:
                return Window::EventType::Invalid;
        }
    }

    Window::KeyType SDLKeySymToKeyType(SDL_Keycode sdlKeyType)
    {
        switch (sdlKeyType)
        {
            case SDLK_a:
                return Window::KeyType::A;
            case SDLK_d:
                return Window::KeyType::D;
            case SDLK_q:
                return Window::KeyType::Q;
            case SDLK_LEFT:
                return Window::KeyType::Left;
            case SDLK_RIGHT:
                return Window::KeyType::Right;
            case SDLK_SPACE:
                return Window::KeyType::Spacebar;
            default:
                return Window::KeyType::Invalid;
        }
    }

    Window::WindowEvent SDLWindowEventToWindowEvent(Uint8 eventId)
    {
        switch (eventId)
        {
            case SDL_WINDOWEVENT_SHOWN:
                return Window::WindowEvent::Shown;
            case SDL_WINDOWEVENT_HIDDEN:
                return Window::WindowEvent::Hidden;
            case SDL_WINDOWEVENT_EXPOSED:
                return Window::WindowEvent::Exposed;
            case SDL_WINDOWEVENT_MOVED:
                return Window::WindowEvent::Moved;
            case SDL_WINDOWEVENT_RESIZED:
                return Window::WindowEvent::Resized;
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                return Window::WindowEvent::SizeChanged;
            case SDL_WINDOWEVENT_MINIMIZED:
                return Window::WindowEvent::Minimized;
            case SDL_WINDOWEVENT_MAXIMIZED:
                return Window::WindowEvent::Maximized;
            case SDL_WINDOWEVENT_RESTORED:
                return Window::WindowEvent::Restored;
            case SDL_WINDOWEVENT_ENTER:
                return Window::WindowEvent::Enter;
            case SDL_WINDOWEVENT_LEAVE:
                return Window::WindowEvent::Leave;
            case SDL_WINDOWEVENT_FOCUS_GAINED:
                return Window::WindowEvent::FocusGained;
            case SDL_WINDOWEVENT_FOCUS_LOST:
                return Window::WindowEvent::FocusLost;
            case SDL_WINDOWEVENT_CLOSE:
                return Window::WindowEvent::Close;
            default:
                return Window::WindowEvent::Invalid;
        }
    }
}

Window::Window(const char* pWindowName, uint32_t width, uint32_t height, bool fullScreen)
{
    RELEASE_CHECK(pWindowName != nullptr, "Window requires a name");

    SDL_ShowCursor(SDL_DISABLE);

    Uint32 CreateWindowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL;
    if (fullScreen)
    {
        CreateWindowFlags |= SDL_WINDOW_FULLSCREEN;
    }
    m_pWindow = SDL_CreateWindow(
            pWindowName,
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            width,
            height,
            CreateWindowFlags);
    RELEASE_CHECK(
        m_pWindow != nullptr,
        "Failed to create SDL window %s: %s", pWindowName, SDL_GetError());

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GLContext context = SDL_GL_CreateContext(m_pWindow);
    if (SDL_GL_MakeCurrent(m_pWindow, context) < 0)
    {
        RELEASE_LOG_FATAL(LOG_RENDER, "Failed to set current GL context: %s", SDL_GetError());
    }

    if (SDL_GL_SetSwapInterval(1) < 0)
    {
        RELEASE_LOG_FATAL(LOG_RENDER, "Failed to set swap interval: %s", SDL_GetError());
    }

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        RELEASE_LOG_FATAL(LOG_RENDER, "Failed to initialize GLEW");
    }

    glViewport(0.f, 0.f, width, height);
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
    while (SDL_PollEvent(&e))
    {
        const Window::EventType EventType = SDLTofivednineEvent(e.type);
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
                break;
            case Window::EventType::WindowEvent:
                if (m_pfnWindowChanged)
                {
                    const Window::WindowEvent windowEvent =
                        SDLWindowEventToWindowEvent(e.window.event);
                    m_pfnWindowChanged(windowEvent, m_pUserPointer);
                }
                break;
            default:
                break;
        }

        return EventType;
    }

    return Window::EventType::None;
}

void Window::Clear(const ColorRGB& clearColor) const
{
    glClearColor(clearColor.R / 255.0f, clearColor.G / 255.0f, clearColor.B / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Window::Update() const
{
    SDL_GL_SwapWindow(m_pWindow);
}

void Window::Quit() const
{
    SDL_Event quitEvent;
    quitEvent.type = SDL_QUIT;
    SDL_PushEvent(&quitEvent);
}

void Window::GetWindowDimensions(uint32_t* pWidthOut, uint32_t* pHeightOut) const
{
    int width, height;
    SDL_GetWindowSize(m_pWindow, &width, &height);

    *pWidthOut = static_cast<uint32_t>(width);
    *pHeightOut = static_cast<uint32_t>(height);
}

void Window::SetKeyStateChangedHandler(FnKeyStateChangedHandler pfnHandler)
{
    m_pfnKeyStateChanged = pfnHandler;
}

void Window::SetWindowStateChangedHandler(FnWindowChangedHandler pfnHandler)
{
    m_pfnWindowChanged = pfnHandler;
}

void Window::SetUserPointer(void* pUserPointer)
{
    m_pUserPointer = pUserPointer;
}
