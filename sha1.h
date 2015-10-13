#ifndef SHA1_H
#define SHA1_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

/*
 * Returns a hash of the file named file_name
 */
char *sha1(const char *file_name);

#endif
