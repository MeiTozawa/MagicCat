module;

#include <memory>

export module CombatController;

import InputService;
import BattleService;
import SceneService;
import CardService;

namespace mc
{
    export constexpr int ACTION_MAGIC = 0;
    export constexpr int ACTION_ROCK = 1;
    export constexpr int ACTION_SCISSORS = 2;
    export constexpr int ACTION_PAPER = 3;
    export constexpr int ACTION_MAX = ACTION_PAPER;

    export class ICombatController
    {
    public:
        virtual ~ICombatController() = default;
        virtual void Update(float deltaTime) = 0;
        virtual void Reset() = 0;
    };

    export std::unique_ptr<ICombatController> CreateCombatController(IInputService& inputService, IBattleService& BattleService, ISceneService& sceneService, ICardService& cardService);
}
