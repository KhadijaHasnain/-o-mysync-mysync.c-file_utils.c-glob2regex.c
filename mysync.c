#include <time.h>  // Include for time_t
#include "mysync.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

int main(int argc, char *argv[]) {
    
    // Define variables to hold command-line options and directory names
    int opt;
    int option_a = 0; // Flag for -a option
    int option_n = 0; // Flag for -n option
    int option_p = 0; // Flag for -p option
    int option_r = 0; // Flag for -r option
    int option_v = 0; // Flag for -v option
    int num_ignore_patterns = 0; // Number of -i patterns
    const char* ignore_patterns[MAX_IGNORE_PATTERNS]; // Store the -i patterns
    int option_o = 0; // Flag for -o option
    const char* output_pattern = NULL; // Store the -o pattern

    // Process command-line options
    while ((opt = getopt(argc, argv, "ainoprv")) != -1) {
        switch (opt) {
            case 'a':
                printf("Option A\n");
                option_a = 1;
                break;
            case 'i':
                if (num_ignore_patterns < MAX_IGNORE_PATTERNS) {
                    ignore_patterns[num_ignore_patterns] = glob2regex(*optarg); // Store the pattern
                    printf(ignore_patterns[num_ignore_patterns]);
                    num_ignore_patterns++;
                } else {
                    fprintf(stderr, "Warning: Maximum number of -i patterns reached.\n");
                }
                break;
            case 'n':
                printf("Option N\n");
                option_n = 1;
                option_v = 1; // -n implies -v
                break;
            case 'o':
                option_o = 1;
                output_pattern = optarg; // Store the -o pattern
                break;
            case 'p':
                option_p = 1;
                break;
            case 'r':
                option_r = 1;
                break;
            case 'v':
                option_v = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [options] directory1 directory2 [directory3 ...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Validate that there are at least two directory arguments
    if (argc - optind < 2) {
        fprintf(stderr, "Error: At least two directory arguments are required.\n");
        exit(EXIT_FAILURE);
    }

    // Obtain the directory names from command-line arguments
    // Obtain the directory names from command-line arguments
    const char* sourceDir = argv[optind];
    const char* destDir = argv[optind + 1];

    // Declare variables to store file information
    struct FileInfo* filesDir1 = NULL;
    struct FileInfo* filesDir2 = NULL;
    int countDir1 = 0;
    int countDir2 = 0;

    // Call functions to list files in the source and destination directories
    // listFiles(sourceDir, &filesDir1, &countDir1, option_a);
    // listFiles(destDir, &filesDir2, &countDir2, option_a);
    listFilesAndDirectories(sourceDir, &filesDir1, &countDir1, option_a);
    listFilesAndDirectories(destDir, &filesDir2, &countDir2, option_a);

    // Call a function to identify files that need synchronization
    identifyFilesForSync(filesDir1, countDir1, filesDir2, countDir2);

    // // Perform synchronization based on the identified files and options
    // synchronizeFiles(sourceDir, destDir, filesDir1, countDir1, option_p);

    // Free the allocated memory for file information
    free(filesDir1);
    free(filesDir2);

    return 0;

    return 0;
}

