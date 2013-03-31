#ifndef	ENCRYPTION_H_
#define	ENCRYPTION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blowfish.h"


int decryption_mode(uint8_t *key, char *filepath);
int tencrypt(uint8_t *plaintext_string, uint8_t *key, uint32_t *ciphertext_string);

#endif
