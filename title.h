#ifndef TITLE_H
#define TITLE_H

#include <raylib.h>
#include "cars.h"

// The choices shown on the title screen.
typedef enum {
    TITLE_PLAY,      // start a fresh round (after the intro story)
    TITLE_VEHICLES,  // open the vehicle selection menu
    TITLE_SETTINGS,  // open the settings menu
    TITLE_QUIT       // close the game
} TitleChoice;

#define TITLE_CHOICE_COUNT 4

// Returns the display name of a vehicle id (1..4).
const char* Title_VehicleName(int vehicleId);

// Keeps track of which list item is highlighted on the current menu.
typedef struct {
    int index;        // highlighted item on the title menu (0..TITLE_CHOICE_COUNT-1)
    int vehicleIndex; // highlighted vehicle on the vehicle menu (0..3)
} TitleScreen;

// Fills in the starting state of the title screen.
void Title_Init(TitleScreen* title);

// Moves the highlight up/down on the main menu and returns TITLE_* when the
// player confirms with ENTER. Returns -1 until a choice is confirmed.
int Title_HandleMenu(TitleScreen* title);

// Draws the main menu.
void Title_DrawMenu(TitleScreen* title, int width, int height);

// Handles the vehicle selection menu. The result tells the caller what the
// player did; `title->vehicleIndex` still holds which car is highlighted.
typedef enum {
    VEHICLE_BROWSE = -1, // the player is just moving between cars
    VEHICLE_BACK   =  0, // the player pressed ESC and wants to go back
    VEHICLE_CHOOSE =  1  // the player pressed ENTER on the highlighted car
} VehicleMenuAction;
int Title_HandleVehicles(TitleScreen* title);

// Draws the vehicle selection menu with a preview, price and lock state.
// `money` is the player's available cash and `ownedMask` says which cars are
// already bought (bit (id-1) set = owned).
void Title_DrawVehicles(TitleScreen* title, int width, int height, Texture2D* carTextures, int currentVehicleId, int money, int ownedMask);

#endif
