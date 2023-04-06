#include "carouselselector.h"
#include "fivednineapp.h"
#include <common/log/check.h>

CarouselSelector::CarouselSelector(fivednineApp* pApp, EventPump* pEventPump)
    : m_pApp(pApp), m_pEventPump(pEventPump)
{
    RELEASE_CHECK(m_pApp, "App pointer cannot be null in carousel selector");
    RELEASE_CHECK(m_pEventPump, "Event pump pointer cannot be null in carousel selector");
}

float Tinted = 0.3f;
float UnTinted = 1.0f;

static const float kGameCardPaddingX = 20.f;
static const uint32_t kInitialCardWidth = 200;
static const uint32_t kInitialCardHeight = 360;

bool CarouselSelector::Initialize()
{
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
        m_pApp->Selector_SetCardTexture(i, CardTexture.c_str());
    }

    m_pApp->Selector_SelectIndex(MiddleCardIndex);
    SnapCameraToCard(MiddleCardIndex);
    return true;
}

void CarouselSelector::Tick(float dtSeconds)
{
    SelectorEvent event;
    while (m_pEventPump->GetNextEvent(&event))
    {
        switch(event.EventType)
        {
            case SelectorEventType::Input:
                HandleInputEvent(event.EventPayload.InputEventPayload);
                break;
            default:
                break;
        }
    }
}

void CarouselSelector::HandleInputEvent(const SelectorInputEventPayload& inputEventPayload)
{
    switch(inputEventPayload.InputEventType)
    {
        case SelectorInputEventType::NextSelection:
        {
            const uint32_t NumCards = m_pApp->Selector_GetNumCards();
            const uint32_t CurrentCardIndex = m_pApp->Selector_GetSelectedIndex();

            if (CurrentCardIndex < (NumCards - 1))
            {
                const uint32_t NewSelectedIndex = CurrentCardIndex + 1;
                m_pApp->Selector_SelectIndex(NewSelectedIndex);
                m_pApp->Selector_SetCardAppearanceParam1f(CurrentCardIndex, "tint", &Tinted);
                m_pApp->Selector_SetCardAppearanceParam1f(NewSelectedIndex, "tint", &UnTinted);

                MoveCameraToCard(NewSelectedIndex);
            }
            break;
        }
        case SelectorInputEventType::PreviousSelection:
        {
            const uint32_t CurrentCardIndex = m_pApp->Selector_GetSelectedIndex();

            if (CurrentCardIndex > 0)
            {
                const uint32_t NewSelectedIndex = CurrentCardIndex - 1;
                m_pApp->Selector_SelectIndex(NewSelectedIndex);
                m_pApp->Selector_SetCardAppearanceParam1f(CurrentCardIndex, "tint", &Tinted);
                m_pApp->Selector_SetCardAppearanceParam1f(NewSelectedIndex, "tint", &UnTinted);

                MoveCameraToCard(NewSelectedIndex);
            }
            break;
        }
        case SelectorInputEventType::ConfirmCurrent:
        {
            m_pApp->Selector_ConfirmCurrentSelection();
            break;
        }
        default:
            break;
    }
}

void CarouselSelector::MoveCameraToCard(uint32_t cardIndex)
{
    glm::vec3 newSelectedCardPosition;
    if (m_pApp->Selector_GetCardPosition(cardIndex, &newSelectedCardPosition))
    {
        uint32_t width;
        uint32_t height;
        m_pApp->Selector_GetDisplayDimensions(&width, &height);

        // TODO: account for current camera position
        newSelectedCardPosition[0] += kInitialCardWidth / 2.f - static_cast<float>(width) / 2.f;
        newSelectedCardPosition[1] += kInitialCardHeight / 2.f - static_cast<float>(height) / 2.f;
        newSelectedCardPosition[2] = 1.f;
        m_pApp->Selector_SetCameraTarget(newSelectedCardPosition);
    }
}

void CarouselSelector::SnapCameraToCard(uint32_t cardIndex)
{
    glm::vec3 newSelectedCardPosition;
    if (m_pApp->Selector_GetCardPosition(cardIndex, &newSelectedCardPosition))
    {
        uint32_t width;
        uint32_t height;
        m_pApp->Selector_GetDisplayDimensions(&width, &height);

        // TODO: account for current camera position
        newSelectedCardPosition[0] += kInitialCardWidth / 2.f - static_cast<float>(width) / 2.f;
        newSelectedCardPosition[1] += kInitialCardHeight / 2.f - static_cast<float>(height) / 2.f;
        newSelectedCardPosition[2] = 1.f;
        m_pApp->Selector_SetCameraPosition(newSelectedCardPosition);
    }
}
