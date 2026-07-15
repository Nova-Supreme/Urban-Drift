#ifndef GAMESTATE_H
#define GAMESTATE_H

// All top-level screens/states the app can be in.
typedef enum {
    STATE_CUTSCENE,
    STATE_MAIN_MENU,
    STATE_LEVEL_SELECT,
    STATE_OPTIONS,
    STATE_CONTROLS,
    STATE_GAME,
    STATE_PAUSE,
    STATE_QUIT     // signals main.c to exit the main loop and close cleanly
} GameState;

#endif
