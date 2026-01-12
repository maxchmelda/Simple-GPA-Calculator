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
    bool hasGrade;
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

        char gradeToSave = curr->hasGrade ? curr->grade : '_';
        fprintf(file, "%s %d %c\n", curr->shortName, curr->credits, gradeToSave);
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

    printf("\n" CLR_HEADER "╔═══════════════════════╦═════════╦════════╗\n");
    printf(                "║ %-21s ║ %-7s ║ %-6s ║\n", "Course", "Credits", "Grade");
    printf(                "╠═══════════════════════╬═════════╬════════╣\n" CLR_RESET);

    for (int i = 0; i < data->entryCnt; i++) {
        Entry * curr = &data->entries[i];
        char gradeDisplay = curr->hasGrade ? curr->grade : '-';

        printf(CLR_HEADER "║" CLR_RESET " %-21s " CLR_HEADER "║" CLR_RESET "   %2d    " CLR_HEADER "║" CLR_RESET "   %c    " CLR_HEADER "║\n" CLR_RESET,
            curr->shortName, curr->credits, gradeDisplay
        );
    }

    printf(CLR_HEADER      "╚═══════════════════════╩═════════╩════════╝\n\n" CLR_RESET);
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
    bool hasGrade = false;

    // parse short name
    while (*cursor == ' ') cursor++;
    while (*cursor != ' ') {
        if (*cursor == '\n' || *cursor == '\0') {
            printf(CLR_ERR "[!] Error: Incomplete format (grade/credits missing).\n" CLR_RESET);
            return;
        }

        if (shortNameLen >= 100) {
            printf(CLR_ERR "[!] Error: name shortcut too long (must be 1-100 characters).\n" CLR_RESET);
            return;
        }

        shortName[shortNameLen++] = *cursor;
        cursor++;
    }
    if (shortNameLen == 0) {
        printf(CLR_ERR "[!] Error: name shortcut too long (must be 1-100 characters).\n" CLR_RESET);
        return;
    }
    shortName[shortNameLen] = '\0';

    // parse credits
    while (*cursor == ' ') cursor++;
    while (*cursor != '\0' && *cursor != '\n' && *cursor != ' ') {
        if (!isdigit(*cursor)) {
            printf(CLR_ERR "[!] Error: Number of credits must be a number.\n" CLR_RESET);
            return;
        }
        credits = credits * 10 + (*cursor - '0');
        cursor++;
    }
    if (credits <= 0 || credits >= 100) {
        printf(CLR_ERR "[!] Error: Invalid input data (credits 1-99).\n" CLR_RESET);
        return;
    }

    // parse grade
    while (*cursor == ' ') cursor++;
    if (*cursor != ' ' && *cursor != '\n' && *cursor != '\0') {
        if (*cursor != 'A' && *cursor != 'B' && *cursor != 'C' && *cursor != 'D' && *cursor != 'E' && *cursor != 'F') {
            printf("cursor: %c\n", *cursor);
            printf(CLR_ERR "[!] Error: Invalid Grade '%c'. Valid grades are A-F.\n" CLR_RESET, *cursor);
            return;
        }
        grade = *cursor;
        hasGrade = true;
        cursor++;
    }

    // check for trailing characters
    while (*cursor != '\n' && *cursor != '\0') {
        if (*cursor != ' ') {
            printf(CLR_ERR "[!] Error: Invalid input.\n" CLR_RESET);
            return;
        }
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
    strcpy(data->entries[data->entryCnt].shortName, shortName);
    if (hasGrade) {
        data->entries[data->entryCnt].grade = grade;
        data->entries[data->entryCnt].hasGrade = true;
    } else {
        data->entries[data->entryCnt].grade = ' ';
        data->entries[data->entryCnt].hasGrade = false;
    }
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
    int gradedCredits = 0;
    int totalCredits = 0;

    for (int i = 0; i < data->entryCnt; i++) {
        Entry * curr = &data->entries[i];
        totalCredits += curr->credits;

        if (curr->hasGrade) {
            gradedCredits += curr->credits;
            if (curr->grade == 'A')      totalPoints += (4.0f * curr->credits);
            else if (curr->grade == 'B') totalPoints += (3.0f * curr->credits);
            else if (curr->grade == 'C') totalPoints += (2.0f * curr->credits);
            else if (curr->grade == 'D') totalPoints += (1.0f * curr->credits);
            else if (curr->grade == 'E') totalPoints += (0.5f * curr->credits);
            else if (curr->grade == 'F') totalPoints += (0.0f * curr->credits);
        }
    }

    printf("\n" CLR_HEADER "╔════════════════════════════════════╗\n");
    printf(                "║        Student Statistics          ║\n");
    printf(                "╠════════════════════════════════════╣\n");
    if (gradedCredits > 0)
        printf(CLR_HEADER  "║" CLR_RESET "  GPA:           >  %.2f <          " CLR_HEADER "║\n" CLR_RESET, totalPoints / (float)gradedCredits);
    else
        printf(CLR_HEADER  "║" CLR_RESET "  GPA:           >  N/A     <       " CLR_HEADER "║\n" CLR_RESET);
    printf(CLR_HEADER      "║" CLR_RESET "  Total Credits: >  %3d <           " CLR_HEADER "║\n" CLR_RESET, totalCredits);
    printf(CLR_HEADER      "╚════════════════════════════════════╝\n\n" CLR_RESET);
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

        while (fscanf(file, "%100s %d %c", shortName, &credits, &grade) == 3) {
            if (data.entryCnt + 1 >= data.entryCap) {
                data.entryCap *= 2;
                data.entries = (Entry *) realloc(data.entries, data.entryCap * sizeof(Entry));
            }

            data.entries[data.entryCnt].credits = credits;
            strcpy(data.entries[data.entryCnt].shortName, shortName);

            if (grade == '_') {
                data.entries[data.entryCnt].grade = ' ';
                data.entries[data.entryCnt].hasGrade = false;
            } else {
                data.entries[data.entryCnt].grade = grade;
                data.entries[data.entryCnt].hasGrade = true;
            }
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