module;

export module GameService; 

export enum EGameState
{
    START, COMBAT, RESULT
};

export class IGameService
{
    public:
    virtual ~IGameService() = default;
    virtual void Start() = 0;
    virtual void End() = 0;
    virtual void Update(float deltaTime) = 0;
    
};