#include <stdio.h>
#include <string.h>
#include "blowfish.h"

int main(char *argc, char **argv) {

	FILE* fp = NULL;	
	char* source = NULL;
	long filesize;
	size_t newLen;

	if((fp = fopen(argv[1], "r")) != NULL) {
		if (fseek(fp, 0L, SEEK_END) == 0) {
			if((filesize = ftell(fp)) == -1) {
			}

			source = malloc(sizeof(char) * (filesize + 1));
			if (fseek(fp, 0L, SEEK_SET) == 0) {
			}

			newLen = fread(source, sizeof(char), filesize, fp);
			if (newLen == 0) {
				fputs("Error reading file", stderr);
			} else {
				source[++newLen] = '\0';
			}
		}
	}	

	blowfish_t ctx;
	int n;

	/* must be less than 56 bytes */
	uint8_t *key = "a random number string would be a better key";
	int keylen = strlen(key);

	uint8_t *plaintext_string = malloc(sizeof(uint8_t) * 300);
	strcpy(plaintext_string, source);
	int plaintext_len = strlen(plaintext_string);

	uint8_t ciphertext_buffer[256];
	uint8_t *ciphertext_string = &ciphertext_buffer[0];
	int ciphertext_len = 0;

	uint32_t message_left;
	uint32_t message_right;
	int block_len;

	init(&ctx, key, keylen);

	printf("Plaintext message string is: %s\n", plaintext_string);

	/* encrypt the plaintext message string */
	printf("Encrypted message string is: ");

	while (plaintext_len) {
		message_left = message_right = 0UL;

		/* crack the message string into a 64-bit block (ok, really two 32-bit blocks); pad with zeros if necessary */
		for (block_len = 0; block_len < 4; block_len++) {
			message_left = message_left << 8;
			if (plaintext_len) {
				message_left += *plaintext_string++;
				plaintext_len--;
			} else { 
				message_left += 0;
			}
		}
		
		for (block_len = 0; block_len < 4; block_len++) {
			message_right = message_right << 8;
			if (plaintext_len) {
				message_right += *plaintext_string++;
				plaintext_len--;
			} else {
				message_right += 0;
			}
		}

		/* encrypt and print the results */
		encrypt(&ctx, &message_left, &message_right);
		printf("%lx%lx", message_left, message_right);

		/* save the results for decryption below */
		*ciphertext_string++ = (uint8_t)(message_left >> 24);
		*ciphertext_string++ = (uint8_t)(message_left >> 16);
		*ciphertext_string++ = (uint8_t)(message_left >> 8);
		*ciphertext_string++ = (uint8_t)message_left;
		*ciphertext_string++ = (uint8_t)(message_right >> 24);
		*ciphertext_string++ = (uint8_t)(message_right >> 16);
		*ciphertext_string++ = (uint8_t)(message_right >> 8);
		*ciphertext_string++ = (uint8_t)message_right;
		ciphertext_len += 8;

	}	
	printf("\n");

	/* reverse the process */
	printf("Decrypted message string is: ");

	ciphertext_string = &ciphertext_buffer[0];
	while(ciphertext_len) {
		message_left = message_right = 0UL;

		for (block_len = 0; block_len < 4; block_len++) {
			message_left = message_left << 8;
			message_left += *ciphertext_string++;
			if (ciphertext_len) {
				ciphertext_len--;
			}
		}
		
		for (block_len = 0; block_len < 4; block_len++) {
			message_right = message_right << 8;
			message_right += *ciphertext_string++;
			if (ciphertext_len) {
				ciphertext_len--;
			}
		}

		decrypt(&ctx, &message_left, &message_right);

		/* if plaintext message string padded, extra zeros here */

		printf("%c%c%c%c%c%c%c%c",
				(int)(message_left >> 24), (int)(message_left >> 16),
				(int)(message_left >> 8), (int)(message_left),
				(int)(message_right >> 24), (int)(message_right >> 16),
				(int)(message_right >> 8), (int)(message_right));
	}

	printf("\n");

	return 0;
}
