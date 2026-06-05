module;

export module CharacterService;

import Enemy;
import Player;

export class ICharacterService
{
public:
    virtual ~ICharacterService() = default;
    virtual Enemy& GetEnemy() = 0;
    virtual bool NextEnemy() = 0;
    virtual Player& GetPlayer() = 0;
    virtual void Reset() = 0;
};

