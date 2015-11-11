#include "init.h"

int repo_status()
{
	struct stat st;

	if (stat(".nako", &st) != -1)
		return 1;
	else
		return 0;
}

int init_repo()
{
	if (repo_status() == 0) {
		mkdir(".nako", 0777);
		mkdir(".nako/objects", 0777);
		mkdir(".nako/branches", 0777);
		mkdir(".nako/snaps", 0777);
		mkdir(".nako/tags", 0777);
	} else
		printf("Error, there is already a repo in this directory\n");
	return 0;
}
