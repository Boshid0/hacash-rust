#ifndef SIMD_HASH_2WAY_H__
#define SIMD_HASH_2WAY_H__ 1

#include "simd-utils.h"

#if defined(__SSE2__) || defined (__ARM_NEON)

typedef struct
{
  uint32_t A[32];
  uint8_t buffer[128];
  uint64_t count;
  unsigned int hashbitlen;
  unsigned int blocksize;
  unsigned int n_feistels;
} simd512_context __attribute__((aligned(64)));

// datalen is bytes
int simd512_init( simd512_context *state);
int simd512_update(simd512_context *state, const void *data, int databitlen );
int simd512_close(simd512_context *state, void *hashval );

#endif

#endif
