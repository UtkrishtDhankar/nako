#ifndef INIT_H
#define INIT_H

#include <stdio.h>
#include <sys/stat.h>

/*
 * Initialises a nako repository in the current folder.
 * Rturns 0 if everything went well, otherwise returns non-zero.
 */
int init_repo();

#endif
