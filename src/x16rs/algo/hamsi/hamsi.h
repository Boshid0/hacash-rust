
#ifndef SPH_HAMSI_H__
#define SPH_HAMSI_H__

#include <stddef.h>
#include <stdint.h>
#include <sph_types.h>

#ifdef __cplusplus
extern "C"{
#endif

/**
 * Output size (in bits) for Hamsi-384.
 */
#define SPH_SIZE_hamsi384   384

/**
 * Output size (in bits) for Hamsi-512.
 */
#define SPH_SIZE_hamsi512   512

/**
 * This structure is a context for Hamsi-384 and Hamsi-512 computations:
 * it contains the intermediate values and some data from the last
 * entered block. Once a Hamsi computation has been performed, the
 * context can be reused for another computation.
 *
 * The contents of this structure are private. A running Hamsi
 * computation can be cloned by copying the context (e.g. with a simple
 * <code>memcpy()</code>).
 */
typedef struct {
	uint8_t partial[8];
	size_t partial_len;
	uint32_t h[16];
	uint64_t count;
} sph_hamsi_big_context;

/**
 * This structure is a context for Hamsi-512 computations. It is
 * identical to the common <code>sph_hamsi_small_context</code>.
 */
typedef sph_hamsi_big_context sph_hamsi512_context;

/**
 * Initialize a Hamsi-512 context. This process performs no memory allocation.
 *
 * @param cc   the Hamsi-512 context (pointer to a
 *             <code>sph_hamsi512_context</code>)
 */
void sph_hamsi512_init(void *cc);

/**
 * Process some data bytes. It is acceptable that <code>len</code> is zero
 * (in which case this function does nothing).
 *
 * @param cc     the Hamsi-512 context
 * @param data   the input data
 * @param len    the input data length (in bytes)
 */
void sph_hamsi512(void *cc, const void *data, size_t len);

/**
 * Terminate the current Hamsi-512 computation and output the result into
 * the provided buffer. The destination buffer must be wide enough to
 * accomodate the result (64 bytes). The context is automatically
 * reinitialized.
 *
 * @param cc    the Hamsi-512 context
 * @param dst   the destination buffer
 */
void sph_hamsi512_close(void *cc, void *dst);

/**
 * Add a few additional bits (0 to 7) to the current computation, then
 * terminate it and output the result in the provided buffer, which must
 * be wide enough to accomodate the result (64 bytes). If bit number i
 * in <code>ub</code> has value 2^i, then the extra bits are those
 * numbered 7 downto 8-n (this is the big-endian convention at the byte
 * level). The context is automatically reinitialized.
 *
 * @param cc    the Hamsi-512 context
 * @param ub    the extra bits
 * @param n     the number of extra bits (0 to 7)
 * @param dst   the destination buffer
 */
void sph_hamsi512_addbits_and_close(
	void *cc, unsigned ub, unsigned n, void *dst);

#define hamsi512_context sph_hamsi512_context
#define hamsi512_init    sph_hamsi512_init
#define hamsi512_update  sph_hamsi512
#define hamsi512_close   sph_hamsi512_close

#ifdef __cplusplus
}
#endif

#endif