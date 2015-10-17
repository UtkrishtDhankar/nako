#include <stdio.h>
#include <stdlib.h>

#include "init.h"
#include "snap.h"

int print_usage()
{
	printf("nako <command> [arguments]\n");
	return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
	if (argc > 1) {
		if (strcmp(argv[1], "init") == 0)
			init_repo();
		else if (strcmp(argv[1], "status") == 0)
			;
		else if (strcmp(argv[1], "stage") == 0)
			;
		else if (strcmp(argv[1], "destage") == 0)
			;
		else if (strcmp(argv[1], "snap") == 0)
			snap_all("Test");
		else if (strcmp(argv[1], "branch") == 0)
			;
		else if (strcmp(argv[1], "switch") == 0)
			;
		else if (strcmp(argv[1], "merge") == 0)
			;
		else
			print_usage();
	}

	return EXIT_SUCCESS;
}
