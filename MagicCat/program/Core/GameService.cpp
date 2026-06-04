module;

#include <dxe.h>
#include <unordered_map>
#include <functional>
#include <memory>

module GameService;

import UiService;
import InputService;
import ServiceLocator;
import Player;
import EnemyService;

import HealthComponent;

import EnemyService;
import Enemy;
import CardService;

constexpr int CARD_MAX = 4;


class GameService : public IGameService
{
private:
    EGameState gameState = START;
    Shared<IUiService> uiService = nullptr;
    Shared<IInputService> inputService = nullptr;
    Shared<IEnemyPool> enemyPool = nullptr;
    Player player{};
    Enemy enemy{};
    Shared<ICardService> cardService;

public:
    GameService()
    {
        // TODO
    }

    void Start() override
    {
        uiService = ServiceLocator::Get<IUiService>();
        inputService = ServiceLocator::Get<IInputService>();
        enemyPool = ServiceLocator::Get<IEnemyPool>();
        cardService = ServiceLocator::Get<ICardService>();
    }

    void End() override
    {
        // TODO
    }

    void Update(float deltaTime) override
    {
        switch (gameState)
        {
        case START:
            if (inputService->IsPressed(InputAction::IgConfirm))
                ChangeToCombat();
            break;
        case COMBAT:
            if (player.IsDead())
                ChangeToStart();
            if (auto p = inputService->OnMouseClick(InputAction::IgMouseClick); p.x >= 0 && p.y >= 0)
            {
                for (const auto& rect : cardService->GetRectOfCards())
                {
                    // TODO: Handling mouse events
                }
            }
            break;
        default: ;
        }

        uiService->Draw();
    }

private:
    void ChangeToStart()
    {
        gameState = START;
        uiService->ChangeSceneTo(START);
    }

    void ChangeToCombat()
    {
        gameState = COMBAT;
        player = Player();
        cardService->DrawCards(CARD_MAX);
        enemy = enemyPool->GetEnemy();
        uiService->ChangeSceneTo(COMBAT);
    }
    
    static void PlayCard(int index, Enemy& enemy, const std::vector<Card>& hand)
    {
        enemy.AddWeight(hand[index]);
    }
};

static struct RegisterGameService
{
    RegisterGameService()
    {
        ServiceLocator::RegisterSingleton<IGameService, GameService>(std::make_shared<GameService>());
    }
} autoRegister_GameService;
