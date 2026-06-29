#include <dxe.h>
#include "gm_main.h"

#include <RenderUtils.h>
#include <windows.h>

#include <ResourceConstantHedder.h>

import AssetService;
import CardService;
import InputService;
import SceneService;
import ConfigService;
import AudioService;
import RenderService;
import BattleService;
using namespace mc;

// Globals to keep Services alive
std::unique_ptr<IRenderService> renderService;
std::unique_ptr<IConfigService> configService;
std::unique_ptr<IAssetService> assetService;
std::unique_ptr<ICardService> cardService;
std::unique_ptr<IInputService> inputService;
std::unique_ptr<IAudioService> audioService;
std::unique_ptr<ISceneService> sceneService;
std::unique_ptr<IBattleService> BattleService;

void InitGameServices()
{
    // 1. Create independent services
    renderService = CreateRenderService();
    configService = CreateConfigService("resource/json/card_config.json", "resource/json/enemy_config.json");
    assetService = CreateAssetService();
    inputService = CreateInputService();

    // 2. Create services that depend on config
    cardService = CreateCardService(*configService);

    // 3. Scene Service — pass renderService for fade transitions
    sceneService = CreateSceneService(renderService.get());

    // 4. Battle Sequence Service depends on config and card.
    //    It also owns the player/enemy characters.
    BattleService = CreateBattleService(*configService, *cardService);

    // 5. Audio Service depends on Asset and the Battle Sequence Service (characters)
    audioService = CreateAudioService(*assetService, *BattleService);

    // 6. Register Scenes with their dependencies
    sceneService->RegisterScene(ESceneState::Info,
                                CreateInfoScene(*inputService, *sceneService, *renderService, *BattleService));
    sceneService->RegisterScene(ESceneState::Combat,
                                CreateCombatScene(*sceneService, *assetService, *cardService, *inputService,
                                                  *renderService, *BattleService));
    sceneService->RegisterScene(ESceneState::Rules,
                                CreateRulesScene(*inputService, *sceneService, *assetService, *renderService));
    sceneService->RegisterScene(ESceneState::Cutscene,
                                CreateCutsceneScene(*sceneService, *assetService, *renderService, *BattleService));
}

void GameStart()
{
    ChangeFontType(DX_FONTTYPE_NORMAL);
    InitGameServices();
    AddFontResourceEx(FILE_PATH_OTF_UNIFONT_17, FR_PRIVATE, nullptr);
    ChangeFont(FONT_NAME_UNIFONT);
    SetFontSize(48);
    ChangeFontType(DX_FONTTYPE_NORMAL);
    SetFontThickness(1);
    SetBackgroundColor(COLOR_BG << 8 >> 24, COLOR_BG << 16 >> 24, COLOR_BG << 24 >> 24);
}

void GameMain(float deltaTime)
{
    sceneService->Update(deltaTime);
}

void GameEnd() {}
