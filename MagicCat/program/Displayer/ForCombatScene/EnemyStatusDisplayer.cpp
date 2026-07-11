module;

#include <memory>
#include <vector>
#include <array>
#include <string>
#include <format>
#include <RenderUtils.h>

module Displayer;

import BattleService;
import RenderService;
import EventBus;
import EffectorFactory;
import AssetEnumMapper;
import HealthComponent;
import Character;
import Enemy;
import SceneService;

namespace mc {
    class EnemyStatusDisplayer : public DelegatingDisplayer
    {
    public:
        EnemyStatusDisplayer(IBattleService& character, IRenderService& render)
            : characterService(character), renderService(render)
        {
            displayers = CreateCompositeDisplayer();
            InitEnemyWeightDisplayers();
            displayers->push_back(CreateLambdaDisplayer([this](float) { PrintEnemyInfoWithoutWeight(); }));

            addWeightEvent = EventBus::Subscribe<AddWeightEvent>([this](const AddWeightEvent& e)
            {
                const int idx = static_cast<int>(e.AttackType);
                if (idx >= 0 && idx < 3 && weightDisplayers[idx])
                    weightDisplayers[idx]->AddEffector(CreateHitFlashEffector(renderService, COLOR_RED, 300));
            });
        }

        ~EnemyStatusDisplayer() override
        {
            EventBus::Unsubscribe(addWeightEvent);
        }

    protected:
        void OnUpdate(float deltaTime) override
        {
            if (displayers) displayers->Update(deltaTime);
        }

        void OnDraw(float deltaTime) const override
        {
            if (displayers) displayers->Draw(deltaTime);
        }

    private:
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
                displayers->push_back(std::move(d));
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
        EventHandle addWeightEvent;
        // ウェイト表示 Displayer（インデックス: 0=Rock, 1=Scissors, 2=Paper）
        std::array<IDisplayer*, 3> weightDisplayers = {};
        std::unique_ptr<ICompositeDisplayer> displayers;

    private:
        static constexpr int ENEMY_NAME_X = 1150;
        static constexpr int ENEMY_NAME_Y = 230;
        static constexpr int ENEMY_HP_X = 1500;
        static constexpr int ENEMY_HP_Y = 230;

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
    };

    std::unique_ptr<IDisplayer> CreateEnemyStatusDisplayer(
        IBattleService& characterService, IRenderService& renderService)
    {
        return std::make_unique<EnemyStatusDisplayer>(characterService, renderService);
    }
} // namespace mc
