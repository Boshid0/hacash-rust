

#ifndef SPH_KECCAK_H__
#define SPH_KECCAK_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <stddef.h>
#include "sph_types.h"

/**
 * Output size (in bits) for Keccak-512.
 */
#define SPH_SIZE_keccak512   512

/**
 * This structure is a context for Keccak computations: it contains the
 * intermediate values and some data from the last entered block. Once a
 * Keccak computation has been performed, the context can be reused for
 * another computation.
 *
 * The contents of this structure are private. A running Keccak computation
 * can be cloned by copying the context (e.g. with a simple
 * <code>memcpy()</code>).
 */
typedef struct {
	unsigned char buf[144];    /* first field, for alignment */
	size_t ptr, lim;
	union {
		sph_u64 wide[25];
		sph_u32 narrow[50];
	} u;
} sph_keccak_context;

/**
 * Type for a Keccak-512 context (identical to the common context).
 */
typedef sph_keccak_context sph_keccak512_context;

/**
 * Initialize a Keccak-512 context. This process performs no memory allocation.
 *
 * @param cc   the Keccak-512 context (pointer to a
 *             <code>sph_keccak512_context</code>)
 */
void sph_keccak512_init(void *cc);

/**
 * Process some data bytes. It is acceptable that <code>len</code> is zero
 * (in which case this function does nothing).
 *
 * @param cc     the Keccak-512 context
 * @param data   the input data
 * @param len    the input data length (in bytes)
 */
void sph_keccak512(void *cc, const void *data, size_t len);

/**
 * Terminate the current Keccak-512 computation and output the result into
 * the provided buffer. The destination buffer must be wide enough to
 * accomodate the result (64 bytes). The context is automatically
 * reinitialized.
 *
 * @param cc    the Keccak-512 context
 * @param dst   the destination buffer
 */
void sph_keccak512_close(void *cc, void *dst);

/**
 * Add a few additional bits (0 to 7) to the current computation, then
 * terminate it and output the result in the provided buffer, which must
 * be wide enough to accomodate the result (64 bytes). If bit number i
 * in <code>ub</code> has value 2^i, then the extra bits are those
 * numbered 7 downto 8-n (this is the big-endian convention at the byte
 * level). The context is automatically reinitialized.
 *
 * @param cc    the Keccak-512 context
 * @param ub    the extra bits
 * @param n     the number of extra bits (0 to 7)
 * @param dst   the destination buffer
 */
void sph_keccak512_addbits_and_close(
	void *cc, unsigned ub, unsigned n, void *dst);



#ifdef __cplusplus
}
#endif

#endif
