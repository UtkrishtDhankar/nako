#ifndef SHA1_H
#define SHA1_H

#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_STRING_SIZE 40

/*
 * Returns a hash of the file named file_name
 */
char *sha1(const char *file_name);

#endif
