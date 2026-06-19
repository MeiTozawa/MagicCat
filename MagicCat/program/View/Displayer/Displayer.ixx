module;

#include <memory>
#include <functional>

export module Displayer;

import RenderService;
import AssetService;
import CardService;
import CharacterService;

namespace mc
{
    export class IDisplayer
    {
    public:
        virtual ~IDisplayer() = default;
        virtual void Update(float deltaTime) = 0;
        virtual void Draw(float deltaTime) const = 0;
    };



    export std::unique_ptr<IDisplayer> CreateAttackDisplayer(float x, float y, float scale, const int* handle);
    export std::unique_ptr<IDisplayer> CreateControlDisplayer(IAssetService* assetService, IRenderService* renderService, uint32_t color = 0xFFFFFF);
    export std::unique_ptr<IDisplayer> CreateCardDisplayer(ICardService* cardService, IAssetService* assetService, IRenderService* renderService);
    export std::unique_ptr<IDisplayer> CreateCharacterDisplayer(ICharacterService* characterService, IRenderService* renderService);

    export class LambdaDisplayer : public IDisplayer
    {
        std::function<void(float)> drawFunc;
        std::function<void(float)> updateFunc;
    public:
        LambdaDisplayer(std::function<void(float)> drawFunc, std::function<void(float)> updateFunc)
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
