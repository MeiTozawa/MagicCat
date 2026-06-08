module;

#include <dxe.h>
#include <memory>

module GameService;

import SceneService;
import InputService;
import ServiceLocator;
import Player;
import HealthComponent;
import Enemy;
import CardService;
import AssetService;
import CharacterService;
import Character;

constexpr int CARD_MAX = 4;


class GameService : public IGameService
{
    Shared<ISceneService> sceneService = nullptr;
    Shared<IInputService> inputService = nullptr;
    Shared<ICharacterService> characterService = nullptr;
    Shared<IAssetService> assetService;
    Shared<ICardService> cardService;

public:
    GameService() {}

    void Start() override
    {
        sceneService = ServiceLocator::Get<ISceneService>();
        inputService = ServiceLocator::Get<IInputService>();
        characterService = ServiceLocator::Get<ICharacterService>();
        cardService = ServiceLocator::Get<ICardService>();
        assetService = ServiceLocator::Get<IAssetService>();
        assetService->LoadAssets();
    }

    void End() override {}

    void Update(float deltaTime) override
    {
        sceneService->Update(deltaTime);
    }
};

static struct RegisterGameService
{
    RegisterGameService()
    {
        ServiceLocator::RegisterSingleton<IGameService, GameService>(std::make_shared<GameService>());
    }
} autoRegister_GameService;
