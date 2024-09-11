

#ifndef SPH_WHIRLPOOL_H__
#define SPH_WHIRLPOOL_H__

#include <stddef.h>
#include "sph_types.h"

#if SPH_64

/**
 * Output size (in bits) for WHIRLPOOL.
 */
#define SPH_SIZE_whirlpool   512

/**
 * Output size (in bits) for WHIRLPOOL-0.
 */
#define SPH_SIZE_whirlpool0   512

/**
 * Output size (in bits) for WHIRLPOOL-1.
 */
#define SPH_SIZE_whirlpool1   512

/**
 * This structure is a context for WHIRLPOOL computations: it contains the
 * intermediate values and some data from the last entered block. Once
 * a WHIRLPOOL computation has been performed, the context can be reused for
 * another computation.
 *
 * The contents of this structure are private. A running WHIRLPOOL computation
 * can be cloned by copying the context (e.g. with a simple
 * <code>memcpy()</code>).
 */
typedef struct {
#ifndef DOXYGEN_IGNORE
	unsigned char buf[64];    /* first field, for alignment */
	sph_u64 state[8];
#if SPH_64
	sph_u64 count;
#else
	sph_u32 count_high, count_low;
#endif
#endif
} sph_whirlpool_context;

/**
 * Initialize a WHIRLPOOL context. This process performs no memory allocation.
 *
 * @param cc   the WHIRLPOOL context (pointer to a
 *             <code>sph_whirlpool_context</code>)
 */
void sph_whirlpool_init(void *cc);

/**
 * Process some data bytes. It is acceptable that <code>len</code> is zero
 * (in which case this function does nothing). This function applies the
 * plain WHIRLPOOL algorithm.
 *
 * @param cc     the WHIRLPOOL context
 * @param data   the input data
 * @param len    the input data length (in bytes)
 */
void sph_whirlpool(void *cc, const void *data, size_t len);

/**
 * Terminate the current WHIRLPOOL computation and output the result into the
 * provided buffer. The destination buffer must be wide enough to
 * accomodate the result (64 bytes). The context is automatically
 * reinitialized.
 *
 * @param cc    the WHIRLPOOL context
 * @param dst   the destination buffer
 */
void sph_whirlpool_close(void *cc, void *dst);

/**
 * WHIRLPOOL-0 uses the same structure than plain WHIRLPOOL.
 */
typedef sph_whirlpool_context sph_whirlpool0_context;

#ifdef DOXYGEN_IGNORE
/**
 * Initialize a WHIRLPOOL-0 context. This function is identical to
 * <code>sph_whirlpool_init()</code>.
 *
 * @param cc   the WHIRLPOOL context (pointer to a
 *             <code>sph_whirlpool0_context</code>)
 */
void sph_whirlpool0_init(void *cc);
#endif

#ifndef DOXYGEN_IGNORE
#define sph_whirlpool0_init   sph_whirlpool_init
#endif

/**
 * Process some data bytes. It is acceptable that <code>len</code> is zero
 * (in which case this function does nothing). This function applies the
 * WHIRLPOOL-0 algorithm.
 *
 * @param cc     the WHIRLPOOL context
 * @param data   the input data
 * @param len    the input data length (in bytes)
 */
void sph_whirlpool0(void *cc, const void *data, size_t len);

/**
 * Terminate the current WHIRLPOOL-0 computation and output the result into the
 * provided buffer. The destination buffer must be wide enough to
 * accomodate the result (64 bytes). The context is automatically
 * reinitialized.
 *
 * @param cc    the WHIRLPOOL-0 context
 * @param dst   the destination buffer
 */
void sph_whirlpool0_close(void *cc, void *dst);

/**
 * WHIRLPOOL-1 uses the same structure than plain WHIRLPOOL.
 */
typedef sph_whirlpool_context sph_whirlpool1_context;

#ifdef DOXYGEN_IGNORE
/**
 * Initialize a WHIRLPOOL-1 context. This function is identical to
 * <code>sph_whirlpool_init()</code>.
 *
 * @param cc   the WHIRLPOOL context (pointer to a
 *             <code>sph_whirlpool1_context</code>)
 */
void sph_whirlpool1_init(void *cc);
#endif

#ifndef DOXYGEN_IGNORE
#define sph_whirlpool1_init   sph_whirlpool_init
#endif

/**
 * Process some data bytes. It is acceptable that <code>len</code> is zero
 * (in which case this function does nothing). This function applies the
 * WHIRLPOOL-1 algorithm.
 *
 * @param cc     the WHIRLPOOL context
 * @param data   the input data
 * @param len    the input data length (in bytes)
 */
void sph_whirlpool1(void *cc, const void *data, size_t len);

/**
 * Terminate the current WHIRLPOOL-1 computation and output the result into the
 * provided buffer. The destination buffer must be wide enough to
 * accomodate the result (64 bytes). The context is automatically
 * reinitialized.
 *
 * @param cc    the WHIRLPOOL-1 context
 * @param dst   the destination buffer
 */
void sph_whirlpool1_close(void *cc, void *dst);

#endif

#endif