module;

#include <memory>
module Displayer;
import RenderService;

namespace mc {
    /// @brief 戦闘アクション（選択されたジャンケンの手など）を画面上に描画するディスプレイヤー
    class AttackDisplayer : public IAttackDisplayer, public DelegatingDisplayer
    {
    public:
        /// @brief 座標とスケールを指定してAttackDisplayerを構築する
        /// @param rs レンダリングサービスへの参照
        /// @param x 描画中心X座標
        /// @param y 描画中心Y座標
        /// @param scale 拡大率
        AttackDisplayer(IRenderService& rs, const float x, const float y, const float scale)
            : x(x), y(y), scale(scale), renderService(rs) {}

        /// @brief 描画するグラフィック画像ハンドルを設定する
        /// @param imageHandle 画像ハンドル
        void SetImage(int imageHandle) override { handle = imageHandle; }

    protected:
        void OnDraw(float deltaTime) const override
        {
            if (handle != -1)
                renderService.DrawRotaGraphF(x, y, scale, 0.0, handle, true);
        }

    private:
        float x, y, scale;
        int handle = -1;
        IRenderService& renderService;
    };

    std::unique_ptr<IAttackDisplayer> CreateAttackDisplayer(IRenderService& renderService,
                                                                   float x, float y, float scale)
    {
        return std::make_unique<AttackDisplayer>(renderService, x, y, scale);
    }
}
