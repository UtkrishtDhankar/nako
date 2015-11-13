#ifndef INIT_H
#define INIT_H

#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <sys/stat.h>
#include <ftw.h>
#include <unistd.h>

/*
 * Initialises a nako repository in the current folder or removes it, if present.
 */
int repo_status();
int init_repo();
int remove_repo();

#endif
