#include "application.h"

void clean(uint8_t *key);
void print_header(void);

int application_mode(void) {
	uint8_t key[55];
	int c;	

	print_header();
	printf("Enter master key > ");
	if(fflush(stdout) != 0) {
		perror("Error fflush stdout");
		return -1;
	}

	if((fgets(key, sizeof(key), stdin)) == NULL) {
		perror("Error fgets stdin");
		return -1;
	}
	
	clean(key);

	do {

		print_header();
		printf("Enter command > ");
		if(fflush(stdout) != 0) {
			perror("Error fflush stdout");
			return -1;
		}

		if((c = getchar()) == EOF) {
			perror("Error getchar stdin");
			return -1;
		}
	
		clean(key);
	
	} while(c != 'q');	


	return 0;	
}

void clean(uint8_t *key) {
	int c;
	char *p = NULL;
	
	if((p = strchr(key, '\n')) != NULL) {
		*p = 0;
	} else {
		while((c = getchar()) != '\n' && c != EOF);
	}
}

void print_header(void){
	printf("\033[2J\033[0;0f");
	printf(" ______________________________________ \n");
	printf("|    _  __                       _     |\n");
	printf("|   | |/ /___  ___  ___ _ __ ___| |_   |\n");
	printf("|   | ' // __|/ _ \\/ __| '__/ _ \\  _|  |\n");
	printf("|   | . \\\\__ \\  __/ (__| | |  __/ |_   |\n");
	printf("|   |_|\\_\\___/\\___|\\___|_|  \\___|\\__|  |\n");
        printf("|______________________________________|\n");                
	printf("\n\n\n");  	
}
