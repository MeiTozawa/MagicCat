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

class GameService : public IGameService
{
private:
    EGameState gameState = START;
    Shared<IUiService> uiService = nullptr;
    Shared<IInputService> inputService = nullptr;
    Shared<IEnemyPool> enemyPool = nullptr;
    std::unordered_map<EGameState, std::function<void(float)>> UpdateStateMappings = {};
    std::unique_ptr<Player> player = nullptr;
public:
    GameService()
    {
        UpdateStateMappings[START] = [this](float deltaTime) { Update_Start(deltaTime); };
        UpdateStateMappings[COMBAT] = [this](float deltaTime) { Update_Combat(deltaTime); };
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
        UpdateStateMappings[gameState](deltaTime);
        uiService->Draw();
    }
    
private:
    void Update_Start(float deltaTime)
    {
        if (inputService->IsPressed(InputAction::IgConfirm))
        {
            gameState = COMBAT;
            player = std::make_unique<Player>();
            uiService->ChangeScene(COMBAT);
        }
    }
    
    void Update_Combat(float deltaTime)
    {
        if (player->IsDead())
        {
            gameState = START;
            uiService->ChangeScene(START);
        }
    }
};

static struct RegisterGameService {
    RegisterGameService() {
        ServiceLocator::RegisterSingleton<IGameService, GameService>(std::make_shared<GameService>());
    }
} autoRegister_GameService;