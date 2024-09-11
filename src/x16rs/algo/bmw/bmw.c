
#include <stddef.h>
#include <string.h>
#include <limits.h>

#ifdef __cplusplus
extern "C"{
#endif

#include "bmw.h"

#ifdef _MSC_VER
#pragma warning (disable: 4146)
#endif

static const uint64_t BMW_IV512[] __attribute__((aligned(64)))= {
	SPH_C64(0x8081828384858687), SPH_C64(0x88898A8B8C8D8E8F),
	SPH_C64(0x9091929394959697), SPH_C64(0x98999A9B9C9D9E9F),
	SPH_C64(0xA0A1A2A3A4A5A6A7), SPH_C64(0xA8A9AAABACADAEAF),
	SPH_C64(0xB0B1B2B3B4B5B6B7), SPH_C64(0xB8B9BABBBCBDBEBF),
	SPH_C64(0xC0C1C2C3C4C5C6C7), SPH_C64(0xC8C9CACBCCCDCECF),
	SPH_C64(0xD0D1D2D3D4D5D6D7), SPH_C64(0xD8D9DADBDCDDDEDF),
	SPH_C64(0xE0E1E2E3E4E5E6E7), SPH_C64(0xE8E9EAEBECEDEEEF),
	SPH_C64(0xF0F1F2F3F4F5F6F7), SPH_C64(0xF8F9FAFBFCFDFEFF)
};

#define LPAR   (

#define I16_16    0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15
#define I16_17    1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16
#define I16_18    2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17
#define I16_19    3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18
#define I16_20    4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19
#define I16_21    5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20
#define I16_22    6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21
#define I16_23    7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22
#define I16_24    8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23
#define I16_25    9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24
#define I16_26   10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25
#define I16_27   11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26
#define I16_28   12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27
#define I16_29   13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28
#define I16_30   14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29
#define I16_31   15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30

#define M16_16    0,  1,  3,  4,  7, 10, 11
#define M16_17    1,  2,  4,  5,  8, 11, 12
#define M16_18    2,  3,  5,  6,  9, 12, 13
#define M16_19    3,  4,  6,  7, 10, 13, 14
#define M16_20    4,  5,  7,  8, 11, 14, 15
#define M16_21    5,  6,  8,  9, 12, 15, 16
#define M16_22    6,  7,  9, 10, 13,  0,  1
#define M16_23    7,  8, 10, 11, 14,  1,  2
#define M16_24    8,  9, 11, 12, 15,  2,  3
#define M16_25    9, 10, 12, 13,  0,  3,  4
#define M16_26   10, 11, 13, 14,  1,  4,  5
#define M16_27   11, 12, 14, 15,  2,  5,  6
#define M16_28   12, 13, 15, 16,  3,  6,  7
#define M16_29   13, 14,  0,  1,  4,  7,  8
#define M16_30   14, 15,  1,  2,  5,  8,  9
#define M16_31   15, 16,  2,  3,  6,  9, 10

#define ss0(x)    (((x) >> 1) ^ SPH_T32((x) << 3) \
                  ^ SPH_ROTL32(x,  4) ^ SPH_ROTL32(x, 19))
#define ss1(x)    (((x) >> 1) ^ SPH_T32((x) << 2) \
                  ^ SPH_ROTL32(x,  8) ^ SPH_ROTL32(x, 23))
#define ss2(x)    (((x) >> 2) ^ SPH_T32((x) << 1) \
                  ^ SPH_ROTL32(x, 12) ^ SPH_ROTL32(x, 25))
#define ss3(x)    (((x) >> 2) ^ SPH_T32((x) << 2) \
                  ^ SPH_ROTL32(x, 15) ^ SPH_ROTL32(x, 29))
#define ss4(x)    (((x) >> 1) ^ (x))
#define ss5(x)    (((x) >> 2) ^ (x))
#define rs1(x)    SPH_ROTL32(x,  3)
#define rs2(x)    SPH_ROTL32(x,  7)
#define rs3(x)    SPH_ROTL32(x, 13)
#define rs4(x)    SPH_ROTL32(x, 16)
#define rs5(x)    SPH_ROTL32(x, 19)
#define rs6(x)    SPH_ROTL32(x, 23)
#define rs7(x)    SPH_ROTL32(x, 27)

#define Ks(j)   SPH_T32((sph_u32)(j) * SPH_C32(0x05555555))
#define Kb(j)   SPH_T64((uint64_t)(j) * SPH_C64(0x0555555555555555))
#define Qb(j)   (qt[j])
#define Qs(j)   (qt[j])
#define M(x)    sph_dec64le_aligned(data + 8 * (x))
#define H(x)    (h[x])

#define sb0(x)    (((x) >> 1) ^ SPH_T64((x) << 3) \
                  ^ SPH_ROTL64(x,  4) ^ SPH_ROTL64(x, 37))
#define sb1(x)    (((x) >> 1) ^ SPH_T64((x) << 2) \
                  ^ SPH_ROTL64(x, 13) ^ SPH_ROTL64(x, 43))
#define sb2(x)    (((x) >> 2) ^ SPH_T64((x) << 1) \
                  ^ SPH_ROTL64(x, 19) ^ SPH_ROTL64(x, 53))
#define sb3(x)    (((x) >> 2) ^ SPH_T64((x) << 2) \
                  ^ SPH_ROTL64(x, 28) ^ SPH_ROTL64(x, 59))
#define sb4(x)    (((x) >> 1) ^ (x))
#define sb5(x)    (((x) >> 2) ^ (x))
#define rb1(x)    SPH_ROTL64(x,  5)
#define rb2(x)    SPH_ROTL64(x, 11)
#define rb3(x)    SPH_ROTL64(x, 27)
#define rb4(x)    SPH_ROTL64(x, 32)
#define rb5(x)    SPH_ROTL64(x, 37)
#define rb6(x)    SPH_ROTL64(x, 43)
#define rb7(x)    SPH_ROTL64(x, 53)

#define add_elt_b(mf, hf, j0m, j1m, j3m, j4m, j7m, j10m, j11m, j16) \
	(SPH_T64(SPH_ROTL64(mf(j0m), j1m) + SPH_ROTL64(mf(j3m), j4m) \
		- SPH_ROTL64(mf(j10m), j11m) + Kb(j16)) ^ hf(j7m))

#define expand1b_inner(qf, mf, hf, i16, \
		i0, i1, i2, i3, i4, i5, i6, i7, i8, \
		i9, i10, i11, i12, i13, i14, i15, \
		i0m, i1m, i3m, i4m, i7m, i10m, i11m) \
	SPH_T64(sb1(qf(i0)) + sb2(qf(i1)) + sb3(qf(i2)) + sb0(qf(i3)) \
		+ sb1(qf(i4)) + sb2(qf(i5)) + sb3(qf(i6)) + sb0(qf(i7)) \
		+ sb1(qf(i8)) + sb2(qf(i9)) + sb3(qf(i10)) + sb0(qf(i11)) \
		+ sb1(qf(i12)) + sb2(qf(i13)) + sb3(qf(i14)) + sb0(qf(i15)) \
		+ add_elt_b(mf, hf, i0m, i1m, i3m, i4m, i7m, i10m, i11m, i16))

#define expand1b(qf, mf, hf, i16) \
	expand1b_(qf, mf, hf, i16, I16_ ## i16, M16_ ## i16)
#define expand1b_(qf, mf, hf, i16, ix, iy) \
	expand1b_inner (qf, mf, hf, i16, ix, iy)

#define expand2b_inner(qf, mf, hf, i16, \
		i0, i1, i2, i3, i4, i5, i6, i7, i8, \
		i9, i10, i11, i12, i13, i14, i15, \
		i0m, i1m, i3m, i4m, i7m, i10m, i11m) \
	SPH_T64(qf(i0) + rb1(qf(i1)) + qf(i2) + rb2(qf(i3)) \
		+ qf(i4) + rb3(qf(i5)) + qf(i6) + rb4(qf(i7)) \
		+ qf(i8) + rb5(qf(i9)) + qf(i10) + rb6(qf(i11)) \
		+ qf(i12) + rb7(qf(i13)) + sb4(qf(i14)) + sb5(qf(i15)) \
		+ add_elt_b(mf, hf, i0m, i1m, i3m, i4m, i7m, i10m, i11m, i16))

#define expand2b(qf, mf, hf, i16) \
	expand2b_(qf, mf, hf, i16, I16_ ## i16, M16_ ## i16)
#define expand2b_(qf, mf, hf, i16, ix, iy) \
	expand2b_inner (qf, mf, hf, i16, ix, iy)

#define MAKE_W(tt, i0, op01, i1, op12, i2, op23, i3, op34, i4) \
	tt((M(i0) ^ H(i0)) op01 (M(i1) ^ H(i1)) op12 (M(i2) ^ H(i2)) \
	op23 (M(i3) ^ H(i3)) op34 (M(i4) ^ H(i4)))

#define Wb0    MAKE_W(SPH_T64,  5, -,  7, +, 10, +, 13, +, 14)
#define Wb1    MAKE_W(SPH_T64,  6, -,  8, +, 11, +, 14, -, 15)
#define Wb2    MAKE_W(SPH_T64,  0, +,  7, +,  9, -, 12, +, 15)
#define Wb3    MAKE_W(SPH_T64,  0, -,  1, +,  8, -, 10, +, 13)
#define Wb4    MAKE_W(SPH_T64,  1, +,  2, +,  9, -, 11, -, 14)
#define Wb5    MAKE_W(SPH_T64,  3, -,  2, +, 10, -, 12, +, 15)
#define Wb6    MAKE_W(SPH_T64,  4, -,  0, -,  3, -, 11, +, 13)
#define Wb7    MAKE_W(SPH_T64,  1, -,  4, -,  5, -, 12, -, 14)
#define Wb8    MAKE_W(SPH_T64,  2, -,  5, -,  6, +, 13, -, 15)
#define Wb9    MAKE_W(SPH_T64,  0, -,  3, +,  6, -,  7, +, 14)
#define Wb10   MAKE_W(SPH_T64,  8, -,  1, -,  4, -,  7, +, 15)
#define Wb11   MAKE_W(SPH_T64,  8, -,  0, -,  2, -,  5, +,  9)
#define Wb12   MAKE_W(SPH_T64,  1, +,  3, -,  6, -,  9, +, 10)
#define Wb13   MAKE_W(SPH_T64,  2, +,  4, +,  7, +, 10, +, 11)
#define Wb14   MAKE_W(SPH_T64,  3, -,  5, +,  8, -, 11, -, 12)
#define Wb15   MAKE_W(SPH_T64, 12, -,  4, -,  6, -,  9, +, 13)

static void
compress_big(const uint8_t *data, const uint64_t h[16], uint64_t dh[16])
{
	// __m512i vm[2];
	// __m512i vh[2];
	// __m512i vq[2];

	//FOLDb;
	uint64_t qt[32], xl, xh; 

    // vh[0] = _mm512_setr_epi64(h[ 1],h[ 2],h[ 3],h[ 4],h[ 5],h[ 6], h[ 7],h[ 8]);
	// vh[1] = _mm512_setr_epi64(h[ 9],h[10],h[11],h[12],h[13],h[14], h[15],h[ 0]);
	// vm[0] = _mm512_setr_epi64(sb0(Wb0),sb1(Wb1),sb2(Wb2),sb3(Wb3),sb4(Wb4),sb0(Wb5),sb1(Wb6),sb2(Wb7));
	// vm[1] = _mm512_setr_epi64(sb3(Wb8),sb4(Wb9),sb0(Wb10),sb1(Wb11),sb2(Wb12),sb3(Wb13),sb4(Wb14),sb0(Wb15));

    // vq[0] = _mm512_and_epi64(_mm512_add_epi64(vm[0],vh[0]),_mm512_set1_epi64(0xFFFFFFFFFFFFFFFF));
	// vq[1] = _mm512_and_epi64(_mm512_add_epi64(vm[1],vh[1]),_mm512_set1_epi64(0xFFFFFFFFFFFFFFFF));

	// _mm512_storeu_epi64(qt, vq[0]);     
	// _mm512_storeu_epi64(qt+8, vq[1]);  

	qt[ 0] = SPH_T64(sb0(Wb0 ) + h[ 1]);
	qt[ 1] = SPH_T64(sb1(Wb1 ) + h[ 2]);
	qt[ 2] = SPH_T64(sb2(Wb2 ) + h[ 3]);
	qt[ 3] = SPH_T64(sb3(Wb3 ) + h[ 4]);
	qt[ 4] = SPH_T64(sb4(Wb4 ) + h[ 5]);
	qt[ 5] = SPH_T64(sb0(Wb5 ) + h[ 6]);
	qt[ 6] = SPH_T64(sb1(Wb6 ) + h[ 7]);
	qt[ 7] = SPH_T64(sb2(Wb7 ) + h[ 8]);
	qt[ 8] = SPH_T64(sb3(Wb8 ) + h[ 9]);
	qt[ 9] = SPH_T64(sb4(Wb9 ) + h[10]);
	qt[10] = SPH_T64(sb0(Wb10) + h[11]);
	qt[11] = SPH_T64(sb1(Wb11) + h[12]);
	qt[12] = SPH_T64(sb2(Wb12) + h[13]);
	qt[13] = SPH_T64(sb3(Wb13) + h[14]);
	qt[14] = SPH_T64(sb4(Wb14) + h[15]);
	qt[15] = SPH_T64(sb0(Wb15) + h[ 0]);

	qt[16] = expand1b(Qb, M, H, 16);
	qt[17] = expand1b(Qb, M, H, 17);
	qt[18] = expand2b(Qb, M, H, 18);
	qt[19] = expand2b(Qb, M, H, 19);
	qt[20] = expand2b(Qb, M, H, 20);
	qt[21] = expand2b(Qb, M, H, 21);
	qt[22] = expand2b(Qb, M, H, 22);
	qt[23] = expand2b(Qb, M, H, 23);
	qt[24] = expand2b(Qb, M, H, 24);
	qt[25] = expand2b(Qb, M, H, 25);
	qt[26] = expand2b(Qb, M, H, 26);
	qt[27] = expand2b(Qb, M, H, 27);
	qt[28] = expand2b(Qb, M, H, 28);
	qt[29] = expand2b(Qb, M, H, 29);
	qt[30] = expand2b(Qb, M, H, 30);
	qt[31] = expand2b(Qb, M, H, 31);

	xl = qt[16] ^ qt[17] ^ qt[18] ^ qt[19] ^ qt[20] ^ qt[21] ^ qt[22] ^ qt[23]; 
	xh = xl ^ qt[24] ^ qt[25] ^ qt[26] ^ qt[27] ^ qt[28] ^ qt[29] ^ qt[30] ^ qt[31]; 
	dh[0] = SPH_T64(((xh <<  5) ^ (qt[16] >>  5) ^ M( 0)) + (xl ^ qt[24] ^ qt[ 0])); 
	dh[1] = SPH_T64(((xh >>  7) ^ (qt[17] <<  8) ^ M( 1)) + (xl ^ qt[25] ^ qt[ 1])); 
	dh[2] = SPH_T64(((xh >>  5) ^ (qt[18] <<  5) ^ M( 2)) + (xl ^ qt[26] ^ qt[ 2])); 
	dh[3] = SPH_T64(((xh >>  1) ^ (qt[19] <<  5) ^ M( 3)) + (xl ^ qt[27] ^ qt[ 3])); 
	dh[4] = SPH_T64(((xh >>  3) ^ (qt[20] <<  0) ^ M( 4)) + (xl ^ qt[28] ^ qt[ 4])); 
	dh[5] = SPH_T64(((xh <<  6) ^ (qt[21] >>  6) ^ M( 5)) + (xl ^ qt[29] ^ qt[ 5])); 
	dh[6] = SPH_T64(((xh >>  4) ^ (qt[22] <<  6) ^ M( 6)) + (xl ^ qt[30] ^ qt[ 6])); 
	dh[7] = SPH_T64(((xh >> 11) ^ (qt[23] <<  2) ^ M( 7)) + (xl ^ qt[31] ^ qt[ 7])); 

	dh[8] = SPH_T64(SPH_ROTL64(dh[4],  9) + (xh ^ qt[24] ^ M( 8)) + ((xl << 8) ^ qt[23] ^ qt[ 8])); 
	dh[9] = SPH_T64(SPH_ROTL64(dh[5], 10) + (xh ^ qt[25] ^ M( 9)) + ((xl >> 6) ^ qt[16] ^ qt[ 9])); 
	dh[10] = SPH_T64(SPH_ROTL64(dh[6], 11) + (xh ^ qt[26] ^ M(10)) + ((xl << 6) ^ qt[17] ^ qt[10])); 
	dh[11] = SPH_T64(SPH_ROTL64(dh[7], 12) + (xh ^ qt[27] ^ M(11)) + ((xl << 4) ^ qt[18] ^ qt[11])); 
	dh[12] = SPH_T64(SPH_ROTL64(dh[0], 13) + (xh ^ qt[28] ^ M(12)) + ((xl >> 3) ^ qt[19] ^ qt[12])); 
	dh[13] = SPH_T64(SPH_ROTL64(dh[1], 14) + (xh ^ qt[29] ^ M(13)) + ((xl >> 4) ^ qt[20] ^ qt[13])); 
	dh[14] = SPH_T64(SPH_ROTL64(dh[2], 15) + (xh ^ qt[30] ^ M(14)) + ((xl >> 7) ^ qt[21] ^ qt[14])); 
	dh[15] = SPH_T64(SPH_ROTL64(dh[3], 16) + (xh ^ qt[31] ^ M(15)) + ((xl >> 2) ^ qt[22] ^ qt[15])); 

}

static const uint64_t final_b[16] = {
	SPH_C64(0xaaaaaaaaaaaaaaa0), SPH_C64(0xaaaaaaaaaaaaaaa1),
	SPH_C64(0xaaaaaaaaaaaaaaa2), SPH_C64(0xaaaaaaaaaaaaaaa3),
	SPH_C64(0xaaaaaaaaaaaaaaa4), SPH_C64(0xaaaaaaaaaaaaaaa5),
	SPH_C64(0xaaaaaaaaaaaaaaa6), SPH_C64(0xaaaaaaaaaaaaaaa7),
	SPH_C64(0xaaaaaaaaaaaaaaa8), SPH_C64(0xaaaaaaaaaaaaaaa9),
	SPH_C64(0xaaaaaaaaaaaaaaaa), SPH_C64(0xaaaaaaaaaaaaaaab),
	SPH_C64(0xaaaaaaaaaaaaaaac), SPH_C64(0xaaaaaaaaaaaaaaad),
	SPH_C64(0xaaaaaaaaaaaaaaae), SPH_C64(0xaaaaaaaaaaaaaaaf)
};

static void
bmw64_init(sph_bmw_big_context *sc, const uint64_t *iv)
{
	memcpy(sc->H, iv, sizeof sc->H);
	sc->ptr = 0;
	sc->bit_count = 0;
}

static void
bmw64(sph_bmw_big_context *sc, const void *data, size_t len)
{
	uint8_t *buf;
	size_t ptr;
	uint64_t htmp[16];
	uint64_t *h1, *h2;

	sc->bit_count += (uint64_t)len << 3;
	buf = sc->buf;
	ptr = sc->ptr;
	h1 = sc->H;
	h2 = htmp;
	while (len > 0) {
		size_t clen;

		clen = (sizeof sc->buf) - ptr;
		if (clen > len)
			clen = len;
		memcpy(buf + ptr, data, clen);
		data = (const uint8_t *)data + clen;
		len -= clen;
		ptr += clen;
		if (ptr == sizeof sc->buf) {
			uint64_t *ht;

			compress_big(buf, h1, h2);
			ht = h1;
			h1 = h2;
			h2 = ht;
			ptr = 0;
		}
	}
	sc->ptr = ptr;
	if (h1 != sc->H)
		memcpy(sc->H, h1, sizeof sc->H);
}

static void
bmw64_close(sph_bmw_big_context *sc, unsigned ub, unsigned n,
	void *dst, size_t out_size_w64)
{
	uint8_t *buf, *out;
	size_t ptr, u, v;
	unsigned z;
	uint64_t h1[16], h2[16], *h;

	buf = sc->buf;
	ptr = sc->ptr;
	z = 0x80 >> n;
	buf[ptr ++] = ((ub & -z) | z) & 0xFF;
	h = sc->H;
	if (ptr > (sizeof sc->buf) - 8) {
		memset(buf + ptr, 0, (sizeof sc->buf) - ptr);
		compress_big(buf, h, h1);
		ptr = 0;
		h = h1;
	}
	memset(buf + ptr, 0, (sizeof sc->buf) - 8 - ptr);
	sph_enc64le_aligned(buf + (sizeof sc->buf) - 8,
		SPH_T64(sc->bit_count + n));
	compress_big(buf, h, h2);
	for (u = 0; u < 16; u ++)
		sph_enc64le_aligned(buf + 8 * u, h2[u]);
	compress_big(buf, final_b, h1);
	out = dst;
	for (u = 0, v = 16 - out_size_w64; u < out_size_w64; u ++, v ++)
		sph_enc64le(out + 8 * u, h1[v]);
}

/* see sph_bmw.h */
void
sph_bmw512_init(void *cc)
{
	bmw64_init(cc, BMW_IV512);
}

/* see sph_bmw.h */
void
sph_bmw512(void *cc, const void *data, size_t len)
{
	bmw64(cc, data, len);
}

/* see sph_bmw.h */
void
sph_bmw512_close(void *cc, void *dst)
{
	sph_bmw512_addbits_and_close(cc, 0, 0, dst);
}

/* see sph_bmw.h */
void
sph_bmw512_addbits_and_close(void *cc, unsigned ub, unsigned n, void *dst)
{
	bmw64_close(cc, ub, n, dst, 8);
	sph_bmw512_init(cc);
}

#undef M
#undef H

#ifdef __cplusplus
}
#endif
