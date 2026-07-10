module;

#include <array>
#include <memory>
#include <RenderUtils.h>

module Displayer;

import InputService;
import RenderService;
import BattleService;
import ConfigService;
import SceneService;
import OSService;
import ButtonGroup;

namespace mc {
    class SaveAndLoadPanel : public MenuPanel
    {
    private:
        static constexpr int RECT_WIDTH  = WINDOW_WIDTH - MENU_BOX_MARGIN_X * 2 - 100;
        static constexpr int RECT_HEIGHT = 100;

        static constexpr int START_X = MENU_BOX_MARGIN_X + 50;
        static constexpr int START_Y = MENU_TEXT_Y;

        static constexpr int OFFSET_Y = RECT_HEIGHT + 80;

        static constexpr int TEXT_CENTER_X = RECT_WIDTH / 2;
        static constexpr int TEXT_CENTER_Y = RECT_HEIGHT / 2;

        static constexpr int INFO_X        = 260; 
        static constexpr int RECT_EXPAND   = 20;  

        static constexpr const wchar_t* SLOT_LABELS[SAVE_SLOT_COUNT] = {
            L"オート", L"スロット 1", L"スロット 2", L"スロット 3"
        };

        // constexpr でスロット矩形を全計算（VolumePanel と同パターン）
        static constexpr auto SLOT_RECTS = []()
        {
            std::array<Rect<int>, SAVE_SLOT_COUNT> rects{};
            for (int i = 0; i < SAVE_SLOT_COUNT; ++i)
            {
                rects[i] = {
                    START_X,
                    START_Y + OFFSET_Y * i,
                    START_X + RECT_WIDTH,
                    START_Y + OFFSET_Y * i + RECT_HEIGHT
                };
            }
            return rects;
        }();

    public:
        SaveAndLoadPanel(SlotPanelMode mode,
                         IInputService& input, IRenderService& render,
                         IBattleService& battle, IConfigService& config,
                         ISceneService& scene, IOSService& os)
            : MenuPanel()
              , mode(mode)
              , inputService(input)
              , renderService(render)
              , battleService(battle)
              , configService(config)
              , sceneService(scene)
              , slotGroup(SLOT_RECTS, input, os, ButtonGroupLayout::Vertical)
        {
            slotGroup.SetFocusedIndex(0);
            RefreshSlotMeta();
        }

    private:
        void OnUpdate(float /*deltaTime*/) override
        {
            slotGroup.Update();

            if (auto confirmed = slotGroup.ConsumeConfirm())
            {
                const int slot = *confirmed;
                if (mode == SlotPanelMode::Save)
                {
                    if (battleService.SaveState(slot))
                        RefreshSlotMeta();
                }
                else // Load
                {
                    if (battleService.LoadState(slot))
                        sceneService.SetCurrentScene(ESceneState::Combat);
                }
            }
        }

        void OnDraw(float /*deltaTime*/) const override
        {
            for (int i = 0; i < SAVE_SLOT_COUNT; ++i)
            {
                const bool isFocused = (slotGroup.GetFocusedIndex() == i);
                const auto& rect     = SLOT_RECTS[i];

                renderService.DrawButton(rect.Expand(RECT_EXPAND), L"",
                                         isFocused, COLOR_BG, COLOR_WHITE);

                const uint32_t fgColor = isFocused ? COLOR_YELLOW : COLOR_WHITE;
                renderService.DrawCenterString(
                    rect.x1 + TEXT_CENTER_X / 4,
                    rect.y1 + TEXT_CENTER_Y,
                    SLOT_LABELS[i], fgColor);

                const auto& meta = slotMeta[i];
                if (meta.exists)
                {
                    auto s = std::format(L"第{}/{}場  HP:{}/{}  敵HP:{}/{}",
                               meta.currentBattle, meta.totalBattles,
                               meta.playerHp, meta.playerMaxHp,
                               meta.enemyHp, meta.enemyMaxHp);
                    renderService.DrawCenterString(
                        rect.x1 + INFO_X + (RECT_WIDTH - INFO_X) / 2,
                        rect.y1 + TEXT_CENTER_Y,
                        s.c_str(), fgColor);
                }
                else
                {
                    renderService.DrawCenterString(
                        rect.x1 + INFO_X + (RECT_WIDTH - INFO_X) / 2,
                        rect.y1 + TEXT_CENTER_Y,
                        L"（空）", COLOR_GRAY);
                }
            }
        }

        void RefreshSlotMeta()
        {
            for (int i = 0; i < SAVE_SLOT_COUNT; ++i)
                slotMeta[i] = configService.GetSaveMetadata(i);
        }

        SlotPanelMode mode;
        IInputService& inputService;
        IRenderService& renderService;
        IBattleService& battleService;
        IConfigService& configService;
        ISceneService& sceneService;

        ButtonGroup slotGroup;
        std::array<SaveMetadata, SAVE_SLOT_COUNT> slotMeta{};
    };

    std::unique_ptr<MenuPanel> CreateSavePanel(IInputService& input,
                                               IRenderService& render,
                                               IBattleService& battle,
                                               IConfigService& config,
                                               ISceneService& scene,
                                               IOSService& os)
    {
        return std::make_unique<SaveAndLoadPanel>(SlotPanelMode::Save,
                                                  input, render, battle, config, scene, os);
    }

    std::unique_ptr<MenuPanel> CreateLoadPanel(IInputService& input,
                                               IRenderService& render,
                                               IBattleService& battle,
                                               IConfigService& config,
                                               ISceneService& scene,
                                               IOSService& os)
    {
        return std::make_unique<SaveAndLoadPanel>(SlotPanelMode::Load,
                                                  input, render, battle, config, scene, os);
    }
} // namespace mc
