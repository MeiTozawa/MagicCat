module;

#include <memory>
#include <vector>
#include <functional>

export module DisplayerBase;

namespace mc
{
    export class IDisplayer
    {
    public:
        virtual ~IDisplayer() = default;
        virtual void Update(float deltaTime) = 0;
        virtual void Draw(float deltaTime) const = 0;
    };

    export struct Displayers : IDisplayer
    {
        std::vector<std::unique_ptr<IDisplayer>> displayers;
        
        IDisplayer* back() const
        {
            return displayers.back().get();
        }
        
        void push_back(std::unique_ptr<IDisplayer> displayer)
        {
            displayers.push_back(std::move(displayer));
        }

        void clear()
        {
            displayers.clear();
        }

        size_t size() const
        {
            return displayers.size();
        }

        std::unique_ptr<IDisplayer>& operator[](size_t index)
        {
            return displayers[index];
        }

        const std::unique_ptr<IDisplayer>& operator[](size_t index) const
        {
            return displayers[index];
        }

        void Update(float deltaTime) override
        {
            for (auto& displayer : displayers)
            {
                displayer->Update(deltaTime);
            }
        }

        void Draw(float deltaTime) const override
        {
            for (auto& displayer : displayers)
            {
                displayer->Draw(deltaTime);
            }
        }
    };

    export class LambdaDisplayer : public IDisplayer
    {
        std::function<void(float)> drawFunc;
        std::function<void(float)> updateFunc;
    public:
        LambdaDisplayer(std::function<void(float)> drawFunc, std::function<void(float)> updateFunc = nullptr)
            : drawFunc(std::move(drawFunc)), updateFunc(std::move(updateFunc)) {}

        void Update(float deltaTime) override
        {
            if (updateFunc) updateFunc(deltaTime);
        }

        void Draw(float deltaTime) const override
        {
            if (drawFunc) drawFunc(deltaTime);
        }
    };

    export std::unique_ptr<IDisplayer> CreateLambdaDisplayer(
        std::function<void(float)> drawFunc,
        std::function<void(float)> updateFunc = nullptr)
    {
        return std::make_unique<LambdaDisplayer>(std::move(drawFunc), std::move(updateFunc));
    }
}
