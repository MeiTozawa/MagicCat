module;

export module CardView;

import CardService;
import ServiceLocator;
import AssetService;
import EventBus;

// card
constexpr int CARD_START_X = 400;
constexpr int CARD_START_Y = 750;
constexpr int DRAW_PILE_X = 50;
constexpr int DRAW_PILE_Y = 400;
constexpr int DISCARD_PILE_X = 50;
constexpr int DISCARD_PILE_Y = 750;
constexpr int OFFSET_X = 250;

// thickness
constexpr int THICKNESS = 5;
constexpr int RADIUS = 30;

// color
constexpr uint32_t COLOR_WHITE = 0xFFFFFF;
constexpr uint32_t COLOR_BLACK = 0;
constexpr uint32_t COLOR_DEFAULT = 0x79D5EE;
constexpr uint32_t COLOR_ROCK = 0x555555;
constexpr uint32_t COLOR_PAPER = 0xF5F5DC;
constexpr uint32_t COLOR_SCISSORS = 0xB0C4DE;

// image
constexpr float IMAGE_OFFSET = 50;
constexpr float IMAGE_SCALE = 0.3f;

export class CardView
{
    Shared<ICardService> cardService;
    Shared<IAssetService> assetService;

    EventHandle handUpdateHandle;
    std::vector<Card> cachedHand;

public:
    CardView()
    {
        cardService = ServiceLocator::Get<ICardService>();
        assetService = ServiceLocator::Get<IAssetService>();

        handUpdateHandle = EventBus::Subscribe<DrawCardEvent>([this](const DrawCardEvent& e) {
            cachedHand = cardService->GetHandCards();
        });
    }

    ~CardView()
    {
        EventBus::Unsubscribe(handUpdateHandle);
    }

    void PrintCards() const
    {
        std::wstring message;
        cardService->ClearRectOfCards();
        const auto& hand = cachedHand;
        auto position = tnl::Vector2i{CARD_START_X, CARD_START_Y};
        for (int i = 0; i < hand.size(); ++i)
        {
            message = std::format(L"+{}", hand[i].Offset);
            printACard(hand[i], position, message.c_str());
            cardService->PushBackRectOfCard({position, {CARD_WIDTH, CARD_HEIGHT}});
            position.x += OFFSET_X;
        }
    }
    
    void PrintDrawPile() const
    {
        std::wstring message = std::format(L"山札\n{:2}枚", cardService->GetDrawCards().size());
        printACard({Null}, {DRAW_PILE_X, DRAW_PILE_Y}, message.c_str(), false);
    }
    
    void PrintDiscardPile() const
    {
        std::wstring message = std::format(L"捨札\n{:2}枚", cardService->GetDiscardCards().size());
        printACard({Null}, {DISCARD_PILE_X, DISCARD_PILE_Y}, message.c_str(), false);
    }

private:
    static void drawCenterText(int middle_x, int middle_y, int lineCount, const wchar_t* message,
                               int color = COLOR_WHITE)
    {
        auto textSize = GetFontSize();
        auto textWidth = GetDrawStringSize(&textSize, &textSize, &lineCount, message, -1);
        DrawString(middle_x - textWidth / 2, middle_y - textSize / 2, message, color);
    }

    void printACard(const Card& card, tnl::Vector2i start_position, const wchar_t* message, bool has_icon = true) const
    {
        auto x = start_position.x, y = start_position.y;
        uint32_t color;
        int thickness = THICKNESS;

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
                icon->setScaleXY({IMAGE_SCALE, IMAGE_SCALE});
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
};
