/**
 * Raw deflate (RFC 1951) into a buffer of the exact uncompressed size.
 * Returns 0 when the output length matches dst_n.
 */
#ifndef RUDEBREAKER_INFLATE_H
#define RUDEBREAKER_INFLATE_H

#include <stddef.h>
#include <stdint.h>

int rb_inflate(const uint8_t *src, size_t src_n, uint8_t *dst, size_t dst_n);

#endif
