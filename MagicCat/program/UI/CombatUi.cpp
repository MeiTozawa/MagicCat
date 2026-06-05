module;

#include <dxe.h>
#include <vector>
#include <memory>
#include <span>

module UiService;

import GameService;
import CardService;
import ServiceLocator;

constexpr float ICON_SCALE = 0.3f;
constexpr int CARD_START_X = 200;
constexpr int CARD_START_Y = 400;
constexpr int OFFSET_X = 250;
constexpr int THICKNESS = 5;
constexpr int RADIUS = 5;

class CombatUi : public IUi
{
private:
    Shared<ICardService> cardService;

public:
    CombatUi()
    {
        loadAssets();
        cardService = ServiceLocator::Get<ICardService>();
    }

    // FIXME: frequent
    void Draw() override
    {
        cardService->ClearRectOfCards();
        auto hand = cardService->GetHandCards();
        auto position = tnl::Vector2i{CARD_START_X, CARD_START_Y};
        for (int i = 0; i < hand.size(); ++i)
        {
            drawACard(hand[i], position, true);
            cardService->PushBackRectOfCard({position, {CARD_WIDTH, CARD_HEIGHT}});
            position.x += OFFSET_X;
        }
    }

private:
    std::vector<Shared<dxe::Sprite>> spriteMappings{nullptr, nullptr, nullptr};


    void drawACard(Card card, tnl::Vector2i start_position, bool is_selected = false) const
    {
        auto x = start_position.x, y = start_position.y;
        uint16_t color = 0;
        if (is_selected)
        {
            color = COLOR_WHITE;
        }
        else
        {
            switch (card.CardType)
            {
            case ROCK:
                color = COLOR_ROCK;
                break;
            case PAPER:
                color = COLOR_PAPER;
                break;
            case SCISSORS:
                color = COLOR_SCISSORS;
                break;
            }
        }

        for (int i = 0; i < THICKNESS; ++i) {
            float currentX1 = x + i;
            float currentY1 = y + i;
            float currentX2 = x + CARD_WIDTH - i;
            float currentY2 = y + CARD_HEIGHT - i;
            float currentRadius = RADIUS - i; 
            if (currentRadius < 0) currentRadius = 0;

            DrawRoundRectAA(currentX1, currentY1, currentX2, currentY2, currentRadius, currentRadius, 32, color, FALSE);
        }

        if (auto icon = spriteMappings[card.CardType])
        {
            icon->setScaleXY({ICON_SCALE, ICON_SCALE});
            icon->setPosition({(x + CARD_WIDTH / 2.5f), (y + CARD_HEIGHT / 3.5f)});
            icon->draw();
        }

        DrawFormatString(x + 10, y + CARD_HEIGHT / 2 + 10, COLOR_BLACK,
                         L"確率を%d上げる", card.Offset);
    }

    void loadAssets()
    {
        try
        {
            auto rock_resource = dxe::SpriteResouce::Create(L"resource/graphics/example/stone.png");
            if (!rock_resource)
                printfDx(L"石の画像の読み込みに失敗");
            else
                spriteMappings[ROCK] = dxe::Sprite::Create(rock_resource);

            auto scissors_resource = dxe::SpriteResouce::Create(L"resource/graphics/example/scissors.png");
            if (!scissors_resource)
                printfDx(L"ハサミの画像の読み込みに失敗");
            else
                spriteMappings[SCISSORS] = dxe::Sprite::Create(scissors_resource);

            auto paper_resource = dxe::SpriteResouce::Create(L"resource/graphics/example/paper.png");
            if (!paper_resource)
                printfDx(L"紙の画像の読み込みに失敗");
            else
                spriteMappings[PAPER] = dxe::Sprite::Create(paper_resource);
        }
        catch (const std::exception&)
        {
            printfDx(L"画像の読み込みに失敗");
        }
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
