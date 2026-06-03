#include <time.h>
#include <string>
#include <fstream>
#include <dxe.h>
#include "../ResourceConstantHedder.h"
#include "gm_main.h"

import GameService;
import ServiceLocator;


Shared<IGameService> gameService;

void gameStart()
{
    gameService = ServiceLocator::Get<IGameService>();
    gameService->Start();
}

//------------------------------------------------------------------------------------------------------------
// 毎フレーム実行されます
void gameMain(float delta_time)
{
    gameService->Update(delta_time);
}

//------------------------------------------------------------------------------------------------------------
// ゲーム終了時に一度だけ実行されます
void gameEnd()
{
    gameService->End();
}

