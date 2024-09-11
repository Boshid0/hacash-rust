#include <stddef.h>
#include "align.h"

#include "keccakp-1600-avx2.c"

int sha3_256_intrs( unsigned char *out, const unsigned char *in, unsigned long long inlen )
{
#ifndef crypto_hash_BYTES
#define crypto_hash_BYTES 32
#endif
    return KeccakWidth1600_Sponge(1088, 512, in, inlen, 0x06, out, crypto_hash_BYTES);
}

