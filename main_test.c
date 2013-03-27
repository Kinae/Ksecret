#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "blowfish.h"

void tencrypt(uint8_t *plaintext_string, uint8_t *key, uint8_t *ciphertext_string);
void tdecrypt(uint8_t *ciphertext_string, uint8_t *key, uint8_t *plaintext_string);

int main(int argc, char **argv) {

	FILE *fp = NULL;	
	char *source = NULL;
	char *filepath = NULL;
	long filesize = 0;
	size_t newLen = 0;
	uint8_t *key = NULL; 

	int optch;
	extern int opterr;
	char format[] = "f:k:DE";

	opterr = 1;

	while((optch = getopt(argc, argv, format)) != -1) {
		switch(optch) {
			case 'f':
				filepath = optarg;
				break;
			case 'k':
				key = optarg;
				break;
			case 'D':
			case 'E':
				break;
		}
	}

	if(key == NULL) { 
		fprintf(stderr, "No secret key has been specified. Use the -k option to set the secret key\n");
		return EXIT_FAILURE;
	}

	if(filepath == NULL) {
		fprintf(stderr, "No file has been specified. Use the -f option to set the filepath\n");
		return EXIT_FAILURE;
	}

	if((fp = fopen(filepath, "r")) == NULL) {
		perror("Error opening file");
		return EXIT_FAILURE;
	}

	if (fseek(fp, 0L, SEEK_END) != 0) {
		perror("Uneable to read the file");
		return EXIT_FAILURE;
	}

	if((filesize = ftell(fp)) == -1) {
		perror("Uneable to read the file");
		return EXIT_FAILURE;
	}

	if((source = calloc(filesize + 2, sizeof(char))) == NULL) {
		perror("Uneable allocating memory");
		return EXIT_FAILURE;
	}
	
	if (fseek(fp, 0L, SEEK_SET) != 0) {
		perror("Uneable to read the file");
		return EXIT_FAILURE;
	}

	if((newLen = fread(source, sizeof(char), filesize, fp)) == 0) {
		perror("Error reading file");
		return EXIT_FAILURE;
	} else {
		source[++newLen] = '\0';
	}

	fclose(fp);

	printf("Plaintext message string is: %s\n", source);
	
	uint8_t *ciphertext_string = calloc(16 + strlen(source) + 1, sizeof(uint8_t));
	if(ciphertext_string == NULL) {
		perror("error allocating memory");
		return EXIT_FAILURE;
	}

	tencrypt(source, key, ciphertext_string);
	printf("Encrypted message string is: %s\n", ciphertext_string);

	uint8_t *plaintext_string = calloc(strlen(ciphertext_string) + 1, sizeof(uint8_t));
	if(plaintext_string == NULL) {
		perror("Error allocating memory");
		return EXIT_FAILURE;
	}

	tdecrypt(ciphertext_string, key, source);
	printf("Plaintext message string is: %s\n", source);

	free(source);
	free(ciphertext_string);
	free(plaintext_string);

	return EXIT_SUCCESS;
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
