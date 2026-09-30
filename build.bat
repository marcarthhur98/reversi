@echo off
setlocal
pushd "%~dp0"
if not exist build mkdir build
gcc -std=c11 -O2 -Wall -Wextra -Werror reversi.c search.c main.c -o build/reversi.exe
if errorlevel 1 goto fail
gcc -std=c11 -O2 -Wall -Wextra -Werror reversi.c search.c tests.c -o build/tests.exe
if errorlevel 1 goto fail
gcc -std=c11 -O2 -Wall -Wextra -Werror reversi.c search.c benchmark.c -o build/benchmark.exe
if errorlevel 1 goto fail
if /I "%~1"=="play" build\reversi.exe
if /I "%~1"=="test" build\tests.exe
if /I "%~1"=="benchmark" build\benchmark.exe
if errorlevel 1 goto fail
popd
exit /b 0
:fail
popd
exit /b 1
