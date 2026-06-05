module;

export module Character;

export class Character
{
protected:
    int rockAttack = 0;
    int scissorsAttack = 0;
    int paperAttack = 0;
    
    virtual ~Character() = default;
};
