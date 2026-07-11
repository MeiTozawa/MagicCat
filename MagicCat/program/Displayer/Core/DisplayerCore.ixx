module;

#include <memory>
#include <vector>
#include <functional>
#include <RenderUtils.h>

export module Displayer:Core;
import EffectorFactory;

namespace mc {
    export class IDisplayer
    {
    public:
        virtual ~IDisplayer() = default;

        virtual void Update(float deltaTime) = 0;
        virtual void Draw(float deltaTime) const = 0;
        virtual void Play() = 0;
        virtual void Stop() = 0;
        virtual void ToggleVisibility() = 0;
        virtual bool IsVisible() const = 0;
        virtual bool IsPlaying() const = 0;

        virtual void AddEffector(std::unique_ptr<Effector> e,
                                 std::function<void()> onComplete = nullptr) = 0;
        virtual Effector* AddEffectorAndGet(std::unique_ptr<Effector> e,
                                            std::function<void()> onComplete = nullptr) = 0;
        virtual void ResetAndAddEffector(std::unique_ptr<Effector> e,
                                         std::function<void()> onComplete = nullptr) = 0;

        virtual void SetPosition(int newX, int newY) = 0;
        virtual Point<int> GetPosition() const = 0;
        virtual bool HasActiveEffectors() const = 0;
    };

    export class ICompositeDisplayer : public IDisplayer
    {
    public:
        virtual IDisplayer* back() const = 0;
        virtual void push_back(std::unique_ptr<IDisplayer> displayer) = 0;
        virtual void clear() = 0;
        virtual size_t size() const = 0;
        virtual std::unique_ptr<IDisplayer>& operator[](size_t index) = 0;
        virtual const std::unique_ptr<IDisplayer>& operator[](size_t index) const = 0;
    };

    export using Displayers = ICompositeDisplayer;

    export std::unique_ptr<ICompositeDisplayer> CreateCompositeDisplayer();

    export std::unique_ptr<IDisplayer> CreateLambdaDisplayer(
        std::function<void(float)> onDrawFunc,
        std::function<void(float)> onUpdateFunc = nullptr);

    export class DelegatingDisplayer : public virtual IDisplayer
    {
    public:
        DelegatingDisplayer()
        {
            m_base = CreateLambdaDisplayer(
                [this](float dt) { OnDraw(dt); },
                [this](float dt) { OnUpdate(dt); }
            );
        }

        void Update(float deltaTime) override { m_base->Update(deltaTime); }
        void Draw(float deltaTime) const override { m_base->Draw(deltaTime); }
        void Play() override { m_base->Play(); }
        void Stop() override { m_base->Stop(); }
        void ToggleVisibility() override { m_base->ToggleVisibility(); }
        bool IsVisible() const override { return m_base->IsVisible(); }
        bool IsPlaying() const override { return m_base->IsPlaying(); }

        void AddEffector(std::unique_ptr<Effector> e, std::function<void()> onComplete = nullptr) override
        {
            m_base->AddEffector(std::move(e), onComplete);
        }

        Effector* AddEffectorAndGet(std::unique_ptr<Effector> e, std::function<void()> onComplete = nullptr) override
        {
            return m_base->AddEffectorAndGet(std::move(e), onComplete);
        }

        void ResetAndAddEffector(std::unique_ptr<Effector> e, std::function<void()> onComplete = nullptr) override
        {
            m_base->ResetAndAddEffector(std::move(e), onComplete);
        }

        void SetPosition(int newX, int newY) override { m_base->SetPosition(newX, newY); }
        Point<int> GetPosition() const override { return m_base->GetPosition(); }
        bool HasActiveEffectors() const override { return m_base->HasActiveEffectors(); }

    protected:
        virtual void OnUpdate(float deltaTime) {}
        virtual void OnDraw(float deltaTime) const = 0;

        int GetX() const { return m_base->GetPosition().x; }
        int GetY() const { return m_base->GetPosition().y; }

    private:
        std::unique_ptr<IDisplayer> m_base;
    };
}