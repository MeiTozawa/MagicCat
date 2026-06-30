module;

export module EffectorFactory;
import RenderService;
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
        IRenderService& renderService, uint32_t color, int flashTime = 300
    );
    export std::unique_ptr<Effector> CreateFadeEffector(
        IRenderService& renderService, int fadeInTime, int holdTime, int fadeOutTime
    );
    /// @brief 不透明 → 透明（alpha 255→0、覆いを剥がして表示する）
    export std::unique_ptr<Effector> CreateFadeOutEffector(
        IRenderService& renderService, int durationMs
    );
    /// @brief 透明 → 不透明（alpha 0→255、BG で覆う）
    export std::unique_ptr<Effector> CreateFadeInEffector(
        IRenderService& renderService, int durationMs
    );
}
