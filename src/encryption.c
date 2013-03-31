#include "encryption.h"

int encryption_mode(uint8_t *key, char *filepath) {

	FILE *fp = NULL;	
	long filesize = 0;
	size_t size = 0;
	uint8_t *source = NULL;
	uint32_t *buffer = NULL;
	uint32_t *ciphertext_string = NULL; 
	
	if((fp = fopen(filepath, "r")) == NULL) {
		perror("Error opening file");
		goto free_error;
	}

	if (fseek(fp, 0L, SEEK_END) != 0) {
		perror("Uneable to read the file");
		goto free_error;
	}

	if((filesize = ftell(fp)) == -1) {
		perror("Uneable to read the file");
		goto free_error;
	}

	if((source = malloc((filesize + 2) *  sizeof(uint8_t))) == NULL) {
		perror("Uneable allocating memory");
		goto free_error;
	}
	
	if (fseek(fp, 0L, SEEK_SET) != 0) {
		perror("Uneable to read the file");
		goto free_error;
	}

	if((size = fread(source, sizeof(uint8_t), filesize, fp)) == 0) {
		perror("Error reading file");
		goto free_error;
	}

	
	if((buffer = calloc(size + 1, sizeof(uint32_t))) == NULL) {
		perror("error allocating memory");
		goto free_error;
	}

	if(fclose(fp) != 0) {
		perror("Error closing file");
		goto free_error;
	}

	ciphertext_string = buffer;
	size = tencrypt(source, key, buffer);
	
	if((fp = fopen(filepath, "w")) == NULL) {
		perror("Error opening file");
		goto free_error;
	}

	if((fwrite(ciphertext_string, sizeof(uint32_t), size, fp)) == 0) {
		perror("Error writing file");
		goto free_error;
	}

	if(fclose(fp) != 0) {
		perror("Error closing file");
		goto free_error;
	}

	if(source) free(source);	
	if(buffer) free(buffer);
	
	return 0;
	
free_error:

	if(fp != NULL && fclose(fp) != 0) {
		perror("Error closing file");
	}

	if(source) free(source);	
	if(buffer) free(buffer);
	
	return -1;
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
