#ifndef SWITCH_H
#define SWITCH_H
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>
#include "sha1.h"

void switch_to_snap(const char *snap_hash);

#endif
