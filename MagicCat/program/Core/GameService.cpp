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
import Enmey;

class GameService : public IGameService
{
private:
    EGameState gameState = START;
    Shared<IUiService> uiService = nullptr;
    Shared<IInputService> inputService = nullptr;
    Shared<IEnemyPool> enemyPool = nullptr;
    Player player{};
    Enemy enemy{};

public:
    GameService()
    {
        
    }

    void Start() override
    {
        uiService = ServiceLocator::Get<IUiService>();
        inputService = ServiceLocator::Get<IInputService>();
        enemyPool = ServiceLocator::Get<IEnemyPool>();
    }

    void End() override
    {
        
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
        enemy = enemyPool->GetEnemy();
        uiService->ChangeSceneTo(COMBAT);
    }
};

static struct RegisterGameService
{
    RegisterGameService()
    {
        ServiceLocator::RegisterSingleton<IGameService, GameService>(std::make_shared<GameService>());
    }
} autoRegister_GameService;
