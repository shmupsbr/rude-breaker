/**
 * RFC 1951 inflate. ZIP method 8 feeds this the raw deflate stream.
 * No zlib wrapper. The bit order is the deflate order.
 */
#include "inflate.h"

#include <string.h>

typedef struct BitIn {
    const uint8_t *p;
    size_t         n;
    size_t         i;
    uint32_t       acc;
    int            bits;
} BitIn;

typedef struct Huff {
    int left[640];
    int right[640];
    int sym[640];
    int used;
} Huff;

static int bit_pull(BitIn *b, int n, int *out)
{
    uint32_t mask;
    int v;

    if (n < 0 || n > 24) {
        return -1;
    }
    while (b->bits < n) {
        if (b->i >= b->n) {
            return -1;
        }
        b->acc |= (uint32_t)b->p[b->i++] << b->bits;
        b->bits += 8;
    }
    mask = (n == 0) ? 0u : ((1u << n) - 1u);
    v = (int)(b->acc & mask);
    b->acc >>= n;
    b->bits -= n;
    *out = v;
    return 0;
}

static void bit_align(BitIn *b)
{
    b->acc = 0;
    b->bits = 0;
}

static void huff_reset(Huff *h)
{
    int i;

    h->used = 1;
    for (i = 0; i < 640; i++) {
        h->left[i] = -1;
        h->right[i] = -1;
        h->sym[i] = -1;
    }
}

static int huff_build(Huff *h, const uint8_t *lens, int nsym)
{
    int bl_count[16];
    int next_code[16];
    int code;
    int bits;
    int s;

    huff_reset(h);
    memset(bl_count, 0, sizeof(bl_count));
    for (s = 0; s < nsym; s++) {
        if (lens[s] > 15) {
            return -1;
        }
        bl_count[lens[s]]++;
    }
    bl_count[0] = 0;
    code = 0;
    next_code[0] = 0;
    for (bits = 1; bits <= 15; bits++) {
        code = (code + bl_count[bits - 1]) << 1;
        next_code[bits] = code;
    }
    for (s = 0; s < nsym; s++) {
        int len = lens[s];
        int c;
        int node;
        int i;

        if (len == 0) {
            continue;
        }
        c = next_code[len]++;
        node = 0;
        for (i = len - 1; i >= 0; i--) {
            int bit = (c >> i) & 1;
            int *slot = bit ? &h->right[node] : &h->left[node];

            if (*slot < 0) {
                if (h->used >= 640) {
                    return -1;
                }
                *slot = h->used++;
            }
            node = *slot;
        }
        h->sym[node] = s;
    }
    return 0;
}

static int huff_decode(BitIn *b, const Huff *h, int *out)
{
    int node = 0;
    int steps = 0;

    while (h->sym[node] < 0) {
        int bit;

        if (steps++ > 16 || bit_pull(b, 1, &bit) != 0) {
            return -1;
        }
        node = bit ? h->right[node] : h->left[node];
        if (node < 0 || node >= h->used) {
            return -1;
        }
    }
    *out = h->sym[node];
    return 0;
}

static const uint16_t LEN_BASE[29] = {
    3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
    35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258
};
static const uint8_t LEN_EXTRA[29] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
    3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0
};
static const uint16_t DIST_BASE[30] = {
    1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
    257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
    8193, 12289, 16385, 24577
};
static const uint8_t DIST_EXTRA[30] = {
    0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
    7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13
};
static const uint8_t CLEN_ORDER[19] = {
    16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
};

static int inflate_block(BitIn *b, const Huff *lit, const Huff *dist,
                         uint8_t *dst, size_t dst_n, size_t *filled)
{
    for (;;) {
        int sym;

        if (huff_decode(b, lit, &sym) != 0) {
            return -1;
        }
        if (sym < 256) {
            if (*filled >= dst_n) {
                return -1;
            }
            dst[(*filled)++] = (uint8_t)sym;
            continue;
        }
        if (sym == 256) {
            return 0;
        }
        if (sym <= 285) {
            int len_i = sym - 257;
            int extra = 0;
            int length;
            int dsym;
            int dist_v;
            size_t from;
            int k;

            if (len_i < 0 || len_i >= 29) {
                return -1;
            }
            if (LEN_EXTRA[len_i] && bit_pull(b, LEN_EXTRA[len_i], &extra) != 0) {
                return -1;
            }
            length = (int)LEN_BASE[len_i] + extra;
            if (huff_decode(b, dist, &dsym) != 0 || dsym < 0 || dsym >= 30) {
                return -1;
            }
            extra = 0;
            if (DIST_EXTRA[dsym] && bit_pull(b, DIST_EXTRA[dsym], &extra) != 0) {
                return -1;
            }
            dist_v = (int)DIST_BASE[dsym] + extra;
            if (dist_v <= 0 || (size_t)dist_v > *filled) {
                return -1;
            }
            from = *filled - (size_t)dist_v;
            if (*filled + (size_t)length > dst_n) {
                return -1;
            }
            for (k = 0; k < length; k++) {
                dst[(*filled)++] = dst[from + (size_t)k];
            }
            continue;
        }
        return -1;
    }
}

static int fixed_trees(Huff *lit, Huff *dist)
{
    uint8_t ll[288];
    uint8_t dd[32];
    int i;

    for (i = 0; i <= 143; i++) {
        ll[i] = 8;
    }
    for (i = 144; i <= 255; i++) {
        ll[i] = 9;
    }
    for (i = 256; i <= 279; i++) {
        ll[i] = 7;
    }
    for (i = 280; i <= 287; i++) {
        ll[i] = 8;
    }
    for (i = 0; i < 32; i++) {
        dd[i] = 5;
    }
    if (huff_build(lit, ll, 288) != 0 || huff_build(dist, dd, 32) != 0) {
        return -1;
    }
    return 0;
}

static int dynamic_trees(BitIn *b, Huff *lit, Huff *dist)
{
    int hlit;
    int hdist;
    int hclen;
    uint8_t clen[19];
    uint8_t lens[320];
    Huff cl;
    int i;
    int n;
    int got;

    memset(clen, 0, sizeof(clen));
    memset(lens, 0, sizeof(lens));
    if (bit_pull(b, 5, &hlit) != 0 || bit_pull(b, 5, &hdist) != 0 ||
        bit_pull(b, 4, &hclen) != 0) {
        return -1;
    }
    hlit += 257;
    hdist += 1;
    hclen += 4;
    if (hlit > 286 || hdist > 32 || hclen > 19) {
        return -1;
    }
    for (i = 0; i < hclen; i++) {
        int v;

        if (bit_pull(b, 3, &v) != 0) {
            return -1;
        }
        clen[CLEN_ORDER[i]] = (uint8_t)v;
    }
    if (huff_build(&cl, clen, 19) != 0) {
        return -1;
    }
    n = hlit + hdist;
    got = 0;
    while (got < n) {
        int sym;
        int repeat;
        int extra = 0;
        uint8_t fill;

        if (huff_decode(b, &cl, &sym) != 0) {
            return -1;
        }
        if (sym < 16) {
            lens[got++] = (uint8_t)sym;
            continue;
        }
        if (sym == 16) {
            if (got == 0 || bit_pull(b, 2, &extra) != 0) {
                return -1;
            }
            repeat = 3 + extra;
            fill = lens[got - 1];
        } else if (sym == 17) {
            if (bit_pull(b, 3, &extra) != 0) {
                return -1;
            }
            repeat = 3 + extra;
            fill = 0;
        } else if (sym == 18) {
            if (bit_pull(b, 7, &extra) != 0) {
                return -1;
            }
            repeat = 11 + extra;
            fill = 0;
        } else {
            return -1;
        }
        if (got + repeat > n) {
            return -1;
        }
        while (repeat-- > 0) {
            lens[got++] = fill;
        }
    }
    if (huff_build(lit, lens, hlit) != 0 ||
        huff_build(dist, lens + hlit, hdist) != 0) {
        return -1;
    }
    return 0;
}

int rb_inflate(const uint8_t *src, size_t src_n, uint8_t *dst, size_t dst_n)
{
    BitIn b;
    size_t filled = 0;
    Huff lit;
    Huff dist;

    if (!src || !dst) {
        return -1;
    }
    memset(&b, 0, sizeof(b));
    b.p = src;
    b.n = src_n;
    for (;;) {
        int bfinal;
        int btype;

        if (bit_pull(&b, 1, &bfinal) != 0 || bit_pull(&b, 2, &btype) != 0) {
            return -1;
        }
        if (btype == 0) {
            int len;
            int nlen;
            int i;

            bit_align(&b);
            if (b.i + 4 > b.n) {
                return -1;
            }
            len = b.p[b.i] | (b.p[b.i + 1] << 8);
            nlen = b.p[b.i + 2] | (b.p[b.i + 3] << 8);
            b.i += 4;
            if ((len ^ 0xFFFF) != nlen || b.i + (size_t)len > b.n) {
                return -1;
            }
            if (filled + (size_t)len > dst_n) {
                return -1;
            }
            for (i = 0; i < len; i++) {
                dst[filled++] = b.p[b.i++];
            }
        } else if (btype == 1 || btype == 2) {
            if (btype == 1) {
                if (fixed_trees(&lit, &dist) != 0) {
                    return -1;
                }
            } else if (dynamic_trees(&b, &lit, &dist) != 0) {
                return -1;
            }
            if (inflate_block(&b, &lit, &dist, dst, dst_n, &filled) != 0) {
                return -1;
            }
        } else {
            return -1;
        }
        if (bfinal) {
            break;
        }
    }
    return filled == dst_n ? 0 : -1;
}
