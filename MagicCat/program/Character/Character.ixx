module;

export module Character;
import EventBus;
import AssetService;

namespace mc
{
    /**
     * @brief 攻撃の属性を定義する列挙型。
     * じゃんけんのルール（グー、チョキ、パー）に基づいて設定されます。
     */
    export enum class EAttackType
    {
        Rock = 0, Scissors = 1, Paper = 2,
    };

    /**
     * @brief キャラクターのタグを定義する列挙型。
     * 衝突判定やイベントの対象者を区別するために使用されます。
     */
    export enum class ETag
    {
        Player, Enemy
    };

    /**
     * @brief 攻撃が相手の攻撃に対して「負ける（不利である）」かどうかを判定します。
     * @param l 自分（判定対象）の攻撃属性
     * @param r 相手の攻撃属性
     * @return 自分の攻撃が相手の攻撃に負ける場合は true、それ以外は false。
     */
    export bool LosesTo(EAttackType l, EAttackType r)
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

    /**
     * @brief ゲーム内のすべてのキャラクターの基底クラス。
     * 共通のプロパティ（名前、属性ごとのダメージ量、スプライト、タグ）を管理します。
     */
    export class Character
    {
    protected:
        std::wstring name;
        int rockDamage = 1;
        int scissorsDamage = 1;
        int paperDamage = 1;
        ESprite sprite = ESprite::Null;

        std::vector<ETag> tags;

    public:
        ESprite GetSprite() const { return sprite; }
        std::wstring GetName() const { return name; }
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

        const std::vector<ETag>& GetTags() const { return tags; }
    };
} // namespace mc
