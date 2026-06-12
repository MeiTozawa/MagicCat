module;

#include <dxe.h>

#include <memory>
#include <string>
#include <format>
#include <cstdint>

module Displayer;

import SceneService;
import EventBus;
import CharacterService;
import ServiceLocator;
import Player;
import Enemy;
import HealthComponent;
import Character;

namespace mc
{
    namespace {
        constexpr int PLAYER_DATA_START_X = 400;
        constexpr int PLAYER_DATA_START_Y = 200;

        constexpr int PLAYER_HP_X = 100;
        constexpr int PLAYER_HP_Y = 200;
        constexpr int PLAYER_MP_X = 100;
        constexpr int PLAYER_MP_Y = 300;

        constexpr int ENEMY_DATA_START_X = 1200;
        constexpr int ENEMY_DATA_START_Y = 300;

        constexpr int TEXT_OFFSET_X = 50;
        constexpr int TEXT_OFFSET_Y = 37;

        constexpr int OFFSET_Y = 120;

        constexpr int RECT_X = 300;
        constexpr int RECT_Y = 100;

        constexpr int THICKNESS = 2;
    }

    class CharacterDisplayer : public IDisplayer
    {
        ICharacterService* characterService;
        int currentFocus = 0;
        EventHandle actionSelectionHandle;

    public:
        CharacterDisplayer()
        {
            characterService = ServiceLocator::Get<ICharacterService>();
            actionSelectionHandle = EventBus::Subscribe<ActionSelectionEvent>([this](const ActionSelectionEvent& e)
            {
                currentFocus = e.selectedIndex;
            });
        }

        ~CharacterDisplayer() override
        {
            EventBus::Unsubscribe(actionSelectionHandle);
        }

        void Update(float deltaTime) override {}

        void Draw(float deltaTime) const override
        {
            PrintPlayerInfo();
            PrintEnemyInfo();
            PrintPlayerActions(currentFocus);
        }

    private:
        void PrintPlayerInfo(uint32_t color = 0xFFFFFF) const
        {
            const Player& player = characterService->GetPlayer();
            const auto playerHealthComp = player.GetHealthComponent();

            auto message = std::format(L"HP: {}/{}", playerHealthComp.GetHealth(), playerHealthComp.GetMaxHealth());
            DrawString(PLAYER_HP_X, PLAYER_HP_Y, message.c_str(), color);
            message = std::format(L"MP: {}/{}", player.GetMp(), player.GetMaxMp());
            DrawString(PLAYER_MP_X, PLAYER_MP_Y, message.c_str(), color);
        }

        void PrintPlayerActions(int focus = 0, uint32_t color = 0xFFFFFF) const
        {
            const Player& player = characterService->GetPlayer();
            for (int i = 0; i < 4; ++i)
            {
                for (int k = 0; k < THICKNESS; ++k)
                {
                    float x1 = PLAYER_DATA_START_X + k;
                    float y1 = PLAYER_DATA_START_Y + k + i * OFFSET_Y;
                    float x2 = PLAYER_DATA_START_X + RECT_X - k;
                    float y2 = PLAYER_DATA_START_Y + RECT_Y - k + i * OFFSET_Y;

                    DrawBoxAA(x1, y1, x2, y2, color, FALSE);
                }

                if (i == focus)
                {
                    for (int k = 2 * THICKNESS; k < 3 * THICKNESS; ++k)
                    {
                        float x1 = PLAYER_DATA_START_X + k;
                        float y1 = PLAYER_DATA_START_Y + k + i * OFFSET_Y;
                        float x2 = PLAYER_DATA_START_X + RECT_X - k;
                        float y2 = PLAYER_DATA_START_Y + RECT_Y - k + i * OFFSET_Y;

                        DrawBoxAA(x1, y1, x2, y2, color, FALSE);
                    }
                }
            }
            DrawFormatString(PLAYER_DATA_START_X + TEXT_OFFSET_X,
                             PLAYER_DATA_START_Y + 0 * OFFSET_Y + TEXT_OFFSET_Y,
                             color, L"  Spells  ");
            DrawFormatString(PLAYER_DATA_START_X + TEXT_OFFSET_X,
                             PLAYER_DATA_START_Y + 1 * OFFSET_Y + TEXT_OFFSET_Y,
                             color, L"Rock:     %d", player.GetDamage(EAttackType::Rock));
            DrawFormatString(PLAYER_DATA_START_X + TEXT_OFFSET_X,
                             PLAYER_DATA_START_Y + 2 * OFFSET_Y + TEXT_OFFSET_Y,
                             color, L"Scissors: %d", player.GetDamage(EAttackType::Scissors));
            DrawFormatString(PLAYER_DATA_START_X + TEXT_OFFSET_X,
                             PLAYER_DATA_START_Y + 3 * OFFSET_Y + TEXT_OFFSET_Y,
                             color, L"Paper:    %d", player.GetDamage(EAttackType::Paper));
        }

        void PrintEnemyInfo(uint32_t color = 0xFFFFFF) const
        {
            const Enemy& enemy = characterService->GetEnemy();
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

            if (auto offset = enemy.GetWeightOffset(EAttackType::Rock); offset == 0)
                message = std::format(L"Rock:     ?");
            else
                message = std::format(L"Rock:     ?+{}", offset);

            DrawString(ENEMY_DATA_START_X + TEXT_OFFSET_X,
                       ENEMY_DATA_START_Y + 0 * OFFSET_Y + TEXT_OFFSET_Y,
                       message.c_str(), color);


            if (auto offset = enemy.GetWeightOffset(EAttackType::Scissors); offset == 0)
                message = std::format(L"Scissors: ?");
            else
                message = std::format(L"Scissors: ?+{}", offset);

            DrawString(ENEMY_DATA_START_X + TEXT_OFFSET_X,
                       ENEMY_DATA_START_Y + 1 * OFFSET_Y + TEXT_OFFSET_Y,
                       message.c_str(), color);


            if (auto offset = enemy.GetWeightOffset(EAttackType::Paper); offset == 0)
                message = std::format(L"Paper:    ?");
            else
                message = std::format(L"Paper:    ?+{}", offset);

            DrawString(ENEMY_DATA_START_X + TEXT_OFFSET_X,
                       ENEMY_DATA_START_Y + 2 * OFFSET_Y + TEXT_OFFSET_Y,
                       message.c_str(), color);
        }
    };

    std::unique_ptr<IDisplayer> CreateCharacterDisplayer()
    {
        return std::make_unique<CharacterDisplayer>();
    }
} // namespace mc
