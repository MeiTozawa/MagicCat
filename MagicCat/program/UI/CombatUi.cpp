module;

#include <dxe.h>
#include <vector>
#include <memory>
#include <span>

module UiService;

import GameService;
import CardService;
import ServiceLocator;
import AssetService;
import AnimationService;

constexpr int CARD_START_X = 150;
constexpr int CARD_START_Y = 400;
constexpr int PLAYER_START_X = 100;
constexpr int PLAYER_START_Y = 100;
constexpr int OFFSET_X = 250;
constexpr int THICKNESS = 5;
constexpr int RADIUS = 30;

constexpr float SPRITE_OFFSET = 50;
constexpr float SPRITE_SCALE = 0.3f;

constexpr uint32_t COLOR_WHITE = 0xFFFFFF;
constexpr uint32_t COLOR_BLACK = 0;
constexpr uint32_t COLOR_ROCK = 0x555555;
constexpr uint32_t COLOR_PAPER = 0xF5F5DC;
constexpr uint32_t COLOR_SCISSORS = 0xB0C4DE;

class CombatUi : public IUi
{
    Shared<ICardService> cardService;
    Shared<IAssetService> assetService;
    Shared<IAnimationService> animationService;

public:
    CombatUi()
    {
        assetService = ServiceLocator::Get<IAssetService>();
        cardService = ServiceLocator::Get<ICardService>();
        animationService = ServiceLocator::Get<IAnimationService>();
    }

    // FIXME: frequent
    void Draw() override
    {
        cardService->ClearRectOfCards();
        auto hand = cardService->GetHandCards();
        auto position = tnl::Vector2i{CARD_START_X, CARD_START_Y};
        for (int i = 0; i < hand.size(); ++i)
        {
            drawACard(hand[i], position);
            cardService->PushBackRectOfCard({position, {CARD_WIDTH, CARD_HEIGHT}});
            position.x += OFFSET_X;
        }
        
        int handle = assetService->GetSpriteHandle(ESprite::Bunny);
        animationService->Draw(handle, PLAYER_START_X, PLAYER_START_Y);
    }

private:
    void drawACard(Card card, tnl::Vector2i start_position) const
    {
        auto x = start_position.x, y = start_position.y;
        uint32_t color = 0;
        int thickness = THICKNESS;
        if (card.is_selected)
        {
            color = COLOR_WHITE;
            thickness *= 2;
        }
        else
        {
            switch (card.CardType)
            {
            case Rock:
                color = COLOR_ROCK;
                break;
            case Paper:
                color = COLOR_PAPER;
                break;
            case Scissors:
                color = COLOR_SCISSORS;
                break;
            }
        }

        for (int i = 0; i < thickness; ++i)
        {
            float currentX1 = x + i;
            float currentY1 = y + i;
            float currentX2 = x + CARD_WIDTH - i;
            float currentY2 = y + CARD_HEIGHT - i;
            float currentRadius = RADIUS - i;
            if (currentRadius < 0) currentRadius = 0;

            DrawRoundRectAA(currentX1, currentY1,
                            currentX2, currentY2,
                            currentRadius, currentRadius,
                            32, color, FALSE);
        }

        if (const auto icon = assetService->GetImage(static_cast<EImage>(card.CardType)))
        {
            icon->setScaleXY({SPRITE_SCALE, SPRITE_SCALE});
            icon->setPosition({(x + CARD_WIDTH / 2.f), (y + CARD_HEIGHT / 3.5f)});
            icon->draw();
        }

        DrawFormatString(x + CARD_WIDTH / 2 - 20, y + CARD_HEIGHT / 2 + 10, COLOR_WHITE,
                         L"+%d", card.Offset);
    }
};

static struct RegisterCombatUi
{
    RegisterCombatUi()
    {
        UiRegistry::GetRegistrations().push_back([]()
        {
            auto manager = ServiceLocator::Get<IUiService>();
            if (manager)
            {
                manager->RegisterScene(COMBAT, std::make_shared<CombatUi>());
            }
        });
    }
} autoRegister_CombatUi;
