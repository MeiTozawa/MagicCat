// MagicCat.cpp : アプリケーションのエントリ ポイントを定義します。
//

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

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

static std::chrono::system_clock::time_point clock_start, clock_end;
static std::chrono::system_clock::time_point fps_clock_start, fps_clock_end;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ここにコードを挿入してください。

    // DirectX バージョン設定
    SetUseDirect3DVersion(DX_DIRECT3D_11);

    // Log.txt の出力
    SetOutApplicationLogValidFlag(FALSE);

    // ウインドウが非アクティブでもアプリケーションを動かすか
    SetAlwaysRunFlag(ALLWAYS_RUN_FLAG);

    // 起動時のウインドウ位置設定
    SetWindowPosition(220, 0);

    // 画面モード変更時( とウインドウモード変更時 )にグラフィックスシステムの設定やグラフィックハンドルを
    // リセットするかどうかを設定する
    // Flag :  TRUE=リセットする( デフォルト )  FALSE=リセットしない
    SetChangeScreenModeGraphicsSystemResetFlag(FALSE);

    // ウィンドウモードで起動
    dxe::SetWindowMode(true);

    // ウィンドウサイズ設定
    SetGraphMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32);

    // ディスプレイの拡大設定に関わらず設定されたサイズでウィンドウを作成
    SetWindowSizeExtendRate(1.0);

    // ScreenFlip 実行時にＶＳＹＮＣ待ちをするかどうかを設定する
    SetWaitVSyncFlag(TRUE);

    // 深度バッファのビット幅
    SetZBufferBitDepth(32);
    SetCreateDrawValidGraphZBufferBitDepth(32);

    // exe の二重起動を許可するか
    SetDoubleStartValidFlag(TRUE);

    // 使用する文字コードを UTF-8 に設定
    SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8);

    // ＤＸライブラリ初期化処理
    if (DxLib_Init() == -1)
    {
        // エラーが起きたら直ちに終了
        return -1;
    }
    SetDrawScreen(DX_SCREEN_BACK);

    // マウスポインタの表示状態を設定する
    dxe::SetVisibleMousePointer(true);

    // ファイルドラッグの受付可否
    SetDragFileValidFlag(TRUE);

    HWND hWnd = GetMainWindowHandle();
    HDC hdc = GetDC(hWnd);

    // ImGuiプロシージャを追加
    extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
    SetHookWinProc(ImGui_ImplWin32_WndProcHandler);

    // dxe 初期化
    dxe::Initialize(hInstance, hWnd, hdc, WINDOW_WIDTH, WINDOW_HEIGHT, FIX_FPS, APPLICATION_START_FRAME);

    // リソースパス＆フォント名 参照テーブルを作成
    dxe::SetFilePathDictionary(FILE_PATH_DICTIONARY);
    dxe::SetFontNameToPathDictionary(FONT_NAME_TO_PATH_DICTIONARY);

    // 計測開始時間初期化
    clock_start = std::chrono::system_clock::now();

    // ゲームスタート処理
    GameStart();

    // メッセージループ
    while (1)
    {

        // フレーム間の経過時間
        // マイクロ秒で計測して秒に変換
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

        // 画面をクリア
        ClearDrawScreen();

        // dxe 更新
        dxe::Update(delta_time);

        if (dxe::IsRunApplication()) {
            GameMain(delta_time);
        }

        if (dxe::IsExitApplication()) break;

        // バックバッファをフリップ
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

    const int t_wait = 100;
    // ゲーム側の終了処理
    Sleep(t_wait);
    GameEnd();

    // dxe 解放処理
    dxe::Release();

    // ＤＸライブラリ使用の終了処理
    //  DXライブラリ拡張 DirectX 解放 と衝突するのでコメントアウト
    //DxLib_End();

    ReleaseDC(hWnd, hdc);


    return 0;
}


