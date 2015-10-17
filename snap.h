#ifndef SNAP_H
#define SNAP_H
#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <dirent.h>

#include "sha1.h"

/*
 * Snaps all the files and directories in the project root directory.
 */
void snap_all(char *message);

#endif
