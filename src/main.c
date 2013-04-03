#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "encryption.h"
#include "decryption.h"

#define ENCRYPTION_MODE 	1
#define DECRYPTION_MODE	 	2

void print_headers(void);

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
				app_mode = DECRYPTION_MODE;
				break;
			case 'E':
				app_mode = ENCRYPTION_MODE;
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

	if(app_mode == 0) {
		fprintf(stderr, "No mode has been specified. Use -D (decryption) or -E (encryption)\n");
		return EXIT_FAILURE;
	}

	print_headers();

	switch(app_mode) {
		case DECRYPTION_MODE :
		 	ret_value = decryption_mode(key, filepath);	
			if(ret_value == -1) {
				fprintf(stderr, "Error while decrypting\n");	
				return EXIT_FAILURE;
			}
			break;
		case ENCRYPTION_MODE :
			ret_value = encryption_mode(key, filepath);
			if(ret_value == -1) {
				fprintf(stderr, "Error while encrypting\n");	
				return EXIT_FAILURE;
			}
			break;
	}

	return EXIT_SUCCESS;
}

void print_headers(void) {
	printf("Ksecret : Tools to encrypt and decrypt a file using blowfish algorithm\n");
	printf("Author  : Anthony Defraine\n");
	printf("Email 	: anthony.defraine@gmail.com\n");
}
