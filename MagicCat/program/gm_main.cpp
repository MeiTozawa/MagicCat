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
Shared<IRenderService> renderService;
Shared<IConfigService> configService;
Shared<IAssetService> assetService;
Shared<ICardService> cardService;
Shared<ICharacterService> characterService;
Shared<IInputService> inputService;
Shared<IAudioService> audioService;
Shared<ISceneService> sceneService;
Shared<IGameService> gameService;

void InitGameServices()
{
    ChangeFontType(DX_FONTTYPE_NORMAL);
    
    // 1. Create independent services
    renderService = CreateRenderService();
    configService = CreateConfigService("resource/json/card_config.json", "resource/json/enemy_config.json");
    assetService = CreateAssetService();
    inputService = CreateInputService();

    // 2. Create services that depend on config
    cardService = CreateCardService(configService.get());
    characterService = CreateCharacterService(configService.get());

    // 3. Audio Service depends on Asset and Character
    audioService = CreateAudioService(assetService.get(), characterService.get());

    // 4. Scene Service needs CharacterService
    sceneService = CreateSceneService(characterService.get());

    // 5. Register Scenes with their dependencies
    sceneService->RegisterScene(Info, CreateInfoScene(inputService.get(), sceneService.get(), renderService.get()));
    sceneService->RegisterScene(Combat, CreateCombatScene(characterService.get(), sceneService.get(), assetService.get(), cardService.get(), inputService.get(), renderService.get()));
    sceneService->RegisterScene(Rules, CreateRulesScene(inputService.get(), sceneService.get(), assetService.get(), renderService.get()));

    // 6. Game Service needs SceneService
    gameService = CreateGameService(sceneService.get());
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
