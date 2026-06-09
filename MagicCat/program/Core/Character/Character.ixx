module;

export module Character;

export enum class EAttackType
{
    Rock, Scissors, Paper
};

export struct Character
{
    int rockAttack = 1;
    int scissorsAttack = 1;
    int paperAttack = 1;
};
