/*
 * File Path: fs/ramfs.h
 * Description: Minimalist Virtual RAM File System Layer.
 */

#ifndef RAMFS_H
#define RAMFS_H

#define MAX_FILENAME 32
#define MAX_FILES 4

typedef struct {
    char name[MAX_FILENAME];
    char* content;
    int size;
} RAM_File;

static RAM_File virtual_disk[MAX_FILES];
static int file_count = 0;

/**
 * Injects a virtual file straight into system RAM memory.
 */
void create_file(char* name, char* content, int size) {
    if (file_count < MAX_FILES) {
        int i = 0;
        while (name[i] != '\0' && i < MAX_FILENAME - 1) {
            virtual_disk[file_count].name[i] = name[i];
            i++;
        }
        virtual_disk[file_count].name[i] = '\0';
        virtual_disk[file_count].content = content;
        virtual_disk[file_count].size = size;
        file_count++;
    }
}

/**
 * Searches the RAM disk for a file name and returns its text payload.
 */
char* read_file(char* name) {
    for (int i = 0; i < file_count; i++) {
        int j = 0;
        while (virtual_disk[i].name[j] == name[j]) {
            if (virtual_disk[i].name[j] == '\0') {
                return virtual_disk[i].content;
            }
            j++;
        }
    }
    return 0;
}

#endif
