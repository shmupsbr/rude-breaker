/**
 * Run a stage and write 640x400 PNG frames of the real playfield.
 */
#include "assets.h"
#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t crc_table[256];
static int crc_ready;

static void crc_init(void)
{
    int i;

    for (i = 0; i < 256; i++) {
        uint32_t c = (uint32_t)i;
        int k;

        for (k = 0; k < 8; k++) {
            c = (c & 1u) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
        }
        crc_table[i] = c;
    }
    crc_ready = 1;
}

static uint32_t crc32(const uint8_t *p, size_t n)
{
    uint32_t c = 0xFFFFFFFFu;
    size_t i;

    if (!crc_ready) {
        crc_init();
    }
    for (i = 0; i < n; i++) {
        c = crc_table[(c ^ p[i]) & 0xFFu] ^ (c >> 8);
    }
    return c ^ 0xFFFFFFFFu;
}

static void be32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);
    p[3] = (uint8_t)v;
}

static int write_chunk(FILE *fp, const char *tag, const uint8_t *data, size_t n)
{
    uint8_t lenb[4];
    uint8_t *crcbuf;
    uint32_t sum;
    uint8_t sumb[4];

    be32(lenb, (uint32_t)n);
    if (fwrite(lenb, 1, 4, fp) != 4 || fwrite(tag, 1, 4, fp) != 4) {
        return -1;
    }
    if (n && fwrite(data, 1, n, fp) != n) {
        return -1;
    }
    crcbuf = (uint8_t *)malloc(4 + n);
    if (!crcbuf) {
        return -1;
    }
    memcpy(crcbuf, tag, 4);
    if (n) {
        memcpy(crcbuf + 4, data, n);
    }
    sum = crc32(crcbuf, 4 + n);
    free(crcbuf);
    be32(sumb, sum);
    return fwrite(sumb, 1, 4, fp) == 4 ? 0 : -1;
}

static uint32_t adler32(const uint8_t *p, size_t n)
{
    uint32_t a = 1;
    uint32_t b = 0;
    size_t i;

    for (i = 0; i < n; i++) {
        a = (a + p[i]) % 65521u;
        b = (b + a) % 65521u;
    }
    return (b << 16) | a;
}

static int write_png(const char *path, const uint32_t *rgba, int w, int h)
{
    size_t stride = (size_t)w * 4u + 1u;
    size_t raw_n = stride * (size_t)h;
    uint8_t *raw;
    uint8_t *z;
    size_t zcap;
    size_t zo;
    size_t off;
    uint32_t ad;
    uint8_t ihdr[13];
    FILE *fp;
    int y;
    int x;
    static const uint8_t sig[8] = {137, 80, 78, 71, 13, 10, 26, 10};

    raw = (uint8_t *)malloc(raw_n);
    if (!raw) {
        return -1;
    }
    for (y = 0; y < h; y++) {
        uint8_t *row = raw + (size_t)y * stride;

        row[0] = 0;
        for (x = 0; x < w; x++) {
            uint32_t px = rgba[(size_t)y * (size_t)w + (size_t)x];

            row[1 + x * 4 + 0] = (uint8_t)(px >> 24);
            row[1 + x * 4 + 1] = (uint8_t)(px >> 16);
            row[1 + x * 4 + 2] = (uint8_t)(px >> 8);
            row[1 + x * 4 + 3] = (uint8_t)px;
        }
    }
    zcap = raw_n + raw_n / 65535u * 5u + 16u;
    z = (uint8_t *)malloc(zcap);
    if (!z) {
        free(raw);
        return -1;
    }
    z[0] = 0x78;
    z[1] = 0x01;
    zo = 2;
    off = 0;
    while (off < raw_n) {
        size_t n = raw_n - off;
        uint16_t nn;
        uint16_t nnot;

        if (n > 65535u) {
            n = 65535u;
        }
        nn = (uint16_t)n;
        nnot = (uint16_t)~nn;
        z[zo++] = (off + n == raw_n) ? 1u : 0u;
        z[zo++] = (uint8_t)nn;
        z[zo++] = (uint8_t)(nn >> 8);
        z[zo++] = (uint8_t)nnot;
        z[zo++] = (uint8_t)(nnot >> 8);
        memcpy(z + zo, raw + off, n);
        zo += n;
        off += n;
    }
    ad = adler32(raw, raw_n);
    z[zo++] = (uint8_t)(ad >> 24);
    z[zo++] = (uint8_t)(ad >> 16);
    z[zo++] = (uint8_t)(ad >> 8);
    z[zo++] = (uint8_t)ad;
    free(raw);

    memset(ihdr, 0, sizeof ihdr);
    be32(ihdr, (uint32_t)w);
    be32(ihdr + 4, (uint32_t)h);
    ihdr[8] = 8;
    ihdr[9] = 6;

    fp = fopen(path, "wb");
    if (!fp) {
        free(z);
        return -1;
    }
    if (fwrite(sig, 1, 8, fp) != 8 ||
        write_chunk(fp, "IHDR", ihdr, 13) != 0 ||
        write_chunk(fp, "IDAT", z, zo) != 0 ||
        write_chunk(fp, "IEND", NULL, 0) != 0) {
        fclose(fp);
        free(z);
        return -1;
    }
    free(z);
    fclose(fp);
    return 0;
}

static void stats(const char *tag, const uint32_t *px, const RbVmInfo *info)
{
    int y;
    int x;
    uint32_t nonzero = 0;
    uint32_t colors = 0;
    uint8_t seen[256];

    memset(seen, 0, sizeof seen);
    for (y = 0; y < RB_NATIVE_H; y++) {
        for (x = 0; x < RB_NATIVE_W; x++) {
            uint32_t c = px[(size_t)y * RB_NATIVE_W + (size_t)x] >> 8;
            uint8_t key = (uint8_t)((c >> 16) ^ (c >> 8) ^ c);

            if (c != 0) {
                nonzero++;
            }
            if (!seen[key]) {
                seen[key] = 1;
                colors++;
            }
        }
    }
    printf("%s gfx=%d chunk=%u cell=%u ship=%d,%d score=%u alive=%u "
           "nonzero=%u buckets=%u\n",
           tag, vm_gfx_active(NULL) ? 0 : 0,
           (unsigned)info->chunk, (unsigned)info->cell,
           (int)info->ship_x, (int)info->ship_y,
           (unsigned)info->score, (unsigned)info->alive,
           (unsigned)nonzero, (unsigned)colors);
    (void)vm_gfx_active;
}

int main(int argc, char **argv)
{
    const char *dir = argc > 1 ? argv[1] : "assets/raw/RUDEBR";
    const char *out_dir = argc > 2 ? argv[2] : "/opt/cursor/artifacts";
    RbAssets assets;
    RbVm *vm = NULL;
    RbInput in;
    uint32_t *px;
    int frame;
    const int shots[] = {90, 1400, 2200};
    int si = 0;

    if (assets_load(&assets, dir) != 0) {
        fprintf(stderr, "assets load failed\n");
        return 1;
    }
    if (vm_create(&vm, &assets) != 0) {
        fprintf(stderr, "vm_create failed\n");
        return 1;
    }
    px = (uint32_t *)calloc((size_t)RB_NATIVE_W * RB_NATIVE_H, sizeof(uint32_t));
    if (!px) {
        return 1;
    }
    memset(&in, 0, sizeof in);
    in.buttons = RB_BTN_A;
    vm_reset_play(vm, 1);
    for (frame = 1; frame <= 2200; frame++) {
        RbVmInfo info;

        vm_set_input(vm, &in);
        vm_frame(vm);
        if (si < 3 && frame == shots[si]) {
            char path[256];
            int rc;

            vm_present(vm, px, RB_NATIVE_W, RB_NATIVE_H);
            vm_info(vm, &info);
            stats("shot", px, &info);
            printf("  active=%d\n", vm_gfx_active(vm));
            snprintf(path, sizeof path, "%s/stage1_f%d.png", out_dir, frame);
            rc = write_png(path, px, RB_NATIVE_W, RB_NATIVE_H);
            printf("  wrote %s (%d)\n", path, rc);
            si++;
        }
    }
    free(px);
    vm_destroy(vm);
    assets_free(&assets);
    return 0;
}
