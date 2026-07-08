module;
#include <cassert>

#include <RandomUtils.h>

export module Enemy;

import Character;
import HealthComponent;
import IDamageable;
import EventBus;
import AssetService;
import Player;

namespace mc {
    /// @brief 敵の手の出現重み（ウェイト）が変更されたときに発行されるイベント
    export struct AddWeightEvent : IEvent
    {
        EAttackType AttackType;
        int Offset;
        AddWeightEvent(EAttackType attackType, int offset) : AttackType(attackType), Offset(offset) {}
    };

    /// @brief 敵キャラクターを表すクラス。行動パターンの確率制御や状態管理を行います。
    export class Enemy : public Character
    {
    public:
        /// @brief パラメータを指定して敵キャラクターを構築する
        /// @param baseWeight 基本ウェイト値（各手の出現確率の基本値）
        /// @param rockDamage 石の基礎ダメージ
        /// @param scissorsDamage 鋏の基礎ダメージ
        /// @param paperDamage 紙の基礎ダメージ
        /// @param name 敵キャラクターの名前
        /// @param sprite スプライトID
        /// @param hp 敵キャラクターの初期HP
        Enemy(int baseWeight = 0, int rockDamage = 0, int scissorsDamage = 0, int paperDamage = 0,
              const std::wstring& name = L"Unknown", ESprite sprite = ESprite::Null, int hp = 10)
            : baseWeight(baseWeight)
        {
#ifdef _DEBUG
            this->baseWeight = 1;
#endif
            Character::name = name;
            Character::sprite = sprite;
            Character::rockDamage = rockDamage;
            Character::scissorsDamage = scissorsDamage;
            Character::paperDamage = paperDamage;
            healthComp = std::make_unique<HealthComponent>(this, hp);
            tags.push_back(ETag::Enemy);
        }

        /// @brief 既存の Enemy インスタンスを破棄・再生成せず、その内部データを
        /// 新しい敵の設定で上書きする。HealthComponent や Enemy への参照は維持される。
        /// @param baseWeight 基本ウェイト値
        /// @param rockDamage 石の基礎ダメージ
        /// @param scissorsDamage 鋏の基礎ダメージ
        /// @param paperDamage 紙の基礎ダメージ
        /// @param name 敵キャラクターの名前
        /// @param sprite スプライトID
        /// @param hp 敵キャラクターの初期HP
        void Reset(int baseWeight, int rockDamage, int scissorsDamage, int paperDamage,
                   const std::wstring& name, ESprite sprite, int hp)
        {
            this->baseWeight = baseWeight;
            Character::name = name;
            Character::sprite = sprite;
            Character::rockDamage = rockDamage;
            Character::scissorsDamage = scissorsDamage;
            Character::paperDamage = paperDamage;
            healthComp->Reset(hp);
            ResetWeights();
            isExposed = false;
        }

        /// @brief 指定した攻撃タイプ（手）の出現ウェイトにオフセットを追加する
        /// @param t 攻撃タイプ
        /// @param weight 追加するウェイト値
        void AddWeight(EAttackType t, int weight)
        {
            switch (t)
            {
            case EAttackType::Rock:     AddRockWeight(weight);     break;
            case EAttackType::Scissors: AddScissorsWeight(weight); break;
            case EAttackType::Paper:    AddPaperWeight(weight);    break;
            default:
                assert(false && "未知の攻撃タイプです");
                break;
            }
        }

        /// @brief 敵の次の攻撃（手）を現在の出現ウェイトの比率からランダムに決定する
        /// @return 決定した攻撃タイプ
        EAttackType GetAttackIntent() const
        {
            int rockWeight     = baseWeight + rockWeightOffset;
            int scissorsWeight = baseWeight + scissorsWeightOffset;
            int paperWeight    = baseWeight + paperWeightOffset;
            int index = Random::RandomSelection(rockWeight, scissorsWeight, paperWeight);
            assert(index >= 0 && index <= 2 && "ランダム攻撃のインデックスが範囲外です");
            constexpr EAttackType mappedTypes[] = {EAttackType::Rock, EAttackType::Scissors, EAttackType::Paper};
            return mappedTypes[index];
        }

        /// @brief プレイヤーが特定の攻撃を出したとき、この敵が「負ける確率」を計算する
        /// @param playerAttack プレイヤーの攻撃タイプ
        /// @return 負ける確率（0.0f 〜 1.0f）
        float GetLoseRateAgainst(EAttackType playerAttack) const
        {
            int rockWeight     = baseWeight + rockWeightOffset;
            int scissorsWeight = baseWeight + scissorsWeightOffset;
            int paperWeight    = baseWeight + paperWeightOffset;
            int total = rockWeight + scissorsWeight + paperWeight;
            if (total <= 0) return 0.f;

            int losingWeight = 0;
            if (LosesTo(EAttackType::Rock,     playerAttack)) losingWeight += rockWeight;
            if (LosesTo(EAttackType::Scissors, playerAttack)) losingWeight += scissorsWeight;
            if (LosesTo(EAttackType::Paper,    playerAttack)) losingWeight += paperWeight;

            return static_cast<float>(losingWeight) / total;
        }

        /// @brief 敵の等価比較演算子
        bool operator==(const Enemy& e) const
        {
            return name == e.name &&
                baseWeight == e.baseWeight &&
                rockDamage == e.rockDamage &&
                scissorsDamage == e.scissorsDamage &&
                paperDamage == e.paperDamage;
        }

        /// @brief 特定の攻撃タイプに対する出現ウェイトオフセットを取得する
        /// @param t 攻撃タイプ
        /// @return ウェイトオフセット値
        int GetWeightOffset(EAttackType t) const
        {
            switch (t)
            {
            case EAttackType::Rock:     return rockWeightOffset;
            case EAttackType::Scissors: return scissorsWeightOffset;
            case EAttackType::Paper:    return paperWeightOffset;
            default:
                assert(false && "未知の攻撃タイプです");
                return 0;
            }
        }

        /// @brief 敵の次の攻撃意図（手）がプレイヤーに見えている（露出している）かどうかを取得する
        /// @return 露出している場合はtrue、そうでない場合はfalse
        bool IsExposed() const { return isExposed; }

        /// @brief 敵の基本出現ウェイトを取得する
        /// @return 基本ウェイト値
        int GetBaseWeight() const { return baseWeight; }

        /// @brief 敵のライフ（HealthComponent）への参照を取得する（読み取り用）
        /// @return HealthComponentへの const 参照
        const HealthComponent& GetHealthComponent() const { return *healthComp; }

        /// @brief 敵のライフ（HealthComponent）への参照を取得する（状態復元用）
        /// @return IDamageable への参照
        IDamageable& GetHealthComponent() { return *healthComp; }

        /// @brief 敵がダメージを受ける処理
        /// @param amount ダメージ量
        void TakeDamage(int amount) const override { healthComp->TakeDamage(amount); }

        /// @brief 各手の出現ウェイトオフセットをリセット（0）する
        void ResetWeights()
        {
            rockWeightOffset = 0;
            scissorsWeightOffset = 0;
            paperWeightOffset = 0;
        }

        /// @brief 敵の攻撃意図の露出状態を設定する
        /// @param exposed 設定する露出状態
        void SetExposed(bool exposed) { isExposed = exposed; }

        /// @brief グー（石）の出現ウェイトオフセットを取得する（シリアライズ用）
        /// @return rockWeightOffset の現在値
        virtual int GetRockOffset() const final { return rockWeightOffset; }

        /// @brief チョキ（鋏）の出現ウェイトオフセットを取得する（シリアライズ用）
        /// @return scissorsWeightOffset の現在値
        virtual int GetScissorsOffset() const final { return scissorsWeightOffset; }

        /// @brief パー（紙）の出現ウェイトオフセットを取得する（シリアライズ用）
        /// @return paperWeightOffset の現在値
        virtual int GetPaperOffset() const final { return paperWeightOffset; }

        /// @brief グー（石）の出現ウェイトオフセットを設定する（デシリアライズ用）
        /// @param value 設定するオフセット値
        virtual void SetRockOffset(int value) final { rockWeightOffset = value; }

        /// @brief チョキ（鋏）の出現ウェイトオフセットを設定する（デシリアライズ用）
        /// @param value 設定するオフセット値
        virtual void SetScissorsOffset(int value) final { scissorsWeightOffset = value; }

        /// @brief パー（紙）の出現ウェイトオフセットを設定する（デシリアライズ用）
        /// @param value 設定するオフセット値
        virtual void SetPaperOffset(int value) final { paperWeightOffset = value; }

    private:
        std::unique_ptr<HealthComponent> healthComp;
        bool isExposed = false;
        int baseWeight = 0;
        int rockWeightOffset = 0;
        int scissorsWeightOffset = 0;
        int paperWeightOffset = 0;

        void AddRockWeight(int weight)     { rockWeightOffset += weight; }
        void AddScissorsWeight(int weight) { scissorsWeightOffset += weight; }
        void AddPaperWeight(int weight)    { paperWeightOffset += weight; }
    };
} // namespace mc
