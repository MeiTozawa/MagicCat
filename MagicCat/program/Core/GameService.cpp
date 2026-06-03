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

import HealthComponent;

class GameService : public IGameService
{
private:
    EGameState gameState = START;
    Shared<IUiService> uiService = nullptr;
    Shared<IInputService> inputService = nullptr;
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
            uiService->ChangeScene(COMBAT);
            gameState = COMBAT;
            player = std::make_unique<Player>();
        }
    }
    
    void Update_Combat(float deltaTime)
    {
        if (player->IsDead())
        {
            uiService->ChangeScene(START);
            gameState = START;
        }
    }
};

static struct RegisterGameService {
    RegisterGameService() {
        ServiceLocator::RegisterSingleton<IGameService, GameService>(std::make_shared<GameService>());
    }
} autoRegister_GameService;