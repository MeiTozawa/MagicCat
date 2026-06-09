module;

#include <array>
#include <dxe.h>

export module DataView;

import CharacterService;
import ServiceLocator;
import Player;
import Enemy;

constexpr int PLAYER_DATA_START_X = 400;
constexpr int PLAYER_DATA_START_Y = 300;
constexpr int ENEMY_DATA_START_X = 1200;
constexpr int ENEMY_DATA_START_Y = 300;
constexpr int OFFSET_Y = 150;
constexpr int RECT_X = 300;
constexpr int RECT_Y = 100;

constexpr std::array<RECTDATA, 3> PlayerRectangles = {{
    {
        PLAYER_DATA_START_X, PLAYER_DATA_START_Y,
        PLAYER_DATA_START_X + RECT_X, PLAYER_DATA_START_Y + RECT_Y,
        DX_BLEND_ONE, 0
    },
    {
        PLAYER_DATA_START_X, PLAYER_DATA_START_Y + OFFSET_Y,
        PLAYER_DATA_START_X + RECT_X, PLAYER_DATA_START_Y + RECT_Y + OFFSET_Y,
        DX_BLEND_ONE, 0
    },
    {
        PLAYER_DATA_START_X, PLAYER_DATA_START_Y + 2 * OFFSET_Y,
        PLAYER_DATA_START_X + RECT_X, PLAYER_DATA_START_Y + RECT_Y + 2 * OFFSET_Y,
        DX_BLEND_ONE, 0
    }
}};

constexpr std::array<RECTDATA, 3> EnemyRectangles = {{
    {
        ENEMY_DATA_START_X, ENEMY_DATA_START_Y,
        ENEMY_DATA_START_X + RECT_X, ENEMY_DATA_START_Y + RECT_Y,
        DX_BLEND_ONE, 0
    },
    {
        ENEMY_DATA_START_X, ENEMY_DATA_START_Y + OFFSET_Y,
        ENEMY_DATA_START_X + RECT_X, ENEMY_DATA_START_Y + RECT_Y + OFFSET_Y,
        DX_BLEND_ONE, 0
    },
    {
        ENEMY_DATA_START_X, ENEMY_DATA_START_Y + 2 * OFFSET_Y,
        ENEMY_DATA_START_X + RECT_X, ENEMY_DATA_START_Y + RECT_Y + 2 * OFFSET_Y,
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

    void PrintPlayerData(uint32_t color = 0xFFFFFF) const
    {
        DrawBoxSet(PlayerRectangles.data(), PlayerRectangles.size());

        DrawFormatString(PLAYER_DATA_START_X, PLAYER_DATA_START_Y + 0 * OFFSET_Y, color, L"Rock: %d", player.rockAttack);
        DrawFormatString(PLAYER_DATA_START_X, PLAYER_DATA_START_Y + 1 * OFFSET_Y, color, L"Scissors: %d", player.scissorsAttack);
        DrawFormatString(PLAYER_DATA_START_X, PLAYER_DATA_START_Y + 2 * OFFSET_Y, color, L"Paper: %d", player.paperAttack);
    }
    
    void PrintEnemyData(uint32_t color = 0xFFFFFF) const
    {
        DrawBoxSet(EnemyRectangles.data(), EnemyRectangles.size());
        
        DrawFormatString(ENEMY_DATA_START_X, ENEMY_DATA_START_Y + 0 * OFFSET_Y, color, L"Rock: %d", enemy.rockWeight);
        DrawFormatString(ENEMY_DATA_START_X, ENEMY_DATA_START_Y + 1 * OFFSET_Y, color, L"Scissors: %d", enemy.scissorsWeight);
        DrawFormatString(ENEMY_DATA_START_X, ENEMY_DATA_START_Y + 2 * OFFSET_Y, color, L"Paper: %d", enemy.paperWeight);
    }
};