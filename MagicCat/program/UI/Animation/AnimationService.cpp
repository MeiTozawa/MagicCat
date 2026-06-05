module;

#include <dxe.h>

module AnimationService;

import ServiceLocator;

class AnimationService : public IAnimationService
{
private:
    std::shared_ptr<IAssetService> assetService;
public:

    AnimationService(){
        assetService = ServiceLocator::Get<IAssetService>();
    }

    void Draw(int spriteHandle, int x, int y, int size) override
    {
        int frame = DerivationGraph(0, 0, 32, 32, spriteHandle);
        DrawExtendGraph(x, y, x + size, y + size, frame, TRUE);
    }
};

static struct RegisterAnimationService
{
    RegisterAnimationService()
    {
        ServiceLocator::RegisterSingleton<IAnimationService, AnimationService>(std::make_shared<AnimationService>());
    }
} autoRegister_AnimationService;
