module;

#include <unordered_map>
#include <memory>
#include <optional>
#include <RenderUtils.h>
#include <dxe.h>

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

    protected:
        void OnDraw(float) const override
        {
            const float w = dxe::GetWindowWidthF(1.f);
            const float h = dxe::GetWindowHeightF(1.f);
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
        std::unique_ptr<Displayer> fadeDisplayer;

        std::optional<ESceneState> pendingScene;

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

        void TransitionTo(ESceneState next)
        {
            pendingScene = next;
            if (renderService)
            {
                fadeDisplayer = std::make_unique<ScreenFadeDisplayer>(*renderService);
                fadeDisplayer->AddEffector(
                    CreateFadeOutEffector(SCENE_FADE_DURATION_MS),
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
                fadeDisplayer->AddEffector(CreateFadeInEffector(SCENE_FADE_DURATION_MS));
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
        explicit SceneService(IRenderService* rs) : renderService(rs)
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
            sceneStack.clear();
            sceneStack.push_back(state);
        }
    };

    std::unique_ptr<ISceneService> CreateSceneService(IRenderService* renderService)
    {
        return std::make_unique<SceneService>(renderService);
    }
} // namespace mc
