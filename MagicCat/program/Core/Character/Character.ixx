module;

export module Character;

export enum class EAttackType
{
    Rock, Scissors, Paper
};

export class Character
{
protected:
    int rockAttack = 0;
    int scissorsAttack = 0;
    int paperAttack = 0;
    
    virtual ~Character() = default;
};
