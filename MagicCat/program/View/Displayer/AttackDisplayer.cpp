module;

#include <dxe.h>
#include <memory>
#include <vector>
#include <string>
#include <format>

module Displayer;

import CardService;
import ServiceLocator;
import AssetService;
import EventBus;
import Character;
namespace mc
{
    class AttackDisplayer : public IDisplayer
    {
        float x, y, scale;
        IAssetService* assetService;
        Shared<dxe::Sprite> icon;

    public:
        AttackDisplayer(const float x, const float y, const float scale, const EAttackType type)
            : x(x),y(y),scale(scale)
        {
            assetService = ServiceLocator::Get<IAssetService>();
            icon = assetService->GetImage(static_cast<EImage>(type));
        }


        void Update(float deltaTime) override {}
        void Draw(float deltaTime) const override
        {
            if (icon != nullptr)
            {
                icon->setScaleXY({scale, scale});
                icon->setPosition({x,y});
                icon->draw();
            }
        }
    };

    std::unique_ptr<IDisplayer> CreateAttackDisplayer(float x, float y, float scale, EAttackType type)
    {
        return std::make_unique<AttackDisplayer>(x, y, scale, type);
    }
}
