#ifndef RECORDS_H
#define RECORDS_H

// A single high-score entry: the name the player picked when they unlocked
// the CAR, and how long the whole run took in seconds.
#define RECORD_NAME_MAX 15
#define MAX_RECORDS 5

typedef struct {
    char name[RECORD_NAME_MAX + 1];
    int seconds;
} RecordEntry;

// Reads records.dat into `entries`; returns how many records were loaded
// (0..max). Returns 0 if there is no records file yet.
int Records_Load(RecordEntry* entries, int max);

// Adds a brand-new record, keeping the list sorted fastest-first (smallest
// seconds at the front). If the list is full the new time must be quicker
// than the current 5th-best record, otherwise it is ignored. The whole list
// is written back to records.dat immediately.
// NOTE: this file is separate from the save file, so starting a NEW GAME
// (which resets save.dat) deliberately leaves the high scores alone.
void Records_Add(RecordEntry* entries, int* count, int max, const char* name, int seconds);

#endif