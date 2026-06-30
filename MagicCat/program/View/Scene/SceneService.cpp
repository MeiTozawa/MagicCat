module;

#include <unordered_map>
#include <memory>
#include <optional>
#include <RenderUtils.h>

module SceneService;

import EventBus;
import BattleService;
import EffectorFactory;
import DisplayerBase;

namespace mc {
    static constexpr int SCENE_FADE_DURATION_MS = 500;

    /// @brief シーン遷移フェード専用 Displayer。
    /// Effector が存在する間だけ全画面色塊を描画する。
    class ScreenFadeDisplayer : public Displayer
    {
        IRenderService& rs;

    public:
        explicit ScreenFadeDisplayer(IRenderService& rs) : rs(rs) {}

    private:
        void OnDraw(float) const override
        {
            const float w = static_cast<float>(rs.GetWindowWidth());
            const float h = static_cast<float>(rs.GetWindowHeight());
            rs.DrawBoxAA(0.f, 0.f, w, h, COLOR_BG, true);
        }

        // effector がない場合（フェード完了後）は何も描画しない
        void Draw(float deltaTime) const override
        {
            if (effectors.empty()) return;
            Displayer::Draw(deltaTime);
        }
    };

    class SceneService : public ISceneService
    {
        std::unordered_map<ESceneState, std::unique_ptr<IScene>> scenes;
        std::vector<ESceneState> sceneStack = {};
        bool initialized = false;

        IRenderService* renderService = nullptr;
        IInputService* inputService = nullptr;
        std::unique_ptr<Displayer> fadeDisplayer;

        std::optional<ESceneState> pendingScene;

        bool cutsceneContextPushed = false;

        EventHandle stageClearHandle;
        EventHandle stageFailHandle;
        EventHandle stageStartedHandle;
        EventHandle enemyDefeatedHandle;
        EventHandle cutsceneFinishedHandle;

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
            // 入力コンテキストはフェード（淡出）を開始する前に切り替える
            SetCutsceneInputContext(next == ESceneState::Cutscene);

            pendingScene = next;
            if (renderService)
            {
                fadeDisplayer = std::make_unique<ScreenFadeDisplayer>(*renderService);
                // 旧シーンを BG で覆う（alpha 0→255）。完了後にシーンを差し替える。
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
                // 新シーンを表示する（BG 覆いを alpha 255→0 で剥がす）。
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

    public:
        explicit SceneService(IRenderService* rs, IInputService* is = nullptr)
            : renderService(rs), inputService(is)
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
                sceneStack.pop_back();
        }

        void Update(float deltaTime) override
        {
            EnsureInitialized();

            if (fadeDisplayer)
                fadeDisplayer->Update(deltaTime);

            if (!sceneStack.empty())
                scenes[sceneStack.back()]->Update(deltaTime);

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
    };

    std::unique_ptr<ISceneService> CreateSceneService(IRenderService* renderService, IInputService* inputService)
    {
        return std::make_unique<SceneService>(renderService, inputService);
    }
} // namespace mc
