module;

export module DataView;

import CharacterService;
import ServiceLocator;
import Player;
import Enemy;

export class DataView
{
    Shared<ICharacterService> characterService;
    Enemy& enemy;
    Player& player;

    DataView() : enemy(), player()
    {
        characterService = ServiceLocator::Get<ICharacterService>();
        characterService->Reset();

        enemy = characterService->GetEnemy();
        player = characterService->GetPlayer();
    }
};
