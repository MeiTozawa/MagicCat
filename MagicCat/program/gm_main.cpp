#include <dxe.h>
#include "gm_main.h"

import GameService;
import ServiceLocator;
import ConfigService;
import AssetService;
import CardService;
import CharacterService;
import InputService;
import SceneService;
import AudioService;
using namespace mc;

IGameService* gameService;

void InitGameServices()
{
    ChangeFontType(DX_FONTTYPE_NORMAL);
    ServiceLocator::RegisterSingleton<IConfigService>(CreateConfigService());
    ServiceLocator::RegisterSingleton<IAssetService>(CreateAssetService());
    ServiceLocator::RegisterSingleton<ICardService>(CreateCardService());
    ServiceLocator::RegisterSingleton<ICharacterService>(CreateCharacterService());
    ServiceLocator::RegisterSingleton<IInputService>(CreateInputService());
    ServiceLocator::RegisterSingleton<IAudioService>(CreateAudioService());
    ServiceLocator::RegisterSingleton<IGameService>(CreateGameService());

    ServiceLocator::RegisterSingleton<ISceneService>(CreateSceneService());
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
    SetBackgroundColor(7, 31, 56);
    gameService = ServiceLocator::Get<IGameService>();
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
