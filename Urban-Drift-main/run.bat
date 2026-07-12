@echo off
cd /d "%~dp0"
"C:\raylib\w64devkit\bin\gcc.exe" main.c cars.c -o urban_drift.exe -I C:/raylib/raylib/src -L C:/raylib/raylib/src -lraylib -lopengl32 -lgdi32 -lwinmm
if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b
)
urban_drift.exe