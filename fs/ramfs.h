/*
 * fs/ramfs.h - Minimalist RAM-based File System for TzeOS Setup
 * Pure in-memory file structures. Zero storage bloat.
 */

#ifndef RAMFS_H
#define RAMFS_H

#define MAX_FILENAME 32
#define MAX_FILES 4

/* Structure for a single file entry */
typedef struct {
    char name[MAX_FILENAME];
    char* content;
    int size;
} RAM_File;

/* Global virtual disk storage layout */
static RAM_File virtual_disk[MAX_FILES];
static int file_count = 0;

/* Helper function to inject files into the setup environment */
void create_file(char* name, char* content, int size) {
    if (file_count < MAX_FILES) {
        int i = 0;
        // Copy filename characters safely
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

/* Search for a file by its name (Returns pointer to content, or 0 if not found) */
char* read_file(char* name) {
    for (int i = 0; i < file_count; i++) {
        int j = 0;
        // Simple string comparison logic
        while (virtual_disk[i].name[j] == name[j]) {
            if (virtual_disk[i].name[j] == '\0') {
                return virtual_disk[i].content; // File found!
            }
            j++;
        }
    }
    return 0; // File not found, bozo
}

#endif