#include "switch.h"

void switch_to_snap(const char *snap_hash)
{
	char *snap_file_name;
	if (asprintf(&snap_file_name, ".nako/snaps/%s", snap_hash) == -1)
		exit(1);

	FILE* snap_file = fopen(snap_file_name, "rx");
	if (snap_file == NULL)
		exit(1);

	char ch;
	while ((ch = getc(snap_file)) != EOF) {
		putc(ch, stdout);
	}
}
