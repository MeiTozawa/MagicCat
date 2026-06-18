call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars64.bat"
cmake --build cmake-build-debug --target MagicCatTests
if %errorlevel% neq 0 exit /b %errorlevel%
.\cmake-build-debug\tests\MagicCatTests.exe
