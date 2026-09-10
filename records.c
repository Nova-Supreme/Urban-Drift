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
    // If the tracker is full, drop the OLDEST record so the new one fits.
    int insert = *count;
    if(insert >= max){
        for(int i = 1; i < max; i++) entries[i - 1] = entries[i];
        insert = max - 1;
    }

    strncpy(entries[insert].name, name, RECORD_NAME_MAX);
    entries[insert].name[RECORD_NAME_MAX] = '\0';
    entries[insert].seconds = seconds;
    if(insert == *count) (*count)++;
    else *count = max;
    // (If we dropped an old record, the count stays full.)

    // Write the whole list back to disk immediately so a crash can't lose it.
    FILE* file = fopen(RECORDS_FILE, "w");
    if(file == NULL) return; // cannot save, but the in-memory list still works
    for(int i = 0; i < *count; i++)
        fprintf(file, "%s %d\n", entries[i].name, entries[i].seconds);
    fclose(file);
}