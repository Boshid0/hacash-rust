#include <stddef.h>
#include <string.h>
#include <limits.h>
#include "simd-utils.h"
#include "blake512-hash.h"

// Blake-512 common

static const uint64_t BLAKE512_IV[8] __attribute__ ((aligned (32))) =
{
  0x6A09E667F3BCC908, 0xBB67AE8584CAA73B,
  0x3C6EF372FE94F82B, 0xA54FF53A5F1D36F1,
  0x510E527FADE682D1, 0x9B05688C2B3E6C1F,
  0x1F83D9ABFB41BD6B, 0x5BE0CD19137E2179
};

/*
static const uint64_t salt_zero_big[4] = { 0, 0, 0, 0 };

static const unsigned sigma[16][16] = {
	{  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 },
	{ 14, 10,  4,  8,  9, 15, 13,  6,  1, 12,  0,  2, 11,  7,  5,  3 },
	{ 11,  8, 12,  0,  5,  2, 15, 13, 10, 14,  3,  6,  7,  1,  9,  4 },
	{  7,  9,  3,  1, 13, 12, 11, 14,  2,  6,  5, 10,  4,  0, 15,  8 },
	{  9,  0,  5,  7,  2,  4, 10, 15, 14,  1, 11, 12,  6,  8,  3, 13 },
	{  2, 12,  6, 10,  0, 11,  8,  3,  4, 13,  7,  5, 15, 14,  1,  9 },
	{ 12,  5,  1, 15, 14, 13,  4, 10,  0,  7,  6,  3,  9,  2,  8, 11 },
	{ 13, 11,  7, 14, 12,  1,  3,  9,  5,  0, 15,  4,  8,  6,  2, 10 },
	{  6, 15, 14,  9, 11,  3,  0,  8, 12,  2, 13,  7,  1,  4, 10,  5 },
	{ 10,  2,  8,  4,  7,  6,  1,  5, 15, 11,  9, 14,  3, 12, 13,  0 },
	{  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 },
	{ 14, 10,  4,  8,  9, 15, 13,  6,  1, 12,  0,  2, 11,  7,  5,  3 },
	{ 11,  8, 12,  0,  5,  2, 15, 13, 10, 14,  3,  6,  7,  1,  9,  4 },
	{  7,  9,  3,  1, 13, 12, 11, 14,  2,  6,  5, 10,  4,  0, 15,  8 },
	{  9,  0,  5,  7,  2,  4, 10, 15, 14,  1, 11, 12,  6,  8,  3, 13 },
	{  2, 12,  6, 10,  0, 11,  8,  3,  4, 13,  7,  5, 15, 14,  1,  9 }
};
*/
/*
static const uint64_t CB[16] __attribute__ ((aligned (32))) =
{
   0x243F6A8885A308D3, 0x13198A2E03707344,
   0xA4093822299F31D0, 0x082EFA98EC4E6C89,
   0x452821E638D01377, 0xBE5466CF34E90C6C,
   0xC0AC29B7C97C50DD, 0x3F84D5B5B5470917,
   0x9216D5D98979FB1B, 0xD1310BA698DFB5AC,
   0x2FFD72DBD01ADFB7, 0xB8E1AFED6A267E96,
   0xBA7C9045F12C7F99, 0x24A19947B3916CF7,
   0x0801F2E2858EFC16, 0x636920D871574E69
}
*/

#define Z00   0
#define Z01   1
#define Z02   2
#define Z03   3
#define Z04   4
#define Z05   5
#define Z06   6
#define Z07   7
#define Z08   8
#define Z09   9
#define Z0A   A
#define Z0B   B
#define Z0C   C
#define Z0D   D
#define Z0E   E
#define Z0F   F

#define Z10   E
#define Z11   A
#define Z12   4
#define Z13   8
#define Z14   9
#define Z15   F
#define Z16   D
#define Z17   6
#define Z18   1
#define Z19   C
#define Z1A   0
#define Z1B   2
#define Z1C   B
#define Z1D   7
#define Z1E   5
#define Z1F   3

#define Z20   B
#define Z21   8
#define Z22   C
#define Z23   0
#define Z24   5
#define Z25   2
#define Z26   F
#define Z27   D
#define Z28   A
#define Z29   E
#define Z2A   3
#define Z2B   6
#define Z2C   7
#define Z2D   1
#define Z2E   9
#define Z2F   4

#define Z30   7
#define Z31   9
#define Z32   3
#define Z33   1
#define Z34   D
#define Z35   C
#define Z36   B
#define Z37   E
#define Z38   2
#define Z39   6
#define Z3A   5
#define Z3B   A
#define Z3C   4
#define Z3D   0
#define Z3E   F
#define Z3F   8

#define Z40   9
#define Z41   0
#define Z42   5
#define Z43   7
#define Z44   2
#define Z45   4
#define Z46   A
#define Z47   F
#define Z48   E
#define Z49   1
#define Z4A   B
#define Z4B   C
#define Z4C   6
#define Z4D   8
#define Z4E   3
#define Z4F   D

#define Z50   2
#define Z51   C
#define Z52   6
#define Z53   A
#define Z54   0
#define Z55   B
#define Z56   8
#define Z57   3
#define Z58   4
#define Z59   D
#define Z5A   7
#define Z5B   5
#define Z5C   F
#define Z5D   E
#define Z5E   1
#define Z5F   9

#define Z60   C
#define Z61   5
#define Z62   1
#define Z63   F
#define Z64   E
#define Z65   D
#define Z66   4
#define Z67   A
#define Z68   0
#define Z69   7
#define Z6A   6
#define Z6B   3
#define Z6C   9
#define Z6D   2
#define Z6E   8
#define Z6F   B

#define Z70   D
#define Z71   B
#define Z72   7
#define Z73   E
#define Z74   C
#define Z75   1
#define Z76   3
#define Z77   9
#define Z78   5
#define Z79   0
#define Z7A   F
#define Z7B   4
#define Z7C   8
#define Z7D   6
#define Z7E   2
#define Z7F   A

#define Z80   6
#define Z81   F
#define Z82   E
#define Z83   9
#define Z84   B
#define Z85   3
#define Z86   0
#define Z87   8
#define Z88   C
#define Z89   2
#define Z8A   D
#define Z8B   7
#define Z8C   1
#define Z8D   4
#define Z8E   A
#define Z8F   5

#define Z90   A
#define Z91   2
#define Z92   8
#define Z93   4
#define Z94   7
#define Z95   6
#define Z96   1
#define Z97   5
#define Z98   F
#define Z99   B
#define Z9A   9
#define Z9B   E
#define Z9C   3
#define Z9D   C
#define Z9E   D
#define Z9F   0

#define Mx(r, i)    Mx_(Z ## r ## i)
#define Mx_(n)      Mx__(n)
#define Mx__(n)     M ## n

#define CBx(r, i)   CBx_(Z ## r ## i)
#define CBx_(n)     CBx__(n)
#define CBx__(n)    CB ## n

#define CB0   0x243F6A8885A308D3
#define CB1   0x13198A2E03707344
#define CB2   0xA4093822299F31D0
#define CB3   0x082EFA98EC4E6C89
#define CB4   0x452821E638D01377
#define CB5   0xBE5466CF34E90C6C
#define CB6   0xC0AC29B7C97C50DD
#define CB7   0x3F84D5B5B5470917
#define CB8   0x9216D5D98979FB1B
#define CB9   0xD1310BA698DFB5AC
#define CBA   0x2FFD72DBD01ADFB7
#define CBB   0xB8E1AFED6A267E96
#define CBC   0xBA7C9045F12C7F99
#define CBD   0x24A19947B3916CF7
#define CBE   0x0801F2E2858EFC16
#define CBF   0x636920D871574E69


///////////////////////////////////////////////////////
//
//          Blake-512 1 way SSE2, AVX2, NEON

#if defined(__AVX2__)

#define BLAKE512_ROUND( r ) \
{ \
   V0 = _mm256_add_epi64( V0, _mm256_add_epi64( V1, \
                           _mm256_set_epi64x( CBx( r, 7 ) ^ Mx( r, 6 ), \
                                              CBx( r, 5 ) ^ Mx( r, 4 ), \
                                              CBx( r, 3 ) ^ Mx( r, 2 ), \
                                              CBx( r, 1 ) ^ Mx( r, 0 ) ) ) ); \
   V3 = mm256_ror_64( _mm256_xor_si256( V3, V0 ), 32 ); \
   V2 = _mm256_add_epi64( V2, V3 ); \
   V1 = mm256_ror_64( _mm256_xor_si256( V1, V2 ), 25 ); \
   V0 = _mm256_add_epi64( V0, _mm256_add_epi64( V1, \
                           _mm256_set_epi64x( CBx( r, 6 ) ^ Mx( r, 7 ), \
                                              CBx( r, 4 ) ^ Mx( r, 5 ), \
                                              CBx( r, 2 ) ^ Mx( r, 3 ), \
                                              CBx( r, 0 ) ^ Mx( r, 1 ) ) ) ); \
   V3 = mm256_ror_64( _mm256_xor_si256( V3, V0 ), 16 ); \
   V2 = _mm256_add_epi64( V2, V3 ); \
   V1 = mm256_ror_64( _mm256_xor_si256( V1, V2 ), 11 ); \
   V0 = mm256_shufll_64( V0 ); \
   V3 = mm256_swap_128( V3 ); \
   V2 = mm256_shuflr_64( V2 ); \
   V0 = _mm256_add_epi64( V0, _mm256_add_epi64( V1, \
                           _mm256_set_epi64x( CBx( r, D ) ^ Mx( r, C ), \
                                              CBx( r, B ) ^ Mx( r, A ), \
                                              CBx( r, 9 ) ^ Mx( r, 8 ), \
                                              CBx( r, F ) ^ Mx( r, E ) ) ) ); \
   V3 = mm256_ror_64( _mm256_xor_si256( V3, V0 ), 32 ); \
   V2 = _mm256_add_epi64( V2, V3 ); \
   V1 = mm256_ror_64( _mm256_xor_si256( V1, V2 ), 25 ); \
   V0 = _mm256_add_epi64( V0, _mm256_add_epi64( V1, \
                           _mm256_set_epi64x( CBx( r, C ) ^ Mx( r, D ), \
                                              CBx( r, A ) ^ Mx( r, B ), \
                                              CBx( r, 8 ) ^ Mx( r, 9 ), \
                                              CBx( r, E ) ^ Mx( r, F ) ) ) ); \
   V3 = mm256_ror_64( _mm256_xor_si256( V3, V0 ), 16 ); \
   V2 = _mm256_add_epi64( V2, V3 ); \
   V1 = mm256_ror_64( _mm256_xor_si256( V1, V2 ), 11 ); \
   V0 = mm256_shuflr_64( V0 ); \
   V3 = mm256_swap_128( V3 ); \
   V2 = mm256_shufll_64( V2 ); \
}

void blake512_transform( uint64_t *H, const uint64_t *buf, const uint64_t T0,
                         const uint64_t T1 )
{
   __m256i V0, V1, V2, V3;
   uint64_t M0, M1, M2, M3, M4, M5, M6, M7, M8, M9, MA, MB, MC, MD, ME, MF;
   
   V0 = casti_m256i( H, 0 );
   V1 = casti_m256i( H, 1 );
   V2 = _mm256_set_epi64x( CB3, CB2, CB1, CB0 );
   V3 = _mm256_set_epi64x( CB7 ^ T1, CB6 ^ T1, CB5 ^ T0, CB4 ^ T0 );

   M0 = bswap_64( buf[ 0] );
   M1 = bswap_64( buf[ 1] );
   M2 = bswap_64( buf[ 2] );
   M3 = bswap_64( buf[ 3] );
   M4 = bswap_64( buf[ 4] );
   M5 = bswap_64( buf[ 5] );
   M6 = bswap_64( buf[ 6] );
   M7 = bswap_64( buf[ 7] );
   M8 = bswap_64( buf[ 8] );
   M9 = bswap_64( buf[ 9] );
   MA = bswap_64( buf[10] );
   MB = bswap_64( buf[11] );
   MC = bswap_64( buf[12] );
   MD = bswap_64( buf[13] );
   ME = bswap_64( buf[14] );
   MF = bswap_64( buf[15] );
   
   BLAKE512_ROUND( 0 );
   BLAKE512_ROUND( 1 );
   BLAKE512_ROUND( 2 );
   BLAKE512_ROUND( 3 );
   BLAKE512_ROUND( 4 );
   BLAKE512_ROUND( 5 );
   BLAKE512_ROUND( 6 );
   BLAKE512_ROUND( 7 );
   BLAKE512_ROUND( 8 );
   BLAKE512_ROUND( 9 );
   BLAKE512_ROUND( 0 );
   BLAKE512_ROUND( 1 );
   BLAKE512_ROUND( 2 );
   BLAKE512_ROUND( 3 );
   BLAKE512_ROUND( 4 );
   BLAKE512_ROUND( 5 );
   
   casti_m256i( H, 0 ) = mm256_xor3( casti_m256i( H, 0 ), V0, V2 );
   casti_m256i( H, 1 ) = mm256_xor3( casti_m256i( H, 1 ), V1, V3 );
}

#else

#define BLAKE512_G( r,  Va, Vb, Vc, Vd, Sa, Sb, Sc, Sd ) \
{ \
   Va = v128_add64( Va, v128_add64( Vb, \
                            v128_set64( CBx( r, Sd ) ^ Mx( r, Sc ), \
                                        CBx( r, Sb ) ^ Mx( r, Sa ) ) ) ); \
   Vd = v128_ror64( v128_xor( Vd, Va ), 32 ); \
   Vc = v128_add64( Vc, Vd ); \
   Vb = v128_ror64( v128_xor( Vb, Vc ), 25 ); \
\
   Va = v128_add64( Va, v128_add64( Vb, \
                            v128_set64( CBx( r, Sc ) ^ Mx( r, Sd ), \
                                        CBx( r, Sa ) ^ Mx( r, Sb ) ) ) ); \
   Vd = v128_ror64( v128_xor( Vd, Va ), 16 ); \
   Vc = v128_add64( Vc, Vd ); \
   Vb = v128_ror64( v128_xor( Vb, Vc ), 11 ); \
}

#define BLAKE512_ROUND( R ) \
{ \
   v128_t V32, V23, V67, V76; \
   BLAKE512_G( R, V[0], V[2], V[4], V[6], 0, 1, 2, 3 ); \
   BLAKE512_G( R, V[1], V[3], V[5], V[7], 4, 5, 6, 7 ); \
   V32 = v128_alignr64( V[3], V[2], 1 ); \
   V23 = v128_alignr64( V[2], V[3], 1 ); \
   V67 = v128_alignr64( V[6], V[7], 1 ); \
   V76 = v128_alignr64( V[7], V[6], 1 ); \
   BLAKE512_G( R, V[0], V32, V[5], V67, 8, 9, A, B ); \
   BLAKE512_G( R, V[1], V23, V[4], V76, C, D, E, F ); \
   V[2] = v128_alignr64( V32, V23, 1 ); \
   V[3] = v128_alignr64( V23, V32, 1 ); \
   V[6] = v128_alignr64( V76, V67, 1 ); \
   V[7] = v128_alignr64( V67, V76, 1 ); \
}

void blake512_transform( uint64_t *H, const uint64_t *buf,
                         const uint64_t T0, const uint64_t T1 )
{
   v128_t V[8];
   uint64_t M0, M1, M2, M3, M4, M5, M6, M7, M8, M9, MA, MB, MC, MD, ME, MF;

   V[0] = casti_v128u64( H, 0 );
   V[1] = casti_v128u64( H, 1 );
   V[2] = casti_v128u64( H, 2 );
   V[3] = casti_v128u64( H, 3 );
   V[4] = v128_set64( CB1, CB0 );
   V[5] = v128_set64( CB3, CB2 );
   V[6] = v128_set64( CB5 ^ T0, CB4 ^ T0 );
   V[7] = v128_set64( CB7 ^ T1, CB6 ^ T1 );

   M0 = bswap_64( buf[ 0] );
   M1 = bswap_64( buf[ 1] );
   M2 = bswap_64( buf[ 2] );
   M3 = bswap_64( buf[ 3] );
   M4 = bswap_64( buf[ 4] );
   M5 = bswap_64( buf[ 5] );
   M6 = bswap_64( buf[ 6] );
   M7 = bswap_64( buf[ 7] );
   M8 = bswap_64( buf[ 8] );
   M9 = bswap_64( buf[ 9] );
   MA = bswap_64( buf[10] );
   MB = bswap_64( buf[11] );
   MC = bswap_64( buf[12] );
   MD = bswap_64( buf[13] );
   ME = bswap_64( buf[14] );
   MF = bswap_64( buf[15] );
   
   BLAKE512_ROUND( 0 );
   BLAKE512_ROUND( 1 );
   BLAKE512_ROUND( 2 );
   BLAKE512_ROUND( 3 );
   BLAKE512_ROUND( 4 );
   BLAKE512_ROUND( 5 );
   BLAKE512_ROUND( 6 );
   BLAKE512_ROUND( 7 );
   BLAKE512_ROUND( 8 );
   BLAKE512_ROUND( 9 );
   BLAKE512_ROUND( 0 );
   BLAKE512_ROUND( 1 );
   BLAKE512_ROUND( 2 );
   BLAKE512_ROUND( 3 );
   BLAKE512_ROUND( 4 );
   BLAKE512_ROUND( 5 );

   casti_v128u64( H, 0 ) = v128_xor3( casti_v128u64( H, 0 ), V[0], V[4] );
   casti_v128u64( H, 1 ) = v128_xor3( casti_v128u64( H, 1 ), V[1], V[5] );
   casti_v128u64( H, 2 ) = v128_xor3( casti_v128u64( H, 2 ), V[2], V[6] );
   casti_v128u64( H, 3 ) = v128_xor3( casti_v128u64( H, 3 ), V[3], V[7] );
}

#endif

void blake512_init( blake512_context *sc )
{
   memcpy( sc->H, BLAKE512_IV, 8 * sizeof(uint64_t) );
   sc->T0 = sc->T1 = 0;
   sc->ptr = 0;
}

void blake512_update(blake512_context *sc, const void *data, size_t len)
{
   if ( len < (sizeof sc->buf) - sc->ptr )
   {
      memcpy( sc->buf + sc->ptr, data, len );
      sc->ptr += len;
      return;
   }

   while ( len > 0 )
   {
      size_t clen;

      clen = (sizeof sc->buf) - sc->ptr;
      if ( clen > len )  clen = len;
      memcpy( sc->buf + sc->ptr, data, clen );
      sc->ptr += clen;
      data = (const unsigned char *)data + clen;
      len -= clen;
      if ( sc->ptr == sizeof sc->buf )
      {
         if ( ( sc->T0 = sc->T0 + 1024 ) < 1024 )
            sc->T1 += 1;

         blake512_transform( sc->H, (uint64_t*)sc->buf, sc->T0, sc->T1 );
         sc->ptr = 0;
      }
   }
}

void blake512_close( blake512_context *sc, void *dst )
{
   unsigned char buf[128] __attribute__((aligned(32)));
   size_t ptr, k;
   unsigned bit_len;
   uint64_t th, tl;

   ptr = sc->ptr;
   memcpy( buf, sc->buf, ptr );
   bit_len = ((unsigned)ptr << 3);
   buf[ptr] = 0x80;
   tl = sc->T0 + bit_len;
   th = sc->T1;

   if ( ptr == 0 )
   {
      sc->T0 = 0xFFFFFFFFFFFFFC00;
      sc->T1 = 0xFFFFFFFFFFFFFFFF;
   }
   else if ( sc->T0 == 0 )
   {
      sc->T0 = 0xFFFFFFFFFFFFFC00 + bit_len;
      sc->T1 -= 1;
   }
   else
      sc->T0 -= 1024 - bit_len;

   if ( bit_len <= 894 )
   {
      memset( buf + ptr + 1, 0, 111 - ptr );
      buf[111] |= 1;
      *((uint64_t*)(buf + 112)) = bswap_64( th );
      *((uint64_t*)(buf + 120)) = bswap_64( tl );
      blake512_update( sc, buf + ptr, 128 - ptr );
   }
   else
   {
      memset( buf + ptr + 1, 0, 127 - ptr );
      blake512_update( sc, buf + ptr, 128 - ptr );
      sc->T0 = 0xFFFFFFFFFFFFFC00;
      sc->T1 = 0xFFFFFFFFFFFFFFFF;
      memset( buf, 0, 112 );
      buf[111] = 1;
      *(uint64_t*)(buf + 112) = bswap_64( th );
      *(uint64_t*)(buf + 120) = bswap_64( tl );
      blake512_update( sc, buf, 128 );
   }
   
   for ( k = 0; k < 8; k ++ )
      ((uint64_t*)dst)[k] = bswap_64( sc->H[k] );
}

void blake512_full( blake512_context *sc, void *dst, const void *data,
                    size_t len )
{
   blake512_init( sc );
   blake512_update( sc, data, len );
   blake512_close( sc, dst );
}

#define READ_STATE64( state ) \
   H0 = (state)->H[0]; \
   H1 = (state)->H[1]; \
   H2 = (state)->H[2]; \
   H3 = (state)->H[3]; \
   H4 = (state)->H[4]; \
   H5 = (state)->H[5]; \
   H6 = (state)->H[6]; \
   H7 = (state)->H[7]; \
   T0 = (state)->T0; \
   T1 = (state)->T1;


#define WRITE_STATE64( state ) \
   (state)->H[0] = H0; \
   (state)->H[1] = H1; \
   (state)->H[2] = H2; \
   (state)->H[3] = H3; \
   (state)->H[4] = H4; \
   (state)->H[5] = H5; \
   (state)->H[6] = H6; \
   (state)->H[7] = H7; \
   (state)->T0 = T0; \
   (state)->T1 = T1;

