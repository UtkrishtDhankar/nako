#include "init.h"

#define MODE 0777

int init_repo()
{
	struct stat st;

	if (stat(".nako", &st) != -1) {
		printf("Error, there is already a repo in this directory\n");
		return 1;
	} else {
		mkdir(".nako", MODE);
		mkdir(".nako/objects", MODE);
		mkdir(".nako/branches", MODE);
		mkdir(".nako/snaps", MODE);
		mkdir(".nako/tags", MODE);

		return 0;
	}
}
