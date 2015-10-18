#include <stdio.h>
#include <stdlib.h>

#include "init.h"
#include "snap.h"

void print_usage()
{
	printf("Usage:	nako <command> [arguments]\n");
}

void print_snap_usage()
{
	printf("Usage:	nako snap <message>\n");
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
		else if (strcmp(argv[1], "snap") == 0) {
			if (argv[2] == NULL)
				print_snap_usage();
			else
				snap_all(argv[2]);
		} else if (strcmp(argv[1], "branch") == 0)
			;
		else if (strcmp(argv[1], "switch") == 0)
			;
		else if (strcmp(argv[1], "merge") == 0)
			;
		else
			print_usage();
	} else
		print_usage();

	return EXIT_SUCCESS;
}
