module;

export module AnimationService;

import AssetService;

export class IAnimationService
{
public:
    virtual ~IAnimationService() = default;
    virtual void Draw(int, int x, int y, int size = 256) = 0;
};