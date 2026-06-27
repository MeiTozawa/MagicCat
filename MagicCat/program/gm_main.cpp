#include <dxe.h>
#include "gm_main.h"
#include <windows.h>

#include <ResourceConstantHedder.h>

import GameService;
import AssetService;
import CardService;
import InputService;
import SceneService;
import ConfigService;
import AudioService;
import RenderService;
import BattleService;
using namespace mc;

// Forward declarations for scene factory functions defined in module SceneService
// implementation units (InfoScene.cpp, CombatScene.cpp). These are not exported
// through SceneService.ixx to avoid a circular module dependency.
namespace mc {
    std::unique_ptr<IScene> CreateInfoScene(IInputService& inputService, ISceneService& sceneService, IRenderService& renderService, IBattleService& BattleService);
    std::unique_ptr<IScene> CreateCombatScene(ISceneService& sceneService, IAssetService& assetService, ICardService& cardService, IInputService& inputService, IRenderService& renderService, IBattleService& BattleService);
}

// Globals to keep Services alive
std::unique_ptr<IRenderService> renderService;
std::unique_ptr<IConfigService> configService;
std::unique_ptr<IAssetService> assetService;
std::unique_ptr<ICardService> cardService;
std::unique_ptr<IInputService> inputService;
std::unique_ptr<IAudioService> audioService;
std::unique_ptr<ISceneService> sceneService;
std::unique_ptr<IBattleService> BattleService;
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

    // 3. Scene Service (no args)
    sceneService = CreateSceneService();

    // 4. Battle Sequence Service depends on config, card, and scene.
    //    It also owns the player/enemy characters.
    BattleService = CreateBattleService(*configService, *cardService, *sceneService);

    // 5. Audio Service depends on Asset and the Battle Sequence Service (characters)
    audioService = CreateAudioService(*assetService, *BattleService);

    // 6. Register Scenes with their dependencies
    sceneService->RegisterScene(ESceneState::Info, CreateInfoScene(*inputService, *sceneService, *renderService, *BattleService));
    sceneService->RegisterScene(ESceneState::Combat, CreateCombatScene(*sceneService, *assetService, *cardService, *inputService, *renderService, *BattleService));
    sceneService->RegisterScene(ESceneState::Rules, CreateRulesScene(*inputService, *sceneService, *assetService, *renderService));

    // 7. Game Service needs SceneService
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
