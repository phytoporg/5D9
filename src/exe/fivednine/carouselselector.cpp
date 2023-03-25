#include "carouselselector.h"
#include "fivednineapp.h"
#include <fivednine/log/check.h>

CarouselSelector::CarouselSelector(fivednineApp* pApp, EventPump* pEventPump)
    : m_pApp(pApp), m_pEventPump(pEventPump)
{
    RELEASE_CHECK(m_pApp, "App pointer cannot be null in carousel selector");
    RELEASE_CHECK(m_pEventPump, "Event pump pointer cannot be null in carousel selector");
}

float Tinted = 0.3f;
float UnTinted = 1.0f;

bool CarouselSelector::Initialize()
{
    const float kGameCardPaddingX = 20.f;

    const uint32_t kInitialCardWidth = 200;
    const uint32_t kInitialCardHeight = 360;

    uint32_t width;
    uint32_t height;
    m_pApp->Selector_GetDisplayDimensions(&width, &height);

    const uint32_t NumCards = m_pApp->Selector_GetNumCards();
    const uint32_t MiddleCardIndex = NumCards / 2;

    // TODO: Align the center card with (0,0)
    const float kInitialCardX = MiddleCardIndex * -1.f * (kInitialCardWidth + kGameCardPaddingX);
    const float cardY = kInitialCardHeight / -2.f;
    const float cardZ = 0.f;

    for (uint32_t i = 0; i < NumCards; ++i)
    {
        const float cardX = kInitialCardX + static_cast<float>(i * (kInitialCardWidth + kGameCardPaddingX));
        m_pApp->Selector_SetCardPosition(i, cardX, cardY, cardZ);
        m_pApp->Selector_SetCardDimensions(i, kInitialCardWidth, kInitialCardHeight);

        // TODO: SetCardAppearanceParam shouldn't require a pointer argument for the uniform value
        float& TintValue = (i == MiddleCardIndex) ? UnTinted : Tinted;
        m_pApp->Selector_SetCardAppearanceParam1f(i, "tint", &TintValue);

        GameInfo gameInfo;
        if (!m_pApp->Selector_GetCardGameInfo(i, &gameInfo))
        {
            continue;
        }
        
        // We're using 600x900 textures for the carousel
        const std::string CardTexture = gameInfo.TexturePrefix + "_600x900";
        m_pApp->Selector_SetCardTexture(i, CardTexture.c_str()); // TODO: per-game texture
    }

    m_pApp->Selector_SelectIndex(MiddleCardIndex);
    return true;
}

void CarouselSelector::Tick(float dtSeconds)
{
    // TODO
}