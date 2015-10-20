#ifndef LOG_H
#define LOG_H
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define TIME_STRING_SIZE 10
#define MAX_MESSAGE_SIZE 1024

/*
 * Shows commits in chronological order.
 */
void show_commits();

#endif
