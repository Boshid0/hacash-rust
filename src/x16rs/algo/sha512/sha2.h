

#ifndef SPH_SHA2_H__
#define SPH_SHA2_H__

#include <stddef.h>
#include "sph_types.h"

#ifdef __cplusplus
extern "C"{
#endif

/**
 * Output size (in bits) for SHA-512.
 */
#define SPH_SIZE_sha512   512

/**
 * This structure is a context for SHA-512 computations: it contains the
 * intermediate values and some data from the last entered block. Once
 * a SHA-512 computation has been performed, the context can be reused for
 * another computation.
 *
 * The contents of this structure are private. A running SHA-384 computation
 * can be cloned by copying the context (e.g. with a simple
 * <code>memcpy()</code>).
 */
typedef struct {
	unsigned char buf[128];    /* first field, for alignment */
	sph_u64 val[8];
	sph_u64 count;
} sph_sha512_context;

/**
 * Initialize a SHA-512 context. This process performs no memory allocation.
 *
 * @param cc   the SHA-512 context (pointer to
 *             a <code>sph_sha512_context</code>)
 */
void sph_sha512_init(sph_sha512_context *cc);

/**
 * Process some data bytes. It is acceptable that <code>len</code> is zero
 * (in which case this function does nothing).
 *
 * @param cc     the SHA-512 context
 * @param data   the input data
 * @param len    the input data length (in bytes)
 */
void sph_sha512_update(sph_sha512_context *cc, const void *data, size_t len);

/**
 * Terminate the current SHA-512 computation and output the result into the
 * provided buffer. The destination buffer must be wide enough to
 * accomodate the result (64 bytes). The context is automatically
 * reinitialized.
 *
 * @param cc    the SHA-512 context
 * @param dst   the destination buffer
 */
void sph_sha512_close(sph_sha512_context *cc, void *dst);

#endif

#ifdef __cplusplus
}
#endif

