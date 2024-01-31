#ifndef MYSYNC_H
#define MYSYNC_H
#define MAX_IGNORE_PATTERNS 100
#define PATH_MAX 1000
#include <time.h>  // Include for time_t
// #include <sys/stat.h>  // Include for mode_t
#include <sys/types.h>  // Include for mode_t

// Data structure to store file information
struct FileInfo {
    char filename[256];  // Change the size as needed
    time_t modification_time;
    mode_t permissions;
};

// Function to list files in a directory and store information
void listFiles(const char* directory, struct FileInfo** files, int* fileCount, int all_files);
void listFilesAndDirectories(const char* directory, struct FileInfo** entries, int* entryCount, int all_entries);

// Function to determine which files need synchronization
void identifyFilesForSync(const struct FileInfo* filesDir1, int countDir1, const struct FileInfo* filesDir2, int countDir2);

// Function to perform the synchronization
void synchronizeFiles(const char* sourceDir, const char* destDir, const struct FileInfo* files, int fileCount, int preservePermissions) ;
// void synchronizeFiles(const char* sourceDir, const char* destDir, const struct FileInfo* files, int fileCount);
char *glob2regex(char *glob);

#endif

