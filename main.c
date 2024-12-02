#include "argumentParser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <regex.h>
#include <fnmatch.h>
#include <libgen.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

//prototyes:
static void crawl(char path[], int maxDepth, long *sizeSum);

static const int MAX_DEPTH = 128;

void crawl_init(char path[]) {
    long *sizeSum = (long*) malloc(sizeof(long));
    *sizeSum = 0;
    //setup log file
    //put @param path in first line of log file
    int status;
    mode_t mode = 0000770; //owner and group can do rwx
    status = mkdir("/home/joppo/dirSizeLogs", mode);
    if (status != 0) {
        printf("%s\n", "[e] could not create directory. It may already exist.");
    }
    char *name = basename(path);

    char *log_file = NULL;
    snprintf(log_file, 4096, "%s/%s.log", "/home/joppo/dieSizeLogs", name);

    //check if file exists:
    int print_headline = 0;
    if (access(log_file, F_OK) != 0) {
        print_headline = 1;
    }

    //make log file for path (if it doesn't exist)
    FILE *file = NULL;
    file = fopen(log_file, "w");
    //file exists now and is writeable
    if (print_headline)
        fprintf(file, "%s\n", path);


    crawl(path, MAX_DEPTH, sizeSum);

    struct timeval t;
    gettimeofday(&t, NULL);

    fprintf(file, "%lu, %d\n", t.tv_sec, *sizeSum);

    fclose(file);
    free(sizeSum);
}

static void crawl(char path[], int maxDepth, long *sizeSum) {

    if (maxDepth <= 0) {
        return;
    }

    struct stat st;
    lstat(path, &st);
    if (!S_ISDIR(st.st_mode)) {
        //path is a not a directory
        long file_size = st.st_size;
        sizeSum += file_size;
        return; //early return
    }

    //printLineUnderConditions(path, pattern, type, sizeMode, size, line_regex);

    DIR* dir = opendir(path);
    if (!dir) return;
    struct dirent* entry = NULL;
    while((entry = readdir(dir))) {
        char* d_name = entry->d_name;

        if (!strcmp(d_name, ".") || !strcmp(d_name, "..")) {
            continue; //ignore . and ..
        }

        //check if path ends with '/':
        size_t len = strlen(path);
        if (path[len-1] == '/') {
            path[len-1] = '\0';
        }

        char curr_path[1024];
        snprintf(curr_path, 1024, "%s/%s", path, d_name);

        if(entry->d_type == 8) {
            //file
            lstat(curr_path, &st);
            sizeSum += st.st_size;
        }

        //invoke recursion:
        crawl(curr_path, maxDepth-1, sizeSum);
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {
    int success = initArgumentParser(argc, argv);
    if (success == -1) {
        return -1;
    }

    // i need:
    // log file path
    // some kind of struct, I'm summing up the sizes in

    int arguments_count = getNumberOfArguments();
    for (int i = 0; i < arguments_count; ++i) {
        char *path = getArgument(i);
        crawl_init(path);
    }

    return 0;
}
