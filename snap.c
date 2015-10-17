#include "snap.h"

/* http://stackoverflow.com/questions/1121383/counting-the-number-of-files-in-a-directory-using-c */
int get_num_files(DIR *dirp)
{
	int num_files = 0;
	struct dirent *entry;

	while ((entry = readdir(dirp)) != NULL)
		if (entry->d_type == DT_REG)
			num_files++;

	rewinddir(dirp);

	return num_files;
}

/* http://stackoverflow.com/questions/1121383/counting-the-number-of-files-in-a-directory-using-c */
int get_num_dirs(DIR *dirp)
{
	int num_dirs = 0;
	struct dirent *entry;

	while ((entry = readdir(dirp)) != NULL)
		if (entry->d_type == DT_DIR)
			num_dirs++;

	rewinddir(dirp);

	num_dirs -= 2; /* We don't need . and .. directories.*/

	return num_dirs;
}

/* FIXME add a list of ignored directories and files */

char *snap_file(const char *file_name)
{
	char *file_hash = sha1(file_name);
	/* 14 bytes for ".nako/objects/", rest for hash and '\0'. */
	char *object_path = malloc((14 + strlen(file_hash) + 1) * sizeof(*object_path));

	strcpy(object_path, ".nako/objects/");
	strcat(object_path, file_hash);

	FILE *input_file = fopen(file_name, "r");
	FILE *object_file = fopen(object_path, "wx");
	if (object_file != NULL) {
		char ch;
		while ((ch = getc(input_file)) != EOF)
			putc(ch, object_file);

		fclose(object_file);
	} else {
		/* If a file already exists, nothing to be done */
	}

	fclose(input_file);
	free(object_path);

	return file_hash;
}

char *snap_dir(const char *dir_name)
{
	DIR *root = opendir(dir_name);

	struct dirent *dp;

	int num_dirs  = get_num_dirs(root);
	int num_files = get_num_files(root);

	char **dir_contents = malloc((num_dirs + num_files) * sizeof(*dir_contents));
	int i = 0;

	while ((dp = readdir(root)) != NULL) {
		if (dp->d_type == DT_REG) {
			/* should be dir_name/dp->d_name */
			char *buf = malloc((strlen(dir_name) + 1 + strlen(dp->d_name) + 1) * sizeof(*buf));
			strcpy(buf, dir_name);
			strcat(buf, "/");
			strcat(buf, dp->d_name);
			char *hash = snap_file(buf);

			dir_contents[i] = malloc((strlen(hash) + 1 + strlen(dp->d_name) + 1) * sizeof(*dir_contents[i]));
			strcpy(dir_contents[i], hash);
			strcat(dir_contents[i], " ");
			strcat(dir_contents[i], dp->d_name);

			i++;
			free(hash);
			free(buf);
		} else if (dp->d_type == DT_DIR) {
			if (strcmp(dp->d_name, ".")     == 0 ||
			    strcmp(dp->d_name, "..")    == 0)
				continue;

			char *buf = malloc((strlen(dir_name) + 1 +
					    strlen(dp->d_name) + 1) * sizeof(*buf));
			strcpy(buf, dir_name);
			strcat(buf, "/");
			strcat(buf, dp->d_name);
			char *hash = snap_dir(buf);

			dir_contents[i] = malloc((strlen(hash) + 1 +
						  strlen(dp->d_name) + 1) *
						 sizeof(*dir_contents[i]));
			strcpy(dir_contents[i], hash);
			strcat(dir_contents[i], " ");
			strcat(dir_contents[i], dp->d_name);

			i++;
			free(hash);
			free(buf);
		}
	}

	FILE *f = fopen(".nako/objects/temp", "w");
	fprintf(f, "%s\n", dir_name);
	for (int j = 0; j < i; j++) {
		fprintf(f, "%s\n", dir_contents[j]);
		free(dir_contents[j]);
	}
	free(dir_contents);
	fclose(f);

	char *hash = sha1(".nako/objects/temp");

	char *dir_file_name = malloc ((14 + 40 + 1) * sizeof(*dir_file_name));
	strcpy(dir_file_name, ".nako/objects/");
	strcat(dir_file_name, hash);

	rename(".nako/objects/temp", dir_file_name);
	free(dir_file_name);
	return hash;
}

char *snap(char **file_names, const int num_files,
	   char **dir_names,  const int num_dirs,
	   const char *message)
{
	char **snap_contents = malloc((num_files + num_dirs + 1) * sizeof(*snap_contents));
	int counter = 0;

	for (int i = 0; i < num_files; i++) {
		char *hash = snap_file(file_names[i]);

		snap_contents[counter] = malloc((strlen(hash) + 1 +
						 strlen(file_names[i]) + 1) *
						sizeof(*snap_contents[counter]));
		strcpy(snap_contents[counter], hash);
		strcat(snap_contents[counter], " ");
		strcat(snap_contents[counter], file_names[i]);
		counter++;

		free(hash);
	}

	for (int i = 0; i < num_dirs; i++) {
		char *hash = snap_dir(dir_names[i]);

		snap_contents[counter] = malloc((strlen(hash) + 1 +
						 strlen(dir_names[i]) + 1) *
						sizeof(*snap_contents[counter]));
		strcpy(snap_contents[counter], hash);
		strcat(snap_contents[counter], " ");
		strcat(snap_contents[counter], dir_names[i]);
		counter++;

		free(hash);
	}

	FILE *f = fopen(".nako/snaps/temp", "w");
	fprintf(f, "%s\n", message);
	for (int j = 0; j < counter; j++) {
		fprintf(f, "%s\n", snap_contents[j]);
		free(snap_contents[j]);
	}
	free(snap_contents);
	fclose(f);

	char *hash = sha1(".nako/snaps/temp");

	char *snap_file_name = malloc ((12 + 40 + 1) * sizeof(*snap_file_name));
	strcpy(snap_file_name, ".nako/snaps/");
	strcat(snap_file_name, hash);

	rename(".nako/snaps/temp", snap_file_name);
	free(snap_file_name);

	return hash;
}

void snap_all(char *message)
{
	DIR *root = opendir(".");

	int num_files = get_num_files(root);
	int num_dirs  = get_num_dirs(root) - 1; /* We're going to ignore .nako. */

	char **files = malloc(num_files * sizeof(*files));
	char **dirs  = malloc(num_dirs * sizeof(*dirs));

	/* Counters to remember which file/dir we're adding */
	int file_id = 0;
	int dir_id = 0;

	struct dirent *dp;

	while ((dp = readdir(root)) != NULL) {
		if (dp->d_type == DT_REG) {
			files[file_id] = malloc((strlen(dp->d_name) + 1)
						* sizeof(*files[file_id]));
			strcpy(files[file_id], dp->d_name);
			file_id++;
		} else if (dp->d_type == DT_DIR) {
			if (strcmp(dp->d_name, ".nako") == 0 ||
			    strcmp(dp->d_name, ".")     == 0 ||
			    strcmp(dp->d_name, "..")    == 0)
				continue;

			dirs[dir_id] = malloc((strlen(dp->d_name) + 1)
					      * sizeof(*dirs[dir_id]));
			strcpy(dirs[dir_id], dp->d_name);
			dir_id++;
		}
	}
	char *snap_hash = snap(files, num_files, dirs, num_dirs, message);

	printf("New snap - %s - made\n", snap_hash);

	free(snap_hash);

	for (int i = 0; i < num_files; i++)
		free(files[i]);

	for (int i = 0; i < num_dirs; i++)
		free(dirs[i]);

	free(files);
	free(dirs);
}
