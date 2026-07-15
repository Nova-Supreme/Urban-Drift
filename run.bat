@echo off
cd /d "%~dp0"

:: 1. Wrap the paths in quotes just in case there are spaces, and use standard backslashes for Windows paths
"C:\raylib\w64devkit\bin\gcc.exe" main.c cars.c player.c passenger.c intro.c render.c menu.c -o urban_drift.exe -I"C:\raylib\raylib\src" -L"C:\raylib\raylib\src" -lraylib -lopengl32 -lgdi32 -lwinmm

if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b
)

echo Build succeeded! Launching game...
urban_drift.exe
