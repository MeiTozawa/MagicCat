module;

#include <memory>
#include <vector>
#include <string>
#include <format>
#include <RenderUtils.h>

export module Displayer:Character;
import DisplayerBase;

import BattleService;
import RenderService;
import EventBus;
import EffectorFactory;
import ViewEnumMapper;
import HealthComponent;
import Character;
import Enemy;
import Player;
import SceneService;

namespace mc { namespace {
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

        constexpr int TEXT_OFFSET_X = 30;
        constexpr int TEXT_OFFSET_Y = 28;

        constexpr int OFFSET_Y = 120;

        constexpr int RECT_X = 300;
        constexpr int RECT_Y = 100;

        constexpr int THICKNESS = 2;
    }

    class CharacterDisplayer : public Displayers
    {
        IBattleService& characterService;
        IRenderService& renderService;
        int currentFocus = 0;
        bool isMagicMenuOpen = false;
        EventHandle actionSelectionEvent;
        EventHandle addWeightEvent;
        std::vector<std::unique_ptr<EffectorPlayer>> enemyWeightEffectors;

    public:
        CharacterDisplayer(IBattleService& character, IRenderService& render)
            : characterService(character), renderService(render)
        {
            InitEnemyWeightEffectors();

            push_back(CreateLambdaDisplayer([this](float) { PrintPlayerInfo(); }));
            push_back(CreateLambdaDisplayer([this](float) { PrintEnemyInfoWithoutWeight(); }));
            push_back(CreateLambdaDisplayer([this](float) { PrintPlayerActions(currentFocus); }));

            actionSelectionEvent = EventBus::Subscribe<ActionSelectionEvent>([this](const ActionSelectionEvent& e)
            {
                currentFocus = e.selectedIndex;
                isMagicMenuOpen = e.isMagicMenuOpen;
            });
            addWeightEvent = EventBus::Subscribe<AddWeightEvent>([this](const AddWeightEvent& e)
            {
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
            Displayers::Update(deltaTime);

            for (auto& eff : enemyWeightEffectors)
                eff->Update(deltaTime);
        }

        void Draw(float deltaTime) const override
        {
            Displayers::Draw(deltaTime);

            for (auto& eff : enemyWeightEffectors)
                eff->Draw(deltaTime);
        }

    private:
        void PrintPlayerInfo() const
        {
            const Player& player = characterService.GetPlayer();
            const auto playerHealthComp = player.GetHealthComponent();

            auto message = std::format(L"HP: {}/{}", playerHealthComp.GetHealth(), playerHealthComp.GetMaxHealth());
            DrawString(PLAYER_HP_X, PLAYER_HP_Y, message.c_str(), COLOR_WHITE);
            message = std::format(L"MP: {}/{}", player.GetMp(), player.GetMaxMp());
            DrawString(PLAYER_MP_X, PLAYER_MP_Y, message.c_str(), COLOR_WHITE);
        }

        void PrintPlayerActions(int focus) const
        {
            const Player& player = characterService.GetPlayer();
            for (int i = 0; i < 4; ++i)
            {
                float x1 = PLAYER_DAMAGE_START_X;
                float y1 = PLAYER_DAMAGE_START_Y + i * OFFSET_Y;
                float x2 = PLAYER_DAMAGE_START_X + RECT_X;
                float y2 = PLAYER_DAMAGE_START_Y + RECT_Y + i * OFFSET_Y;


                DrawHollowBox(&renderService, x1, y1, x2, y2, THICKNESS, COLOR_WHITE);

                if (i == focus)
                {
                    // 驕ｸ謚樔ｸｭ縺ｮ蝣ｴ蜷医・蟆代＠蜀・・縺ｫ霑ｽ蜉縺ｮ譫邱壹ｒ謠冗判縺励※螟ｪ縺擾ｼ医∪縺溘・莠碁㍾縺ｫ・芽ｦ九○繧・
                    DrawHollowBox(&renderService, x1 + 2 * THICKNESS, y1 + 2 * THICKNESS,
                                  x2 - 2 * THICKNESS, y2 - 2 * THICKNESS,
                                  THICKNESS, COLOR_WHITE);
                }
            }
            if (isMagicMenuOpen)
            {
                DrawString(PLAYER_DAMAGE_START_X + TEXT_OFFSET_X,
                           PLAYER_DAMAGE_START_Y + 0 * OFFSET_Y + TEXT_OFFSET_Y,
                           L"  戻る", COLOR_WHITE);

                uint32_t c1 = player.IsMagicUsable(EMagic::Clairvoyance) ? COLOR_WHITE : COLOR_GRAY;
                DrawString(PLAYER_DAMAGE_START_X + TEXT_OFFSET_X,
                           PLAYER_DAMAGE_START_Y + 1 * OFFSET_Y + TEXT_OFFSET_Y,
                           L"透視 (10MP)", c1);

                uint32_t c2 = player.IsMagicUsable(EMagic::PowerBoost) ? COLOR_WHITE : COLOR_GRAY;
                DrawString(PLAYER_DAMAGE_START_X + TEXT_OFFSET_X,
                           PLAYER_DAMAGE_START_Y + 2 * OFFSET_Y + TEXT_OFFSET_Y,
                           L"⚔UP (7MP)", c2);

                uint32_t c3 = player.IsMagicUsable(EMagic::Heal) ? COLOR_WHITE : COLOR_GRAY;
                DrawString(PLAYER_DAMAGE_START_X + TEXT_OFFSET_X,
                           PLAYER_DAMAGE_START_Y + 3 * OFFSET_Y + TEXT_OFFSET_Y,
                           L"回復 (5MP)", c3);
            }
            else
            {
                DrawString(PLAYER_DAMAGE_START_X + TEXT_OFFSET_X,
                           PLAYER_DAMAGE_START_Y + 0 * OFFSET_Y + TEXT_OFFSET_Y,
                           L"  魔法", COLOR_WHITE);

                constexpr std::pair<EAttackType, const wchar_t*> attackTypes[] = {
                    {EAttackType::Rock, L"✊"},
                    {EAttackType::Scissors, L"✌"},
                    {EAttackType::Paper, L"✋"}
                };

                for (int i = 0; i < 3; ++i)
                {
                    auto color = COLOR_WHITE;
                    std::wstring message = std::format(L"{}⚔：{}", attackTypes[i].second,
                                                       player.GetBaseDamage(attackTypes[i].first));
                    if (int offset = player.GetAttackOffset(); offset != 0)
                    {
                        color = COLOR_RED;
                        message += std::format(L"+{}", offset);
                    }
                    DrawString(PLAYER_DAMAGE_START_X + TEXT_OFFSET_X,
                               PLAYER_DAMAGE_START_Y + (i + 1) * OFFSET_Y + TEXT_OFFSET_Y,
                               message.c_str(), color);
                }
            }
        }

        void InitEnemyWeightEffectors()
        {
            enemyWeightEffectors.clear();

            auto rockWeightDisplayer = CreateLambdaDisplayer([this](float)
            {
                const Enemy& enemy = characterService.GetEnemy();
                std::wstring message = L"✊⚖：";
                if (enemy.IsExposed())
                    message += std::to_wstring(enemy.GetBaseWeight());
                else
                    message += L"?";
                if (auto offset = enemy.GetWeightOffset(EAttackType::Rock); offset != 0)
                    message += std::format(L"+{}", offset);

                DrawString(ENEMY_WEIGHT_START_X + TEXT_OFFSET_X,
                           ENEMY_WEIGHT_START_Y + 0 * OFFSET_Y + TEXT_OFFSET_Y,
                           message.c_str(), COLOR_WHITE);
            });

            enemyWeightEffectors.push_back(CreateHitFlashEffector(std::move(rockWeightDisplayer), COLOR_RED, 300));

            auto scissorsWeightDisplayer = CreateLambdaDisplayer([this](float)
            {
                const Enemy& enemy = characterService.GetEnemy();
                std::wstring message = L"✌⚖：";
                if (enemy.IsExposed())
                    message += std::to_wstring(enemy.GetBaseWeight());
                else
                    message += L"?";
                if (auto offset = enemy.GetWeightOffset(EAttackType::Scissors); offset != 0)
                    message += std::format(L"+{}", offset);

                DrawString(ENEMY_WEIGHT_START_X + TEXT_OFFSET_X,
                           ENEMY_WEIGHT_START_Y + 1 * OFFSET_Y + TEXT_OFFSET_Y,
                           message.c_str(), COLOR_WHITE);
            });

            enemyWeightEffectors.push_back(CreateHitFlashEffector(std::move(scissorsWeightDisplayer), COLOR_RED, 300));

            auto paperWeightDisplayer = CreateLambdaDisplayer([this](float)
            {
                const Enemy& enemy = characterService.GetEnemy();
                std::wstring message = L"✋⚖：";
                if (enemy.IsExposed())
                    message += std::to_wstring(enemy.GetBaseWeight());
                else
                    message += L"?";
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
            const Enemy& enemy = characterService.GetEnemy();
            const auto enemyHealthComp = enemy.GetHealthComponent();

            auto message = enemy.GetName();
            DrawString(ENEMY_NAME_X, ENEMY_NAME_Y, message.c_str(), COLOR_WHITE);

            message = std::format(L"HP: {}/{}", enemyHealthComp.GetHealth(), enemyHealthComp.GetMaxHealth());
            DrawString(ENEMY_HP_X, ENEMY_HP_Y, message.c_str(), COLOR_WHITE);

            for (int i = 0; i < 3; ++i)
            {
                float x1 = ENEMY_WEIGHT_START_X;
                float y1 = ENEMY_WEIGHT_START_Y + i * OFFSET_Y;
                float x2 = ENEMY_WEIGHT_START_X + RECT_X;
                float y2 = ENEMY_WEIGHT_START_Y + RECT_Y + i * OFFSET_Y;

                DrawHollowBox(&renderService, x1, y1, x2, y2, THICKNESS, COLOR_WHITE);
            }

            for (int i = 0; i < 3; ++i)
            {
                float x1 = ENEMY_DAMAGE_START_X;
                float y1 = ENEMY_DAMAGE_START_Y + i * OFFSET_Y;
                float x2 = ENEMY_DAMAGE_START_X + RECT_X;
                float y2 = ENEMY_DAMAGE_START_Y + RECT_Y + i * OFFSET_Y;

                DrawHollowBox(&renderService, x1, y1, x2, y2, THICKNESS, COLOR_WHITE);
            }

            if (enemy.IsExposed())
            {
                DrawString(ENEMY_DAMAGE_START_X + TEXT_OFFSET_X,
                           ENEMY_DAMAGE_START_Y + 0 * OFFSET_Y + TEXT_OFFSET_Y,
                           std::format(L"✊⚔：{}", enemy.GetBaseDamage(EAttackType::Rock)).c_str(), COLOR_WHITE);
                DrawString(ENEMY_DAMAGE_START_X + TEXT_OFFSET_X,
                           ENEMY_DAMAGE_START_Y + 1 * OFFSET_Y + TEXT_OFFSET_Y,
                           std::format(L"✌⚔：{}", enemy.GetBaseDamage(EAttackType::Scissors)).c_str(), COLOR_WHITE);
                DrawString(ENEMY_DAMAGE_START_X + TEXT_OFFSET_X,
                           ENEMY_DAMAGE_START_Y + 2 * OFFSET_Y + TEXT_OFFSET_Y,
                           std::format(L"✋⚔：{}", enemy.GetBaseDamage(EAttackType::Paper)).c_str(), COLOR_WHITE);
            }
            else
            {
                DrawString(ENEMY_DAMAGE_START_X + TEXT_OFFSET_X,
                           ENEMY_DAMAGE_START_Y + 0 * OFFSET_Y + TEXT_OFFSET_Y,
                           L"✊⚔：?", COLOR_WHITE);
                DrawString(ENEMY_DAMAGE_START_X + TEXT_OFFSET_X,
                           ENEMY_DAMAGE_START_Y + 1 * OFFSET_Y + TEXT_OFFSET_Y,
                           L"✌⚔：?", COLOR_WHITE);
                DrawString(ENEMY_DAMAGE_START_X + TEXT_OFFSET_X,
                           ENEMY_DAMAGE_START_Y + 2 * OFFSET_Y + TEXT_OFFSET_Y,
                           L"✋⚔：?", COLOR_WHITE);
            }
        }
    };

    export std::unique_ptr<IDisplayer> CreateCharacterDisplayer(IBattleService& characterService,
                                                                IRenderService& renderService)
    {
        return std::make_unique<CharacterDisplayer>(characterService, renderService);
    }
} // namespace mc
