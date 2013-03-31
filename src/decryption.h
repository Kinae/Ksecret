#ifndef	DECRYPTION_H_
#define	DECRYPTION_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blowfish.h"

int decryption_mode(uint8_t *key, char *filepath);
int tdecrypt(uint32_t *ciphertext_string, int ciphertext_len, uint8_t *key, uint8_t *plaintext_string);

#endif
