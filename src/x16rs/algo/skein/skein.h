

#ifndef SPH_SKEIN_H__
#define SPH_SKEIN_H__

#ifdef __cplusplus
extern "C"{
#endif

#include <stddef.h>
#include "sph_types.h"

/**
 * Output size (in bits) for Skein-512.
 */
#define SPH_SIZE_skein512   512

/**
 * This structure is a context for Skein computations (with a 384- or
 * 512-bit output): it contains the intermediate values and some data
 * from the last entered block. Once a Skein computation has been
 * performed, the context can be reused for another computation.
 *
 * The contents of this structure are private. A running Skein computation
 * can be cloned by copying the context (e.g. with a simple
 * <code>memcpy()</code>).
 */
typedef struct {
	unsigned char buf[64];    /* first field, for alignment */
	size_t ptr;
	sph_u64 h0, h1, h2, h3, h4, h5, h6, h7;
	sph_u64 bcount;
} sph_skein_big_context;

/**
 * Type for a Skein-512 context (identical to the common "big" context).
 */
typedef sph_skein_big_context sph_skein512_context;

/**
 * Initialize a Skein-512 context. This process performs no memory allocation.
 *
 * @param cc   the Skein-512 context (pointer to a
 *             <code>sph_skein512_context</code>)
 */
void sph_skein512_init(void *cc);

/**
 * Process some data bytes. It is acceptable that <code>len</code> is zero
 * (in which case this function does nothing).
 *
 * @param cc     the Skein-512 context
 * @param data   the input data
 * @param len    the input data length (in bytes)
 */
void sph_skein512(void *cc, const void *data, size_t len);

/**
 * Terminate the current Skein-512 computation and output the result into
 * the provided buffer. The destination buffer must be wide enough to
 * accomodate the result (64 bytes). The context is automatically
 * reinitialized.
 *
 * @param cc    the Skein-512 context
 * @param dst   the destination buffer
 */
void sph_skein512_close(void *cc, void *dst);

#ifdef __cplusplus
}
#endif

#endif
