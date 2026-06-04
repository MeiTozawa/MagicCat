module;

export module EnemyService;

import Enmey;

export class IEnemyPool
{
public:
    virtual ~IEnemyPool() = default;
    virtual Enemy GetEnemy() = 0;
};

export const Enemy ENEMY_NUT = Enemy{5, 3, 1, 1, L"ナッツ"};
export const Enemy ENEMY_CABBAGE = Enemy{5, 1, 1, 3, L"キャベツ"};
export const Enemy ENEMY_KNIFE = Enemy{3, 1, 5, 1, L"包丁"};

