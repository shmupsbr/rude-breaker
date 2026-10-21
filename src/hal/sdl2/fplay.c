/**
 * Compile's FPLAY 2.11a, the INT 7Fh player linked as FPLAY.COM.
 *
 * The image is loaded at offset 0x100 with every segment 0, which is the
 * COM's own segment. SONG.DAT is copied to 0x4000, the address AH=9 reads
 * it to, and near-called init at 0xBD0 counts the tracks. Play, stop, and
 * fade enter the real handler at 0x801 so it returns with IRET. The timer
 * IRQ is the ISR at 0x1000, run from the mixer at the YM2608 timer-A rate.
 *
 * OUT 0x188/0x18A (and the second pair 0x18C/0x18E) go to a YM2608.
 * IN 0x188 returns busy-bit clear; the driver's SFMADR spins while that
 * bit is set. The register file is mirrored so the timer period still
 * comes from 0x24/0x25.
 */
#include "fplay.h"
#include "opna.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEM_N 65536u
#define CF 0x0001u
#define PF 0x0004u
#define AF 0x0010u
#define ZF 0x0040u
#define SF 0x0080u
#define IF 0x0200u
#define DF 0x0400u
#define OF 0x0800u

enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { S_ES, S_CS, S_SS, S_DS };

static uint8_t mem[MEM_N];
static uint16_t r[8];
static uint16_t sreg[4];
static uint16_t ip;
static uint16_t flags;
static int over_on;
static uint16_t over_seg;
static int rep; /* 1 = REP/REPE, 2 = REPNE */
static int ready;
static int unknown_op;
static uint16_t unknown_ip;

static uint8_t opn_reg[2][256];
static uint8_t opn_latch[2];
static unsigned opn_writes;

static int timer_left;
static int isr_busy;

static uint8_t fetch8(void)
{
    uint8_t b = mem[ip];

    ip = (uint16_t)(ip + 1u);
    return b;
}

static uint16_t fetch16(void)
{
    uint16_t w = (uint16_t)mem[ip] | ((uint16_t)mem[(uint16_t)(ip + 1u)] << 8);

    ip = (uint16_t)(ip + 2u);
    return w;
}

static uint32_t linear(uint16_t seg, uint16_t off)
{
    return (((uint32_t)seg << 4) + off) & 0xFFFFu;
}

static uint8_t rd8(uint16_t seg, uint16_t off)
{
    return mem[linear(seg, off)];
}

static void wr8(uint16_t seg, uint16_t off, uint8_t v)
{
    mem[linear(seg, off)] = v;
}

static uint16_t rd16(uint16_t seg, uint16_t off)
{
    uint32_t a = linear(seg, off);

    return (uint16_t)mem[a] | ((uint16_t)mem[(a + 1u) & 0xFFFFu] << 8);
}

static void wr16(uint16_t seg, uint16_t off, uint16_t v)
{
    uint32_t a = linear(seg, off);

    mem[a] = (uint8_t)v;
    mem[(a + 1u) & 0xFFFFu] = (uint8_t)(v >> 8);
}

static void push16(uint16_t v)
{
    r[R_SP] = (uint16_t)(r[R_SP] - 2u);
    wr16(sreg[S_SS], r[R_SP], v);
}

static uint16_t pop16(void)
{
    uint16_t v = rd16(sreg[S_SS], r[R_SP]);

    r[R_SP] = (uint16_t)(r[R_SP] + 2u);
    return v;
}

static int parity(uint8_t v)
{
    v = (uint8_t)(v ^ (v >> 4));
    v = (uint8_t)(v ^ (v >> 2));
    v = (uint8_t)(v ^ (v >> 1));
    return (v & 1u) == 0;
}

static void logic_flags(unsigned result, int width)
{
    uint8_t low = (uint8_t)result;

    flags = (uint16_t)(flags & ~(CF | PF | AF | ZF | SF | OF));
    if (parity(low)) {
        flags = (uint16_t)(flags | PF);
    }
    if (low == 0 && (width == 8 || (result & 0xFF00u) == 0)) {
        flags = (uint16_t)(flags | ZF);
    }
    if (width == 8) {
        if (result & 0x80u) {
            flags = (uint16_t)(flags | SF);
        }
    } else if (result & 0x8000u) {
        flags = (uint16_t)(flags | SF);
    }
}

static void arith_flags(unsigned a, unsigned b, unsigned result, int sub, int width)
{
    unsigned sign = (width == 8) ? 0x80u : 0x8000u;
    unsigned mask = (width == 8) ? 0xFFu : 0xFFFFu;

    flags = (uint16_t)(flags & ~(CF | PF | AF | ZF | SF | OF));
    if (parity((uint8_t)result)) {
        flags = (uint16_t)(flags | PF);
    }
    if ((result & mask) == 0) {
        flags = (uint16_t)(flags | ZF);
    }
    if (result & sign) {
        flags = (uint16_t)(flags | SF);
    }
    if (width == 8) {
        if (result & 0x100u) {
            flags = (uint16_t)(flags | CF);
        }
    } else if (result & 0x10000u) {
        flags = (uint16_t)(flags | CF);
    }
    if (((a ^ (sub ? ~b : b) ^ result) & 0x10u) != 0) {
        flags = (uint16_t)(flags | AF);
    }
    if (((a ^ (sub ? b : ~b)) & (a ^ result) & sign) != 0) {
        flags = (uint16_t)(flags | OF);
    }
}

static uint16_t seg_default(int base_bp)
{
    if (over_on) {
        return over_seg;
    }
    return base_bp ? sreg[S_SS] : sreg[S_DS];
}

/* Decode ModR/M. *isreg is set for a register operand. */
static uint16_t ea(uint8_t modrm, int *isreg, int *regn, uint16_t *seg)
{
    int mod = modrm >> 6;
    int rm = modrm & 7;
    uint16_t off = 0;
    int bp = 0;

    if (mod == 3) {
        *isreg = 1;
        *regn = rm;
        *seg = 0;
        return 0;
    }
    *isreg = 0;
    switch (rm) {
    case 0:
        off = (uint16_t)(r[R_BX] + r[R_SI]);
        break;
    case 1:
        off = (uint16_t)(r[R_BX] + r[R_DI]);
        break;
    case 2:
        off = (uint16_t)(r[R_BP] + r[R_SI]);
        bp = 1;
        break;
    case 3:
        off = (uint16_t)(r[R_BP] + r[R_DI]);
        bp = 1;
        break;
    case 4:
        off = r[R_SI];
        break;
    case 5:
        off = r[R_DI];
        break;
    case 6:
        if (mod == 0) {
            off = fetch16();
        } else {
            off = r[R_BP];
            bp = 1;
        }
        break;
    default:
        off = r[R_BX];
        break;
    }
    if (mod == 1) {
        off = (uint16_t)(off + (uint16_t)(int16_t)(int8_t)fetch8());
    } else if (mod == 2) {
        off = (uint16_t)(off + fetch16());
    }
    *seg = seg_default(bp);
    return off;
}

static uint8_t reg8(int n)
{
    if (n < 4) {
        return (uint8_t)r[n];
    }
    return (uint8_t)(r[n - 4] >> 8);
}

static void set_reg8(int n, uint8_t v)
{
    if (n < 4) {
        r[n] = (uint16_t)((r[n] & 0xFF00u) | v);
    } else {
        r[n - 4] = (uint16_t)((r[n - 4] & 0x00FFu) | ((uint16_t)v << 8));
    }
}

static uint8_t get8(int isreg, int regn, uint16_t seg, uint16_t off)
{
    if (isreg) {
        return reg8(regn);
    }
    return rd8(seg, off);
}

static void put8(int isreg, int regn, uint16_t seg, uint16_t off, uint8_t v)
{
    if (isreg) {
        set_reg8(regn, v);
    } else {
        wr8(seg, off, v);
    }
}

static uint16_t get16(int isreg, int regn, uint16_t seg, uint16_t off)
{
    if (isreg) {
        return r[regn];
    }
    return rd16(seg, off);
}

static void put16(int isreg, int regn, uint16_t seg, uint16_t off, uint16_t v)
{
    if (isreg) {
        r[regn] = v;
    } else {
        wr16(seg, off, v);
    }
}

static unsigned alu8(int op, unsigned a, unsigned b)
{
    unsigned rslt;
    int cf = (flags & CF) ? 1 : 0;

    switch (op) {
    case 0:
        rslt = a + b;
        arith_flags(a, b, rslt, 0, 8);
        return rslt & 0xFFu;
    case 1:
        rslt = a | b;
        logic_flags(rslt, 8);
        return rslt;
    case 2:
        rslt = a + b + (unsigned)cf;
        arith_flags(a, b, rslt, 0, 8);
        return rslt & 0xFFu;
    case 3:
        rslt = a - b - (unsigned)cf;
        arith_flags(a, b, rslt, 1, 8);
        return rslt & 0xFFu;
    case 4:
        rslt = a & b;
        logic_flags(rslt, 8);
        return rslt;
    case 5:
        rslt = a - b;
        arith_flags(a, b, rslt, 1, 8);
        return rslt & 0xFFu;
    case 6:
        rslt = a ^ b;
        logic_flags(rslt, 8);
        return rslt;
    default:
        rslt = a - b;
        arith_flags(a, b, rslt, 1, 8);
        return a;
    }
}

static unsigned alu16(int op, unsigned a, unsigned b)
{
    unsigned rslt;
    int cf = (flags & CF) ? 1 : 0;

    switch (op) {
    case 0:
        rslt = a + b;
        arith_flags(a, b, rslt, 0, 16);
        return rslt & 0xFFFFu;
    case 1:
        rslt = a | b;
        logic_flags(rslt, 16);
        return rslt;
    case 2:
        rslt = a + b + (unsigned)cf;
        arith_flags(a, b, rslt, 0, 16);
        return rslt & 0xFFFFu;
    case 3:
        rslt = a - b - (unsigned)cf;
        arith_flags(a, b, rslt, 1, 16);
        return rslt & 0xFFFFu;
    case 4:
        rslt = a & b;
        logic_flags(rslt, 16);
        return rslt;
    case 5:
        rslt = a - b;
        arith_flags(a, b, rslt, 1, 16);
        return rslt & 0xFFFFu;
    case 6:
        rslt = a ^ b;
        logic_flags(rslt, 16);
        return rslt;
    default:
        rslt = a - b;
        arith_flags(a, b, rslt, 1, 16);
        return a;
    }
}

static int jcc(int cond)
{
    int cf = (flags & CF) != 0;
    int zf = (flags & ZF) != 0;
    int sf = (flags & SF) != 0;
    int of = (flags & OF) != 0;
    int pf = (flags & PF) != 0;

    switch (cond) {
    case 0:
        return of;
    case 1:
        return !of;
    case 2:
        return cf;
    case 3:
        return !cf;
    case 4:
        return zf;
    case 5:
        return !zf;
    case 6:
        return cf || zf;
    case 7:
        return !cf && !zf;
    case 8:
        return sf;
    case 9:
        return !sf;
    case 10:
        return pf;
    case 11:
        return !pf;
    case 12:
        return sf != of;
    case 13:
        return sf == of;
    case 14:
        return zf || (sf != of);
    default:
        return !zf && (sf == of);
    }
}

static void shift8(int kind, int isreg, int regn, uint16_t seg, uint16_t off, unsigned count)
{
    uint8_t v;
    unsigned c;
    unsigned i;
    int cf;

    count &= 31u;
    if (count == 0) {
        return;
    }
    v = get8(isreg, regn, seg, off);
    cf = (flags & CF) != 0;
    for (i = 0; i < count; i++) {
        int msb = (v & 0x80u) != 0;
        int lsb = v & 1;

        switch (kind & 7) {
        case 0:
            cf = msb;
            v = (uint8_t)((v << 1) | (cf ? 1 : 0));
            break;
        case 1:
            cf = lsb;
            v = (uint8_t)((v >> 1) | (cf ? 0x80u : 0));
            break;
        case 2:
            c = cf ? 1u : 0u;
            cf = msb;
            v = (uint8_t)((v << 1) | c);
            break;
        case 3:
            c = cf ? 0x80u : 0u;
            cf = lsb;
            v = (uint8_t)((v >> 1) | c);
            break;
        case 6:
        case 4:
            cf = msb;
            v = (uint8_t)(v << 1);
            break;
        case 5:
            cf = lsb;
            v = (uint8_t)(v >> 1);
            break;
        default:
            cf = lsb;
            v = (uint8_t)(((int8_t)v) >> 1);
            break;
        }
    }
    put8(isreg, regn, seg, off, v);
    logic_flags(v, 8);
    if (cf) {
        flags = (uint16_t)(flags | CF);
    }
    if (count == 1u && (kind & 7) <= 4u) {
        int of = ((v & 0x80u) != 0) != cf;

        if ((kind & 7) == 0 || (kind & 7) == 4 || (kind & 7) == 2) {
            if (of) {
                flags = (uint16_t)(flags | OF);
            }
        }
    }
}

static void shift16(int kind, int isreg, int regn, uint16_t seg, uint16_t off, unsigned count)
{
    uint16_t v;
    unsigned i;
    int cf;

    count &= 31u;
    if (count == 0) {
        return;
    }
    v = get16(isreg, regn, seg, off);
    cf = (flags & CF) != 0;
    for (i = 0; i < count; i++) {
        int msb = (v & 0x8000u) != 0;
        int lsb = v & 1;
        unsigned c;

        switch (kind & 7) {
        case 0:
            cf = msb;
            v = (uint16_t)((v << 1) | (cf ? 1 : 0));
            break;
        case 1:
            cf = lsb;
            v = (uint16_t)((v >> 1) | (cf ? 0x8000u : 0));
            break;
        case 2:
            c = cf ? 1u : 0u;
            cf = msb;
            v = (uint16_t)((v << 1) | c);
            break;
        case 3:
            c = cf ? 0x8000u : 0u;
            cf = lsb;
            v = (uint16_t)((v >> 1) | c);
            break;
        case 6:
        case 4:
            cf = msb;
            v = (uint16_t)(v << 1);
            break;
        case 5:
            cf = lsb;
            v = (uint16_t)(v >> 1);
            break;
        default:
            cf = lsb;
            v = (uint16_t)(((int16_t)v) >> 1);
            break;
        }
    }
    put16(isreg, regn, seg, off, v);
    logic_flags(v, 16);
    if (cf) {
        flags = (uint16_t)(flags | CF);
    }
}

static void mul8(uint8_t src, int signed_mul)
{
    if (signed_mul) {
        int16_t p = (int16_t)((int8_t)reg8(0) * (int8_t)src);

        r[R_AX] = (uint16_t)p;
        if ((int8_t)reg8(0) == (int8_t)p) {
            flags = (uint16_t)(flags & ~(CF | OF));
        } else {
            flags = (uint16_t)(flags | CF | OF);
        }
    } else {
        uint16_t p = (uint16_t)reg8(0) * src;

        r[R_AX] = p;
        if ((p & 0xFF00u) != 0) {
            flags = (uint16_t)(flags | CF | OF);
        } else {
            flags = (uint16_t)(flags & ~(CF | OF));
        }
    }
}

static void mul16(uint16_t src, int signed_mul)
{
    if (signed_mul) {
        int32_t p = (int32_t)(int16_t)r[R_AX] * (int32_t)(int16_t)src;

        r[R_AX] = (uint16_t)p;
        r[R_DX] = (uint16_t)((uint32_t)p >> 16);
        if ((int16_t)r[R_AX] == p) {
            flags = (uint16_t)(flags & ~(CF | OF));
        } else {
            flags = (uint16_t)(flags | CF | OF);
        }
    } else {
        uint32_t p = (uint32_t)r[R_AX] * src;

        r[R_AX] = (uint16_t)p;
        r[R_DX] = (uint16_t)(p >> 16);
        if (r[R_DX] != 0) {
            flags = (uint16_t)(flags | CF | OF);
        } else {
            flags = (uint16_t)(flags & ~(CF | OF));
        }
    }
}

static void div8(uint8_t src, int signed_div)
{
    if (src == 0) {
        r[R_AX] = 0xFFFFu;
        return;
    }
    if (signed_div) {
        int16_t n = (int16_t)r[R_AX];
        int8_t d = (int8_t)src;
        int q = n / d;
        int m = n % d;

        set_reg8(0, (uint8_t)q);
        set_reg8(4, (uint8_t)m);
    } else {
        unsigned n = r[R_AX];
        unsigned q = n / src;
        unsigned m = n % src;

        set_reg8(0, (uint8_t)q);
        set_reg8(4, (uint8_t)m);
    }
}

static void div16(uint16_t src, int signed_div)
{
    if (src == 0) {
        return;
    }
    if (signed_div) {
        int32_t n = (int32_t)(((uint32_t)r[R_DX] << 16) | r[R_AX]);
        int32_t d = (int16_t)src;

        r[R_AX] = (uint16_t)(n / d);
        r[R_DX] = (uint16_t)(n % d);
    } else {
        uint32_t n = ((uint32_t)r[R_DX] << 16) | r[R_AX];

        r[R_AX] = (uint16_t)(n / src);
        r[R_DX] = (uint16_t)(n % src);
    }
}

static void string_step(uint16_t *reg, int width)
{
    if (flags & DF) {
        *reg = (uint16_t)(*reg - (uint16_t)width);
    } else {
        *reg = (uint16_t)(*reg + (uint16_t)width);
    }
}

static void do_string(int op)
{
    int width = (op & 1) ? 2 : 1;
    uint16_t src_seg = over_on ? over_seg : sreg[S_DS];

    for (;;) {
        switch (op) {
        case 0xA4:
            wr8(sreg[S_ES], r[R_DI], rd8(src_seg, r[R_SI]));
            string_step(&r[R_SI], 1);
            string_step(&r[R_DI], 1);
            break;
        case 0xA5:
            wr16(sreg[S_ES], r[R_DI], rd16(src_seg, r[R_SI]));
            string_step(&r[R_SI], 2);
            string_step(&r[R_DI], 2);
            break;
        case 0xAA:
            wr8(sreg[S_ES], r[R_DI], reg8(0));
            string_step(&r[R_DI], 1);
            break;
        case 0xAB:
            wr16(sreg[S_ES], r[R_DI], r[R_AX]);
            string_step(&r[R_DI], 2);
            break;
        case 0xAC:
            set_reg8(0, rd8(src_seg, r[R_SI]));
            string_step(&r[R_SI], 1);
            break;
        case 0xAD:
            r[R_AX] = rd16(src_seg, r[R_SI]);
            string_step(&r[R_SI], 2);
            break;
        case 0xA6:
        case 0xA7: {
            unsigned a = (width == 1) ? rd8(src_seg, r[R_SI]) : rd16(src_seg, r[R_SI]);
            unsigned b = (width == 1) ? rd8(sreg[S_ES], r[R_DI]) : rd16(sreg[S_ES], r[R_DI]);

            if (width == 1) {
                alu8(7, a, b);
            } else {
                alu16(7, a, b);
            }
            string_step(&r[R_SI], width);
            string_step(&r[R_DI], width);
            break;
        }
        case 0xAE:
            alu8(7, reg8(0), rd8(sreg[S_ES], r[R_DI]));
            string_step(&r[R_DI], 1);
            break;
        case 0xAF:
            alu16(7, r[R_AX], rd16(sreg[S_ES], r[R_DI]));
            string_step(&r[R_DI], 2);
            break;
        default:
            return;
        }
        if (rep == 0) {
            return;
        }
        r[R_CX] = (uint16_t)(r[R_CX] - 1u);
        if (r[R_CX] == 0) {
            return;
        }
        if ((op == 0xA6 || op == 0xA7 || op == 0xAE || op == 0xAF)) {
            int zf = (flags & ZF) != 0;

            if (rep == 1 && !zf) {
                return;
            }
            if (rep == 2 && zf) {
                return;
            }
        }
    }
}

static void group_ff(uint8_t modrm)
{
    int isreg = 0;
    int regn = 0;
    uint16_t seg = 0;
    uint16_t off = ea(modrm, &isreg, &regn, &seg);
    int kind = (modrm >> 3) & 7;
    uint16_t v;
    uint16_t nxt;

    if (kind == 0) {
        v = get16(isreg, regn, seg, off);
        arith_flags(v, 1, v + 1u, 0, 16);
        put16(isreg, regn, seg, off, (uint16_t)(v + 1u));
        return;
    }
    if (kind == 1) {
        v = get16(isreg, regn, seg, off);
        arith_flags(v, 1, v - 1u, 1, 16);
        put16(isreg, regn, seg, off, (uint16_t)(v - 1u));
        return;
    }
    if (kind == 6) {
        push16(get16(isreg, regn, seg, off));
        return;
    }
    if (kind == 2) {
        nxt = ip;
        push16(nxt);
        ip = get16(isreg, regn, seg, off);
        return;
    }
    if (kind == 4) {
        ip = get16(isreg, regn, seg, off);
        return;
    }
    if (kind == 3 || kind == 5) {
        uint16_t nip = get16(isreg, regn, seg, off);
        uint16_t ncs = rd16(seg, (uint16_t)(off + 2u));

        if (kind == 3) {
            push16(sreg[S_CS]);
            push16(ip);
        }
        ip = nip;
        sreg[S_CS] = ncs;
    }
}

static int cpu_step(void)
{
    uint8_t op = fetch8();
    uint8_t modrm;
    int isreg;
    int regn;
    uint16_t seg;
    uint16_t off;
    unsigned a;
    unsigned b;

    if (op == 0xF4) {
        return 1;
    }
    if (op == 0x26 || op == 0x2E || op == 0x36 || op == 0x3E) {
        static const int map[4] = { S_ES, S_CS, S_SS, S_DS };

        over_on = 1;
        over_seg = sreg[map[(op >> 3) & 3]];
        return 0;
    }
    if (op == 0xF2) {
        rep = 2;
        return 0;
    }
    if (op == 0xF3) {
        rep = 1;
        return 0;
    }
    if (op == 0xF0 || op == 0x66) {
        return 0;
    }
    /* 04/05, 0C/0D, ... 3C/3D: ALU immediate into AL/AX, no ModR/M. */
    if (op < 0x40u && ((op & 7u) == 4u || (op & 7u) == 5u)) {
        int alu = (op >> 3) & 7;

        if ((op & 1u) == 0) {
            uint8_t res = (uint8_t)alu8(alu, reg8(0), fetch8());

            if (alu != 7) {
                set_reg8(0, res);
            }
        } else {
            uint16_t res = (uint16_t)alu16(alu, r[R_AX], fetch16());

            if (alu != 7) {
                r[R_AX] = res;
            }
        }
        return 0;
    }
    if (op < 0x40u && (op & 7u) < 4u) {
        int wide = op & 1;
        int dir = op & 2;
        int alu = (op >> 3) & 7;

        modrm = fetch8();
        off = ea(modrm, &isreg, &regn, &seg);
        if (wide) {
            uint16_t rm = get16(isreg, regn, seg, off);
            uint16_t reg = r[(modrm >> 3) & 7];
            uint16_t res;

            if (dir) {
                res = (uint16_t)alu16(alu, reg, rm);
                if (alu != 7) {
                    r[(modrm >> 3) & 7] = res;
                }
            } else {
                res = (uint16_t)alu16(alu, rm, reg);
                if (alu != 7) {
                    put16(isreg, regn, seg, off, res);
                }
            }
        } else {
            uint8_t rm = get8(isreg, regn, seg, off);
            uint8_t reg = reg8((modrm >> 3) & 7);
            uint8_t res;

            if (dir) {
                res = (uint8_t)alu8(alu, reg, rm);
                if (alu != 7) {
                    set_reg8((modrm >> 3) & 7, res);
                }
            } else {
                res = (uint8_t)alu8(alu, rm, reg);
                if (alu != 7) {
                    put8(isreg, regn, seg, off, res);
                }
            }
        }
        return 0;
    }
    if (op >= 0x40u && op <= 0x47u) {
        a = r[op & 7];
        arith_flags(a, 1, a + 1u, 0, 16);
        r[op & 7] = (uint16_t)(a + 1u);
        return 0;
    }
    if (op >= 0x48u && op <= 0x4Fu) {
        a = r[op & 7];
        arith_flags(a, 1, a - 1u, 1, 16);
        r[op & 7] = (uint16_t)(a - 1u);
        return 0;
    }
    if (op >= 0x50u && op <= 0x57u) {
        push16(r[op & 7]);
        return 0;
    }
    if (op >= 0x58u && op <= 0x5Fu) {
        r[op & 7] = pop16();
        return 0;
    }
    if (op == 0x60) {
        uint16_t sp = r[R_SP];
        int i;

        for (i = 0; i < 8; i++) {
            push16(i == R_SP ? sp : r[i]);
        }
        return 0;
    }
    if (op == 0x61) {
        int i;

        for (i = 7; i >= 0; i--) {
            r[i] = pop16();
        }
        return 0;
    }
    if (op == 0x68) {
        push16(fetch16());
        return 0;
    }
    if (op == 0x6A) {
        push16((uint16_t)(int16_t)(int8_t)fetch8());
        return 0;
    }
    if (op >= 0x70u && op <= 0x7Fu) {
        int8_t rel = (int8_t)fetch8();

        if (jcc(op & 0x0F)) {
            ip = (uint16_t)(ip + (uint16_t)(int16_t)rel);
        }
        return 0;
    }
    if (op >= 0x80u && op <= 0x83u) {
        int wide = (op & 1u) != 0;
        int alu;
        unsigned imm;

        modrm = fetch8();
        alu = (modrm >> 3) & 7;
        off = ea(modrm, &isreg, &regn, &seg);
        if (op == 0x80) {
            imm = fetch8();
            a = get8(isreg, regn, seg, off);
            b = alu8(alu, a, imm);
            if (alu != 7) {
                put8(isreg, regn, seg, off, (uint8_t)b);
            }
        } else if (op == 0x81) {
            imm = fetch16();
            a = get16(isreg, regn, seg, off);
            b = alu16(alu, a, imm);
            if (alu != 7) {
                put16(isreg, regn, seg, off, (uint16_t)b);
            }
        } else if (op == 0x82 || op == 0x83) {
            imm = (uint16_t)(int16_t)(int8_t)fetch8();
            if (!wide && op == 0x82) {
                a = get8(isreg, regn, seg, off);
                b = alu8(alu, a, imm & 0xFFu);
                if (alu != 7) {
                    put8(isreg, regn, seg, off, (uint8_t)b);
                }
            } else {
                a = get16(isreg, regn, seg, off);
                b = alu16(alu, a, imm);
                if (alu != 7) {
                    put16(isreg, regn, seg, off, (uint16_t)b);
                }
            }
        }
        return 0;
    }
    if (op == 0x84 || op == 0x85) {
        modrm = fetch8();
        off = ea(modrm, &isreg, &regn, &seg);
        if (op == 0x84) {
            alu8(4, get8(isreg, regn, seg, off), reg8((modrm >> 3) & 7));
        } else {
            alu16(4, get16(isreg, regn, seg, off), r[(modrm >> 3) & 7]);
        }
        return 0;
    }
    if (op == 0x86 || op == 0x87) {
        uint8_t b8;
        uint16_t b16;
        int reg = 0;

        modrm = fetch8();
        reg = (modrm >> 3) & 7;
        off = ea(modrm, &isreg, &regn, &seg);
        if (op == 0x86) {
            b8 = get8(isreg, regn, seg, off);
            put8(isreg, regn, seg, off, reg8(reg));
            set_reg8(reg, b8);
        } else {
            b16 = get16(isreg, regn, seg, off);
            put16(isreg, regn, seg, off, r[reg]);
            r[reg] = b16;
        }
        return 0;
    }
    if (op >= 0x88u && op <= 0x8Bu) {
        int wide = op & 1;
        int dir = op & 2;

        modrm = fetch8();
        off = ea(modrm, &isreg, &regn, &seg);
        if (!wide) {
            if (dir) {
                set_reg8((modrm >> 3) & 7, get8(isreg, regn, seg, off));
            } else {
                put8(isreg, regn, seg, off, reg8((modrm >> 3) & 7));
            }
        } else if (dir) {
            r[(modrm >> 3) & 7] = get16(isreg, regn, seg, off);
        } else {
            put16(isreg, regn, seg, off, r[(modrm >> 3) & 7]);
        }
        return 0;
    }
    if (op == 0x8C) {
        modrm = fetch8();
        off = ea(modrm, &isreg, &regn, &seg);
        put16(isreg, regn, seg, off, sreg[(modrm >> 3) & 3]);
        return 0;
    }
    if (op == 0x8D) {
        modrm = fetch8();
        off = ea(modrm, &isreg, &regn, &seg);
        r[(modrm >> 3) & 7] = off;
        return 0;
    }
    if (op == 0x8E) {
        modrm = fetch8();
        off = ea(modrm, &isreg, &regn, &seg);
        sreg[(modrm >> 3) & 3] = get16(isreg, regn, seg, off);
        return 0;
    }
    if (op == 0x8F) {
        modrm = fetch8();
        off = ea(modrm, &isreg, &regn, &seg);
        put16(isreg, regn, seg, off, pop16());
        return 0;
    }
    if (op >= 0x90u && op <= 0x97u) {
        if (op != 0x90) {
            uint16_t t = r[R_AX];

            r[R_AX] = r[op & 7];
            r[op & 7] = t;
        }
        return 0;
    }
    if (op == 0x98) {
        r[R_AX] = (uint16_t)(int16_t)(int8_t)reg8(0);
        return 0;
    }
    if (op == 0x99) {
        r[R_DX] = (r[R_AX] & 0x8000u) ? 0xFFFFu : 0;
        return 0;
    }
    if (op == 0x9C) {
        push16((uint16_t)(flags | 2u));
        return 0;
    }
    if (op == 0x9D) {
        flags = pop16();
        return 0;
    }
    if (op == 0x9E) {
        flags = (uint16_t)((flags & 0xFF00u) | reg8(4));
        return 0;
    }
    if (op == 0x9F) {
        set_reg8(4, (uint8_t)flags);
        return 0;
    }
    if (op == 0xA0) {
        set_reg8(0, rd8(over_on ? over_seg : sreg[S_DS], fetch16()));
        return 0;
    }
    if (op == 0xA1) {
        r[R_AX] = rd16(over_on ? over_seg : sreg[S_DS], fetch16());
        return 0;
    }
    if (op == 0xA2) {
        wr8(over_on ? over_seg : sreg[S_DS], fetch16(), reg8(0));
        return 0;
    }
    if (op == 0xA3) {
        wr16(over_on ? over_seg : sreg[S_DS], fetch16(), r[R_AX]);
        return 0;
    }
    if (op >= 0xA4u && op <= 0xA7u) {
        do_string(op);
        return 0;
    }
    if (op == 0xA8) {
        alu8(4, reg8(0), fetch8());
        return 0;
    }
    if (op == 0xA9) {
        alu16(4, r[R_AX], fetch16());
        return 0;
    }
    if (op >= 0xAAu && op <= 0xAFu) {
        do_string(op);
        return 0;
    }
    if (op >= 0xB0u && op <= 0xB7u) {
        set_reg8(op & 7, fetch8());
        return 0;
    }
    if (op >= 0xB8u && op <= 0xBFu) {
        r[op & 7] = fetch16();
        return 0;
    }
    if (op == 0xC0 || op == 0xC1) {
        unsigned count;

        modrm = fetch8();
        off = ea(modrm, &isreg, &regn, &seg);
        count = fetch8();
        if (op == 0xC0) {
            shift8((modrm >> 3) & 7, isreg, regn, seg, off, count);
        } else {
            shift16((modrm >> 3) & 7, isreg, regn, seg, off, count);
        }
        return 0;
    }
    if (op == 0xC2 || op == 0xC3) {
        uint16_t extra = (op == 0xC2) ? fetch16() : 0;

        ip = pop16();
        r[R_SP] = (uint16_t)(r[R_SP] + extra);
        return 0;
    }
    if (op == 0xC4 || op == 0xC5) {
        modrm = fetch8();
        off = ea(modrm, &isreg, &regn, &seg);
        r[(modrm >> 3) & 7] = rd16(seg, off);
        sreg[op == 0xC4 ? S_ES : S_DS] = rd16(seg, (uint16_t)(off + 2u));
        return 0;
    }
    if (op == 0xC6 || op == 0xC7) {
        modrm = fetch8();
        off = ea(modrm, &isreg, &regn, &seg);
        if (op == 0xC6) {
            put8(isreg, regn, seg, off, fetch8());
        } else {
            put16(isreg, regn, seg, off, fetch16());
        }
        return 0;
    }
    if (op == 0xCA || op == 0xCB) {
        uint16_t extra = (op == 0xCA) ? fetch16() : 0;

        ip = pop16();
        sreg[S_CS] = pop16();
        r[R_SP] = (uint16_t)(r[R_SP] + extra);
        return 0;
    }
    if (op == 0xCD) {
        uint8_t n = fetch8();

        if (n == 0x21) {
            flags = (uint16_t)(flags | CF);
            r[R_AX] = 1;
        } else if (unknown_op == 0) {
            unknown_op = 0xCD;
            unknown_ip = (uint16_t)(ip - 2u);
            fprintf(stderr, "fplay: INT %02X at %04X\n", n, unknown_ip);
        }
        return 0;
    }
    if (op == 0xCF) {
        ip = pop16();
        sreg[S_CS] = pop16();
        flags = pop16();
        return 0;
    }
    if (op >= 0xD0u && op <= 0xD3u) {
        unsigned count = (op & 2) ? (unsigned)reg8(1) : 1u;

        modrm = fetch8();
        off = ea(modrm, &isreg, &regn, &seg);
        if ((op & 1) == 0) {
            shift8((modrm >> 3) & 7, isreg, regn, seg, off, count);
        } else {
            shift16((modrm >> 3) & 7, isreg, regn, seg, off, count);
        }
        return 0;
    }
    if (op == 0xD7) {
        set_reg8(0, rd8(over_on ? over_seg : sreg[S_DS],
                        (uint16_t)(r[R_BX] + reg8(0))));
        return 0;
    }
    if (op >= 0xE0u && op <= 0xE3u) {
        int8_t rel = (int8_t)fetch8();
        int take = 0;

        if (op == 0xE3) {
            take = r[R_CX] == 0;
        } else {
            r[R_CX] = (uint16_t)(r[R_CX] - 1u);
            if (op == 0xE2) {
                take = r[R_CX] != 0;
            } else if (op == 0xE1) {
                take = r[R_CX] != 0 && (flags & ZF);
            } else {
                take = r[R_CX] != 0 && (flags & ZF) == 0;
            }
        }
        if (take) {
            ip = (uint16_t)(ip + (uint16_t)(int16_t)rel);
        }
        return 0;
    }
    if (op == 0xE4 || op == 0xE5) {
        (void)fetch8();
        if (op == 0xE4) {
            set_reg8(0, 0);
        } else {
            r[R_AX] = 0;
        }
        return 0;
    }
    if (op == 0xE6 || op == 0xE7) {
        (void)fetch8();
        (void)reg8(0);
        return 0;
    }
    if (op == 0xE8 || op == 0xE9) {
        int16_t rel = (int16_t)fetch16();

        if (op == 0xE8) {
            push16(ip);
        }
        ip = (uint16_t)(ip + (uint16_t)rel);
        return 0;
    }
    if (op == 0xEA) {
        uint16_t nip = fetch16();
        uint16_t ncs = fetch16();

        ip = nip;
        sreg[S_CS] = ncs;
        return 0;
    }
    if (op == 0xEB) {
        int8_t rel = (int8_t)fetch8();

        ip = (uint16_t)(ip + (uint16_t)(int16_t)rel);
        return 0;
    }
    if (op == 0xEC || op == 0xED) {
        /* Status port 0x188 must report the chip idle (bit 7 clear). */
        if (op == 0xEC) {
            set_reg8(0, 0);
        } else {
            r[R_AX] = 0;
        }
        return 0;
    }
    if (op == 0xEE || op == 0xEF) {
        uint16_t port = r[R_DX];
        uint8_t val = reg8(0);

        if (port == 0x188u || port == 0x18Cu) {
            opn_latch[port == 0x18Cu] = val;
            opna_write(port, val);
        } else if (port == 0x18Au || port == 0x18Eu) {
            int bank = port == 0x18Eu;
            uint8_t addr = opn_latch[bank];

            opn_reg[bank][addr] = val;
            opn_writes++;
            opna_write(port, val);
        }
        return 0;
    }
    if (op == 0xF5) {
        flags = (uint16_t)(flags ^ CF);
        return 0;
    }
    if (op == 0xF6 || op == 0xF7) {
        int kind;

        modrm = fetch8();
        kind = (modrm >> 3) & 7;
        off = ea(modrm, &isreg, &regn, &seg);
        if (op == 0xF6) {
            uint8_t src = get8(isreg, regn, seg, off);

            if (kind == 0 || kind == 1) {
                alu8(4, src, fetch8());
            } else if (kind == 2) {
                put8(isreg, regn, seg, off, (uint8_t)~src);
            } else if (kind == 3) {
                arith_flags(0, src, 0u - src, 1, 8);
                put8(isreg, regn, seg, off, (uint8_t)(0u - src));
            } else if (kind == 4) {
                mul8(src, 0);
            } else if (kind == 5) {
                mul8(src, 1);
            } else if (kind == 6) {
                div8(src, 0);
            } else {
                div8(src, 1);
            }
        } else {
            uint16_t src = get16(isreg, regn, seg, off);

            if (kind == 0 || kind == 1) {
                alu16(4, src, fetch16());
            } else if (kind == 2) {
                put16(isreg, regn, seg, off, (uint16_t)~src);
            } else if (kind == 3) {
                arith_flags(0, src, 0u - src, 1, 16);
                put16(isreg, regn, seg, off, (uint16_t)(0u - src));
            } else if (kind == 4) {
                mul16(src, 0);
            } else if (kind == 5) {
                mul16(src, 1);
            } else if (kind == 6) {
                div16(src, 0);
            } else {
                div16(src, 1);
            }
        }
        return 0;
    }
    if (op == 0xF8) {
        flags = (uint16_t)(flags & ~CF);
        return 0;
    }
    if (op == 0xF9) {
        flags = (uint16_t)(flags | CF);
        return 0;
    }
    if (op == 0xFA || op == 0xFB) {
        if (op == 0xFA) {
            flags = (uint16_t)(flags & ~IF);
        } else {
            flags = (uint16_t)(flags | IF);
        }
        return 0;
    }
    if (op == 0xFC) {
        flags = (uint16_t)(flags & ~DF);
        return 0;
    }
    if (op == 0xFD) {
        flags = (uint16_t)(flags | DF);
        return 0;
    }
    if (op == 0xFE || op == 0xFF) {
        if (op == 0xFE) {
            int kind;

            modrm = fetch8();
            kind = (modrm >> 3) & 7;
            off = ea(modrm, &isreg, &regn, &seg);
            a = get8(isreg, regn, seg, off);
            if (kind == 0) {
                arith_flags(a, 1, a + 1u, 0, 8);
                put8(isreg, regn, seg, off, (uint8_t)(a + 1u));
            } else if (kind == 1) {
                arith_flags(a, 1, a - 1u, 1, 8);
                put8(isreg, regn, seg, off, (uint8_t)(a - 1u));
            }
        } else {
            modrm = fetch8();
            group_ff(modrm);
        }
        return 0;
    }
    if (op == 0x06 || op == 0x0E || op == 0x16 || op == 0x1E) {
        static const int which[4] = { S_ES, S_CS, S_SS, S_DS };

        push16(sreg[which[(op >> 3) & 3]]);
        return 0;
    }
    if (op == 0x07 || op == 0x17 || op == 0x1F) {
        static const int which[4] = { S_ES, S_CS, S_SS, S_DS };

        sreg[which[(op >> 3) & 3]] = pop16();
        return 0;
    }
    if (unknown_op == 0) {
        unknown_op = op;
        unknown_ip = (uint16_t)(ip - 1u);
        fprintf(stderr, "fplay: unknown opcode %02X at %04X\n", op, unknown_ip);
    }
    return 1;
}

/* Prefixes must survive into the following opcode. */
static int cpu_insn(void)
{
    over_on = 0;
    rep = 0;
    for (;;) {
        uint16_t saved_ip = ip;
        uint8_t op = mem[ip];
        int halt;

        if (op == 0x26 || op == 0x2E || op == 0x36 || op == 0x3E ||
            op == 0xF0 || op == 0xF2 || op == 0xF3 || op == 0x66) {
            halt = cpu_step();
            (void)saved_ip;
            if (halt) {
                return 1;
            }
            continue;
        }
        return cpu_step();
    }
}

static void cpu_run_fixed(int limit)
{
    int steps = 0;

    while (steps < limit) {
        if (cpu_insn()) {
            return;
        }
        steps++;
        if (ip == 0 && mem[0] == 0xF4) {
            return;
        }
    }
    fprintf(stderr, "fplay: step limit at ip %04X\n", ip);
}

static void enter_near(uint16_t dest)
{
    push16(0);
    ip = dest;
    cpu_run_fixed(4000000);
}

static void enter_iret(uint16_t dest)
{
    push16((uint16_t)(flags | 2u));
    push16(sreg[S_CS]);
    push16(0);
    ip = dest;
    cpu_run_fixed(400000);
}

static int timer_samples(void)
{
    int ta = ((int)opn_reg[0][0x24] << 2) | (opn_reg[0][0x25] & 3);
    int span = 1024 - ta;
    int n;

    if (span < 1) {
        span = 1;
    }
    /* 44100 * 72 * (1024-TA) / 3993600 */
    n = (int)((31752u * (unsigned)span) / 39936u);
    if (n < 8) {
        n = 8;
    }
    return n;
}

static void mix_frame(int16_t *dst)
{
    opna_sample(dst);
}

static void service_timer(void)
{
    if (isr_busy || !ready) {
        return;
    }
    isr_busy = 1;
    enter_iret(0x1000);
    isr_busy = 0;
}

static uint8_t *read_file(const char *path, size_t *n_out)
{
    FILE *fp = fopen(path, "rb");
    uint8_t *buf;
    long n;
    size_t got;

    if (!fp) {
        return NULL;
    }
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return NULL;
    }
    n = ftell(fp);
    if (n <= 0 || n > 1024 * 1024) {
        fclose(fp);
        return NULL;
    }
    if (fseek(fp, 0, SEEK_SET) != 0) {
        fclose(fp);
        return NULL;
    }
    buf = (uint8_t *)malloc((size_t)n);
    if (!buf) {
        fclose(fp);
        return NULL;
    }
    got = fread(buf, 1, (size_t)n, fp);
    fclose(fp);
    if (got != (size_t)n) {
        free(buf);
        return NULL;
    }
    *n_out = got;
    return buf;
}

int fplay_ready(void)
{
    return ready;
}

int fplay_bind(const char *dir)
{
    char path[1024];
    uint8_t *com;
    uint8_t *song;
    size_t com_n = 0;
    size_t song_n = 0;
    size_t copy;

    ready = 0;
    if (!dir || !dir[0]) {
        return -1;
    }
    snprintf(path, sizeof path, "%s/FPLAY.COM", dir);
    com = read_file(path, &com_n);
    snprintf(path, sizeof path, "%s/SONG.DAT", dir);
    song = read_file(path, &song_n);
    if (!com || !song) {
        free(com);
        free(song);
        fprintf(stderr, "fplay: FPLAY.COM or SONG.DAT missing in %s\n", dir);
        return -1;
    }
    memset(mem, 0, sizeof mem);
    memset(r, 0, sizeof r);
    memset(sreg, 0, sizeof sreg);
    memset(opn_reg, 0, sizeof opn_reg);
    unknown_op = 0;
    opn_writes = 0;
    mem[0] = 0xF4;
    copy = com_n;
    if (copy > MEM_N - 0x100u) {
        copy = MEM_N - 0x100u;
    }
    memcpy(mem + 0x100, com, copy);
    copy = song_n;
    if (copy > MEM_N - 0x4000u) {
        copy = MEM_N - 0x4000u;
    }
    memcpy(mem + 0x4000, song, copy);
    free(com);
    free(song);
    flags = IF | 2u;
    r[R_SP] = 0xFFFE;
    ip = 0;
    opna_autoload_rhythm(dir);
    opna_reset();
    enter_near(0x0BD0);
    ready = (unknown_op == 0 && mem[0x280] != 0) ? 1 : 0;
    timer_left = timer_samples();
    fprintf(stderr, "fplay: %u tracks, writes=%u unknown=%02X@%04X ready=%d\n",
            mem[0x280], opn_writes, unknown_op, unknown_ip, ready);
    return ready ? 0 : -1;
}

void fplay_command(uint8_t ah, uint8_t al)
{
    if (!ready || isr_busy) {
        return;
    }
    isr_busy = 1;
    r[R_AX] = (uint16_t)(((uint16_t)ah << 8) | al);
    enter_iret(0x0801);
    isr_busy = 0;
}

void fplay_render(int16_t *stereo, int frames)
{
    int i;

    if (!stereo || frames <= 0) {
        return;
    }
    if (!ready) {
        memset(stereo, 0, (size_t)frames * 2u * sizeof(int16_t));
        return;
    }
    for (i = 0; i < frames; i++) {
        if (timer_left <= 0 && !isr_busy) {
            service_timer();
            timer_left = timer_samples();
        }
        mix_frame(stereo + (size_t)i * 2u);
        if (timer_left > 0) {
            timer_left--;
        }
    }
}
