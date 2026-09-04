#ifndef SAVE_H
#define SAVE_H

#include <stdbool.h>

// The player's progress, kept in a plain text file called "save.dat" in the
// game folder. It holds:
//   money          - how much money the player currently has saved
//   savedVehicleId - which car the last round was driving with (for CONTINUE)
//   ownedMask      - which cars have been bought (bit (id-1) is 1 = owned)
//   active         - 1 if the last round can be continued, 0 if it was lost
typedef struct {
    int money;
    int savedVehicleId;
    int ownedMask;
    int active;
} SaveData;

// Returns true if a saved file already exists on disk.
bool Save_Exists(void);

// Reads the save file into `data`. Returns false if there is no save file.
bool Save_Load(SaveData* data);

// Writes the given data to disk, overwriting any old save. `active` says
// whether the round can be CONTINUEd (1) or was lost (0).
void Save_Write(SaveData data, int active);

// Deletes the save file entirely (used by the "wipe all data" setting so the
// player can start completely fresh).
void Save_Delete(void);

#endif
