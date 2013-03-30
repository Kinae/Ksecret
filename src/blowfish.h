#ifndef	BLOWFISH_H_
#define	BLOWFISH_H_

#include	<stdint.h>

#define	MAX_KEY_BYTES	56	//	448

typedef	struct	{
	uint32_t	P[16	+	2];
	uint32_t	S[4][256];
}	blowfish_t;

void	init(blowfish_t	*pblowfish,	uint8_t	*key,	int	keyLength);
void	encrypt(blowfish_t	*pblowfish,	uint32_t	*xl,	uint32_t	*xr);
void	decrypt(blowfish_t	*pblowfish,	uint32_t	*xl,	uint32_t	*xr);

#endif
