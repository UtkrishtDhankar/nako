#include "snap.h"

/*
 * Gets the number of files inside the folder dirp.
 */
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

/*
 * Gets the number of directories inside dirp, excluding . and .. directories.
 */
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
/*
 * Snaps the contents of the file whose name relative to the project parent
 * directory is file_name. Doesn't create a new file if one already exists.
 * Returns the SHA1 hash of the file it snaps.
 */
static char *snap_file(const char *file_name)
{
	char *file_hash = sha1(file_name);
	/* 14 bytes for ".nako/objects/", rest for hash and '\0'. */
	char *object_path = NULL;
	asprintf(&object_path, ".nako/objects/%s", file_hash);

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

/* Implementation is a few lines ahead. Subfunctions of snap_dir call
   snap_dir, so we need a prototype */
static char *snap_dir(const char *dir_name);

/*
 * Writes out a formatted line to content, to be written in the object file
 * of dir_name. Part of the internals of snap_dir.
 */
static inline void handle_file(char **content, const char *dir_name,
			       const char *file_name)
{
	char *buf = NULL;
	asprintf(&buf, "%s/%s", dir_name, file_name);

	char *hash = snap_file(buf);

	asprintf(content, "%s %s", hash, buf);

	free(hash);
	free(buf);
}

/*
 * Stores the line to be written in the snap file for the subdirectory
 * subdir_name of dir_name inside content. Part of internals of snap_dir() */
static inline void handle_dir(char **content, const char *dir_name,
			      const char *subdir_name)
{
	char *buf = NULL;
	asprintf(&buf, "%s/%s", dir_name, subdir_name);

	char *hash = snap_dir(buf);

	content = NULL;
	asprintf(content, "%s %s/", hash, buf);

	free(hash);
	free(buf);
}

/*
 * Creates a snap file for the directory named dir_name, with the contents
 * dir_contents, whose size is num_contents.
 */
static inline char *write_contents(      char **dir_contents,
				   const char  *dir_name,
				   const int    num_contents)
{
	FILE *f = fopen(".nako/objects/temp", "w");
	fprintf(f, "%s\n", dir_name);

	for (int j = 0; j < num_contents; j++) {
		fprintf(f, "%s\n", dir_contents[j]);
	}

	char *hash = sha1(".nako/objects/temp");

	char *dir_file_name = NULL;
	asprintf(&dir_file_name, ".nako/objects/%s", hash);

	rename(".nako/objects/temp", dir_file_name);
	free(dir_file_name);

	return hash;
}

/*
 * Snaps the contents of the directory dir_name recursively.
 * Returns a SHA1 hash of the snap file of the directory.
 */
static char *snap_dir(const char *dir_name)
{
	DIR *root = opendir(dir_name);

	struct dirent *dp;

	int num_dirs  = get_num_dirs(root);
	int num_files = get_num_files(root);

	/* Stores the contents of the snap file for the current directory. */
	char **dir_contents = malloc((num_dirs + num_files)
				* sizeof(*dir_contents));

	int i = 0; /* Counter for storing things in dir_contents */

	while ((dp = readdir(root)) != NULL) {
		if (dp->d_type == DT_REG) {
			handle_file(&dir_contents[i], dir_name ,dp->d_name);

			i++;
		} else if (dp->d_type == DT_DIR) {
			if (strcmp(dp->d_name, ".")     == 0 ||
			    strcmp(dp->d_name, "..")    == 0)
				continue;
			handle_dir(&dir_contents[i], dir_name, dp->d_name);

			i++;
		}
	}
	char *hash = write_contents(dir_contents, dir_name,i);

	for (int j = 0; j < i; j++)
		free(dir_contents[j]);

	free(dir_contents);

	return hash;
}

/*
 * General purpose function to snap the changes in given files and directories
 * with the given snap message
 */
static char *snap(char **file_names, const int num_files,
	   char **dir_names,  const int num_dirs,
	   const char *message)
{
	char **snap_contents = malloc(((num_files + num_dirs)
				* sizeof(*snap_contents)));
	int counter = 0;

	for (int i = 0; i < num_files; i++) {
		char *hash = snap_file(file_names[i]);

		snap_contents[counter] = NULL;
		asprintf(&snap_contents[counter], "%s %s", hash, file_names[i]);
		counter++;

		free(hash);
	}

	for (int i = 0; i < num_dirs; i++) {
		char *hash = snap_dir(dir_names[i]);

		snap_contents[counter] = NULL;
		asprintf(&snap_contents[counter], "%s %s/", hash, file_names[i]);
		counter++;

		free(hash);
	}

	FILE *f = fopen(".nako/snaps/temp", "w");
	fprintf(f, "%s\n", message);

	time_t current_time = time(NULL);
	fprintf(f, "%ld\n", current_time);


	for (int j = 0; j < counter; j++) {
		fprintf(f, "%s\n", snap_contents[j]);
		free(snap_contents[j]);
	}

	free(snap_contents);
	fclose(f);

	char *hash = sha1(".nako/snaps/temp");

	char *snap_file_name = NULL;
	asprintf(&snap_file_name, "%s/%s", ".nako/snaps/", hash);

	rename(".nako/snaps/temp", snap_file_name);
	free(snap_file_name);

	return hash;
}

/*
 * Snaps all the files and folders inside the project parent directory
 */
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
