# MagicCat

English | [日本語](README_jp.md)

MagicCat is a Windows-only mini roguelike card battle game inspired by Fortune 499.
It is built with C++23, DxLib, and C++20 modules (`.ixx`).

## Requirements

- **Compiler:** Visual Studio 2022 / MSVC v143 with Desktop development with C++, C++20 modules support, and Windows SDK 10.0.19041.0 or later.
- **Build system:** CMake 3.28 or later and Ninja. Ninja is required for C++20 module dependency scanning.
- **Library:** [DxLib Visual C++ version](https://dxlib.xsrv.jp/dload.html), extracted to a local directory that contains `DxLib.h` and `DxLib_x64.lib`.
- **Optional tools:** `ccache` for faster rebuilds and `lld-link` for faster linking. CMake detects them automatically when available.

## Clean Workspace Setup

Clone the repository and enter the repo root:

```powershell
git clone <repository-url> MagicCat
cd MagicCat
```

Create `CMakeUserPresets.json` in the repository root. This file is gitignored because it contains machine-local DxLib paths.

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

Replace `DXLIB_DIR` with your actual DxLib installation path. Use forward slashes (`/`) or escaped backslashes (`\\`).

If you use Microsoft Visual Studio Code with the official **CMake Tools** and **C/C++** extensions, selecting a Visual Studio 2022 amd64 CMake kit is usually enough; you do not need to manually call `vcvars64.bat`.

For direct command-line builds outside Visual Studio Code or Visual Studio, use **Developer PowerShell for VS 2022**. If you must initialize MSVC from an ordinary PowerShell, run:
```powershell
& "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
```

For direct command-line builds, verify the compiler is available:

```powershell
cl
```

## Build Commands

Configure from the repository root:

```powershell
cmake --preset windows-default
```

This reads `CMakeUserPresets.json`, uses Ninja, writes build files to `cmake-build-debug/`, configures `DXLIB_DIR`, enables `/utf-8`, defines the Windows Unicode build macros, fetches test dependencies, and scans C++20 module dependencies.

Expected configure output includes:

```text
-- Configuring done
-- Generating done
-- Build files have been written to: <repo-root>/cmake-build-debug
```

Build all targets:

```powershell
cmake --build cmake-build-debug
```

Build a specific target:

```powershell
cmake --build cmake-build-debug --target MagicCat
cmake --build cmake-build-debug --target MagicCatTests
cmake --build cmake-build-debug --target MagicCatCore
```

Build targets:

- `MagicCatCore`: static library containing game logic and C++20 modules.
- `MagicCat`: WIN32 game executable.
- `MagicCatTests`: test executable using GoogleTest, GMock, RapidCheck, and RapidCheck GTest integration.

The first build scans all `.ixx` files and can take several minutes. Later builds reuse cached module dependency information. Module BMI files (`.ifc`) are generated under `cmake-build-debug/MagicCat/CMakeFiles/MagicCatCore.dir/`.

## Test Execution

Build and run tests directly from the repository root:

```powershell
cmake --build cmake-build-debug --target MagicCatTests
.\cmake-build-debug\tests\MagicCatTests.exe
```

Useful GoogleTest options:

```powershell
.\cmake-build-debug\tests\MagicCatTests.exe --gtest_filter="BattleService*"
.\cmake-build-debug\tests\MagicCatTests.exe --gtest_list_tests
.\cmake-build-debug\tests\MagicCatTests.exe --gtest_verbose
```

Run through CTest:

```powershell
ctest --test-dir cmake-build-debug --output-on-failure
ctest --test-dir cmake-build-debug -j 8 --output-on-failure
ctest --test-dir cmake-build-debug -R "BattleService" --verbose
```

CTest discovers GoogleTest test cases during CMake configure. RapidCheck property-based tests may take longer because they generate test inputs.

## Runtime Requirements

The game and some integration tests must run from the repository root so resource paths resolve.

```text
MagicCat/                          <- Run from here
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

Correct command-line execution from repo root:

```powershell
.\cmake-build-debug\MagicCat\MagicCat.exe
```

Running from `cmake-build-debug/MagicCat/` will fail to load resources because paths are rooted at `MagicCat/resource/...`.

IDE working directory setup:

- **Visual Studio / Rider:** set the working directory to `${ProjectDir}` or the repository root.
- **CLion:** set the run configuration working directory to the repository root.
- **VS Code:** set `cwd` in `launch.json` to `${workspaceFolder}`.

## Build Reproducibility Checklist

### Prerequisites

- [ ] Visual Studio 2022 installed with Desktop development with C++, C++20 modules support, and Windows SDK 10.0.19041.0 or later.
- [ ] CMake 3.28 or later installed and available in PATH. Verify with `cmake --version`.
- [ ] Ninja installed and available in PATH. Verify with `ninja --version`.
- [ ] DxLib Visual C++ version downloaded and extracted.
- [ ] Path to the directory containing `DxLib.h` and `DxLib_x64.lib` is known.

### Initial Setup

- [ ] Repository cloned.
- [ ] Current terminal is in the repository root.
- [ ] `CMakeUserPresets.json` created in the repository root.
- [ ] `DXLIB_DIR` points to the actual DxLib path.
- [ ] Preset name is `windows-default` and generator is `Ninja`.
- [ ] For direct command-line builds, Developer PowerShell is open or MSVC is initialized with `vcvars64.bat`.
- [ ] For Visual Studio Code builds, a Visual Studio 2022 amd64 CMake kit is selected.
- [ ] For direct command-line builds, `cl` prints the MSVC compiler version.

### Build

- [ ] `cmake --preset windows-default` completes without `DXLIB_DIR`, Ninja, or compiler errors.
- [ ] Configure output shows `-- Configuring done` and `-- Generating done`.
- [ ] `cmake-build-debug/` is created.
- [ ] `cmake --build cmake-build-debug` completes without compilation errors.
- [ ] First build completes C++20 module scanning.
- [ ] `cmake-build-debug\MagicCat\MagicCat.exe` exists.
- [ ] `cmake-build-debug\tests\MagicCatTests.exe` exists.
- [ ] `cmake-build-debug\MagicCat\MagicCatCore.lib` exists.

### Tests

- [ ] Tests are run from the repository root.
- [ ] `.\cmake-build-debug\tests\MagicCatTests.exe` executes without resource path errors.
- [ ] `ctest --test-dir cmake-build-debug --output-on-failure` discovers and runs tests.
- [ ] Failures, if any, are documented.

### Runtime

- [ ] Current directory is the repository root.
- [ ] `MagicCat/resource/` exists.
- [ ] `MagicCat/resource/Json/card_config.json` exists.
- [ ] `MagicCat/resource/Json/enemy_config.json` exists.
- [ ] `MagicCat/resource/Json/game_config.json` exists.
- [ ] `.\cmake-build-debug\MagicCat\MagicCat.exe` starts without resource errors.
- [ ] Japanese text displays correctly when the required fonts are available.

### CI / Automation

- [ ] CI environment has MSVC Build Tools 2022 and Windows SDK.
- [ ] CI environment has Ninja.
- [ ] CI script initializes MSVC with `vcvars64.bat`.
- [ ] DxLib is available in CI through cache, download, or another approved mechanism.
- [ ] CMake receives `DXLIB_DIR` through a preset or `-DDXLIB_DIR=<path>`.
- [ ] Tests run with `--output-on-failure`.
- [ ] Artifacts such as `MagicCat.exe`, `MagicCatTests.exe`, and test results are uploaded when needed.

## Build Artifacts

Generated files are not committed:

```text
cmake-build-debug/          # Debug build artifacts
cmake-build-release/        # Release build artifacts
CMakeUserPresets.json       # Machine-local DxLib path
build_tests.bat             # Machine-local build script
build.bat                   # Machine-local build script
*.ifc                       # Module BMI files
*.obj                       # Object files
*.pdb                       # Debug symbols
*.ilk                       # Incremental link files
```

Source-controlled build and source locations:

```text
CMakeLists.txt              # Root build configuration
MagicCat/CMakeLists.txt     # Game target configuration
tests/CMakeLists.txt        # Test target configuration
common/                     # Third-party libraries
MagicCat/program/           # Game source and modules
MagicCat/resource/          # Game assets and configs
tests/test_*.cpp            # Test source files
tests/MockServices.h        # Test mocks
README.md                   # English setup and build documentation
README_jp.md                # Japanese setup and build documentation
```

## C++20 Modules

MagicCat uses C++20 modules for game code:

- Module interfaces are `MagicCat/program/**/*.ixx`.
- Module implementations are `MagicCat/program/**/*.cpp`.
- The `Displayer` module uses partitions such as `Displayer:Core`, `Displayer:Combat`, `Displayer:Cutscene`, `Displayer:Menu`, `Displayer:MenuIcon`, and `Displayer:Sprite`.

Ninja is required because CMake's MSVC module support depends on Ninja dependency scanning. The module build process scans `.ixx` files, builds a dependency graph, compiles modules in dependency order, emits `.ifc` files, and links objects into the final target.

`MagicCatCore` uses `UNITY_BUILD ON` to batch implementation files for faster compilation. It also uses `<dxe.h>` as a public precompiled header for DxLib-heavy translation units.

## Compiler and Linker Flags

`MagicCatCore` compile definitions:

- `NOMINMAX` prevents Windows `min` and `max` macros from conflicting with `std::min` and `std::max`.
- `WIN32` and `_WINDOWS` enable Windows platform paths.
- `UNICODE` and `_UNICODE` enable Unicode builds.

Common compile options:

- `/utf-8` for UTF-8 source and execution character sets.
- `-std:c++latest` for C++23 with C++20 modules.
- `/Ob0 /Od /RTC1` in Debug.
- `/O2 /Ob2` in Release.
- `/MDd` in Debug and `/MD` in Release.
- `/Zi` for debug symbols.
- `/bigobj` for `MagicCatTests`.

Executable linker options include `/SUBSYSTEM:WINDOWS`, `/DEBUG`, `/INCREMENTAL` in Debug, and `/INCREMENTAL:NO` in Release. If `lld-link` is found, CMake may add the configured faster-linker option.

## Troubleshooting

| Symptom | Likely Cause | Solution |
|---------|--------------|----------|
| `stdio.h not found` or standard library errors | MSVC environment is not initialized | Open Developer PowerShell for VS 2022 or run `vcvars64.bat` |
| `Generator not found: Ninja` | Ninja is not installed or not in PATH | Install Ninja, add it to PATH, restart the terminal, and verify `ninja --version` |
| `DXLIB_DIR is not set or does not exist` | Missing or incorrect `CMakeUserPresets.json` | Create the preset file and point `DXLIB_DIR` to the directory containing `DxLib.h` |
| `error C7612: could not find module 'ModuleName'` | Module build order issue or stale BMI cache | Clean `cmake-build-debug/`, reconfigure, and rebuild |
| Resource not found in tests or game | Wrong working directory | Run from the repository root |
| Module scanning takes a long time | First C++20 module build | Wait for the first build; later builds should be faster |

Clean rebuild:

```powershell
Remove-Item -Recurse -Force cmake-build-debug
cmake --preset windows-default
cmake --build cmake-build-debug
```

Force a module rescan without deleting the whole build directory:

```powershell
(Get-Item MagicCat\program\EventBus.ixx).LastWriteTime = Get-Date
cmake --build cmake-build-debug
```

## IDE Setup

### Visual Studio 2022

- Open the repository root with `File > Open > Folder...`.
- Select the `windows-default` CMake preset.
- Set the working directory for `MagicCat.exe` to the repository root.

### Rider / CLion

- Open the repository root.
- Select the `windows-default` CMake profile or preset.
- Set the run configuration working directory to the repository root.

### VS Code

Use Microsoft Visual Studio Code for MSVC/CMake development on this project.

Do not use third-party VS Code distributions such as Cursor or Kiro as the primary C++ IDE for this project. They cannot use Microsoft's official **C/C++** extension under the same licensing/distribution model as Visual Studio Code, which means MSVC compiler discovery, IntelliSense, and CMake kit integration may not work correctly.

- Install **CMake Tools** and Microsoft's official **C/C++** extension in Visual Studio Code.
- Open the repository root.
- Select a Visual Studio 2022 amd64 CMake kit.
- Run `CMake: Configure`.
- Set `"cwd": "${workspaceFolder}"` in `launch.json`.

If Visual Studio Code shows syntax errors at `module;`, IntelliSense is not using the CMake-provided C++23/module flags. Selecting a valid CMake kit and completing configure usually resolves it.

## Known Limitations

- Windows only because DxLib is Windows-only.
- MSVC v143 only; other compilers are not supported for this project.
- x64 builds are expected. 32-bit builds are not tested.
- Ninja is required for C++20 modules with this CMake/MSVC setup.
- Runtime resource paths assume the process starts from the repository root.
- First builds are slower because module dependency scanning has to run.
