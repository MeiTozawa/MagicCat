module;

#include <dxe.h>

export module DataView;

import CharacterService;
import ServiceLocator;
import Player;
import Enemy;

constexpr int PLAYER_DATA_START_X = 400;
constexpr int PLAYER_DATA_START_Y = 300;
constexpr int OFFSET_Y = 150;
constexpr int RECT_X = 300;
constexpr int RECT_Y = 100;

constexpr std::array<RECTDATA, 3> Rectangles = {{
    {
        PLAYER_DATA_START_X, PLAYER_DATA_START_Y,
        PLAYER_DATA_START_X + RECT_X, PLAYER_DATA_START_Y + RECT_Y
        DX_BLEND_ONE, 0
    },
    {
        PLAYER_DATA_START_X, PLAYER_DATA_START_Y + OFFSET_Y,
        PLAYER_DATA_START_X + RECT_X, PLAYER_DATA_START_Y + RECT_Y + OFFSET_Y
        DX_BLEND_ONE, 0
    },
    {
        PLAYER_DATA_START_X, PLAYER_DATA_START_Y + 2 * OFFSET_Y,
        PLAYER_DATA_START_X + RECT_X, PLAYER_DATA_START_Y + RECT_Y + 2 * OFFSET_Y
        DX_BLEND_ONE, 0
    }
}};


export class DataView
{
    Shared<ICharacterService> characterService;
    const Enemy& enemy;
    const Player& player;
    

    
public:
    DataView() : characterService(ServiceLocator::Get<ICharacterService>()), 
                 enemy(characterService->GetEnemy()), 
                 player(characterService->GetPlayer())
    {
        characterService->Reset();
    }

    void PrintPlayerData()
    {
        DrawBoxSet(Rectangles.data(), Rectangles.size());
    }
};
