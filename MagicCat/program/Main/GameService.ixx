module;

export module GameService;

import SceneService;

namespace mc {

    export class IGameService
    {
    public:
        virtual ~IGameService() = default;
        virtual void Start() = 0;
        virtual void End() = 0;
        virtual void Update(float deltaTime) = 0;
    };

    export std::unique_ptr<IGameService> CreateGameService(ISceneService& sceneService);
}
