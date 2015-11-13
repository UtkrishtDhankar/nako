#ifndef CLEAN_H
#define CLEAN_H

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>

/*
 * Removes the nako repository in the current folder.
 */
int removedir(char path[500]);
int clean_repo();

#endif
