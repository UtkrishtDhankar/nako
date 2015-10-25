#ifndef INIT_H
#define INIT_H

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

/*
 * Initialises a nako repository in the current folder.
 */
int init_repo();
int init_clean();

#endif
