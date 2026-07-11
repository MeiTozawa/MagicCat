module;

#include <memory>
#include <string>
#include <RenderUtils.h>

export module Displayer:Combat;
export import :Core;
import AssetService;
import RenderService;
import CardService;
import BattleService;
import InputService;
import OSService;

namespace mc {
    export class IDialogDisplayer : public virtual IDisplayer
    {
    public:
        ~IDialogDisplayer() override = default;

        virtual void SetMessage(const std::wstring& message, uint32_t messageColor) = 0;
    };

    export std::unique_ptr<IDialogDisplayer> CreateDialogDisplayer(
        IRenderService& renderService,
        int centerX,
        int topY);

    export class IAttackDisplayer : public virtual IDisplayer
    {
    public:
        ~IAttackDisplayer() override = default;

        virtual void SetImage(int imageHandle) = 0;
    };

    export std::unique_ptr<IAttackDisplayer> CreateAttackDisplayer(
        IRenderService& renderService,
        float x,
        float y,
        float scale);

    export std::unique_ptr<IDisplayer> CreateCardDisplayer(
        ICardService& cardService,
        IAssetService& assetService,
        IRenderService& renderService);

    export std::unique_ptr<IDisplayer> CreatePlayerStatusDisplayer(
        IBattleService& characterService,
        IRenderService& renderService);

    export std::unique_ptr<IDisplayer> CreateEnemyStatusDisplayer(
        IBattleService& characterService,
        IRenderService& renderService);

    export std::unique_ptr<IDisplayer> CreateControlDisplayer(
        IAssetService& assetService,
        IRenderService& renderService,
        IInputService& inputService,
        IOSService& osService,
        uint32_t color = COLOR_WHITE);
}
