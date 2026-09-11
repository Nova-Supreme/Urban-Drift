#ifndef TITLE_H
#define TITLE_H

#include <raylib.h>
#include <stdbool.h>
#include "cars.h"

// The choices shown on the title screen. TITLE_CONTINUE only appears when a
// saved run is still in progress (the last game was quit mid-run, not lost).
typedef enum {
    TITLE_CONTINUE,  // carry on the last run from where it was quit
    TITLE_PLAY,      // start a completely fresh game (money included)
    TITLE_QUIT       // close the game
} TitleChoice;

#define TITLE_CHOICE_COUNT 3

// Returns the display name of a vehicle id (1..4).
const char* Title_VehicleName(int vehicleId);

// Keeps track of which list item is highlighted on the current menu.
typedef struct {
    int index;        // highlighted item on the title menu (0..TITLE_CHOICE_COUNT-1)
    bool hasContinue; // true when a saved run exists that can be continued
} TitleScreen;

// Fills in the starting state of the title screen. `hasContinue` says whether
// the "CONTINUE" option should be shown (true when a save file holds a run
// that was quit while still active).
void Title_Init(TitleScreen* title, bool hasContinue);

// Re-checks whether a run can be continued and resets the highlight to the
// first menu entry. Call it each time the title screen is shown.
void Title_Refresh(TitleScreen* title, bool hasContinue);

// Moves the highlight up/down on the main menu and returns TITLE_* when the
// player confirms with ENTER. Returns -1 until a choice is confirmed. The
// returned choice already skips CONTINUE when there is no save to continue.
int Title_HandleMenu(TitleScreen* title);

// Draws the main menu on screen.
void Title_DrawMenu(TitleScreen* title, int width, int height);

#endif