#include "switch.h"

/*
 * Restores an object to the current directory
 * Here, snap_line is the snap of the
 */
static inline void restore_object(const char *snap_line)
{

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
