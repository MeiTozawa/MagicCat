// MagicCat.cpp : アプリケーションのエントリ ポイント

#pragma warning(disable:4819)
#include <Windows.h>
#include <chrono>
#pragma comment(lib,"winmm.lib")
#include "framework.h"
#include "MagicCat.h"
#include "dxe.h"
#include "app_build_setting.h"
#include "program/gm_main.h"
#include "ResourceConstantHedder.h"


#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

static std::chrono::system_clock::time_point clock_start, clock_end;
static std::chrono::system_clock::time_point fps_clock_start, fps_clock_end;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    SetUseDirect3DVersion(DX_DIRECT3D_11);
    SetOutApplicationLogValidFlag(FALSE);
    SetAlwaysRunFlag(ALLWAYS_RUN_FLAG);
    SetWindowPosition(0, 0);

    // 画面モード変更時にグラフィックハンドルをリセットしない
    SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

    dxe::SetWindowMode(true);
    SetGraphMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32);
    SetWindowSizeExtendRate(1.0);
    SetWaitVSyncFlag(TRUE);
    SetZBufferBitDepth(32);
    SetCreateDrawValidGraphZBufferBitDepth(32);
    SetDoubleStartValidFlag(TRUE);
    SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);

    if (DxLib_Init() == -1)
    {
        return -1;
    }
    SetDrawScreen(DX_SCREEN_BACK);

    dxe::SetVisibleMousePointer(true);
    SetDragFileValidFlag(TRUE);

    HWND hWnd = GetMainWindowHandle();
    HDC hdc = GetDC(hWnd);

    extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    SetHookWinProc(ImGui_ImplWin32_WndProcHandler);

    dxe::Initialize(hInstance, hWnd, hdc, WINDOW_WIDTH, WINDOW_HEIGHT, FIX_FPS, APPLICATION_START_FRAME);
    dxe::SetFilePathDictionary(FILE_PATH_DICTIONARY);
    dxe::SetFontNameToPathDictionary(FONT_NAME_TO_PATH_DICTIONARY);

    clock_start = std::chrono::system_clock::now();

    GameStart();

    while (1)
    {
        // フレーム間の経過時間をマイクロ秒で計測して秒に変換
        clock_end = std::chrono::system_clock::now();
        double micro_seconds = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_start).count());
        float delta_time = static_cast<float>(micro_seconds / 1000.0 / 1000.0);
        clock_start = clock_end;

        if (ProcessMessage() == -1) {
            break;
        }

        wchar_t filepath[256] = { 0 };
        if (0 == GetDragFilePath(filepath)) {
            dxe::SetDragFilePath(filepath);
        }
        DragFileInfoClear();

        fps_clock_start = std::chrono::system_clock::now();

        ClearDrawScreen();
        dxe::Update(delta_time);

        if (dxe::IsRunApplication()) {
            GameMain(delta_time);
        }

        if (dxe::IsExitApplication()) break;

        ScreenFlip();

        // フレームレートコントロール
        fps_clock_end = std::chrono::system_clock::now();
        double fps_mic = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(fps_clock_end - fps_clock_start).count());
        float fps_mils = static_cast<float>(fps_mic / 1000.0);
        dxe::SetUnlimitDeltaTime(fps_mils);
        float fps_lim = 1000.0f / (FIX_FPS + 0.5f);

        if (fps_lim > fps_mils) {
            timeBeginPeriod(1);
            Sleep(DWORD(fps_lim - fps_mils));
            timeEndPeriod(1);
        }
    }

    Sleep(100);
    GameEnd();

    dxe::Release();

    // DxLib_End() は dxe::Release() と競合するため呼ばない

    ReleaseDC(hWnd, hdc);

    return 0;
}


