call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
cmake --build cmake-build-debug --target MagicCatTests
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
.\cmake-build-debug\tests\MagicCatTests.exe
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%
