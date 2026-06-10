module;

export module ControlView;

import ServiceLocator;
import AssetService;

namespace mc {


constexpr int KB_Q_X = 100;
constexpr int KB_UP_X = 500;
constexpr int KB_DOWN_X = 560;
constexpr int KB_SPACE_X = 620;
constexpr int Y = 100;

constexpr int TEXT_OFFSET_X = 40;
constexpr int TEXT_OFFSET_Y = -15;

export class ControlView
{
    IAssetService* assetService;

public:
    ControlView()
    {
        assetService = ServiceLocator::Get<IAssetService>();
    }

    void PrintControl(uint32_t color = 0xFFFFFF) const
    {
        auto icon = assetService->GetImage(EImage::KB_Q);
        icon->setPosition({KB_Q_X, Y});
        icon->draw();
        DrawString(KB_Q_X + TEXT_OFFSET_X, Y + TEXT_OFFSET_Y, L"カードを引く", color);
        
        icon = assetService->GetImage(EImage::KB_UP);
        icon->setPosition({KB_UP_X, Y});
        icon->draw();
        icon = assetService->GetImage(EImage::KB_DOWN);
        icon->setPosition({KB_DOWN_X, Y});
        icon->draw();
        icon = assetService->GetImage(EImage::KB_SPACE);
        icon->setPosition({KB_SPACE_X, Y});
        icon->draw();
        DrawString(KB_SPACE_X + TEXT_OFFSET_X, Y + TEXT_OFFSET_Y, L"選択する", color);
    }
};

} // namespace mc

