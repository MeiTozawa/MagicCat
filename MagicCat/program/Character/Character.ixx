module;

export module Character;
import EventBus;

namespace mc {

export enum class EAttackType
{
    Rock, Scissors, Paper,
};

export bool Fail(EAttackType l, EAttackType r)
{
    return (l == EAttackType::Rock && r == EAttackType::Paper) ||
        (l == EAttackType::Scissors && r == EAttackType::Rock) ||
        (l == EAttackType::Paper && r == EAttackType::Scissors);
}

export struct CombatEvent : IEvent
{
    EAttackType playerAttackType;
    EAttackType enemyAttackType;
    int playerAttackDamage;
    int enemyAttackDamage;

    CombatEvent(EAttackType playerAttackType, EAttackType enemyAttackType,
                int playerAttackDamage, int enemyAttackDamage) :
        playerAttackType(playerAttackType), enemyAttackType(enemyAttackType),
        playerAttackDamage(playerAttackDamage), enemyAttackDamage(enemyAttackDamage) {}
};

export struct Character
{
    int rockDamage = 1;
    int scissorsDamage = 1;
    int paperDamage = 1;
    
    int GetDamage(EAttackType t) const
    {
        switch (t)
        {
        case EAttackType::Rock:
            return rockDamage;
        case EAttackType::Scissors:
            return scissorsDamage;
        case EAttackType::Paper:
            return paperDamage;
        }
        return -1;
    }
};

} // namespace mc

