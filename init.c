#include "init.h"

int init_repo()
{
	struct stat st = {0};
	if (stat(".nako", &st) != -1) {
		printf("Error, there is already a repo in this directory\n");
		return 1;
	} else {
		mkdir(".nako", 0777);
		mkdir(".nako/objects", 0777);
		mkdir(".nako/branches", 0777);
		mkdir(".nako/snaps", 0777);
		mkdir(".nako/tags", 0777);

		return 0;
	}
}
