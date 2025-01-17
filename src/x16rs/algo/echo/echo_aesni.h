/*
 * file        : hash_api.h
 * version     : 1.0.208
 * date        : 14.12.2010
 * 
 * ECHO vperm implementation Hash API
 *
 * Cagdas Calik
 * ccalik@metu.edu.tr
 * Institute of Applied Mathematics, Middle East Technical University, Turkey.
 *
 */


#ifndef HASH_API_H
#define HASH_API_H

#ifdef __AES__
#define HASH_IMPL_STR	"ECHO-aesni"
#else
#define HASH_IMPL_STR	"ECHO-vperm"
#endif

#include "simd-utils.h"

typedef unsigned char BitSequence;
typedef unsigned long long DataLength;
typedef enum { SUCCESS = 0, FAIL = 1, BAD_HASHBITLEN = 2} HashReturn;

typedef struct
{
	v128_t			state[4][4];
        BitSequence             buffer[192];
	v128_t			k;
	v128_t			hashsize;
	v128_t			const1536;

	unsigned int	uRounds;
	unsigned int	uHashSize;
	unsigned int	uBlockLength;
	unsigned int	uBufferBytes;
	DataLength		processed_bits;

} hashState_echo __attribute__ ((aligned (64)));

HashReturn init_echo(hashState_echo *state);

HashReturn reinit_echo(hashState_echo *state);

HashReturn update_echo(hashState_echo *state, const void *data, uint32_t databitlen);

HashReturn final_echo(hashState_echo *state, void *hashval);

HashReturn hash_echo(int hashbitlen, const void *data, uint32_t databitlen, void *hashval);

HashReturn update_final_echo( hashState_echo *state, void *hashval,
                              const void *data, uint32_t databitlen );
HashReturn echo_full( hashState_echo *state, void *hashval,
            int nHashSize, const void *data, uint32_t databitlen );

#define echo512_context hashState_echo
#define echo512_init init_echo
#define echo512_update update_echo
#define echo512_close final_echo

#endif // HASH_API_H

