#include "sha1.h"

void create_hash_string(char *hash_string, const unsigned char *hash)
{
	strcpy(hash_string, "");
	for (int i = 0; i < 20; i++) {
		char buf[3];
		sprintf(buf, "%02x", hash[i]);
		strcat(hash_string, buf);
	}
}

/*
 * Gets the number of bytes in a file
 */
static size_t get_file_size(FILE* f)
{
	fseek(f, 0, SEEK_END);
	size_t file_size = ftell(f);

	rewind(f);

	return file_size;
}

/*
 * Returns a hash of the file named file_name
 */
char *sha1(const char *file_name)
{
	/* Open the file requested by the user and find its file size. */
	FILE *input_file = fopen(file_name, "r");

	uint64_t message_size_bytes = get_file_size(input_file);

	/* We need to store the contents of the file in a string to further.
	   The string needs to be as long as it is to have enough space for the
	   padding that'll be done later */
	unsigned char *message = malloc((message_size_bytes + 64 - message_size_bytes % 64)
	                        * (sizeof(*message)));
	fread(message, sizeof(*message), message_size_bytes, input_file);
	fclose(input_file);

	unsigned char *hash = malloc(SHA_DIGEST_LENGTH * sizeof(*hash));
	SHA1(message, message_size_bytes, hash);

	char *hash_string = malloc(41 * sizeof(*hash_string));
	create_hash_string(hash_string, hash);
	return hash_string;
}
