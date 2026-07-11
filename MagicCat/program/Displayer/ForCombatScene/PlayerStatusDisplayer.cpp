module;

#include <memory>
#include <vector>
#include <string>
#include <format>
#include <RenderUtils.h>

module Displayer;

import BattleService;
import RenderService;
import EventBus;
import AssetEnumMapper;
import HealthComponent;
import Character;
import Player;
import SceneService;

namespace mc {
    class PlayerStatusDisplayer : public DelegatingDisplayer
    {
    public:
        PlayerStatusDisplayer(IBattleService& character, IRenderService& render)
            : characterService(character), renderService(render)
        {
            displayers = CreateCompositeDisplayer();
            displayers->push_back(CreateLambdaDisplayer([this](float) { PrintPlayerInfo(); }));
            displayers->push_back(CreateLambdaDisplayer([this](float) { PrintPlayerActions(currentFocus); }));

            actionSelectionEvent = EventBus::Subscribe<ActionSelectionEvent>([this](const ActionSelectionEvent& e)
            {
                currentFocus = e.selectedIndex;
                isMagicMenuOpen = e.isMagicMenuOpen;
            });
        }

        ~PlayerStatusDisplayer() override
        {
            EventBus::Unsubscribe(actionSelectionEvent);
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

        IBattleService& characterService;
        IRenderService& renderService;
        int currentFocus = 0;
        bool isMagicMenuOpen = false;
        EventHandle actionSelectionEvent;
        std::unique_ptr<ICompositeDisplayer> displayers;

    private:
        static constexpr int PLAYER_DAMAGE_START_X = ACTION_MENU_X;
        static constexpr int PLAYER_DAMAGE_START_Y = ACTION_MENU_Y;

        static constexpr int PLAYER_HP_X = 60;
        static constexpr int PLAYER_HP_Y = 200;
        static constexpr int PLAYER_MP_X = 60;
        static constexpr int PLAYER_MP_Y = 300;

        static constexpr int TEXT_OFFSET_X = 30;
        static constexpr int TEXT_OFFSET_Y = 28;

        static constexpr int OFFSET_Y = ACTION_MENU_STEP_Y;

        static constexpr int RECT_X = ACTION_MENU_W;
        static constexpr int RECT_Y = ACTION_MENU_H;

        static constexpr int THICKNESS = 2;
    };

    std::unique_ptr<IDisplayer> CreatePlayerStatusDisplayer(
        IBattleService& characterService, IRenderService& renderService)
    {
        return std::make_unique<PlayerStatusDisplayer>(characterService, renderService);
    }
} // namespace mc
