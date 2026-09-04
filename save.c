#include "save.h"
#include <stdio.h>

// The name of the file our progress is stored in (created in the game folder).
#define SAVE_FILE "save.dat"

// A brand-new player owns just the starter car (the rickshaw, id 1) and has
// not played a round yet, so there is nothing active to continue.
static const SaveData DEFAULT_SAVE = { 0, 1, 1, 0 };

bool Save_Exists(void)
{
    FILE* file = fopen(SAVE_FILE, "r");
    if(file == NULL) return false;
    fclose(file);
    return true;
}

bool Save_Load(SaveData* data)
{
    FILE* file = fopen(SAVE_FILE, "r");
    if(file == NULL){
        *data = DEFAULT_SAVE; // no file yet, use the starting position
        return false;
    }

    // The file is four numbers, one per line: money, vehicle id, owned mask,
    // and whether the round can still be continued (active).
    int money = 0;
    int vehicle = 0;
    int mask = 0;
    int active = 0;
    int items = fscanf(file, "%d\n%d\n%d\n%d", &money, &vehicle, &mask, &active);
    fclose(file);

    // Old save files only had the first three numbers; treat those as a run
    // that is still active (so an update doesn't break an existing continue).
    if(items == 3) active = 1;

    if(items < 3) { // the file was incomplete or unreadable
        *data = DEFAULT_SAVE;
        return false;
    }

    // Guard against nonsense values so the game never starts in a broken state.
    if(money < 0)  money = 0;
    if(vehicle < 1 || vehicle > 4) vehicle = 1;
    if(mask < 1)   mask = 1; // the starter car is always owned
    if(active != 0 && active != 1) active = 1;

    data->money = money;
    data->savedVehicleId = vehicle;
    data->ownedMask = mask;
    data->active = active;
    return true;
}

void Save_Write(SaveData data, int active)
{
    FILE* file = fopen(SAVE_FILE, "w"); // "w" creates/overwrites the file
    if(file == NULL) return;            // if it cannot be opened, do nothing

    fprintf(file, "%d\n%d\n%d\n%d\n", data.money, data.savedVehicleId, data.ownedMask, active);
    fclose(file);
}

void Save_Delete(void)
{
    // remove() returns 0 if the file was deleted (or never existed, which is
    // also fine - there is nothing left to wipe).
    remove(SAVE_FILE);
}
