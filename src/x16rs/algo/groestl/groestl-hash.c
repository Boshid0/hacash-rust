/* hash.c     Aug 2011
 *
 * Groestl implementation for different versions.
 * Author: Krystian Matusiewicz, Günther A. Roland, Martin Schläffer
 *
 * This code is placed in the public domain
 */

// Optimized for hash and data length that are integrals of v128_t 


#include <memory.h>
#include "simd-utils.h"

//#if defined(__AES__) || defined(__ARM_FEATURE_AES)

#include "groestl-intr-aes.h"

int groestl512( hashState_groestl* ctx, void* output, const void* input, uint64_t databitlen )
{

   int i;
   ctx->hashlen = 64;

   for ( i = 0; i < SIZE512; i++ )
   {
      ctx->chaining[i] = v128_zero;
      ctx->buffer[i]   = v128_zero;
   }
   ctx->chaining[ 6 ] = v128_set64( 0x0200000000000000, 0 );
   ctx->buf_ptr = 0;

   // --- update ---
   
   const int len = (int)databitlen / 128;
   const int hashlen_m128i = ctx->hashlen / 16;   // bytes to v128_t
   const int hash_offset = SIZE512 - hashlen_m128i;
   uint64_t blocks = len / SIZE512;
   v128_t* in = (v128_t*)input;
   
   // digest any full blocks, process directly from input 
   DECL_STATE_TF1024;
  
   for ( i = 0; i < blocks; i++ ) {
      v128_t *chaining = ctx->chaining;
      v128_t *message  = &in[ i * SIZE512 ];
      TF1024(chaining, message);
   }
   ctx->buf_ptr = blocks * SIZE512;

   // copy any remaining data to buffer, it may already contain data
   // from a previous update for a midstate precalc
   for ( i = 0; i < len % SIZE512; i++ )
       ctx->buffer[ i ] = in[ ctx->buf_ptr + i ];
   // use i as rem_ptr in final

   //--- final ---

   blocks++;      // adjust for final block

   if ( i == len -1 )
   {
       // only 128 bits left in buffer, all padding at once
      ctx->buffer[i] = v128_set64( blocks << 56, 0x80 );
   }
   else
   {
       // add first padding
       ctx->buffer[i] = v128_set64( 0, 0x80 );
       // add zero padding
       for ( i += 1; i < SIZE512 - 1; i++ )
           ctx->buffer[i] = v128_zero;

       // add length padding, second last byte is zero unless blocks > 255
       ctx->buffer[i] = v128_set64( blocks << 56, 0 ); 
   }

   // digest final padding block and do output transform
   v128_t *chaining = ctx->chaining;
   v128_t *message  = ctx->buffer;
   TF1024(chaining, message );
   DECL_STATE_OF1024;
   OF1024( chaining );

   // store hash result in output 
   for ( i = 0; i < hashlen_m128i; i++ )
      casti_v128( output, i ) = ctx->chaining[ hash_offset + i ];

   return 0;
}
   
int groestl_hash(unsigned char *out, const unsigned char *in, unsigned long long inlen)
{
    hashState_groestl ctx;
  return  groestl512(&ctx, (void *)out, in, inlen << 3);
}


//#endif   /// SSSE3 or NEON
