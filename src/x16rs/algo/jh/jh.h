
#ifndef SPH_JH_H__
#define SPH_JH_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <stddef.h>
#include "sph_types.h"

/**
 * Output size (in bits) for JH-224.
 */
#define SPH_SIZE_jh224   224

/**
 * Output size (in bits) for JH-256.
 */
#define SPH_SIZE_jh256   256

/**
 * Output size (in bits) for JH-384.
 */
#define SPH_SIZE_jh384   384

/**
 * Output size (in bits) for JH-512.
 */
#define SPH_SIZE_jh512   512

/**
 * This structure is a context for JH computations: it contains the
 * intermediate values and some data from the last entered block. Once
 * a JH computation has been performed, the context can be reused for
 * another computation.
 *
 * The contents of this structure are private. A running JH computation
 * can be cloned by copying the context (e.g. with a simple
 * <code>memcpy()</code>).
 */
typedef struct {
#ifndef DOXYGEN_IGNORE
	unsigned char buf[64];    /* first field, for alignment */
	size_t ptr;
	union {
#if SPH_64
		sph_u64 wide[16];
#endif
		sph_u32 narrow[32];
	} H;
#if SPH_64
	sph_u64 block_count;
#else
	sph_u32 block_count_high, block_count_low;
#endif
#endif
} sph_jh_context;

/**
 * Type for a JH-224 context (identical to the common context).
 */
typedef sph_jh_context sph_jh224_context;

/**
 * Type for a JH-256 context (identical to the common context).
 */
typedef sph_jh_context sph_jh256_context;

/**
 * Type for a JH-384 context (identical to the common context).
 */
typedef sph_jh_context sph_jh384_context;

/**
 * Type for a JH-512 context (identical to the common context).
 */
typedef sph_jh_context sph_jh512_context;

/**
 * Initialize a JH-224 context. This process performs no memory allocation.
 *
 * @param cc   the JH-224 context (pointer to a
 *             <code>sph_jh224_context</code>)
 */
void sph_jh224_init(void *cc);

/**
 * Process some data bytes. It is acceptable that <code>len</code> is zero
 * (in which case this function does nothing).
 *
 * @param cc     the JH-224 context
 * @param data   the input data
 * @param len    the input data length (in bytes)
 */
void sph_jh224(void *cc, const void *data, size_t len);

/**
 * Terminate the current JH-224 computation and output the result into
 * the provided buffer. The destination buffer must be wide enough to
 * accomodate the result (28 bytes). The context is automatically
 * reinitialized.
 *
 * @param cc    the JH-224 context
 * @param dst   the destination buffer
 */
void sph_jh224_close(void *cc, void *dst);

/**
 * Add a few additional bits (0 to 7) to the current computation, then
 * terminate it and output the result in the provided buffer, which must
 * be wide enough to accomodate the result (28 bytes). If bit number i
 * in <code>ub</code> has value 2^i, then the extra bits are those
 * numbered 7 downto 8-n (this is the big-endian convention at the byte
 * level). The context is automatically reinitialized.
 *
 * @param cc    the JH-224 context
 * @param ub    the extra bits
 * @param n     the number of extra bits (0 to 7)
 * @param dst   the destination buffer
 */
void sph_jh224_addbits_and_close(
	void *cc, unsigned ub, unsigned n, void *dst);

/**
 * Initialize a JH-256 context. This process performs no memory allocation.
 *
 * @param cc   the JH-256 context (pointer to a
 *             <code>sph_jh256_context</code>)
 */
void sph_jh256_init(void *cc);

/**
 * Process some data bytes. It is acceptable that <code>len</code> is zero
 * (in which case this function does nothing).
 *
 * @param cc     the JH-256 context
 * @param data   the input data
 * @param len    the input data length (in bytes)
 */
void sph_jh256(void *cc, const void *data, size_t len);

/**
 * Terminate the current JH-256 computation and output the result into
 * the provided buffer. The destination buffer must be wide enough to
 * accomodate the result (32 bytes). The context is automatically
 * reinitialized.
 *
 * @param cc    the JH-256 context
 * @param dst   the destination buffer
 */
void sph_jh256_close(void *cc, void *dst);

/**
 * Add a few additional bits (0 to 7) to the current computation, then
 * terminate it and output the result in the provided buffer, which must
 * be wide enough to accomodate the result (32 bytes). If bit number i
 * in <code>ub</code> has value 2^i, then the extra bits are those
 * numbered 7 downto 8-n (this is the big-endian convention at the byte
 * level). The context is automatically reinitialized.
 *
 * @param cc    the JH-256 context
 * @param ub    the extra bits
 * @param n     the number of extra bits (0 to 7)
 * @param dst   the destination buffer
 */
void sph_jh256_addbits_and_close(
	void *cc, unsigned ub, unsigned n, void *dst);

/**
 * Initialize a JH-384 context. This process performs no memory allocation.
 *
 * @param cc   the JH-384 context (pointer to a
 *             <code>sph_jh384_context</code>)
 */
void sph_jh384_init(void *cc);

/**
 * Process some data bytes. It is acceptable that <code>len</code> is zero
 * (in which case this function does nothing).
 *
 * @param cc     the JH-384 context
 * @param data   the input data
 * @param len    the input data length (in bytes)
 */
void sph_jh384(void *cc, const void *data, size_t len);

/**
 * Terminate the current JH-384 computation and output the result into
 * the provided buffer. The destination buffer must be wide enough to
 * accomodate the result (48 bytes). The context is automatically
 * reinitialized.
 *
 * @param cc    the JH-384 context
 * @param dst   the destination buffer
 */
void sph_jh384_close(void *cc, void *dst);

/**
 * Add a few additional bits (0 to 7) to the current computation, then
 * terminate it and output the result in the provided buffer, which must
 * be wide enough to accomodate the result (48 bytes). If bit number i
 * in <code>ub</code> has value 2^i, then the extra bits are those
 * numbered 7 downto 8-n (this is the big-endian convention at the byte
 * level). The context is automatically reinitialized.
 *
 * @param cc    the JH-384 context
 * @param ub    the extra bits
 * @param n     the number of extra bits (0 to 7)
 * @param dst   the destination buffer
 */
void sph_jh384_addbits_and_close(
	void *cc, unsigned ub, unsigned n, void *dst);

/**
 * Initialize a JH-512 context. This process performs no memory allocation.
 *
 * @param cc   the JH-512 context (pointer to a
 *             <code>sph_jh512_context</code>)
 */
void sph_jh512_init(void *cc);

/**
 * Process some data bytes. It is acceptable that <code>len</code> is zero
 * (in which case this function does nothing).
 *
 * @param cc     the JH-512 context
 * @param data   the input data
 * @param len    the input data length (in bytes)
 */
void sph_jh512(void *cc, const void *data, size_t len);

/**
 * Terminate the current JH-512 computation and output the result into
 * the provided buffer. The destination buffer must be wide enough to
 * accomodate the result (64 bytes). The context is automatically
 * reinitialized.
 *
 * @param cc    the JH-512 context
 * @param dst   the destination buffer
 */
void sph_jh512_close(void *cc, void *dst);

/**
 * Add a few additional bits (0 to 7) to the current computation, then
 * terminate it and output the result in the provided buffer, which must
 * be wide enough to accomodate the result (64 bytes). If bit number i
 * in <code>ub</code> has value 2^i, then the extra bits are those
 * numbered 7 downto 8-n (this is the big-endian convention at the byte
 * level). The context is automatically reinitialized.
 *
 * @param cc    the JH-512 context
 * @param ub    the extra bits
 * @param n     the number of extra bits (0 to 7)
 * @param dst   the destination buffer
 */
void sph_jh512_addbits_and_close(
	void *cc, unsigned ub, unsigned n, void *dst);


#define jh512_context sph_jh512_context
#define jh512_init    sph_jh512_init
#define jh512_update  sph_jh512
#define jh512_close   sph_jh512_close

#ifdef __cplusplus
}
#endif

#endif
