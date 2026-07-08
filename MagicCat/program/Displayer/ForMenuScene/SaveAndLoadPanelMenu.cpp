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

namespace {
    constexpr int BOX_MARGIN_X   = 100;
    constexpr int LINE_SPACING   = 60;
    constexpr int INDENT_LEVEL_1 = 50;
    constexpr int INDENT_LEVEL_2 = 80;
    constexpr int INFO_OFFSET    = 200;

    constexpr const wchar_t* SLOT_LABELS[SAVE_SLOT_COUNT] = {
        L"オート", L"スロット 1", L"スロット 2", L"スロット 3"
    };
} // anonymous namespace

class SaveAndLoadPanel : public MenuPanel {
public:
    SaveAndLoadPanel(int boxX1, int textY, SlotPanelMode mode,
                     IInputService& input, IRenderService& render,
                     IBattleService& battle, IConfigService& config,
                     ISceneService& scene, IOSService& os)
        : MenuPanel(boxX1, textY)
        , mode(mode)
        , inputService(input)
        , renderService(render)
        , battleService(battle)
        , configService(config)
        , sceneService(scene)
        , slotGroup(BuildSlotRects(textY), input, os, ButtonGroupLayout::Vertical)
    {}

    void Activate() override
    {
        MenuPanel::Activate();
        slotGroup.SetFocusedIndex(0);
        if (mode == SlotPanelMode::Load)
            RefreshSlotMeta();
    }

    void Deactivate() override
    {
        MenuPanel::Deactivate();
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
                {
                    RefreshSlotMeta();
                    Deactivate();
                }
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
        const wchar_t* label = (mode == SlotPanelMode::Save) ? L"セーブ" : L"ロード";
        renderService.DrawString(x + INDENT_LEVEL_1, y, label, COLOR_TEXT_NORMAL);

        for (int i = 0; i < SAVE_SLOT_COUNT; ++i)
        {
            const int      rowY    = y + LINE_SPACING * (i + 1);
            const bool     focused = (slotGroup.GetFocusedIndex() == i);
            const uint32_t col     = focused ? COLOR_TEXT_GREEN : COLOR_TEXT_NORMAL;
            const auto&    meta    = slotMeta[i];

            renderService.DrawString(x + INDENT_LEVEL_2, rowY, SLOT_LABELS[i], col);

            if (meta.exists)
            {
                wchar_t buf[128];
                swprintf_s(buf, L"第%d場戦闘 / 共%d場  プレイヤーHP:%d/%d  敵HP:%d/%d",
                           meta.currentBattle, meta.totalBattles,
                           meta.playerHp, meta.playerMaxHp,
                           meta.enemyHp, meta.enemyMaxHp);
                renderService.DrawString(x + INDENT_LEVEL_2 + INFO_OFFSET, rowY, buf, col);
            }
            else
            {
                renderService.DrawString(x + INDENT_LEVEL_2 + INFO_OFFSET, rowY, L"（空）", col);
            }
        }
    }

    void RefreshSlotMeta()
    {
        for (int i = 0; i < SAVE_SLOT_COUNT; ++i)
            slotMeta[i] = configService.GetSaveMetadata(i);
    }

    static std::array<Rect<int>, SAVE_SLOT_COUNT> BuildSlotRects(int textY)
    {
        std::array<Rect<int>, SAVE_SLOT_COUNT> rects{};
        for (int i = 0; i < SAVE_SLOT_COUNT; ++i)
        {
            const int y1 = textY + LINE_SPACING * (i + 1);
            rects[i] = { BOX_MARGIN_X + INDENT_LEVEL_2, y1,
                         WINDOW_WIDTH - BOX_MARGIN_X,    y1 + LINE_SPACING };
        }
        return rects;
    }

    SlotPanelMode   mode;
    IInputService&  inputService;
    IRenderService& renderService;
    IBattleService& battleService;
    IConfigService& configService;
    ISceneService&  sceneService;

    ButtonGroup slotGroup;
    std::array<SaveMetadata, SAVE_SLOT_COUNT> slotMeta{};
};

std::unique_ptr<MenuPanel> CreateSavePanel(int boxX1, int textY,
                                            IInputService& input,
                                            IRenderService& render,
                                            IBattleService& battle,
                                            IConfigService& config,
                                            ISceneService& scene,
                                            IOSService& os)
{
    return std::make_unique<SaveAndLoadPanel>(boxX1, textY, SlotPanelMode::Save,
                                              input, render, battle, config, scene, os);
}

std::unique_ptr<MenuPanel> CreateLoadPanel(int boxX1, int textY,
                                            IInputService& input,
                                            IRenderService& render,
                                            IBattleService& battle,
                                            IConfigService& config,
                                            ISceneService& scene,
                                            IOSService& os)
{
    return std::make_unique<SaveAndLoadPanel>(boxX1, textY, SlotPanelMode::Load,
                                              input, render, battle, config, scene, os);
}

} // namespace mc
