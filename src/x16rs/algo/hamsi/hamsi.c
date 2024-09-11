/* $Id: hamsi.c 251 2010-10-19 14:31:51Z tp $ */
/*
 * Hamsi implementation.
 *
 * ==========================(LICENSE BEGIN)============================
 *
 * Copyright (c) 2007-2010  Projet RNRT SAPHIR
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * ===========================(LICENSE END)=============================
 *
 * @author   Thomas Pornin <thomas.pornin@cryptolog.com>
 */

#include <stddef.h>
#include <string.h>

#include "hamsi.h"

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

#if !defined SPH_HAMSI_EXPAND_SMALL
#if SPH_SMALL_FOOTPRINT_HAMSI
#define SPH_HAMSI_EXPAND_SMALL  4
#else
#define SPH_HAMSI_EXPAND_SMALL  8
#endif
#endif

#if !defined SPH_HAMSI_EXPAND_BIG
#define SPH_HAMSI_EXPAND_BIG    8
#endif

#ifdef _MSC_VER
#pragma warning (disable: 4146)
#endif

#include "hamsi_helper.c"

static const sph_u32 HAMSI_IV512[] = {
	SPH_C32(0x73746565), SPH_C32(0x6c706172), SPH_C32(0x6b204172),
	SPH_C32(0x656e6265), SPH_C32(0x72672031), SPH_C32(0x302c2062),
	SPH_C32(0x75732032), SPH_C32(0x3434362c), SPH_C32(0x20422d33),
	SPH_C32(0x30303120), SPH_C32(0x4c657576), SPH_C32(0x656e2d48),
	SPH_C32(0x65766572), SPH_C32(0x6c65652c), SPH_C32(0x2042656c),
	SPH_C32(0x6769756d)
};

static const sph_u32 alpha_n[] = {
	SPH_C32(0xff00f0f0), SPH_C32(0xccccaaaa), SPH_C32(0xf0f0cccc),
	SPH_C32(0xff00aaaa), SPH_C32(0xccccaaaa), SPH_C32(0xf0f0ff00),
	SPH_C32(0xaaaacccc), SPH_C32(0xf0f0ff00), SPH_C32(0xf0f0cccc),
	SPH_C32(0xaaaaff00), SPH_C32(0xccccff00), SPH_C32(0xaaaaf0f0),
	SPH_C32(0xaaaaf0f0), SPH_C32(0xff00cccc), SPH_C32(0xccccf0f0),
	SPH_C32(0xff00aaaa), SPH_C32(0xccccaaaa), SPH_C32(0xff00f0f0),
	SPH_C32(0xff00aaaa), SPH_C32(0xf0f0cccc), SPH_C32(0xf0f0ff00),
	SPH_C32(0xccccaaaa), SPH_C32(0xf0f0ff00), SPH_C32(0xaaaacccc),
	SPH_C32(0xaaaaff00), SPH_C32(0xf0f0cccc), SPH_C32(0xaaaaf0f0),
	SPH_C32(0xccccff00), SPH_C32(0xff00cccc), SPH_C32(0xaaaaf0f0),
	SPH_C32(0xff00aaaa), SPH_C32(0xccccf0f0)
};

static const sph_u32 alpha_f[] = {
	SPH_C32(0xcaf9639c), SPH_C32(0x0ff0f9c0), SPH_C32(0x639c0ff0),
	SPH_C32(0xcaf9f9c0), SPH_C32(0x0ff0f9c0), SPH_C32(0x639ccaf9),
	SPH_C32(0xf9c00ff0), SPH_C32(0x639ccaf9), SPH_C32(0x639c0ff0),
	SPH_C32(0xf9c0caf9), SPH_C32(0x0ff0caf9), SPH_C32(0xf9c0639c),
	SPH_C32(0xf9c0639c), SPH_C32(0xcaf90ff0), SPH_C32(0x0ff0639c),
	SPH_C32(0xcaf9f9c0), SPH_C32(0x0ff0f9c0), SPH_C32(0xcaf9639c),
	SPH_C32(0xcaf9f9c0), SPH_C32(0x639c0ff0), SPH_C32(0x639ccaf9),
	SPH_C32(0x0ff0f9c0), SPH_C32(0x639ccaf9), SPH_C32(0xf9c00ff0),
	SPH_C32(0xf9c0caf9), SPH_C32(0x639c0ff0), SPH_C32(0xf9c0639c),
	SPH_C32(0x0ff0caf9), SPH_C32(0xcaf90ff0), SPH_C32(0xf9c0639c),
	SPH_C32(0xcaf9f9c0), SPH_C32(0x0ff0639c)
};

#define INPUT_BIG   do { \
		unsigned acc = buf[0]; \
		const sph_u32 *rp; \
		rp = &T512_0[acc][0]; \
		m0 = rp[0]; \
		m1 = rp[1]; \
		m2 = rp[2]; \
		m3 = rp[3]; \
		m4 = rp[4]; \
		m5 = rp[5]; \
		m6 = rp[6]; \
		m7 = rp[7]; \
		m8 = rp[8]; \
		m9 = rp[9]; \
		mA = rp[10]; \
		mB = rp[11]; \
		mC = rp[12]; \
		mD = rp[13]; \
		mE = rp[14]; \
		mF = rp[15]; \
		acc = buf[1]; \
		rp = &T512_8[acc][0]; \
		m0 ^= rp[0]; \
		m1 ^= rp[1]; \
		m2 ^= rp[2]; \
		m3 ^= rp[3]; \
		m4 ^= rp[4]; \
		m5 ^= rp[5]; \
		m6 ^= rp[6]; \
		m7 ^= rp[7]; \
		m8 ^= rp[8]; \
		m9 ^= rp[9]; \
		mA ^= rp[10]; \
		mB ^= rp[11]; \
		mC ^= rp[12]; \
		mD ^= rp[13]; \
		mE ^= rp[14]; \
		mF ^= rp[15]; \
		acc = buf[2]; \
		rp = &T512_16[acc][0]; \
		m0 ^= rp[0]; \
		m1 ^= rp[1]; \
		m2 ^= rp[2]; \
		m3 ^= rp[3]; \
		m4 ^= rp[4]; \
		m5 ^= rp[5]; \
		m6 ^= rp[6]; \
		m7 ^= rp[7]; \
		m8 ^= rp[8]; \
		m9 ^= rp[9]; \
		mA ^= rp[10]; \
		mB ^= rp[11]; \
		mC ^= rp[12]; \
		mD ^= rp[13]; \
		mE ^= rp[14]; \
		mF ^= rp[15]; \
		acc = buf[3]; \
		rp = &T512_24[acc][0]; \
		m0 ^= rp[0]; \
		m1 ^= rp[1]; \
		m2 ^= rp[2]; \
		m3 ^= rp[3]; \
		m4 ^= rp[4]; \
		m5 ^= rp[5]; \
		m6 ^= rp[6]; \
		m7 ^= rp[7]; \
		m8 ^= rp[8]; \
		m9 ^= rp[9]; \
		mA ^= rp[10]; \
		mB ^= rp[11]; \
		mC ^= rp[12]; \
		mD ^= rp[13]; \
		mE ^= rp[14]; \
		mF ^= rp[15]; \
		acc = buf[4]; \
		rp = &T512_32[acc][0]; \
		m0 ^= rp[0]; \
		m1 ^= rp[1]; \
		m2 ^= rp[2]; \
		m3 ^= rp[3]; \
		m4 ^= rp[4]; \
		m5 ^= rp[5]; \
		m6 ^= rp[6]; \
		m7 ^= rp[7]; \
		m8 ^= rp[8]; \
		m9 ^= rp[9]; \
		mA ^= rp[10]; \
		mB ^= rp[11]; \
		mC ^= rp[12]; \
		mD ^= rp[13]; \
		mE ^= rp[14]; \
		mF ^= rp[15]; \
		acc = buf[5]; \
		rp = &T512_40[acc][0]; \
		m0 ^= rp[0]; \
		m1 ^= rp[1]; \
		m2 ^= rp[2]; \
		m3 ^= rp[3]; \
		m4 ^= rp[4]; \
		m5 ^= rp[5]; \
		m6 ^= rp[6]; \
		m7 ^= rp[7]; \
		m8 ^= rp[8]; \
		m9 ^= rp[9]; \
		mA ^= rp[10]; \
		mB ^= rp[11]; \
		mC ^= rp[12]; \
		mD ^= rp[13]; \
		mE ^= rp[14]; \
		mF ^= rp[15]; \
		acc = buf[6]; \
		rp = &T512_48[acc][0]; \
		m0 ^= rp[0]; \
		m1 ^= rp[1]; \
		m2 ^= rp[2]; \
		m3 ^= rp[3]; \
		m4 ^= rp[4]; \
		m5 ^= rp[5]; \
		m6 ^= rp[6]; \
		m7 ^= rp[7]; \
		m8 ^= rp[8]; \
		m9 ^= rp[9]; \
		mA ^= rp[10]; \
		mB ^= rp[11]; \
		mC ^= rp[12]; \
		mD ^= rp[13]; \
		mE ^= rp[14]; \
		mF ^= rp[15]; \
		acc = buf[7]; \
		rp = &T512_56[acc][0]; \
		m0 ^= rp[0]; \
		m1 ^= rp[1]; \
		m2 ^= rp[2]; \
		m3 ^= rp[3]; \
		m4 ^= rp[4]; \
		m5 ^= rp[5]; \
		m6 ^= rp[6]; \
		m7 ^= rp[7]; \
		m8 ^= rp[8]; \
		m9 ^= rp[9]; \
		mA ^= rp[10]; \
		mB ^= rp[11]; \
		mC ^= rp[12]; \
		mD ^= rp[13]; \
		mE ^= rp[14]; \
		mF ^= rp[15]; \
	} while (0)

#define SBOX(a, b, c, d)   do { \
		sph_u32 t; \
		t = (a); \
		(a) &= (c); \
		(a) ^= (d); \
		(c) ^= (b); \
		(c) ^= (a); \
		(d) |= t; \
		(d) ^= (b); \
		t ^= (c); \
		(b) = (d); \
		(d) |= t; \
		(d) ^= (a); \
		(a) &= (b); \
		t ^= (a); \
		(b) ^= (d); \
		(b) ^= t; \
		(a) = (c); \
		(c) = (b); \
		(b) = (d); \
		(d) = SPH_T32(~t); \
	} while (0)

#define HAMSI_L(a, b, c, d)   do { \
		(a) = SPH_ROTL32(a, 13); \
		(c) = SPH_ROTL32(c, 3); \
		(b) ^= (a) ^ (c); \
		(d) ^= (c) ^ SPH_T32((a) << 3); \
		(b) = SPH_ROTL32(b, 1); \
		(d) = SPH_ROTL32(d, 7); \
		(a) ^= (b) ^ (d); \
		(c) ^= (d) ^ SPH_T32((b) << 7); \
		(a) = SPH_ROTL32(a, 5); \
		(c) = SPH_ROTL32(c, 22); \
	} while (0)

#define DECL_STATE_BIG \
	sph_u32 c0, c1, c2, c3, c4, c5, c6, c7; \
	sph_u32 c8, c9, cA, cB, cC, cD, cE, cF;

#define READ_STATE_BIG(sc)   do { \
		c0 = sc->h[0x0]; \
		c1 = sc->h[0x1]; \
		c2 = sc->h[0x2]; \
		c3 = sc->h[0x3]; \
		c4 = sc->h[0x4]; \
		c5 = sc->h[0x5]; \
		c6 = sc->h[0x6]; \
		c7 = sc->h[0x7]; \
		c8 = sc->h[0x8]; \
		c9 = sc->h[0x9]; \
		cA = sc->h[0xA]; \
		cB = sc->h[0xB]; \
		cC = sc->h[0xC]; \
		cD = sc->h[0xD]; \
		cE = sc->h[0xE]; \
		cF = sc->h[0xF]; \
	} while (0)

#define WRITE_STATE_BIG(sc)   do { \
		sc->h[0x0] = c0; \
		sc->h[0x1] = c1; \
		sc->h[0x2] = c2; \
		sc->h[0x3] = c3; \
		sc->h[0x4] = c4; \
		sc->h[0x5] = c5; \
		sc->h[0x6] = c6; \
		sc->h[0x7] = c7; \
		sc->h[0x8] = c8; \
		sc->h[0x9] = c9; \
		sc->h[0xA] = cA; \
		sc->h[0xB] = cB; \
		sc->h[0xC] = cC; \
		sc->h[0xD] = cD; \
		sc->h[0xE] = cE; \
		sc->h[0xF] = cF; \
	} while (0)

#define s00   m0
#define s01   m1
#define s02   c0
#define s03   c1
#define s04   m2
#define s05   m3
#define s06   c2
#define s07   c3
#define s08   c4
#define s09   c5
#define s0A   m4
#define s0B   m5
#define s0C   c6
#define s0D   c7
#define s0E   m6
#define s0F   m7
#define s10   m8
#define s11   m9
#define s12   c8
#define s13   c9
#define s14   mA
#define s15   mB
#define s16   cA
#define s17   cB
#define s18   cC
#define s19   cD
#define s1A   mC
#define s1B   mD
#define s1C   cE
#define s1D   cF
#define s1E   mE
#define s1F   mF

#define ROUND_BIG(rc, alpha)   do { \
		s00 ^= alpha[0x00]; \
		s01 ^= alpha[0x01] ^ (sph_u32)(rc); \
		s02 ^= alpha[0x02]; \
		s03 ^= alpha[0x03]; \
		s04 ^= alpha[0x04]; \
		s05 ^= alpha[0x05]; \
		s06 ^= alpha[0x06]; \
		s07 ^= alpha[0x07]; \
		s08 ^= alpha[0x08]; \
		s09 ^= alpha[0x09]; \
		s0A ^= alpha[0x0A]; \
		s0B ^= alpha[0x0B]; \
		s0C ^= alpha[0x0C]; \
		s0D ^= alpha[0x0D]; \
		s0E ^= alpha[0x0E]; \
		s0F ^= alpha[0x0F]; \
		s10 ^= alpha[0x10]; \
		s11 ^= alpha[0x11]; \
		s12 ^= alpha[0x12]; \
		s13 ^= alpha[0x13]; \
		s14 ^= alpha[0x14]; \
		s15 ^= alpha[0x15]; \
		s16 ^= alpha[0x16]; \
		s17 ^= alpha[0x17]; \
		s18 ^= alpha[0x18]; \
		s19 ^= alpha[0x19]; \
		s1A ^= alpha[0x1A]; \
		s1B ^= alpha[0x1B]; \
		s1C ^= alpha[0x1C]; \
		s1D ^= alpha[0x1D]; \
		s1E ^= alpha[0x1E]; \
		s1F ^= alpha[0x1F]; \
		SBOX(s00, s08, s10, s18); \
		SBOX(s01, s09, s11, s19); \
		SBOX(s02, s0A, s12, s1A); \
		SBOX(s03, s0B, s13, s1B); \
		SBOX(s04, s0C, s14, s1C); \
		SBOX(s05, s0D, s15, s1D); \
		SBOX(s06, s0E, s16, s1E); \
		SBOX(s07, s0F, s17, s1F); \
		HAMSI_L(s00, s09, s12, s1B); \
		HAMSI_L(s01, s0A, s13, s1C); \
		HAMSI_L(s02, s0B, s14, s1D); \
		HAMSI_L(s03, s0C, s15, s1E); \
		HAMSI_L(s04, s0D, s16, s1F); \
		HAMSI_L(s05, s0E, s17, s18); \
		HAMSI_L(s06, s0F, s10, s19); \
		HAMSI_L(s07, s08, s11, s1A); \
		HAMSI_L(s00, s02, s05, s07); \
		HAMSI_L(s10, s13, s15, s16); \
		HAMSI_L(s09, s0B, s0C, s0E); \
		HAMSI_L(s19, s1A, s1C, s1F); \
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
		cF = (sc->h[0xF] ^= s17); \
		cE = (sc->h[0xE] ^= s16); \
		cD = (sc->h[0xD] ^= s15); \
		cC = (sc->h[0xC] ^= s14); \
		cB = (sc->h[0xB] ^= s13); \
		cA = (sc->h[0xA] ^= s12); \
		c9 = (sc->h[0x9] ^= s11); \
		c8 = (sc->h[0x8] ^= s10); \
		c7 = (sc->h[0x7] ^= s07); \
		c6 = (sc->h[0x6] ^= s06); \
		c5 = (sc->h[0x5] ^= s05); \
		c4 = (sc->h[0x4] ^= s04); \
		c3 = (sc->h[0x3] ^= s03); \
		c2 = (sc->h[0x2] ^= s02); \
		c1 = (sc->h[0x1] ^= s01); \
		c0 = (sc->h[0x0] ^= s00); \
	} while (0)

static void
hamsi_big(sph_hamsi_big_context *sc, const unsigned char *buf, size_t num)
{
	DECL_STATE_BIG
#if !SPH_64
	sph_u32 tmp;
#endif

#if SPH_64
	sc->count += (sph_u64)num << 6;
#else
	tmp = SPH_T32((sph_u32)num << 6);
	sc->count_low = SPH_T32(sc->count_low + tmp);
	sc->count_high += (sph_u32)((num >> 13) >> 13);
	if (sc->count_low < tmp)
		sc->count_high ++;
#endif
	READ_STATE_BIG(sc);
	while (num -- > 0) {
		sph_u32 m0, m1, m2, m3, m4, m5, m6, m7;
		sph_u32 m8, m9, mA, mB, mC, mD, mE, mF;

		INPUT_BIG;
		P_BIG;
		T_BIG;
		buf += 8;
	}
	WRITE_STATE_BIG(sc);
}

static void
hamsi_big_final(sph_hamsi_big_context *sc, const unsigned char *buf)
{
	sph_u32 m0, m1, m2, m3, m4, m5, m6, m7;
	sph_u32 m8, m9, mA, mB, mC, mD, mE, mF;
	DECL_STATE_BIG

	READ_STATE_BIG(sc);
	INPUT_BIG;
	PF_BIG;
	T_BIG;
	WRITE_STATE_BIG(sc);
}

static void
hamsi_big_init(sph_hamsi_big_context *sc, const sph_u32 *iv)
{
	sc->partial_len = 0;
	memcpy(sc->h, iv, sizeof sc->h);
#if SPH_64
	sc->count = 0;
#else
	sc->count_high = sc->count_low = 0;
#endif
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
#if SPH_64
	sph_enc64be(pad, sc->count + (ptr << 3) + n);
#else
	sph_enc32be(pad, sc->count_high);
	sph_enc32be(pad + 4, sc->count_low + (ptr << 3) + n);
#endif
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
	hamsi_big_init(cc, HAMSI_IV512);
}

/* see sph_hamsi.h */
void
sph_hamsi512_addbits_and_close(void *cc, unsigned ub, unsigned n, void *dst)
{
	hamsi_big_close(cc, ub, n, dst, 16);
	hamsi_big_init(cc, HAMSI_IV512);
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

#undef DECL_STATE_BIG
#undef READ_STATE_BIG
#undef WRITE_STATE_BIG

#ifdef __cplusplus
}
#endif