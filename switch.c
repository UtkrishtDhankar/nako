#include "switch.h"

#define KILOBYTE      1024
#define HASH_STR_SIZE 40

/*
 * Copies the source file's contents to dest.
 */
static inline void copy_file (FILE *dest, FILE *source)
{
	char ch;
	while((ch = fgetc(source)) != EOF) {
		fputc(ch, dest);
	}
}

/*
 * Restores the file whose name is name, and has the SHA1 digest hash.
 */
static void restore_file(const char *name, const char *hash)
{
	FILE *file  = fopen(name, "wx");

	char *object_name = NULL;
	asprintf(&object_name, "%s/%s", ".nako/objects",
			hash);
	FILE *object_file = fopen(object_name, "r");
	free(object_name);

	if (file == NULL) { /* If there was already a file of that name. */
		char *file_hash = sha1(name);
		if (strcmp(file_hash, hash) == 0) {
			; /* The file hasn't been changed, nothing to do  */
		} else {
			file = fopen(name, "w");

			copy_file(file, object_file);
		}
	} else {
		copy_file(file, object_file);
	}

	fclose(file);
	fclose(object_file);
}

/*
 * Writes the full name of the file (relative to project parent directory)
 * to a char pointer on the heap. Remember to free this object after
 * calling this function. This is part of the internals of restore_object().
 */
static inline char *extract_name(const char *snap_line)
{
	/* Allocating 1 KB of memory for object name. Will reallocate more
	   space if needed. */
	char *object_name = malloc(KILOBYTE * sizeof(*object_name));

	int snap_pos = HASH_STR_SIZE;
	int name_pos = 0;
	int name_size = KILOBYTE;

	while(snap_line[snap_pos++] != '\n') {
		object_name[name_pos++] = snap_line[snap_pos];
		if (name_pos >= name_size) {
			object_name = realloc(object_name,
				name_size + KILOBYTE);
		}
	}

	/* Overwrite the '\n' at the end of the name with '\0' to
	terminate the string. */
	object_name[name_pos - 1] = '\0';

	return object_name;
}

/*
 * Returns true if object is a directory, otherwise returns false. Is part of
 * the internals of restore_object.
 */
static inline bool is_dir(const char *object_name)
{
	size_t len = strlen(object_name);
	if (object_name[len - 1] == '/')
		return true;
	else
		return false;
}

/*
 * Restores an object to the current directory
 * Here, snap_line is the line containing the hash and
 * the name of the object.
 */
static void restore_object(const char *snap_line)
{
	char hash[HASH_STR_SIZE + 1];

	sscanf(snap_line, "%s", hash);

	char *object_name = extract_name(snap_line);

	if (is_dir(object_name)) {
		printf("Directory\n");
	} else {
		restore_file(object_name, hash);
	}

	free(object_name);
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

	/* Skip over the first two lines (snap message and time) */
	for (int i = 0; i < 2; i++) {
		getline(&line, &linecap, snap_file);
	}

	while (getline(&line, &linecap, snap_file) != -1) {
		restore_object(line);
	}

	fclose(snap_file);

	free(snap_file_name);
	free(line);
}
