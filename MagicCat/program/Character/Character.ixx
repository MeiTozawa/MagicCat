module;
#include <cassert>

export module Character;
import EventBus;
import AssetService;

namespace mc {
    export enum class EAttackType
    {
        Rock = 0, Scissors = 1, Paper = 2,
    };

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

    /// @brief 戦闘（ジャンケン）が解決したときに発行されるイベント
    export struct CombatEvent : IEvent
    {
        EAttackType playerAttackType;
        EAttackType enemyAttackType;
        int playerAttackDamage;
        int enemyAttackDamage;
        float playerWinRate;
        /// @brief 戦闘解決時点での敵の各手の weight offset（Rock=0, Scissors=1, Paper=2）
        int enemyWeightOffsets[3];

        CombatEvent(EAttackType playerAttackType, EAttackType enemyAttackType,
                    int playerAttackDamage, int enemyAttackDamage, float playerWinRate = 0.f,
                    int rockOffset = 0, int scissorsOffset = 0, int paperOffset = 0) :
            playerAttackType(playerAttackType), enemyAttackType(enemyAttackType),
            playerAttackDamage(playerAttackDamage), enemyAttackDamage(enemyAttackDamage),
            playerWinRate(playerWinRate),
            enemyWeightOffsets{rockOffset, scissorsOffset, paperOffset} {}
    };

    /**
     * @brief ゲーム内のすべてのキャラクターの基底クラス。
     * 共通のプロパティ（名前、属性ごとのダメージ量、スプライト、タグ）を管理します。
     */
    export class Character
    {
    public:
        virtual ~Character() = default;

        /// @brief ダメージを受ける純粋仮想関数
        /// @param amount ダメージ量
        virtual void TakeDamage(int amount) const = 0;

        /// @brief キャラクターのスプライト列挙値を取得する
        /// @return スプライトID
        ESprite GetSprite() const { return sprite; }

        /// @brief キャラクターの名前を取得する
        /// @return ワイド文字列の名前
        std::wstring GetName() const { return name; }

        /// @brief 攻撃力の一時的な増加・減少（オフセット）を設定する
        /// @param offset 設定するオフセット値
        void SetDamageOffset(int offset) { damageOffset = offset; }

        /// @brief 現在の攻撃力オフセット値を取得する
        /// @return 攻撃力オフセット値
        int GetDamageOffset() const { return damageOffset; }

        /// @brief 攻撃力オフセット値をリセット（0）にする
        void ResetDamageOffset() { damageOffset = 0; }

        /// @brief キャラクターに付与されているタグリストを取得する
        /// @return タグのベクターへの参照
        const std::vector<ETag>& GetTags() const { return tags; }

        /// @brief 特定の攻撃タイプに対する強化無しの基礎ダメージを取得する
        /// @param t 攻撃タイプ
        /// @return 基礎ダメージ値
        int GetBaseDamage(EAttackType t) const
        {
            switch (t)
            {
            case EAttackType::Rock:     return rockDamage;
            case EAttackType::Scissors: return scissorsDamage;
            case EAttackType::Paper:    return paperDamage;
            default:
                assert(false && "未知の攻撃タイプです");
                return 0;
            }
        }

        /// @brief 特定の攻撃タイプに対する現在の（オフセット込みの）ダメージを取得する
        /// @param t 攻撃タイプ
        /// @return ダメージ値
        int GetDamage(EAttackType t) const
        {
            switch (t)
            {
            case EAttackType::Rock:     return rockDamage + damageOffset;
            case EAttackType::Scissors: return scissorsDamage + damageOffset;
            case EAttackType::Paper:    return paperDamage + damageOffset;
            default:
                assert(false && "未知の攻撃タイプです");
                return 0;
            }
        }

    protected:
        std::wstring name;
        int rockDamage = 1;
        int scissorsDamage = 1;
        int paperDamage = 1;
        int damageOffset = 0;
        ESprite sprite = ESprite::Null;
        std::vector<ETag> tags;
    };
} // namespace mc
