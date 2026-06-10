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

namespace mc {

constexpr int CARD_MAX = 4;


class GameService : public IGameService
{
    ISceneService* sceneService = nullptr;
    IInputService* inputService = nullptr;
    ICharacterService* characterService = nullptr;
    IAssetService* assetService = nullptr;
    ICardService* cardService = nullptr;

public:
    GameService() {}

    void Start() override
    {
        sceneService = ServiceLocator::Get<ISceneService>();
        inputService = ServiceLocator::Get<IInputService>();
        assetService = ServiceLocator::Get<IAssetService>();

        characterService = ServiceLocator::Get<ICharacterService>();
        characterService->Reset();
        
        cardService = ServiceLocator::Get<ICardService>();
    }

    void End() override {}

    void Update(float deltaTime) override
    {
        sceneService->Update(deltaTime);
    }
};

Shared<IGameService> CreateGameService()
{
    return std::make_shared<GameService>();
}

} // namespace mc

