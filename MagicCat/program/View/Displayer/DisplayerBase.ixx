module;

#include <memory>
#include <vector>
#include <functional>

export module DisplayerBase;
import EffectorFactory;

namespace mc {
    export class Displayer
    {
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

    public:
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnDraw(float deltaTime) const = 0;

        virtual ~Displayer() = default;

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

        virtual void Play() { isPlaying = true; }
        virtual void Stop() { isPlaying = false; }
        virtual void ToggleVisibility() final { isVisible = !isVisible; }
        virtual bool IsVisible() const final { return isVisible; }
        virtual bool IsPlaying() const final { return isPlaying; }

        virtual void AddEffector(std::unique_ptr<Effector> e,
                                 std::function<void()> onComplete = nullptr) final
        {
            effectors.push_back({std::move(e), std::move(onComplete)});
        }

        virtual Effector* AddEffectorAndGet(std::unique_ptr<Effector> e,
                                            std::function<void()> onComplete = nullptr) final
        {
            auto* raw = e.get();
            effectors.push_back({std::move(e), std::move(onComplete)});
            return raw;
        }

        virtual void ResetAndAddEffector(std::unique_ptr<Effector> e,
                                         std::function<void()> onComplete = nullptr) final
        {
            effectors.clear();
            effectors.push_back({std::move(e), std::move(onComplete)});
            stopOnEffectEnd = true;
            isPlaying = true;
        }

        virtual void SetPosition(int newX, int newY) final
        {
            x = newX;
            y = newY;
        }
    };

    export struct Displayers : Displayer
    {
        std::vector<std::unique_ptr<Displayer>> displayers;

        Displayer* back() const { return displayers.back().get(); }

        void push_back(std::unique_ptr<Displayer> displayer)
        {
            displayers.push_back(std::move(displayer));
        }

        void clear() { displayers.clear(); }
        size_t size() const { return displayers.size(); }

        std::unique_ptr<Displayer>& operator[](size_t index)
        {
            return displayers[index];
        }

        const std::unique_ptr<Displayer>& operator[](size_t index) const
        {
            return displayers[index];
        }

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

    export class LambdaDisplayer : public Displayer
    {
        std::function<void(float)> onDrawFunc;
        std::function<void(float)> onUpdateFunc;

    public:
        LambdaDisplayer(std::function<void(float)> drawFunc, std::function<void(float)> updateFunc = nullptr)
            : onDrawFunc(std::move(drawFunc)), onUpdateFunc(std::move(updateFunc)) {}

        void OnUpdate(float deltaTime) override
        {
            if (onUpdateFunc) onUpdateFunc(deltaTime);
        }

        void OnDraw(float deltaTime) const override
        {
            if (onDrawFunc) onDrawFunc(deltaTime);
        }
    };

    export std::unique_ptr<Displayer> CreateLambdaDisplayer(
        std::function<void(float)> onDrawFunc,
        std::function<void(float)> onUpdateFunc = nullptr)
    {
        return std::make_unique<LambdaDisplayer>(std::move(onDrawFunc), std::move(onUpdateFunc));
    }
}
