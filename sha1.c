#include "sha1.h"

/*
 * Performs the left rotate operation on num by i.
 */
static inline uint32_t left_rot (uint32_t num, short i)
{
	return ((num << i) | (num >> (32 - i)));
}

/*
 * Creates 80 32-bit words out of the 512 bit chunk
 */
static inline uint32_t *wordify(unsigned char *chunk)
{
	uint32_t *words = malloc (80 * (sizeof(*words)));

	for (int i = 0; i < 16; i++) {
		words[i] = 0;

		for (int j = 0; j < 4; j++) {
			words[i] += ((uint32_t)chunk[4 * i + j]) << ((3 - j) * 8);
		}
	}

	for (int i = 16; i < 80; i++) {
		words[i] = words[i - 3] ^ words[i - 8]
		         ^ words[i - 14] ^ words[i - 16];

		words[i] = left_rot(words[i], 1);
	}

	return words;
}

/*
 * Calculates the SHA1 hash from given chunks.
 */
static uint32_t *sha1_from_chunks(unsigned char **chunks, long num_chunks)
{
	uint32_t *hash = malloc (5 * sizeof(*hash));

	hash[0] = 0x67452301;
	hash[1] = 0xefcdab89;
	hash[2] = 0x98badcfe;
	hash[3] = 0x10325476;
	hash[4] = 0xc3d2e1f0;

	for (int i = 0; i < num_chunks; i++) {
		uint32_t a = hash[0];
		uint32_t b = hash[1];
		uint32_t c = hash[2];
		uint32_t d = hash[3];
		uint32_t e = hash[4];

		uint32_t f;
		uint32_t k;

		uint32_t *words = wordify(chunks[i]);

		for (int j = 0; j < 80; j++) {
			if (j < 20) {
				f = (b & c) | ((~b) & d);
				k = 0x5a827999;
			} else if (j < 40) {
				f = b ^ c ^ d;
				k = 0x6ed9eba1;
			} else if (j < 60) {
				f = (b & c) | (b & d) | (c & d);
				k = 0x8f1bbcdc;
			} else {
				f = b ^ c ^ d;
				k = 0xca62c1d6;
			}

			uint32_t temp = left_rot(a, 5) + f + e + k + words[j];

			e = d;
			d = c;
			c = left_rot(b, 30);
			b = a;
			a = temp;
		}

		hash[0] += a;
		hash[1] += b;
		hash[2] += c;
		hash[3] += d;
		hash[4] += e;

		free(words);
	}

	return hash;
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
 * Returns an array of chunks on the heap from the message
 */
static unsigned char **sha1_chunkify(const unsigned char *message,
	                             const uint64_t message_length)
{
	long num_chunks = message_length / 64;
	unsigned char **chunks = malloc (num_chunks * sizeof(*chunks));

	for (int i = 0; i < num_chunks; i++) {
		chunks[i] = malloc (64 * sizeof(*chunks[i]));

		for (int j = 0; j < 64; j++) {
			chunks[i][j] = message[64 * i + j];
		}
	}

	return chunks;
}
/*
 * Pad the message with its length.
 * The input lengths are in bytes, while the padding, in accordance with
 * the SHA1 algorithm, is done in bits.
 */
static inline void append_msg_len(unsigned char *message,
			          uint64_t *message_length,
				  uint64_t original_msglen)
{
	int shift = 56;
	while (shift >= 0) {
		/* Add the next 8 bits. */
		message[*message_length] = (8 * original_msglen >> shift) & 0xff;
		shift -= 8;
		*message_length += 1;
	}
}

/*
 * Appends enough zeroes until the message has just enough room for appending
 * the message length, i.e, length 448 mod 512
 */
static inline void append_zeroes(unsigned char *message, uint64_t *message_length)
{
	while (*message_length % 64 != 56) {
		message[*message_length] = (unsigned char) 0x00;
		*message_length += 1;
	}
}

/*
 * Pads the message according to sha1. Also modifies message_length to be the
 * length after the padding
 *
 * This works with unsigned character arrays. Of course, as we'll be padding
 * with zeroes, further processing needs to not consider this array as
 * a null terminated string.
 */
static void sha1_pad(unsigned char *message, uint64_t *message_length)
{
	uint64_t original_msglen = *message_length;

	/* pad with a 1 at the end. As we need to add zeroes until the message
	   length (bits) is congruent with 448 mod 512, we add a 10000000 ()*/
	message[*message_length] = (unsigned char) 0x80;
	*message_length += 1;

	append_zeroes(message, message_length);

	append_msg_len(message, message_length, original_msglen);
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

	sha1_pad(message, &message_size_bytes);

	unsigned char **chunks = sha1_chunkify(message, message_size_bytes);
	long num_chunks = message_size_bytes / 64;

	uint32_t *hash = sha1_from_chunks(chunks, num_chunks);

	char *hash_string = malloc (41 * sizeof(*hash_string));
	sprintf(hash_string, "%08x%08x%08x%08x%08x", hash[0], hash[1], hash[2], hash[3], hash[4]);

	for (int i = 0; i < num_chunks; i++)
		free(chunks[i]);
	free(chunks);
	free(hash);
	free(message);

	printf("%s\n", hash_string);

	return hash_string;
}
