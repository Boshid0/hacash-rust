#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "simd-hash.h"

uint32_t SIMD_IV_512[] __attribute__((aligned(64))) =
{
   0x0ba16b95, 0x72f999ad, 0x9fecc2ae, 0xba3264fc,
   0x5e894929, 0x8e9f30e5, 0x2f1daa37, 0xf0f2c558,
   0xac506643, 0xa90635a5, 0xe25b878b, 0xaab7878f,
   0x88817f7a, 0x0a02892b, 0x559a7550, 0x598f657e,
   0x7eef60a1, 0x6b70e3e8, 0x9c1714d1, 0xb958e2a8,
   0xab02675e, 0xed1c014f, 0xcd8d65bb, 0xfdb7a257,
   0x09254899, 0xd699c7bc, 0x9019b6dc, 0x2b9022e4,
   0x8fa14956, 0x21bf9bd3, 0xb94d0943, 0x6ffddc22
};

#define CAT(x, y) x##y
#define XCAT(x,y) CAT(x,y)

#define SUM7_00 0
#define SUM7_01 1
#define SUM7_02 2
#define SUM7_03 3
#define SUM7_04 4
#define SUM7_05 5
#define SUM7_06 6

#define SUM7_10 1
#define SUM7_11 2
#define SUM7_12 3
#define SUM7_13 4
#define SUM7_14 5
#define SUM7_15 6
#define SUM7_16 0

#define SUM7_20 2
#define SUM7_21 3
#define SUM7_22 4
#define SUM7_23 5
#define SUM7_24 6
#define SUM7_25 0
#define SUM7_26 1

#define SUM7_30 3
#define SUM7_31 4
#define SUM7_32 5
#define SUM7_33 6
#define SUM7_34 0
#define SUM7_35 1
#define SUM7_36 2

#define SUM7_40 4
#define SUM7_41 5
#define SUM7_42 6
#define SUM7_43 0
#define SUM7_44 1
#define SUM7_45 2
#define SUM7_46 3

#define SUM7_50 5
#define SUM7_51 6
#define SUM7_52 0
#define SUM7_53 1
#define SUM7_54 2
#define SUM7_55 3
#define SUM7_56 4

#define SUM7_60 6
#define SUM7_61 0
#define SUM7_62 1
#define SUM7_63 2
#define SUM7_64 3
#define SUM7_65 4
#define SUM7_66 5

#define PERM( p, z, d, a, shufxor ) \
   XCAT( PERM_, XCAT( SUM7_ ## z, p ) )( d, a, shufxor )

#define PERM_0( d, a, shufxor ) /* XOR 1 */ \
do { \
    d##l = shufxor( a##l, 1 ); \
    d##h = shufxor( a##h, 1 ); \
 } while(0)

#define PERM_1(d,a,shufxor) /* XOR 6 */ \
do { \
    d##l = shufxor( a##h, 2 ); \
    d##h = shufxor( a##l, 2 ); \
} while(0)

#define PERM_2(d,a,shufxor) /* XOR 2 */ \
do { \
    d##l = shufxor( a##l, 2 ); \
    d##h = shufxor( a##h, 2 ); \
} while(0)

#define PERM_3(d,a,shufxor) /* XOR 3 */ \
do { \
    d##l = shufxor( a##l, 3 ); \
    d##h = shufxor( a##h, 3 ); \
} while(0)

#define PERM_4(d,a,shufxor) /* XOR 5 */ \
do { \
    d##l = shufxor( a##h, 1 ); \
    d##h = shufxor( a##l, 1 ); \
} while(0)

#define PERM_5(d,a,shufxor) /* XOR 7 */ \
do { \
    d##l = shufxor( a##h, 3 ); \
    d##h = shufxor( a##l, 3 ); \
} while(0)

#define PERM_6(d,a,shufxor) /* XOR 4 */ \
do { \
    d##l = a##h; \
    d##h = a##l; \
} while(0)

#if defined(__SSE2__) || defined(__ARM_NEON)

union _m128_v16
{
  uint16_t u16[8];
  v128u16_t v128;
} __attribute__((aligned(64)));
typedef union _m128_v16 m128_v16;

static const m128_v16 FFT64_twiddle[] __attribute__((aligned(64))) =
{
   {{ 1,    2,    4,    8,   16,   32,   64,  128 }},
   {{ 1,   60,    2,  120,    4,  -17,    8,  -34 }},
   {{ 1,  120,    8,  -68,   64,  -30,   -2,   17 }},
   {{ 1,   46,   60,  -67,    2,   92,  120,  123 }},
   {{ 1,   92,  -17,  -22,   32,  117,  -30,   67 }},
   {{ 1,  -67,  120,  -73,    8,  -22,  -68,  -70 }},
   {{ 1,  123,  -34,  -70,  128,   67,   17,   35 }}
};

static const m128_v16 FFT128_twiddle[] __attribute__((aligned(64))) =
{
   {{   1, -118,   46,  -31,   60,  116,  -67,  -61 }},
   {{   2,   21,   92,  -62,  120,  -25,  123, -122 }},
   {{   4,   42,  -73, -124,  -17,  -50,  -11,   13 }},
   {{   8,   84,  111,    9,  -34, -100,  -22,   26 }},
   {{  16,  -89,  -35,   18,  -68,   57,  -44,   52 }},
   {{  32,   79,  -70,   36,  121,  114,  -88,  104 }},
   {{  64,  -99,  117,   72,  -15,  -29,   81,  -49 }},
   {{ 128,   59,  -23, -113,  -30,  -58,  -95,  -98 }}
};

static const m128_v16 FFT256_twiddle[] __attribute__((aligned(64))) =
{
   {{   1,   41, -118,   45,   46,   87,  -31,   14 }},
   {{  60, -110,  116, -127,  -67,   80,  -61,   69 }},
   {{   2,   82,   21,   90,   92,  -83,  -62,   28 }},
   {{ 120,   37,  -25,    3,  123,  -97, -122, -119 }},
   {{   4,  -93,   42,  -77,  -73,   91, -124,   56 }},
   {{ -17,   74,  -50,    6,  -11,   63,   13,   19 }},
   {{   8,   71,   84,  103,  111,  -75,    9,  112 }},
   {{ -34, -109, -100,   12,  -22,  126,   26,   38 }},
   {{  16, -115,  -89,  -51,  -35,  107,   18,  -33 }},
   {{ -68,   39,   57,   24,  -44,   -5,   52,   76 }},
   {{  32,   27,   79, -102,  -70,  -43,   36,  -66 }},
   {{ 121,   78,  114,   48,  -88,  -10,  104, -105 }},
   {{  64,   54,  -99,   53,  117,  -86,   72,  125 }},
   {{ -15, -101,  -29,   96,   81,  -20,  -49,   47 }},
   {{ 128,  108,   59,  106,  -23,   85, -113,   -7 }},
   {{ -30,   55,  -58,  -65,  -95,  -40,  -98,   94 }},
};

#if defined(__x86_64__)

#define SHUFXOR_1(x)        _mm_shuffle_epi32(x,0xb1)
#define SHUFXOR_2(x)        _mm_shuffle_epi32(x,0x4e)
#define SHUFXOR_3(x)        _mm_shuffle_epi32(x,0x1b)

#elif defined(__aarch64__)

#define SHUFXOR_1(x)        vrev64q_u32(x)
#define SHUFXOR_2(x)        v128_rev64(x)
#define SHUFXOR_3(x)        v128_rev64(v128_qrev32(x))

#else
//unknown or unsupported architecture
#endif

#define shufxor(x,s)   XCAT(SHUFXOR_,s)(x) 

#define REDUCE(x) \
  v128_sub16( v128_and( x, v128_64( \
                         0x00ff00ff00ff00ff ) ), v128_sra16( x, 8 ) )

#define EXTRA_REDUCE_S(x)\
  v128_sub16( x, v128_and( \
          v128_64( 0x0101010101010101 ), \
          v128_cmpgt16( x, v128_64( 0x0080008000800080 ) ) ) )

#define REDUCE_FULL_S( x )  EXTRA_REDUCE_S( REDUCE (x ) )

#define DO_REDUCE_FULL_S(i) \
do { \
    X(i) = REDUCE( X(i) );                        \
    X(i) = EXTRA_REDUCE_S( X(i) );                \
} while(0)


static void FFT64( void *a )
{
  v128u16_t* const A = a;
  register v128u16_t X0, X1, X2, X3, X4, X5, X6, X7;

#define X(i) X##i

  X0 = A[0];
  X1 = A[1];
  X2 = A[2];
  X3 = A[3];
  X4 = A[4];
  X5 = A[5];
  X6 = A[6];
  X7 = A[7];

#define DO_REDUCE(i)   X(i) = REDUCE( X(i) )

   // Begin with 8 parallels DIF FFT_8
   //
   // FFT_8 using w=4 as 8th root of unity
   //  Unrolled decimation in frequency (DIF) radix-2 NTT.
   //  Output data is in revbin_permuted order.

  static const int w[] = {0, 2, 4, 6};

#define BUTTERFLY_0( i,j ) \
do { \
    v128u16_t v = X(j); \
    X(j) = v128_add16( X(i), X(j) ); \
    X(i) = v128_sub16( X(i), v ); \
} while(0)

#define BUTTERFLY_N( i,j,n ) \
do { \
    v128u16_t v = X(j); \
    X(j) = v128_add16( X(i), X(j) ); \
    X(i) = v128_sl16( v128_sub16( X(i), v ), w[n] ); \
} while(0)

  BUTTERFLY_0( 0, 4 );
  BUTTERFLY_N( 1, 5, 1 );
  BUTTERFLY_N( 2, 6, 2 );
  BUTTERFLY_N( 3, 7, 3 );

  DO_REDUCE( 2 );
  DO_REDUCE( 3 );

  BUTTERFLY_0( 0, 2 );
  BUTTERFLY_0( 4, 6 );
  BUTTERFLY_N( 1, 3, 2 );
  BUTTERFLY_N( 5, 7, 2 );

  DO_REDUCE( 1 );

  BUTTERFLY_0( 0, 1 );
  BUTTERFLY_0( 2, 3 );
  BUTTERFLY_0( 4, 5 );
  BUTTERFLY_0( 6, 7 );

  /* We don't need to reduce X(7) */
  DO_REDUCE_FULL_S( 0 );
  DO_REDUCE_FULL_S( 1 );
  DO_REDUCE_FULL_S( 2 );
  DO_REDUCE_FULL_S( 3 );
  DO_REDUCE_FULL_S( 4 );
  DO_REDUCE_FULL_S( 5 );
  DO_REDUCE_FULL_S( 6 );

#undef BUTTERFLY_0
#undef BUTTERFLY_N

  // Multiply by twiddle factors
  X(6) = v128_mul16( X(6), FFT64_twiddle[0].v128 );
  X(5) = v128_mul16( X(5), FFT64_twiddle[1].v128 );
  X(4) = v128_mul16( X(4), FFT64_twiddle[2].v128 );
  X(3) = v128_mul16( X(3), FFT64_twiddle[3].v128 );
  X(2) = v128_mul16( X(2), FFT64_twiddle[4].v128 );
  X(1) = v128_mul16( X(1), FFT64_twiddle[5].v128 );
  X(0) = v128_mul16( X(0), FFT64_twiddle[6].v128 );

  // Transpose the FFT state with a revbin order permutation
  // on the rows and the column.
  // This will make the full FFT_64 in order.
#define INTERLEAVE(i,j) \
  do { \
    v128u16_t t1= X(i); \
    v128u16_t t2= X(j); \
    X(i) = v128_unpacklo16( t1, t2 ); \
    X(j) = v128_unpackhi16( t1, t2 ); \
  } while(0)

  INTERLEAVE( 1, 0 );
  INTERLEAVE( 3, 2 );
  INTERLEAVE( 5, 4 );
  INTERLEAVE( 7, 6 );

  INTERLEAVE( 2, 0 );
  INTERLEAVE( 3, 1 );
  INTERLEAVE( 6, 4 );
  INTERLEAVE( 7, 5 );

  INTERLEAVE( 4, 0 );
  INTERLEAVE( 5, 1 );
  INTERLEAVE( 6, 2 );
  INTERLEAVE( 7, 3 );

#undef INTERLEAVE

   //Finish with 8 parallels DIT FFT_8
   //FFT_8 using w=4 as 8th root of unity
   // Unrolled decimation in time (DIT) radix-2 NTT.
   // Input data is in revbin_permuted order.

#define BUTTERFLY_0( i,j ) \
do { \
   v128u16_t u = X(j); \
   X(j) = v128_sub16( X(j), X(i) ); \
   X(i) = v128_add16( u, X(i) ); \
} while(0)


#define BUTTERFLY_N( i,j,n ) \
do { \
   v128u16_t u = X(j); \
   X(i) = v128_sl16( X(i), w[n] ); \
   X(j) = v128_sub16( X(j), X(i) ); \
   X(i) = v128_add16( u, X(i) ); \
} while(0)

  DO_REDUCE( 0 );
  DO_REDUCE( 1 );
  DO_REDUCE( 2 );
  DO_REDUCE( 3 );
  DO_REDUCE( 4 );
  DO_REDUCE( 5 );
  DO_REDUCE( 6 );
  DO_REDUCE( 7 );

  BUTTERFLY_0( 0, 1 );
  BUTTERFLY_0( 2, 3 );
  BUTTERFLY_0( 4, 5 );
  BUTTERFLY_0( 6, 7 );

  BUTTERFLY_0( 0, 2 );
  BUTTERFLY_0( 4, 6 );
  BUTTERFLY_N( 1, 3, 2 );
  BUTTERFLY_N( 5, 7, 2 );

  DO_REDUCE( 3 );

  BUTTERFLY_0( 0, 4 );
  BUTTERFLY_N( 1, 5, 1 );
  BUTTERFLY_N( 2, 6, 2 );
  BUTTERFLY_N( 3, 7, 3 );

  DO_REDUCE_FULL_S( 0 );
  DO_REDUCE_FULL_S( 1 );
  DO_REDUCE_FULL_S( 2 );
  DO_REDUCE_FULL_S( 3 );
  DO_REDUCE_FULL_S( 4 );
  DO_REDUCE_FULL_S( 5 );
  DO_REDUCE_FULL_S( 6 );
  DO_REDUCE_FULL_S( 7 );

#undef BUTTERFLY_0
#undef BUTTERFLY_N
#undef DO_REDUCE

  A[0] = X0;
  A[1] = X1;
  A[2] = X2;
  A[3] = X3;
  A[4] = X4;
  A[5] = X5;
  A[6] = X6;
  A[7] = X7;

#undef X
}

static void FFT128( void *a )
{
  int i;
  // Temp space to help for interleaving in the end
  v128u16_t B[8];
  v128u16_t *A = (v128u16_t*) a;

  /* Size-2 butterflies */
  for ( i = 0; i<8; i++ )
  {
    B[ i ]   = v128_add16( A[ i ], A[ i+8 ] );
    B[ i ]   = REDUCE_FULL_S( B[ i ] );
    A[ i+8 ] = v128_sub16( A[ i ], A[ i+8 ] );
    A[ i+8 ] = REDUCE_FULL_S( A[ i+8 ] );
    A[ i+8 ] = v128_mul16( A[ i+8 ], FFT128_twiddle[i].v128 );
    A[ i+8 ] = REDUCE_FULL_S( A[ i+8 ] );
  }

  FFT64( B );
  FFT64( A+8 );

  /* Transpose (i.e. interleave) */
  for ( i = 0; i < 8; i++ )
  {
    A[ 2*i   ] = v128_unpacklo16( B[ i ], A[ i+8 ] );
    A[ 2*i+1 ] = v128_unpackhi16( B[ i ], A[ i+8 ] );
  }
}

void FFT256_msg( uint16_t *a, const uint8_t *x, int final )
{
  const v128u16_t zero = v128_zero;
  static const m128_v16 Tweak      = {{ 0,0,0,0,0,0,0,1 }};
  static const m128_v16 FinalTweak = {{ 0,0,0,0,0,1,0,1 }};

  v128u8_t  *X = (v128u8_t*)x;
  v128u16_t *A = (v128u16_t*)a;

#define UNPACK( i ) \
do { \
    v128u8_t t = X[i]; \
    A[ 2*i      ] = v128_unpacklo8( t, zero ); \
    A[ 2*i + 16 ] = v128_mul16( A[ 2*i ], \
                                        FFT256_twiddle[ 2*i ].v128 ); \
    A[ 2*i + 16 ] = REDUCE( A[ 2*i + 16 ] ); \
    A[ 2*i +  1 ] = v128_unpackhi8( t, zero ); \
    A[ 2*i + 17 ] = v128_mul16( A[ 2*i + 1 ], \
                                        FFT256_twiddle[ 2*i + 1 ].v128 ); \
    A[ 2*i + 17 ] = REDUCE( A[ 2*i + 17 ] ); \
} while(0)

   // This allows to tweak the last butterflies to introduce X^127
#define UNPACK_TWEAK( i,tw ) \
do { \
    v128u8_t t = X[i]; \
    v128u16_t tmp; \
    A[ 2*i      ] = v128_unpacklo8( t, zero ); \
    A[ 2*i + 16 ] = v128_mul16( A[ 2*i ], \
                                        FFT256_twiddle[ 2*i ].v128 ); \
    A[ 2*i + 16 ] = REDUCE( A[ 2*i + 16 ] ); \
    tmp           = v128_unpackhi8( t, zero ); \
    A[ 2*i +  1 ] = v128_add16( tmp, tw ); \
    A[ 2*i + 17 ] = v128_mul16( v128_sub16( tmp, tw ), \
                                        FFT256_twiddle[ 2*i + 1 ].v128 ); \
  } while(0)

  UNPACK( 0 );
  UNPACK( 1 );
  UNPACK( 2 );
  UNPACK( 3 );
  UNPACK( 4 );
  UNPACK( 5 );
  UNPACK( 6 );
  if ( final )
    UNPACK_TWEAK( 7, FinalTweak.v128 );
  else
    UNPACK_TWEAK( 7, Tweak.v128 );

#undef UNPACK
#undef UNPACK_TWEAK

  FFT128( a );
  FFT128( a+128 );
}

#define c1_16( x ) {{ x,x,x,x, x,x,x,x }}

static void ROUNDS512( uint32_t *state, const uint8_t *msg, uint16_t *fft )
{
  register v128u32_t S0l, S1l, S2l, S3l, S0h, S1h, S2h, S3h;
  v128u32_t *S = (v128u32_t*)state;
  v128u8_t  *M = (v128u8_t*) msg;
  v128u16_t *W = (v128u16_t*)fft;
  static const m128_v16 code[] = { c1_16(185), c1_16(233) };

  S0l = v128_xor( S[0], M[0] );
  S0h = v128_xor( S[1], M[1] );
  S1l = v128_xor( S[2], M[2] );
  S1h = v128_xor( S[3], M[3] );
  S2l = v128_xor( S[4], M[4] );
  S2h = v128_xor( S[5], M[5] );
  S3l = v128_xor( S[6], M[6] );
  S3h = v128_xor( S[7], M[7] );

#define S(i) S##i

#define F_0(B, C, D) \
   v128_xor( v128_and( v128_xor( C,D ), B ), D )

#define F_1(B, C, D) \
   v128_or( v128_and( D, C ),\
                    v128_and( v128_or( D,C ), B ) )

#define Fl(a,b,c,fun) F_##fun (a##l,b##l,c##l)
#define Fh(a,b,c,fun) F_##fun (a##h,b##h,c##h)

#define STEP_1_( a,b,c,d,w,fun,r,s,z,p ) \
do { \
    TTl  = Fl( a,b,c,fun ); \
    TTh  = Fh( a,b,c,fun ); \
    a##l = v128_rol32( a##l, r ); \
    a##h = v128_rol32( a##h, r ); \
    w##l = v128_add32( w##l, d##l ); \
    w##h = v128_add32( w##h, d##h ); \
    TTl  = v128_add32( TTl, w##l ); \
    TTh  = v128_add32( TTh, w##h ); \
    TTl  = v128_rol32( TTl, s ); \
    TTh  = v128_rol32( TTh, s ); \
    PERM( p, z,d,a, shufxor ); \
} while(0)

#define STEP_1( a,b,c,d,w,fun,r,s,z,p )   STEP_1_( a,b,c,d,w,fun,r,s,z,p )

#define STEP_2_( a,b,c,d,w,fun,r,s ) \
do { \
    d##l = v128_add32( d##l, TTl ); \
    d##h = v128_add32( d##h, TTh ); \
} while(0)

#define STEP_2( a,b,c,d,w,fun,r,s )  STEP_2_( a,b,c,d,w,fun,r,s )

#define STEP( a,b,c,d,w1,w2,fun,r,s,z,p ) \
do { \
    register v128u32_t TTl, TTh, Wl=w1, Wh=w2; \
    STEP_1( a,b,c,d,W,fun,r,s,z,p ); \
    STEP_2( a,b,c,d,W,fun,r,s ); \
} while(0);

#define MSG_l(x) (2*(x))
#define MSG_h(x) (2*(x)+1)

#define MSG( w,hh,ll,u,z ) \
do { \
    int a = MSG_##u(hh); \
    int b = MSG_##u(ll); \
    w##l = v128_unpacklo16( W[a], W[b] ); \
    w##l = v128_mul16( w##l, code[z].v128 ); \
    w##h = v128_unpackhi16( W[a], W[b]) ; \
    w##h = v128_mul16( w##h, code[z].v128 ); \
} while(0)

#define _ROUND( h0,l0,u0,h1,l1,u1,h2,l2,u2,h3,l3,u3,fun,r,s,t,u,z,p ) \
do { \
    register v128u32_t W0l, W1l, W2l, W3l, TTl; \
    register v128u32_t W0h, W1h, W2h, W3h, TTh; \
    MSG( W0, h0, l0, u0, z ); \
    STEP_1( S(0), S(1), S(2), S(3), W0, fun, r, s, 0, p ); \
    MSG( W1, h1, l1, u1, z ); \
    STEP_2( S(0), S(1), S(2), S(3), W0, fun, r, s ); \
    STEP_1( S(3), S(0), S(1), S(2), W1, fun, s, t, 1, p ); \
    MSG( W2,h2,l2,u2,z ); \
    STEP_2( S(3), S(0), S(1), S(2), W1, fun, s, t ); \
    STEP_1( S(2), S(3), S(0), S(1), W2, fun, t, u, 2, p ); \
    MSG( W3,h3,l3,u3,z ); \
    STEP_2( S(2), S(3), S(0), S(1), W2, fun, t, u ); \
    STEP_1( S(1), S(2), S(3), S(0), W3, fun, u, r, 3, p ); \
    STEP_2( S(1), S(2), S(3), S(0), W3, fun, u, r ); \
} while(0)

   // 4 rounds with code 185
   _ROUND(  2, 10, l,  3, 11, l,  0,  8, l,  1,  9, l, 0, 3,  23, 17, 27, 0, 0);
   _ROUND(  3, 11, h,  2, 10, h,  1,  9, h,  0,  8, h, 1, 3,  23, 17, 27, 0, 4);
   _ROUND(  7, 15, h,  5, 13, h,  6, 14, l,  4, 12, l, 0, 28, 19, 22, 7,  0, 1);
   _ROUND(  4, 12, h,  6, 14, h,  5, 13, l,  7, 15, l, 1, 28, 19, 22, 7,  0, 5);

   // 4 rounds with code 233
   _ROUND(  0,  4, h,  1,  5, l,  3,  7, h,  2,  6, l, 0, 29,  9, 15,  5, 1, 2);
   _ROUND(  3,  7, l,  2,  6, h,  0,  4, l,  1,  5, h, 1, 29,  9, 15,  5, 1, 6);
   _ROUND( 11, 15, l,  8, 12, l,  8, 12, h, 11, 15, h, 0,  4, 13, 10, 25, 1, 3);
   _ROUND(  9, 13, h, 10, 14, h, 10, 14, l,  9, 13, l, 1,  4, 13, 10, 25, 1, 0);

   // 1 round as feed-forward
   STEP( S(0), S(1), S(2), S(3), S[0], S[1], 0,  4, 13, 0, 4 );
   STEP( S(3), S(0), S(1), S(2), S[2], S[3], 0, 13, 10, 1, 4 );
   STEP( S(2), S(3), S(0), S(1), S[4], S[5], 0, 10, 25, 2, 4 );
   STEP( S(1), S(2), S(3), S(0), S[6], S[7], 0, 25,  4, 3, 4 );

   S[0] = S0l;  S[1] = S0h;  S[2] = S1l;  S[3] = S1h;
   S[4] = S2l;  S[5] = S2h;  S[6] = S3l;  S[7] = S3h;

#undef STEP_1
#undef STEP_1_
#undef STEP_2
#undef STEP_2_
#undef STEP
#undef _ROUND
#undef S
#undef F_0
#undef F_1
#undef Fl
#undef Fh
#undef MSG_l
#undef MSG_h
#undef MSG
}

static void SIMD512_Compress( simd512_context *state, const void *m, int final )
{
   m128_v16 Y[32];
   uint16_t *y = (uint16_t*) Y[0].u16;

   FFT256_msg( y, m, final );
   ROUNDS512( state->A, m, y );
}

int simd512_init( simd512_context *state)
{
  v128u32_t *A = (v128u32_t*)state->A;

  state->hashbitlen = 512;
  state->n_feistels = 8;
  state->blocksize = 128*8;
  state->count = 0;

  for ( int i = 0; i < 8; i++ )
       A[i] = v128_set32( SIMD_IV_512[4*i+3], SIMD_IV_512[4*i+2],
                             SIMD_IV_512[4*i+1], SIMD_IV_512[4*i+0] );
  return 0;
}

int simd512_update(simd512_context *state, const void *data, int datalen )
{
  int current;
  int bs = state->blocksize;  // bits in one lane
 
  int databitlen = datalen * 8;
  current = state->count & (bs - 1);

  while ( databitlen > 0 )
  {
    if ( current == 0 && databitlen >= bs )
    {
      // We can hash the data directly from the input buffer.
      SIMD512_Compress( state, data, 0 );
      databitlen -= bs;
      data += ( bs/8 );
      state->count += bs;
    }
    else
    {
      // Copy a chunk of data to the buffer
      int len = bs - current;
      if ( databitlen < len )
      {
        memcpy( state->buffer + ( current/8 ), data, ( (databitlen+7)/8 ) );
        state->count += databitlen;
        break;
      }
      else
      {
        memcpy( state->buffer + (current/8), data, (len/8) );
        state->count += len;
        databitlen -= len;
        data += ( len/8 );
        current = 0;
        SIMD512_Compress( state, state->buffer, 0 );
      }
    }
  }
  return 0;
}

int simd512_close(simd512_context *state, void *hashval )
{
 //finalize
  int i;
  uint64_t l;
  int isshort = 1;
  int current = state->count & (state->blocksize - 1);

  // If there is still some data in the buffer, hash it
  if ( current )
  {
    current = ( current+7 ) / 8;
    memset( state->buffer + current, 0, ( state->blocksize/8 - current) );
    SIMD512_Compress( state, state->buffer, 0 );
  }

  //* Input the message length as the last block
  memset( state->buffer, 0, ( state->blocksize/8 ) );
  l = state->count;
  for ( i = 0; i < 8; i++ )
  {
    state->buffer[ i    ] = l & 0xff;
    l >>= 8;
  }
  if ( state->count < 16384 )
    isshort = 2;

  SIMD512_Compress( state, state->buffer, isshort );
  memcpy( hashval, state->A, ( state->hashbitlen / 8 ) );

  return 0;
}

#undef _ROUND
#undef shuxor
#undef REDUCE
#undef EXTRA_REDUCE_S
#undef REDUCE_FULL_S
#undef DO_REDUCE_FULL_S
#undef c1_16
#undef SHUFXOR_1 
#undef SHUFXOR_2 
#undef SHUFXOR_3 

#endif


