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

constexpr int TEXT_OFFSET_X = 50;
constexpr int TEXT_OFFSET_Y = 37;

constexpr int OFFSET_Y = 150;

constexpr int RECT_X = 300;
constexpr int RECT_Y = 100;

constexpr int THICKNESS = 2;

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
        for (int i = 0; i < 3; ++i)
        {
            for (int k = 0; k < THICKNESS; ++k)
            {
                float x1 = PLAYER_DATA_START_X + k;
                float y1 = PLAYER_DATA_START_Y + k + i * OFFSET_Y;
                float x2 = PLAYER_DATA_START_X + RECT_X - k;
                float y2 = PLAYER_DATA_START_Y + RECT_Y - k + i * OFFSET_Y;

                DrawBoxAA(x1, y1, x2, y2, color, FALSE);
            }
        }

        DrawFormatString(PLAYER_DATA_START_X + TEXT_OFFSET_X,
                         PLAYER_DATA_START_Y + 0 * OFFSET_Y + TEXT_OFFSET_Y,
                         color, L"Rock:     %d", player.rockAttack);
        DrawFormatString(PLAYER_DATA_START_X + TEXT_OFFSET_X,
                         PLAYER_DATA_START_Y + 1 * OFFSET_Y + TEXT_OFFSET_Y,
                         color, L"Scissors: %d", player.scissorsAttack);
        DrawFormatString(PLAYER_DATA_START_X + TEXT_OFFSET_X,
                         PLAYER_DATA_START_Y + 2 * OFFSET_Y + TEXT_OFFSET_Y,
                         color, L"Paper:    %d", player.paperAttack);
    }

    void PrintEnemyData(uint32_t color = 0xFFFFFF) const
    {
        for (int i = 0; i < 3; ++i)
        {
            for (int k = 0; k < THICKNESS; ++k)
            {
                float x1 = ENEMY_DATA_START_X + k;
                float y1 = ENEMY_DATA_START_Y + k + i * OFFSET_Y;
                float x2 = ENEMY_DATA_START_X + RECT_X - k;
                float y2 = ENEMY_DATA_START_Y + RECT_Y - k + i * OFFSET_Y;

                DrawBoxAA(x1, y1, x2, y2, color, FALSE);
            }
        }

        std::wstring message;
        if (auto offset = enemy.GetRockWeightOffset(); offset == 0)
            message = std::format(L"Rock:     ?");
        else
            message = std::format(L"Rock:     ?+{:2}", offset);
        DrawString(ENEMY_DATA_START_X + TEXT_OFFSET_X,
                   ENEMY_DATA_START_Y + 0 * OFFSET_Y + TEXT_OFFSET_Y,
                   message.c_str(), color);
        
        if (auto offset = enemy.GetScissorsWeightOffset(); offset == 0)
            message = std::format(L"Scissors: ?");
        else
            message = std::format(L"Scissors: ?+{:2}", offset);
        DrawString(ENEMY_DATA_START_X + TEXT_OFFSET_X,
                   ENEMY_DATA_START_Y + 1 * OFFSET_Y + TEXT_OFFSET_Y,
                   message.c_str(), color);       
        
        if (auto offset = enemy.GetPaperWeightOffset(); offset == 0)
            message = std::format(L"Paper:    ?");
        else
            message = std::format(L"Paper:    ?+{:2}", offset);
        DrawString(ENEMY_DATA_START_X + TEXT_OFFSET_X,
                   ENEMY_DATA_START_Y + 2 * OFFSET_Y + TEXT_OFFSET_Y,
                   message.c_str(), color);
    }
};
