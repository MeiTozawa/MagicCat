module;

export module EffectorFactory;
namespace mc {
    export enum class EEffector
    {
        HitFlash, Fade
    };

    export class Effector
    {
    public:
        virtual ~Effector() = default;
        virtual bool Update(float deltaTime) = 0;
        virtual void BeforeDraw() const = 0;
        virtual void AfterDraw() const = 0;
        /// @brief この Effector が適用された状態で Displayer を描画すべきか。
        /// false を返した場合 OnDraw はスキップされる。
        virtual bool ShouldDraw() const { return true; }
    };

    export std::unique_ptr<Effector> CreateHitFlashEffector(
        uint32_t color, int flashTime = 300
    );
    export std::unique_ptr<Effector> CreateFadeEffector(
        int fadeInTime, int holdTime, int fadeOutTime
    );
    /// @brief 透明 → 不透明（単方向フェードアウト）
    export std::unique_ptr<Effector> CreateFadeOutEffector(
        int durationMs
    );
    /// @brief 不透明 → 透明（単方向フェードイン）
    export std::unique_ptr<Effector> CreateFadeInEffector(
        int durationMs
    );
}
