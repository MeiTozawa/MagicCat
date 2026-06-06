module;

#include <RandomUtils.h>
#include <tnl_rect.h>

module CardService;

import ServiceLocator;
import Character;

class CardService : public ICardService
{
    std::vector<tnl::Rect> rectOfCards = {};

public:
    CardService()
    {
        deck.push_back(CARD_ROCK_2);
        deck.push_back(CARD_ROCK_3);
        deck.push_back(CARD_ROCK_4);
        deck.push_back(CARD_SCISSORS_2);
        deck.push_back(CARD_SCISSORS_3);
        deck.push_back(CARD_SCISSORS_4);
        deck.push_back(CARD_PAPER_2);
        deck.push_back(CARD_PAPER_3);
        deck.push_back(CARD_PAPER_4);

        drawPile = std::vector<Card>(deck);

        Random::Shuffle(drawPile);
    }

    const std::vector<Card>& GetHandCards() override
    {
        return hand;
    }

    const std::vector<Card>& DrawCards(int count = 1) override
    {
        discardPile.insert(discardPile.end(), hand.begin(), hand.end());
        hand.clear();
        for (int i = 0; i < count; ++i)
        {
            if (drawPile.size() == 0)
            {
                drawPile.insert(drawPile.end(), discardPile.begin(), discardPile.end());
                Random::Shuffle(drawPile);
            }
            auto c = drawPile.back();

            drawPile.pop_back();

            discardPile.push_back(c);

            if (i == 0)
            {
                focus = 0;
                c.is_selected = true;
            }
            hand.push_back(c);
        }
        return hand;
    }

    void ClearRectOfCards() override
    {
        rectOfCards.clear();
    }

    const std::vector<tnl::Rect>& GetRectOfCards() override
    {
        return rectOfCards;
    }

    void PushBackRectOfCard(tnl::Rect r) override
    {
        rectOfCards.push_back(r);
    }

    void MoveFocusToRight() override
    {
        if (hand.empty()) return;
        if (focus >= hand.size() - 1) return;

        hand[focus].is_selected = false;
        focus++;
        hand[focus].is_selected = true;
    }

    void MoveFocusToLeft() override
    {
        if (hand.empty()) return;
        if (focus <= 0) return;

        hand[focus].is_selected = false;
        focus--;
        hand[focus].is_selected = true;
    }

    void PlayCard(Enemy& enemy) override
    {
        if (hand.empty()) return;
        auto card = hand.begin() + focus;
        int t = card->CardType;
        assert(t >= 0 && t <= 2);
        enemy.AddWeight(static_cast<EAttackType>(t), card->Offset);

        hand.erase(card);

        if (focus >= hand.size())
            focus--;
        if (!hand.empty())
            hand[focus].is_selected = true;
    }

    const std::vector<Card>& GetDrawCards() override
    {
        return drawPile;
    }
    const std::vector<Card>& GetDiscardCards() override
    {
        return discardPile;
    }

private:
    std::vector<Card> deck = std::vector<Card>();
    std::vector<Card> hand = std::vector<Card>();
    std::vector<Card> drawPile = std::vector<Card>();
    std::vector<Card> discardPile = std::vector<Card>();
    int focus = 0;
};

static struct RegisterCardService
{
    RegisterCardService()
    {
        ServiceLocator::RegisterSingleton<ICardService, CardService>(std::make_shared<CardService>());
    }
} autoRegister_CardService;
