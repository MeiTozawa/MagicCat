module;

#include <memory>
#include <vector>
#include <functional>

export module DisplayerBase;
import EffectorFactory;

namespace mc {
    /// @brief 画面上の描画要素とそれに適用される演出エフェクト（Effector）を管理する基底クラス
    export class Displayer
    {
    public:
        virtual ~Displayer() = default;

        /// @brief 毎フレームの更新処理。再生中であればOnUpdateやエフェクトの更新を行う。
        /// @param deltaTime 前のフレームからの経過時間（秒）
        virtual void Update(float deltaTime)
        {
            if (!isPlaying) return;
            OnUpdate(deltaTime);
            if (!effectors.empty())
            {
                // erase_if 中に effectors が変更されるとイテレータが無効になるため、
                // コールバックを先に収集してから一括実行する。
                std::vector<std::function<void()>> callbacks;
                std::erase_if(effectors, [&](const auto& entry)
                {
                    const bool done = !entry.effector->Update(deltaTime);
                    if (done && entry.onComplete)
                        callbacks.push_back(entry.onComplete);
                    return done;
                });
                for (const auto& cb : callbacks)
                    cb();
                if (stopOnEffectEnd && effectors.empty())
                {
                    stopOnEffectEnd = false;
                    isPlaying = false;
                }
            }
        }

        /// @brief 描画処理。再生かつ可視であればエフェクト適用後にOnDrawを実行する。
        /// @param deltaTime 前のフレームからの経過時間（秒）
        virtual void Draw(float deltaTime) const
        {
            if (!isVisible || !isPlaying) return;
            if (!effectors.empty())
            {
                bool shouldDraw = true;
                for (const auto& entry : effectors)
                {
                    entry.effector->BeforeDraw();
                    if (!entry.effector->ShouldDraw()) shouldDraw = false;
                }
                if (shouldDraw) OnDraw(deltaTime);
                for (const auto& entry : effectors)
                    entry.effector->AfterDraw();
            }
            else
            {
                OnDraw(deltaTime);
            }
        }

        /// @brief 要素の再生・更新を有効にする
        virtual void Play() { isPlaying = true; }

        /// @brief 要素の再生・更新を一時停止する
        virtual void Stop() { isPlaying = false; }

        /// @brief 表示・非表示の状態を反転する
        virtual void ToggleVisibility() final { isVisible = !isVisible; }

        /// @brief 現在表示されているかを取得する
        /// @return 表示されている場合はtrue、そうでない場合はfalse
        virtual bool IsVisible() const final { return isVisible; }

        /// @brief 現在再生中（更新が有効）かを取得する
        /// @return 再生中の場合はtrue、そうでない場合はfalse
        virtual bool IsPlaying() const final { return isPlaying; }

        /// @brief 演出エフェクト（Effector）を追加する
        /// @param e 追加するエフェクトのユニークポインタ
        /// @param onComplete エフェクト完了時に呼び出されるコールバック
        virtual void AddEffector(std::unique_ptr<Effector> e,
                                 std::function<void()> onComplete = nullptr) final
        {
            effectors.push_back({std::move(e), std::move(onComplete)});
        }

        /// @brief 演出エフェクト（Effector）を追加し、その生ポインタを返す
        /// @param e 追加するエフェクトのユニークポインタ
        /// @param onComplete エフェクト完了時に呼び出されるコールバック
        /// @return 追加されたエフェクトへの生ポインタ
        virtual Effector* AddEffectorAndGet(std::unique_ptr<Effector> e,
                                            std::function<void()> onComplete = nullptr) final
        {
            auto* raw = e.get();
            effectors.push_back({std::move(e), std::move(onComplete)});
            return raw;
        }

        /// @brief 既存のエフェクトをクリアし、新しいエフェクトを適用すると同時に再生状態にする
        /// @param e 追加するエフェクトのユニークポインタ
        /// @param onComplete エフェクト完了時に呼び出されるコールバック
        virtual void ResetAndAddEffector(std::unique_ptr<Effector> e,
                                         std::function<void()> onComplete = nullptr) final
        {
            effectors.clear();
            effectors.push_back({std::move(e), std::move(onComplete)});
            stopOnEffectEnd = true;
            isPlaying = true;
        }

        /// @brief 描画要素の基準座標を設定する
        /// @param newX 新しいX座標
        /// @param newY 新しいY座標
        virtual void SetPosition(int newX, int newY) final
        {
            x = newX;
            y = newY;
        }

    protected:
        int x = 0, y = 0;
        bool isVisible = true;
        bool isPlaying = true;
        bool stopOnEffectEnd = false; ///< 全 Effector 完了時に自動 Stop するか

        struct EffectorEntry
        {
            std::unique_ptr<Effector> effector;
            std::function<void()> onComplete; ///< 完了時コールバック（省略可）
        };

        std::vector<EffectorEntry> effectors = {};

    private:
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnDraw(float deltaTime) const = 0;
    };

    /// @brief 複数の Displayer をグループ化して一括管理・更新・描画するためのコンテナクラス
    export struct Displayers : Displayer
    {
        std::vector<std::unique_ptr<Displayer>> displayers;

        /// @brief コンテナの末尾の Displayer へのポインタを取得する
        /// @return Displayer へのポインタ
        Displayer* back() const { return displayers.back().get(); }

        /// @brief コンテナに新しい Displayer を追加する
        /// @param displayer 追加する Displayer のユニークポインタ
        void push_back(std::unique_ptr<Displayer> displayer)
        {
            displayers.push_back(std::move(displayer));
        }

        /// @brief すべての Displayer をクリア（破棄）する
        void clear() { displayers.clear(); }

        /// @brief 保持している Displayer の数を取得する
        /// @return 保持数
        size_t size() const { return displayers.size(); }

        /// @brief インデックスを指定して Displayer を取得する
        /// @param index インデックス
        /// @return Displayer のユニークポインタへの参照
        std::unique_ptr<Displayer>& operator[](size_t index)
        {
            return displayers[index];
        }

        /// @brief インデックスを指定して Displayer を取得する（読み取り専用）
        /// @param index インデックス
        /// @return Displayer のユニークポインタへの参照（const）
        const std::unique_ptr<Displayer>& operator[](size_t index) const
        {
            return displayers[index];
        }

    private:
        void OnUpdate(float deltaTime) override
        {
            for (auto& displayer : displayers)
                displayer->Update(deltaTime);
        }

        void OnDraw(float deltaTime) const override
        {
            for (auto& displayer : displayers)
                displayer->Draw(deltaTime);
        }
    };

    /// @brief 描画処理や更新処理をラムダ式/コールバック関数で動的に指定できる Displayer 拡張クラス
    export class LambdaDisplayer : public Displayer
    {
    public:
        /// @brief コールバック関数を指定して LambdaDisplayer を構築する
        /// @param drawFunc 描画時に呼び出される関数
        /// @param updateFunc 更新時に呼び出される関数（省略可）
        LambdaDisplayer(std::function<void(float)> drawFunc, std::function<void(float)> updateFunc = nullptr)
            : onDrawFunc(std::move(drawFunc)), onUpdateFunc(std::move(updateFunc)) {}

    private:
        void OnUpdate(float deltaTime) override
        {
            if (onUpdateFunc) onUpdateFunc(deltaTime);
        }

        void OnDraw(float deltaTime) const override
        {
            if (onDrawFunc) onDrawFunc(deltaTime);
        }

        std::function<void(float)> onDrawFunc;
        std::function<void(float)> onUpdateFunc;
    };

    export std::unique_ptr<Displayer> CreateLambdaDisplayer(
        std::function<void(float)> onDrawFunc,
        std::function<void(float)> onUpdateFunc = nullptr)
    {
        return std::make_unique<LambdaDisplayer>(std::move(onDrawFunc), std::move(onUpdateFunc));
    }
}
