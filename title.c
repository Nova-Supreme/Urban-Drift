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

void Title_Init(TitleScreen* title)
{
    title->index = 0;
    title->vehicleIndex = 0;
}

int Title_HandleMenu(TitleScreen* title)
{
    // Move the highlight up and down. Loop round at the edges.
    if(IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
        title->index = (title->index - 1 + TITLE_CHOICE_COUNT) % TITLE_CHOICE_COUNT;
    if(IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
        title->index = (title->index + 1) % TITLE_CHOICE_COUNT;

    // ENTER confirms the highlighted choice.
    if(IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE))
        return title->index;

    return -1; // nothing confirmed yet
}

void Title_DrawMenu(TitleScreen* title, int width, int height)
{
    // Menu labels in the same order as the TitleChoice enum.
    const char* labels[TITLE_CHOICE_COUNT] = {
        "PLAY",
        "VEHICLES",
        "SETTINGS",
        "QUIT"
    };

    DrawCenteredText("URBAN DRIFT", height/2 - 200, 60, WHITE, width);

    // Draw each option, with the highlighted one bigger and in a bright colour.
    for(int i = 0; i < TITLE_CHOICE_COUNT; i++){
        int y = height/2 - 80 + i*55;
        bool selected = (i == title->index);

        Color color = selected ? YELLOW : GRAY;

        // ">" marker beside the highlighted option.
        if(selected) DrawText(">", width/2 - 120, y, 30, color);

        DrawCenteredText(labels[i], y, selected ? 35 : 30, color, width);
    }

    DrawCenteredText("Use W/S or arrows to move, ENTER to select", height - 40, 16, DARKGRAY, width);
}

int Title_HandleVehicles(TitleScreen* title)
{
    // Move between the four vehicles (ids 1..4).
    if(IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
        title->vehicleIndex = (title->vehicleIndex - 1 + 4) % 4;
    if(IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
        title->vehicleIndex = (title->vehicleIndex + 1) % 4;

    // ENTER means "picked this car" (caller decides buy vs drive), ESC backs out.
    if(IsKeyPressed(KEY_ENTER)) return VEHICLE_CHOOSE;
    if(IsKeyPressed(KEY_ESCAPE)) return VEHICLE_BACK;

    return VEHICLE_BROWSE; // still browsing
}

void Title_DrawVehicles(TitleScreen* title, int width, int height, Texture2D* carTextures, int currentVehicleId, int money, int ownedMask)
{
    DrawCenteredText("CHOOSE YOUR VEHICLE", 60, 45, WHITE, width);

    // The highlighted vehicle (index 0..3 maps to id 1..4).
    int vehicleId = title->vehicleIndex + 1;
    Texture2D tex = carTextures[title->vehicleIndex];

    // A SMALL preview of the car, centred near the middle of the screen.
    // Every car is scaled to the SAME preview height, so they all look the
    // same size even though the image files have different proportions.
    float targetHeight = 130.0f;
    float scale = targetHeight / (float)tex.height;
    float texw = tex.width*scale;
    float texh = tex.height*scale;
    Rectangle src = {0.0f, 0.0f, (float)tex.width, (float)tex.height};
    Rectangle dst = {(width-texw)/2.0f, height/2 - texh - 40.0f, texw, texh};
    DrawTexturePro(tex, src, dst, (Vector2){0.0f,0.0f}, 0.0f, WHITE);

    // Work out the car's name and status (locked, owned, or the current car).
    const char* name = Title_VehicleName(vehicleId);
    bool owned   = (ownedMask & (1 << (vehicleId - 1))) != 0;
    bool current = (vehicleId == currentVehicleId);

    Color statusColor = WHITE;
    const char* status;
    if(current){ status = "(CURRENT)";            statusColor = YELLOW; }
    else if(owned){ status = "OWNED - ENTER to drive"; statusColor = GREEN; }
    else { status = TextFormat("LOCKED - $%d", GetVehiclePreset(vehicleId).cost); statusColor = RED; }

    DrawCenteredText(name,   height/2 + 10, 30, statusColor, width);
    DrawCenteredText(status, height/2 + 50, 25, statusColor, width);

    // If it is still locked, say whether the player can afford it.
    if(!owned && !current){
        int cost = GetVehiclePreset(vehicleId).cost;
        if(money >= cost)
            DrawCenteredText("Press ENTER to buy", height/2 + 85, 18, GOLD, width);
        else
            DrawCenteredText("Not enough money", height/2 + 85, 18, DARKGRAY, width);
    }

    // Show the player's available money in the top-right corner.
    const char* cash = TextFormat("MONEY: $%d", money);
    int cashWidth = MeasureText(cash, 25);
    DrawText(cash, width - cashWidth - 20, 20, 25, GOLD);

    // Left/right arrows and the controls hint.
    DrawText("<", width/2 - 260, height/2 - 60, 40, GRAY);
    DrawText(">", width/2 + 230, height/2 - 60, 40, GRAY);
    DrawCenteredText("A/D or arrows to change, ENTER to buy or drive, ESC to go back",
                     height - 40, 16, DARKGRAY, width);
}
