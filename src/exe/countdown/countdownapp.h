#pragma once

#include <cstdint>

namespace countdown 
{
    namespace render 
    {
        class Window;
    }
}

class CountdownApp
{
    public:
        CountdownApp(countdown::render::Window* pWindow);

        void Tick(float dtSeconds);
        void Draw();

    private:
        countdown::render::Window* m_pWindow = nullptr;
};