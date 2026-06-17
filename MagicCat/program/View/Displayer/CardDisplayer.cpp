module;

#include <dxe.h>
#include <memory>
#include <vector>
#include <string>
#include <format>
#include <DrawStringUtils.h>

module Displayer;

import CardService;
import ServiceLocator;
import AssetService;
import EventBus;
import EffectorFactory;

namespace mc
{
    /// かつて、constexprには内部リンク性があり、これは暗黙的にstaticが追加されたのと同じ効果を持っていました。
    /// しかし、C++20のモジュールスコープにおいて、C++20委員会はconstexprの暗黙的な内部リンク性を廃止し、モジュールリンク性を付与しました。
    /// これにより、constexprで定義された定数は現在のモジュール内のすべてのファイルで共有されるようになり、その結果、再定義が可能になりました。
    namespace
    {
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

        // image
        constexpr float IMAGE_SCALE = 0.3f;
    }

    class CardDisplayer : public IDisplayer
    {
        ICardService* cardService;
        IAssetService* assetService;

        EventHandle handUpdateHandle;
        std::vector<Card> cachedHand;
        std::vector<std::unique_ptr<IDisplayer>> cardDisplayers;

        class PrintACardDisplayer : public IDisplayer {
        public:
            Card card; tnl::Vector2i start_position; std::wstring message; bool has_icon;
            PrintACardDisplayer(Card card, tnl::Vector2i start_position, std::wstring message, bool has_icon = true) :
                card(card), start_position(start_position), message(std::move(message)), has_icon(has_icon) {}

            void Update(float deltaTime) override {}
            void Draw(float deltaTime) const override {
                auto x = start_position.x, y = start_position.y;
                uint32_t color;
                int thickness = THICKNESS;

                switch (card.CardType)
                {
                case Rock:
                    color = COLOR_CARD_ROCK;
                    break;
                case Paper:
                    color = COLOR_CARD_PAPER;
                    break;
                case Scissors:
                    color = COLOR_CARD_SCISSORS;
                    break;
                default:
                    color = COLOR_CARD_DEFAULT;
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
                    int icon = ServiceLocator::Get<IAssetService>()->GetImageHandle(static_cast<EImage>(card.CardType));
                    if (icon != -1)
                    {
                        DrawRotaGraphF(x + CARD_WIDTH / 2.f, y + CARD_HEIGHT / 3.5f, IMAGE_SCALE, 0.0, icon, TRUE);
                    }

                    drawCenterText(x + CARD_WIDTH / 2, y + CARD_HEIGHT / 2 + 10, lineCount, message.c_str(), color);
                }
                else
                {
                    lineCount += 1;
                    drawCenterText(x + CARD_WIDTH / 2, y + CARD_HEIGHT / 2, lineCount, message.c_str(), color);
                }
            }
        };

        void RebuildDisplayers(bool isDraw = false)
        {
            cardDisplayers.clear();
            auto position = tnl::Vector2i{CARD_START_X, CARD_START_Y};
            for (size_t i = 0; i < cachedHand.size(); ++i)
            {
                std::wstring msg = std::format(L"+{}", cachedHand[i].Value);
                auto cardDisp = std::make_unique<PrintACardDisplayer>(cachedHand[i], position, msg);
                
                // If this is the newest card, wrap it in HitFlashEffector
                if (isDraw && i == cachedHand.size() - 1) {
                    auto flashEffector = CreateHitFlashEffector(std::move(cardDisp), 0x000000, 300);
                    flashEffector->Play();
                    cardDisplayers.push_back(std::move(flashEffector));
                } else {
                    cardDisplayers.push_back(std::move(cardDisp));
                }
                
                position.x += OFFSET_X;
            }
        }

    public:
        CardDisplayer()
        {
            cardService = ServiceLocator::Get<ICardService>();
            assetService = ServiceLocator::Get<IAssetService>();

            cachedHand = cardService->GetHandCards();
            RebuildDisplayers(false);

            handUpdateHandle = EventBus::Subscribe<HandUpdatedEvent>([this](const HandUpdatedEvent& e)
            {
                bool isDraw = e.cards.size() > cachedHand.size();
                cachedHand = e.cards;
                RebuildDisplayers(isDraw);
            });
        }

        ~CardDisplayer() override
        {
            EventBus::Unsubscribe(handUpdateHandle);
        }

        void Update(float deltaTime) override 
        {
            for (auto& display : cardDisplayers)
            {
                display->Update(deltaTime);
            }
        }

        void Draw(float deltaTime) const override
        {
            InitDrawPile(deltaTime);
            InitDiscardPile(deltaTime);
            for (auto& display : cardDisplayers)
            {
                display->Draw(deltaTime);
            }
        }

    private:
        void InitDrawPile(float deltaTime) const
        {
            std::wstring message = std::format(L"山札\n{:2}枚", cardService->GetDrawCards().size());
            PrintACardDisplayer({Null}, {DRAW_PILE_X, DRAW_PILE_Y}, message, false).Draw(deltaTime);
        }

        void InitDiscardPile(float deltaTime) const
        {
            std::wstring message = std::format(L"捨札\n{:2}枚", cardService->GetDiscardCards().size());
            PrintACardDisplayer({Null}, {DISCARD_PILE_X, DISCARD_PILE_Y}, message, false).Draw(deltaTime);
        }

    private:
        static void drawCenterText(int middle_x, int middle_y, int lineCount, const wchar_t* message,
                                   int color = COLOR_WHITE)
        {
            auto textSize = GetFontSize();
            auto textWidth = GetDrawStringSize(&textSize, &textSize, &lineCount, message, -1);
            DrawString(middle_x - textWidth / 2, middle_y - textSize / 2, message, color);
        }


    };

    std::unique_ptr<IDisplayer> CreateCardDisplayer()
    {
        return std::make_unique<CardDisplayer>();
    }
} // namespace mc
