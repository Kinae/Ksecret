#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "blowfish.h"

int decryption_mode(uint8_t *key, char *filepath);
int encryption_mode(uint8_t *key, char *filepath);

int tencrypt(uint8_t *plaintext_string, uint8_t *key, uint32_t *ciphertext_string);
int tdecrypt(uint32_t *ciphertext_string, int ciphertext_len, uint8_t *key, uint8_t *plaintext_string);

int main(int argc, char **argv) {

	char *filepath = NULL;
	uint8_t *key = NULL;
	int app_mode = 0;
	int ret_value = 0; 

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
				app_mode = 1;
				break;
			case 'E':
				app_mode = 2;
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

	switch(app_mode) {
		case 0 :
			break;
		case 1 :
		 	ret_value = decryption_mode(key, filepath);	
			break;
		case 2 :
			ret_value = encryption_mode(key, filepath);
			break;
	}

	return EXIT_SUCCESS;
}


int decryption_mode(uint8_t *key, char *filepath) {
	
	FILE *fp = NULL;	
	long filesize = 0;
	size_t size = 0;
	uint32_t *source = NULL;
	uint8_t *buffer = NULL;
	uint8_t *plaintext_string = NULL;
	
	if((fp = fopen(filepath, "r")) == NULL) {
		perror("Error opening file");
		return -1;
	}

	if (fseek(fp, 0L, SEEK_END) != 0) {
		perror("Uneable to read the file");
		return -1;
	}

	if((filesize = ftell(fp)) == -1) {
		perror("Uneable to read the file");
		return -1;
	}

	if((source = malloc((filesize + 2) *  sizeof(uint32_t))) == NULL) {
		perror("Uneable allocating memory");
		return -1;
	}
	
	if (fseek(fp, 0L, SEEK_SET) != 0) {
		perror("Uneable to read the file");
		return -1;
	}

	if((size = fread(source, sizeof(uint32_t), filesize, fp)) == 0) {
		perror("Error reading file");
		return -1;
	}

	fclose(fp);

  if((buffer = calloc(size + 1, sizeof(uint32_t))) == NULL) {
		perror("Error allocating memory");
		return -1;
	}

  plaintext_string = buffer;
 	size = tdecrypt(source, size, key, buffer);

	if((fp = fopen(filepath, "w")) == NULL) {
		perror("Error opening file");
		return -1;
	}

	fwrite(plaintext_string, sizeof(uint8_t), size, fp);

	fclose(fp);
	free(source);
	free(buffer);

	return 0;
}

int encryption_mode(uint8_t *key, char *filepath) {

	FILE *fp = NULL;	
	long filesize = 0;
	size_t size = 0;
	uint8_t *source = NULL;
	uint32_t *buffer = NULL;
	uint32_t *ciphertext_string = NULL; 
	
	if((fp = fopen(filepath, "r")) == NULL) {
		perror("Error opening file");
		return -1;
	}

	if (fseek(fp, 0L, SEEK_END) != 0) {
		perror("Uneable to read the file");
		return -1;
	}

	if((filesize = ftell(fp)) == -1) {
		perror("Uneable to read the file");
		return -1;
	}

	if((source = malloc((filesize + 2) *  sizeof(uint8_t))) == NULL) {
		perror("Uneable allocating memory");
		return -1;
	}
	
	if (fseek(fp, 0L, SEEK_SET) != 0) {
		perror("Uneable to read the file");
		return -1;
	}

	if((size = fread(source, sizeof(uint8_t), filesize, fp)) == 0) {
		perror("Error reading file");
		return -1;
	}

	fclose(fp);
	
	if((buffer = calloc(size + 1, sizeof(uint32_t))) == NULL) {
		perror("error allocating memory");
		return -1;
	}

	ciphertext_string = buffer;
	size = tencrypt(source, key, buffer);
	
	if((fp = fopen(filepath, "w")) == NULL) {
		perror("Error opening file");
		return -1;
	}

	fwrite(ciphertext_string, sizeof(uint32_t), size, fp);

	fclose(fp);
	free(source);	
	free(buffer);
	
	return 0;
}

int tencrypt(uint8_t *plaintext_string, uint8_t *key, uint32_t *ciphertext_string) {
	
	blowfish_t ctx;
	int keylen = 0; 
	int plaintext_len = 0;
	int ciphertext_len = 0;
	uint32_t message_left;
	uint32_t message_right;
	int block_len;

	keylen = strlen(key);
	plaintext_len = strlen(plaintext_string);	
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
		*ciphertext_string++ = message_left;
		*ciphertext_string++ = message_right;
		ciphertext_len += 2;
	}

	return ciphertext_len;
}

int tdecrypt(uint32_t *ciphertext_string, int ciphertext_len, uint8_t *key, uint8_t *plaintext_string) {

	blowfish_t ctx;
	int keylen = 0;
	int plaintext_len = 0;
	uint32_t message_left;
	uint32_t message_right;
	int block_len;

	keylen = strlen(key);
	init(&ctx, key, keylen);	

	while(ciphertext_len) {
		message_left = *ciphertext_string++;
		message_right = *ciphertext_string++;
	
		decrypt(&ctx, &message_left, &message_right);
	
		*plaintext_string++ = (uint8_t)(message_left >> 24);
		*plaintext_string++ = (uint8_t)(message_left >> 16);
		*plaintext_string++ = (uint8_t)(message_left >> 8);
		*plaintext_string++ = (uint8_t)(message_left);
		*plaintext_string++ = (uint8_t)(message_right >> 24);
		*plaintext_string++ = (uint8_t)(message_right >> 16);
		*plaintext_string++ = (uint8_t)(message_right >> 8);
		*plaintext_string++ = (uint8_t)(message_right);

		ciphertext_len -= 2;
		plaintext_len += 8;	
	}

	return plaintext_len;
}
