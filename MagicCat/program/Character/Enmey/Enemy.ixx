module;

#include <random>

export module Enmey;

import Character;
import HealthComponent;


export class Enemy : public Character, public HealthComponent
{
public:
    Enemy(int baseProbability)
        : baseProbability(baseProbability)
    {
        rockProbability = baseProbability;
        scissorsProbability = baseProbability;
        paperProbability = baseProbability;
        
        std::random_device rd;
        rng = std::mt19937(rd());
    }
    
    void AddRockProbability(int v)
    {
        rockProbability += v;
    }
    
    void AddScissorsProbability(int v)
    {
        scissorsProbability += v;
    }
    
    void AddPaperProbability(int v)
    {
        paperProbability += v;
    }
    
    void Attack()
    {
        
    }

private:
    int baseProbability = 0;
    int rockProbability = 0;
    int scissorsProbability = 0;
    int paperProbability = 0;
    std::mt19937 rng;
};
