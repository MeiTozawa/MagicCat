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
import Character;

namespace mc {

constexpr int CARD_MAX = 4;


class GameService : public IGameService
{
    ISceneService& sceneService;

public:
    explicit GameService(ISceneService& sceneService) : sceneService(sceneService) {}

    void Start() override
    {
    }

    void End() override {}

    void Update(float deltaTime) override
    {
        sceneService.Update(deltaTime);
    }
};

std::unique_ptr<IGameService> CreateGameService(ISceneService& sceneService)
{
    return std::make_unique<GameService>(sceneService);
}

} // namespace mc
