module;

#include <random>

export module Enmey;

import Character;
import HealthComponent;


export class Enemy : public Character, IDamageable
{
public:
    Enemy(float baseProbability)
        : _baseProbability(baseProbability)
    {
        _rockProbability = baseProbability;
        _scissorsProbability = baseProbability;
        _paperProbability = baseProbability;
        
        std::random_device rd;
        _rng = std::mt19937(rd());
    }
    
    void AddRockProbability(int v)
    {
        _rockProbability += v;
    }
    
    void AddScissorsProbability(int v)
    {
        _scissorsProbability += v;
    }
    
    void AddPaperProbability(int v)
    {
        _paperProbability += v;
    }

private:
    int _baseProbability = 0;
    int _rockProbability = 0;
    int _scissorsProbability = 0;
    int _paperProbability = 0;
    std::mt19937 _rng;
};
