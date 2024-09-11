/*
 * file        : hash_api.h
 * version     : 1.0.208
 * date        : 14.12.2010
 * 
 * Fugue vperm implementation Hash API
 *
 * Cagdas Calik
 * ccalik@metu.edu.tr
 * Institute of Applied Mathematics, Middle East Technical University, Turkey.
 *
 */

#ifndef FUGUE_HASH_API_H
#define FUGUE_HASH_API_H

#ifdef __cplusplus
extern "C"{
#endif

#include "simd-utils.h"

typedef struct
{
	v128_t			state[12];
	unsigned int	base;
	unsigned int	uHashSize;
	unsigned int	uBlockLength;
	unsigned int	uBufferBytes;
	uint64_t 		processed_bits;
	uint8_t  		buffer[4];

} hashState_fugue __attribute__ ((aligned (64)));

#if ( defined(__SSE4_1__) && defined(__AES__) ) || ( defined(__ARM_NEON) && defined(__ARM_FEATURE_AES) )


// These functions are deprecated, use the lower case macro aliases that use
// the standard interface. This will be cleaned up at a later date.
int fugue512_Init( hashState_fugue *state);

int fugue512_Update( hashState_fugue *state, const void *data,
                     uint64_t databitlen );

int fugue512_Final( hashState_fugue *state, void *hashval );

#define fugue512_context hashState_fugue
#define fugue512_init    fugue512_Init
#define fugue512_update  fugue512_Update
#define fugue512_close   fugue512_Final

#endif // AES

#ifdef __cplusplus
}
#endif

#endif // HASH_API_H

