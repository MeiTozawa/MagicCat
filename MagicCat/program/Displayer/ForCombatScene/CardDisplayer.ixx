module;

#include <memory>
#include <vector>
#include <string>
#include <format>
#include <cassert>
#include <algorithm>
#include <RenderUtils.h>

export module Displayer:Card;
import DisplayerBase;

import CardService;
import RenderService;
import AssetService;
import EventBus;
import EffectorFactory;
import AssetEnumMapper;

namespace mc {
    class CardDisplayer : public Displayers
    {
    public:
        CardDisplayer(ICardService& card, IAssetService& asset, IRenderService& render)
            : cardService(card), assetService(asset), renderService(render)
        {
            cachedHand = cardService.GetHandCards();
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

    private:
        std::unique_ptr<Displayer> CreatePrintACardDisplayer(Card card, Point<int> start_position,
                                                             std::wstring message) const
        {
            return CreateLambdaDisplayer([card, start_position, message, this](float)
            {
                auto x = start_position.x, y = start_position.y;
                uint32_t color;
                bool has_icon = true;
                switch (card.CardType)
                {
                case ECardType::Rock:     color = COLOR_CARD_ROCK;     break;
                case ECardType::Paper:    color = COLOR_CARD_PAPER;    break;
                case ECardType::Scissors: color = COLOR_CARD_SCISSORS; break;
                case ECardType::Magic:    color = COLOR_CARD_MAGIC;    break;
                default:
                    has_icon = false;
                    color = COLOR_CARD_DEFAULT;
                    break;
                }
                DrawCardBorder(x, y, color);
                DrawCardContent(x, y, card, message, color, has_icon);
            });
        }

        void DrawCardBorder(int x, int y, uint32_t color) const
        {
            for (int i = 0; i < THICKNESS; ++i)
            {
                float x1 = x + i;
                float y1 = y + i;
                float x2 = x + CARD_WIDTH - i;
                float y2 = y + CARD_HEIGHT - i;
                float radius = std::max(0.f, static_cast<float>(RADIUS - i));
                DrawRoundRectAA(x1, y1, x2, y2, radius, radius, 32, color, FALSE);
            }
        }

        void DrawCardContent(int x, int y, Card card, const std::wstring& message,
                             uint32_t color, bool has_icon) const
        {
            if (has_icon)
            {
                int icon = assetService.GetImageHandle(ToImage(card.CardType));
                if (icon != -1)
                {
                    renderService.DrawRotaGraphF(x + CARD_WIDTH / 2.f, y + CARD_HEIGHT / 3.5f,
                                                 IMAGE_SCALE, 0.0, icon, true);
                }
                renderService.DrawCenterString(x + CARD_WIDTH / 2, y + CARD_HEIGHT / 2 + 10,
                                               message.c_str(), color);
            }
            else
            {
                renderService.DrawCenterString(x + CARD_WIDTH / 2, y + CARD_HEIGHT / 2 - 30,
                                               message.c_str(), color);
            }
        }

        void RebuildDisplayers(bool isDraw = false)
        {
            displayers.clear();
            auto position = Point<int>{CARD_START_X, CARD_START_Y};
            for (size_t i = 0; i < cachedHand.size(); ++i)
            {
                std::wstring msg = std::format(L"+{}", cachedHand[i].Power);
                auto cardDisplay = CreatePrintACardDisplayer(cachedHand[i], position, msg);

                if (isDraw && i == cachedHand.size() - 1)
                    cardDisplay->AddEffector(CreateHitFlashEffector(renderService, 0x000000, 300));

                push_back(std::move(cardDisplay));
                position.x += OFFSET_X;
            }

            std::wstring drawPileMsg = std::format(L"山札\n{:2}枚", cardService.GetDrawCards().size());
            push_back(CreatePrintACardDisplayer({ECardType::Null}, {DRAW_PILE_X1, DRAW_PILE_Y1}, drawPileMsg));

            std::wstring discardPileMsg = std::format(L"捨札\n{:2}枚", cardService.GetDiscardCards().size());
            push_back(CreatePrintACardDisplayer({ECardType::Null}, {DISCARD_PILE_X, DISCARD_PILE_Y}, discardPileMsg));
        }

        ICardService& cardService;
        IAssetService& assetService;
        IRenderService& renderService;
        EventHandle handUpdateHandle;
        std::vector<Card> cachedHand;

    private:
        static constexpr int CARD_START_X = 400;
        static constexpr int CARD_START_Y = 750;
        static constexpr int DISCARD_PILE_X = 50;
        static constexpr int DISCARD_PILE_Y = 750;
        static constexpr int OFFSET_X = 250;

        static constexpr int THICKNESS = 5;
        static constexpr int RADIUS = 30;

        static constexpr float IMAGE_SCALE = 0.3f;
    };

    export std::unique_ptr<Displayer> CreateCardDisplayer(ICardService& cardService, IAssetService& assetService,
                                                          IRenderService& renderService)
    {
        return std::make_unique<CardDisplayer>(cardService, assetService, renderService);
    }
} // namespace mc
