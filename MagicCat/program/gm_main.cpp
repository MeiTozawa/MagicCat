#include <dxe.h>
#include "gm_main.h"

import GameService;
import ServiceLocator;
import AssetService;
import CardService;
import CharacterService;
import InputService;
import SceneService;

using namespace mc;

IGameService* gameService;

void InitGameServices()
{
    ServiceLocator::RegisterSingleton<IAssetService>(CreateAssetService());
    ServiceLocator::Get<IAssetService>()->LoadAssets();
    
    ServiceLocator::RegisterSingleton<ICardService>(CreateCardService());
    ServiceLocator::RegisterSingleton<ICharacterService>(CreateCharacterService());
    ServiceLocator::RegisterSingleton<IInputService>(CreateInputService());
    ServiceLocator::RegisterSingleton<ISceneService>(CreateSceneService());
    ServiceLocator::RegisterSingleton<IGameService>(CreateGameService());

    if (auto sceneService = ServiceLocator::Get<ISceneService>())
    {
        sceneService->RegisterScene(Start, CreateStartScene());
        sceneService->RegisterScene(Combat, CreateCombatScene());
    }
}

void GameStart()
{
    InitGameServices();
    SetFontSize(32);
    SetBackgroundColor(7 , 31, 56);
    gameService = ServiceLocator::Get<IGameService>();
    gameService->Start();
}

//------------------------------------------------------------------------------------------------------------
// 毎フレーム実行されます
void GameMain(float deltaTime)
{
    gameService->Update(deltaTime);
}

//------------------------------------------------------------------------------------------------------------
// ゲーム終了時に一度だけ実行されます
void GameEnd()
{
    gameService->End();
}

