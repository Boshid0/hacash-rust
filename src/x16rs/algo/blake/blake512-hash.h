#ifndef BLAKE512_HASH__
#define BLAKE512_HASH__ 1

#include <stddef.h>

/////////////////////////
//
//  Blake-512 1 way SSE2, AVX2, NEON

typedef struct
{
   unsigned char buf[128];    /* first field, for alignment */
   uint64_t H[8];
   uint64_t T0, T1;
   size_t ptr;
} blake512_context __attribute__ ((aligned (32)));

void  blake512_transform( uint64_t *H, const uint64_t *buf,
                          const uint64_t T0, const uint64_t T1 );
void blake512_init( blake512_context *sc );
void blake512_update( blake512_context *sc, const void *data, size_t len );
void blake512_close( blake512_context *sc, void *dst );
void blake512_full( blake512_context *sc, void *dst, const void *data,
                    size_t len );

#endif  // BLAKE512_HASH_H__
