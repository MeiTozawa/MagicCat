module;

#include <dxe.h>

#include <memory>
#include <string>
#include <format>
#include <DrawStringUtils.h>

module Displayer;

import SceneService;
import EventBus;
import CharacterService;
import ServiceLocator;
import Player;
import Enemy;
import HealthComponent;
import Character;
import EffectorFactory;

namespace mc
{
    namespace
    {
        constexpr int PLAYER_DAMAGE_START_X = 400;
        constexpr int PLAYER_DAMAGE_START_Y = 200;

        constexpr int PLAYER_HP_X = 60;
        constexpr int PLAYER_HP_Y = 200;
        constexpr int ENEMY_NAME_X = 1150;
        constexpr int ENEMY_NAME_Y = 230;
        constexpr int ENEMY_HP_X = 1500;
        constexpr int ENEMY_HP_Y = 230;
        constexpr int PLAYER_MP_X = 60;
        constexpr int PLAYER_MP_Y = 300;

        constexpr int ENEMY_WEIGHT_START_X = 1150;
        constexpr int ENEMY_WEIGHT_START_Y = 300;
        constexpr int ENEMY_DAMAGE_START_X = 1500;
        constexpr int ENEMY_DAMAGE_START_Y = 300;

        constexpr int TEXT_OFFSET_X = 40;
        constexpr int TEXT_OFFSET_Y = 30;

        constexpr int OFFSET_Y = 120;

        constexpr int RECT_X = 300;
        constexpr int RECT_Y = 100;

        constexpr int THICKNESS = 2;
    }

    class CharacterDisplayer : public IDisplayer
    {
        ICharacterService* characterService;
        int currentFocus = 0;
        EventHandle actionSelectionEvent;
        EventHandle addWeightEvent;
        std::vector<std::unique_ptr<EffectorPlayer>> enemyWeightEffectors;

    public:
        CharacterDisplayer()
        {
            characterService = ServiceLocator::Get<ICharacterService>();
            InitEnemyWeightEffectors();
            actionSelectionEvent = EventBus::Subscribe<ActionSelectionEvent>([this](const ActionSelectionEvent& e)
            {
                currentFocus = e.selectedIndex;
            });
            addWeightEvent = EventBus::Subscribe<AddWeightEvent>([this](const AddWeightEvent& e)
            {
                InitEnemyWeightEffectors();
                enemyWeightEffectors[static_cast<int>(e.AttackType)]->Play();
            });
        }

        ~CharacterDisplayer() override
        {
            EventBus::Unsubscribe(actionSelectionEvent);
            EventBus::Unsubscribe(addWeightEvent);
        }

        void Update(float deltaTime) override
        {
            for (auto& eff : enemyWeightEffectors)
                eff->Update(deltaTime);
        }

        void Draw(float deltaTime) const override
        {
            PrintPlayerInfo();
            PrintEnemyInfoWithoutWeight();
            PrintPlayerActions(currentFocus);

            for (auto& eff : enemyWeightEffectors)
                eff->Draw(deltaTime);
        }

    private:
        void PrintPlayerInfo() const
        {
            const Player& player = characterService->GetPlayer();
            const auto playerHealthComp = player.GetHealthComponent();

            auto message = std::format(L"HP: {}/{}", playerHealthComp.GetHealth(), playerHealthComp.GetMaxHealth());
            DrawString(PLAYER_HP_X, PLAYER_HP_Y, message.c_str(), COLOR_WHITE);
            message = std::format(L"MP: {}/{}", player.GetMp(), player.GetMaxMp());
            DrawString(PLAYER_MP_X, PLAYER_MP_Y, message.c_str(), COLOR_WHITE);
        }

        void PrintPlayerActions(int focus) const
        {
            const Player& player = characterService->GetPlayer();
            for (int i = 0; i < 4; ++i)
            {
                for (int k = 0; k < THICKNESS; ++k)
                {
                    float x1 = PLAYER_DAMAGE_START_X + k;
                    float y1 = PLAYER_DAMAGE_START_Y + k + i * OFFSET_Y;
                    float x2 = PLAYER_DAMAGE_START_X + RECT_X - k;
                    float y2 = PLAYER_DAMAGE_START_Y + RECT_Y - k + i * OFFSET_Y;

                    DrawBoxAA(x1, y1, x2, y2, COLOR_WHITE, FALSE);
                }

                if (i == focus)
                {
                    for (int k = 2 * THICKNESS; k < 3 * THICKNESS; ++k)
                    {
                        float x1 = PLAYER_DAMAGE_START_X + k;
                        float y1 = PLAYER_DAMAGE_START_Y + k + i * OFFSET_Y;
                        float x2 = PLAYER_DAMAGE_START_X + RECT_X - k;
                        float y2 = PLAYER_DAMAGE_START_Y + RECT_Y - k + i * OFFSET_Y;

                        DrawBoxAA(x1, y1, x2, y2, COLOR_WHITE, FALSE);
                    }
                }
            }
            DrawFormatString(PLAYER_DAMAGE_START_X + TEXT_OFFSET_X,
                             PLAYER_DAMAGE_START_Y + 0 * OFFSET_Y + TEXT_OFFSET_Y,
                             COLOR_WHITE, L"  魔法");
            DrawFormatString(PLAYER_DAMAGE_START_X + TEXT_OFFSET_X,
                             PLAYER_DAMAGE_START_Y + 1 * OFFSET_Y + TEXT_OFFSET_Y,
                             COLOR_WHITE, L"✊ ⚔：%d", player.GetDamage(EAttackType::Rock));
            DrawFormatString(PLAYER_DAMAGE_START_X + TEXT_OFFSET_X,
                             PLAYER_DAMAGE_START_Y + 2 * OFFSET_Y + TEXT_OFFSET_Y,
                             COLOR_WHITE, L"✌ ⚔：%d", player.GetDamage(EAttackType::Scissors));
            DrawFormatString(PLAYER_DAMAGE_START_X + TEXT_OFFSET_X,
                             PLAYER_DAMAGE_START_Y + 3 * OFFSET_Y + TEXT_OFFSET_Y,
                             COLOR_WHITE, L"✋ ⚔：%d", player.GetDamage(EAttackType::Paper));
        }

        void InitEnemyWeightEffectors()
        {
            enemyWeightEffectors.clear();

            auto rockWeightDisplayer = CreateLambdaDisplayer([this](float)
            {
                const Enemy& enemy = characterService->GetEnemy();
                std::wstring message = L"✊ ⚖：?";
                if (auto offset = enemy.GetWeightOffset(EAttackType::Rock); offset != 0)
                    message += std::format(L"+{}", offset);

                DrawString(ENEMY_WEIGHT_START_X + TEXT_OFFSET_X,
                           ENEMY_WEIGHT_START_Y + 0 * OFFSET_Y + TEXT_OFFSET_Y,
                           message.c_str(), COLOR_WHITE);
            });

            enemyWeightEffectors.push_back(CreateHitFlashEffector(std::move(rockWeightDisplayer), COLOR_RED, 300));

            auto scissorsWeightDisplayer = CreateLambdaDisplayer([this](float)
            {
                const Enemy& enemy = characterService->GetEnemy();
                std::wstring message = L"✌ ⚖：?";
                if (auto offset = enemy.GetWeightOffset(EAttackType::Scissors); offset != 0)
                    message += std::format(L"+{}", offset);

                DrawString(ENEMY_WEIGHT_START_X + TEXT_OFFSET_X,
                           ENEMY_WEIGHT_START_Y + 1 * OFFSET_Y + TEXT_OFFSET_Y,
                           message.c_str(), COLOR_WHITE);
            });

            enemyWeightEffectors.push_back(CreateHitFlashEffector(std::move(scissorsWeightDisplayer), COLOR_RED, 300));

            auto paperWeightDisplayer = CreateLambdaDisplayer([this](float)
            {
                const Enemy& enemy = characterService->GetEnemy();
                std::wstring message = L"✋ ⚖：?";
                if (auto offset = enemy.GetWeightOffset(EAttackType::Paper); offset != 0)
                    message += std::format(L"+{}", offset);

                DrawString(ENEMY_WEIGHT_START_X + TEXT_OFFSET_X,
                           ENEMY_WEIGHT_START_Y + 2 * OFFSET_Y + TEXT_OFFSET_Y,
                           message.c_str(), COLOR_WHITE);
            });

            enemyWeightEffectors.push_back(CreateHitFlashEffector(std::move(paperWeightDisplayer), COLOR_RED, 300));
        }

        void PrintEnemyInfoWithoutWeight() const
        {
            const Enemy& enemy = characterService->GetEnemy();
            const auto enemyHealthComp = enemy.GetHealthComponent();

            auto message = enemy.GetName();
            DrawString(ENEMY_NAME_X, ENEMY_NAME_Y, message.c_str(), COLOR_WHITE);

            message = std::format(L"HP: {}/{}", enemyHealthComp.GetHealth(), enemyHealthComp.GetMaxHealth());
            DrawString(ENEMY_HP_X, ENEMY_HP_Y, message.c_str(), COLOR_WHITE);

            for (int i = 0; i < 3; ++i)
            {
                for (int k = 0; k < THICKNESS; ++k)
                {
                    float x1 = ENEMY_WEIGHT_START_X + k;
                    float y1 = ENEMY_WEIGHT_START_Y + k + i * OFFSET_Y;
                    float x2 = ENEMY_WEIGHT_START_X + RECT_X - k;
                    float y2 = ENEMY_WEIGHT_START_Y + RECT_Y - k + i * OFFSET_Y;

                    DrawBoxAA(x1, y1, x2, y2, COLOR_WHITE, FALSE);
                }
            }

            for (int i = 0; i < 3; ++i)
            {
                for (int k = 0; k < THICKNESS; ++k)
                {
                    float x1 = ENEMY_DAMAGE_START_X + k;
                    float y1 = ENEMY_DAMAGE_START_Y + k + i * OFFSET_Y;
                    float x2 = ENEMY_DAMAGE_START_X + RECT_X - k;
                    float y2 = ENEMY_DAMAGE_START_Y + RECT_Y - k + i * OFFSET_Y;

                    DrawBoxAA(x1, y1, x2, y2, COLOR_WHITE, FALSE);
                }
            }

            DrawFormatString(ENEMY_DAMAGE_START_X + TEXT_OFFSET_X,
                             ENEMY_DAMAGE_START_Y + 0 * OFFSET_Y + TEXT_OFFSET_Y,
                             COLOR_WHITE, L"✊ ⚔：%d", enemy.GetDamage(EAttackType::Rock));
            DrawFormatString(ENEMY_DAMAGE_START_X + TEXT_OFFSET_X,
                             ENEMY_DAMAGE_START_Y + 1 * OFFSET_Y + TEXT_OFFSET_Y,
                             COLOR_WHITE, L"✌ ⚔：%d", enemy.GetDamage(EAttackType::Scissors));
            DrawFormatString(ENEMY_DAMAGE_START_X + TEXT_OFFSET_X,
                             ENEMY_DAMAGE_START_Y + 2 * OFFSET_Y + TEXT_OFFSET_Y,
                             COLOR_WHITE, L"✋ ⚔：%d", enemy.GetDamage(EAttackType::Paper));
        }
    };

    std::unique_ptr<IDisplayer> CreateCharacterDisplayer()
    {
        return std::make_unique<CharacterDisplayer>();
    }
} // namespace mc
