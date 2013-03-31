#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

#include "encryption.h"
#include "decryption.h"
#include "application.h"

#define APPLICATION_MODE	0
#define ENCRYPTION_MODE 	1	
#define DECRYPTION_MODE	 	2

int main(int argc, char **argv) {

	char *filepath = NULL;
	uint8_t *key = NULL;
	int app_mode = APPLICATION_MODE;
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
				app_mode = ENCRYPTION_MODE;
				break;
			case 'E':
				app_mode = DECRYPTION_MODE;
				break;
		}
	}

	if(key == NULL && app_mode != APPLICATION_MODE) { 
		fprintf(stderr, "No secret key has been specified. Use the -k option to set the secret key\n");
		return EXIT_FAILURE;
	}

	if(filepath == NULL && app_mode != APPLICATION_MODE) {
		fprintf(stderr, "No file has been specified. Use the -f option to set the filepath\n");
		return EXIT_FAILURE;
	}

	switch(app_mode) {
		case APPLICATION_MODE :
			ret_value = application_mode();
			if(ret_value == -1) {
				fprintf(stderr, "Error while running appliction");	
				return EXIT_FAILURE;
			}
			break;
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
