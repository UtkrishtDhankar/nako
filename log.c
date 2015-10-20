#include "log.h"

static inline int get_num_files(DIR *dirp)
{
	int num_files = 0;
	struct dirent *entry;

	while ((entry = readdir(dirp)) != NULL)
		if (entry->d_type == DT_REG)
			num_files++;

	rewinddir(dirp);

	return num_files;
}

void show_commits()
{
	const char *snap_dir_name = ".nako/snaps";

	DIR *snap_dir = opendir(snap_dir_name);
	struct dirent *dp;

	int num_files = get_num_files(snap_dir);

	char **snap_hashes   = malloc(num_files * sizeof(*snap_hashes));
	char **snap_messages = malloc(num_files * sizeof(*snap_messages));
	char **snap_times    = malloc(num_files * sizeof(*snap_times));

	int counter = 0;
	while ((dp = readdir(snap_dir)) != NULL) {
		if (dp->d_type == DT_REG) {
			snap_hashes[counter] = NULL;
			asprintf(&snap_hashes[counter], "%s", dp->d_name);

			char *full_file_name = NULL;
			asprintf(&full_file_name, "%s/%s", snap_dir_name, dp->d_name);

			FILE *snap_file = fopen(full_file_name, "r");

			free(full_file_name);

			snap_messages[counter] = malloc(1024 * sizeof(*snap_messages[counter]));
			snap_times[counter]    = malloc(11 * sizeof(*snap_times[counter]));
			fscanf(snap_file, "%[^\n]s", snap_messages[counter]);
			fscanf(snap_file, "%s", snap_times[counter]);

			printf("%s @ %s s - %s\n", snap_hashes[counter],
				snap_times[counter], snap_messages[counter]);

			free(snap_times[counter]);
			free(snap_messages[counter]);
			free(snap_hashes[counter]);

			counter++;
		}
	}
}
