#include "switch.h"

#define KILOBYTE      1024
#define HASH_STR_SIZE 40

/*
 * Restores an object to the current directory
 * Here, snap_line is the line containing the hash and the name of the object
 */
static void restore_object(const char *snap_line)
{
	char hash[HASH_STR_SIZE + 1];

	sscanf(snap_line, "%s", hash);

	/* Allocating 1 KB of memory for object name. Will reallocate more
	   space if needed. */
	char *object_name = malloc(KILOBYTE * sizeof(*object_name));

	int snap_pos = HASH_STR_SIZE;
	int name_pos = 0;
	int name_size = KILOBYTE;
	while(snap_line[snap_pos++] != '\0') {
		object_name[name_pos++] = snap_line[snap_pos];
		if (name_pos >= name_size) {
			object_name = realloc(object_name,
				name_size + KILOBYTE);
		}
	}

	printf("HASH - %s\nNAME - %s", hash, object_name);
}

void switch_to_snap(const char *snap_hash)
{
	char *snap_file_name;
	if (asprintf(&snap_file_name, ".nako/snaps/%s", snap_hash) == -1)
		exit(1);

	FILE* snap_file = fopen(snap_file_name, "rx");
	if (snap_file == NULL)
		exit(1);

	char   *line   = NULL;
	size_t linecap = 0;
	while (getline(&line, &linecap, snap_file) != -1) {
		restore_object(line);
	}

	fclose(snap_file);
	free(snap_file_name);
	free(line);
}
