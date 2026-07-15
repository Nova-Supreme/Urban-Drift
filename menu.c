#include "menu.h"
#include <raylib.h>

// ---- Item counts for each menu ----
#define MAIN_MENU_ITEM_COUNT     5
#define LEVEL_SELECT_ITEM_COUNT  4
#define OPTIONS_ITEM_COUNT       4
#define PAUSE_MENU_ITEM_COUNT    3

// ---- Per-menu selection state ----
static int mainMenuSelected    = 0;
static int levelSelectSelected = 0;
static int optionsSelected     = 0;
static int pauseSelected       = 0;

// =========================================================================
// Shared drawing helpers
// =========================================================================

static void DrawMenuTitle(const char* title, int windowwidth, int y)
{
    int fontsize = 50;
    int textwidth = MeasureText(title, fontsize);
    DrawText(title, (windowwidth - textwidth) / 2, y, fontsize, WHITE);
}

// Draws a vertical list of items, highlighting the selected one with
// a gold color and an arrow marker.
static void DrawMenuItems(const char** items, int count, int selected, int windowwidth, int startY, int spacing, int fontsize)
{
    for(int i = 0; i < count; i++){
        Color color = (i == selected) ? GOLD : WHITE;
        int textwidth = MeasureText(items[i], fontsize);
        int x = (windowwidth - textwidth) / 2;
        int y = startY + i * spacing;

        if(i == selected) DrawText(">", x - 30, y, fontsize, GOLD);
        DrawText(items[i], x, y, fontsize, color);
    }
}

// Shared up/down navigation with wraparound. Returns true if selection changed.
static void NavigateVertical(int* selected, int count)
{
    if(IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        *selected = (*selected + 1) % count;
    if(IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        *selected = (*selected - 1 + count) % count;
}

// =========================================================================
// Main Menu
// =========================================================================

void Menu_ResetMainMenu(void)
{
    mainMenuSelected = 0;
}

GameState UpdateMainMenu(void)
{
    NavigateVertical(&mainMenuSelected, MAIN_MENU_ITEM_COUNT);

    if(IsKeyPressed(KEY_ENTER)){
        switch(mainMenuSelected){
            case 0: return STATE_GAME;          // Play
            case 1: return STATE_LEVEL_SELECT;  // Level Select
            case 2: return STATE_OPTIONS;       // Options
            case 3: return STATE_CONTROLS;      // Controls
            case 4: return STATE_QUIT;          // Quit
        }
    }
    return STATE_MAIN_MENU;
}

void DrawMainMenu(int windowwidth, int windowheight)
{
    const char* items[MAIN_MENU_ITEM_COUNT] = {
        "Play", "Level Select", "Options", "Controls", "Quit"
    };

    DrawMenuTitle("URBAN DRIFT", windowwidth, windowheight / 4);
    DrawMenuItems(items, MAIN_MENU_ITEM_COUNT, mainMenuSelected, windowwidth, windowheight / 2 - 50, 50, 30);
}

// =========================================================================
// Level Select
// =========================================================================

void Menu_ResetLevelSelect(void)
{
    levelSelectSelected = 0;
}

GameState UpdateLevelSelect(void)
{
    if(IsKeyPressed(KEY_ESCAPE)) return STATE_MAIN_MENU;

    NavigateVertical(&levelSelectSelected, LEVEL_SELECT_ITEM_COUNT);

    if(IsKeyPressed(KEY_ENTER)){
        if(levelSelectSelected == LEVEL_SELECT_ITEM_COUNT - 1) // "Back" is always the last item
            return STATE_MAIN_MENU;

        // Levels are placeholders - all currently launch the same existing
        // gameplay. Swap this out once real level data/loading exists.
        return STATE_GAME;
    }
    return STATE_LEVEL_SELECT;
}

void DrawLevelSelect(int windowwidth, int windowheight)
{
    const char* items[LEVEL_SELECT_ITEM_COUNT] = {
        "Level 1", "Level 2", "Level 3", "Back"
    };

    DrawMenuTitle("LEVEL SELECT", windowwidth, windowheight / 4);
    DrawMenuItems(items, LEVEL_SELECT_ITEM_COUNT, levelSelectSelected, windowwidth, windowheight / 2 - 50, 50, 30);
}

// =========================================================================
// Options (placeholder only - no actual settings applied yet)
// =========================================================================

GameState UpdateOptions(void)
{
    if(IsKeyPressed(KEY_ESCAPE)) return STATE_MAIN_MENU;

    NavigateVertical(&optionsSelected, OPTIONS_ITEM_COUNT);

    if(IsKeyPressed(KEY_ENTER) && optionsSelected == OPTIONS_ITEM_COUNT - 1) // "Back"
        return STATE_MAIN_MENU;

    return STATE_OPTIONS;
}

void DrawOptions(int windowwidth, int windowheight)
{
    const char* items[OPTIONS_ITEM_COUNT] = {
        "Music Volume", "SFX Volume", "Fullscreen", "Back"
    };

    DrawMenuTitle("OPTIONS", windowwidth, windowheight / 4);
    DrawMenuItems(items, OPTIONS_ITEM_COUNT, optionsSelected, windowwidth, windowheight / 2 - 50, 50, 30);
}

// =========================================================================
// Controls (static reference screen)
// =========================================================================

GameState UpdateControls(void)
{
    if(IsKeyPressed(KEY_ESCAPE)) return STATE_MAIN_MENU;
    return STATE_CONTROLS;
}

void DrawControls(int windowwidth, int windowheight)
{
    DrawMenuTitle("CONTROLS", windowwidth, windowheight / 4);

    const char* lines[] = {
        "W A S D          Move",
        "SPACE            Brake",
        "ESC              Pause / Back",
        "ENTER            Select Menu Option"
    };
    int linecount = 4;
    int fontsize = 24;
    int spacing = 36;
    int startY = windowheight / 2 - 40;

    for(int i = 0; i < linecount; i++){
        int textwidth = MeasureText(lines[i], fontsize);
        DrawText(lines[i], (windowwidth - textwidth) / 2, startY + i * spacing, fontsize, WHITE);
    }

    const char* backhint = "Press ESC to return to Main Menu";
    int hintwidth = MeasureText(backhint, 18);
    DrawText(backhint, (windowwidth - hintwidth) / 2, startY + linecount * spacing + 30, 18, GRAY);
}

// =========================================================================
// Pause Menu
// =========================================================================

void Menu_ResetPauseMenu(void)
{
    pauseSelected = 0;
}

GameState UpdatePauseMenu(void)
{
    NavigateVertical(&pauseSelected, PAUSE_MENU_ITEM_COUNT);

    if(IsKeyPressed(KEY_ESCAPE)) return STATE_GAME; // ESC also resumes, matching old toggle behavior

    if(IsKeyPressed(KEY_ENTER)){
        switch(pauseSelected){
            case 0: return STATE_GAME;       // Resume
            case 1: return STATE_MAIN_MENU;  // Main Menu
            case 2: return STATE_QUIT;       // Quit
        }
    }
    return STATE_PAUSE;
}

void DrawPauseMenu(int windowwidth, int windowheight)
{
    // Dim the frozen gameplay frame behind the menu.
    DrawRectangle(0, 0, windowwidth, windowheight, (Color){0, 0, 0, 150});

    DrawMenuTitle("PAUSED", windowwidth, windowheight / 2 - 150);

    const char* items[PAUSE_MENU_ITEM_COUNT] = {
        "Resume", "Main Menu", "Quit"
    };
    DrawMenuItems(items, PAUSE_MENU_ITEM_COUNT, pauseSelected, windowwidth, windowheight / 2 - 40, 50, 30);
}
