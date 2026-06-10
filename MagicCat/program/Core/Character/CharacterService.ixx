module;

export module CharacterService;

import Enemy;
import Player;
import EventBus;
import Character;

namespace mc {



export class ICharacterService
{
public:
    virtual ~ICharacterService() = default;
    virtual Enemy& GetEnemy() = 0;
    virtual bool NextEnemy() = 0;
    virtual Player& GetPlayer() = 0;
    virtual void Reset() = 0;
};

export Shared<ICharacterService> CreateCharacterService();

} // namespace mc

