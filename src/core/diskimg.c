/**
 * Read SGM1.DAT, FPLAY.COM, SONG.DAT, and optional RBRANK.DAT from a
 * directory or from the FAT12/FAT16 filesystem inside a PC-98 disk image.
 */
#include "diskimg.h"
#include "inflate.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(__linux__) || defined(__APPLE__)
#include <dirent.h>
#define RB_HAS_DIRENT 1
#endif

enum { DISK_MAX = 128u * 1024u * 1024u };

static void set_str(char *dst, size_t n, const char *src)
{
    if (!dst || n == 0) {
        return;
    }
    if (!src) {
        dst[0] = 0;
        return;
    }
    snprintf(dst, n, "%s", src);
}

void disk_init(RbDisk *d)
{
    if (!d) {
        return;
    }
    memset(d, 0, sizeof(*d));
}

void disk_free(RbDisk *d)
{
    if (!d) {
        return;
    }
    free(d->sgm1);
    free(d->fplay);
    free(d->song);
    free(d->rank);
    memset(d, 0, sizeof(*d));
}

static int read_whole(const char *path, uint8_t **out, size_t *out_n)
{
    FILE *fp;
    long sz;
    uint8_t *buf;

    *out = NULL;
    *out_n = 0;
    fp = fopen(path, "rb");
    if (!fp) {
        return -1;
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return -1;
    }
    sz = ftell(fp);
    if (sz < 0 || (unsigned long)sz > DISK_MAX) {
        fclose(fp);
        return -1;
    }
    rewind(fp);
    buf = (uint8_t *)malloc((size_t)sz ? (size_t)sz : 1u);
    if (!buf) {
        fclose(fp);
        return -1;
    }
    if (sz > 0 && fread(buf, 1, (size_t)sz, fp) != (size_t)sz) {
        free(buf);
        fclose(fp);
        return -1;
    }
    fclose(fp);
    *out = buf;
    *out_n = (size_t)sz;
    return 0;
}

static int is_dir(const char *path)
{
    FILE *fp;
    char probe[1200];
    int n;

    /* A directory cannot be opened as a binary file on either host we
     * ship. Probing a child name is enough to tell a folder from an image. */
    n = snprintf(probe, sizeof(probe), "%s/SGM1.DAT", path);
    if (n > 0 && (size_t)n < sizeof(probe)) {
        fp = fopen(probe, "rb");
        if (fp) {
            fclose(fp);
            return 1;
        }
    }
#if defined(RB_HAS_DIRENT)
    {
        DIR *dir = opendir(path);

        if (dir) {
            closedir(dir);
            return 1;
        }
    }
#endif
    return 0;
}

static const char *ext_of(const char *name)
{
    const char *dot = strrchr(name ? name : "", '.');

    return dot ? dot : "";
}

static int ext_is(const char *name, const char *ext)
{
    const char *e = ext_of(name);
    size_t i;

    for (i = 0; ext[i]; i++) {
        char a = e[i];
        char b = ext[i];

        if (a >= 'A' && a <= 'Z') {
            a = (char)(a - 'A' + 'a');
        }
        if (b >= 'A' && b <= 'Z') {
            b = (char)(b - 'A' + 'a');
        }
        if (a != b) {
            return 0;
        }
    }
    return e[i] == 0;
}

static void parent_dir(const char *path, char *out, size_t n)
{
    char tmp[1024];
    char *slash;

    set_str(tmp, sizeof(tmp), path);
    slash = strrchr(tmp, '/');
    if (!slash) {
        slash = strrchr(tmp, '\\');
    }
    if (!slash) {
        set_str(out, n, ".");
        return;
    }
    if (slash == tmp) {
        set_str(out, n, "/");
        return;
    }
    *slash = 0;
    set_str(out, n, tmp);
}

static int bpb_ok(const uint8_t *p, size_t n)
{
    unsigned bps;
    unsigned spc;
    unsigned nfats;
    unsigned rsec;
    unsigned nroot;
    unsigned spf;

    if (!p || n < 64) {
        return 0;
    }
    if (p[0] != 0xEB && p[0] != 0xE9) {
        return 0;
    }
    bps = (unsigned)p[11] | ((unsigned)p[12] << 8);
    if (bps != 256 && bps != 512 && bps != 1024) {
        return 0;
    }
    spc = p[13];
    if (spc == 0 || (spc & (spc - 1u)) != 0) {
        return 0;
    }
    rsec = (unsigned)p[14] | ((unsigned)p[15] << 8);
    nfats = p[16];
    nroot = (unsigned)p[17] | ((unsigned)p[18] << 8);
    spf = (unsigned)p[22] | ((unsigned)p[23] << 8);
    if (rsec == 0 || nfats < 1 || nfats > 2 || nroot == 0 || spf == 0) {
        return 0;
    }
    if (p[21] < 0xF0) {
        return 0;
    }
    if ((size_t)rsec * bps + (size_t)nfats * spf * bps > n) {
        return 0;
    }
    return 1;
}

static const uint8_t *find_bpb(const uint8_t *img, size_t n, size_t *off_out)
{
    size_t off;

    if (bpb_ok(img, n)) {
        *off_out = 0;
        return img;
    }
    for (off = 256; off + 64 < n && off < 8u * 1024u * 1024u; off += 256) {
        if (bpb_ok(img + off, n - off)) {
            *off_out = off;
            return img + off;
        }
    }
    return NULL;
}

static uint16_t rd16(const uint8_t *p)
{
    return (uint16_t)(p[0] | (p[1] << 8));
}

static uint32_t rd32(const uint8_t *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
           ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static int payload_shift(const uint8_t *img, size_t n, const char *name,
                         const uint8_t **raw, size_t *raw_n)
{
    /* Anex86 FDI and HDI share a 4096-byte header of little-endian words. */
    if ((ext_is(name, ".fdi") || ext_is(name, ".hdi")) && n > 4096) {
        uint32_t header = rd32(img + 8);
        uint32_t dsize = rd32(img + 12);
        uint32_t bps = rd32(img + 16);

        if ((header == 4096 || header == 512) && header < n &&
            (bps == 256 || bps == 512 || bps == 1024) &&
            (dsize == 0 || (size_t)header + (size_t)dsize <= n)) {
            *raw = img + header;
            *raw_n = dsize ? (size_t)dsize : (n - header);
            return 0;
        }
    }
    if (n >= 16 && memcmp(img, "T98HDDIMAGE", 11) == 0) {
        uint32_t header = 512;

        if (n >= 0x110) {
            uint32_t h = rd32(img + 0x100);
            if (h >= 512 && h < n && h <= 8192) {
                header = h;
            }
        }
        *raw = img + header;
        *raw_n = n - header;
        return 0;
    }
    *raw = img;
    *raw_n = n;
    return 0;
}

static int d88_linear(const uint8_t *img, size_t n, uint8_t **out, size_t *out_n)
{
    uint32_t tracks[164];
    uint8_t *lin;
    size_t lin_n;
    unsigned bps = 0;
    unsigned spt = 0;
    unsigned heads = 2;
    unsigned max_c = 0;
    int t;
    int i;

    if (n < 0x2B0) {
        return -1;
    }
    for (i = 0; i < 164; i++) {
        tracks[i] = rd32(img + 0x20 + (size_t)i * 4u);
        if (tracks[i] != 0 && (tracks[i] < 0x2B0 || tracks[i] >= n)) {
            return -1;
        }
    }
    if (tracks[0] == 0) {
        return -1;
    }
    /* Geometry comes from the sector headers. 1024-byte 2HD is the
     * common PC-98 floppy; 512-byte sectors are accepted too. */
    for (t = 0; t < 164; t++) {
        size_t p;
        unsigned nsec;
        unsigned s;

        if (tracks[t] == 0) {
            continue;
        }
        p = tracks[t];
        if (p + 16 > n) {
            return -1;
        }
        nsec = rd16(img + p + 4);
        if (nsec == 0 || nsec > 32) {
            return -1;
        }
        for (s = 0; s < nsec; s++) {
            unsigned sz;
            unsigned nn;

            if (p + 16 > n) {
                return -1;
            }
            nn = img[p + 3];
            sz = rd16(img + p + 14);
            if (sz == 0) {
                sz = 128u << (nn > 7 ? 7 : nn);
            }
            if (p + 16 + sz > n) {
                return -1;
            }
            if (bps == 0) {
                bps = sz;
            }
            if (sz != bps) {
                return -1;
            }
            if (img[p + 1] + 1u > heads) {
                heads = img[p + 1] + 1u;
            }
            if (img[p] > max_c) {
                max_c = img[p];
            }
            if (nsec > spt) {
                spt = nsec;
            }
            p += 16u + sz;
        }
    }
    if (bps == 0 || spt == 0 || heads == 0) {
        return -1;
    }
    lin_n = (size_t)(max_c + 1u) * heads * spt * bps;
    if (lin_n == 0 || lin_n > DISK_MAX) {
        return -1;
    }
    lin = (uint8_t *)calloc(1, lin_n);
    if (!lin) {
        return -1;
    }
    for (t = 0; t < 164; t++) {
        size_t p;
        unsigned nsec;
        unsigned s;

        if (tracks[t] == 0) {
            continue;
        }
        p = tracks[t];
        nsec = rd16(img + p + 4);
        for (s = 0; s < nsec; s++) {
            unsigned c = img[p];
            unsigned h = img[p + 1];
            unsigned r = img[p + 2];
            unsigned sz = rd16(img + p + 14);
            size_t dest;

            if (sz == 0) {
                sz = bps;
            }
            if (r >= 1 && h < heads && r <= spt) {
                dest = (((size_t)c * heads + h) * spt + (r - 1u)) * bps;
                if (dest + sz <= lin_n) {
                    memcpy(lin + dest, img + p + 16, sz);
                }
            }
            p += 16u + sz;
        }
    }
    *out = lin;
    *out_n = lin_n;
    return 0;
}

static int looks_like_d88(const uint8_t *img, size_t n, const char *name)
{
    uint32_t t0;
    uint8_t kind;

    if (n < 0x2B0) {
        return 0;
    }
    if (!ext_is(name, ".d88")) {
        return 0;
    }
    kind = img[0x1B];
    t0 = rd32(img + 0x20);
    if (kind != 0x00 && kind != 0x10 && kind != 0x20) {
        return 0;
    }
    return t0 >= 0x2B0 && t0 < n;
}

typedef struct FatView {
    const uint8_t *base;
    size_t         n;
    unsigned       bps;
    unsigned       spc;
    unsigned       fat16;
    size_t         fat_off;
    size_t         fat_n;
    size_t         root_off;
    size_t         root_n;
    size_t         data_off;
} FatView;

static int fat_open(const uint8_t *img, size_t n, size_t bpb_off, FatView *f)
{
    const uint8_t *p;
    unsigned bps, spc, rsec, nfats, nroot, spf, tot16;
    uint32_t tot32;
    uint32_t root_secs;
    uint32_t data_secs;
    uint32_t clusters;
    size_t avail;

    memset(f, 0, sizeof(*f));
    if (bpb_off >= n) {
        return -1;
    }
    p = img + bpb_off;
    avail = n - bpb_off;
    if (!bpb_ok(p, avail)) {
        return -1;
    }
    bps = (unsigned)rd16(p + 11);
    spc = p[13];
    rsec = rd16(p + 14);
    nfats = p[16];
    nroot = rd16(p + 17);
    tot16 = rd16(p + 19);
    spf = rd16(p + 22);
    tot32 = tot16 ? tot16 : rd32(p + 32);
    root_secs = ((uint32_t)nroot * 32u + (bps - 1u)) / bps;
    if ((uint32_t)rsec + (uint32_t)nfats * spf + root_secs >= tot32) {
        return -1;
    }
    data_secs = tot32 - ((uint32_t)rsec + (uint32_t)nfats * spf + root_secs);
    clusters = data_secs / spc;
    f->base = p;
    f->n = avail;
    f->bps = bps;
    f->spc = spc;
    f->fat16 = clusters >= 4085u;
    f->fat_off = (size_t)rsec * bps;
    f->fat_n = (size_t)spf * bps;
    f->root_off = f->fat_off + (size_t)nfats * f->fat_n;
    f->root_n = (size_t)nroot * 32u;
    f->data_off = f->root_off + f->root_n;
    if (f->data_off > avail || f->fat_off + f->fat_n > avail) {
        return -1;
    }
    return 0;
}

static unsigned fat_next(const FatView *f, unsigned cluster)
{
    const uint8_t *fat = f->base + f->fat_off;

    if (!f->fat16) {
        size_t off = (size_t)cluster + (size_t)cluster / 2u;
        unsigned val;

        if (off + 1 >= f->fat_n) {
            return 0xFFFu;
        }
        val = fat[off] | (fat[off + 1] << 8);
        return (cluster & 1u) ? (val >> 4) : (val & 0xFFFu);
    }
    if ((size_t)cluster * 2u + 1u >= f->fat_n) {
        return 0xFFFFu;
    }
    return fat[cluster * 2u] | (fat[cluster * 2u + 1u] << 8);
}

static int fat_end(const FatView *f, unsigned cluster)
{
    if (!f->fat16) {
        return cluster < 2 || cluster >= 0xFF8;
    }
    return cluster < 2 || cluster >= 0xFFF8;
}

static uint8_t *fat_chain(const FatView *f, unsigned start, size_t *out_n)
{
    unsigned cluster = start;
    unsigned seen[4096];
    unsigned nseen = 0;
    size_t csize = (size_t)f->spc * f->bps;
    uint8_t *buf;
    size_t cap;
    size_t used = 0;
    unsigned guard;

    *out_n = 0;
    if (fat_end(f, start)) {
        return NULL;
    }
    cap = csize * 4u;
    if (cap < csize) {
        return NULL;
    }
    buf = (uint8_t *)malloc(cap);
    if (!buf) {
        return NULL;
    }
    for (guard = 0; guard < 8192 && !fat_end(f, cluster); guard++) {
        size_t off;
        unsigned s;

        for (s = 0; s < nseen; s++) {
            if (seen[s] == cluster) {
                free(buf);
                return NULL;
            }
        }
        if (nseen < 4096) {
            seen[nseen++] = cluster;
        }
        off = f->data_off + (size_t)(cluster - 2u) * csize;
        if (off + csize > f->n) {
            free(buf);
            return NULL;
        }
        if (used + csize > cap) {
            size_t ncap = cap * 2u;
            uint8_t *grown;

            if (ncap > DISK_MAX || ncap < cap) {
                free(buf);
                return NULL;
            }
            grown = (uint8_t *)realloc(buf, ncap);
            if (!grown) {
                free(buf);
                return NULL;
            }
            buf = grown;
            cap = ncap;
        }
        memcpy(buf + used, f->base + off, csize);
        used += csize;
        cluster = fat_next(f, cluster);
    }
    *out_n = used;
    return buf;
}

static void entry_name(const uint8_t *e, char out[13])
{
    int i;
    int n = 0;

    for (i = 0; i < 8 && e[i] != ' '; i++) {
        out[n++] = (char)e[i];
    }
    if (e[8] != ' ') {
        out[n++] = '.';
        for (i = 8; i < 11 && e[i] != ' '; i++) {
            out[n++] = (char)e[i];
        }
    }
    out[n] = 0;
}

static int name_eq(const char *a, const char *b)
{
    size_t i;

    for (i = 0; a[i] || b[i]; i++) {
        char ca = a[i];
        char cb = b[i];

        if (ca >= 'a' && ca <= 'z') {
            ca = (char)(ca - 'a' + 'A');
        }
        if (cb >= 'a' && cb <= 'z') {
            cb = (char)(cb - 'a' + 'A');
        }
        if (ca != cb) {
            return 0;
        }
    }
    return 1;
}

static void keep_file(RbDisk *d, const char *name, uint8_t *data, size_t n, uint32_t size)
{
    uint8_t **slot = NULL;
    size_t *slot_n = NULL;

    if (size > n) {
        size = (uint32_t)n;
    }
    if (name_eq(name, "SGM1.DAT")) {
        slot = &d->sgm1;
        slot_n = &d->sgm1_n;
    } else if (name_eq(name, "FPLAY.COM")) {
        slot = &d->fplay;
        slot_n = &d->fplay_n;
    } else if (name_eq(name, "SONG.DAT")) {
        slot = &d->song;
        slot_n = &d->song_n;
    } else if (name_eq(name, "RBRANK.DAT")) {
        slot = &d->rank;
        slot_n = &d->rank_n;
    } else {
        free(data);
        return;
    }
    if (*slot) {
        free(data);
        return;
    }
    if (size < n) {
        uint8_t *trimmed = (uint8_t *)realloc(data, size ? size : 1u);

        if (trimmed) {
            data = trimmed;
        }
    }
    *slot = data;
    *slot_n = size;
}

static void walk_dir(const FatView *f, const uint8_t *ents, size_t ents_n, RbDisk *d, int depth)
{
    size_t i;

    if (depth > 8 || !ents) {
        return;
    }
    for (i = 0; i + 32 <= ents_n; i += 32) {
        const uint8_t *e = ents + i;
        char name[13];
        unsigned attr;
        unsigned cluster;
        uint32_t size;
        uint8_t *chain;
        size_t chain_n;

        if (e[0] == 0x00) {
            break;
        }
        if (e[0] == 0xE5 || e[11] == 0x0F) {
            continue;
        }
        attr = e[11];
        if (attr & 0x08) {
            continue;
        }
        entry_name(e, name);
        if (name[0] == '.' || name[0] == 0) {
            continue;
        }
        cluster = rd16(e + 26);
        size = rd32(e + 28);
        if (attr & 0x10) {
            chain = fat_chain(f, cluster, &chain_n);
            walk_dir(f, chain, chain_n, d, depth + 1);
            free(chain);
            continue;
        }
        chain = fat_chain(f, cluster, &chain_n);
        if (!chain) {
            continue;
        }
        keep_file(d, name, chain, chain_n, size);
    }
}

static int fat_extract(const uint8_t *img, size_t n, RbDisk *d)
{
    size_t bpb_off = 0;
    const uint8_t *bpb;
    FatView fat;

    bpb = find_bpb(img, n, &bpb_off);
    if (!bpb) {
        return -1;
    }
    if (fat_open(img, n, bpb_off, &fat) != 0) {
        return -1;
    }
    walk_dir(&fat, fat.base + fat.root_off, fat.root_n, d, 0);
    d->ok = d->sgm1 && d->fplay && d->song;
    return d->ok ? 0 : -1;
}

static int unzip_image(const uint8_t *zip, size_t n, uint8_t **out, size_t *out_n, char *name_out, size_t name_n)
{
    size_t eocd = 0;
    int found = 0;
    uint16_t entries;
    uint32_t cd_off;
    uint32_t p;
    uint16_t ei;
    int best = 0;
    uint32_t best_local = 0;
    uint32_t best_comp = 0;
    uint32_t best_uncomp = 0;
    uint16_t best_method = 0;
    char best_name[256];

    best_name[0] = 0;
    if (n < 22) {
        return -1;
    }
    {
        size_t start = n > 66000u ? n - 66000u : 0;
        size_t i;

        for (i = n - 22; i >= start; i--) {
            if (zip[i] == 'P' && zip[i + 1] == 'K' && zip[i + 2] == 5 && zip[i + 3] == 6) {
                eocd = i;
                found = 1;
                break;
            }
            if (i == start) {
                break;
            }
        }
    }
    if (!found || eocd + 22 > n) {
        return -1;
    }
    entries = rd16(zip + eocd + 10);
    cd_off = rd32(zip + eocd + 16);
    if (cd_off >= n) {
        return -1;
    }
    p = cd_off;
    for (ei = 0; ei < entries && p + 46 <= n; ei++) {
        uint16_t method;
        uint32_t comp;
        uint32_t uncomp;
        uint16_t namelen;
        uint16_t extra;
        uint16_t comment;
        uint32_t local;
        char name[256];
        unsigned k;
        int score;

        if (rd32(zip + p) != 0x02014b50u) {
            break;
        }
        method = rd16(zip + p + 10);
        comp = rd32(zip + p + 20);
        uncomp = rd32(zip + p + 24);
        namelen = rd16(zip + p + 28);
        extra = rd16(zip + p + 30);
        comment = rd16(zip + p + 32);
        local = rd32(zip + p + 42);
        if (p + 46u + namelen > n) {
            return -1;
        }
        k = namelen < 255 ? namelen : 255;
        memcpy(name, zip + p + 46, k);
        name[k] = 0;
        score = 0;
        if (ext_is(name, ".hdm")) {
            score = 6;
        } else if (ext_is(name, ".fdi")) {
            score = 5;
        } else if (ext_is(name, ".d88")) {
            score = 4;
        } else if (ext_is(name, ".hdi")) {
            score = 3;
        } else if (ext_is(name, ".nhd")) {
            score = 2;
        } else if (name[0] && name[strlen(name) - 1] != '/') {
            score = 1;
        }
        if (score > best && uncomp > 0 && uncomp <= DISK_MAX &&
            (method == 0 || method == 8)) {
            best = score;
            best_local = local;
            best_comp = comp;
            best_uncomp = uncomp;
            best_method = method;
            set_str(best_name, sizeof(best_name), name);
        }
        p += 46u + namelen + extra + comment;
    }
    if (!best || best_local + 30 > n) {
        return -1;
    }
    {
        uint16_t namelen = rd16(zip + best_local + 26);
        uint16_t extra = rd16(zip + best_local + 28);
        size_t data = (size_t)best_local + 30u + namelen + extra;
        uint8_t *buf;

        if (rd32(zip + best_local) != 0x04034b50u || data + best_comp > n) {
            return -1;
        }
        buf = (uint8_t *)malloc(best_uncomp ? best_uncomp : 1u);
        if (!buf) {
            return -1;
        }
        if (best_method == 0) {
            if (best_comp != best_uncomp) {
                free(buf);
                return -1;
            }
            memcpy(buf, zip + data, best_uncomp);
        } else if (rb_inflate(zip + data, best_comp, buf, best_uncomp) != 0) {
            free(buf);
            return -1;
        }
        *out = buf;
        *out_n = best_uncomp;
        set_str(name_out, name_n, best_name);
        return 0;
    }
}

static int image_to_raw(const uint8_t *img, size_t n, const char *name,
                        uint8_t **raw, size_t *raw_n, int *owned)
{
    const uint8_t *view;
    size_t view_n;
    uint8_t *lin = NULL;
    size_t lin_n = 0;

    *owned = 0;
    *raw = NULL;
    *raw_n = 0;
    if (n >= 4 && img[0] == 'P' && img[1] == 'K' && img[2] == 3 && img[3] == 4) {
        char inner[256];
        uint8_t *unz = NULL;
        size_t unz_n = 0;
        int rc;

        if (unzip_image(img, n, &unz, &unz_n, inner, sizeof(inner)) != 0) {
            return -1;
        }
        rc = image_to_raw(unz, unz_n, inner, raw, raw_n, owned);
        if (rc != 0 || !*owned) {
            if (*owned) {
                free(*raw);
            }
            *raw = unz;
            *raw_n = unz_n;
            *owned = 1;
            return 0;
        }
        free(unz);
        return 0;
    }
    if (looks_like_d88(img, n, name)) {
        if (d88_linear(img, n, &lin, &lin_n) != 0) {
            return -1;
        }
        *raw = lin;
        *raw_n = lin_n;
        *owned = 1;
        return 0;
    }
    payload_shift(img, n, name, &view, &view_n);
    *raw = (uint8_t *)view;
    *raw_n = view_n;
    *owned = 0;
    return 0;
}

static int open_image_bytes(const uint8_t *img, size_t n, const char *name, RbDisk *d)
{
    uint8_t *raw = NULL;
    size_t raw_n = 0;
    int owned = 0;
    int rc;

    if (image_to_raw(img, n, name, &raw, &raw_n, &owned) != 0) {
        return -1;
    }
    rc = fat_extract(raw, raw_n, d);
    if (owned) {
        free(raw);
    }
    d->saw_image = 1;
    return rc;
}

static int open_image_file(const char *path, RbDisk *d)
{
    uint8_t *img = NULL;
    size_t n = 0;
    int rc;

    if (read_whole(path, &img, &n) != 0) {
        return -1;
    }
    rc = open_image_bytes(img, n, path, d);
    free(img);
    set_str(d->label, sizeof(d->label), path);
    parent_dir(path, d->data_dir, sizeof(d->data_dir));
    return rc;
}

static int take_loose(RbDisk *d, const char *dir, const char *filename, uint8_t **slot, size_t *slot_n)
{
    char path[1200];
    int n;

    n = snprintf(path, sizeof(path), "%s/%s", dir, filename);
    if (n <= 0 || (size_t)n >= sizeof(path)) {
        return -1;
    }
    return read_whole(path, slot, slot_n);
}

static int open_loose_dir(const char *dir, RbDisk *d)
{
    char nested[1100];

    take_loose(d, dir, "SGM1.DAT", &d->sgm1, &d->sgm1_n);
    if (!d->sgm1) {
        snprintf(nested, sizeof(nested), "%s/RUDEBR", dir);
        take_loose(d, nested, "SGM1.DAT", &d->sgm1, &d->sgm1_n);
        if (d->sgm1) {
            dir = nested;
        }
    }
    if (!d->sgm1) {
        return -1;
    }
    take_loose(d, dir, "FPLAY.COM", &d->fplay, &d->fplay_n);
    take_loose(d, dir, "SONG.DAT", &d->song, &d->song_n);
    take_loose(d, dir, "RBRANK.DAT", &d->rank, &d->rank_n);
    set_str(d->label, sizeof(d->label), dir);
    set_str(d->data_dir, sizeof(d->data_dir), dir);
    d->ok = d->sgm1 && d->fplay && d->song;
    d->saw_image = 0;
    return d->ok ? 0 : -1;
}

int disk_find_image(const char *dir, char *out, size_t n)
{
#if !defined(RB_HAS_DIRENT)
    (void)dir;
    (void)out;
    (void)n;
    return -1;
#else
    static const char *exts[] = { ".hdm", ".zip", ".fdi", ".d88", ".hdi", ".nhd" };
    size_t e;

    if (!dir || !out || n == 0) {
        return -1;
    }
    for (e = 0; e < sizeof(exts) / sizeof(exts[0]); e++) {
        DIR *dp = opendir(dir);
        struct dirent *ent;

        if (!dp) {
            return -1;
        }
        while ((ent = readdir(dp)) != NULL) {
            char path[1200];
            int nw;
            RbDisk probe;

            if (!ext_is(ent->d_name, exts[e])) {
                continue;
            }
            nw = snprintf(path, sizeof(path), "%s/%s", dir, ent->d_name);
            if (nw <= 0 || (size_t)nw >= sizeof(path)) {
                continue;
            }
            disk_init(&probe);
            if (open_image_file(path, &probe) == 0 && probe.ok) {
                disk_free(&probe);
                closedir(dp);
                set_str(out, n, path);
                return 0;
            }
            disk_free(&probe);
        }
        closedir(dp);
    }
    return -1;
#endif
}

int disk_open(const char *path, RbDisk *d)
{
    char found[1200];

    if (!d) {
        return -1;
    }
    disk_free(d);
    disk_init(d);
    if (!path || !path[0]) {
        return -1;
    }
    set_str(d->label, sizeof(d->label), path);
    if (!is_dir(path)) {
        return open_image_file(path, d);
    }
    if (open_loose_dir(path, d) == 0) {
        return 0;
    }
    disk_free(d);
    disk_init(d);
    set_str(d->label, sizeof(d->label), path);
    if (disk_find_image(path, found, sizeof(found)) == 0) {
        return open_image_file(found, d);
    }
    return -1;
}
