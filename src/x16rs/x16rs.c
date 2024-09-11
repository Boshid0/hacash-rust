#include "x16rs.h"
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#pragma GCC diagnostic ignored "-Wpointer-sign"

#if USE_NEW_ALGO
#include "algo/blake/blake512-hash.c"
#include "algo/cubehash/cubehash_sse2.c"
#include "algo/echo/echo_aesni.c"
#include "algo/fugue/fugue-aesni.c"
#include "algo/groestl/groestl-hash.c"
#include "algo/jh/jh_sse2.c"
#include "algo/luffa/luffa_for_sse2.c"
#include "algo/shavite/shavite-aesni.c"
#include "algo/whirlpool/whirlpool_asm.c"
#include "algo/simd/simd-hash.c"
#include "algo/sha3/sha3_intrs.c"
#include "algo/hamsi/hamsi-intrs.c"
#else
#include "algo/blake/blake.c"
#include "algo/cubehash/cubehash.c"
#include "algo/echo/echo.c"
#include "algo/fugue/fugue.c"
#include "algo/groestl/groestl.c"
#include "algo/jh/jh.c"
#include "algo/luffa/luffa.c"
#include "algo/shavite/shavite.c"
#include "algo/simd/simd.c"
#include "algo/sha3/sha3.c"
#include "algo/hamsi/hamsi.c"
#include "algo/whirlpool/whirlpool.c"
#endif

#include "algo/bmw/bmw.c"
#include "algo/keccak/keccak.c"
#include "algo/sha512/sha2big.c"
#include "algo/shabal/shabal.c"
#include "algo/skein/skein.c"

void sha3_256(const char *input, const int in_size, char *output)
{
#if USE_NEW_ALGO
   sha3_256_intrs(output, input, in_size);
#else
    sha3_256_context ctx;
    sha3_256_init(&ctx);
    sha3_256_update(&ctx, input, in_size);
    sha3_256_close(&ctx, output);
#endif
}

void x16rs_blake512(const char *data, size_t len, char *output)
{
    blake512_context ctx;
    blake512_init(&ctx);
    blake512_update(&ctx, data, len);
    blake512_close(&ctx, (void *)output);
}

void x16rs_bmw512(const char *data, size_t len, char *output)
{
    sph_bmw512_context ctx;
    sph_bmw512_init(&ctx);
    sph_bmw512(&ctx, data, len);
    sph_bmw512_close(&ctx, (void *)output);
}

void x16rs_cubehash512(const char *data, size_t len, char *output)
{
    cubehash512_context ctx;
    cubehash512_init(&ctx);
    cubehash512_update(&ctx, data, len);
    cubehash512_close(&ctx, (void *)output);
}

void x16rs_echo512(const char *data, size_t len, char *output)
{
    echo512_context ctx;
    echo512_init(&ctx);
    echo512_update(&ctx, data, len);
    echo512_close(&ctx, (void *)output);
}

void x16rs_fugue512(const char *data, size_t len, char *output)
{
    fugue512_context ctx;
    fugue512_init(&ctx);
    fugue512_update(&ctx, data, len);
    fugue512_close(&ctx, (void *)output);
}

void x16rs_groestl512(const char *data, size_t len, char *output)
{
#if USE_NEW_ALGO
    groestl_hash(output, data, len);
#else
    groestl512_context ctx;
    groestl512_init(&ctx);
    groestl512_update(&ctx, data, len);
    groestl512_close(&ctx, (void *)output);
#endif
}

void x16rs_hamsi512(const char *data, size_t len, char *output)
{
    hamsi512_context ctx;
    hamsi512_init(&ctx);
    hamsi512_update(&ctx, data, len);
    hamsi512_close(&ctx, (void *)output);
}

void x16rs_jh512(const char *data, size_t len, char *output)
{
    jh512_context ctx;
    jh512_init(&ctx);
    jh512_update(&ctx, data, len);
    jh512_close(&ctx, (void *)output);
}

void x16rs_keccak512(const char *data, size_t len, char *output)
{
    sph_keccak512_context ctc;
    sph_keccak512_init(&ctc);
    sph_keccak512(&ctc, data, len);
    sph_keccak512_close(&ctc, (void *)output);
}

void x16rs_luffa512(const char *data, size_t len, char *output)
{
    luffa512_context ctx;
    luffa512_init(&ctx);
    luffa512_update(&ctx, data, len);
    luffa512_close(&ctx, (void *)output);
}

void x16rs_sha512(const char *data, size_t len, char *output)
{
    sph_sha512_context ctx;
    sph_sha512_init(&ctx);
    sph_sha512_update(&ctx, data, len);
    sph_sha512_close(&ctx, (void *)output);
}

void x16rs_shabal512(const char *data, size_t len, char *output)
{
    sph_shabal512_context ctx;
    sph_shabal512_init(&ctx);
    sph_shabal512(&ctx, data, len);
    sph_shabal512_close(&ctx, (void *)output);
}

void x16rs_shavite512(const char *data, size_t len, char *output)
{
    shavite512_context ctx;
    shavite512_init(&ctx);
    shavite512_update(&ctx, data, len);
    shavite512_close(&ctx, (void *)output);
}

void x16rs_simd512(const char *data, size_t len, char *output)
{
    simd512_context ctx;
    simd512_init(&ctx);
    simd512_update(&ctx, data, len);
    simd512_close(&ctx, (void *)output);
}

void x16rs_skein512(const char *data, size_t len, char *output)
{
    sph_skein512_context ctx;
    sph_skein512_init(&ctx);
    sph_skein512(&ctx, data, len);
    sph_skein512_close(&ctx, (void *)output);
}

void x16rs_whirlpool512(const char *data, size_t len, char *output)
{
    whirlpool_context ctx;
    whirlpool_init(&ctx);
    whirlpool_update(&ctx, data, len);
    whirlpool_close(&ctx, (void *)output);
}

/*------------------------miner api------------------*/
enum Algo
{
    BLAKE = 0,
    BMW,
    GROESTL,
    JH,
    KECCAK,
    SKEIN,
    LUFFA,
    CUBEHASH,
    SHAVITE,
    SIMD,
    ECHO,
    HAMSI,
    FUGUE,
    SHABAL,
    WHIRLPOOL,
    SHA512,
    HASH_FUNC_COUNT
};

// input length must more than 32
static const size_t x16rs_hash_insize = 32;
void c_x16rs_hash(const int loopnum, const char *input_hash, char *output_hash)
{
    // printf("\n"); fflush(stdout);
    uint32_t inputoutput[64 / 4];

    uint32_t *input_hash_ptr32 = (uint32_t *)input_hash;
    inputoutput[0] = input_hash_ptr32[0];
    inputoutput[1] = input_hash_ptr32[1];
    inputoutput[2] = input_hash_ptr32[2];
    inputoutput[3] = input_hash_ptr32[3];
    inputoutput[4] = input_hash_ptr32[4];
    inputoutput[5] = input_hash_ptr32[5];
    inputoutput[6] = input_hash_ptr32[6];
    inputoutput[7] = input_hash_ptr32[7];
    int n;
    for (n = 0; n < loopnum; n++)
    {

        uint8_t algo = inputoutput[7] % 16;
        switch (algo)
        {
        case BLAKE:
            // printf("BLAKE,");
            x16rs_blake512((char *)inputoutput, x16rs_hash_insize, (char *)inputoutput);
            break;
        case BMW:
            // printf("BMW,");
            x16rs_bmw512((char *)inputoutput, x16rs_hash_insize, (char *)inputoutput);
            break;
        case GROESTL:
            // printf("GROESTL,");
            x16rs_groestl512((char *)inputoutput, x16rs_hash_insize, (char *)inputoutput);
            break;
        case SKEIN:
            // printf("SKEIN,");
            x16rs_skein512((char *)inputoutput, x16rs_hash_insize, (char *)inputoutput);
            break;
        case JH:
            // printf("JH,");
            x16rs_jh512((char *)inputoutput, x16rs_hash_insize, (char *)inputoutput);
            break;
        case KECCAK:
            // printf("KECCAK,");
            x16rs_keccak512((char *)inputoutput, x16rs_hash_insize, (char *)inputoutput);
            break;
        case LUFFA:
            // printf("LUFFA,");
            x16rs_luffa512((char *)inputoutput, x16rs_hash_insize, (char *)inputoutput);
            break;
        case CUBEHASH:
            // printf("CUBEHASH,");
            x16rs_cubehash512((char *)inputoutput, x16rs_hash_insize, (char *)inputoutput);
            break;
        case SHAVITE:
            // printf("SHAVITE,");
            x16rs_shavite512((char *)inputoutput, x16rs_hash_insize, (char *)inputoutput);
            break;
        case SIMD:
            // printf("SIMD,");
            x16rs_simd512((char *)inputoutput, x16rs_hash_insize, (char *)inputoutput);
            break;
        case ECHO:
            // printf("ECHO,");
            x16rs_echo512((char *)inputoutput, x16rs_hash_insize, (char *)inputoutput);
            break;
        case HAMSI:
            // printf("HAMSI,");
            x16rs_hamsi512((char *)inputoutput, x16rs_hash_insize, (char *)inputoutput);
            break;
        case FUGUE:
            // printf("FUGUE,");
            x16rs_fugue512((char *)inputoutput, x16rs_hash_insize, (char *)inputoutput);
            break;
        case SHABAL:
            // printf("SHABAL,");
            x16rs_shabal512((char *)inputoutput, x16rs_hash_insize, (char *)inputoutput);
            break;
        case WHIRLPOOL:
            // printf("WHIRLPOOL,");
            x16rs_whirlpool512((char *)inputoutput, x16rs_hash_insize, (char *)inputoutput);
            break;
        case SHA512:
            // printf("SHA512,");
            x16rs_sha512((char *)inputoutput, x16rs_hash_insize, (char *)inputoutput);
            break;
        }

        //printf("algo == (%d) == {%d} \n", algo, inputoutput[7]);
    }
    // printf("output_hash_ptr32");
    uint32_t *output_hash_ptr32 = (uint32_t *)output_hash;
    output_hash_ptr32[0] = inputoutput[0];
    output_hash_ptr32[1] = inputoutput[1];
    output_hash_ptr32[2] = inputoutput[2];
    output_hash_ptr32[3] = inputoutput[3];
    output_hash_ptr32[4] = inputoutput[4];
    output_hash_ptr32[5] = inputoutput[5];
    output_hash_ptr32[6] = inputoutput[6];
    output_hash_ptr32[7] = inputoutput[7];
}

// void main() {  }