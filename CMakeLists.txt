cmake_minimum_required(VERSION 3.30)
project(directorySizeHistory C)

set(CMAKE_C_STANDARD 11)

add_executable(directorySizeHistory main.c
        argumentParser.c
        argumentParser.h
        logger.c
        logger.h)
