module;

export module Character;

export class Character
{
protected:
    int _rockAttack = 0;
    int _scissorsAttack = 0;
    int _paperAttack = 0;
    
    Character() = default;
    ~Character() = default;
};
