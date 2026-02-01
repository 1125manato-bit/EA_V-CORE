@echo off
setlocal

echo Searching for CMake...
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo Error: CMake not found. Please install CMake and add it to PATH.
    exit /b 1
)

echo Found CMake.

set "BUILD_DIR=build_windows"

echo Configuring...
cmake -B "%BUILD_DIR%" -S . -DCMAKE_BUILD_TYPE=Release
if %ERRORLEVEL% NEQ 0 (
    echo Configuration failed.
    exit /b 1
)

echo Building...
cmake --build "%BUILD_DIR%" --config Release --parallel 4
if %ERRORLEVEL% NEQ 0 (
    echo Build failed.
    exit /b 1
)

echo Build Complete.
echo Artifacts should be in %BUILD_DIR%\EA_V-CORE_artefacts\Release\VST3\
