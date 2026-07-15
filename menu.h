#ifndef MENU_H
#define MENU_H

#include "gamestate.h"

// --- Main Menu ---
void Menu_ResetMainMenu(void);              // call when (re)entering this screen
GameState UpdateMainMenu(void);              // handles input, returns next GameState
void DrawMainMenu(int windowwidth, int windowheight);

// --- Level Select ---
void Menu_ResetLevelSelect(void);
GameState UpdateLevelSelect(void);
void DrawLevelSelect(int windowwidth, int windowheight);

// --- Options (placeholder settings only) ---
GameState UpdateOptions(void);
void DrawOptions(int windowwidth, int windowheight);

// --- Controls (static reference screen) ---
GameState UpdateControls(void);
void DrawControls(int windowwidth, int windowheight);

// --- Pause Menu ---
void Menu_ResetPauseMenu(void);
GameState UpdatePauseMenu(void);
void DrawPauseMenu(int windowwidth, int windowheight);

#endif
