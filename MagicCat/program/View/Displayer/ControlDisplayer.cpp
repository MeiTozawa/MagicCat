module;

module Displayer;

import ServiceLocator;
import AssetService;

namespace mc
{
    namespace {
        constexpr int KB_Q_X = 100;
        constexpr int KB_UP_X = 500;
        constexpr int KB_DOWN_X = 560;
        constexpr int KB_SPACE_X = 620;
        constexpr int Y = 100;

        constexpr int TEXT_OFFSET_X = 40;
        constexpr int TEXT_OFFSET_Y = -15;
    }

    class ControlDisplayer : public IDisplayer
    {
        IAssetService* assetService;
        uint32_t color;

    public:
        ControlDisplayer(uint32_t color) : color(color)
        {
            assetService = ServiceLocator::Get<IAssetService>();
        }

        void Update(float deltaTime) override {}

        void Draw(float deltaTime) const override
        {
            int icon = assetService->GetImageHandle(EImage::KB_Q);
            DrawRotaGraphF(KB_Q_X, Y, 1.0, 0.0, icon, TRUE);
            DrawString(KB_Q_X + TEXT_OFFSET_X, Y + TEXT_OFFSET_Y, L"カードを引く", color);

            icon = assetService->GetImageHandle(EImage::KB_UP);
            DrawRotaGraphF(KB_UP_X, Y, 1.0, 0.0, icon, TRUE);
            
            icon = assetService->GetImageHandle(EImage::KB_DOWN);
            DrawRotaGraphF(KB_DOWN_X, Y, 1.0, 0.0, icon, TRUE);
            
            icon = assetService->GetImageHandle(EImage::KB_SPACE);
            DrawRotaGraphF(KB_SPACE_X, Y, 1.0, 0.0, icon, TRUE);
            DrawString(KB_SPACE_X + TEXT_OFFSET_X, Y + TEXT_OFFSET_Y, L"選択する", color);
        }
    };

    std::unique_ptr<IDisplayer> CreateControlDisplayer(uint32_t color)
    {
        return std::make_unique<ControlDisplayer>(color);
    }
} // namespace mc
