#include "decryption.h"

int decryption_mode(uint8_t *key, char *filepath) {
	
	FILE *fp = NULL;	
	long filesize = 0;
	size_t size = 0;
	uint32_t *source = NULL;
	uint8_t *buffer = NULL;
	uint8_t *plaintext_string = NULL;
	
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

	if((source = malloc((filesize + 2) *  sizeof(uint32_t))) == NULL) {
		perror("Uneable allocating memory");
		goto free_error;
	}
	
	if (fseek(fp, 0L, SEEK_SET) != 0) {
		perror("Uneable to read the file");
		goto free_error;
	}

	if((size = fread(source, sizeof(uint32_t), filesize, fp)) == 0) {
		perror("Error reading file");
		goto free_error;
	}

	if(fclose(fp) != 0) {
		perror("Error closing file");
		goto free_error;
	}

	if((buffer = calloc(size + 1, sizeof(uint32_t))) == NULL) {
		perror("Error allocating memory");
		goto free_error;
	}

	plaintext_string = buffer;
 	size = tdecrypt(source, size, key, buffer);

	if((fp = fopen(filepath, "w")) == NULL) {
		perror("Error opening file");
		goto free_error;
	}

	if((fwrite(plaintext_string, sizeof(uint8_t), size, fp)) == 0){
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


