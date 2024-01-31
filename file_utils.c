#include "mysync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "mysync.h"
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <utime.h>
#include <sys/stat.h>  // Include for mode_t
#include <time.h>  // Include for time_t

// Function to list files in a directory and store information
void listFiles(const char* directory, struct FileInfo** files, int* fileCount, int all_files) {
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(directory)) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        // Ignore '.' and '..' entries
        if ((strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) && all_files == 0) {
            continue;
        }

        // Construct the full path of the file
        char fullpath[PATH_MAX];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", directory, entry->d_name);

        // Check if the entry is a regular file
        struct stat fileStat;
        if (stat(fullpath, &fileStat) == 0 && S_ISREG(fileStat.st_mode)) {
            // Store file information in the 'files' array
            struct FileInfo fileInfo;
            snprintf(fileInfo.filename, sizeof(fileInfo.filename), "%s", entry->d_name);
            fileInfo.modification_time = fileStat.st_mtime;
            fileInfo.permissions = fileStat.st_mode & 0777; // Store permissions as an octal number

            (*fileCount)++;
            *files = (struct FileInfo*)realloc(*files, *fileCount * sizeof(struct FileInfo));
            (*files)[*fileCount - 1] = fileInfo;
        }
    }

    closedir(dir);
}

void listFilesAndDirectories(const char* directory, struct FileInfo** entries, int* entryCount, int all_entries) {
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(directory)) == NULL) {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        // Ignore '.' and '..' entries
        if (entry->d_name[0] == '.' && all_entries == 0) {
            continue;
        }

        // Construct the full path of the file or directory
        char fullpath[PATH_MAX];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", directory, entry->d_name);

        // Check if the entry is a directory
        struct stat entryStat;
        if (stat(fullpath, &entryStat) == 0) {
            if (S_ISREG(entryStat.st_mode) || (S_ISDIR(entryStat.st_mode))) {
                // Store entry information in the 'entries' array
                struct FileInfo entryInfo;
                snprintf(entryInfo.filename, sizeof(entryInfo.filename), "%s", entry->d_name);
                entryInfo.modification_time = entryStat.st_mtime;
                entryInfo.permissions = entryStat.st_mode & 0777; // Store permissions as an octal number

                (*entryCount)++;
                *entries = (struct FileInfo*)realloc(*entries, *entryCount * sizeof(struct FileInfo));
                (*entries)[*entryCount - 1] = entryInfo;
            }
        }
    }

    closedir(dir);
}


// Function to determine which files need synchronization
void identifyFilesForSync(const struct FileInfo* filesDir1, int countDir1, const struct FileInfo* filesDir2, int countDir2) {
    // Iterate through files in the first directory and check if they exist in the second directory or if they differ
    for (int i = 0; i < countDir1; i++) {
        int foundInDir2 = 0;

        for (int j = 0; j < countDir2; j++) {
            if (strcmp(filesDir1[i].filename, filesDir2[j].filename) == 0) {
                foundInDir2 = 1;

                // Compare modification times and permissions to determine if the file needs synchronization
                if (filesDir1[i].modification_time != filesDir2[j].modification_time ||
                    filesDir1[i].permissions != filesDir2[j].permissions) {
                    // Perform synchronization for this file
                    // You can add logic to copy the file, update permissions, etc.
                    // realpath(filesDir1[i].filename, filesDir1[i].filename);
                    printf("Synchronize file: %s\n", filesDir1[i].filename);
                }
                break; // File found in both directories, no need to search further
            }
        }

        // If the file is not found in the second directory, it needs synchronization
        if (!foundInDir2) {
            // Perform synchronization for this file
            // You can add logic to copy the file, update permissions, etc.
            // realpath(filesDir1[i].filename, filesDir1[i].filename);
            printf("Synchronize file: %s\n", filesDir1[i].filename);
        }
    }

    // Check for files in the second directory that are not in the first directory
    for (int i = 0; i < countDir2; i++) {
        int foundInDir1 = 0;

        for (int j = 0; j < countDir1; j++) {
            if (strcmp(filesDir2[i].filename, filesDir1[j].filename) == 0) {
                foundInDir1 = 1;
                break; // File found in both directories, no need to search further
            }
        }

        // If the file is not found in the first directory, it needs synchronization
        if (!foundInDir1) {
            // Perform synchronization for this file
            // You can add logic to copy the file, update permissions, etc.
            printf("Synchronize file: %s\n", filesDir2[i].filename);
        }
    }
}




// Function to perform the synchronization
void synchronizeFiles(const char* sourceDir, const char* destDir, const struct FileInfo* files, int fileCount, int preservePermissions) {
    for (int i = 0; i < fileCount; i++) {
        const char* sourcePath = files[i].filename;
        char destPath[PATH_MAX];

        // Construct the full destination path
        snprintf(destPath, sizeof(destPath), "%s/%s", destDir, sourcePath);

        // Open source file for reading
        int srcFile = open(sourcePath, O_RDONLY);
        if (srcFile == -1) {
            perror("open");
            continue; // Skip this file if it cannot be opened
        }

        // Open destination file for writing
        int destFile = open(destPath, O_WRONLY | O_CREAT, files[i].permissions);
        if (destFile == -1) {
            perror("open");
            close(srcFile); // Close the source file before skipping
            continue;
        }

        // Read and write the file content
        char buffer[4096];
        ssize_t bytesRead;

        while ((bytesRead = read(srcFile, buffer, sizeof(buffer))) > 0) {
            ssize_t bytesWritten = write(destFile, buffer, bytesRead);
            if (bytesWritten != bytesRead) {
                perror("write");
                break;
            }
        }

        // Close the files
        close(srcFile);
        close(destFile);

        // Preserve modification time if requested
        if (preservePermissions) {
            struct stat st;
            if (stat(sourcePath, &st) == 0) {
                struct utimbuf utimebuf;
                utimebuf.actime = st.st_atime;
                utimebuf.modtime = st.st_mtime;
                utime(destPath, &utimebuf);
            }
        }
    }
}

