#include "title.h"

// A small helper that draws one centred line of text.
static void DrawCenteredText(const char* text, int y, int size, Color color, int width)
{
    int textWidth = MeasureText(text, size);
    DrawText(text, (width - textWidth) / 2, y, size, color);
}

const char* Title_VehicleName(int vehicleId)
{
    switch(vehicleId){
        case 1: return "RICKSHAW";
        case 2: return "CNG";
        case 3: return "AUTO";
        case 4: return "CAR";
        default: return "UNKNOWN";
    }
}

void Title_Init(TitleScreen* title, bool hasContinue)
{
    title->index = 0;
    title->hasContinue = hasContinue;
}

void Title_Refresh(TitleScreen* title, bool hasContinue)
{
    // Called while the title screen is shown so the CONTINUE entry matches the
    // save file after a wipe or a lost run. Do NOT touch `index` here - this
    // runs every frame, so resetting it would swallow the player's input.
    title->hasContinue = hasContinue;
}

int Title_HandleMenu(TitleScreen* title)
{
    // How many items the menu has depends on whether there is a run to
    // continue. Without one, CONTINUE is hidden and everything shifts up.
    int count = TITLE_CHOICE_COUNT - (title->hasContinue ? 0 : 1);
    if(title->index >= count) title->index = count - 1; // the menu just shrank

    // Move the highlight up and down. Loop round at the edges.
    if(IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        title->index = (title->index - 1 + count) % count;
    if(IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        title->index = (title->index + 1) % count;

    // ENTER confirms the highlighted choice.
    if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE)){
        // The menu index is offset by one when CONTINUE is on screen, so the
        // first visible item is CONTINUE with a run, otherwise NEW GAME.
        if(title->hasContinue) return title->index;
        return title->index + 1; // index 0 becomes NEW GAME, 1 QUIT
    }

    return -1; // nothing confirmed yet
}

void Title_DrawMenu(TitleScreen* title, int width, int height)
{
    // The labels are drawn in the same order as the menu index. CONTINUE is
    // only listed when a saved run exists.
    const char* labels[TITLE_CHOICE_COUNT] = { "CONTINUE", "NEW GAME", "QUIT" };
    int count = TITLE_CHOICE_COUNT - (title->hasContinue ? 0 : 1);
    int first = title->hasContinue ? 0 : 1; // index 0 is displayed only if CONTINUE exists

    DrawCenteredText("URBAN DRIFT", height/2 - 200, 60, WHITE, width);

    // Draw each option, with the highlighted one bigger and in a bright colour.
    for(int d = 0; d < count; d++){
        int i = first + d;
        int y = height/2 - 80 + d*55;
        bool selected = (d == title->index);

        Color color = selected ? YELLOW : GRAY;

        // ">" marker beside the highlighted option.
        if(selected) DrawText(">", width/2 - 120, y, 30, color);

        DrawCenteredText(labels[i], y, selected ? 35 : 30, color, width);
    }

    DrawCenteredText("Use W/S or arrows to move, ENTER to select", height - 40, 16, DARKGRAY, width);
}