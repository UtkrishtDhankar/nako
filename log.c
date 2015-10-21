#include "log.h"

struct snap {
	FILE *file;     /* The file for the snap */

	char *hash;     /* The hash of the snap */
	char *message;  /* The message accompanying the snap. */
	char *time_str; /* The string representation of the unix time
	                   of the snap */
};

static int snapcmp(const void *a, const void *b) 
{
	const struct snap *s1 = a;
	const struct snap *s2 = b;
	
	return -strcmp(s1->time_str, s2->time_str);
}

inline static int get_num_files(DIR *dirp)
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

	struct snap *snaps = malloc(num_files * sizeof(*snaps));

	int counter = 0;
	while ((dp = readdir(snap_dir)) != NULL) {
		if (dp->d_type == DT_REG) {
			snaps[counter].hash = NULL;
			asprintf(&snaps[counter].hash, "%s", dp->d_name);

			char *full_file_name = NULL;
			asprintf(&full_file_name, "%s/%s", snap_dir_name,
					dp->d_name);

			snaps[counter].file = fopen(full_file_name, "r");

			free(full_file_name);

			snaps[counter].message = malloc(1024
				* sizeof(*snaps[counter].message));
			snaps[counter].time_str = malloc(11
				* sizeof(*snaps[counter].time_str));

			fscanf(snaps[counter].file, "%[^\n]s",
				snaps[counter].message);
			fscanf(snaps[counter].file, "%s",
				snaps[counter].time_str);

			counter++;
		}
	}

	qsort(snaps, num_files, sizeof(*snaps), &snapcmp);

	for (counter = 0; counter < num_files; counter++) {
			time_t snap_time = atol(snaps[counter].time_str);
			struct tm *lt = localtime(&snap_time);
			char time_str[80];
			strftime(time_str, 80, "%c", lt);

			printf("%s @ %s s - %s\n", snaps[counter].hash,
				time_str,
				snaps[counter].message);

			free(snaps[counter].time_str);
			free(snaps[counter].message);
			free(snaps[counter].hash);
	}
}
