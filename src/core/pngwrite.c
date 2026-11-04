/**
 * PNG writer for F12 screenshots.
 *
 * The stream is a real PNG: signature, IHDR, one IDAT, IEND. The image
 * is zlib-wrapped deflate stored blocks (RFC 1950 / 1951), so the
 * encoder does not need zlib or any other library. Pixels in are
 * 0xRRGGBBAA; the file is 8-bit RGB with filter None.
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
    size_t blocks;
    size_t cap;
    uint8_t *idat;
    size_t at;
    size_t off;
    uint32_t a32 = 1;
    uint32_t b32 = 0;
    uint8_t ihdr[13];
    int rc = -1;

    if (!fp || !px || w <= 0 || h <= 0 || w > 4096 || h > 4096) {
        return -1;
    }
    raw_n = (size_t)h * (1u + (size_t)w * 3u);
    blocks = (raw_n + 65535u) / 65535u;
    cap = 2u + blocks * 5u + raw_n + 4u;
    idat = (uint8_t *)malloc(cap);
    if (!idat) {
        return -1;
    }
    idat[0] = 0x78;
    idat[1] = 0x01;
    at = 2;
    off = 0;
    while (off < raw_n) {
        size_t n = raw_n - off;
        size_t i;
        uint16_t len;
        uint16_t nlen;

        if (n > 65535u) {
            n = 65535u;
        }
        len = (uint16_t)n;
        nlen = (uint16_t)~len;
        idat[at++] = (off + n == raw_n) ? 1 : 0;
        idat[at++] = (uint8_t)len;
        idat[at++] = (uint8_t)(len >> 8);
        idat[at++] = (uint8_t)nlen;
        idat[at++] = (uint8_t)(nlen >> 8);
        for (i = 0; i < n; i++) {
            uint8_t by = raw_at(px, w, off + i);

            idat[at++] = by;
            a32 += by;
            if (a32 >= 65521u) {
                a32 -= 65521u;
            }
            b32 += a32;
            if (b32 >= 65521u) {
                b32 %= 65521u;
            }
        }
        off += n;
    }
    idat[at++] = (uint8_t)(b32 >> 8);
    idat[at++] = (uint8_t)b32;
    idat[at++] = (uint8_t)(a32 >> 8);
    idat[at++] = (uint8_t)a32;
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
