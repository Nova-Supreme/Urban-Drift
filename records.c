#include "records.h"
#include <stdio.h>
#include <string.h>

// The records live in their own file, separate from the game's progress save,
// so they survive a NEW GAME reset. Each line is one record: "NAME seconds".
#define RECORDS_FILE "records.dat"

int Records_Load(RecordEntry* entries, int max)
{
    FILE* file = fopen(RECORDS_FILE, "r");
    if(file == NULL) return 0; // no records yet

    int count = 0;
    while(count < max){
        if(fscanf(file, "%15s %d", entries[count].name, &entries[count].seconds) != 2)
            break;
        count++;
    }
    fclose(file);
    return count;
}

void Records_Add(RecordEntry* entries, int* count, int max, const char* name, int seconds)
{
    // Top-5 board: a brand-new time only earns a spot if it beats the slowest
    // record currently on the list.
    if(*count >= max && seconds >= entries[max - 1].seconds) return;

    // Find where the new time belongs: smallest seconds = fastest = first.
    int insert = 0;
    while(insert < *count && entries[insert].seconds <= seconds) insert++;

    // Shift every worse record right by one to make room (the slowest one,
    // at the end, falls off the board when the list is already full).
    int last = (*count < max) ? *count : max - 1;
    for(int i = last; i > insert; i--) entries[i] = entries[i - 1];
    if(*count < max) (*count)++;

    strncpy(entries[insert].name, name, RECORD_NAME_MAX);
    entries[insert].name[RECORD_NAME_MAX] = '\0';
    entries[insert].seconds = seconds;

    // Write the whole list back to disk immediately so a crash can't lose it.
    FILE* file = fopen(RECORDS_FILE, "w");
    if(file == NULL) return; // cannot save, but the in-memory list still works
    for(int i = 0; i < *count; i++)
        fprintf(file, "%s %d\n", entries[i].name, entries[i].seconds);
    fclose(file);
}