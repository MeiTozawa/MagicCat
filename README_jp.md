# MagicCat

[English](README.md) | 日本語

MagicCat は、Fortune 499 に影響を受けた Windows 専用のミニローグライクカードバトルゲームです。
C++23、[DxLib](https://dxlib.xsrv.jp/)、C++20 モジュール (`.ixx`) を使用しています。

## 必須環境

- **コンパイラ:** Visual Studio 2022 / MSVC v143。Desktop development with C++、C++20 modules support、Windows SDK 10.0.19041.0 以降が必要です。
- **ビルドシステム:** CMake 3.28 以降と Ninja。C++20 モジュールの依存関係スキャンに Ninja が必要です。
- **ライブラリ:** [DxLib Visual C++ 版](https://dxlib.xsrv.jp/dload.html)。`DxLib.h` と `DxLib_x64.lib` を含むローカルディレクトリに展開してください。
- **任意ツール:** `ccache` と `lld-link`。利用可能な場合は CMake が自動検出します。

## クリーン環境でのセットアップ

リポジトリを clone して、リポジトリルートへ移動します。

```powershell
git clone <repository-url> MagicCat
cd MagicCat
```

リポジトリルートに `CMakeUserPresets.json` を作成します。このファイルは開発者ごとの DxLib パスを含むため、Git 管理外です。

```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "windows-default",
      "displayName": "Windows Default Config (Ninja)",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/cmake-build-debug",
      "cacheVariables": {
        "DXLIB_DIR": "C:/DxLib_VC/プロジェクトに追加すべきファイル_VC用",
        "CMAKE_BUILD_TYPE": "Debug",
        "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
      }
    }
  ]
}
```

`DXLIB_DIR` は実際に DxLib を展開したパスへ変更してください。パスにはスラッシュ (`/`) またはエスケープ済みバックスラッシュ (`\\`) を使います。

Microsoft Visual Studio Code で公式 **CMake Tools** と **C/C++** extension を使う場合、通常は Visual Studio 2022 amd64 CMake kit を選択すれば十分です。手動で `vcvars64.bat` を呼び出す必要はありません。

Visual Studio Code や Visual Studio の外で command-line build を直接行う場合は、**Developer PowerShell for VS 2022** を使ってください。通常の PowerShell で MSVC を初期化する必要がある場合のみ、以下を実行します。
```powershell
& "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
```

command-line build を直接行う場合は、compiler が利用可能か確認します。

```powershell
cl
```

## ビルドコマンド

リポジトリルートから構成します。

```powershell
cmake --preset windows-default
```

このコマンドは `CMakeUserPresets.json` を読み取り、Ninja を使用し、`cmake-build-debug/` にビルドファイルを生成し、`DXLIB_DIR` を設定し、`/utf-8` と Windows Unicode ビルド用の定義を有効化し、テスト依存関係を取得し、C++20 モジュール依存関係をスキャンします。

期待される出力例:

```text
-- Configuring done
-- Generating done
-- Build files have been written to: <repo-root>/cmake-build-debug
```

全ターゲットをビルドします。

```powershell
cmake --build cmake-build-debug
```

特定ターゲットだけをビルドする場合:

```powershell
cmake --build cmake-build-debug --target MagicCat
cmake --build cmake-build-debug --target MagicCatTests
cmake --build cmake-build-debug --target MagicCatCore
```

ビルドターゲット:

- `MagicCatCore`: ゲームロジックと C++20 モジュールを含む static library。
- `MagicCat`: WIN32 ゲーム実行ファイル。
- `MagicCatTests`: GoogleTest、GMock、RapidCheck、RapidCheck GTest integration を使用するテスト実行ファイル。

初回ビルドではすべての `.ixx` ファイルをスキャンするため、数分かかる場合があります。以降のビルドではキャッシュされたモジュール依存情報が再利用されます。モジュール BMI ファイル (`.ifc`) は `cmake-build-debug/MagicCat/CMakeFiles/MagicCatCore.dir/` に生成されます。

## テスト実行

リポジトリルートからテストをビルドして直接実行します。

```powershell
cmake --build cmake-build-debug --target MagicCatTests
.\cmake-build-debug\tests\MagicCatTests.exe
```

便利な GoogleTest オプション:

```powershell
.\cmake-build-debug\tests\MagicCatTests.exe --gtest_filter="BattleService*"
.\cmake-build-debug\tests\MagicCatTests.exe --gtest_list_tests
.\cmake-build-debug\tests\MagicCatTests.exe --gtest_verbose
```

CTest で実行する場合:

```powershell
ctest --test-dir cmake-build-debug --output-on-failure
ctest --test-dir cmake-build-debug -j 8 --output-on-failure
ctest --test-dir cmake-build-debug -R "BattleService" --verbose
```

CTest は CMake configure 時に GoogleTest のテストケースを検出します。RapidCheck のプロパティベーステストは入力生成を行うため、時間がかかる場合があります。

## 実行時要件

ゲームと一部の統合テストは、リソースパス解決のためリポジトリルートから実行する必要があります。

```text
MagicCat/                          <- ここから実行
├── MagicCat/
│   └── resource/
│       ├── Json/
│       │   ├── card_config.json
│       │   ├── enemy_config.json
│       │   └── game_config.json
│       ├── Images/
│       ├── Sounds/
│       └── Fonts/
└── cmake-build-debug/
    ├── MagicCat/
    │   └── MagicCat.exe
    └── tests/
        └── MagicCatTests.exe
```

正しいコマンドライン実行:

```powershell
.\cmake-build-debug\MagicCat\MagicCat.exe
```

`cmake-build-debug/MagicCat/` から直接実行すると、`MagicCat/resource/...` のリソースを読み込めません。

IDE の作業ディレクトリ設定:

- **Visual Studio / Rider:** 作業ディレクトリを `${ProjectDir}` またはリポジトリルートに設定します。
- **CLion:** Run configuration の working directory をリポジトリルートに設定します。
- **VS Code:** `launch.json` の `cwd` を `${workspaceFolder}` に設定します。

## ビルド再現性チェックリスト

### 必須環境

- [ ] Visual Studio 2022 がインストール済み。Desktop development with C++、C++20 modules support、Windows SDK 10.0.19041.0 以降を含む。
- [ ] CMake 3.28 以降が PATH にある。`cmake --version` で確認済み。
- [ ] Ninja が PATH にある。`ninja --version` で確認済み。
- [ ] DxLib Visual C++ 版をダウンロードし展開済み。
- [ ] `DxLib.h` と `DxLib_x64.lib` を含むディレクトリのパスを把握済み。

### 初期セットアップ

- [ ] リポジトリを clone 済み。
- [ ] ターミナルの現在位置がリポジトリルート。
- [ ] `CMakeUserPresets.json` をリポジトリルートに作成済み。
- [ ] `DXLIB_DIR` が実際の DxLib パスを指している。
- [ ] プリセット名が `windows-default`、generator が `Ninja`。
- [ ] command-line build を直接行う場合、Developer PowerShell を開いている、または `vcvars64.bat` で MSVC を初期化済み。
- [ ] Visual Studio Code で build する場合、Visual Studio 2022 amd64 CMake kit を選択済み。
- [ ] command-line build を直接行う場合、`cl` が MSVC compiler version を表示する。

### ビルド

- [ ] `cmake --preset windows-default` が `DXLIB_DIR`、Ninja、compiler エラーなしで完了する。
- [ ] configure 出力に `-- Configuring done` と `-- Generating done` が表示される。
- [ ] `cmake-build-debug/` が作成される。
- [ ] `cmake --build cmake-build-debug` がコンパイルエラーなしで完了する。
- [ ] 初回ビルドで C++20 モジュールスキャンが完了する。
- [ ] `cmake-build-debug\MagicCat\MagicCat.exe` が存在する。
- [ ] `cmake-build-debug\tests\MagicCatTests.exe` が存在する。
- [ ] `cmake-build-debug\MagicCat\MagicCatCore.lib` が存在する。

### テスト

- [ ] テストをリポジトリルートから実行している。
- [ ] `.\cmake-build-debug\tests\MagicCatTests.exe` がリソースパスエラーなしで実行される。
- [ ] `ctest --test-dir cmake-build-debug --output-on-failure` がテストを検出して実行する。
- [ ] 失敗がある場合は内容を記録している。

### 実行時

- [ ] 現在位置がリポジトリルート。
- [ ] `MagicCat/resource/` が存在する。
- [ ] `MagicCat/resource/Json/card_config.json` が存在する。
- [ ] `MagicCat/resource/Json/enemy_config.json` が存在する。
- [ ] `MagicCat/resource/Json/game_config.json` が存在する。
- [ ] `.\cmake-build-debug\MagicCat\MagicCat.exe` がリソースエラーなしで起動する。
- [ ] 必要なフォントが利用可能な環境で日本語テキストが正しく表示される。

### CI / 自動化

- [ ] CI 環境に MSVC Build Tools 2022 と Windows SDK がある。
- [ ] CI 環境に Ninja がある。
- [ ] CI script が `vcvars64.bat` で MSVC を初期化する。
- [ ] CI で DxLib が cache、download、または承認済みの方法で利用できる。
- [ ] CMake が preset または `-DDXLIB_DIR=<path>` で `DXLIB_DIR` を受け取る。
- [ ] テストを `--output-on-failure` 付きで実行する。
- [ ] 必要に応じて `MagicCat.exe`、`MagicCatTests.exe`、テスト結果を artifacts として保存する。

## ビルド生成物

生成物はコミットしません。

```text
cmake-build-debug/          # Debug ビルド出力
cmake-build-release/        # Release ビルド出力
CMakeUserPresets.json       # マシン固有の DxLib パス
build_tests.bat             # マシン固有のビルドスクリプト
build.bat                   # マシン固有のビルドスクリプト
*.ifc                       # モジュール BMI ファイル
*.obj                       # オブジェクトファイル
*.pdb                       # デバッグシンボル
*.ilk                       # インクリメンタルリンクファイル
```

ソース管理される主な場所:

```text
CMakeLists.txt              # ルートビルド設定
MagicCat/CMakeLists.txt     # ゲームターゲット設定
tests/CMakeLists.txt        # テストターゲット設定
common/                     # third-party libraries
MagicCat/program/           # ゲームソースとモジュール
MagicCat/resource/          # ゲーム assets と configs
tests/test_*.cpp            # テストソース
tests/MockServices.h        # テスト用 mocks
README.md                   # 英語版セットアップとビルド文書
README_jp.md                # 日本語版セットアップとビルド文書
```

## C++20 モジュール

MagicCat はゲームコードに C++20 モジュールを使用しています。

- モジュールインターフェースは `MagicCat/program/**/*.ixx`。
- モジュール実装は `MagicCat/program/**/*.cpp`。
- `Displayer` module は `Displayer:Core`、`Displayer:Combat`、`Displayer:Cutscene`、`Displayer:Menu`、`Displayer:MenuIcon`、`Displayer:Sprite` などの partition を使います。

CMake の MSVC module support は Ninja の依存関係スキャンに依存するため、Ninja が必要です。モジュールビルドでは `.ixx` をスキャンし、依存グラフを作成し、依存順に compile し、`.ifc` を生成して link します。

`MagicCatCore` は compile 高速化のため `UNITY_BUILD ON` を使用します。また、DxLib を多く使う翻訳単位向けに `<dxe.h>` を public precompiled header として使用します。

## コンパイラとリンカ設定

`MagicCatCore` の主な compile definitions:

- `NOMINMAX`: Windows の `min` / `max` macro と `std::min` / `std::max` の衝突を防ぎます。
- `WIN32` と `_WINDOWS`: Windows platform 用定義。
- `UNICODE` と `_UNICODE`: Unicode build 用定義。

主な compile options:

- `/utf-8`: source と execution character set を UTF-8 にします。
- `-std:c++latest`: C++23 と C++20 modules を使用します。
- `/Ob0 /Od /RTC1`: Debug 用。
- `/O2 /Ob2`: Release 用。
- `/MDd`: Debug 用 dynamic CRT。
- `/MD`: Release 用 dynamic CRT。
- `/Zi`: debug symbols。
- `/bigobj`: `MagicCatTests` 用。

実行ファイルの linker option には `/SUBSYSTEM:WINDOWS`、`/DEBUG`、Debug の `/INCREMENTAL`、Release の `/INCREMENTAL:NO` が含まれます。`lld-link` が見つかった場合、CMake が設定済みの高速 linker option を追加することがあります。

## トラブルシューティング

| 症状 | 主な原因 | 対処 |
|------|----------|------|
| `stdio.h not found` または標準ライブラリエラー | MSVC 環境が初期化されていない | Developer PowerShell for VS 2022 を開くか `vcvars64.bat` を実行する |
| `Generator not found: Ninja` | Ninja が未インストール、または PATH にない | Ninja をインストールし PATH に追加し、terminal を再起動して `ninja --version` を確認する |
| `DXLIB_DIR is not set or does not exist` | `CMakeUserPresets.json` がない、または内容が誤っている | preset file を作り、`DXLIB_DIR` を `DxLib.h` を含む directory に向ける |
| `error C7612: could not find module 'ModuleName'` | module build order の問題、または stale BMI cache | `cmake-build-debug/` を削除し、再 configure / rebuild する |
| game や tests で resource not found | 作業ディレクトリが違う | リポジトリルートから実行する |
| module scanning が長い | 初回 C++20 module build | 初回は待つ。以降は通常速くなる |

クリーンリビルド:

```powershell
Remove-Item -Recurse -Force cmake-build-debug
cmake --preset windows-default
cmake --build cmake-build-debug
```

ビルドディレクトリを消さずに module rescan を促す場合:

```powershell
(Get-Item MagicCat\program\EventBus.ixx).LastWriteTime = Get-Date
cmake --build cmake-build-debug
```

## IDE 設定

### Visual Studio 2022

- `File > Open > Folder...` でリポジトリルートを開きます。
- `windows-default` CMake preset を選択します。
- `MagicCat.exe` の working directory をリポジトリルートに設定します。

### Rider / CLion

- リポジトリルートを開きます。
- `windows-default` CMake profile / preset を選択します。
- Run configuration の working directory をリポジトリルートに設定します。

### VS Code

この project の MSVC / CMake 開発には Microsoft Visual Studio Code を使用してください。

Cursor や Kiro のような third-party VS Code distribution を、この project の主要な C++ IDE として使わないでください。これらは Microsoft 公式 **C/C++** extension を Visual Studio Code と同じ licensing / distribution model で利用できないため、MSVC compiler discovery、IntelliSense、CMake kit integration が正しく動作しない場合があります。

- Visual Studio Code に **CMake Tools** と Microsoft 公式 **C/C++** extension をインストールします。
- リポジトリルートを開きます。
- Visual Studio 2022 amd64 CMake kit を選択します。
- `CMake: Configure` を実行します。
- `launch.json` に `"cwd": "${workspaceFolder}"` を設定します。

Visual Studio Code で `module;` に syntax error が出る場合、IntelliSense が CMake 由来の C++23/module flags を使っていません。有効な CMake kit を選択して configure を完了すると解消することが多いです。

## 既知の制限

- DxLib に依存するため Windows 専用です。
- MSVC v143 前提です。他 compiler はこの project では support していません。
- x64 build 前提です。32-bit build は未テストです。
- この CMake / MSVC 構成では C++20 modules に Ninja が必要です。
- runtime resource path は process がリポジトリルートから起動する前提です。
- 初回 build は module dependency scanning のため遅くなります。
