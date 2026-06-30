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

// サービスのライフタイムを管理するグローバル変数
std::unique_ptr<IRenderService> renderService;
std::unique_ptr<IConfigService> configService;
std::unique_ptr<IAssetService> assetService;
std::unique_ptr<ICardService> cardService;
std::unique_ptr<IInputService> inputService;
std::unique_ptr<IAudioService> audioService;
std::unique_ptr<ISceneService> sceneService;
std::unique_ptr<IBattleService> battleService;

void InitGameServices()
{
    configService = CreateConfigService("resource/json/card_config.json", "resource/json/enemy_config.json");
    assetService = CreateAssetService();
    inputService = CreateInputService();

    cardService = CreateCardService(*configService);
    sceneService = CreateSceneService(renderService.get(), inputService.get());
    battleService = CreateBattleService(*configService, *cardService, *assetService);
    audioService = CreateAudioService(*assetService, *battleService);

    sceneService->RegisterScene(ESceneState::Info,
                                CreateInfoScene(*inputService, *sceneService, *renderService, *battleService));
    sceneService->RegisterScene(ESceneState::Combat,
                                CreateCombatScene(*sceneService, *assetService, *cardService, *inputService,
                                                  *renderService, *battleService));
    sceneService->RegisterScene(ESceneState::Rules,
                                CreateRulesScene(*inputService, *sceneService, *assetService, *renderService));
    sceneService->RegisterScene(ESceneState::Cutscene,
                                CreateCutsceneScene(*sceneService, *assetService, *renderService,
                                                    *battleService));
}

void GameStart()
{
    renderService = CreateRenderService();
    InitGameServices();
    AddFontResourceEx(FILE_PATH_OTF_UNIFONT_17, FR_PRIVATE, nullptr);
    renderService->SetFontTypeNormal();
    renderService->ChangeFont(FONT_NAME_UNIFONT);
    renderService->SetFontSize(48);
    renderService->SetFontTypeNormal();
    renderService->SetFontThickness(1);
    renderService->SetBackgroundColor(
        (COLOR_BG >> 16) & 0xFF,
        (COLOR_BG >> 8)  & 0xFF,
        (COLOR_BG)       & 0xFF);
}

void GameMain(float deltaTime)
{
    audioService->Update(deltaTime);
    sceneService->Update(deltaTime);
}

void GameEnd() {}
