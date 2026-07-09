module;

#include <memory>
#include <vector>
#include <array>
#include <string>
#include <format>
#include <RenderUtils.h>

export module Displayer:Character;
import DisplayerBase;

import BattleService;
import RenderService;
import EventBus;
import EffectorFactory;
import AssetEnumMapper;
import HealthComponent;
import Character;
import Enemy;
import Player;
import SceneService;

namespace mc {
    class CharacterDisplayer : public Displayers
    {
    private:
        static constexpr int PLAYER_DAMAGE_START_X = ACTION_MENU_X;
        static constexpr int PLAYER_DAMAGE_START_Y = ACTION_MENU_Y;

        static constexpr int PLAYER_HP_X = 60;
        static constexpr int PLAYER_HP_Y = 200;
        static constexpr int ENEMY_NAME_X = 1150;
        static constexpr int ENEMY_NAME_Y = 230;
        static constexpr int ENEMY_HP_X = 1500;
        static constexpr int ENEMY_HP_Y = 230;
        static constexpr int PLAYER_MP_X = 60;
        static constexpr int PLAYER_MP_Y = 300;

        static constexpr int ENEMY_WEIGHT_START_X = 1150;
        static constexpr int ENEMY_WEIGHT_START_Y = 300;
        static constexpr int ENEMY_DAMAGE_START_X = 1500;
        static constexpr int ENEMY_DAMAGE_START_Y = 300;

        static constexpr int TEXT_OFFSET_X = 30;
        static constexpr int TEXT_OFFSET_Y = 28;

        static constexpr int OFFSET_Y = ACTION_MENU_STEP_Y;

        static constexpr int RECT_X = ACTION_MENU_W;
        static constexpr int RECT_Y = ACTION_MENU_H;

        static constexpr int THICKNESS = 2;

    public:
        CharacterDisplayer(IBattleService& character, IRenderService& render)
            : characterService(character), renderService(render)
        {
            InitEnemyWeightDisplayers();

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
                const int idx = static_cast<int>(e.AttackType);
                if (idx >= 0 && idx < 3 && weightDisplayers[idx])
                    weightDisplayers[idx]->AddEffector(CreateHitFlashEffector(renderService, COLOR_RED, 300));
            });
        }

        ~CharacterDisplayer() override
        {
            EventBus::Unsubscribe(actionSelectionEvent);
            EventBus::Unsubscribe(addWeightEvent);
        }

    private:
        void PrintPlayerInfo() const
        {
            const Player& player = characterService.GetPlayer();
            const auto playerHealthComp = player.GetHealthComponent();

            auto message = std::format(L"HP: {}/{}", playerHealthComp.GetHealth(), playerHealthComp.GetMaxHealth());
            renderService.DrawString(PLAYER_HP_X, PLAYER_HP_Y, message.c_str(), COLOR_WHITE);
            message = std::format(L"MP: {}/{}", player.GetMp(), player.GetMaxMp());
            renderService.DrawString(PLAYER_MP_X, PLAYER_MP_Y, message.c_str(), COLOR_WHITE);
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

                renderService.DrawHollowBox(x1, y1, x2, y2, THICKNESS, COLOR_WHITE);

                if (i == focus)
                {
                    // 選択中の枠は内側にもう一重追加して太く見せる
                    renderService.DrawHollowBox(x1 + 2 * THICKNESS, y1 + 2 * THICKNESS,
                                                x2 - 2 * THICKNESS, y2 - 2 * THICKNESS,
                                                THICKNESS, COLOR_WHITE);
                }
            }
            if (isMagicMenuOpen)
                PrintMagicMenu(player);
            else
                PrintAttackMenu(player);
        }

        void PrintMagicMenu(const Player& player) const
        {
            renderService.DrawString(PLAYER_DAMAGE_START_X + TEXT_OFFSET_X,
                       PLAYER_DAMAGE_START_Y + 0 * OFFSET_Y + TEXT_OFFSET_Y,
                       L"  戻る", COLOR_WHITE);

            uint32_t c1 = player.IsMagicUsable(EMagic::Clairvoyance) ? COLOR_WHITE : COLOR_GRAY;
            renderService.DrawString(PLAYER_DAMAGE_START_X + TEXT_OFFSET_X,
                       PLAYER_DAMAGE_START_Y + 1 * OFFSET_Y + TEXT_OFFSET_Y,
                       L"透視 (10MP)", c1);

            uint32_t c2 = player.IsMagicUsable(EMagic::PowerBoost) ? COLOR_WHITE : COLOR_GRAY;
            renderService.DrawString(PLAYER_DAMAGE_START_X + TEXT_OFFSET_X,
                       PLAYER_DAMAGE_START_Y + 2 * OFFSET_Y + TEXT_OFFSET_Y,
                       L"⚔UP (7MP)", c2);

            uint32_t c3 = player.IsMagicUsable(EMagic::Heal) ? COLOR_WHITE : COLOR_GRAY;
            renderService.DrawString(PLAYER_DAMAGE_START_X + TEXT_OFFSET_X,
                       PLAYER_DAMAGE_START_Y + 3 * OFFSET_Y + TEXT_OFFSET_Y,
                       L"回復 (5MP)", c3);
        }

        void PrintAttackMenu(const Player& player) const
        {
            renderService.DrawString(PLAYER_DAMAGE_START_X + TEXT_OFFSET_X,
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
                if (int offset = player.GetDamageOffset(); offset != 0)
                {
                    color = COLOR_RED;
                    message += std::format(L"+{}", offset);
                }
                renderService.DrawString(PLAYER_DAMAGE_START_X + TEXT_OFFSET_X,
                           PLAYER_DAMAGE_START_Y + (i + 1) * OFFSET_Y + TEXT_OFFSET_Y,
                           message.c_str(), color);
            }
        }

        void InitEnemyWeightDisplayers()
        {
            constexpr EAttackType types[3] = {EAttackType::Rock, EAttackType::Scissors, EAttackType::Paper};

            for (int i = 0; i < 3; ++i)
            {
                constexpr int offsetY[3] = {0, 1, 2};
                constexpr const wchar_t* icons[3] = {L"✊", L"✌", L"✋"};
                auto d = CreateLambdaDisplayer([this, t = types[i], icon = icons[i], row = offsetY[i]](float)
                {
                    const Enemy& enemy = characterService.GetEnemy();
                    std::wstring message = std::wstring(icon) + L"⚖：";
                    if (enemy.IsExposed())
                        message += std::to_wstring(enemy.GetBaseWeight());
                    else
                        message += L"?";
                    if (auto offset = enemy.GetWeightOffset(t); offset != 0)
                        message += std::format(L"+{}", offset);

                    renderService.DrawString(ENEMY_WEIGHT_START_X + TEXT_OFFSET_X,
                               ENEMY_WEIGHT_START_Y + row * OFFSET_Y + TEXT_OFFSET_Y,
                               message.c_str(), COLOR_WHITE);
                });
                weightDisplayers[i] = d.get();
                push_back(std::move(d));
            }
        }

        void PrintEnemyInfoWithoutWeight() const
        {
            const Enemy& enemy = characterService.GetEnemy();
            const auto enemyHealthComp = enemy.GetHealthComponent();

            renderService.DrawString(ENEMY_NAME_X, ENEMY_NAME_Y, enemy.GetName().c_str(), COLOR_WHITE);
            renderService.DrawString(ENEMY_HP_X, ENEMY_HP_Y,
                std::format(L"HP: {}/{}", enemyHealthComp.GetHealth(), enemyHealthComp.GetMaxHealth()).c_str(),
                COLOR_WHITE);

            DrawEnemyBoxes();
            DrawEnemyDamageValues(enemy);
        }

        void DrawEnemyBoxes() const
        {
            for (int i = 0; i < 3; ++i)
            {
                renderService.DrawHollowBox(
                    ENEMY_WEIGHT_START_X,               ENEMY_WEIGHT_START_Y + i * OFFSET_Y,
                    ENEMY_WEIGHT_START_X + RECT_X,      ENEMY_WEIGHT_START_Y + RECT_Y + i * OFFSET_Y,
                    THICKNESS, COLOR_WHITE);
                renderService.DrawHollowBox(
                    ENEMY_DAMAGE_START_X,               ENEMY_DAMAGE_START_Y + i * OFFSET_Y,
                    ENEMY_DAMAGE_START_X + RECT_X,      ENEMY_DAMAGE_START_Y + RECT_Y + i * OFFSET_Y,
                    THICKNESS, COLOR_WHITE);
            }
        }

        void DrawEnemyDamageValues(const Enemy& enemy) const
        {
            constexpr std::pair<EAttackType, const wchar_t*> rows[3] = {
                {EAttackType::Rock,     L"✊⚔："},
                {EAttackType::Scissors, L"✌⚔："},
                {EAttackType::Paper,    L"✋⚔："},
            };
            for (int i = 0; i < 3; ++i)
            {
                std::wstring msg = rows[i].second;
                msg += enemy.IsExposed()
                    ? std::to_wstring(enemy.GetBaseDamage(rows[i].first))
                    : L"?";
                renderService.DrawString(
                    ENEMY_DAMAGE_START_X + TEXT_OFFSET_X,
                    ENEMY_DAMAGE_START_Y + i * OFFSET_Y + TEXT_OFFSET_Y,
                    msg.c_str(), COLOR_WHITE);
            }
        }

        IBattleService& characterService;
        IRenderService& renderService;
        int currentFocus = 0;
        bool isMagicMenuOpen = false;
        EventHandle actionSelectionEvent;
        EventHandle addWeightEvent;
        // ウェイト表示 Displayer（インデックス: 0=Rock, 1=Scissors, 2=Paper）
        std::array<Displayer*, 3> weightDisplayers = {};
    };

    export std::unique_ptr<Displayer> CreateCharacterDisplayer(IBattleService& characterService,
                                                               IRenderService& renderService)
    {
        return std::make_unique<CharacterDisplayer>(characterService, renderService);
    }
} // namespace mc
