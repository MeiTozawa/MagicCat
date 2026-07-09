module;

#include <algorithm>
#include <app_build_setting.h>
#include <unordered_map>
#include <memory>
#include <optional>
#include <RenderUtils.h>

module SceneService;

import EventBus;
import BattleService;
import EffectorFactory;
import DisplayerBase;
import AssetService;

namespace mc {
    /// @brief シーン遷移フェード専用 Displayer。
    /// Effector が存在する間だけ全画面色塊を描画する。
    class ScreenFadeDisplayer : public Displayer
    {
    public:
        explicit ScreenFadeDisplayer(IRenderService& rs) : renderService(rs) {}

        void Draw(float deltaTime) const override
        {
            if (effectors.empty()) return;
            Displayer::Draw(deltaTime);
        }

    private:
        void OnDraw(float) const override
        {
            renderService.DrawBoxAA(0.f, 0.f, WINDOW_WIDTH, WINDOW_HEIGHT, COLOR_BG, true);
        }

        IRenderService& renderService;
    };

    class SceneService : public ISceneService
    {
    private:
        static constexpr int SCENE_FADE_DURATION_MS = 500;

    public:
        explicit SceneService(IRenderService* rs, IInputService* is = nullptr,
                              IAssetService* as = nullptr, IOSService* os = nullptr)
            : renderService(rs), inputService(is), assetService(as), osService(os)
        {
            stageClearHandle = EventBus::Subscribe<StageClearEvent>([this](const StageClearEvent&)
            {
                TransitionTo(ESceneState::Info);
            });
            stageFailHandle = EventBus::Subscribe<StageFailEvent>([this](const StageFailEvent&)
            {
                TransitionTo(ESceneState::Info);
            });
            stageStartedHandle = EventBus::Subscribe<StageStartedEvent>([this](const StageStartedEvent&)
            {
                TransitionTo(ESceneState::Cutscene);
            });
            enemyDefeatedHandle = EventBus::Subscribe<EnemyDefeatedEvent>([this](const EnemyDefeatedEvent&)
            {
                TransitionTo(ESceneState::Cutscene);
            });
            cutsceneFinishedHandle = EventBus::Subscribe<CutsceneFinishedEvent>([this](const CutsceneFinishedEvent&)
            {
                TransitionTo(ESceneState::Combat);
            });
        }

        ~SceneService() override
        {
            EventBus::Unsubscribe(stageClearHandle);
            EventBus::Unsubscribe(stageFailHandle);
            EventBus::Unsubscribe(stageStartedHandle);
            EventBus::Unsubscribe(enemyDefeatedHandle);
            EventBus::Unsubscribe(cutsceneFinishedHandle);
        }

        void RegisterScene(ESceneState type, std::unique_ptr<IScene>&& scene) override
        {
            scenes[type] = std::move(scene);
        }

        void PushScene(ESceneState type) override
        {
            EnsureInitialized();
            if (scenes.contains(type))
            {
                sceneStack.push_back(type);
                scenes[type]->Start();
            }
        }

        void PopScene() override
        {
            EnsureInitialized();
            if (sceneStack.size() > 1)
            {
                if (sceneStack.back() == ESceneState::Menu)
                    menuJustClosed = true;
                sceneStack.pop_back();
            }
        }

        void Update(float deltaTime) override
        {
            EnsureInitialized();

            if (fadeDisplayer)
                fadeDisplayer->Update(deltaTime);

            if (!sceneStack.empty())
                scenes[sceneStack.back()]->Update(deltaTime);
            
            HandleMenuButton(deltaTime);

            if (fadeDisplayer)
                fadeDisplayer->Draw(deltaTime);
        }

        ESceneState GetCurrentScene() override
        {
            return sceneStack.empty() ? ESceneState::Info : sceneStack.back();
        }

        void SetCurrentScene(ESceneState state) override
        {
            if (!scenes.contains(state) || !scenes[state]) return;

            sceneStack.clear();
            sceneStack.push_back(state);
            scenes[state]->Start();
            StartFadeIn();
        }

    private:
        void HandleMenuButton(float deltaTime)
        {
            if (!inputService || !renderService) return;

            const bool hasCutscene = std::ranges::any_of(sceneStack, [](ESceneState s)
            {
                return s == ESceneState::Cutscene;
            });
            if (hasCutscene) return;

            const bool isMenuOpen = std::ranges::any_of(sceneStack, [](ESceneState s)
            {
                return s == ESceneState::Menu;
            });

            if (assetService)
            {
                const int iconHandle = assetService->GetImageHandle(EImage::BUTTON_MENU);
                if (iconHandle >= 0)
                    renderService->DrawRotaGraphF(
                        static_cast<float>(MENU_ICON_X),
                        static_cast<float>(MENU_ICON_Y),
                        0.8, 0.0, iconHandle, true);
            }

            if (osService && inputService->GetActiveDevice() == InputDevice::Mouse
                && inputService->IsMouseOver(MENU_ICON_RECT))
                osService->SetCursorPointer();

            const auto click = inputService->OnMouseClick(InputAction::MouseClick);
            const bool iconClicked = click.x != -1 && click.y != -1 && click.In(MENU_ICON_RECT);

            if (isMenuOpen)
            {
                if (iconClicked)
                {
                    inputService->PopContext();
                    PopScene();
                }
            }
            else
            {
                if (menuJustClosed)
                {
                    menuJustClosed = false;
                    return;
                }

                if (inputService->IsPressed(InputAction::ToggleMenu))
                {
                    PushScene(ESceneState::Menu);
                    return;
                }

                if (iconClicked)
                {
                    PushScene(ESceneState::Menu);
                }
            }
        }
        
        void EnsureInitialized()
        {
            if (!initialized)
            {
                initialized = true;
                if (!scenes.empty() && scenes.contains(ESceneState::Info))
                {
                    sceneStack.push_back(ESceneState::Info);
                    scenes[ESceneState::Info]->Start();
                    StartFadeIn();
                }
            }
        }

        /// @brief Cutscene 用の入力コンテキストをフェード開始前に切り替える。
        /// 押し込み／取り出しが対になるようフラグで管理する。
        void SetCutsceneInputContext(bool entering)
        {
            if (!inputService) return;
            if (entering && !cutsceneContextPushed)
            {
                inputService->PushContext(InputContext::Cutscene);
                cutsceneContextPushed = true;
            }
            else if (!entering && cutsceneContextPushed)
            {
                inputService->PopContext();
                cutsceneContextPushed = false;
            }
        }

        void TransitionTo(ESceneState next)
        {
            SetCutsceneInputContext(next == ESceneState::Cutscene);

            pendingScene = next;
            if (renderService)
            {
                fadeDisplayer = std::make_unique<ScreenFadeDisplayer>(*renderService);
                fadeDisplayer->AddEffector(
                    CreateFadeInEffector(*renderService, SCENE_FADE_DURATION_MS),
                    [this]() { ApplyPendingTransition(); }
                );
                fadeDisplayer->Play();
            }
            else
            {
                ApplyPendingTransition();
            }
        }

        void StartFadeIn()
        {
            if (renderService)
            {
                fadeDisplayer = std::make_unique<ScreenFadeDisplayer>(*renderService);
                fadeDisplayer->AddEffector(CreateFadeOutEffector(*renderService, SCENE_FADE_DURATION_MS));
                fadeDisplayer->Play();
            }
        }

        void ApplyPendingTransition()
        {
            if (!pendingScene) return;
            const ESceneState next = *pendingScene;
            pendingScene.reset();

            sceneStack.clear();
            sceneStack.push_back(next);

            if (scenes.contains(next) && scenes[next])
                scenes[next]->Start();

            StartFadeIn();
        }

        std::unordered_map<ESceneState, std::unique_ptr<IScene>> scenes;
        std::vector<ESceneState> sceneStack = {};
        bool initialized = false;

        IRenderService* renderService = nullptr;
        IInputService* inputService = nullptr;
        IAssetService* assetService = nullptr;
        IOSService* osService = nullptr;
        std::unique_ptr<Displayer> fadeDisplayer;

        std::optional<ESceneState> pendingScene;

        bool cutsceneContextPushed = false;
        bool menuJustClosed = false;

        EventHandle stageClearHandle;
        EventHandle stageFailHandle;
        EventHandle stageStartedHandle;
        EventHandle enemyDefeatedHandle;
        EventHandle cutsceneFinishedHandle;
    };

    std::unique_ptr<ISceneService> CreateSceneService(IRenderService* renderService, IInputService* inputService,
                                                      IAssetService* assetService, IOSService* osService)
    {
        return std::make_unique<SceneService>(renderService, inputService, assetService, osService);
    }
} // namespace mc
