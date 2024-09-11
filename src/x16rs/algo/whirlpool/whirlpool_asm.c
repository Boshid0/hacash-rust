#include <stdint.h>
#include <stddef.h>

extern void whirlpool_compress(uint8_t state[64], const uint8_t block[64]);

#define BLOCK_LEN 64 // In bytes
#define STATE_LEN 64 // In bytes

typedef struct
{
    uint8_t buf[BLOCK_LEN];   /* buffer of data to hash */
    uint8_t state[STATE_LEN]; /* the hashing state */
    uint32_t count;
    uint64_t total_bytes;
} whirlpool_context;

void whirlpool_init(whirlpool_context *ctx)
{
    memset(ctx->state, 0, sizeof ctx->state);
    memset(ctx->buf, 0, sizeof ctx->buf);
    ctx->count = 0;
    ctx->total_bytes = 0;
}

void whirlpool_update(whirlpool_context *ctx, const void *data, int len)
{
    ctx->total_bytes += len;
    uint32_t read = 0;
    if (ctx->count > 0)
    {
        uint32_t end = ctx->count + len;
		if (end > BLOCK_LEN)
			end = BLOCK_LEN;
        memcpy(ctx->buf + ctx->count, data, end-ctx->count);
		read += (end-ctx->count);
		
        if (end == BLOCK_LEN)
        {
            whirlpool_compress(ctx->state, ctx->buf);
            ctx->count = 0;
        }
        else
        {
            ctx->count = end;
        }
    }

    while (len - read >= BLOCK_LEN)
    {
        uint8_t temp[BLOCK_LEN];
        memcpy(temp, data + read ,BLOCK_LEN );

        whirlpool_compress(ctx->state, temp);
        read += BLOCK_LEN;
    }

    if (len - read > 0)
    {
        ctx->count = len - read;
        memcpy(ctx->buf, data + read, ctx->count);
    }
}

void whirlpool_close(whirlpool_context *ctx, void *hashval)
{
    const int LENGTH_SIZE = 32;

    uint8_t temp[BLOCK_LEN];
    memset(temp, 0 , BLOCK_LEN);
    memcpy(temp, ctx->buf, ctx->count);
    temp[ctx->count] = 0x80;
    ctx->count++;

    if (BLOCK_LEN - ctx->count < LENGTH_SIZE)
    {
        whirlpool_compress(ctx->state, temp);
        memset(temp, 0 , LENGTH_SIZE);
    }

    temp[BLOCK_LEN - 1] = (uint8_t)((ctx->total_bytes & 0x1FU) << 3);
    ctx->total_bytes >>= 5;
    for (int i = 1; i < LENGTH_SIZE; i++, ctx->total_bytes >>= 8)
    {
        temp[BLOCK_LEN - 1 - i] = (uint8_t)(ctx->total_bytes & 0xFFU);
    }
    whirlpool_compress(ctx->state, temp);
    memcpy(hashval, ctx->state, BLOCK_LEN);
}
