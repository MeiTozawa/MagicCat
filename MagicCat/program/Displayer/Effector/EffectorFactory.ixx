module;

export module EffectorFactory;
import RenderService;
namespace mc {
    export enum class EEffector
    {
        HitFlash, Fade
    };

    /// @brief 描画要素（Displayer）に対して一時的なブレンド効果や変形効果などの演出を付与するベースクラス
    export class Effector
    {
    public:
        virtual ~Effector() = default;

        /// @brief 演出状態を更新する。
        /// @param deltaTime 前のフレームからの経過時間（秒）
        /// @return 演出が継続中の場合はtrue、終了した場合はfalse
        virtual bool Update(float deltaTime) = 0;

        /// @brief 描画処理の直前に呼ばれ、ブレンドモードの設定などを行う
        virtual void BeforeDraw() const = 0;

        /// @brief 描画処理の直後に呼ばれ、ブレンドモードの解除などを行う
        virtual void AfterDraw() const = 0;

        /// @brief この Effector が適用された状態で Displayer を描画すべきか。
        /// false を返した場合 OnDraw はスキップされる。
        /// @return 描画すべき場合はtrue、そうでない場合はfalse
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
