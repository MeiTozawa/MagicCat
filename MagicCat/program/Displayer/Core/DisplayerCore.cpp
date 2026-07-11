module;

#include <memory>
#include <vector>
#include <functional>
#include <algorithm>
#include <RenderUtils.h>

module Displayer;

namespace mc {

    class Displayer : virtual public IDisplayer
    {
    public:
        ~Displayer() override = default;

        void Update(float deltaTime) override
        {
            if (!isPlaying) return;
            OnUpdate(deltaTime);
            UpdateEffectors(deltaTime);
        }

        void Draw(float deltaTime) const override
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

        void Play() override { isPlaying = true; }
        void Stop() override { isPlaying = false; }
        void ToggleVisibility() override { isVisible = !isVisible; }
        bool IsVisible() const override { return isVisible; }
        bool IsPlaying() const override { return isPlaying; }

        void AddEffector(std::unique_ptr<Effector> e, std::function<void()> onComplete) override
        {
            effectors.push_back({std::move(e), std::move(onComplete)});
        }

        Effector* AddEffectorAndGet(std::unique_ptr<Effector> e, std::function<void()> onComplete) override
        {
            auto* raw = e.get();
            effectors.push_back({std::move(e), std::move(onComplete)});
            return raw;
        }

        void ResetAndAddEffector(std::unique_ptr<Effector> e, std::function<void()> onComplete) override
        {
            effectors.clear();
            effectors.push_back({std::move(e), std::move(onComplete)});
            stopOnEffectEnd = true;
            isPlaying = true;
        }

        void SetPosition(int newX, int newY) override
        {
            position.x = newX;
            position.y = newY;
        }

        Point<int> GetPosition() const override { return position; }
        bool HasActiveEffectors() const override { return !effectors.empty(); }

    protected:
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnDraw(float deltaTime) const = 0;


    private:
        void UpdateEffectors(float deltaTime)
        {
            if (!effectors.empty())
            {
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

        Point<int> position{0, 0};
        bool isVisible = true;
        bool isPlaying = true;
        bool stopOnEffectEnd = false;

        struct EffectorEntry
        {
            std::unique_ptr<Effector> effector;
            std::function<void()> onComplete;
        };

        std::vector<EffectorEntry> effectors = {};
    };

    class CompositeDisplayer : public Displayer, public ICompositeDisplayer
    {
    public:
        CompositeDisplayer() = default;

        IDisplayer* back() const override { return children.back().get(); }

        void push_back(std::unique_ptr<IDisplayer> displayer) override
        {
            children.push_back(std::move(displayer));
        }

        void clear() override { children.clear(); }

        size_t size() const override { return children.size(); }

        std::unique_ptr<IDisplayer>& operator[](size_t index) override
        {
            return children[index];
        }

        const std::unique_ptr<IDisplayer>& operator[](size_t index) const override
        {
            return children[index];
        }

        // Delegate to IDisplayer methods since Displayer implements them:
        void Update(float deltaTime) override { Displayer::Update(deltaTime); }
        void Draw(float deltaTime) const override { Displayer::Draw(deltaTime); }
        void Play() override { Displayer::Play(); }
        void Stop() override { Displayer::Stop(); }
        void ToggleVisibility() override { Displayer::ToggleVisibility(); }
        bool IsVisible() const override { return Displayer::IsVisible(); }
        bool IsPlaying() const override { return Displayer::IsPlaying(); }
        void AddEffector(std::unique_ptr<Effector> e, std::function<void()> onComplete) override
        {
            Displayer::AddEffector(std::move(e), onComplete);
        }
        Effector* AddEffectorAndGet(std::unique_ptr<Effector> e, std::function<void()> onComplete) override
        {
            return Displayer::AddEffectorAndGet(std::move(e), onComplete);
        }
        void ResetAndAddEffector(std::unique_ptr<Effector> e, std::function<void()> onComplete) override
        {
            Displayer::ResetAndAddEffector(std::move(e), onComplete);
        }
        void SetPosition(int newX, int newY) override { Displayer::SetPosition(newX, newY); }
        Point<int> GetPosition() const override { return Displayer::GetPosition(); }
        bool HasActiveEffectors() const override { return Displayer::HasActiveEffectors(); }

    protected:
        void OnUpdate(float deltaTime) override
        {
            for (auto& child : children)
                child->Update(deltaTime);
        }

        void OnDraw(float deltaTime) const override
        {
            for (const auto& child : children)
                child->Draw(deltaTime);
        }

    private:
        std::vector<std::unique_ptr<IDisplayer>> children;
    };

    class LambdaDisplayer : public Displayer
    {
    public:
        LambdaDisplayer(std::function<void(float)> drawFunc, std::function<void(float)> updateFunc = nullptr)
            : onDrawFunc(std::move(drawFunc)), onUpdateFunc(std::move(updateFunc)) {}

    protected:
        void OnUpdate(float deltaTime) override
        {
            if (onUpdateFunc) onUpdateFunc(deltaTime);
        }

        void OnDraw(float deltaTime) const override
        {
            if (onDrawFunc) onDrawFunc(deltaTime);
        }

    private:
        std::function<void(float)> onDrawFunc;
        std::function<void(float)> onUpdateFunc;
    };

    std::unique_ptr<ICompositeDisplayer> CreateCompositeDisplayer()
    {
        return std::make_unique<CompositeDisplayer>();
    }

    std::unique_ptr<IDisplayer> CreateLambdaDisplayer(
        std::function<void(float)> onDrawFunc,
        std::function<void(float)> onUpdateFunc)
    {
        return std::make_unique<LambdaDisplayer>(std::move(onDrawFunc), std::move(onUpdateFunc));
    }
}
