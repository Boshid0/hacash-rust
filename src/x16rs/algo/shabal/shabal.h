

#ifndef SPH_SHABAL_H__
#define SPH_SHABAL_H__

#include <stddef.h>
#include "sph_types.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * Output size (in bits) for Shabal-512.
 */
#define SPH_SIZE_shabal512   512

/**
 * This structure is a context for Shabal computations: it contains the
 * intermediate values and some data from the last entered block. Once
 * a Shabal computation has been performed, the context can be reused for
 * another computation.
 *
 * The contents of this structure are private. A running Shabal computation
 * can be cloned by copying the context (e.g. with a simple
 * <code>memcpy()</code>).
 */
typedef struct {
	unsigned char buf[64];    /* first field, for alignment */
	size_t ptr;
	sph_u32 A[12], B[16], C[16];
	sph_u32 Whigh, Wlow;
} sph_shabal_context;

/**
 * Type for a Shabal-512 context (identical to the common context).
 */
typedef sph_shabal_context sph_shabal512_context;

/**
 * Initialize a Shabal-512 context. This process performs no memory allocation.
 *
 * @param cc   the Shabal-512 context (pointer to a
 *             <code>sph_shabal512_context</code>)
 */
void sph_shabal512_init(void *cc);

/**
 * Process some data bytes. It is acceptable that <code>len</code> is zero
 * (in which case this function does nothing).
 *
 * @param cc     the Shabal-512 context
 * @param data   the input data
 * @param len    the input data length (in bytes)
 */
void sph_shabal512(void *cc, const void *data, size_t len);

/**
 * Terminate the current Shabal-512 computation and output the result into
 * the provided buffer. The destination buffer must be wide enough to
 * accomodate the result (64 bytes). The context is automatically
 * reinitialized.
 *
 * @param cc    the Shabal-512 context
 * @param dst   the destination buffer
 */
void sph_shabal512_close(void *cc, void *dst);

#ifdef __cplusplus
}
#endif

#endif
