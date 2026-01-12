#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

// Definice barev pro profesionálnější vzhled
#define CLR_RESET  "\033[0m"
#define CLR_HEADER "\033[1;36m"
#define CLR_OK     "\033[1;32m"
#define CLR_ERR    "\033[1;31m"
#define CLR_INFO   "\033[1;33m"

typedef struct {
    char shortName[101];
    int credits;
    char grade;
} Entry;

typedef struct {
    Entry * entries;
    int entryCnt;
    int entryCap;
} Data;

void saveToFile(Data * data) {
    FILE * file = fopen("data.txt", "w");
    if (!file) return;
    for (int i = 0; i < data->entryCnt; i++) {
        Entry * curr = &data->entries[i];
        fprintf(file, "%s %c %d\n", curr->shortName, curr->grade, curr->credits);
    }
    fclose(file);
}

bool parseCommand(char ** cursor, char * command) {
    while (**cursor == ' ') {
        (*cursor)++;
    }
    if (**cursor != '?' && **cursor != '#' && **cursor != '+' && **cursor != '-' && **cursor != '=') return false;
    *command = **cursor;
    (*cursor)++;
    return true;
}

void readAllGrades(Data * data) {
    if (data->entryCnt == 0) {
        printf(CLR_INFO "[-] Database is currently empty.\n" CLR_RESET);
        return;
    }

    printf("\n" CLR_HEADER "+----------------------+--------+---------+\n");
    printf("| %-20s | %-6s | %-7s |\n", "Course", "Grade", "Credits");
    printf("+----------------------+--------+---------+\n" CLR_RESET);

    for (int i = 0; i < data->entryCnt; i++) {
        Entry * curr = &data->entries[i];
        // Formátování: %-20s (20 zn) + mezery okolo = 22 znaku pro prvni sloupec
        printf("| %-20s |   %c    |   %2d    |\n",
            curr->shortName, curr->grade, curr->credits
        );
    }
    printf(CLR_HEADER "+----------------------+--------+---------+\n\n" CLR_RESET);
}

int compByShortName(const void * a, const void * b) {
    Entry * A = (Entry *) a;
    Entry * B = (Entry *) b;
    return strcmp(A->shortName, B->shortName);
}

void addEntry(Data * data, char * cursor) {
    char shortName[101];
    int shortNameLen = 0;
    int credits = 0;
    char grade;

    while (*cursor == ' ') cursor++;
    while (*cursor != ' ') {
        if (*cursor == '\n' || *cursor == '\0') {
            printf(CLR_ERR "[!] Error: Incomplete format (grade/credits missing).\n" CLR_RESET);
            return;
        }
        shortName[shortNameLen++] = *cursor;
        cursor++;
    }
    shortName[shortNameLen] = '\0';

    while (*cursor == ' ') cursor++;
    if (*cursor != 'A' && *cursor != 'B' && *cursor != 'C' && *cursor != 'D' && *cursor != 'E' && *cursor != 'F') {
        printf(CLR_ERR "[!] Error: Invalid Grade '%c'. Valid grades are A-F.\n" CLR_RESET, *cursor);
        return;
    }
    grade = *cursor;
    cursor++;

    while (*cursor == ' ') cursor++;
    while (*cursor != '\0' && *cursor != '\n' && *cursor != ' ') {
        if (!isdigit(*cursor)) {
            printf(CLR_ERR "[!] Error: Number of credits must be a number.\n" CLR_RESET);
            return;
        }
        credits = credits * 10 + (*cursor - '0');
        cursor++;
    }

    if (credits <= 0 || credits >= 100 || shortNameLen == 0) {
        printf(CLR_ERR "[!] Error: Invalid input data (credits 1-99).\n" CLR_RESET);
        return;
    }

    qsort(data->entries, data->entryCnt, sizeof(Entry), compByShortName);
    Entry key;
    strcpy(key.shortName, shortName);
    void * existing = bsearch(&key, data->entries, data->entryCnt, sizeof(Entry), compByShortName);

    if (existing) {
        printf(CLR_ERR "[!] Error: Entry: [%s] already exists in the db.\n" CLR_RESET, shortName);
        return;
    }

    if (data->entryCnt + 1 >= data->entryCap) {
        data->entryCap *= 2;
        data->entries = (Entry *) realloc(data->entries, data->entryCap * sizeof(Entry));
    }

    data->entries[data->entryCnt].credits = credits;
    data->entries[data->entryCnt].grade = grade;
    strcpy(data->entries[data->entryCnt].shortName, shortName);
    data->entryCnt++;

    saveToFile(data);
    printf(CLR_OK "[+] Entry [%s] was successfully added.\n" CLR_RESET, shortName);
}

void removeEntry(Data * data, char * cursor) {
    char shortName[101];
    int shortNameLen = 0;

    while (*cursor == ' ') cursor++;
    while (*cursor != ' ' && *cursor != '\0' && *cursor != '\n') {
        shortName[shortNameLen++] = *cursor;
        cursor++;
    }
    shortName[shortNameLen] = '\0';

    if (shortNameLen == 0) {
        printf(CLR_ERR "[!] Error: Please specify the course name shortcut to delete it.\n" CLR_RESET);
        return;
    }

    qsort(data->entries, data->entryCnt, sizeof(Entry), compByShortName);
    Entry key;
    strcpy(key.shortName, shortName);
    Entry * existing = (Entry *) bsearch(&key, data->entries, data->entryCnt, sizeof(Entry), compByShortName);

    if (!existing) {
        printf(CLR_ERR "[!] Error: Couldn't find [%s] in the database.\n" CLR_RESET, shortName);
        return;
    }

    int index = existing - data->entries;
    int toMove = data->entryCnt - index - 1;
    if (toMove > 0) {
        memmove(existing, existing + 1, toMove * sizeof(Entry) );
    }
    data->entryCnt--;

    saveToFile(data);
    printf(CLR_OK "[-] Entry [%s] was removed from the database.\n" CLR_RESET, shortName);
}

void calculateGPA(Data * data) {
    if (data->entryCnt == 0) {
        printf(CLR_INFO "[-] Can't calculate GPA (no data).\n" CLR_RESET);
        return;
    }

    float totalPoints = 0;
    int totalCredits = 0;

    for (int i = 0; i < data->entryCnt; i++) {
        Entry * curr = &data->entries[i];
        totalCredits += curr->credits;
        if (curr->grade == 'A') totalPoints += (4.0 * curr->credits);
        else if (curr->grade == 'B') totalPoints += (3.0 * curr->credits);
        else if (curr->grade == 'C') totalPoints += (2.0 * curr->credits);
        else if (curr->grade == 'D') totalPoints += (1.0 * curr->credits);
        else if (curr->grade == 'E') totalPoints += (0.5 * curr->credits);
    }

    float gpa = totalPoints / (float)totalCredits;
    printf("\n" CLR_HEADER "╔════════════════════════════════════╗\n");
    printf(                "║        Current student's GPA       ║\n");
    printf(                "╠════════════════════════════════════╣\n");
    printf(                "║             >  %.2f  <             ║\n", gpa);
    printf(                "╚════════════════════════════════════╝\n\n" CLR_RESET);
}

int main () {
    Data data;
    data.entryCnt = 0;
    data.entryCap = 2;
    data.entries = (Entry *) malloc(data.entryCap * sizeof(Entry));

    FILE * file = fopen("data.txt", "r");
    if (file) {
        char shortName[101];
        int credits;
        char grade;
        while (fscanf(file, "%100s %c %d", shortName, &grade, &credits ) != EOF) {
            if (data.entryCnt + 1 >= data.entryCap) {
                data.entryCap *= 2;
                data.entries = (Entry *) realloc(data.entries, data.entryCap * sizeof(Entry));
            }
            data.entries[data.entryCnt].credits = credits;
            data.entries[data.entryCnt].grade = grade;
            strcpy(data.entries[data.entryCnt].shortName, shortName);
            data.entryCnt++;
        }
        fclose(file);
    }

    printf(CLR_HEADER "=== Simple GPA calculator ===\n" CLR_RESET);
    printf("Commands: " CLR_INFO "+ [name shortcut] [grade] [credits]" CLR_RESET " | " CLR_INFO "- [name shortcut]" CLR_RESET "\n");
    printf("Functions:  " CLR_INFO "?" CLR_RESET " (list of all entries) | " CLR_INFO "=" CLR_RESET " (calculate GPA) | " CLR_INFO "#" CLR_RESET " (# of entries)\n");
    printf("--------------------------------------------\n");

    char * line = NULL;
    size_t size = 0;

    while (printf(">> "), getline(&line, &size, stdin) != EOF) {
        char * cursor = line;
        char command;
        if (!parseCommand(&cursor, &command)) {
            if (*cursor != '\n' && *cursor != '\0')
                printf(CLR_ERR "[!] Unkonwn command, please use: +, -, ?, = or #.\n" CLR_RESET);
            continue;
        }
        switch (command) {
            case '?': readAllGrades(&data); break;
            case '#': printf(CLR_INFO "[i] Number of entries: %d\n" CLR_RESET, data.entryCnt); break;
            case '+': addEntry(&data, cursor); break;
            case '-': removeEntry(&data, cursor); break;
            case '=': calculateGPA(&data); break;
        }
    }

    free(line);
    free(data.entries);
    return 0;
}