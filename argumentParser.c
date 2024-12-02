#include "argumentParser.h"
#include <stddef.h>

// includes
#include <stdint.h>

// global vars
int MAX_LENGTH = 256;

uint32_t args_opts_num;
char* program;
char** arg_values;
uint32_t num_of_args;

// helpers
uint32_t first_opt_i;
/**
 *
 * @param arg the argument to be validated
 * @return 1 if arg is an option, 0 if arg is a program argument
 */
int validateArg(char *arg) {
    if (arg[0] != '-') return 0;
    //leading dash, so HAS to have an '=' to be an option
    for (int i = 1; i < MAX_LENGTH; ++i) {
        if (arg[i] == '\0') { // reached end of string
            return 0;
        }
        if (arg[i] == '=') {
            break;
        }
    }
    return 1;
}

int initArgumentParser(int argc, char* argv[]) {
    if (argc < 1) {
        return -1;
    }
    //validate argv:
    int first_opt_found = 0;
    for (int i = 0; i < argc; ++i) {
        //arg can't come after option
        int is_option = validateArg(argv[i]);
        if (!is_option && first_opt_found) { //argument after an option was already found
            return -1;
        }
        if (is_option && !first_opt_found) {
            first_opt_found = 1;
            first_opt_i = i;
            num_of_args = i - 1;
        }
    }
    if (!first_opt_found) { // no options were found
        num_of_args = argc -1;
    }
    args_opts_num = argc - 1;
    program = argv[0];
    arg_values = argv;
    //num_of_args = i - 1;
    return 0;
}

char* getCommand(void) {
    return program;
}

int getNumberOfArguments(void) {
    return (int) num_of_args;
}

char* getArgument(int index) {
    if (index >= num_of_args) {
        return NULL;
    }
    char* arg = arg_values[index+1];
    return arg;
}

char* getValueForOption(char *keyName) {
    uint32_t opts_num = args_opts_num - num_of_args;
    for (int i = 0; i < opts_num; ++i) {
        char* curr_opt = arg_values[first_opt_i + i];
        for (int j = 1; j < MAX_LENGTH; ++j) {
            if (keyName[j-1] == '\0' && curr_opt[j] == '=') {
                //end of key
                return &curr_opt[j+1];
            }
            if (curr_opt[j] != keyName[j-1]) {
                break;
            }
        }
    }
    return NULL;
}
