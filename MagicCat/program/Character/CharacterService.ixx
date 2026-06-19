module;

export module CharacterService;

import Enemy;
import Player;
import EventBus;
import Character;
import ConfigService;

namespace mc
{
    export class ICharacterService
    {
    public:
        virtual ~ICharacterService() = default;
        virtual void Start() =0;
        virtual Enemy& GetEnemy() = 0;
        virtual Player& GetPlayer() = 0;
    };

    export Shared<ICharacterService> CreateCharacterService(IConfigService* configService);
} // namespace mc
