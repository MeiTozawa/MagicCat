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

constexpr int CARD_START_X = 400;
constexpr int CARD_START_Y = 750;
constexpr int DRAW_PILE_X = 50;
constexpr int DRAW_PILE_Y = 400;
constexpr int DISCARD_PILE_X = 50;
constexpr int DISCARD_PILE_Y = 750;

constexpr int PLAYER_START_X = 300;
constexpr int PLAYER_START_Y = 450;
constexpr int OFFSET_X = 250;
constexpr int THICKNESS = 5;
constexpr int RADIUS = 30;

constexpr float SPRITE_OFFSET = 50;
constexpr float SPRITE_SCALE = 0.3f;

constexpr uint32_t COLOR_WHITE = 0xFFFFFF;
constexpr uint32_t COLOR_BLACK = 0;
constexpr uint32_t COLOR_DEFAULT = 0x79D5EE;
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
        std::wstring message = std::format(L"山札\n{:2}枚", cardService->GetDrawCards().size());
        drawACard({Null}, {DRAW_PILE_X, DRAW_PILE_Y}, message.c_str(), false);
        message = std::format(L"捨札\n{:2}枚", cardService->GetDiscardCards().size());
        drawACard({Null}, {DISCARD_PILE_X, DISCARD_PILE_Y}, message.c_str(), false);

        cardService->ClearRectOfCards();
        auto hand = cardService->GetHandCards();
        auto position = tnl::Vector2i{CARD_START_X, CARD_START_Y};
        for (int i = 0; i < hand.size(); ++i)
        {
            message = std::format(L"+{}", hand[i].Offset);
            drawACard(hand[i], position, message.c_str());
            cardService->PushBackRectOfCard({position, {CARD_WIDTH, CARD_HEIGHT}});
            position.x += OFFSET_X;
        }

        int handle = assetService->GetSpriteHandle(ESprite::Bunny);
        animationService->Draw(handle, PLAYER_START_X, PLAYER_START_Y);
    }

private:
    void drawACard(const Card& card, tnl::Vector2i start_position, const wchar_t* message, bool has_icon = true) const
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
            default:
                color = COLOR_DEFAULT;
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

        int lineCount = 1;
        if (has_icon)
        {
            if (const auto icon = assetService->GetImage(static_cast<EImage>(card.CardType)))
            {
                icon->setScaleXY({SPRITE_SCALE, SPRITE_SCALE});
                icon->setPosition({(x + CARD_WIDTH / 2.f), (y + CARD_HEIGHT / 3.5f)});
                icon->draw();
            }

            drawCenterText(x + CARD_WIDTH / 2, y + CARD_HEIGHT / 2 + 10, lineCount, message, color);
        }
        else
        {
            lineCount += 1;
            drawCenterText(x + CARD_WIDTH / 2, y + CARD_HEIGHT / 2, lineCount, message, color);
        }
    }

    static void drawCenterText(int middle_x, int middle_y, int lineCount, const wchar_t* message, int color = COLOR_WHITE)
    {
        auto textSize = GetFontSize();
        auto textWidth = GetDrawStringSize(&textSize, &textSize, &lineCount, message, -1);
        DrawString(middle_x - textWidth / 2, middle_y - textSize / 2, message, color);
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
