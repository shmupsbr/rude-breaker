/**
 * PNG writer for F12 screenshots.
 *
 * The stream is a real PNG: signature, IHDR, one IDAT, IEND. The image
 * is zlib-wrapped deflate (LZ77 plus the fixed Huffman codes), written
 * here with no third-party compressor. Pixels in are 0xRRGGBBAA; the
 * file is 8-bit RGB with filter None.
 */
#define _POSIX_C_SOURCE 200809L

#include "pngwrite.h"
#include "exepath.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#if defined(_WIN32)
#include <direct.h>
#else
#include <sys/types.h>
#endif

static uint32_t g_crc[256];
static int g_crc_ok;

static void crc_init(void)
{
    int n;
    int k;

    if (g_crc_ok) {
        return;
    }
    for (n = 0; n < 256; n++) {
        uint32_t c = (uint32_t)n;

        for (k = 0; k < 8; k++) {
            c = (c & 1u) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
        }
        g_crc[n] = c;
    }
    g_crc_ok = 1;
}

static uint32_t crc_acc(uint32_t c, const uint8_t *p, size_t n)
{
    size_t i;

    crc_init();
    for (i = 0; i < n; i++) {
        c = g_crc[(c ^ p[i]) & 0xFFu] ^ (c >> 8);
    }
    return c;
}

static void put_be32(uint8_t *d, uint32_t v)
{
    d[0] = (uint8_t)(v >> 24);
    d[1] = (uint8_t)(v >> 16);
    d[2] = (uint8_t)(v >> 8);
    d[3] = (uint8_t)v;
}

static int write_chunk(FILE *fp, const char type[4], const uint8_t *data, uint32_t len)
{
    uint8_t b[4];
    uint32_t c;

    put_be32(b, len);
    if (fwrite(b, 1, 4, fp) != 4) {
        return -1;
    }
    if (fwrite(type, 1, 4, fp) != 4) {
        return -1;
    }
    if (len && fwrite(data, 1, len, fp) != len) {
        return -1;
    }
    c = crc_acc(0xFFFFFFFFu, (const uint8_t *)type, 4);
    c = crc_acc(c, data, len);
    c ^= 0xFFFFFFFFu;
    put_be32(b, c);
    if (fwrite(b, 1, 4, fp) != 4) {
        return -1;
    }
    return 0;
}

/* Fixed-Huffman deflate (RFC 1951) plus a zlib wrapper (RFC 1950). */
static int g_sym_len[288];
static int g_sym_rev[288];
static int g_len_sym[259];
static int g_len_ebits[259];
static int g_len_extra[259];
static int g_dist_ebits[30];
static int g_dist_base[30];
static int g_codes_ready;

static int bitrev(int c, int n)
{
    int r = 0;
    int i;

    for (i = 0; i < n; i++) {
        r = (r << 1) | ((c >> i) & 1);
    }
    return r;
}

static void init_codes(void)
{
    int bl[16];
    int nextc[16];
    int i;
    int bits;
    int code;
    int length;
    static const int len_eb[29] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
                                    3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0};
    static const int dist_eb[30] = {0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
                                     7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13};

    if (g_codes_ready) {
        return;
    }
    memset(bl, 0, sizeof bl);
    for (i = 0; i <= 143; i++) {
        g_sym_len[i] = 8;
    }
    for (i = 144; i <= 255; i++) {
        g_sym_len[i] = 9;
    }
    for (i = 256; i <= 279; i++) {
        g_sym_len[i] = 7;
    }
    for (i = 280; i <= 287; i++) {
        g_sym_len[i] = 8;
    }
    for (i = 0; i < 288; i++) {
        bl[g_sym_len[i]]++;
    }
    code = 0;
    bl[0] = 0;
    for (bits = 1; bits <= 15; bits++) {
        code = (code + bl[bits - 1]) << 1;
        nextc[bits] = code;
    }
    for (i = 0; i < 288; i++) {
        int n = g_sym_len[i];

        g_sym_rev[i] = bitrev(nextc[n]++, n);
    }
    length = 3;
    for (code = 0; code < 29 && length <= 258; code++) {
        int eb = len_eb[code];
        int n = 1 << eb;
        int base = length;

        for (i = 0; i < n && length <= 258; i++, length++) {
            g_len_sym[length] = 257 + code;
            g_len_ebits[length] = eb;
            g_len_extra[length] = length - base;
        }
    }
    length = 1;
    for (code = 0; code < 30; code++) {
        int eb = dist_eb[code];

        g_dist_ebits[code] = eb;
        g_dist_base[code] = length;
        length += 1 << eb;
    }
    g_codes_ready = 1;
}

typedef struct Bitw {
    uint8_t *p;
    size_t n;
    size_t cap;
    uint32_t acc;
    int bits;
} Bitw;

static int bw_byte(Bitw *b, uint8_t v)
{
    if (b->n + 1 > b->cap) {
        size_t nc = b->cap ? b->cap * 2u : 4096u;
        uint8_t *q;

        while (nc < b->n + 1) {
            nc *= 2u;
        }
        q = (uint8_t *)realloc(b->p, nc);
        if (!q) {
            return -1;
        }
        b->p = q;
        b->cap = nc;
    }
    b->p[b->n++] = v;
    return 0;
}

static int bw_bits(Bitw *b, uint32_t v, int n)
{
    if (n <= 0) {
        return 0;
    }
    b->acc |= (v & ((1u << n) - 1u)) << b->bits;
    b->bits += n;
    while (b->bits >= 8) {
        if (bw_byte(b, (uint8_t)(b->acc & 0xFFu)) != 0) {
            return -1;
        }
        b->acc >>= 8;
        b->bits -= 8;
    }
    return 0;
}

static int bw_flush(Bitw *b)
{
    if (b->bits > 0) {
        return bw_byte(b, (uint8_t)(b->acc & 0xFFu));
    }
    return 0;
}

static int emit_lit(Bitw *b, int sym)
{
    return bw_bits(b, (uint32_t)g_sym_rev[sym], g_sym_len[sym]);
}

static int emit_match(Bitw *b, int len, int dist)
{
    int dcode = 0;
    int i;

    if (emit_lit(b, g_len_sym[len]) != 0) {
        return -1;
    }
    if (bw_bits(b, (uint32_t)g_len_extra[len], g_len_ebits[len]) != 0) {
        return -1;
    }
    for (i = 0; i < 30; i++) {
        if (g_dist_base[i] <= dist) {
            dcode = i;
        } else {
            break;
        }
    }
    if (bw_bits(b, (uint32_t)bitrev(dcode, 5), 5) != 0) {
        return -1;
    }
    return bw_bits(b, (uint32_t)(dist - g_dist_base[dcode]), g_dist_ebits[dcode]);
}

/* LZ77, window 32K, then one fixed-Huffman block. Returns the zlib
 * payload (header + deflate + adler32) or NULL. */
static uint8_t *deflate_zlib(const uint8_t *raw, size_t n, size_t *out_n)
{
    enum { HASH = 32768, WIN = 32768, MAXL = 258, CHAIN = 96 };
    uint32_t *head;
    uint32_t *prev;
    Bitw bw;
    size_t i;
    uint32_t a32 = 1;
    uint32_t b32 = 0;

    init_codes();
    head = (uint32_t *)calloc(HASH, sizeof(uint32_t));
    prev = (uint32_t *)calloc(n + 1u, sizeof(uint32_t));
    memset(&bw, 0, sizeof bw);
    if (!head || !prev) {
        free(head);
        free(prev);
        return NULL;
    }
    if (bw_byte(&bw, 0x78) != 0 || bw_byte(&bw, 0x9C) != 0) {
        free(head);
        free(prev);
        free(bw.p);
        return NULL;
    }
    /* One fixed block, final. */
    if (bw_bits(&bw, 1u, 1) != 0 || bw_bits(&bw, 1u, 2) != 0) {
        free(head);
        free(prev);
        free(bw.p);
        return NULL;
    }
    i = 0;
    while (i < n) {
        int best_l = 0;
        int best_d = 0;
        size_t left = n - i;

        if (left >= 3) {
            unsigned h = ((unsigned)raw[i] * 31u + raw[i + 1]) * 31u + raw[i + 2];
            uint32_t hp = head[h & (HASH - 1u)];
            int chain = 0;

            h &= (HASH - 1u);
            while (hp && chain < CHAIN) {
                size_t p = (size_t)hp - 1u;
                int dist = (int)(i - p);
                int maxl;
                int len;

                if (dist <= 0 || dist > WIN) {
                    break;
                }
                maxl = left > (size_t)MAXL ? MAXL : (int)left;
                if (raw[p] == raw[i] && raw[p + 1] == raw[i + 1] && raw[p + 2] == raw[i + 2]) {
                    len = 3;
                    while (len < maxl && raw[p + (size_t)len] == raw[i + (size_t)len]) {
                        len++;
                    }
                    if (len > best_l) {
                        best_l = len;
                        best_d = dist;
                        if (len == maxl) {
                            break;
                        }
                    }
                }
                hp = prev[p];
                chain++;
            }
            prev[i] = head[h];
            head[h] = (uint32_t)(i + 1u);
        }
        if (best_l >= 3) {
            size_t k;

            if (emit_match(&bw, best_l, best_d) != 0) {
                free(head);
                free(prev);
                free(bw.p);
                return NULL;
            }
            for (k = 1; k < (size_t)best_l && i + k + 2 < n; k++) {
                unsigned h = ((unsigned)raw[i + k] * 31u + raw[i + k + 1]) * 31u + raw[i + k + 2];

                h &= (HASH - 1u);
                prev[i + k] = head[h];
                head[h] = (uint32_t)(i + k + 1u);
            }
            i += (size_t)best_l;
        } else {
            if (emit_lit(&bw, raw[i]) != 0) {
                free(head);
                free(prev);
                free(bw.p);
                return NULL;
            }
            i++;
        }
    }
    if (emit_lit(&bw, 256) != 0 || bw_flush(&bw) != 0) {
        free(head);
        free(prev);
        free(bw.p);
        return NULL;
    }
    for (i = 0; i < n; i++) {
        a32 += raw[i];
        if (a32 >= 65521u) {
            a32 -= 65521u;
        }
        b32 += a32;
        if (b32 >= 65521u) {
            b32 %= 65521u;
        }
    }
    if (bw_byte(&bw, (uint8_t)(b32 >> 8)) != 0 || bw_byte(&bw, (uint8_t)b32) != 0 ||
        bw_byte(&bw, (uint8_t)(a32 >> 8)) != 0 || bw_byte(&bw, (uint8_t)a32) != 0) {
        free(head);
        free(prev);
        free(bw.p);
        return NULL;
    }
    free(head);
    free(prev);
    *out_n = bw.n;
    return bw.p;
}

/* Byte i of the filtered RGB image (one filter byte, then w*3 pixels). */
static uint8_t raw_at(const uint32_t *px, int w, size_t i)
{
    size_t stride = 1u + (size_t)w * 3u;
    size_t y = i / stride;
    size_t x = i % stride;
    uint32_t p;
    int ch;

    if (x == 0) {
        return 0;
    }
    x -= 1;
    p = px[y * (size_t)w + x / 3u];
    ch = (int)(x % 3u);
    if (ch == 0) {
        return (uint8_t)(p >> 24);
    }
    if (ch == 1) {
        return (uint8_t)(p >> 16);
    }
    return (uint8_t)(p >> 8);
}

int rb_write_png(FILE *fp, const uint32_t *px, int w, int h)
{
    static const uint8_t sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};
    size_t raw_n;
    uint8_t *idat;
    size_t at = 0;
    size_t off;
    uint8_t ihdr[13];
    int rc = -1;

    if (!fp || !px || w <= 0 || h <= 0 || w > 4096 || h > 4096) {
        return -1;
    }
    raw_n = (size_t)h * (1u + (size_t)w * 3u);
    {
        uint8_t *raw = (uint8_t *)malloc(raw_n);

        if (!raw) {
            return -1;
        }
        for (off = 0; off < raw_n; off++) {
            raw[off] = raw_at(px, w, off);
        }
        idat = deflate_zlib(raw, raw_n, &at);
        free(raw);
    }
    if (!idat) {
        return -1;
    }
    if (fwrite(sig, 1, 8, fp) != 8) {
        goto done;
    }
    memset(ihdr, 0, sizeof ihdr);
    put_be32(ihdr, (uint32_t)w);
    put_be32(ihdr + 4, (uint32_t)h);
    ihdr[8] = 8;
    ihdr[9] = 2;
    if (write_chunk(fp, "IHDR", ihdr, 13) != 0) {
        goto done;
    }
    if (write_chunk(fp, "IDAT", idat, (uint32_t)at) != 0) {
        goto done;
    }
    if (write_chunk(fp, "IEND", NULL, 0) != 0) {
        goto done;
    }
    rc = 0;
done:
    free(idat);
    return rc;
}

static int path_exists(const char *p)
{
    struct stat st;

    return stat(p, &st) == 0;
}

static int make_dir(const char *p)
{
#if defined(_WIN32)
    if (_mkdir(p) == 0 || errno == EEXIST) {
        return 0;
    }
#else
    if (mkdir(p, 0755) == 0 || errno == EEXIST) {
        return 0;
    }
#endif
    return -1;
}

static void slashify(char *s)
{
    char *p;

    for (p = s; *p; p++) {
        if (*p == '\\') {
            *p = '/';
        }
    }
}

/* Game root: the data folder when it is a directory, otherwise the
 * folder beside a disk image, otherwise the executable, otherwise
 * RUDEBR_ASSETS, otherwise the working directory. */
static void game_root(const char *asset, char *out, size_t n)
{
    struct stat st;
    char tmp[1024];
    char *slash;
    const char *env;

    out[0] = 0;
    if (asset && asset[0]) {
        snprintf(tmp, sizeof tmp, "%s", asset);
        slashify(tmp);
        if (stat(tmp, &st) == 0 && S_ISDIR(st.st_mode)) {
            snprintf(out, n, "%s", tmp);
            return;
        }
        slash = strrchr(tmp, '/');
        if (slash && slash != tmp) {
            *slash = 0;
            snprintf(out, n, "%s", tmp);
            return;
        }
    }
    if (rb_exe_dir(out, n) == 0) {
        return;
    }
    env = getenv("RUDEBR_ASSETS");
    if (env && env[0]) {
        snprintf(out, n, "%s", env);
        slashify(out);
        return;
    }
    snprintf(out, n, "%s", ".");
}

int rb_save_screenshot(const uint32_t *px, int w, int h, const char *asset)
{
    char root[1024];
    char dir[1100];
    char path[1200];
    char stamp[32];
    time_t now;
    struct tm tm;
    FILE *fp;
    int n;
    int i;
    int wrote;

    if (!px) {
        return -1;
    }
    now = time(NULL);
#if defined(_WIN32)
    if (localtime_s(&tm, &now) != 0) {
        return -1;
    }
#else
    if (!localtime_r(&now, &tm)) {
        return -1;
    }
#endif
    snprintf(stamp, sizeof stamp, "%04d%02d%02d_%02d%02d%02d",
             tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
             tm.tm_hour, tm.tm_min, tm.tm_sec);
    game_root(asset, root, sizeof root);
    n = snprintf(dir, sizeof dir, "%s/screenshots", root);
    if (n <= 0 || (size_t)n >= sizeof dir) {
        return -1;
    }
    if (make_dir(dir) != 0) {
        fprintf(stderr, "screenshot: cannot create %s\n", dir);
        return -1;
    }
    wrote = -1;
    for (i = 0; i < 100; i++) {
        if (i == 0) {
            n = snprintf(path, sizeof path, "%s/RudeBreaker_%s.png", dir, stamp);
        } else {
            n = snprintf(path, sizeof path, "%s/RudeBreaker_%s_%d.png", dir, stamp, i + 1);
        }
        if (n <= 0 || (size_t)n >= sizeof path) {
            return -1;
        }
        if (path_exists(path)) {
            continue;
        }
        fp = fopen(path, "wb");
        if (!fp) {
            fprintf(stderr, "screenshot: cannot write %s\n", path);
            return -1;
        }
        wrote = rb_write_png(fp, px, w, h);
        if (fclose(fp) != 0) {
            wrote = -1;
        }
        if (wrote != 0) {
            fprintf(stderr, "screenshot: write failed %s\n", path);
            return -1;
        }
        fprintf(stderr, "screenshot: %s\n", path);
        return 0;
    }
    fprintf(stderr, "screenshot: too many files for %s\n", stamp);
    return -1;
}
