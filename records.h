#ifndef RECORDS_H
#define RECORDS_H

// A single world-record entry: the name the player picked when they unlocked
// the CAR, and how long the whole run took in seconds.
#define RECORD_NAME_MAX 15
#define MAX_RECORDS 10

typedef struct {
    char name[RECORD_NAME_MAX + 1];
    int seconds;
} RecordEntry;

// Reads records.dat into `entries`; returns how many records were loaded
// (0..max). Returns 0 if there is no records file yet.
int Records_Load(RecordEntry* entries, int max);

// Adds a brand-new record, appending it to the tracker and writing the whole
// list back to records.dat immediately. When the tracker is full the oldest
// entry is dropped to make room.
// NOTE: this file is separate from the save file, so starting a NEW GAME
// (which resets save.dat) deliberately leaves the world records alone.
void Records_Add(RecordEntry* entries, int* count, int max, const char* name, int seconds);

#endif