#include <stdio.h>

#include "init.h"

#include "snap.h"

int main(int argc, char *argv[])
{
	if (argc > 1) {
		if (strcmp(argv[1], "init") == 0)
			;
		else if (strcmp(argv[1], "status") == 0)
			;
		else if (strcmp(argv[1], "stage") == 0)
			;
		else if (strcmp(argv[1], "destage") == 0)
			;
		else if (strcmp(argv[1], "snap") == 0)
			;
		else if (strcmp(argv[1], "branch") == 0)
			;
		else if (strcmp(argv[1], "switch") == 0)
			;
		else if (strcmp(argv[1], "merge") == 0)
			;
		else if (strcmp(argv[1], "hello world") == 0)
			;
		else
			printf("Wrong argument\n");
	}
	return EXIT_SUCCESS;
}
