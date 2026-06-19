module;

#include <dxe.h>
#include <memory>

module GameService;

import SceneService;
import InputService;
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

public:
    GameService(ISceneService* sceneService) : sceneService(sceneService) {}

    void Start() override
    {
    }

    void End() override {}

    void Update(float deltaTime) override
    {
        sceneService->Update(deltaTime);
    }
};

Shared<IGameService> CreateGameService(ISceneService* sceneService)
{
    return std::make_shared<GameService>(sceneService);
}

} // namespace mc

