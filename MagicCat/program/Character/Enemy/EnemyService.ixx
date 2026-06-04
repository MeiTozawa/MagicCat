module;

export module EnemyService;

export import Enemy;

export class IEnemyPool
{
public:
    virtual ~IEnemyPool() = default;
    virtual Enemy GetEnemy() = 0;
};

