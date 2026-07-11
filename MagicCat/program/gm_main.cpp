#include <dxe.h>
#include "gm_main.h"

#include <RenderUtils.h>
#include <windows.h>
#include <ResourceConstantHedder.h>
#include <RandomUtils.h>

import AssetService;
import CardService;
import InputService;
import SceneService;
import ConfigService;
import PersistenceService;
import AudioService;
import RenderService;
import BattleService;
import OSService;
import Displayer;
using namespace mc;

/// @brief 全サービスの所有権を保持する構造体。
/// メンバーは依存トポロジー順（被依存側が先）に宣言し、
/// RAII デストラクタによって逆順に解放される。
struct GameContext {
    std::unique_ptr<IRenderService>  renderService;   // 依存なし
    std::unique_ptr<IConfigService>  configService;   // 依存なし（IConfigService を提供）
    std::unique_ptr<IPersistenceService> persistenceService; // 依存なし（IPersistenceService を提供）
    std::unique_ptr<IAssetService>   assetService;    // 依存なし
    std::unique_ptr<ICardService>    cardService;     // configService に依存
    std::unique_ptr<IBattleService>  battleService;   // configService、persistenceService、cardService、assetService に依存
    std::unique_ptr<IAudioService>   audioService;    // persistenceService、assetService、battleService に依存
    std::unique_ptr<IInputService>   inputService;    // 依存なし
    std::unique_ptr<IOSService>      osService;       // 依存なし
    std::unique_ptr<ISceneService>   sceneService;    // renderService、inputService、osService に依存
    std::unique_ptr<MenuIconDisplayer> menuIconDisplayer; // sceneService、inputService、renderService、assetService、osService に依存
};

/// @brief GameStart の static GameContext へのポインタ。
/// file-scope に std::unique_ptr は持たず、生ポインタのみを残す。
static GameContext* g_ctx = nullptr;

void GameStart()
{
#ifdef _DEBUG
    Random::Seed(0);
#else
    std::random_device rd;
    Random::Seed(rd());
#endif
    static GameContext ctx;
    g_ctx = &ctx;

    ctx.renderService  = CreateRenderService();                                              // 依存なし
    ctx.configService  = CreateConfigService("resource/json/card_config.json",
                                             "resource/json/enemy_config.json",
                                             "resource/json/game_config.json");              // 依存なし
    ctx.persistenceService = CreatePersistenceService();                                     // 依存なし
    ctx.assetService   = CreateAssetService();                                               // 依存なし
    ctx.inputService   = CreateInputService();                                               // 依存なし
    ctx.osService      = CreateWindowService();                                              // 依存なし
    ctx.cardService    = CreateCardService(*ctx.configService);                              // IConfigService に依存
    ctx.sceneService   = CreateSceneService(ctx.renderService.get(),
                                            ctx.inputService.get(),
                                            ctx.osService.get());                           // render、input、os に依存
    ctx.battleService  = CreateBattleService(*ctx.configService, *ctx.persistenceService,
                                             *ctx.cardService, *ctx.assetService);          // config + card + asset に依存
    ctx.audioService   = CreateAudioService(*ctx.persistenceService, *ctx.assetService,
                                             *ctx.battleService);                            // persistence + asset + battle に依存

    ctx.sceneService->RegisterScene(ESceneState::Info,
                                    CreateInfoScene(*ctx.inputService, *ctx.sceneService,
                                                    *ctx.renderService, *ctx.battleService));
    ctx.sceneService->RegisterScene(ESceneState::Combat,
                                    CreateCombatScene(*ctx.sceneService, *ctx.assetService,
                                                      *ctx.cardService, *ctx.inputService,
                                                      *ctx.renderService, *ctx.battleService,
                                                      *ctx.osService));
    ctx.sceneService->RegisterScene(ESceneState::Menu,
                                    CreateMenuScene(*ctx.inputService, *ctx.sceneService,
                                                    *ctx.assetService, *ctx.renderService,
                                                    *ctx.audioService, *ctx.persistenceService,
                                                    *ctx.battleService, *ctx.osService));
    ctx.sceneService->RegisterScene(ESceneState::Cutscene,
                                    CreateCutsceneScene(*ctx.sceneService, *ctx.assetService,
                                                        *ctx.renderService, *ctx.battleService));

    ctx.menuIconDisplayer = CreateMenuIconDisplayer(
        *ctx.sceneService, *ctx.inputService, *ctx.renderService, *ctx.assetService, *ctx.osService);

    AddFontResourceEx(FILE_PATH_OTF_UNIFONT_17, FR_PRIVATE, nullptr);
    ctx.renderService->SetFontTypeNormal();
    ctx.renderService->ChangeFont(FONT_NAME_UNIFONT);
    ctx.renderService->SetFontSize(FONT_SIZE);
    ctx.renderService->SetFontTypeNormal();
    ctx.renderService->SetFontThickness(1);
    ctx.renderService->SetBackgroundColor(
        (COLOR_BG >> 16) & 0xFF,
        (COLOR_BG >> 8) & 0xFF,
        (COLOR_BG) & 0xFF);
}

void GameMain(float deltaTime)
{
    g_ctx->inputService->Update();
    g_ctx->audioService->Update(deltaTime);
    g_ctx->sceneService->Update(deltaTime);
    g_ctx->menuIconDisplayer->Update(deltaTime);
    g_ctx->menuIconDisplayer->Draw(deltaTime);
}

void GameEnd()
{
    RemoveFontResourceEx(FILE_PATH_OTF_UNIFONT_17, FR_PRIVATE, nullptr);
}
