#include "clean.h"

int removedir(char path[500])
{
	DIR *pdir = NULL;
	struct dirent *pent = NULL;
	struct stat eStat;

	pdir = opendir(path);
	char x[500];

	if ( pdir == NULL)
		return 1;

	while ((pent = readdir(pdir)) != NULL) {
		if ((strcmp((pent->d_name), ".") == 0) || (strcmp((pent->d_name),
								  "..") == 0))
			continue;
		else {
			strcpy(x, path);

			path = strcat(path, "/");
			path = strcat(path, pent->d_name);

			if (stat(path, &eStat))
				printf("This is a file and not a directory."
				       "Error = %s\n", strerror(errno));
			else {
				if (S_ISDIR(eStat.st_mode)) {
					removedir(path);
					strcpy(path, x);
				} else {
					unlink(path);
					strcpy(path, x);
				}
			}
		}
	}

	if (!rmdir(path))
		return 0;
	else
		return 1;

}

int clean_repo()
{
	char *path_dir = malloc(500);
	strcpy(path_dir, ".nako");

	removedir(path_dir);
	
	return 0;
}
