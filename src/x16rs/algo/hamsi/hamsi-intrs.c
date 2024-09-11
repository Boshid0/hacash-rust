
#include <stddef.h>
#include <string.h>

#include "hamsi.h"
#include "simd-utils.h"

#ifdef __cplusplus
extern "C"{
#endif

#if SPH_SMALL_FOOTPRINT && !defined SPH_SMALL_FOOTPRINT_HAMSI
#define SPH_SMALL_FOOTPRINT_HAMSI   1
#endif

/*
 * The SPH_HAMSI_EXPAND_* define how many input bits we handle in one
 * table lookup during message expansion (1 to 8, inclusive). If we note
 * w the number of bits per message word (w=32 for Hamsi-224/256, w=64
 * for Hamsi-384/512), r the size of a "row" in 32-bit words (r=8 for
 * Hamsi-224/256, r=16 for Hamsi-384/512), and n the expansion level,
 * then we will get t tables (where t=ceil(w/n)) of individual size
 * 2^n*r*4 (in bytes). The last table may be shorter (e.g. with w=32 and
 * n=5, there are 7 tables, but the last one uses only two bits on
 * input, not five).
 *
 * Also, we read t rows of r words from RAM. Words in a given row are
 * concatenated in RAM in that order, so most of the cost is about
 * reading the first row word; comparatively, cache misses are thus
 * less expensive with Hamsi-512 (r=16) than with Hamsi-256 (r=8).
 *
 * When n=1, tables are "special" in that we omit the first entry of
 * each table (which always contains 0), so that total table size is
 * halved.
 *
 * We thus have the following (size1 is the cumulative table size of
 * Hamsi-224/256; size2 is for Hamsi-384/512; similarly, t1 and t2
 * are for Hamsi-224/256 and Hamsi-384/512, respectively).
 *
 *   n      size1      size2    t1    t2
 * ---------------------------------------
 *   1       1024       4096    32    64
 *   2       2048       8192    16    32
 *   3       2688      10880    11    22
 *   4       4096      16384     8    16
 *   5       6272      25600     7    13
 *   6      10368      41984     6    11
 *   7      16896      73856     5    10
 *   8      32768     131072     4     8
 *
 * So there is a trade-off: a lower n makes the tables fit better in
 * L1 cache, but increases the number of memory accesses. The optimal
 * value depends on the amount of available L1 cache and the relative
 * impact of a cache miss.
 *
 * Experimentally, in ideal benchmark conditions (which are not necessarily
 * realistic with regards to L1 cache contention), it seems that n=8 is
 * the best value on "big" architectures (those with 32 kB or more of L1
 * cache), while n=4 is better on "small" architectures. This was tested
 * on an Intel Core2 Q6600 (both 32-bit and 64-bit mode), a PowerPC G3
 * (32 kB L1 cache, hence "big"), and a MIPS-compatible Broadcom BCM3302
 * (8 kB L1 cache).
 *
 * Note: with n=1, the 32 tables (actually implemented as one big table)
 * are read entirely and sequentially, regardless of the input data,
 * thus avoiding any data-dependent table access pattern.
 */

#define SPH_HAMSI_EXPAND_BIG    8


#ifdef _MSC_VER
#pragma warning (disable: 4146)
#endif

#include "hamsi_helper.c"

static const uint32_t HAMSI_IV224[] __attribute__ ((aligned (32)))= {
	0xc3967a67, 0xc3bc6c20, 0x4bc3bcc3,
	0xa7c3bc6b, 0x2c204b61, 0x74686f6c,
	0x69656b65, 0x20556e69
};

/*
 * This version is the one used in the Hamsi submission package for
 * round 2 of the SHA-3 competition; the UTF-8 encoding is wrong and
 * shall soon be corrected in the official Hamsi specification.
 *
static const uint32_t HAMSI_IV224[] = {
	0x3c967a67, 0x3cbc6c20, 0xb4c343c3,
	0xa73cbc6b, 0x2c204b61, 0x74686f6c,
	0x69656b65, 0x20556e69
};
 */

static const uint32_t HAMSI_IV512[] __attribute__ ((aligned (32)))= {
	0x73746565, 0x6c706172, 0x6b204172,
	0x656e6265, 0x72672031, 0x302c2062,
	0x75732032, 0x3434362c, 0x20422d33,
	0x30303120, 0x4c657576, 0x656e2d48,
	0x65766572, 0x6c65652c, 0x2042656c,
	0x6769756d
};

static const uint32_t alpha_n[] __attribute__ ((aligned (32)))= {
	0xff00f0f0, 0xccccaaaa, 0xf0f0cccc,
	0xff00aaaa, 0xccccaaaa, 0xf0f0ff00,
	0xaaaacccc, 0xf0f0ff00, 0xf0f0cccc,
	0xaaaaff00, 0xccccff00, 0xaaaaf0f0,
	0xaaaaf0f0, 0xff00cccc, 0xccccf0f0,
	0xff00aaaa, 0xccccaaaa, 0xff00f0f0,
	0xff00aaaa, 0xf0f0cccc, 0xf0f0ff00,
	0xccccaaaa, 0xf0f0ff00, 0xaaaacccc,
	0xaaaaff00, 0xf0f0cccc, 0xaaaaf0f0,
	0xccccff00, 0xff00cccc, 0xaaaaf0f0,
	0xff00aaaa, 0xccccf0f0
};

static const uint32_t alpha_f[] __attribute__ ((aligned (32)))= {
	0xcaf9639c, 0x0ff0f9c0, 0x639c0ff0,
	0xcaf9f9c0, 0x0ff0f9c0, 0x639ccaf9,
	0xf9c00ff0, 0x639ccaf9, 0x639c0ff0,
	0xf9c0caf9, 0x0ff0caf9, 0xf9c0639c,
	0xf9c0639c, 0xcaf90ff0, 0x0ff0639c,
	0xcaf9f9c0, 0x0ff0f9c0, 0xcaf9639c,
	0xcaf9f9c0, 0x639c0ff0, 0x639ccaf9,
	0x0ff0f9c0, 0x639ccaf9, 0xf9c00ff0,
	0xf9c0caf9, 0x639c0ff0, 0xf9c0639c,
	0x0ff0caf9, 0xcaf90ff0, 0xf9c0639c,
	0xcaf9f9c0, 0x0ff0639c
};

#define SBOX(a, b, c, d)   do { \
		__m256i t; \
		t = (a); \
		(a) = _mm256_and_si256((a),(c)); \
		(a) = _mm256_xor_si256((a),(d)); \
		(c) = _mm256_xor_si256((c),(b)); \
		(c) = _mm256_xor_si256((c),(a)); \
		(d) = _mm256_or_si256((d),(t)); \
        (d) = _mm256_xor_si256((d),(b)); \
	    t   = _mm256_xor_si256(t,(c)); \
		(b) = (d); \
		(d) = _mm256_or_si256((d),(t)); \
		(d) = _mm256_xor_si256((d),(a)); \
		(a) = _mm256_and_si256((a),(b)); \
		(t) = _mm256_xor_si256(t,(a)); \
		(b) = _mm256_xor_si256((b),(d)); \
		(b) = _mm256_xor_si256((b),t); \
		(a) = (c); \
		(c) = (b); \
		(b) = (d); \
		(d) = _mm256_andnot_si256(t, _mm256_set1_epi32(0xFFFFFFFF)); \
	} while (0)

#define v256_rol32( v, c ) \
   _mm256_or_si256( _mm256_slli_epi32( v, c ), _mm256_srli_epi32( v, 32-(c) ) )

// a ^ b ^ c
#if defined(__AVX512VL__)
#define v256_xor3( a, b, c )      _mm256_ternarylogic_epi32( a, b, c, 0x96 )
#else
#define v256_xor3( a, b, c ) \
  _mm256_xor_si256( a, _mm256_xor_si256( b, c ) )
#endif

#define HAMSI_L(a, b, c, d)   do { \
		a = v256_rol32( a, 13 ); \
		c = v256_rol32( c,  3 ); \
		b = v256_xor3( a, b, c ); \
		d = v256_xor3( d, c, _mm256_slli_epi32( a, 3 ) ); \
		b = v256_rol32( b, 1 ); \
		d = v256_rol32( d, 7 ); \
		a = v256_xor3( a, b, d ); \
		c = v256_xor3( c, d, _mm256_slli_epi32( b, 7 ) ); \
		a = v256_rol32( a,  5 ); \
		c = v256_rol32( c, 22 ); \
	} while (0)

#define DECL_STATE_BIG_hamsi  uint32_t c[16] __attribute__ ((aligned (32)));
	// uint32_t c0, c1, c2, c3, c4, c5, c6, c7; 
	// uint32_t c8, c9, cA, cB, cC, cD, cE, cF;

#define READ_STATE_BIG_hamsi(sc)   do { \
        memcpy(c,sc->h,sizeof(c)); \
	} while (0)

#define WRITE_STATE_BIG_hamsi(sc)   do { \
       memcpy(sc->h,c,sizeof(sc->h)); \
	} while (0)

#define s00   m[0]
#define s01   m[1]
#define s02   c[0]
#define s03   c[1]
#define s04   m[2]
#define s05   m[3]
#define s06   c[2]
#define s07   c[3]
#define s08   c[4]
#define s09   c[5]
#define s0A   m[4]
#define s0B   m[5]
#define s0C   c[6]
#define s0D   c[7]
#define s0E   m[6]
#define s0F   m[7]
#define s10   m[8]
#define s11   m[9]
#define s12   c[8]
#define s13   c[9]
#define s14   m[10]
#define s15   m[11]
#define s16   c[10]
#define s17   c[11]
#define s18   c[12]
#define s19   c[13]
#define s1A   m[12]
#define s1B   m[13]
#define s1C   c[14]
#define s1D   c[15]
#define s1E   m[14]
#define s1F   m[15]

#if defined(__AVX512VL__)
#define INPUT_BIG              \
	do                         \
	{                          \
		unsigned acc = buf[0]; \
		__m512i rp_0, rp_8, rp_16, rp_24, rp_32, rp_40, rp_48, rp_56; \
		rp_0  = _mm512_loadu_si512((__m512i *)(&T512_0[buf[0]][0]));\
		rp_8  = _mm512_loadu_si512((__m512i *)(&T512_8[buf[1]][0])); \
		rp_16 = _mm512_loadu_si512((__m512i *)(&T512_16[buf[2]][0]));\
		rp_24 = _mm512_loadu_si512((__m512i *)(&T512_24[buf[3]][0]));\
		rp_32 = _mm512_loadu_si512((__m512i *)(&T512_32[buf[4]][0]));\
		rp_40 = _mm512_loadu_si512((__m512i *)(&T512_40[buf[5]][0])); \
		rp_48 = _mm512_loadu_si512((__m512i *)(&T512_48[buf[6]][0]));\
		rp_56 = _mm512_loadu_si512((__m512i *)(&T512_56[buf[7]][0])); \
		__m512i tmp;                        \
		tmp = _mm512_xor_si512(rp_0, rp_8); \
		tmp = _mm512_xor_si512(tmp, rp_16); \
		tmp = _mm512_xor_si512(tmp, rp_24);\
		tmp = _mm512_xor_si512(tmp, rp_32);\
		tmp = _mm512_xor_si512(tmp, rp_40);\
		tmp = _mm512_xor_si512(tmp, rp_48);\
		tmp = _mm512_xor_si512(tmp, rp_56);\
		_mm512_storeu_epi32(m, tmp);  \
	} while (0)

#else 

#define INPUT_BIG              \
	do                         \
	{                          \
		unsigned acc = buf[0]; \
		__m256i rp_0,rp_4,rp_8,rp_12,rp_16,rp_20,rp_24,rp_28,rp_32,rp_36,rp_40,rp_44,rp_48,rp_52,rp_56,rp_60; \
		rp_0  = _mm256_loadu_si256((__m256i *)(&T512_0[buf[0]][0]));\
		rp_4  = _mm256_loadu_si256((__m256i *)(&T512_0[buf[0]][0])+1);\
		rp_8  = _mm256_loadu_si256((__m256i *)(&T512_8[buf[1]][0])); \
		rp_12 = _mm256_loadu_si256((__m256i *)(&T512_8[buf[1]][0])+1); \
		rp_16 = _mm256_loadu_si256((__m256i *)(&T512_16[buf[2]][0]));\
		rp_20 = _mm256_loadu_si256((__m256i *)(&T512_16[buf[2]][0])+1);\
		rp_24 = _mm256_loadu_si256((__m256i *)(&T512_24[buf[3]][0]));\
		rp_28 = _mm256_loadu_si256((__m256i *)(&T512_24[buf[3]][0])+1);\
		rp_32 = _mm256_loadu_si256((__m256i *)(&T512_32[buf[4]][0]));\
		rp_36 = _mm256_loadu_si256((__m256i *)(&T512_32[buf[4]][0])+1);\
		rp_40 = _mm256_loadu_si256((__m256i *)(&T512_40[buf[5]][0])); \
		rp_44 = _mm256_loadu_si256((__m256i *)(&T512_40[buf[5]][0])+1); \
		rp_48 = _mm256_loadu_si256((__m256i *)(&T512_48[buf[6]][0]));\
		rp_52 = _mm256_loadu_si256((__m256i *)(&T512_48[buf[6]][0])+1);\
		rp_56 = _mm256_loadu_si256((__m256i *)(&T512_56[buf[7]][0])); \
		rp_60 = _mm256_loadu_si256((__m256i *)(&T512_56[buf[7]][0])+1); \
		__m256i tmp0, tmp1;                        \
		tmp0 = _mm256_xor_si256(rp_0, rp_8); \
		tmp1 = _mm256_xor_si256(rp_4, rp_12); \
		tmp0 = _mm256_xor_si256(tmp0, rp_16); \
		tmp1 = _mm256_xor_si256(tmp1, rp_20); \
		tmp0 = _mm256_xor_si256(tmp0, rp_24);\
		tmp1 = _mm256_xor_si256(tmp1, rp_28);\
		tmp0 = _mm256_xor_si256(tmp0, rp_32);\
		tmp1 = _mm256_xor_si256(tmp1, rp_36);\
		tmp0 = _mm256_xor_si256(tmp0, rp_40);\
		tmp1 = _mm256_xor_si256(tmp1, rp_44);\
		tmp0 = _mm256_xor_si256(tmp0, rp_48);\
		tmp1 = _mm256_xor_si256(tmp1, rp_52);\
		tmp0 = _mm256_xor_si256(tmp0, rp_56);\
		tmp1 = _mm256_xor_si256(tmp1, rp_60);\
		_mm256_store_si256((__m256i *)m, tmp0);  \
		_mm256_store_si256((__m256i *)m +1, tmp1);  \
	} while (0)

#endif

#define ROUND_BIG(rc, alpha)   do { \
		__m256i ss0, ss1, ss2, ss3;                                                                                                                                                \
		ss0 = _mm256_xor_si256(_mm256_setr_epi32(s00, s01, s02, s03, s04, s05, s06, s07), _mm256_setr_epi32(alpha[0x00], (alpha[0x01] ^ (uint32_t)(rc)), alpha[0x02], alpha[0x03], alpha[0x04], alpha[0x05], alpha[0x06], alpha[0x07])); \
		ss1 = _mm256_xor_si256(_mm256_setr_epi32(s08, s09, s0A, s0B, s0C, s0D, s0E, s0F), _mm256_loadu_si256((__m256i *)alpha +1));                    \
		ss2 = _mm256_xor_si256(_mm256_setr_epi32(s10, s11, s12, s13, s14, s15, s16, s17), _mm256_loadu_si256((__m256i *)alpha +2));                    \
		ss3 = _mm256_xor_si256(_mm256_setr_epi32(s18, s19, s1A, s1B, s1C, s1D, s1E, s1F), _mm256_loadu_si256((__m256i *)alpha +3));                    \
		SBOX(ss0, ss1, ss2, ss3);                                                                                                                                                  \
		uint32_t tmp[8] __attribute__ ((aligned (32)));                                                                                                                                                           \
		__m256i l0, l1, l2, l3;                                                                                                                                                    \
		l0 = ss0; \
		_mm256_store_si256((__m256i *)tmp, ss1);                                                                                                                                             \
		l1 = _mm256_setr_epi32(tmp[1], tmp[2], tmp[3], tmp[4], tmp[5], tmp[6], tmp[7], tmp[0]);                                                                                     \
		_mm256_store_si256((__m256i *)tmp, ss2);                                                                                                                                             \
	    l2 = _mm256_setr_epi32(tmp[2], tmp[3], tmp[4], tmp[5], tmp[6], tmp[7], tmp[0], tmp[1]);                                                                                     \
		_mm256_store_si256((__m256i *)tmp, ss3);                                                                                                                                             \
		l3 = _mm256_setr_epi32(tmp[3], tmp[4], tmp[5], tmp[6], tmp[7], tmp[0], tmp[1], tmp[2]);                                                                                     \
		HAMSI_L(l0, l1, l2, l3);   \
		_mm256_store_si256((__m256i *)tmp, l0);                                                                                                                                               \
		s00 = tmp[0];                                                                                                                                                               \
		s01 = tmp[1];                                                                                                                                                               \
		s02 = tmp[2];                                                                                                                                                               \
		s03 = tmp[3];                                                                                                                                                               \
		s04 = tmp[4];                                                                                                                                                               \
		s05 = tmp[5];                                                                                                                                                               \
		s06 = tmp[6];                                                                                                                                                               \
		s07 = tmp[7];                                                                                                                                                               \
		_mm256_store_si256((__m256i *)tmp, l1);                                                                                                                                               \
		s09 = tmp[0];                                                                                                                                                               \
		s0A = tmp[1];                                                                                                                                                               \
		s0B = tmp[2];                                                                                                                                                               \
		s0C = tmp[3];                                                                                                                                                               \
		s0D = tmp[4];                                                                                                                                                               \
		s0E = tmp[5];                                                                                                                                                               \
		s0F = tmp[6];                                                                                                                                                               \
		s08 = tmp[7];                                                                                                                                                               \
		_mm256_store_si256((__m256i *)tmp, l2);                                                                                                                                               \
		s12 = tmp[0];                                                                                                                                                               \
		s13 = tmp[1];                                                                                                                                                               \
		s14 = tmp[2];                                                                                                                                                               \
		s15 = tmp[3];                                                                                                                                                               \
		s16 = tmp[4];                                                                                                                                                               \
		s17 = tmp[5];                                                                                                                                                               \
		s10 = tmp[6];                                                                                                                                                               \
		s11 = tmp[7];                                                                                                                                                               \
		_mm256_store_si256((__m256i *)tmp, l3);                                                                                                                                               \
		s1B = tmp[0];                                                                                                                                                               \
		s1C = tmp[1];                                                                                                                                                               \
		s1D = tmp[2];                                                                                                                                                               \
		s1E = tmp[3];                                                                                                                                                               \
		s1F = tmp[4];                                                                                                                                                               \
		s18 = tmp[5];                                                                                                                                                               \
		s19 = tmp[6];                                                                                                                                                               \
		s1A = tmp[7];                                                                                                                                                               \
		l0 = _mm256_setr_epi32(s00, s10, s09, s19, 0, 0, 0, 0);                                                                                                                    \
		l1 = _mm256_setr_epi32(s02, s13, s0B, s1A, 0, 0, 0, 0);                                                                                                                    \
		l2 = _mm256_setr_epi32(s05, s15, s0C, s1C, 0, 0, 0, 0);                                                                                                                    \
		l3 = _mm256_setr_epi32(s07, s16, s0E, s1F, 0, 0, 0, 0);                                                                                                                    \
		HAMSI_L(l0, l1, l2, l3);                                                                                                                                                   \
		_mm256_store_si256((__m256i *)tmp, l0);                                                                                                                                               \
		s00 = tmp[0];                                                                                                                                                               \
		s10 = tmp[1];                                                                                                                                                               \
		s09 = tmp[2];                                                                                                                                                               \
		s19 = tmp[3];                                                                                                                                                               \
		_mm256_store_si256((__m256i *)tmp, l1);                                                                                                                                               \
		s02 = tmp[0];                                                                                                                                                               \
		s13 = tmp[1];                                                                                                                                                               \
		s0B = tmp[2];                                                                                                                                                               \
		s1A = tmp[3];                                                                                                                                                               \
		_mm256_store_si256((__m256i *)tmp, l2);                                                                                                                                               \
		s05 = tmp[0];                                                                                                                                                               \
		s15 = tmp[1];                                                                                                                                                               \
		s0C = tmp[2];                                                                                                                                                               \
		s1C = tmp[3];                                                                                                                                                               \
		_mm256_store_si256((__m256i *)tmp, l3);                                                                                                                                               \
		s07 = tmp[0];                                                                                                                                                               \
		s16 = tmp[1];                                                                                                                                                               \
		s0E = tmp[2];                                                                                                                                                               \
		s1F = tmp[3];  \
	} while (0)

#define P_BIG   do { \
		ROUND_BIG(0, alpha_n); \
		ROUND_BIG(1, alpha_n); \
		ROUND_BIG(2, alpha_n); \
		ROUND_BIG(3, alpha_n); \
		ROUND_BIG(4, alpha_n); \
		ROUND_BIG(5, alpha_n); \
	} while (0)

#define PF_BIG   do { \
		ROUND_BIG(0, alpha_f); \
		ROUND_BIG(1, alpha_f); \
		ROUND_BIG(2, alpha_f); \
		ROUND_BIG(3, alpha_f); \
		ROUND_BIG(4, alpha_f); \
		ROUND_BIG(5, alpha_f); \
		ROUND_BIG(6, alpha_f); \
		ROUND_BIG(7, alpha_f); \
		ROUND_BIG(8, alpha_f); \
		ROUND_BIG(9, alpha_f); \
		ROUND_BIG(10, alpha_f); \
		ROUND_BIG(11, alpha_f); \
	} while (0)

#define T_BIG   do { \
		/* order is important */ \
		c[15] = (sc->h[0xF] ^= s17); \
		c[14] = (sc->h[0xE] ^= s16); \
		c[13] = (sc->h[0xD] ^= s15); \
		c[12] = (sc->h[0xC] ^= s14); \
		c[11] = (sc->h[0xB] ^= s13); \
		c[10] = (sc->h[0xA] ^= s12); \
		c[9] = (sc->h[0x9] ^= s11); \
		c[8] = (sc->h[0x8] ^= s10); \
		c[7] = (sc->h[0x7] ^= s07); \
		c[6] = (sc->h[0x6] ^= s06); \
		c[5] = (sc->h[0x5] ^= s05); \
		c[4] = (sc->h[0x4] ^= s04); \
		c[3] = (sc->h[0x3] ^= s03); \
		c[2] = (sc->h[0x2] ^= s02); \
		c[1] = (sc->h[0x1] ^= s01); \
		c[0] = (sc->h[0x0] ^= s00); \
	} while (0)

static void
hamsi_big(sph_hamsi_big_context *sc, const unsigned char *buf, size_t num)
{
	DECL_STATE_BIG_hamsi

	sc->count += (uint64_t)num << 6;
	READ_STATE_BIG_hamsi(sc);
	while (num -- > 0) {
		uint32_t m[16] __attribute__ ((aligned (32)));
		// uint32_t m0, m1, m2, m3, m4, m5, m6, m7;
		// uint32_t m8, m9, mA, mB, mC, mD, mE, mF;

		INPUT_BIG;
		P_BIG;
		T_BIG;
		buf += 8;
	}
	WRITE_STATE_BIG_hamsi(sc);
}

static void
hamsi_big_final(sph_hamsi_big_context *sc, const unsigned char *buf)
{
	uint32_t m[16] __attribute__ ((aligned (32)));
	DECL_STATE_BIG_hamsi

	READ_STATE_BIG_hamsi(sc);
	INPUT_BIG;
	PF_BIG;
	T_BIG;
	WRITE_STATE_BIG_hamsi(sc);
}

static void
hamsi_big_init(sph_hamsi_big_context *sc, const uint32_t *iv)
{
	sc->partial_len = 0;
	memcpy(sc->h, iv, sizeof sc->h);

	sc->count = 0;
}

static void
hamsi_big_core(sph_hamsi_big_context *sc, const void *data, size_t len)
{
	if (sc->partial_len != 0) {
		size_t mlen;

		mlen = 8 - sc->partial_len;
		if (len < mlen) {
			memcpy(sc->partial + sc->partial_len, data, len);
			sc->partial_len += len;
			return;
		} else {
			memcpy(sc->partial + sc->partial_len, data, mlen);
			len -= mlen;
			data = (const unsigned char *)data + mlen;
			hamsi_big(sc, sc->partial, 1);
			sc->partial_len = 0;
		}
	}

	hamsi_big(sc, data, (len >> 3));
	data = (const unsigned char *)data + (len & ~(size_t)7);
	len &= (size_t)7;
	memcpy(sc->partial, data, len);
	sc->partial_len = len;
}

static void
hamsi_big_close(sph_hamsi_big_context *sc,
	unsigned ub, unsigned n, void *dst, size_t out_size_w32)
{
	unsigned char pad[8];
	size_t ptr, u;
	unsigned z;
	unsigned char *out;

	ptr = sc->partial_len;

	sph_enc64be(pad, sc->count + (ptr << 3) + n);

	z = 0x80 >> n;
	sc->partial[ptr ++] = ((ub & -z) | z) & 0xFF;
	while (ptr < 8)
		sc->partial[ptr ++] = 0;
	hamsi_big(sc, sc->partial, 1);
	hamsi_big_final(sc, pad);
	out = dst;
	if (out_size_w32 == 12) {
		sph_enc32be(out +  0, sc->h[ 0]);
		sph_enc32be(out +  4, sc->h[ 1]);
		sph_enc32be(out +  8, sc->h[ 3]);
		sph_enc32be(out + 12, sc->h[ 4]);
		sph_enc32be(out + 16, sc->h[ 5]);
		sph_enc32be(out + 20, sc->h[ 6]);
		sph_enc32be(out + 24, sc->h[ 8]);
		sph_enc32be(out + 28, sc->h[ 9]);
		sph_enc32be(out + 32, sc->h[10]);
		sph_enc32be(out + 36, sc->h[12]);
		sph_enc32be(out + 40, sc->h[13]);
		sph_enc32be(out + 44, sc->h[15]);
	} else {
		for (u = 0; u < 16; u ++)
			sph_enc32be(out + (u << 2), sc->h[u]);
	}
}

/* see sph_hamsi.h */
void
sph_hamsi512_init(void *cc)
{
	hamsi_big_init(cc, HAMSI_IV512);
}

/* see sph_hamsi.h */
void
sph_hamsi512(void *cc, const void *data, size_t len)
{
	hamsi_big_core(cc, data, len);
}

/* see sph_hamsi.h */
void
sph_hamsi512_close(void *cc, void *dst)
{
	hamsi_big_close(cc, 0, 0, dst, 16);
	//hamsi_big_init(cc, HAMSI_IV512);
}

/* see sph_hamsi.h */
void
sph_hamsi512_addbits_and_close(void *cc, unsigned ub, unsigned n, void *dst)
{
	hamsi_big_close(cc, ub, n, dst, 16);
	//hamsi_big_init(cc, HAMSI_IV512);
}

#undef s00  
#undef s01   
#undef s02   
#undef s03   
#undef s04   
#undef s05   
#undef s06   
#undef s07   
#undef s08   
#undef s09   
#undef s0A   
#undef s0B   
#undef s0C   
#undef s0D   
#undef s0E   
#undef s0F   
#undef s10  
#undef s11   
#undef s12   
#undef s13   
#undef s14  
#undef s15   
#undef s16   
#undef s17   
#undef s18   
#undef s19   
#undef s1A   
#undef s1B   
#undef s1C   
#undef s1D   
#undef s1E   
#undef s1F  

#ifdef __cplusplus
}
#endif