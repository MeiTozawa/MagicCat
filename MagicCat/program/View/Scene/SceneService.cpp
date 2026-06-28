module;

#include <unordered_map>
#include <memory>

module SceneService;

import EventBus;
import BattleService;

namespace mc
{
    class SceneService : public ISceneService
    {
        std::unordered_map<ESceneState, std::unique_ptr<IScene>> scenes;
        std::vector<ESceneState> sceneStack = {};
        bool initialized = false;
        EventHandle stageClearHandle;
        EventHandle stageFailHandle;
        EventHandle stageStartedHandle;

        /**
         * @brief 依存関係の遅延初期化（Lazy Initialization）を行います。
         * SceneServiceのコンストラクタ実行時点では、ServiceLocatorにすべてのサービス
         * （例: CharacterService, InputServiceなど）が登録されていない可能性があるため、
         * 実際にシーンが操作されるタイミングで一度だけ初期化を実行します。
         */
        void EnsureInitialized()
        {
            if (!initialized)
            {
                initialized = true;
                if (!scenes.empty() && scenes.contains(ESceneState::Info))
                {
                    sceneStack.push_back(ESceneState::Info);
                    scenes[ESceneState::Info]->Start();
                }
            }
        }

    public:
        SceneService()
        {
            stageClearHandle = EventBus::Subscribe<StageClearEvent>([this](const StageClearEvent&) {
                SetCurrentScene(ESceneState::Info);
                if (scenes.contains(ESceneState::Info) && scenes[ESceneState::Info])
                    scenes[ESceneState::Info]->Start();
            });
            stageFailHandle = EventBus::Subscribe<StageFailEvent>([this](const StageFailEvent&) {
                SetCurrentScene(ESceneState::Info);
                if (scenes.contains(ESceneState::Info) && scenes[ESceneState::Info])
                    scenes[ESceneState::Info]->Start();
            });
            stageStartedHandle = EventBus::Subscribe<StageStartedEvent>([this](const StageStartedEvent&) {
                PushScene(ESceneState::Combat);
            });
        }

        ~SceneService() override
        {
            EventBus::Unsubscribe(stageClearHandle);
            EventBus::Unsubscribe(stageFailHandle);
            EventBus::Unsubscribe(stageStartedHandle);
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
                sceneStack.pop_back();
            }
        }

        void Update(float deltaTime) override
        {
            EnsureInitialized();
            if (!sceneStack.empty())
            {
                scenes[sceneStack.back()]->Update(deltaTime);
            }
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

    std::unique_ptr<ISceneService> CreateSceneService()
    {
        return std::make_unique<SceneService>();
    }
} // namespace mc
