module;

export module GameService; 

export enum EGameState
{
    START, COMBAT
};

class IGameService
{
    public:
    virtual ~IGameService();
    
};