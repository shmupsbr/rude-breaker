/**
 * Script VM.
 *
 * Fetch matches game image 0x2FD6: read the opcode, store the IP just
 * past it, then run the handler. Opcode 0x04 returns to the dispatcher
 * for one frame. Opcode 0x06 waits N frames (N = 1 runs the next opcode
 * on the following frame). A child allocated at a higher slot runs in the
 * same forward scan; a child allocated behind the parent runs next frame.
 *
 * Opcode 0x0C is a far call into the script chunk. Those targets are
 * native islands (ship 0x53AB, movers 0x6F2E and its siblings, score
 * 0x71DC, options 0x59CB). The island runner below executes that code
 * until RETF. Sub-op 0x2A is the aim step (game image 0x0F88): it turns
 * by the spin word, it does not snap to the target heading.
 */
#include "vm.h"
#include "collision.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ENT_N    256
#define ENT_STR  0x52
#define G_SIZE   0x200
#define CHUNK_N  40
#define CHUNK_CAP (256u * 1024u)
#define SCRATCH  39

#define MODE_FETCH   0
#define MODE_DELAY   1
#define MODE_WAIT    2
#define MODE_FOREVER 3

#define SEG_DS 0
#define SEG_SS 1
#define SEG_CS 2
#define SEG_ES 3

struct RbVm {
    const RbAssets *assets;
    uint8_t *chunk[CHUNK_N];
    uint32_t clen[CHUNK_N];
    uint8_t ent[ENT_N * ENT_STR];
    uint8_t g[G_SIZE];
    uint8_t vars[256];
    uint8_t mode[ENT_N];
    uint8_t cof[ENT_N];
    uint16_t origin[ENT_N];
    uint16_t rng_mix;
    int rng_cf;
    uint16_t held;
    uint16_t prev_held;
    int quit;
    int boot;
    int view_chunk;
    uint32_t spawns;
    uint32_t step_faults;
    uint32_t op_miss;
    uint32_t nat_miss;
    uint16_t miss_ip;
    uint8_t miss_op;
    uint16_t nat_miss_ip;
    uint32_t aim_ops;
    uint32_t ship_natives;
    uint32_t option_natives;
    uint32_t loads;
    uint16_t load_record;
    uint16_t load_header;
    uint8_t seen_op[256];
    uint8_t nat_logged;
};

/* Quarter sine, game image 0x1848. */
static const int16_t SIN_Q[64] = {
    0, 804, 1608, 2410, 3212, 4011, 4808, 5602,
    6393, 7179, 7962, 8739, 9512, 10278, 11039, 11793,
    12539, 13279, 14010, 14732, 15446, 16151, 16846, 17530,
    18204, 18868, 19519, 20159, 20787, 21403, 22005, 22594,
    23170, 23731, 24279, 24811, 25329, 25832, 26319, 26790,
    27245, 27683, 28105, 28510, 28898, 29268, 29621, 29956,
    30273, 30571, 30852, 31113, 31356, 31580, 31785, 31971,
    32137, 32285, 32412, 32521, 32609, 32678, 32728, 32757
};

/* Ratio scan at game image 0x18C8, used by atan 0x0EC6. */
static const uint16_t ATAN_T[64] = {
    0, 6, 13, 19, 25, 32, 38, 44,
    51, 57, 64, 71, 78, 85, 92, 99,
    106, 113, 121, 129, 137, 145, 153, 162,
    171, 180, 190, 200, 210, 221, 232, 244,
    256, 269, 282, 297, 312, 328, 345, 363,
    383, 404, 427, 452, 479, 509, 541, 578,
    618, 664, 715, 775, 844, 925, 1022, 1140,
    1287, 1475, 1726, 2076, 2599, 3471, 5211, 10428
};

static uint16_t eu16(const RbVm *vm, int i, int off)
{
    return read_u16_le(vm->ent + i * ENT_STR + off);
}

static void su16(RbVm *vm, int i, int off, uint16_t v)
{
    write_u16_le(vm->ent + i * ENT_STR + off, v);
}

static uint8_t eu8(const RbVm *vm, int i, int off)
{
    return vm->ent[i * ENT_STR + off];
}

static void su8(RbVm *vm, int i, int off, uint8_t v)
{
    vm->ent[i * ENT_STR + off] = v;
}

static uint16_t gu16(const RbVm *vm, int off)
{
    if (off < 0 || off + 1 >= G_SIZE) {
        return 0;
    }
    return read_u16_le(vm->g + off);
}

static void sgu16(RbVm *vm, int off, uint16_t v)
{
    if (off < 0 || off + 1 >= G_SIZE) {
        return;
    }
    write_u16_le(vm->g + off, v);
}

static uint8_t *slot_ptr(RbVm *vm, int i)
{
    return vm->ent + i * ENT_STR;
}

static const uint8_t *code_of(const RbVm *vm, int ent, uint32_t *len)
{
    unsigned c = vm->cof[ent];

    if (c >= CHUNK_N || !vm->chunk[c]) {
        *len = 0;
        return NULL;
    }
    *len = vm->clen[c];
    return vm->chunk[c];
}

static uint8_t rd8(const RbVm *vm, int ent, uint16_t ip)
{
    uint32_t n = 0;
    const uint8_t *p = code_of(vm, ent, &n);

    if (!p || (uint32_t)ip >= n) {
        return 0;
    }
    return p[ip];
}

static uint16_t rd16(const RbVm *vm, int ent, uint16_t ip)
{
    uint32_t n = 0;
    const uint8_t *p = code_of(vm, ent, &n);

    if (!p || (uint32_t)ip + 1u >= n) {
        return 0;
    }
    return read_u16_le(p + ip);
}

static void clear_slot(RbVm *vm, int i)
{
    memset(slot_ptr(vm, i), 0, ENT_STR);
    vm->mode[i] = MODE_FETCH;
    vm->origin[i] = 0;
}

/* ---- LZ77 (loader 0x6ACD) ---------------------------------------------- */

static uint32_t lz_decompress(const uint8_t *src, uint32_t src_n,
                              uint8_t *dst, uint32_t dst_n)
{
    uint32_t si = 0;
    uint32_t di = 0;

    while (si < src_n && di < dst_n) {
        uint8_t ctrl = src[si++];

        if (ctrl == 0) {
            break;
        }
        if (ctrl < 0x80u) {
            uint32_t n = ctrl;

            if (si + n > src_n) {
                n = src_n - si;
            }
            if (di + n > dst_n) {
                n = dst_n - di;
            }
            memcpy(dst + di, src + si, n);
            si += ctrl;
            di += n;
            if (si > src_n) {
                break;
            }
        } else {
            uint32_t n = (uint32_t)(ctrl & 0x7Fu) + 3u;
            uint32_t dist;
            uint32_t k;

            if (si >= src_n) {
                break;
            }
            dist = (uint32_t)src[si++] + 1u;
            for (k = 0; k < n && di < dst_n; k++) {
                if (dist > di) {
                    dst[di++] = 0;
                } else {
                    dst[di] = dst[di - dist];
                    di++;
                }
            }
        }
    }
    return di;
}

static int ensure_chunk(RbVm *vm, unsigned index)
{
    uint32_t sz = 0;
    const uint8_t *src;
    uint8_t *dst;
    uint32_t n;

    if (!vm->assets || index >= CHUNK_N || index >= vm->assets->sgm1.count) {
        return -1;
    }
    if (vm->chunk[index]) {
        return 0;
    }
    src = assets_sgm1_chunk(vm->assets, (uint16_t)index, &sz);
    if (!src || sz == 0) {
        return -1;
    }
    dst = (uint8_t *)malloc(CHUNK_CAP);
    if (!dst) {
        return -1;
    }
    memset(dst, 0, CHUNK_CAP);
    n = lz_decompress(src, sz, dst, CHUNK_CAP);
    vm->chunk[index] = dst;
    vm->clen[index] = n;
    return 0;
}

/* ---- RNG 0xE5D / 0xE4E -------------------------------------------------- */

static uint16_t rng_byte(RbVm *vm)
{
    uint16_t ax = gu16(vm, 0x5C);
    uint16_t bx = vm->rng_mix;
    uint32_t wide;
    int cf = vm->rng_cf;
    uint8_t al = (uint8_t)ax;
    uint8_t ah = (uint8_t)(ax >> 8);
    uint8_t bl;
    uint8_t bh;
    uint8_t tmp;

    bx = (uint16_t)(bx + gu16(vm, 0x64));
    bx = (uint16_t)(bx + ax);
    bl = (uint8_t)bx;
    bh = (uint8_t)(bx >> 8);
    tmp = bh;
    bh = al;
    al = tmp;
    ax = (uint16_t)al | ((uint16_t)ah << 8);
    bx = (uint16_t)bl | ((uint16_t)bh << 8);

    {
        int new_cf = bx & 1;

        bx = (uint16_t)((bx >> 1) | (cf ? 0x8000u : 0u));
        cf = new_cf;
    }
    {
        int rol_cf = (ax >> 15) & 1;

        ax = (uint16_t)((ax << 1) | (unsigned)rol_cf);
        cf = rol_cf;
    }
    wide = (uint32_t)bx - ax - (unsigned)cf;
    cf = (wide >> 16) & 1;
    bx = (uint16_t)wide;
    wide = (uint32_t)bx - ax;
    cf = (wide >> 16) & 1;
    bx = (uint16_t)wide;
    wide = (uint32_t)ax + bx;
    cf = (wide >> 16) & 1;
    ax = (uint16_t)wide;
    wide = (uint32_t)vm->rng_mix + ax;
    cf = (wide >> 16) & 1;
    vm->rng_mix = (uint16_t)wide;
    {
        unsigned sum = (ax & 0xFFu) + (ax >> 8);

        cf = sum > 0xFFu;
        ax = (uint16_t)((ax & 0xFF00u) | (sum & 0xFFu));
    }
    sgu16(vm, 0x5C, ax);
    vm->rng_cf = cf;
    return ax;
}

static uint16_t rng_word(RbVm *vm)
{
    uint16_t hi = rng_byte(vm);
    uint16_t lo = rng_byte(vm);

    return (uint16_t)((hi << 8) | (lo & 0xFFu));
}

/* ---- sincos / atan / aim ----------------------------------------------- */

static int16_t sincos(uint8_t angle)
{
    uint16_t bx = angle;

    if ((bx & 0xC0u) == 0u) {
        return SIN_Q[bx];
    }
    if ((bx & 0x80u) == 0u) {
        bx = (uint16_t)(0u - bx);
        bx = (uint16_t)(bx + 0x7Fu);
        return SIN_Q[bx & 0x3Fu];
    }
    if ((bx & 0x40u) == 0u) {
        bx = (uint16_t)(bx - 0x80u);
        return (int16_t)(-SIN_Q[bx & 0x3Fu]);
    }
    bx = (uint16_t)(0u - bx);
    bx = (uint16_t)(bx + 0xFFu);
    return (int16_t)(-SIN_Q[bx & 0x3Fu]);
}

static uint16_t atan_scan(uint16_t ratio, uint16_t base, int step_down)
{
    uint16_t ax = base;
    int i;

    for (i = 0; i < 64; i++) {
        if (ratio < ATAN_T[i]) {
            break;
        }
        if (step_down) {
            ax = (uint16_t)(ax - 0x100u);
        } else {
            ax = (uint16_t)(ax + 0x100u);
        }
    }
    return ax;
}

/* 0x0EC6. CX is target X, DX is target Y. Result angle is in AX. */
static uint16_t atan_to(RbVm *vm, int ent, uint16_t tx, uint16_t ty)
{
    int16_t ey = (int16_t)eu16(vm, ent, 0x24);
    int16_t ex = (int16_t)eu16(vm, ent, 0x22);
    int16_t dx;
    int16_t dy;
    uint16_t numer;
    uint16_t denom;
    uint32_t ratio;

    if (ey < (int16_t)ty) {
        if (ex < (int16_t)tx) {
            dx = (int16_t)((int16_t)tx - ex);
            dy = (int16_t)((int16_t)ty - ey);
            numer = (uint16_t)dx;
            denom = (uint16_t)dy;
            if (denom == 0) {
                return 0x4000u;
            }
            ratio = ((uint32_t)numer << 8) / denom;
            if (ratio > 0xFFFFu) {
                ratio = 0xFFFFu;
            }
            return atan_scan((uint16_t)ratio, 0x4000u, 1);
        }
        dx = (int16_t)(ex - (int16_t)tx);
        dy = (int16_t)((int16_t)ty - ey);
        numer = (uint16_t)dx;
        denom = (uint16_t)dy;
        if (denom == 0) {
            return 0x3F00u;
        }
        ratio = ((uint32_t)numer << 8) / denom;
        if (ratio > 0xFFFFu) {
            ratio = 0xFFFFu;
        }
        return atan_scan((uint16_t)ratio, 0x3F00u, 0);
    }
    if (ex > (int16_t)tx) {
        dx = (int16_t)(ex - (int16_t)tx);
        dy = (int16_t)(ey - (int16_t)ty);
        numer = (uint16_t)dx;
        denom = (uint16_t)dy;
        if (denom == 0) {
            return 0xC000u;
        }
        ratio = ((uint32_t)numer << 8) / denom;
        if (ratio > 0xFFFFu) {
            ratio = 0xFFFFu;
        }
        return atan_scan((uint16_t)ratio, 0xC000u, 1);
    }
    dx = (int16_t)((int16_t)tx - ex);
    dy = (int16_t)(ey - (int16_t)ty);
    numer = (uint16_t)dx;
    denom = (uint16_t)dy;
    if (denom == 0) {
        return 0xBF00u;
    }
    ratio = ((uint32_t)numer << 8) / denom;
    if (ratio > 0xFFFFu) {
        ratio = 0xFFFFu;
    }
    return atan_scan((uint16_t)ratio, 0xBF00u, 0);
}

/* 0x0F88. Step the stored heading toward the atan result by the spin word. */
static uint16_t aim_step(RbVm *vm, int ent, uint16_t tx, uint16_t ty)
{
    uint16_t want = atan_to(vm, ent, tx, ty);
    uint16_t ang = eu16(vm, ent, 0x2E);
    uint16_t spin = eu16(vm, ent, 0x30);
    unsigned cur = (want >> 8) & 0xFFu;
    unsigned have = (ang >> 8) & 0xFFu;
    unsigned delta = (cur + 0x100u - have) & 0xFFu;

    if (delta < 0x80u) {
        return (uint16_t)(ang + spin);
    }
    if (delta == 0x80u) {
        return 0;
    }
    return (uint16_t)(ang - spin);
}

/* ---- ALU / compare ------------------------------------------------------ */

static void rcl_bd(RbVm *vm, int cf)
{
    uint8_t b = vm->g[0xBD];

    vm->g[0xBD] = (uint8_t)((b << 1) | (cf ? 1 : 0));
}

/* Incoming CPU carry into RCR is 0: the handlers XOR a register clear
 * before the call, and the following ADD of the table base does not carry.
 * The bit the next ADC consumes is bit 0, saved by the previous RCL. */
static int rcr_bd(RbVm *vm)
{
    uint8_t b = vm->g[0xBD];
    int out = b & 1;

    vm->g[0xBD] = (uint8_t)(b >> 1);
    return out;
}

static uint16_t word_alu(RbVm *vm, uint8_t op, uint16_t cur, uint16_t imm)
{
    uint32_t wide;
    int cf;
    uint16_t ax = cur;
    uint16_t dx = imm;

    switch (op) {
    case 0x00:
        return dx;
    case 0x02:
        return (uint16_t)(0u - dx);
    case 0x04:
        return (uint16_t)(int16_t)(int8_t)(dx & 0xFFu);
    case 0x06:
        wide = (uint32_t)ax + dx;
        rcl_bd(vm, wide > 0xFFFFu);
        return (uint16_t)wide;
    case 0x08:
        wide = (uint32_t)ax - dx;
        rcl_bd(vm, wide > 0xFFFFu);
        return (uint16_t)wide;
    case 0x0A:
        cf = rcr_bd(vm);
        wide = (uint32_t)ax + dx + (unsigned)cf;
        rcl_bd(vm, wide > 0xFFFFu);
        return (uint16_t)wide;
    case 0x0C:
        cf = rcr_bd(vm);
        wide = (uint32_t)ax - dx - (unsigned)cf;
        rcl_bd(vm, wide > 0xFFFFu);
        return (uint16_t)wide;
    case 0x0E: {
        int16_t sx = (int16_t)(int8_t)(dx & 0xFFu);

        wide = (uint32_t)ax + (uint16_t)sx;
        rcl_bd(vm, wide > 0xFFFFu);
        return (uint16_t)wide;
    }
    case 0x10: {
        int16_t sx = (int16_t)(int8_t)(dx & 0xFFu);

        wide = (uint32_t)ax - (uint16_t)sx;
        rcl_bd(vm, wide > 0xFFFFu);
        return (uint16_t)wide;
    }
    case 0x12:
        return (uint16_t)(ax & dx);
    case 0x14:
        return (uint16_t)(ax | dx);
    case 0x16:
        return (uint16_t)(ax ^ dx);
    case 0x18:
        return (uint16_t)((uint32_t)ax * dx);
    case 0x1A:
        if (dx == 0) {
            return 0;
        }
        return (uint16_t)(ax / dx);
    case 0x1C:
        ax = rng_word(vm);
        if (dx == 0) {
            return 0;
        }
        return (uint16_t)(ax % dx);
    case 0x1E: {
        unsigned n = dx & 31u;

        if (n >= 16u) {
            return 0;
        }
        return (uint16_t)(ax << n);
    }
    case 0x20: {
        unsigned n = dx & 31u;

        if (n >= 16u) {
            return (ax & 0x8000u) ? 0xFFFFu : 0u;
        }
        return (uint16_t)((int16_t)ax >> n);
    }
    default:
        return cur;
    }
}

static uint8_t byte_alu(RbVm *vm, uint8_t op, uint8_t cur, uint8_t imm)
{
    unsigned wide;
    int cf;

    switch (op) {
    case 0x00:
    case 0x04:
        return imm;
    case 0x02:
        return (uint8_t)(0u - imm);
    case 0x06:
    case 0x0E:
        wide = (unsigned)cur + imm;
        rcl_bd(vm, wide > 0xFFu);
        return (uint8_t)wide;
    case 0x08:
    case 0x10:
        wide = (unsigned)cur - imm;
        rcl_bd(vm, wide > 0xFFu);
        return (uint8_t)wide;
    case 0x0A:
        cf = rcr_bd(vm);
        wide = (unsigned)cur + imm + (unsigned)cf;
        rcl_bd(vm, wide > 0xFFu);
        return (uint8_t)wide;
    case 0x0C:
        cf = rcr_bd(vm);
        wide = (unsigned)cur - imm - (unsigned)cf;
        rcl_bd(vm, wide > 0xFFu);
        return (uint8_t)wide;
    case 0x12:
        return (uint8_t)(cur & imm);
    case 0x14:
        return (uint8_t)(cur | imm);
    case 0x16:
        return (uint8_t)(cur ^ imm);
    case 0x18:
        return (uint8_t)(cur * imm);
    case 0x1A:
        if (imm == 0) {
            return 0;
        }
        return (uint8_t)(cur / imm);
    case 0x1C: {
        uint8_t r = (uint8_t)rng_word(vm);

        if (imm == 0) {
            return 0;
        }
        return (uint8_t)(r % imm);
    }
    case 0x1E: {
        unsigned n = imm & 31u;

        if (n >= 8u) {
            return 0;
        }
        return (uint8_t)(cur << n);
    }
    case 0x20: {
        unsigned n = imm & 31u;

        if (n >= 8u) {
            return (cur & 0x80u) ? 0xFFu : 0u;
        }
        return (uint8_t)((int8_t)cur >> n);
    }
    default:
        return cur;
    }
}

/* Carry clear is true (JNC at 0x39AF). */
static int cmp_true(uint8_t op, uint16_t ax, uint16_t dx)
{
    switch (op) {
    case 0x00:
        return ax == dx;
    case 0x02:
        return ax != dx;
    case 0x04:
        return ax > dx;
    case 0x06:
        return ax < dx;
    case 0x08:
        return (int16_t)ax > (int16_t)dx;
    case 0x0A:
        return (int16_t)ax < (int16_t)dx;
    case 0x0C:
        return ax >= dx;
    case 0x0E:
        return ax <= dx;
    case 0x10:
        return (int16_t)ax >= (int16_t)dx;
    case 0x12:
        return (int16_t)ax <= (int16_t)dx;
    case 0x14:
        return (ax & dx) == 0;
    case 0x16:
        return (ax & dx) != 0;
    default:
        return 0;
    }
}

/* ---- bytecode stack (entity +0x0C / +0x0E) ------------------------------ */

static int push_w(RbVm *vm, int ent, uint16_t v)
{
    uint16_t sp = eu16(vm, ent, 0x0C);

    if ((unsigned)sp + 2u > 0x44u) {
        return -1;
    }
    su16(vm, ent, 0x0E + sp, v);
    su16(vm, ent, 0x0C, (uint16_t)(sp + 2u));
    return 0;
}

static int pop_w(RbVm *vm, int ent, uint16_t *v)
{
    uint16_t sp = eu16(vm, ent, 0x0C);

    if (sp < 2u) {
        return -1;
    }
    sp = (uint16_t)(sp - 2u);
    su16(vm, ent, 0x0C, sp);
    *v = eu16(vm, ent, 0x0E + sp);
    return 0;
}

/* ---- sub-ops ------------------------------------------------------------ */

static uint16_t sub_skip(uint8_t op)
{
    static const uint8_t len[0x2C] = {
        3, 0, 4, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 4, 0,
        3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 4, 0, 4, 0,
        4, 0, 4, 0, 4, 0, 4, 0, 4, 0, 4
    };

    if (op < 0x2Cu) {
        return len[op];
    }
    return 0;
}

static void skip_subops(RbVm *vm, int script, uint16_t *di)
{
    for (;;) {
        uint8_t op = rd8(vm, script, *di);

        *di = (uint16_t)(*di + 1u);
        if (op == 0xFFu) {
            return;
        }
        *di = (uint16_t)(*di + sub_skip(op));
    }
}

static void one_subop(RbVm *vm, int script, int target, uint16_t *di)
{
    uint8_t op = rd8(vm, script, *di);
    uint8_t a;
    uint8_t b;
    uint8_t alu;
    uint16_t w;
    uint16_t imm;
    uint16_t cur;

    *di = (uint16_t)(*di + 1u);
    if (op == 0xFFu) {
        *di = (uint16_t)(*di - 1u);
        return;
    }
    switch (op) {
    case 0x00:
        a = rd8(vm, script, *di);
        alu = rd8(vm, script, (uint16_t)(*di + 1u));
        b = rd8(vm, script, (uint16_t)(*di + 2u));
        su8(vm, target, a, byte_alu(vm, alu, eu8(vm, target, a), b));
        *di = (uint16_t)(*di + 3u);
        break;
    case 0x02:
        a = rd8(vm, script, *di);
        alu = rd8(vm, script, (uint16_t)(*di + 1u));
        imm = rd16(vm, script, (uint16_t)(*di + 2u));
        su16(vm, target, a, word_alu(vm, alu, eu16(vm, target, a), imm));
        *di = (uint16_t)(*di + 4u);
        break;
    case 0x04:
        a = rd8(vm, script, *di);
        alu = rd8(vm, script, (uint16_t)(*di + 1u));
        b = rd8(vm, script, (uint16_t)(*di + 2u));
        su8(vm, target, a, byte_alu(vm, alu, eu8(vm, target, a), eu8(vm, target, b)));
        *di = (uint16_t)(*di + 3u);
        break;
    case 0x06:
        a = rd8(vm, script, *di);
        alu = rd8(vm, script, (uint16_t)(*di + 1u));
        b = rd8(vm, script, (uint16_t)(*di + 2u));
        su16(vm, target, a, word_alu(vm, alu, eu16(vm, target, a), eu16(vm, target, b)));
        *di = (uint16_t)(*di + 3u);
        break;
    case 0x08:
    case 0x14:
        a = rd8(vm, script, *di);
        alu = rd8(vm, script, (uint16_t)(*di + 1u));
        b = rd8(vm, script, (uint16_t)(*di + 2u));
        su8(vm, target, a, byte_alu(vm, alu, eu8(vm, target, a), vm->g[b]));
        *di = (uint16_t)(*di + 3u);
        break;
    case 0x0A:
    case 0x16:
        a = rd8(vm, script, *di);
        alu = rd8(vm, script, (uint16_t)(*di + 1u));
        b = rd8(vm, script, (uint16_t)(*di + 2u));
        su16(vm, target, a, word_alu(vm, alu, eu16(vm, target, a), gu16(vm, b)));
        *di = (uint16_t)(*di + 3u);
        break;
    case 0x0C:
        a = rd8(vm, script, *di);
        alu = rd8(vm, script, (uint16_t)(*di + 1u));
        b = rd8(vm, script, (uint16_t)(*di + 2u));
        vm->g[a] = byte_alu(vm, alu, vm->g[a], b);
        *di = (uint16_t)(*di + 3u);
        break;
    case 0x0E:
        a = rd8(vm, script, *di);
        alu = rd8(vm, script, (uint16_t)(*di + 1u));
        imm = rd16(vm, script, (uint16_t)(*di + 2u));
        sgu16(vm, a, word_alu(vm, alu, gu16(vm, a), imm));
        *di = (uint16_t)(*di + 4u);
        break;
    case 0x10:
        a = rd8(vm, script, *di);
        alu = rd8(vm, script, (uint16_t)(*di + 1u));
        b = rd8(vm, script, (uint16_t)(*di + 2u));
        vm->g[a] = byte_alu(vm, alu, vm->g[a], eu8(vm, target, b));
        *di = (uint16_t)(*di + 3u);
        break;
    case 0x12:
        a = rd8(vm, script, *di);
        alu = rd8(vm, script, (uint16_t)(*di + 1u));
        b = rd8(vm, script, (uint16_t)(*di + 2u));
        sgu16(vm, a, word_alu(vm, alu, gu16(vm, a), eu16(vm, target, b)));
        *di = (uint16_t)(*di + 3u);
        break;
    case 0x18:
        a = rd8(vm, script, *di);
        alu = rd8(vm, script, (uint16_t)(*di + 1u));
        b = rd8(vm, script, (uint16_t)(*di + 2u));
        su8(vm, target, a,
            byte_alu(vm, alu, eu8(vm, target, a), vm->g[eu8(vm, target, b)]));
        *di = (uint16_t)(*di + 3u);
        break;
    case 0x1A:
        a = rd8(vm, script, *di);
        alu = rd8(vm, script, (uint16_t)(*di + 1u));
        b = rd8(vm, script, (uint16_t)(*di + 2u));
        su16(vm, target, a,
             word_alu(vm, alu, eu16(vm, target, a), gu16(vm, eu8(vm, target, b))));
        *di = (uint16_t)(*di + 3u);
        break;
    case 0x1C:
    case 0x20:
    case 0x24:
    case 0x28:
        *di = (uint16_t)(*di + 4u);
        break;
    case 0x1E:
    case 0x22: {
        int16_t prod;
        uint16_t ang;
        uint8_t dest;
        uint8_t src;
        uint8_t spd;

        dest = rd8(vm, script, *di);
        alu = rd8(vm, script, (uint16_t)(*di + 1u));
        src = rd8(vm, script, (uint16_t)(*di + 2u));
        spd = rd8(vm, script, (uint16_t)(*di + 3u));
        ang = eu16(vm, target, src);
        if (op == 0x22) {
            ang = (uint16_t)(ang + 0x4000u);
        }
        {
            int16_t sample = sincos((uint8_t)(ang >> 8));
            int16_t s = (int16_t)(int8_t)((uint16_t)sample >> 8);
            int16_t m = (int16_t)(int8_t)(eu16(vm, target, spd) >> 8);

            prod = (int16_t)(s * m);
        }
        cur = eu16(vm, target, dest);
        su16(vm, target, dest, word_alu(vm, alu, cur, (uint16_t)prod));
        *di = (uint16_t)(*di + 4u);
        break;
    }
    case 0x26:
    case 0x2A: {
        uint8_t dest = rd8(vm, script, *di);
        uint8_t gx;
        uint8_t gy;
        uint16_t got;

        alu = rd8(vm, script, (uint16_t)(*di + 1u));
        gx = rd8(vm, script, (uint16_t)(*di + 2u));
        gy = rd8(vm, script, (uint16_t)(*di + 3u));
        if (op == 0x2A) {
            vm->aim_ops++;
            got = aim_step(vm, target, gu16(vm, gx), gu16(vm, gy));
        } else {
            got = atan_to(vm, target, gu16(vm, gx), gu16(vm, gy));
        }
        cur = eu16(vm, target, dest);
        su16(vm, target, dest, word_alu(vm, alu, cur, got));
        *di = (uint16_t)(*di + 4u);
        break;
    }
    default:
        w = sub_skip(op);
        *di = (uint16_t)(*di + w);
        break;
    }
}

static void run_subops(RbVm *vm, int script, int target, uint16_t *di)
{
    int guard = 0;

    while (guard++ < 256) {
        if (rd8(vm, script, *di) == 0xFFu) {
            *di = (uint16_t)(*di + 1u);
            return;
        }
        one_subop(vm, script, target, di);
    }
}

/* ---- spawn -------------------------------------------------------------- */

static int alloc_span(RbVm *vm, int from, int count)
{
    int n;

    if (from < 0) {
        from = 0;
    }
    for (n = 0; n < count; n++) {
        int i = from + n;

        if (i >= ENT_N) {
            break;
        }
        if (eu16(vm, i, 0) == 0) {
            clear_slot(vm, i);
            su16(vm, i, 0, 0x8000u);
            vm->mode[i] = MODE_FETCH;
            return i;
        }
    }
    return -1;
}

static int alloc_back(RbVm *vm)
{
    int i;

    for (i = ENT_N - 1; i >= 0; i--) {
        if (eu16(vm, i, 0) == 0) {
            clear_slot(vm, i);
            su16(vm, i, 0, 0x8000u);
            vm->mode[i] = MODE_FETCH;
            return i;
        }
    }
    return -1;
}

static void finish_spawn(RbVm *vm, int parent, int slot, uint16_t child, uint16_t typ)
{
    if (slot < 0) {
        return;
    }
    su16(vm, slot, 0, (uint16_t)(eu16(vm, slot, 0) | typ));
    su16(vm, slot, 8, child);
    vm->cof[slot] = vm->cof[parent];
    vm->origin[slot] = child;
    vm->mode[slot] = MODE_FETCH;
    vm->spawns++;
}

/* ---- native island runner (80186 subset used by the chunks) ------------ */

static uint8_t reg_lo(const uint16_t *r, int i)
{
    if (i < 4) {
        return (uint8_t)r[i];
    }
    return (uint8_t)(r[i - 4] >> 8);
}

static void reg_set_lo(uint16_t *r, int i, uint8_t v)
{
    if (i < 4) {
        r[i] = (uint16_t)((r[i] & 0xFF00u) | v);
    } else {
        r[i - 4] = (uint16_t)((r[i - 4] & 0x00FFu) | ((uint16_t)v << 8));
    }
}

typedef struct Ea {
    int is_reg;
    int reg;
    int seg;
    uint16_t addr;
} Ea;

static int decode_ea(const uint8_t *cs, uint32_t len, uint16_t *ip,
                      const uint16_t *r, int prefixed, int pseg, Ea *ea, uint8_t *modrm)
{
    uint8_t m;
    int mod;
    int rm;
    int16_t disp = 0;
    uint16_t base = 0;
    int uses_bp = 0;

    if (*ip >= len) {
        return -1;
    }
    m = cs[(*ip)++];
    *modrm = m;
    mod = m >> 6;
    rm = m & 7;
    if (mod == 3) {
        ea->is_reg = 1;
        ea->reg = rm;
        ea->seg = SEG_DS;
        ea->addr = 0;
        return 0;
    }
    ea->is_reg = 0;
    if (mod == 1) {
        if (*ip >= len) {
            return -1;
        }
        disp = (int8_t)cs[(*ip)++];
    } else if (mod == 2 || (mod == 0 && rm == 6)) {
        if ((uint32_t)*ip + 1u >= len) {
            return -1;
        }
        disp = (int16_t)read_u16_le(cs + *ip);
        *ip = (uint16_t)(*ip + 2u);
    }
    switch (rm) {
    case 0: base = (uint16_t)(r[3] + r[6]); break;
    case 1: base = (uint16_t)(r[3] + r[7]); break;
    case 2: base = (uint16_t)(r[5] + r[6]); uses_bp = 1; break;
    case 3: base = (uint16_t)(r[5] + r[7]); uses_bp = 1; break;
    case 4: base = r[6]; break;
    case 5: base = r[7]; break;
    case 6:
        if (mod == 0) {
            base = (uint16_t)disp;
            disp = 0;
        } else {
            base = r[5];
            uses_bp = 1;
        }
        break;
    default:
        base = r[3];
        break;
    }
    ea->addr = (uint16_t)(base + (uint16_t)disp);
    if (prefixed) {
        ea->seg = pseg;
    } else {
        ea->seg = uses_bp ? SEG_SS : SEG_DS;
    }
    return 0;
}

static int mem_read(RbVm *vm, int ent, int seg, uint16_t addr, int is_byte, uint16_t *out)
{
    const uint8_t *p = NULL;
    uint32_t n = 0;

    if (seg == SEG_CS || seg == SEG_ES) {
        p = code_of(vm, ent, &n);
    } else if (seg == SEG_SS) {
        p = vm->g;
        n = G_SIZE;
    } else {
        p = vm->ent;
        n = sizeof vm->ent;
    }
    if (!p || (uint32_t)addr >= n) {
        *out = 0;
        return -1;
    }
    if (is_byte) {
        *out = p[addr];
        return 0;
    }
    if ((uint32_t)addr + 1u >= n) {
        *out = 0;
        return -1;
    }
    *out = read_u16_le(p + addr);
    return 0;
}

static void mem_write(RbVm *vm, int ent, int seg, uint16_t addr, int is_byte, uint16_t v)
{
    uint8_t *p = NULL;
    (void)ent;
    uint32_t n = 0;

    if (seg == SEG_SS) {
        p = vm->g;
        n = G_SIZE;
    } else if (seg == SEG_DS) {
        p = vm->ent;
        n = sizeof vm->ent;
    } else {
        return;
    }
    if ((uint32_t)addr >= n) {
        return;
    }
    if (is_byte) {
        p[addr] = (uint8_t)v;
        return;
    }
    if ((uint32_t)addr + 1u >= n) {
        return;
    }
    write_u16_le(p + addr, v);
}

static uint16_t ea_read(RbVm *vm, int ent, const uint16_t *r, const Ea *ea, int is_byte)
{
    uint16_t v = 0;

    if (ea->is_reg) {
        if (is_byte) {
            return reg_lo(r, ea->reg);
        }
        return r[ea->reg & 7];
    }
    mem_read(vm, ent, ea->seg, ea->addr, is_byte, &v);
    return v;
}

static void ea_write(RbVm *vm, int ent, uint16_t *r, const Ea *ea, int is_byte, uint16_t v)
{
    if (ea->is_reg) {
        if (is_byte) {
            reg_set_lo(r, ea->reg, (uint8_t)v);
        } else {
            r[ea->reg & 7] = v;
        }
        return;
    }
    mem_write(vm, ent, ea->seg, ea->addr, is_byte, v);
}

static void flags_logic(uint16_t result, int width, int *cf, int *zf, int *sf, int *of)
{
    *cf = 0;
    *of = 0;
    *zf = result == 0;
    if (width == 8) {
        *zf = (result & 0xFFu) == 0;
        *sf = (result & 0x80u) != 0;
    } else {
        *sf = (result & 0x8000u) != 0;
    }
}

static uint16_t flags_add(uint16_t a, uint16_t b, int cin, int width,
                           int *cf, int *zf, int *sf, int *of)
{
    uint32_t mask = (width == 8) ? 0xFFu : 0xFFFFu;
    uint32_t sign = (width == 8) ? 0x80u : 0x8000u;
    uint32_t r = (a & mask) + (b & mask) + (unsigned)cin;

    *cf = r > mask;
    r &= mask;
    *zf = r == 0;
    *sf = (r & sign) != 0;
    *of = ((~(a ^ b)) & (a ^ r) & sign) != 0;
    return (uint16_t)r;
}

static uint16_t flags_sub(uint16_t a, uint16_t b, int cin, int width,
                           int *cf, int *zf, int *sf, int *of)
{
    uint32_t mask = (width == 8) ? 0xFFu : 0xFFFFu;
    uint32_t sign = (width == 8) ? 0x80u : 0x8000u;
    uint32_t r = (a & mask) - (b & mask) - (unsigned)cin;

    *cf = (r & 0x10000u) != 0 || (width == 8 && (r & 0x100u) != 0);
    /* Borrow is the high bit of the wide subtract. Recompute cleanly. */
    if (width == 8) {
        *cf = (unsigned)(a & 0xFFu) < (unsigned)(b & 0xFFu) + (unsigned)cin;
    } else {
        *cf = (uint32_t)a < (uint32_t)b + (unsigned)cin;
    }
    r &= mask;
    *zf = r == 0;
    *sf = (r & sign) != 0;
    *of = ((a ^ b) & (a ^ r) & sign) != 0;
    return (uint16_t)r;
}

static int jcc(uint8_t cond, int cf, int zf, int sf, int of)
{
    switch (cond & 0x0Eu) {
    case 0x00: return of;
    case 0x02: return cf;
    case 0x04: return zf;
    case 0x06: return cf || zf;
    case 0x08: return sf;
    case 0x0A: return 0;
    case 0x0C: return sf != of;
    case 0x0E: return zf || (sf != of);
    default: return 0;
    }
}

static void x86_native(RbVm *vm, int ent, uint16_t ip0)
{
    uint16_t r[8];
    uint16_t stk[96];
    int sp = 96;
    int cf = 0;
    int zf = 1;
    int sf = 0;
    int of = 0;
    uint16_t ip = ip0;
    uint32_t len = 0;
    const uint8_t *cs = code_of(vm, ent, &len);
    int steps = 0;

    if (!cs) {
        return;
    }
    memset(r, 0, sizeof r);
    r[6] = (uint16_t)(ent * ENT_STR);
    if ((ip0 & 0xFFF0u) == 0x53A0u || ip0 == 0x53ABu || ip0 == 0x53B1u) {
        vm->ship_natives++;
    }
    /* 0x59CB is the left option (OR 0x200, ship X - 16 at 0x59FF).
     * 0x5F7B is the right option (ship X + 16). Chunk 1's OR is at 0x5A05. */
    if (ip0 == 0x59CBu || ip0 == 0x59FFu || ip0 == 0x5A05u || ip0 == 0x5A0Bu ||
        ip0 == 0x5F7Bu) {
        vm->option_natives++;
    }
    while (steps++ < 100000) {
        uint8_t op;
        int prefixed = 0;
        int pseg = SEG_DS;
        int prefs = 0;
        Ea ea;
        uint8_t modrm = 0;
        int is_byte;
        int dir_to_reg;
        int alu;
        uint16_t a;
        uint16_t b;
        uint16_t res;
        int width;

        if ((uint32_t)ip >= len) {
            return;
        }
        for (;;) {
            uint8_t p = cs[ip];

            if (p == 0x26) {
                prefixed = 1;
                pseg = SEG_ES;
                ip++;
            } else if (p == 0x2E) {
                prefixed = 1;
                pseg = SEG_CS;
                ip++;
            } else if (p == 0x36) {
                prefixed = 1;
                pseg = SEG_SS;
                ip++;
            } else if (p == 0x3E) {
                prefixed = 1;
                pseg = SEG_DS;
                ip++;
            } else if (p == 0xF0 || p == 0xF2 || p == 0xF3) {
                ip++;
            } else {
                break;
            }
            if (++prefs > 6 || (uint32_t)ip >= len) {
                return;
            }
        }
        op = cs[ip++];

        if (op == 0xCBu || op == 0xCAu) {
            return;
        }
        if (op == 0xC3u || op == 0xC2u) {
            if (sp >= 96) {
                return;
            }
            ip = stk[sp++];
            if (op == 0xC2u) {
                ip = (uint16_t)(ip); /* imm16 is a stack adjust; our stack is by slot */
                if ((uint32_t)ip + 1u < len) {
                    /* consume the imm so a fall-through would be right; we already popped */
                    (void)read_u16_le(cs + (uint16_t)(ip)); /* not this ip */
                }
            }
            if (op == 0xC2u && (uint32_t)(steps) < 100000u) {
                /* The immediate sits at the caller's next IP, already popped.
                 * C2's immediate is encoded after the opcode, before the return. */
            }
            continue;
        }
        /* C2 immediate was not consumed because we returned via the stack.
         * The bytes after C2 are not on the fall-through path. */

        if (op >= 0x40u && op <= 0x47u) {
            uint16_t old = r[op - 0x40u];
            int ocf = cf;

            r[op - 0x40u] = flags_add(old, 1, 0, 16, &cf, &zf, &sf, &of);
            cf = ocf;
            continue;
        }
        if (op >= 0x48u && op <= 0x4Fu) {
            uint16_t old = r[op - 0x48u];
            int ocf = cf;

            r[op - 0x48u] = flags_sub(old, 1, 0, 16, &cf, &zf, &sf, &of);
            cf = ocf;
            continue;
        }
        if (op >= 0x50u && op <= 0x57u) {
            if (sp <= 0) {
                return;
            }
            stk[--sp] = r[op - 0x50u];
            continue;
        }
        if (op >= 0x58u && op <= 0x5Fu) {
            if (sp >= 96) {
                return;
            }
            r[op - 0x58u] = stk[sp++];
            continue;
        }
        if (op >= 0xB0u && op <= 0xB7u) {
            if ((uint32_t)ip >= len) {
                return;
            }
            reg_set_lo(r, op - 0xB0u, cs[ip++]);
            continue;
        }
        if (op >= 0xB8u && op <= 0xBFu) {
            if ((uint32_t)ip + 1u >= len) {
                return;
            }
            r[op - 0xB8u] = read_u16_le(cs + ip);
            ip = (uint16_t)(ip + 2u);
            continue;
        }
        if (op == 0x90u) {
            continue;
        }
        if (op >= 0x91u && op <= 0x97u) {
            uint16_t tmp = r[0];

            r[0] = r[op - 0x90u];
            r[op - 0x90u] = tmp;
            continue;
        }
        if (op == 0x98u) {
            if (r[0] & 0x80u) {
                r[0] = (uint16_t)(r[0] | 0xFF00u);
            } else {
                r[0] = (uint16_t)(r[0] & 0x00FFu);
            }
            continue;
        }
        if (op == 0x99u) {
            r[2] = (r[0] & 0x8000u) ? 0xFFFFu : 0u;
            continue;
        }
        if (op == 0xF8u) {
            cf = 0;
            continue;
        }
        if (op == 0xF9u) {
            cf = 1;
            continue;
        }
        if (op == 0xFCu || op == 0xFDu) {
            continue;
        }
        if (op == 0xEBu || (op >= 0x70u && op <= 0x7Fu)) {
            int8_t rel;
            int take = 1;

            if ((uint32_t)ip >= len) {
                return;
            }
            rel = (int8_t)cs[ip++];
            if (op != 0xEBu) {
                take = jcc((uint8_t)(op & 0x0Fu), cf, zf, sf, of);
                if (op & 1) {
                    take = !take;
                }
            }
            if (take) {
                ip = (uint16_t)(ip + rel);
            }
            continue;
        }
        if (op == 0xE9u || op == 0xE8u) {
            int16_t rel;

            if ((uint32_t)ip + 1u >= len) {
                return;
            }
            rel = (int16_t)read_u16_le(cs + ip);
            ip = (uint16_t)(ip + 2u);
            if (op == 0xE8u) {
                if (sp <= 0) {
                    return;
                }
                stk[--sp] = ip;
            }
            ip = (uint16_t)(ip + rel);
            continue;
        }
        if (op == 0xE3u) {
            int8_t rel = (int8_t)cs[ip++];

            if (r[1] == 0) {
                ip = (uint16_t)(ip + rel);
            }
            continue;
        }
        if ((op & 0xC0u) == 0x00u && (op & 0x07u) <= 3u && op < 0x40u) {
            int regn;

            is_byte = (op & 1) == 0;
            dir_to_reg = (op & 2) != 0;
            alu = (op >> 3) & 7;
            width = is_byte ? 8 : 16;
            if (decode_ea(cs, len, &ip, r, prefixed, pseg, &ea, &modrm) != 0) {
                return;
            }
            regn = (modrm >> 3) & 7;
            if (dir_to_reg) {
                a = is_byte ? reg_lo(r, regn) : r[regn];
                b = ea_read(vm, ent, r, &ea, is_byte);
            } else {
                a = ea_read(vm, ent, r, &ea, is_byte);
                b = is_byte ? reg_lo(r, regn) : r[regn];
            }
            switch (alu) {
            case 0:
                res = flags_add(a, b, 0, width, &cf, &zf, &sf, &of);
                break;
            case 1:
                res = (uint16_t)(a | b);
                flags_logic(res, width, &cf, &zf, &sf, &of);
                break;
            case 2:
                res = flags_add(a, b, cf, width, &cf, &zf, &sf, &of);
                break;
            case 3:
                res = flags_sub(a, b, cf, width, &cf, &zf, &sf, &of);
                break;
            case 4:
                res = (uint16_t)(a & b);
                flags_logic(res, width, &cf, &zf, &sf, &of);
                break;
            case 5:
                res = flags_sub(a, b, 0, width, &cf, &zf, &sf, &of);
                break;
            case 6:
                res = (uint16_t)(a ^ b);
                flags_logic(res, width, &cf, &zf, &sf, &of);
                break;
            default:
                (void)flags_sub(a, b, 0, width, &cf, &zf, &sf, &of);
                continue;
            }
            if (dir_to_reg) {
                if (is_byte) {
                    reg_set_lo(r, regn, (uint8_t)res);
                } else {
                    r[regn] = res;
                }
            } else {
                ea_write(vm, ent, r, &ea, is_byte, res);
            }
            continue;
        }
        if (op == 0x04u || op == 0x0Cu || op == 0x14u || op == 0x1Cu ||
            op == 0x24u || op == 0x2Cu || op == 0x34u || op == 0x3Cu ||
            op == 0xA8u) {
            uint8_t imm;

            if ((uint32_t)ip >= len) {
                return;
            }
            imm = cs[ip++];
            a = (uint8_t)r[0];
            alu = op >> 3;
            if (op == 0xA8u) {
                flags_logic((uint16_t)(a & imm), 8, &cf, &zf, &sf, &of);
                continue;
            }
            switch (alu) {
            case 0: res = flags_add(a, imm, 0, 8, &cf, &zf, &sf, &of); break;
            case 1: res = (uint16_t)(a | imm); flags_logic(res, 8, &cf, &zf, &sf, &of); break;
            case 2: res = flags_add(a, imm, cf, 8, &cf, &zf, &sf, &of); break;
            case 3: res = flags_sub(a, imm, cf, 8, &cf, &zf, &sf, &of); break;
            case 4: res = (uint16_t)(a & imm); flags_logic(res, 8, &cf, &zf, &sf, &of); break;
            case 5: res = flags_sub(a, imm, 0, 8, &cf, &zf, &sf, &of); break;
            case 6: res = (uint16_t)(a ^ imm); flags_logic(res, 8, &cf, &zf, &sf, &of); break;
            default:
                (void)flags_sub(a, imm, 0, 8, &cf, &zf, &sf, &of);
                continue;
            }
            reg_set_lo(r, 0, (uint8_t)res);
            continue;
        }
        if (op == 0x05u || op == 0x0Du || op == 0x15u || op == 0x1Du ||
            op == 0x25u || op == 0x2Du || op == 0x35u || op == 0x3Du ||
            op == 0xA9u) {
            uint16_t imm;

            if ((uint32_t)ip + 1u >= len) {
                return;
            }
            imm = read_u16_le(cs + ip);
            ip = (uint16_t)(ip + 2u);
            a = r[0];
            alu = op >> 3;
            if (op == 0xA9u) {
                flags_logic((uint16_t)(a & imm), 16, &cf, &zf, &sf, &of);
                continue;
            }
            switch (alu) {
            case 0: res = flags_add(a, imm, 0, 16, &cf, &zf, &sf, &of); break;
            case 1: res = (uint16_t)(a | imm); flags_logic(res, 16, &cf, &zf, &sf, &of); break;
            case 2: res = flags_add(a, imm, cf, 16, &cf, &zf, &sf, &of); break;
            case 3: res = flags_sub(a, imm, cf, 16, &cf, &zf, &sf, &of); break;
            case 4: res = (uint16_t)(a & imm); flags_logic(res, 16, &cf, &zf, &sf, &of); break;
            case 5: res = flags_sub(a, imm, 0, 16, &cf, &zf, &sf, &of); break;
            case 6: res = (uint16_t)(a ^ imm); flags_logic(res, 16, &cf, &zf, &sf, &of); break;
            default:
                (void)flags_sub(a, imm, 0, 16, &cf, &zf, &sf, &of);
                continue;
            }
            r[0] = res;
            continue;
        }
        if (op >= 0x80u && op <= 0x83u) {
            uint16_t imm;
            int regn;

            is_byte = (op & 1) == 0;
            width = is_byte ? 8 : 16;
            if (decode_ea(cs, len, &ip, r, prefixed, pseg, &ea, &modrm) != 0) {
                return;
            }
            alu = (modrm >> 3) & 7;
            if (op == 0x81u) {
                if ((uint32_t)ip + 1u >= len) {
                    return;
                }
                imm = read_u16_le(cs + ip);
                ip = (uint16_t)(ip + 2u);
            } else {
                if ((uint32_t)ip >= len) {
                    return;
                }
                imm = cs[ip++];
                if (op == 0x83u && (imm & 0x80u)) {
                    imm |= 0xFF00u;
                }
            }
            a = ea_read(vm, ent, r, &ea, is_byte);
            regn = 0;
            (void)regn;
            switch (alu) {
            case 0: res = flags_add(a, imm, 0, width, &cf, &zf, &sf, &of); break;
            case 1: res = (uint16_t)((a | imm) & (is_byte ? 0xFFu : 0xFFFFu));
                flags_logic(res, width, &cf, &zf, &sf, &of); break;
            case 2: res = flags_add(a, imm, cf, width, &cf, &zf, &sf, &of); break;
            case 3: res = flags_sub(a, imm, cf, width, &cf, &zf, &sf, &of); break;
            case 4: res = (uint16_t)(a & imm); flags_logic(res, width, &cf, &zf, &sf, &of); break;
            case 5: res = flags_sub(a, imm, 0, width, &cf, &zf, &sf, &of); break;
            case 6: res = (uint16_t)(a ^ imm); flags_logic(res, width, &cf, &zf, &sf, &of); break;
            default:
                (void)flags_sub(a, imm, 0, width, &cf, &zf, &sf, &of);
                continue;
            }
            ea_write(vm, ent, r, &ea, is_byte, res);
            continue;
        }
        if (op == 0x84u || op == 0x85u) {
            int regn;

            is_byte = op == 0x84u;
            if (decode_ea(cs, len, &ip, r, prefixed, pseg, &ea, &modrm) != 0) {
                return;
            }
            regn = (modrm >> 3) & 7;
            a = ea_read(vm, ent, r, &ea, is_byte);
            b = is_byte ? reg_lo(r, regn) : r[regn];
            flags_logic((uint16_t)(a & b), is_byte ? 8 : 16, &cf, &zf, &sf, &of);
            continue;
        }
        if (op == 0x86u || op == 0x87u) {
            int regn;
            uint16_t rv;

            is_byte = op == 0x86u;
            if (decode_ea(cs, len, &ip, r, prefixed, pseg, &ea, &modrm) != 0) {
                return;
            }
            regn = (modrm >> 3) & 7;
            a = ea_read(vm, ent, r, &ea, is_byte);
            rv = is_byte ? reg_lo(r, regn) : r[regn];
            if (is_byte) {
                reg_set_lo(r, regn, (uint8_t)a);
            } else {
                r[regn] = a;
            }
            ea_write(vm, ent, r, &ea, is_byte, rv);
            continue;
        }
        if (op == 0x88u || op == 0x89u || op == 0x8Au || op == 0x8Bu) {
            int regn;

            is_byte = (op & 1) == 0;
            dir_to_reg = (op & 2) != 0;
            if (decode_ea(cs, len, &ip, r, prefixed, pseg, &ea, &modrm) != 0) {
                return;
            }
            regn = (modrm >> 3) & 7;
            if (dir_to_reg) {
                a = ea_read(vm, ent, r, &ea, is_byte);
                if (is_byte) {
                    reg_set_lo(r, regn, (uint8_t)a);
                } else {
                    r[regn] = a;
                }
            } else {
                a = is_byte ? reg_lo(r, regn) : r[regn];
                ea_write(vm, ent, r, &ea, is_byte, a);
            }
            continue;
        }
        if (op == 0x8Eu) {
            if (decode_ea(cs, len, &ip, r, prefixed, pseg, &ea, &modrm) != 0) {
                return;
            }
            (void)ea_read(vm, ent, r, &ea, 0);
            continue;
        }
        if (op == 0x8Cu) {
            if (decode_ea(cs, len, &ip, r, prefixed, pseg, &ea, &modrm) != 0) {
                return;
            }
            ea_write(vm, ent, r, &ea, 0, 0);
            continue;
        }
        if (op == 0xC6u || op == 0xC7u) {
            is_byte = op == 0xC6u;
            if (decode_ea(cs, len, &ip, r, prefixed, pseg, &ea, &modrm) != 0) {
                return;
            }
            if (is_byte) {
                if ((uint32_t)ip >= len) {
                    return;
                }
                ea_write(vm, ent, r, &ea, 1, cs[ip++]);
            } else {
                if ((uint32_t)ip + 1u >= len) {
                    return;
                }
                ea_write(vm, ent, r, &ea, 0, read_u16_le(cs + ip));
                ip = (uint16_t)(ip + 2u);
            }
            continue;
        }
        if (op == 0xFEu || op == 0xFFu) {
            int regn;
            int ocf;

            is_byte = op == 0xFEu;
            width = is_byte ? 8 : 16;
            if (decode_ea(cs, len, &ip, r, prefixed, pseg, &ea, &modrm) != 0) {
                return;
            }
            regn = (modrm >> 3) & 7;
            a = ea_read(vm, ent, r, &ea, is_byte);
            ocf = cf;
            if (regn == 0) {
                res = flags_add(a, 1, 0, width, &cf, &zf, &sf, &of);
                cf = ocf;
                ea_write(vm, ent, r, &ea, is_byte, res);
            } else if (regn == 1) {
                res = flags_sub(a, 1, 0, width, &cf, &zf, &sf, &of);
                cf = ocf;
                ea_write(vm, ent, r, &ea, is_byte, res);
            } else if (regn == 2 && !is_byte) {
                if (sp <= 0) {
                    return;
                }
                stk[--sp] = ip;
                ip = a;
            } else if (regn == 4 && !is_byte) {
                ip = a;
            } else if (regn == 6 && !is_byte) {
                if (sp <= 0) {
                    return;
                }
                stk[--sp] = a;
            } else {
                if (vm->nat_logged < 8) {
                    fprintf(stderr, "vm: native %04X unhandled FF/%d\n", ip0, regn);
                    vm->nat_logged++;
                }
                return;
            }
            continue;
        }
        if (op == 0xF6u || op == 0xF7u) {
            int regn;
            uint16_t imm = 0;

            is_byte = op == 0xF6u;
            width = is_byte ? 8 : 16;
            if (decode_ea(cs, len, &ip, r, prefixed, pseg, &ea, &modrm) != 0) {
                return;
            }
            regn = (modrm >> 3) & 7;
            if (regn == 0) {
                if (is_byte) {
                    imm = cs[ip++];
                } else {
                    imm = read_u16_le(cs + ip);
                    ip = (uint16_t)(ip + 2u);
                }
                a = ea_read(vm, ent, r, &ea, is_byte);
                flags_logic((uint16_t)(a & imm), width, &cf, &zf, &sf, &of);
                continue;
            }
            a = ea_read(vm, ent, r, &ea, is_byte);
            if (regn == 2) {
                res = is_byte ? (uint16_t)(~a & 0xFFu) : (uint16_t)~a;
                ea_write(vm, ent, r, &ea, is_byte, res);
            } else if (regn == 3) {
                res = flags_sub(0, a, 0, width, &cf, &zf, &sf, &of);
                ea_write(vm, ent, r, &ea, is_byte, res);
            } else if (regn == 4 || regn == 5) {
                if (is_byte) {
                    int16_t sa = (int16_t)(int8_t)(r[0] & 0xFFu);
                    int16_t sb = (regn == 5) ? (int16_t)(int8_t)a : (int16_t)(a & 0xFFu);
                    int32_t p = (int32_t)sa * (int32_t)sb;

                    if (regn == 4) {
                        p = (uint8_t)(r[0] & 0xFFu) * (uint8_t)a;
                    }
                    r[0] = (uint16_t)p;
                } else {
                    int32_t p;

                    if (regn == 5) {
                        p = (int32_t)(int16_t)r[0] * (int32_t)(int16_t)a;
                    } else {
                        p = (int32_t)((uint32_t)r[0] * (uint32_t)a);
                    }
                    r[0] = (uint16_t)p;
                    r[2] = (uint16_t)((uint32_t)p >> 16);
                }
            } else if (regn == 6 || regn == 7) {
                if (a == 0) {
                    r[0] = 0;
                    r[2] = 0;
                } else if (is_byte) {
                    uint16_t axv = (uint16_t)(r[0] & 0xFFu);

                    r[0] = (uint16_t)(((axv % a) << 8) | (axv / a));
                } else {
                    uint32_t num = ((uint32_t)r[2] << 16) | r[0];

                    r[0] = (uint16_t)(num / a);
                    r[2] = (uint16_t)(num % a);
                }
            }
            continue;
        }
        if (op == 0xC0u || op == 0xC1u || op == 0xD0u || op == 0xD1u ||
            op == 0xD2u || op == 0xD3u) {
            int regn;
            unsigned n;
            unsigned i;
            uint16_t v;
            uint32_t mask;
            uint32_t sign;

            is_byte = (op & 1) == 0;
            width = is_byte ? 8 : 16;
            mask = is_byte ? 0xFFu : 0xFFFFu;
            sign = is_byte ? 0x80u : 0x8000u;
            if (decode_ea(cs, len, &ip, r, prefixed, pseg, &ea, &modrm) != 0) {
                return;
            }
            regn = (modrm >> 3) & 7;
            if (op <= 0xC1u) {
                n = cs[ip++] & 31u;
            } else if (op >= 0xD2u) {
                n = (r[1] & 0xFFu) & 31u;
            } else {
                n = 1;
            }
            v = ea_read(vm, ent, r, &ea, is_byte) & (uint16_t)mask;
            for (i = 0; i < n; i++) {
                if (regn == 4) { /* SHL */
                    cf = (v & sign) != 0;
                    v = (uint16_t)((v << 1) & mask);
                } else if (regn == 5) { /* SHR */
                    cf = v & 1;
                    v = (uint16_t)((v >> 1) & mask);
                } else if (regn == 7) { /* SAR */
                    cf = v & 1;
                    v = (uint16_t)(((v >> 1) | (v & sign)) & mask);
                } else if (regn == 0) { /* ROL */
                    cf = (v & sign) != 0;
                    v = (uint16_t)(((v << 1) | (cf ? 1u : 0u)) & mask);
                } else if (regn == 1) { /* ROR */
                    cf = v & 1;
                    v = (uint16_t)(((v >> 1) | (cf ? sign : 0u)) & mask);
                }
            }
            zf = v == 0;
            sf = (v & sign) != 0;
            ea_write(vm, ent, r, &ea, is_byte, v);
            continue;
        }
        if (op == 0xA0u || op == 0xA1u) {
            uint16_t addr = read_u16_le(cs + ip);
            uint16_t v = 0;

            ip = (uint16_t)(ip + 2u);
            mem_read(vm, ent, prefixed ? pseg : SEG_DS, addr, op == 0xA0u, &v);
            if (op == 0xA0u) {
                reg_set_lo(r, 0, (uint8_t)v);
            } else {
                r[0] = v;
            }
            continue;
        }
        if (op == 0xA2u || op == 0xA3u) {
            uint16_t addr = read_u16_le(cs + ip);

            ip = (uint16_t)(ip + 2u);
            mem_write(vm, ent, prefixed ? pseg : SEG_DS, addr, op == 0xA2u,
                      op == 0xA2u ? (uint8_t)r[0] : r[0]);
            continue;
        }
        if (op == 0xCDu) {
            uint8_t vec = cs[ip++];

            if (vec == 0x7Fu && (vm->g[0xB9] & 0x04u) == 0) {
                if ((r[0] >> 8) == 0) {
                    sys_play_music((uint16_t)(r[0] & 0xFFu));
                } else {
                    sys_play_sound((uint16_t)(r[0] & 0xFFu), 180);
                }
            }
            continue;
        }
        if (op == 0x68u) {
            if (sp <= 0) {
                return;
            }
            stk[--sp] = read_u16_le(cs + ip);
            ip = (uint16_t)(ip + 2u);
            continue;
        }
        if (op == 0x6Au) {
            if (sp <= 0) {
                return;
            }
            stk[--sp] = (uint16_t)(int16_t)(int8_t)cs[ip++];
            continue;
        }

        vm->nat_miss++;
        vm->nat_miss_ip = ip0;
        if (vm->nat_logged < 12) {
            fprintf(stderr, "vm: native %04X unknown opcode %02X at %04X\n",
                    ip0, op, (unsigned)(ip - 1u));
            vm->nat_logged++;
        }
        return;
    }
    vm->nat_miss++;
}

/* ---- sound opcodes ------------------------------------------------------ */

static void play_al(RbVm *vm, uint8_t ah, uint8_t al)
{
    if (vm->g[0xB9] & 0x04u) {
        return;
    }
    if (ah == 0) {
        if (al != vm->g[0xBB]) {
            vm->g[0xBB] = al;
            sys_play_music(al);
        }
    } else if (ah == 1) {
        sys_play_sound(al, 180);
    } else if (ah == 3) {
        vm->g[0xBB] = 0xFF;
        sys_play_music(0);
    } else if (ah == 4) {
        vm->g[0xBB] = 0xFF;
        sys_play_sound(al, 180);
    }
}

/* ---- scroll ------------------------------------------------------------ */

static void scroll_4b(RbVm *vm)
{
    uint16_t span = (uint16_t)(gu16(vm, 0x7E) << 4);
    int16_t sub = (int16_t)gu16(vm, 0x7A);
    int16_t speed = (int16_t)gu16(vm, 0x82);
    int16_t y;
    uint16_t cell;
    uint16_t old;

    sub = (int16_t)(sub - speed);
    if (sub < 0) {
        sub = (int16_t)(sub + (int16_t)span);
    }
    if ((uint16_t)sub >= span && span != 0) {
        sub = (int16_t)(sub - (int16_t)span);
    }
    sgu16(vm, 0x7A, (uint16_t)sub);

    y = (int16_t)gu16(vm, 0x74);
    y = (int16_t)(y + speed);
    if (y <= 0) {
        y = (int16_t)(y + 0x190);
    }
    while (y > 0x190) {
        y = (int16_t)(y - 0x190);
    }
    sgu16(vm, 0x74, (uint16_t)y);
    sgu16(vm, 0x76, (uint16_t)(0x190 - y));
    vm->g[0xBE] = 0;
    cell = (uint16_t)((uint16_t)sub >> 4);
    old = gu16(vm, 0x7C);
    sgu16(vm, 0x7C, cell);
    if (cell != old) {
        vm->g[0xBE] = 1;
    }
}

static void scroll_4d(RbVm *vm)
{
    int n;

    vm->g[0xC1] = 0;
    for (n = 0; n < 25; n++) {
        int16_t y = (int16_t)gu16(vm, 0x74);
        int16_t cell = (int16_t)gu16(vm, 0x7C);
        int16_t width = (int16_t)gu16(vm, 0x7E);

        y = (int16_t)(y + 0x10);
        if (y > 0x190) {
            y = (int16_t)(y - 0x190);
        }
        sgu16(vm, 0x74, (uint16_t)y);
        sgu16(vm, 0x76, (uint16_t)(0x190 - y));
        cell = (int16_t)(cell - 1);
        if (cell >= width) {
            cell = (int16_t)(cell - width);
        }
        if (cell < 0) {
            cell = (int16_t)(cell + width);
        }
        sgu16(vm, 0x7C, (uint16_t)cell);
        sgu16(vm, 0x7A, (uint16_t)(cell << 4));
    }
}

/* ---- hit tests ---------------------------------------------------------- */

static int boxes_hit(RbVm *vm, int self, int other)
{
    return rb_hit_test((int16_t)eu16(vm, self, 0x22),
                       (int16_t)eu16(vm, self, 0x24),
                       (int16_t)eu16(vm, self, 0x42),
                       (int16_t)eu16(vm, self, 0x44),
                       (int16_t)eu16(vm, self, 0x46),
                       (int16_t)eu16(vm, self, 0x48),
                       (int16_t)eu16(vm, other, 0x22),
                       (int16_t)eu16(vm, other, 0x24),
                       (int16_t)eu16(vm, other, 0x42),
                       (int16_t)eu16(vm, other, 0x44),
                       (int16_t)eu16(vm, other, 0x46),
                       (int16_t)eu16(vm, other, 0x48));
}

static int mask_ok(uint16_t type, uint16_t mask)
{
    return (type & mask) != 0 && (type & 0x0100u) == 0;
}

/* ---- chunk switch (opcode 0x0D) ----------------------------------------- */

static void do_load(RbVm *vm, int ent, uint8_t record, uint8_t header)
{
    uint16_t nip;

    vm->loads++;
    vm->load_record = record;
    vm->load_header = header;
    if (ensure_chunk(vm, record) != 0 || vm->clen[record] < 2u) {
        return;
    }
    if ((unsigned)header * 2u + 1u >= vm->clen[record]) {
        return;
    }
    nip = read_u16_le(vm->chunk[record] + (unsigned)header * 2u);
    vm->cof[ent] = record;
    su16(vm, ent, 8, nip);
    if (record >= 1 && record <= 6) {
        vm->view_chunk = (int)record;
    }
    /* Header 0 of chunk 1 is the boot script. Header 2 is the stage-clear
     * script (chunk 1 IP 0x78C3); it keeps running and may load the next
     * stage itself. */
    if (record == 1 && header == 0) {
        vm->boot = 1;
    }
}

/* ---- one entity --------------------------------------------------------- */

static void note_miss(RbVm *vm, int ent, uint8_t op, uint16_t ip)
{
    vm->op_miss++;
    vm->miss_op = op;
    vm->miss_ip = ip;
    if (!vm->seen_op[op]) {
        vm->seen_op[op] = 1;
        fprintf(stderr, "vm: unknown opcode %02X at chunk %u ip %04X\n",
                op, (unsigned)vm->cof[ent], ip);
    }
    vm->mode[ent] = MODE_FOREVER;
}

static void run_fetch(RbVm *vm, int ent)
{
    int steps = 0;

    while (steps++ < 20000) {
        uint16_t di;
        uint8_t op;
        uint16_t a;
        uint16_t b;
        uint8_t f;
        uint8_t alu;
        uint8_t imm8;
        int ok;

        if (eu16(vm, ent, 0) == 0) {
            return;
        }
        if (vm->g[0xB7] != 0) {
            return;
        }
        if (vm->mode[ent] != MODE_FETCH) {
            return;
        }
        di = eu16(vm, ent, 8);
        op = rd8(vm, ent, di);
        di = (uint16_t)(di + 1u);
        su16(vm, ent, 8, di);

        switch (op) {
        case 0x00:
            vm->mode[ent] = MODE_FOREVER;
            return;
        case 0x01:
            clear_slot(vm, ent);
            return;
        case 0x02: {
            int i;

            for (i = 0; i < ENT_N; i++) {
                if (i != ent) {
                    clear_slot(vm, i);
                }
            }
            break;
        }
        case 0x03:
            sgu16(vm, 0x6C, (uint16_t)(gu16(vm, 0x6C) | 0x8000u));
            sgu16(vm, 0x6E, (uint16_t)(gu16(vm, 0x6E) | 0x8000u));
            vm->quit = 1;
            return;
        case 0x04:
            return;
        case 0x05:
            su16(vm, ent, 8, rd16(vm, ent, di));
            break;
        case 0x06: {
            uint16_t n = rd16(vm, ent, di);

            di = (uint16_t)(di + 2u);
            su16(vm, ent, 8, di);
            if (n <= 1u) {
                vm->mode[ent] = MODE_FETCH;
            } else {
                su16(vm, ent, 6, (uint16_t)(n - 1u));
                vm->mode[ent] = MODE_DELAY;
            }
            return;
        }
        case 0x07:
            if ((gu16(vm, 0x6E) & 0x003Fu) == 0) {
                vm->mode[ent] = MODE_WAIT;
            }
            return;
        case 0x08:
            a = rd16(vm, ent, di);
            di = (uint16_t)(di + 2u);
            if (push_w(vm, ent, di) != 0) {
                clear_slot(vm, ent);
                return;
            }
            su16(vm, ent, 8, a);
            break;
        case 0x09:
            if (pop_w(vm, ent, &a) != 0) {
                clear_slot(vm, ent);
                return;
            }
            su16(vm, ent, 8, a);
            break;
        case 0x0A:
            a = rd16(vm, ent, di);
            di = (uint16_t)(di + 2u);
            su16(vm, ent, 8, di);
            if (push_w(vm, ent, di) != 0 || push_w(vm, ent, a) != 0) {
                clear_slot(vm, ent);
                return;
            }
            break;
        case 0x0B: {
            uint16_t sp = eu16(vm, ent, 0x0C);
            uint16_t count;

            if (sp < 4u) {
                clear_slot(vm, ent);
                return;
            }
            count = eu16(vm, ent, 0x0E + sp - 2u);
            count = (uint16_t)(count - 1u);
            su16(vm, ent, 0x0E + sp - 2u, count);
            if (count == 0) {
                su16(vm, ent, 0x0C, (uint16_t)(sp - 4u));
                su16(vm, ent, 8, di);
            } else {
                su16(vm, ent, 8, eu16(vm, ent, 0x0E + sp - 4u));
            }
            break;
        }
        case 0x0C:
            a = rd16(vm, ent, di);
            su16(vm, ent, 8, (uint16_t)(di + 2u));
            x86_native(vm, ent, a);
            if (eu16(vm, ent, 0) == 0) {
                return;
            }
            break;
        case 0x0D:
            do_load(vm, ent, rd8(vm, ent, (uint16_t)(di + 1u)),
                    rd8(vm, ent, (uint16_t)(di + 3u)));
            break;
        case 0x0E:
            sgu16(vm, 0x60, rd16(vm, ent, di));
            su16(vm, ent, 8, (uint16_t)(di + 2u));
            break;
        case 0x0F:
            sgu16(vm, 0x60, gu16(vm, 0x62));
            break;
        case 0x10:
            a = rd16(vm, ent, di);
            sgu16(vm, 0x60, a);
            sgu16(vm, 0x62, a);
            su16(vm, ent, 8, (uint16_t)(di + 2u));
            break;
        case 0x11:
            f = rd8(vm, ent, di);
            alu = rd8(vm, ent, (uint16_t)(di + 1u));
            imm8 = rd8(vm, ent, (uint16_t)(di + 2u));
            vm->vars[f] = byte_alu(vm, alu, vm->vars[f], imm8);
            su16(vm, ent, 8, (uint16_t)(di + 3u));
            break;
        case 0x12:
            f = rd8(vm, ent, di);
            alu = rd8(vm, ent, (uint16_t)(di + 1u));
            imm8 = rd8(vm, ent, (uint16_t)(di + 2u));
            vm->vars[f] = byte_alu(vm, alu, vm->vars[f], vm->vars[imm8]);
            su16(vm, ent, 8, (uint16_t)(di + 3u));
            break;
        case 0x13:
        case 0x14: {
            uint8_t count = rd8(vm, ent, di);
            uint16_t target = rd16(vm, ent, (uint16_t)(di + 1u));
            uint8_t left = count;

            di = (uint16_t)(di + 3u);
            ok = 1;
            while (left != 0) {
                uint8_t field = rd8(vm, ent, di);
                uint8_t cop = rd8(vm, ent, (uint16_t)(di + 1u));
                uint8_t ib = rd8(vm, ent, (uint16_t)(di + 2u));

                di = (uint16_t)(di + 3u);
                left--;
                if (!cmp_true(cop, vm->vars[field], ib)) {
                    ok = 0;
                    di = (uint16_t)(di + (uint16_t)left * 3u);
                    break;
                }
            }
            if (!ok) {
                su16(vm, ent, 8, di);
            } else if (op == 0x13) {
                su16(vm, ent, 8, target);
            } else {
                if (push_w(vm, ent, di) != 0) {
                    clear_slot(vm, ent);
                    return;
                }
                su16(vm, ent, 8, target);
            }
            break;
        }
        case 0x15:
        case 0x16:
        case 0x17: {
            uint16_t child = rd16(vm, ent, di);
            uint16_t typ = rd16(vm, ent, (uint16_t)(di + 2u));
            int slot = -1;

            if (op == 0x15) {
                slot = alloc_span(vm, 0, ENT_N);
            } else if (op == 0x16) {
                slot = alloc_span(vm, ent, ENT_N - ent);
            } else {
                slot = alloc_back(vm);
            }
            finish_spawn(vm, ent, slot, child, typ);
            su16(vm, ent, 8, (uint16_t)(di + 4u));
            break;
        }
        case 0x18:
        case 0x19: {
            uint16_t child = rd16(vm, ent, di);
            uint16_t typ = rd16(vm, ent, (uint16_t)(di + 2u));
            uint16_t want = rd16(vm, ent, (uint16_t)(di + 4u));
            int slot;

            if (op == 0x18 || (uint32_t)want * ENT_STR >= (uint32_t)ent * ENT_STR) {
                int from = want < ENT_N ? (int)want : ENT_N;
                int count = ENT_N - from;

                slot = alloc_span(vm, from, count);
            } else {
                slot = alloc_span(vm, ent, ENT_N - ent);
            }
            finish_spawn(vm, ent, slot, child, typ);
            su16(vm, ent, 8, (uint16_t)(di + 6u));
            break;
        }
        case 0x1A:
        case 0x1C: {
            uint8_t field = rd8(vm, ent, di);
            uint8_t cop = rd8(vm, ent, (uint16_t)(di + 1u));
            uint16_t rhs;
            uint16_t lhs;

            if (op == 0x1A) {
                rhs = rd8(vm, ent, (uint16_t)(di + 2u));
                lhs = eu8(vm, ent, field);
            } else {
                rhs = eu8(vm, ent, rd8(vm, ent, (uint16_t)(di + 2u)));
                lhs = eu8(vm, ent, field);
            }
            if (cmp_true(cop, lhs, rhs)) {
                su16(vm, ent, 8, rd16(vm, ent, (uint16_t)(di + 3u)));
            } else {
                su16(vm, ent, 8, (uint16_t)(di + 5u));
            }
            break;
        }
        case 0x1B:
        case 0x1D: {
            uint8_t field = rd8(vm, ent, di);
            uint8_t cop = rd8(vm, ent, (uint16_t)(di + 1u));
            uint16_t lhs = (op == 0x1B) ? eu16(vm, ent, field)
                                        : eu16(vm, ent, field);
            uint16_t rhs = (op == 0x1B) ? rd16(vm, ent, (uint16_t)(di + 2u))
                                        : eu16(vm, ent, rd8(vm, ent, (uint16_t)(di + 2u)));
            uint16_t target;
            unsigned skip = (op == 0x1B) ? 6u : 5u;
            unsigned toff = (op == 0x1B) ? 4u : 3u;

            target = rd16(vm, ent, (uint16_t)(di + toff));
            if (cmp_true(cop, lhs, rhs)) {
                su16(vm, ent, 8, target);
            } else {
                su16(vm, ent, 8, (uint16_t)(di + skip));
            }
            break;
        }
        case 0x1E:
        case 0x1F: {
            uint8_t field = rd8(vm, ent, di);
            uint8_t cop = rd8(vm, ent, (uint16_t)(di + 1u));
            uint16_t lhs;
            uint16_t rhs;
            uint16_t target;
            unsigned skip;
            unsigned toff;
            uint16_t ret;

            if (op == 0x1E) {
                lhs = eu8(vm, ent, field);
                rhs = rd8(vm, ent, (uint16_t)(di + 2u));
                toff = 3;
                skip = 5;
            } else {
                lhs = eu16(vm, ent, field);
                rhs = rd16(vm, ent, (uint16_t)(di + 2u));
                toff = 4;
                skip = 6;
            }
            target = rd16(vm, ent, (uint16_t)(di + toff));
            if (cmp_true(cop, lhs, rhs)) {
                ret = (uint16_t)(di + skip);
                if (push_w(vm, ent, ret) != 0) {
                    clear_slot(vm, ent);
                    return;
                }
                su16(vm, ent, 8, target);
            } else {
                su16(vm, ent, 8, (uint16_t)(di + skip));
            }
            break;
        }
        case 0x20:
            run_subops(vm, ent, ent, &di);
            su16(vm, ent, 8, di);
            break;
        case 0x21: {
            uint16_t mask = rd16(vm, ent, di);
            uint16_t start;
            int i;

            di = (uint16_t)(di + 2u);
            start = di;
            for (i = 0; i < ENT_N; i++) {
                uint16_t t = eu16(vm, i, 0);
                uint16_t walk = start;

                if ((t & mask) != 0) {
                    run_subops(vm, ent, i, &walk);
                }
            }
            skip_subops(vm, ent, &di);
            su16(vm, ent, 8, di);
            break;
        }
        case 0x22: {
            uint16_t mask = rd16(vm, ent, di);
            int i;
            int found = -1;

            di = (uint16_t)(di + 2u);
            for (i = 0; i < ENT_N; i++) {
                if ((eu16(vm, i, 0) & mask) != 0) {
                    found = i;
                    break;
                }
            }
            if (found < 0) {
                skip_subops(vm, ent, &di);
            } else {
                run_subops(vm, ent, found, &di);
            }
            su16(vm, ent, 8, di);
            break;
        }
        case 0x23:
        case 0x24: {
            uint16_t id = rd16(vm, ent, di);
            uint16_t start;
            int i;

            di = (uint16_t)(di + 2u);
            start = di;
            if (op == 0x24) {
                int found = -1;

                for (i = 0; i < ENT_N; i++) {
                    if (eu16(vm, i, 0) != 0 && eu16(vm, i, 4) == id) {
                        found = i;
                        break;
                    }
                }
                if (found < 0) {
                    skip_subops(vm, ent, &di);
                } else {
                    run_subops(vm, ent, found, &di);
                }
            } else {
                for (i = 0; i < ENT_N; i++) {
                    uint16_t walk = start;

                    if (eu16(vm, i, 0) != 0 && eu16(vm, i, 4) == id) {
                        run_subops(vm, ent, i, &walk);
                    }
                }
                skip_subops(vm, ent, &di);
            }
            su16(vm, ent, 8, di);
            break;
        }
        case 0x25:
        case 0x27:
        case 0x29: {
            uint8_t field = rd8(vm, ent, di);
            uint8_t cop = rd8(vm, ent, (uint16_t)(di + 1u));
            uint16_t lhs = vm->g[field];
            uint16_t rhs;

            if (op == 0x27) {
                rhs = vm->g[rd8(vm, ent, (uint16_t)(di + 2u))];
            } else {
                rhs = rd8(vm, ent, (uint16_t)(di + 2u));
            }
            a = rd16(vm, ent, (uint16_t)(di + 3u));
            if (cmp_true(cop, lhs, rhs)) {
                if (op == 0x29) {
                    if (push_w(vm, ent, (uint16_t)(di + 5u)) != 0) {
                        clear_slot(vm, ent);
                        return;
                    }
                }
                su16(vm, ent, 8, a);
            } else {
                su16(vm, ent, 8, (uint16_t)(di + 5u));
            }
            break;
        }
        case 0x26:
        case 0x28:
        case 0x2A: {
            uint8_t field = rd8(vm, ent, di);
            uint8_t cop = rd8(vm, ent, (uint16_t)(di + 1u));
            uint16_t lhs = gu16(vm, field);
            uint16_t rhs;
            unsigned skip = (op == 0x26 || op == 0x2A) ? 6u : 5u;
            unsigned toff = (op == 0x26 || op == 0x2A) ? 4u : 3u;

            if (op == 0x28) {
                rhs = gu16(vm, rd8(vm, ent, (uint16_t)(di + 2u)));
            } else if (op == 0x2A) {
                rhs = rd16(vm, ent, (uint16_t)(di + 2u));
            } else {
                rhs = rd16(vm, ent, (uint16_t)(di + 2u));
            }
            a = rd16(vm, ent, (uint16_t)(di + toff));
            if (cmp_true(cop, lhs, rhs)) {
                if (op == 0x2A) {
                    if (push_w(vm, ent, (uint16_t)(di + skip)) != 0) {
                        clear_slot(vm, ent);
                        return;
                    }
                }
                su16(vm, ent, 8, a);
            } else {
                su16(vm, ent, 8, (uint16_t)(di + skip));
            }
            break;
        }
        case 0x2B:
            f = rd8(vm, ent, di);
            alu = rd8(vm, ent, (uint16_t)(di + 1u));
            imm8 = rd8(vm, ent, (uint16_t)(di + 2u));
            vm->g[f] = byte_alu(vm, alu, vm->g[f], imm8);
            su16(vm, ent, 8, (uint16_t)(di + 3u));
            break;
        case 0x2C:
            f = rd8(vm, ent, di);
            alu = rd8(vm, ent, (uint16_t)(di + 1u));
            b = rd16(vm, ent, (uint16_t)(di + 2u));
            sgu16(vm, f, word_alu(vm, alu, gu16(vm, f), b));
            su16(vm, ent, 8, (uint16_t)(di + 4u));
            break;
        case 0x2D:
            f = rd8(vm, ent, di);
            alu = rd8(vm, ent, (uint16_t)(di + 1u));
            imm8 = rd8(vm, ent, (uint16_t)(di + 2u));
            vm->g[f] = byte_alu(vm, alu, vm->g[f], vm->g[imm8]);
            su16(vm, ent, 8, (uint16_t)(di + 3u));
            break;
        case 0x2E:
            f = rd8(vm, ent, di);
            alu = rd8(vm, ent, (uint16_t)(di + 1u));
            imm8 = rd8(vm, ent, (uint16_t)(di + 2u));
            sgu16(vm, f, word_alu(vm, alu, gu16(vm, f), gu16(vm, imm8)));
            su16(vm, ent, 8, (uint16_t)(di + 3u));
            break;
        case 0x2F:
            f = rd8(vm, ent, di);
            alu = rd8(vm, ent, (uint16_t)(di + 1u));
            imm8 = rd8(vm, ent, (uint16_t)(di + 2u));
            vm->g[f] = byte_alu(vm, alu, vm->g[f], eu8(vm, ent, imm8));
            su16(vm, ent, 8, (uint16_t)(di + 3u));
            break;
        case 0x30:
            f = rd8(vm, ent, di);
            alu = rd8(vm, ent, (uint16_t)(di + 1u));
            imm8 = rd8(vm, ent, (uint16_t)(di + 2u));
            sgu16(vm, f, word_alu(vm, alu, gu16(vm, f), eu16(vm, ent, imm8)));
            su16(vm, ent, 8, (uint16_t)(di + 3u));
            break;
        case 0x31:
        case 0x3E:
        case 0x3F:
        case 0x40:
        case 0x47:
            su16(vm, ent, 8, (uint16_t)(di + 2u));
            break;
        case 0x32:
        case 0x33:
        case 0x34:
        case 0x48:
        case 0x49:
            su16(vm, ent, 8, (uint16_t)(di + 5u));
            break;
        case 0x35:
        case 0x36:
        case 0x37:
        case 0x38:
        case 0x39:
            su16(vm, ent, 8, (uint16_t)(di + 4u));
            break;
        case 0x3A:
            su16(vm, ent, 8, (uint16_t)(di + 12u));
            break;
        case 0x3B:
            su16(vm, ent, 8, (uint16_t)(di + 11u));
            break;
        case 0x3C:
        case 0x3D:
        case 0x4A:
            su16(vm, ent, 8, (uint16_t)(di + (op == 0x4A ? 6u : 6u)));
            break;
        case 0x41:
        case 0x42:
        case 0x46:
            imm8 = rd8(vm, ent, di);
            su16(vm, ent, 8, (uint16_t)(di + 1u));
            if (op == 0x41) {
                play_al(vm, 0, imm8);
            } else if (op == 0x42) {
                play_al(vm, 1, imm8);
            } else {
                play_al(vm, 4, imm8);
            }
            break;
        case 0x43:
            f = rd8(vm, ent, di);
            su16(vm, ent, 8, (uint16_t)(di + 1u));
            play_al(vm, 1, vm->g[f]);
            break;
        case 0x44:
            f = rd8(vm, ent, di);
            su16(vm, ent, 8, (uint16_t)(di + 1u));
            play_al(vm, 1, vm->g[f]);
            break;
        case 0x45:
            play_al(vm, 3, 0);
            break;
        case 0x4B:
            scroll_4b(vm);
            break;
        case 0x4C:
        case 0x4E:
            break;
        case 0x4D:
            scroll_4d(vm);
            break;
        case 0x4F:
        case 0x50: {
            uint16_t mask = rd16(vm, ent, di);
            uint16_t start;
            int i;
            int hit = 0;

            di = (uint16_t)(di + 2u);
            start = di;
            for (i = 0; i < ENT_N; i++) {
                uint16_t t = eu16(vm, i, 0);

                if (!mask_ok(t, mask)) {
                    continue;
                }
                if (!boxes_hit(vm, ent, i)) {
                    continue;
                }
                if (op == 0x50) {
                    su16(vm, ent, 0x3C, (uint16_t)(eu16(vm, ent, 0x3C) - 1u));
                    run_subops(vm, ent, i, &di);
                    su16(vm, ent, 8, di);
                    hit = 1;
                    break;
                }
                {
                    uint16_t walk = start;

                    run_subops(vm, ent, i, &walk);
                }
                su16(vm, ent, 0x3C, (uint16_t)(eu16(vm, ent, 0x3C) - 1u));
            }
            if (!hit) {
                if (op == 0x4F) {
                    skip_subops(vm, ent, &di);
                } else {
                    skip_subops(vm, ent, &di);
                }
                su16(vm, ent, 8, di);
            }
            break;
        }
        default:
            note_miss(vm, ent, op, (uint16_t)(di - 1u));
            return;
        }
    }
    vm->step_faults++;
}

static void run_entity(RbVm *vm, int ent)
{
    if (eu16(vm, ent, 0) == 0) {
        return;
    }
    if (vm->mode[ent] == MODE_FOREVER) {
        return;
    }
    if (vm->mode[ent] == MODE_DELAY) {
        uint16_t t = eu16(vm, ent, 6);

        if (t == 0) {
            vm->mode[ent] = MODE_FETCH;
            return;
        }
        t = (uint16_t)(t - 1u);
        su16(vm, ent, 6, t);
        if (t == 0) {
            vm->mode[ent] = MODE_FETCH;
        }
        return;
    }
    if (vm->mode[ent] == MODE_WAIT) {
        if ((gu16(vm, 0x6E) & 0x003Fu) != 0) {
            vm->mode[ent] = MODE_FETCH;
        }
        return;
    }
    run_fetch(vm, ent);
}

/* ---- public ------------------------------------------------------------- */

static uint16_t held_from(const RbInput *in)
{
    uint16_t h = 0;

    if (!in) {
        return 0;
    }
    if (in->buttons & RB_BTN_UP) {
        h |= 0x0001u;
    }
    if (in->buttons & RB_BTN_LEFT) {
        h |= 0x0002u;
    }
    if (in->buttons & RB_BTN_RIGHT) {
        h |= 0x0004u;
    }
    if (in->buttons & RB_BTN_DOWN) {
        h |= 0x0008u;
    }
    if (in->buttons & RB_BTN_A) {
        h |= 0x0010u;
    }
    if (in->buttons & RB_BTN_B) {
        h |= 0x0020u;
    }
    if (in->buttons & RB_BTN_C) {
        h |= 0x0040u;
    }
    if (in->buttons & RB_BTN_START) {
        h |= 0x0080u;
    }
    if (in->buttons & RB_BTN_QUIT) {
        h |= 0x8000u;
    }
    return h;
}

int vm_create(RbVm **out, const RbAssets *assets)
{
    RbVm *vm = (RbVm *)calloc(1, sizeof(*vm));
    unsigned i;

    if (!vm) {
        return -1;
    }
    vm->assets = assets;
    vm->chunk[SCRATCH] = (uint8_t *)calloc(1, 64);
    vm->clen[SCRATCH] = 64;
    if (assets) {
        for (i = 0; i < 7 && i < assets->sgm1.count; i++) {
            (void)ensure_chunk(vm, i);
        }
    }
    *out = vm;
    return 0;
}

void vm_destroy(RbVm *vm)
{
    unsigned i;

    if (!vm) {
        return;
    }
    for (i = 0; i < CHUNK_N; i++) {
        free(vm->chunk[i]);
    }
    free(vm);
}

void vm_reset_play(RbVm *vm, uint16_t stage)
{
    unsigned chunk;
    uint16_t ip;

    if (!vm) {
        return;
    }
    if (stage < 1) {
        stage = 1;
    }
    if (stage > 5) {
        stage = 5;
    }
    chunk = (unsigned)stage + 1u;
    memset(vm->ent, 0, sizeof vm->ent);
    memset(vm->g, 0, sizeof vm->g);
    memset(vm->vars, 0, sizeof vm->vars);
    memset(vm->mode, 0, sizeof vm->mode);
    memset(vm->origin, 0, sizeof vm->origin);
    vm->spawns = 0;
    vm->step_faults = 0;
    vm->op_miss = 0;
    vm->nat_miss = 0;
    vm->aim_ops = 0;
    vm->ship_natives = 0;
    vm->option_natives = 0;
    vm->loads = 0;
    vm->quit = 0;
    vm->boot = 0;
    vm->rng_mix = 0x1234u;
    vm->rng_cf = 0;
    sgu16(vm, 0x5C, 0x1234u);
    sgu16(vm, 0x62, 3);
    sgu16(vm, 0x60, 3);
    /* Chunk 1's pre-stage script (0x9507) arms the mode byte before it
     * loads a stage. The stage chunk never writes it back to 1, and the
     * ship native ignores the mode button while the byte is still 0.
     * A direct stage entry has to keep that armed value. */
    vm->g[0xD5] = 1;
    vm->g[0xD7] = 1;
    vm->view_chunk = (int)chunk;
    if (ensure_chunk(vm, chunk) != 0 || vm->clen[chunk] < 2u) {
        return;
    }
    ip = read_u16_le(vm->chunk[chunk]);
    clear_slot(vm, 0);
    su16(vm, 0, 0, 0x4000u);
    su16(vm, 0, 8, ip);
    vm->cof[0] = (uint8_t)chunk;
    vm->origin[0] = ip;
    vm->mode[0] = MODE_FETCH;
}

void vm_set_hiscore(RbVm *vm, uint32_t score)
{
    if (!vm) {
        return;
    }
    sgu16(vm, 0x8A, (uint16_t)score);
    sgu16(vm, 0x8C, (uint16_t)(score >> 16));
}

void vm_set_input(RbVm *vm, const RbInput *in)
{
    uint16_t held;
    uint16_t edge;

    if (!vm) {
        return;
    }
    held = held_from(in);
    edge = (uint16_t)((~vm->prev_held | 0x0040u) & held);
    vm->held = held;
    sgu16(vm, 0x6C, held);
    sgu16(vm, 0x6E, edge);
    sgu16(vm, 0x70, edge);
    if (edge & 0x8000u) {
        vm->quit = 1;
    }
}

void vm_frame(RbVm *vm)
{
    int i;

    if (!vm) {
        return;
    }
    for (i = 0; i < ENT_N; i++) {
        run_entity(vm, i);
    }
    vm->prev_held = vm->held;
}

void vm_info(const RbVm *vm, RbVmInfo *out)
{
    int i;
    uint16_t alive = 0;

    memset(out, 0, sizeof(*out));
    if (!vm) {
        return;
    }
    for (i = 0; i < ENT_N; i++) {
        if (eu16(vm, i, 0) != 0) {
            alive++;
        }
    }
    out->chunk = (uint16_t)vm->view_chunk;
    out->cell = gu16(vm, 0x7C);
    out->speed = (int16_t)gu16(vm, 0x82);
    out->scroll_y = (int16_t)gu16(vm, 0x74);
    out->lives = (int16_t)gu16(vm, 0x62);
    out->ship_x = (int16_t)gu16(vm, 0x96);
    out->ship_y = (int16_t)gu16(vm, 0x98);
    out->score = (uint32_t)gu16(vm, 0x8E) | ((uint32_t)gu16(vm, 0x90) << 16);
    out->hiscore = (uint32_t)gu16(vm, 0x8A) | ((uint32_t)gu16(vm, 0x8C) << 16);
    out->alive = alive;
    out->spawns = vm->spawns;
    out->step_faults = vm->step_faults;
    out->op_miss = vm->op_miss;
    out->nat_miss = vm->nat_miss;
    out->miss_ip = vm->miss_ip;
    out->miss_op = vm->miss_op;
    out->nat_miss_ip = vm->nat_miss_ip;
    out->aim_ops = vm->aim_ops;
    out->ship_natives = vm->ship_natives;
    out->option_natives = vm->option_natives;
    out->loads = vm->loads;
    out->load_record = vm->load_record;
    out->load_header = vm->load_header;
}

int vm_count_origin(const RbVm *vm, uint16_t script_ip)
{
    int i;
    int n = 0;

    if (!vm) {
        return 0;
    }
    for (i = 0; i < ENT_N; i++) {
        if (eu16(vm, i, 0) != 0 && vm->origin[i] == script_ip) {
            n++;
        }
    }
    return n;
}

int vm_collect_sprites(const RbVm *vm, RbVmSprite *out, int max)
{
    int i;
    int n = 0;

    if (!vm || !out || max <= 0) {
        return 0;
    }
    for (i = 0; i < ENT_N && n < max; i++) {
        uint16_t type = eu16(vm, i, 0);
        int16_t x;
        int16_t y;
        int16_t l;
        int16_t r;
        int16_t t;
        int16_t b;
        RbVmSprite *s;

        if (type == 0) {
            continue;
        }
        x = (int16_t)eu16(vm, i, 0x22);
        y = (int16_t)eu16(vm, i, 0x24);
        l = (int16_t)eu16(vm, i, 0x42);
        r = (int16_t)eu16(vm, i, 0x44);
        t = (int16_t)eu16(vm, i, 0x46);
        b = (int16_t)eu16(vm, i, 0x48);
        if (x == 0 && y == 0 && l == 0 && r == 0 && (type & 0x8000u) == 0) {
            continue;
        }
        s = &out[n++];
        s->x = (int16_t)(x + l);
        s->y = (int16_t)(y + t);
        s->w = (int16_t)(r - l);
        s->h = (int16_t)(b - t);
        if (s->w <= 0) {
            s->w = 6;
            s->x = (int16_t)(x - 3);
        }
        if (s->h <= 0) {
            s->h = 6;
            s->y = (int16_t)(y - 3);
        }
        s->type = type;
        s->id = eu16(vm, i, 4);
        if (s->id == 0x0012u) {
            s->rgba = 0xFF40C0FFu;
        } else if (type & 0x1000u) {
            s->rgba = 0xFFFF40FFu;
        } else if (type & 0x00C0u) {
            s->rgba = 0xFFFF4040u;
        } else if (type & 0x0200u) {
            s->rgba = 0xFF8080FFu;
        } else if (type & 0x0020u) {
            s->rgba = 0xFF40FF80u;
        } else if (type & 0x0010u) {
            s->rgba = 0xFFFFFF40u;
        } else {
            s->rgba = 0xFFFFFFFFu;
        }
    }
    return n;
}

int vm_quit_requested(const RbVm *vm)
{
    return vm && vm->quit;
}

int vm_boot_requested(const RbVm *vm)
{
    return vm && vm->boot;
}

int vm_self_test(RbVm *vm)
{
    uint8_t prog[] = {
        0x06, 0x01, 0x00,
        0x2C, 0x80, 0x00, 0x34, 0x12,
        0x04
    };
    int16_t x_before;
    uint16_t aimed;
    uint16_t ang0;

    if (!vm || !vm->chunk[2] || vm->clen[2] < 0x7000u) {
        fprintf(stderr, "vm self-test: chunk 2 missing (%u)\n",
                vm ? vm->clen[2] : 0);
        return 1;
    }
    if (vm->clen[1] < 40000u) {
        fprintf(stderr, "vm self-test: chunk 1 size %u\n", vm->clen[1]);
        return 1;
    }

    memcpy(vm->chunk[SCRATCH], prog, sizeof prog);
    vm->clen[SCRATCH] = sizeof prog;
    memset(vm->ent, 0, sizeof vm->ent);
    memset(vm->g, 0, sizeof vm->g);
    memset(vm->mode, 0, sizeof vm->mode);
    su16(vm, 0, 0, 0x4000u);
    su16(vm, 0, 8, 0);
    vm->cof[0] = SCRATCH;
    vm->mode[0] = MODE_FETCH;
    vm_frame(vm);
    if (gu16(vm, 0x80) != 0) {
        fprintf(stderr, "vm self-test: delay ran early\n");
        return 2;
    }
    vm_frame(vm);
    if (gu16(vm, 0x80) != 0x1234u) {
        fprintf(stderr, "vm self-test: delay value %04X\n", gu16(vm, 0x80));
        return 3;
    }

    memset(vm->ent, 0, sizeof vm->ent);
    su16(vm, 1, 0, 0x8020u);
    su16(vm, 1, 0x22, 100);
    su16(vm, 1, 0x24, 10);
    su16(vm, 1, 0x26, 0x0180u);
    su16(vm, 1, 0x28, 0x0200u);
    vm->cof[1] = 2;
    x86_native(vm, 1, 0x6F2Eu);
    if ((int16_t)eu16(vm, 1, 0x22) != 101 || (int16_t)eu16(vm, 1, 0x24) != 12) {
        fprintf(stderr, "vm self-test: mover %d,%d\n",
                (int)(int16_t)eu16(vm, 1, 0x22),
                (int)(int16_t)eu16(vm, 1, 0x24));
        return 4;
    }

    memset(slot_ptr(vm, 2), 0, ENT_STR);
    su16(vm, 2, 0, 0x8000u);
    su16(vm, 2, 0x22, 0);
    su16(vm, 2, 0x24, 0);
    su16(vm, 2, 0x2E, 0);
    su16(vm, 2, 0x30, 0x0100u);
    sgu16(vm, 0x9A, 80);
    sgu16(vm, 0x9C, 40);
    ang0 = eu16(vm, 2, 0x2E);
    aimed = aim_step(vm, 2, gu16(vm, 0x9A), gu16(vm, 0x9C));
    x_before = (int16_t)(aimed - ang0);
    if (x_before != 0x0100 && x_before != (int16_t)0xFF00 && aimed != 0) {
        fprintf(stderr, "vm self-test: aim stepped by %d to %04X\n",
                (int)x_before, aimed);
        return 5;
    }
    return 0;
}
