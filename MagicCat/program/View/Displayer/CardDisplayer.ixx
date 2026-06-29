module;

#include <memory>
#include <vector>
#include <string>
#include <format>
#include <cassert>
#include <RenderUtils.h>

export module Displayer:Card;
import DisplayerBase;

import CardService;
import RenderService;
import AssetService;
import EventBus;
import EffectorFactory;
import ViewEnumMapper;

namespace mc
{

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

    class CardDisplayer : public Displayers
    {
        ICardService& cardService;
        IAssetService& assetService;
        IRenderService& renderService;

        EventHandle handUpdateHandle;
        std::vector<Card> cachedHand;

        std::unique_ptr<IDisplayer> CreatePrintACardDisplayer(Card card, tnl::Vector2i start_position, std::wstring message) const
        {
            return CreateLambdaDisplayer([card, start_position, message, this](float deltaTime)
            {
                auto x = start_position.x, y = start_position.y;
                uint32_t color;
                int thickness = THICKNESS;
                bool has_icon = true;
                switch (card.CardType)
                {
                case ECardType::Rock:
                    color = COLOR_CARD_ROCK;
                    break;
                case ECardType::Paper:
                    color = COLOR_CARD_PAPER;
                    break;
                case ECardType::Scissors:
                    color = COLOR_CARD_SCISSORS;
                    break;
                case ECardType::Magic:
                    color = COLOR_CARD_MAGIC;
                    break;
                default:
                    has_icon = false;
                    color = COLOR_CARD_DEFAULT;
                    break;
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
                if (has_icon)
                {
                    int icon = assetService.GetImageHandle(ToImage(card.CardType));
                    if (icon != -1)
                    {
                        DrawRotaGraphF(x + CARD_WIDTH / 2.f, y + CARD_HEIGHT / 3.5f, IMAGE_SCALE, 0.0, icon, TRUE);
                    }
                    renderService.DrawCenterString( x + CARD_WIDTH / 2, y + CARD_HEIGHT / 2 + 10,
                                     message.c_str(), color);
                }
                else
                {
                    renderService.DrawCenterString( x + CARD_WIDTH / 2, y + CARD_HEIGHT / 2 - 30,
                                     message.c_str(), color);
                }
            });
        }

        void RebuildDisplayers(bool isDraw = false)
        {
            displayers.clear();
            auto position = tnl::Vector2i{CARD_START_X, CARD_START_Y};
            for (size_t i = 0; i < cachedHand.size(); ++i)
            {
                std::wstring msg = std::format(L"+{}", cachedHand[i].Power);
                auto cardDisplay = CreatePrintACardDisplayer(cachedHand[i], position, msg);

                // If this is the newest card, wrap it in HitFlashEffector
                if (isDraw && i == cachedHand.size() - 1)
                {
                    auto flashEffector = CreateHitFlashEffector(std::move(cardDisplay), 0x000000, 300);
                    flashEffector->Play();
                    push_back(std::move(flashEffector));
                }
                else
                {
                    push_back(std::move(cardDisplay));
                }

                position.x += OFFSET_X;
            }
            
            // Add draw pile
            std::wstring drawPileMsg = std::format(L"山札\n{:2}枚", cardService.GetDrawCards().size());
            push_back(CreatePrintACardDisplayer({ECardType::Null}, {DRAW_PILE_X, DRAW_PILE_Y}, drawPileMsg));
            
            // Add discard pile
            std::wstring discardPileMsg = std::format(L"捨札\n{:2}枚", cardService.GetDiscardCards().size());
            push_back(CreatePrintACardDisplayer({ECardType::Null}, {DISCARD_PILE_X, DISCARD_PILE_Y}, discardPileMsg));
        }

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
    };

    export std::unique_ptr<IDisplayer> CreateCardDisplayer(ICardService& cardService, IAssetService& assetService, IRenderService& renderService)
    {
        return std::make_unique<CardDisplayer>(cardService, assetService, renderService);
    }
} // namespace mc
