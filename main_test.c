#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "blowfish.h"

void tencrypt(uint8_t *plaintext_string, uint8_t *key, uint8_t *ciphertext_string);
void tdecrypt(uint8_t *ciphertext_string, uint8_t *key, uint8_t *plaintext_string);

int main(char *argc, char **argv) {

	FILE *fp = NULL;	
	char *source = NULL;
	long filesize;
	size_t newLen;
	uint8_t *key = "key"; 
	int i = 0;

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

	uint8_t *ciphertext_string = malloc(sizeof(uint8_t) * 16 + sizeof(uint8_t) * 16 * (8 / 8) + 1000);
	uint8_t *plaintext_string = malloc(sizeof(uint8_t) * 16 + sizeof(uint8_t) * 16 * (8 / 8) + 1000);
	
	printf("Plaintext message string is: %s\n", source);
	tencrypt(source, key, ciphertext_string);
	printf("Encrypted message string is: %s\n", ciphertext_string);
	tdecrypt(ciphertext_string, key, source);
	printf("Plaintext message string is: %s\n", source);

	free(source);

	return 0;
}

void tencrypt(uint8_t *plaintext_string, uint8_t *key, uint8_t *ciphertext_string) {
	blowfish_t ctx;

	int keylen = strlen(key);
	int plaintext_len = strlen(plaintext_string);

	uint32_t message_left;
	uint32_t message_right;
	int block_len;

	init(&ctx, key, keylen);
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

		encrypt(&ctx, &message_left, &message_right);
		*ciphertext_string++ = (uint8_t)(message_left >> 24);
		*ciphertext_string++ = (uint8_t)(message_left >> 16);
		*ciphertext_string++ = (uint8_t)(message_left >> 8);
		*ciphertext_string++ = (uint8_t)message_left;
		*ciphertext_string++ = (uint8_t)(message_right >> 24);
		*ciphertext_string++ = (uint8_t)(message_right >> 16);
		*ciphertext_string++ = (uint8_t)(message_right >> 8);
		*ciphertext_string++ = (uint8_t)message_right;
	}
	
	*ciphertext_string = '\0';	
}

void tdecrypt(uint8_t *ciphertext_string, uint8_t *key, uint8_t *plaintext_string) {
	blowfish_t ctx;
	
	int keylen = strlen(key);
	int ciphertext_len = strlen(ciphertext_string);

	uint32_t message_left;
	uint32_t message_right;
	int block_len;

	init(&ctx, key, keylen);
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
		*plaintext_string++ = (uint8_t)(message_left >> 24);
		*plaintext_string++ = (uint8_t)(message_left >> 16);
		*plaintext_string++ = (uint8_t)(message_left >> 8);
		*plaintext_string++ = (uint8_t)message_left;
		*plaintext_string++ = (uint8_t)(message_right >> 24);
		*plaintext_string++ = (uint8_t)(message_right >> 16);
		*plaintext_string++ = (uint8_t)(message_right >> 8);
		*plaintext_string++ = (uint8_t)message_right;
	}

	*plaintext_string = '\0';
}
