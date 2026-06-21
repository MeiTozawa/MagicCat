# MagicCat

## 🇺🇸 English

This is a mini roguelike card game inspired by Fortune 499.

A C++23 game project utilizing [DxLib](https://dxlib.xsrv.jp/) and modern C++20 Modules (`.ixx`).

### Requirements
* **C++ Compiler**: Visual Studio 2022 (MSVC v143) with C++20 Modules support.
* **Build System**: CMake (3.28+) and **Ninja** (Required for C++20 modules scanning).
* **Library**: [DxLib (Visual C++ Version)](https://dxlib.xsrv.jp/dload.html).

### Setup Instructions

1. **Download DxLib**
   Download the Visual C++ version of DxLib and extract it to a folder on your computer (e.g., `C:/DxLib_VC/プロジェクトに追加すべきファイル_VC用`).

2. **Configure Local Environment (CMake Presets)**
   Because DxLib's path is different for every developer, this project uses `CMakeUserPresets.json` to manage local paths.
   
   Create a file named `CMakeUserPresets.json` in the root directory of the project (this file is git-ignored):
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
           "DXLIB_DIR": "C:/DxLib/プロジェクトに追加すべきファイル_VC用",
           "CMAKE_BUILD_TYPE": "Debug"
         }
       }
     ]
   }
   ```
   *Make sure to change the `DXLIB_DIR` value to your actual extracted DxLib path.*

3. **Build the Project**
   Open the project in a CMake-compatible IDE (CLion, Visual Studio, or VS Code), select the `windows-default` preset, and build. 
   
   If using the command line:
   ```bash
   cmake --preset windows-default
   cmake --build cmake-build-debug
   ```

4. **Run**
   Make sure the **Working Directory** in your IDE's Run Configuration is set to the project root folder (where this `README.md` is located), otherwise the game will fail to load image resources.

### Troubleshooting

**VS Code: Error at `module;` (C++20 Modules)**
If you open this project in VS Code and see syntax errors under `module;`, it means IntelliSense is not properly configured for C++23.
**Solution:**
1. Install the **CMake Tools** and **C/C++** extensions.
2. Select a valid CMake Kit in the status bar (e.g., `Visual Studio Community 2022 Release - amd64`).
3. Let CMake configure the project. Once configured, CMake will provide the correct C++23 standard and module flags to IntelliSense, and the errors will disappear.

---

## 🇯🇵 日本語

[DxLib](https://dxlib.xsrv.jp/) と最新の C++20 モジュール (`.ixx`) を活用した C++23 ゲームプロジェクト。

これはミニローグライクカードゲームで、ゲームプレイの設計は『Fortune 499』を参考にしています。

### 必須環境
* **C++コンパイラ**: Visual Studio 2022 (MSVC v143) (C++20 Modules サポート必須)
* **ビルドシステム**: CMake (3.28以上) および **Ninja** (C++20 モジュールのスキャンに必須)
* **ライブラリ**: [DxLib (Visual C++用)](https://dxlib.xsrv.jp/dload.html)

### セットアップ手順

1. **DxLibのダウンロード**
   DxLibのVisual C++用パッケージをダウンロードし、任意のフォルダに解凍します（例: `C:/DxLib_VC/プロジェクトに追加すべきファイル_VC用`）。

2. **ローカル環境の設定 (CMake Presets)**
   DxLibの保存場所は開発者ごとに異なるため、このプロジェクトでは `CMakeUserPresets.json` を使ってローカルパスを管理します。

   プロジェクトのルートディレクトリに `CMakeUserPresets.json` というファイルを作成してください（このファイルはGitの管理外になります）:
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
           "DXLIB_DIR": "C:/DxLib/プロジェクトに追加すべきファイル_VC用",
           "CMAKE_BUILD_TYPE": "Debug"
         }
       }
     ]
   }
   ```
   ※ `DXLIB_DIR` の値は、実際にDxLibを解凍したパスに書き換えてください。

3. **プロジェクトのビルド**
   CLion、Visual Studio、VS CodeなどのCMake対応IDEでプロジェクトを開き、`windows-default` プリセットを選択してビルドします。

   コマンドラインを使用する場合:
   ```bash
   cmake --preset windows-default
   cmake --build cmake-build-debug
   ```

4. **実行時の注意**
   IDEの実行構成（Run Configuration）で、**「作業ディレクトリ（Working Directory）」** をプロジェクトのルートフォルダ（この `README.md` がある場所）に設定して実行してください。設定しない場合、画像などのリソースファイルの読み込みに失敗します。

### トラブルシューティング

**VS Code: `module;` でエラーが出る場合**
VS Codeでプロジェクトを開いた際、`module;` に構文エラー（赤い波線）が表示される場合、IntelliSenseがC++23としてコードを認識していません。
**解決策:**
1. **CMake Tools** と **C/C++** 拡張機能をインストールしてください。
2. VS Code下部のステータスバーから、適切なキット (Kit) を選択してください (例: `Visual Studio Community 2022 Release - amd64`)。
3. CMakeの構成 (Configure) を完了させてください。完了すると、CMakeがIntelliSenseに正しいC++23標準とモジュールのフラグを渡し、エラーが消えます。
