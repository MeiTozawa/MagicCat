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
import ViewEnumMapper;
import HealthComponent;
import Character;
import Enemy;
import Player;
import SceneService;

namespace mc {
    namespace {
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
        // ウェイト表示 Displayer（インデックス: 0=Rock, 1=Scissors, 2=Paper）
        std::array<Displayer*, 3> weightDisplayers = {};

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
            else
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
        }

        void InitEnemyWeightDisplayers()
        {
            // Rock=0, Scissors=1, Paper=2 の順で Displayer を生成する
            // raw pointer を weightDisplayers に保持し、AddEffector 呼び出しに使う
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

            auto message = enemy.GetName();
            renderService.DrawString(ENEMY_NAME_X, ENEMY_NAME_Y, message.c_str(), COLOR_WHITE);

            message = std::format(L"HP: {}/{}", enemyHealthComp.GetHealth(), enemyHealthComp.GetMaxHealth());
            renderService.DrawString(ENEMY_HP_X, ENEMY_HP_Y, message.c_str(), COLOR_WHITE);

            for (int i = 0; i < 3; ++i)
            {
                float x1 = ENEMY_WEIGHT_START_X;
                float y1 = ENEMY_WEIGHT_START_Y + i * OFFSET_Y;
                float x2 = ENEMY_WEIGHT_START_X + RECT_X;
                float y2 = ENEMY_WEIGHT_START_Y + RECT_Y + i * OFFSET_Y;

                renderService.DrawHollowBox(x1, y1, x2, y2, THICKNESS, COLOR_WHITE);
            }

            for (int i = 0; i < 3; ++i)
            {
                float x1 = ENEMY_DAMAGE_START_X;
                float y1 = ENEMY_DAMAGE_START_Y + i * OFFSET_Y;
                float x2 = ENEMY_DAMAGE_START_X + RECT_X;
                float y2 = ENEMY_DAMAGE_START_Y + RECT_Y + i * OFFSET_Y;

                renderService.DrawHollowBox(x1, y1, x2, y2, THICKNESS, COLOR_WHITE);
            }

            if (enemy.IsExposed())
            {
                renderService.DrawString(ENEMY_DAMAGE_START_X + TEXT_OFFSET_X,
                           ENEMY_DAMAGE_START_Y + 0 * OFFSET_Y + TEXT_OFFSET_Y,
                           std::format(L"✊⚔：{}", enemy.GetBaseDamage(EAttackType::Rock)).c_str(), COLOR_WHITE);
                renderService.DrawString(ENEMY_DAMAGE_START_X + TEXT_OFFSET_X,
                           ENEMY_DAMAGE_START_Y + 1 * OFFSET_Y + TEXT_OFFSET_Y,
                           std::format(L"✌⚔：{}", enemy.GetBaseDamage(EAttackType::Scissors)).c_str(), COLOR_WHITE);
                renderService.DrawString(ENEMY_DAMAGE_START_X + TEXT_OFFSET_X,
                           ENEMY_DAMAGE_START_Y + 2 * OFFSET_Y + TEXT_OFFSET_Y,
                           std::format(L"✋⚔：{}", enemy.GetBaseDamage(EAttackType::Paper)).c_str(), COLOR_WHITE);
            }
            else
            {
                renderService.DrawString(ENEMY_DAMAGE_START_X + TEXT_OFFSET_X,
                           ENEMY_DAMAGE_START_Y + 0 * OFFSET_Y + TEXT_OFFSET_Y,
                           L"✊⚔：?", COLOR_WHITE);
                renderService.DrawString(ENEMY_DAMAGE_START_X + TEXT_OFFSET_X,
                           ENEMY_DAMAGE_START_Y + 1 * OFFSET_Y + TEXT_OFFSET_Y,
                           L"✌⚔：?", COLOR_WHITE);
                renderService.DrawString(ENEMY_DAMAGE_START_X + TEXT_OFFSET_X,
                           ENEMY_DAMAGE_START_Y + 2 * OFFSET_Y + TEXT_OFFSET_Y,
                           L"✋⚔：?", COLOR_WHITE);
            }
        }
    };

    export std::unique_ptr<Displayer> CreateCharacterDisplayer(IBattleService& characterService,
                                                               IRenderService& renderService)
    {
        return std::make_unique<CharacterDisplayer>(characterService, renderService);
    }
} // namespace mc
