module;

#include <memory>
#include <vector>
#include <algorithm>
#include <RenderUtils.h>

module SceneService;

import BattleService;
import AssetService;
import RenderService;
import EventBus;
import InputService;
import Displayer;

namespace mc {
    static constexpr float CUTSCENE_DURATION = 1.0f;
    static constexpr float SPRITE_EXT_RATE = 8.f;

    static constexpr int SLOT_COUNT = 3;
    static constexpr float SLOT_X_FRACTIONS[3] = {0.25f, 0.50f, 0.75f};
    static constexpr float SLOT_Y_FRACTION = 0.45f;
    static constexpr float BORDER_HALF_WIDTH = 96.f;
    static constexpr float BORDER_HALF_HEIGHT = 96.f;
    static constexpr int BORDER_CORNER_RADIUS = 16;
    static constexpr int BORDER_THICKNESS = 4;

    static Point SlotCenter(float screenW, float screenH, int slotIndex)
    {
        return {screenW * SLOT_X_FRACTIONS[slotIndex], screenH * SLOT_Y_FRACTION};
    }

    struct EnemySlotView
    {
        int slotIndex;
        ESprite sprite;
        int spriteHandle;
        SpriteInfo info;
        Point center;
    };

    class CutsceneScene : public IScene
    {
        IInputService& inputService;
        ISceneService& sceneService;
        IAssetService& assetService;
        IRenderService& renderService;
        IBattleService& battleService;

        std::vector<EnemySlotView> slots;
        std::unique_ptr<CutsceneFocusDisplayer> borderDisplayer;

        float timer = 0.f;
        bool finished = false;

    public:
        CutsceneScene(IInputService& inputService, ISceneService& sceneService,
                      IAssetService& assetService, IRenderService& renderService,
                      IBattleService& battleService)
            : inputService(inputService)
              , sceneService(sceneService)
              , assetService(assetService)
              , renderService(renderService)
              , battleService(battleService) {}

        void Start() override
        {
            inputService.PushContext(InputContext::CutScene);

            const float screenW = static_cast<float>(renderService.GetWindowWidth());
            const float screenH = static_cast<float>(renderService.GetWindowHeight());

            timer = 0.f;
            finished = false;

            const auto& sequence = battleService.GetSequence();
            int rawIndex = battleService.GetCurrentEnemyIndex();

            const int slotCount = std::min(SLOT_COUNT, static_cast<int>(sequence.size()));
            const int currentIdx = std::clamp(rawIndex, 0, SLOT_COUNT - 1);

            slots.clear();
            for (int i = 0; i < slotCount; ++i)
            {
                EnemySlotView sv;
                sv.slotIndex = i;
                sv.sprite = assetService.ParseSprite(sequence[i].spriteName);
                sv.spriteHandle = assetService.GetSpriteHandle(sv.sprite);
                sv.info = assetService.GetSpriteInfo(sv.sprite);
                sv.center = SlotCenter(screenW, screenH, i);
                slots.push_back(sv);
            }

            const int srcIdx = std::max(0, currentIdx - 1);
            borderDisplayer = CreateCutsceneFocusDisplayer(
                renderService,
                SlotCenter(screenW, screenH, srcIdx),
                SlotCenter(screenW, screenH, currentIdx),
                BORDER_HALF_WIDTH, BORDER_HALF_HEIGHT,
                BORDER_CORNER_RADIUS, BORDER_THICKNESS);
        }

        void Update(float dt) override
        {
            if (!finished)
                timer += dt;

            for (const auto& sv : slots)
            {
                const float cx = sv.center.x;
                const float cy = sv.center.y;

                if (sv.spriteHandle >= 0 && sv.sprite != ESprite::Null)
                {
                    renderService.DrawRectRotaGraph(
                        static_cast<int>(cx), static_cast<int>(cy),
                        0, 0, sv.info.size.x, sv.info.size.y,
                        SPRITE_EXT_RATE, 0.0, sv.spriteHandle, true, false);
                }
                else
                {
                    renderService.DrawBoxAA(cx - 48.f, cy - 48.f,
                                            cx + 48.f, cy + 48.f, 0x808080, true);
                }
            }

            borderDisplayer->Update(dt);
            borderDisplayer->Draw(dt);

            if (!finished && timer >= CUTSCENE_DURATION)
            {
                finished = true;
                inputService.PopContext();
                EventBus::Publish(CutsceneFinishedEvent{});
            }
        }
    };

    std::unique_ptr<IScene> CreateCutsceneScene(
        IInputService& inputService,
        ISceneService& sceneService,
        IAssetService& assetService,
        IRenderService& renderService,
        IBattleService& battleService)
    {
        return std::make_unique<CutsceneScene>(
            inputService, sceneService, assetService, renderService, battleService);
    }
} // namespace mc
