#include <dxe.h>
#include "gm_main.h"
#include <windows.h>

#include <ResourceConstantHedder.h>

import GameService;
import AssetService;
import CardService;
import CharacterService;
import InputService;
import SceneService;
import ConfigService;
import AudioService;
import RenderService;
using namespace mc;

// Globals to keep Services alive
std::unique_ptr<IRenderService> renderService;
std::unique_ptr<IConfigService> configService;
std::unique_ptr<IAssetService> assetService;
std::unique_ptr<ICardService> cardService;
std::unique_ptr<ICharacterService> characterService;
std::unique_ptr<IInputService> inputService;
std::unique_ptr<IAudioService> audioService;
std::unique_ptr<ISceneService> sceneService;
std::unique_ptr<IGameService> gameService;

void InitGameServices()
{
    ChangeFontType(DX_FONTTYPE_NORMAL);
    
    // 1. Create independent services
    renderService = CreateRenderService();
    configService = CreateConfigService("resource/json/card_config.json", "resource/json/enemy_config.json");
    assetService = CreateAssetService();
    inputService = CreateInputService();

    // 2. Create services that depend on config
    cardService = CreateCardService(*configService);
    characterService = CreateCharacterService(*configService);

    // 3. Audio Service depends on Asset and Character
    audioService = CreateAudioService(*assetService, *characterService);

    // 4. Scene Service needs CharacterService
    sceneService = CreateSceneService(*characterService);

    // 5. Register Scenes with their dependencies
    sceneService->RegisterScene(ESceneState::Info, CreateInfoScene(*inputService, *sceneService, *renderService));
    sceneService->RegisterScene(ESceneState::Combat, CreateCombatScene(*characterService, *sceneService, *assetService, *cardService, *inputService, *renderService));
    sceneService->RegisterScene(ESceneState::Rules, CreateRulesScene(*inputService, *sceneService, *assetService, *renderService));

    // 6. Game Service needs SceneService
    gameService = CreateGameService(*sceneService);
}

void GameStart()
{
    InitGameServices();
    AddFontResourceEx(FILE_PATH_OTF_UNIFONT_17, FR_PRIVATE, nullptr);
    ChangeFont(FONT_NAME_UNIFONT); 
    SetFontSize(48);
    ChangeFontType(DX_FONTTYPE_NORMAL);
    SetFontThickness(1);
    SetBackgroundColor(7, 31, 56);
    gameService->Start();
}

void GameMain(float deltaTime)
{
    gameService->Update(deltaTime);
}

void GameEnd()
{
    gameService->End();
}
