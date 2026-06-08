module;

export module Character;

export enum class EAttackType
{
    Rock, Scissors, Paper
};

export struct Character
{
    int rockAttack = 0;
    int scissorsAttack = 0;
    int paperAttack = 0;
};
