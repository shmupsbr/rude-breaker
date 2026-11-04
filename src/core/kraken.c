/**
 * Kraken. Host fight drawn over the stage the ship is already in.
 *
 * The central hull is a silver stand-in until the recurring mid-boss
 * sprite ids are hardcoded. Arms and orbs are new pixels. Fortress
 * patterns are the fortress's own scripts, run at half their wait.
 */
#include "kraken.h"
#include "gfx.h"

#include <stdio.h>
#include <string.h>

#define KR_TENT   4
#define KR_SEGS   8
#define KR_MAXC   16
#define KR_CYCLE  ((10 * 92) / 3) /* ten seconds of logic frames */
#define KR_HOLD   20000
#define KR_SENT   0xCE11u
#define KR_CYC_N  6
#define KR_SH_N   24
#define KR_PAT_N  8

enum {
    PH_OFF = 0,
    PH_LEAVE,
    PH_WAIT,
    PH_RETURN,
    PH_FIGHT,
    PH_DEAD
};

/* 0 = east, 64 = south, 128 = west, 192 = north. Screen Y grows down. */
static const int16_t QS[65] = {
    0, 6, 13, 19, 25, 31, 38, 44, 50, 56, 62, 68, 74, 80, 86, 92,
    98, 104, 109, 115, 121, 126, 132, 137, 142, 147, 152, 157, 162, 167, 172, 177,
    181, 185, 190, 194, 198, 202, 206, 209, 213, 216, 220, 223, 226, 229, 231, 234,
    237, 239, 241, 243, 245, 247, 248, 250, 251, 252, 253, 254, 255, 255, 256, 256, 256
};

static const int SOCK_X[KR_TENT] = {-12, 12, -10, 10};
static const int SOCK_Y[KR_TENT] = {-6, -6, 12, 12};
static const int REST_A[KR_TENT] = {160, 224, 96, 32};

typedef struct HalbCand {
    uint8_t cells[KR_MAXC * KR_MAXC];
    uint8_t w, h;
    uint16_t seg;
    uint8_t key;
    int stages;
    int area;
    int outside;
    int inside;
} HalbCand;

typedef struct ShotRec {
    uint8_t raw[82];
    uint8_t cof;
    uint16_t ip;
    uint16_t origin;
    int16_t dx, dy;
    uint16_t t;
} ShotRec;

typedef struct Cycle {
    ShotRec sh[KR_SH_N];
    int n;
    uint16_t dur;
} Cycle;

typedef struct Pat {
    uint16_t ip;
    uint16_t type;
} Pat;

static HalbCand s_cand[8];
static int s_ncand;
static int s_best;

static Cycle s_cyc[KR_CYC_N];
static int s_rec;
static int s_ncyc;
static int s_boss_f;
static int s_watch_ready;
static uint16_t s_prev_org[256];
static int s_boss_hp;

static Pat s_pat[KR_PAT_N];
static int s_npat;
static int s_scanned;

static int s_phase;
static int s_timer;
static int s_medals;
static int s_d1;
static int s_d1_ok;
static int s_hx, s_hy;
static int s_khp, s_khp_max;
static int s_thp[KR_TENT];
static int s_tmax;
static int s_slot[KR_TENT][KR_SEGS];
static int s_hull_slot;
static int s_ang[KR_TENT][KR_SEGS];
static int s_px[KR_TENT][KR_SEGS];
static int s_py[KR_TENT][KR_SEGS];
static int s_hom[6];
static int s_hom_from[6];
static int s_hom_age[6];
static int s_nhom;
static int s_fire_gap;
static int s_dash;
static int s_dash_i;
static int s_dash_t;
static int s_dash_wait;
static int s_gen;
static int s_cyc_i;
static int s_cyc_t;
static uint32_t s_rng = 0xC0FFEEu;
static int s_fair, s_deaths, s_loop, s_stage;
static int s_noship;
static int s_boom[KR_TENT];
static int s_bx[KR_TENT], s_by[KR_TENT];
static int s_big;
#if KRAKEN_DEBUG
static int s_arm;
static int s_bypass;
static int s_dbg_f;
#endif

static int ksin(int a)
{
    int s = 1;

    a &= 255;
    if (a & 128) {
        s = -1;
        a &= 127;
    }
    if (a > 64) {
        a = 128 - a;
    }
    return s * (int)QS[a];
}

static int kcos(int a)
{
    return ksin(a + 64);
}

static int adiff(int a, int b)
{
    int d = (a - b) & 255;

    if (d > 128) {
        d -= 256;
    }
    return d;
}

static int astep(int cur, int want, int turn)
{
    int d = adiff(want, cur);

    if (d > turn) {
        d = turn;
    }
    if (d < -turn) {
        d = -turn;
    }
    return (cur + d) & 255;
}

static int ang_to(int dx, int dy)
{
    int best = 64;
    int best_d = -0x3fffffff;
    int a;

    for (a = 0; a < 256; a += 4) {
        int dot = dx * kcos(a) + dy * ksin(a);

        if (dot > best_d) {
            best_d = dot;
            best = a;
        }
    }
    return best;
}

static int krand(int n)
{
    s_rng = s_rng * 1664525u + 1013904223u;
    if (n <= 1) {
        return 0;
    }
    return (int)(s_rng % (uint32_t)n);
}

static int is_fort(uint16_t org)
{
    return org == 0x8490u || org == 0x8722u || org == 0x882Au ||
           org == 0x864Du || org == 0x895Eu;
}

static int medal_need(RbVm *vm)
{
    int i;
    int n = 0;

    for (i = 1; i <= 5; i++) {
        int s = vm_medal_spots(vm, i);

        n += s < 0 ? 10 : s;
    }
    return n * 2;
}

static int qualified(RbVm *vm)
{
    /* Debug entry starts the fight itself. It must not also swallow
     * the stage's own loads, or the picture never comes up. */
    if (!s_fair || s_deaths != 0 || s_loop < 2 || !vm) {
        return 0;
    }
    return s_medals >= medal_need(vm);
}

static void medal_tick(RbVm *vm)
{
    int d1;

    if (!vm || s_stage < 1 || s_stage > 5) {
        return;
    }
    d1 = (int)vm_global_u8(vm, 0xD1);
    if (!s_d1_ok) {
        s_d1 = d1;
        s_d1_ok = 1;
        return;
    }
    if (d1 > s_d1) {
        s_medals += d1 - s_d1;
    }
    s_d1 = d1;
}

static int tent_alive_n(void)
{
    int i;
    int n = 0;

    for (i = 0; i < KR_TENT; i++) {
        if (s_thp[i] > 0) {
            n++;
        }
    }
    return n;
}

static int alive_tent(int k)
{
    int i;
    int n = 0;

    for (i = 0; i < KR_TENT; i++) {
        if (s_thp[i] > 0) {
            if (n == k) {
                return i;
            }
            n++;
        }
    }
    return 0;
}

static void slots_init(void)
{
    int i;
    int s;

    for (i = 0; i < KR_TENT; i++) {
        for (s = 0; s < KR_SEGS; s++) {
            s_slot[i][s] = -1;
            s_ang[i][s] = REST_A[i];
        }
    }
    s_hull_slot = -1;
    s_gen = -1;
    s_nhom = 0;
    s_dash = 0;
    s_dash_wait = KR_CYCLE;
    s_fire_gap = 0;
}

void kraken_reset(void)
{
    memset(s_cand, 0, sizeof s_cand);
    memset(s_cyc, 0, sizeof s_cyc);
    memset(s_pat, 0, sizeof s_pat);
    memset(s_prev_org, 0, sizeof s_prev_org);
    s_ncand = 0;
    s_best = -1;
    s_rec = 0;
    s_ncyc = 0;
    s_boss_f = 0;
    s_watch_ready = 0;
    s_boss_hp = 0;
    s_npat = 0;
    s_scanned = 0;
    s_phase = PH_OFF;
    s_timer = 0;
    s_medals = 0;
    s_d1_ok = 0;
    s_khp = 0;
    s_noship = 0;
    s_big = 0;
    s_cyc_i = 0;
    s_cyc_t = 0;
    memset(s_boom, 0, sizeof s_boom);
    slots_init();
#if KRAKEN_DEBUG
    s_arm = 0;
    s_bypass = 0;
    s_dbg_f = 0;
#endif
}

int kraken_active(void)
{
    return s_phase != PH_OFF;
}

#if KRAKEN_DEBUG
void kraken_debug_arm(void)
{
    if (s_phase == PH_OFF) {
        s_arm = 1;
        s_bypass = 1;
    }
}

int kraken_debug_armed(void)
{
    return s_arm;
}

void kraken_dump_screen(const RbVm *vm)
{
    int i;
    int n = 0;

    fprintf(stderr,
            "kraken dump begin (chunk=script record, bank=sprite sheet, pat=pattern word, cells=frame bytes)\n");
    if (!vm) {
        fprintf(stderr, "kraken dump end n=0\n");
        return;
    }
    for (i = 0; i < 256; i++) {
        RbEntView v;
        uint8_t cells[12];
        int nc;
        int k;
        char cellbuf[48];
        int used = 0;

        if (!vm_ent_view(vm, i, &v)) {
            continue;
        }
        nc = vm_ent_cells(vm, i, cells, (int)sizeof cells);
        cellbuf[0] = 0;
        for (k = 0; k < nc && k < 12; k++) {
            int wr = snprintf(cellbuf + used, sizeof cellbuf - (size_t)used,
                              "%s%u", k ? "," : "", (unsigned)cells[k]);

            if (wr < 0 || used + wr >= (int)sizeof cellbuf) {
                break;
            }
            used += wr;
        }
        fprintf(stderr,
                "kraken obj slot=%d type=%04X kind=%04X org=%04X chunk=%u bank=%u pat=%u x=%d y=%d w=%u h=%u hp=%u cells=%s\n",
                i, v.type, v.kind, v.origin, (unsigned)v.cof, (unsigned)v.seg,
                (unsigned)v.pat, (int)v.x, (int)v.y, (unsigned)v.w, (unsigned)v.h,
                (unsigned)v.hp, cellbuf);
        n++;
    }
    fprintf(stderr, "kraken dump end n=%d\n", n);
    fflush(stderr);
}
#endif

static void note_halb(const RbEntView *v, const uint8_t *cells, int ncells, int stage)
{
    int i;
    int area;
    HalbCand *c;

    if (v->w < 3 || v->h < 4 || v->w > KR_MAXC || v->h > KR_MAXC) {
        return;
    }
    area = (int)v->w * (int)v->h;
    if (area < 12 || v->seg > 255 || ncells < area) {
        return;
    }
    if (is_fort(v->origin) || v->origin == 0x4DE4u || v->origin == KR_SENT) {
        return;
    }
    if (v->kind == 0x0012u || v->kind == 0x0014u) {
        return;
    }
    if ((v->type & 0x0080u) && !(v->type & 0x0060u)) {
        return;
    }
    for (i = 0; i < s_ncand; i++) {
        if (s_cand[i].seg == v->seg && s_cand[i].w == v->w &&
            s_cand[i].h == v->h && s_cand[i].key == cells[0]) {
            break;
        }
    }
    if (i == s_ncand) {
        if (s_ncand >= 8) {
            return;
        }
        s_ncand++;
        memset(&s_cand[i], 0, sizeof s_cand[i]);
        s_cand[i].w = v->w;
        s_cand[i].h = v->h;
        s_cand[i].seg = v->seg;
        s_cand[i].key = cells[0];
    }
    c = &s_cand[i];
    memcpy(c->cells, cells, (size_t)area);
    c->area = area;
    if (stage >= 1 && stage <= 5) {
        c->stages |= 1 << (stage - 1);
    }
    if (v->y < 24 || v->y > 370 || v->x < 96 || v->x > 544) {
        c->outside = 1;
    }
    if (v->y > 40 && v->y < 340 && v->x > 120 && v->x < 520) {
        c->inside = 1;
    }
}

static void pick_halb(void)
{
    int i;
    int best = -1;
    int score = -1;

    for (i = 0; i < s_ncand; i++) {
        int st = 0;
        int b;
        int sc;
        int moved = s_cand[i].outside && s_cand[i].inside;

        for (b = 0; b < 5; b++) {
            if (s_cand[i].stages & (1 << b)) {
                st++;
            }
        }
        sc = st * 1000 + (moved ? 800 : 0) + s_cand[i].area;
        if (sc > score) {
            score = sc;
            best = i;
        }
    }
    s_best = best;
}

static void close_cycle(void)
{
    if (s_cyc[s_rec].n < 3) {
        s_cyc[s_rec].n = 0;
        return;
    }
    s_cyc[s_rec].dur = 265;
    if (s_rec + 1 < KR_CYC_N) {
        s_rec++;
        s_cyc[s_rec].n = 0;
    }
}

static void watch_world(RbVm *vm, int stage)
{
    int i;
    int fort = 0;
    int ax = 0;
    int ay = 0;
    int an = 0;

    if (!vm || s_phase != PH_OFF || stage < 1 || stage > 5) {
        return;
    }
    for (i = 0; i < 256; i++) {
        RbEntView v;
        uint8_t cells[KR_MAXC * KR_MAXC];
        int n;

        if (!vm_ent_view(vm, i, &v)) {
            s_prev_org[i] = 0;
            continue;
        }
        if (is_fort(v.origin) && v.hp > 0 && v.hp < 0x8000u) {
            fort = 1;
            if (v.hp > s_boss_hp) {
                s_boss_hp = (int)v.hp;
            }
            if (v.origin == 0x8490u || an == 0) {
                ax = v.x;
                ay = v.y;
            }
            an++;
        }
        n = vm_ent_cells(vm, i, cells, KR_MAXC * KR_MAXC);
        note_halb(&v, cells, n, stage);
        if (s_watch_ready && fort && (v.type & 0x0080u) && !(v.type & 0x1000u) &&
            v.origin != s_prev_org[i] && v.origin != 0 && !is_fort(v.origin)) {
            Cycle *c = &s_cyc[s_rec];
            ShotRec *sh;
            uint16_t ip = 0;

            if (c->n < KR_SH_N && vm_ent_raw(vm, i, c->sh[c->n].raw, &ip)) {
                sh = &c->sh[c->n];
                sh->cof = v.cof;
                sh->ip = ip;
                sh->origin = v.origin;
                sh->dx = (int16_t)(v.x - (an ? ax : 320));
                sh->dy = (int16_t)(v.y - (an ? ay : 80));
                sh->t = (uint16_t)(s_boss_f % 245);
                c->n++;
            }
        }
        s_prev_org[i] = v.origin;
    }
    if (fort) {
        s_boss_f++;
        if (s_boss_f > 0 && (s_boss_f % 245) == 0) {
            close_cycle();
        }
    }
    s_watch_ready = 1;
}

static void scan_patterns(RbVm *vm)
{
    static const uint16_t fin[] = {0x8490u, 0x8722u, 0x882Au, 0x864Du, 0x895Eu};
    const uint8_t *c;
    uint32_t n = 0;
    unsigned f;

    if (s_scanned || !vm) {
        return;
    }
    c = vm_chunk_bytes(vm, 6, &n);
    if (!c || n < 0x9000u) {
        return;
    }
    s_scanned = 1;
    for (f = 0; f < sizeof fin / sizeof fin[0]; f++) {
        uint32_t i = fin[f];
        uint32_t end = i + 0x180u;

        if (end > n) {
            end = n;
        }
        while (i + 5 < end && s_npat < KR_PAT_N) {
            uint8_t op = c[i];

            if (op == 0x15u || op == 0x16u || op == 0x17u) {
                uint16_t ip = (uint16_t)(c[i + 1] | ((uint16_t)c[i + 2] << 8));
                uint16_t typ = (uint16_t)(c[i + 3] | ((uint16_t)c[i + 4] << 8));
                int k;
                int dup = 0;

                if (ip > 0x4000u && (uint32_t)ip < n && typ != 0 &&
                    ((typ & 0x0080u) || (typ >= 0x0040u && typ <= 0x004Fu))) {
                    for (k = 0; k < s_npat; k++) {
                        if (s_pat[k].ip == ip) {
                            dup = 1;
                        }
                    }
                    if (!dup) {
                        s_pat[s_npat].ip = ip;
                        s_pat[s_npat].type = typ;
                        s_npat++;
                    }
                }
                i += 5;
            } else {
                i++;
            }
        }
    }
}

static int count_cycles(void)
{
    int n = 0;
    int i;

    for (i = 0; i < KR_CYC_N; i++) {
        if (s_cyc[i].n >= 3) {
            n++;
        }
    }
    return n;
}

static void spawn_parts(RbVm *vm)
{
    int i;
    int s;

    for (i = 0; i < KR_TENT; i++) {
        if (s_thp[i] <= 0) {
            continue;
        }
        for (s = 0; s < KR_SEGS; s++) {
            if (s_slot[i][s] < 0) {
                /* 0x0008 is outside the ship-hurt masks, so an arm does not
                 * kill the player on contact. Shots are tested by the host. */
                s_slot[i][s] = vm_spawn_hold(vm, 0x0008u, (int16_t)s_px[i][s],
                                             (int16_t)s_py[i][s], KR_HOLD,
                                             -7, 7, -7, 7);
            }
        }
    }
    if (tent_alive_n() == 0 && s_hull_slot < 0 && s_khp > 0) {
        s_hull_slot = vm_spawn_hold(vm, 0x0008u, (int16_t)s_hx, (int16_t)s_hy,
                                    KR_HOLD, -16, 16, -20, 20);
    }
}

static void clear_parts(RbVm *vm)
{
    int i;
    int s;

    for (i = 0; i < KR_TENT; i++) {
        for (s = 0; s < KR_SEGS; s++) {
            if (s_slot[i][s] >= 0) {
                vm_ent_clear(vm, s_slot[i][s]);
                s_slot[i][s] = -1;
            }
        }
    }
    if (s_hull_slot >= 0) {
        vm_ent_clear(vm, s_hull_slot);
        s_hull_slot = -1;
    }
    for (i = 0; i < s_nhom; i++) {
        if (s_hom[i] >= 0) {
            vm_ent_clear(vm, s_hom[i]);
        }
    }
    s_nhom = 0;
    vm_clear_haste(vm);
    s_gen = -1;
}

static void pose_at(int shipx, int shipy, int stretch_all)
{
    int ti;

    if (s_phase == PH_FIGHT) {
        s_hy = 96 + ksin(s_timer * 2) / 40;
    }
    for (ti = 0; ti < KR_TENT; ti++) {
        int stretch = stretch_all;
        int aim;
        int blend;
        int turn0;
        int i;
        int x;
        int y;

        if (s_phase == PH_FIGHT && s_thp[ti] <= 0) {
            continue;
        }
        if (s_phase == PH_FIGHT && s_dash && ti == s_dash_i && s_thp[ti] > 0) {
            int t = s_dash_t;
            int u;

            if (t < 36) {
                u = t * 256 / 36;
            } else if (t < 48) {
                u = 256;
            } else if (t < 76) {
                u = 256 - (t - 48) * 256 / 28;
            } else {
                u = 0;
            }
            stretch = 256 + (640 - 256) * u / 256;
            turn0 = 16;
        } else {
            turn0 = 6;
        }
        aim = ang_to(shipx - (s_hx + SOCK_X[ti]), shipy - (s_hy + SOCK_Y[ti]));
        blend = astep(REST_A[ti], aim, (s_dash && ti == s_dash_i) ? 100 : 52);
        s_ang[ti][0] = astep(s_ang[ti][0], blend, turn0);
        for (i = 1; i < KR_SEGS; i++) {
            s_ang[ti][i] = astep(s_ang[ti][i], s_ang[ti][i - 1], turn0 + 4);
        }
        x = s_hx + SOCK_X[ti];
        y = s_hy + SOCK_Y[ti];
        for (i = 0; i < KR_SEGS; i++) {
            int wave = ksin((s_timer * 3 + ti * 48 + i * 20) & 255) / 40;
            int a = (s_ang[ti][i] + wave) & 255;
            int len = (5 + (KR_SEGS - i) * 2) * stretch / 256;

            if (len < 3) {
                len = 3;
            }
            x += kcos(a) * len / 256;
            y += ksin(a) * len / 256;
            s_px[ti][i] = x;
            s_py[ti][i] = y;
        }
    }
}

static void place_parts(RbVm *vm)
{
    int i;
    int s;

    for (i = 0; i < KR_TENT; i++) {
        if (s_thp[i] <= 0) {
            continue;
        }
        for (s = 0; s < KR_SEGS; s++) {
            int id = s_slot[i][s];
            RbEntView v;

            if (id < 0 || !vm_ent_view(vm, id, &v) || v.origin != KR_SENT) {
                s_slot[i][s] = -1;
                continue;
            }
            vm_ent_place(vm, id, (int16_t)s_px[i][s], (int16_t)s_py[i][s],
                         s + 1 == KR_SEGS ? (int16_t)-16 : (int16_t)-10,
                         s + 1 == KR_SEGS ? (int16_t)16 : (int16_t)10,
                         s + 1 == KR_SEGS ? (int16_t)-16 : (int16_t)-10,
                         s + 1 == KR_SEGS ? (int16_t)16 : (int16_t)10);
            /* Only the stretching tip is a real hit. The rest of the arm is not. */
            vm_ent_set_type(vm, id,
                            (s_phase == PH_FIGHT && s_dash && i == s_dash_i &&
                             s + 1 == KR_SEGS)
                                ? 0x0080u
                                : 0x0008u);
        }
    }
    if (s_hull_slot >= 0) {
        RbEntView v;

        if (!vm_ent_view(vm, s_hull_slot, &v) || v.origin != KR_SENT) {
            s_hull_slot = -1;
        } else {
            vm_ent_place(vm, s_hull_slot, (int16_t)s_hx, (int16_t)s_hy,
                         -18, 18, -24, 24);
        }
    }
    for (i = 0; i < s_nhom; i++) {
        RbEntView v;
        int x;
        int y;
        int dx;
        int dy;
        int adx;
        int ady;
        int mag;
        int shipx = 320;
        int shipy = 300;

        if (s_hom[i] < 0 || !vm_ent_view(vm, s_hom[i], &v)) {
            s_hom[i] = -1;
            continue;
        }
        vm_ship_pos(vm, &shipx, &shipy);
        x = v.x;
        y = v.y;
        dx = shipx - x;
        dy = shipy - y;
        adx = dx < 0 ? -dx : dx;
        ady = dy < 0 ? -dy : dy;
        mag = adx > ady ? adx + ady / 2 : ady + adx / 2;
        if (mag < 1) {
            mag = 1;
        }
        x += dx * 4 / mag;
        y += dy * 4 / mag;
        s_hom_age[i]++;
        if (s_hom_age[i] > 220 || x < 70 || x > 570 || y < -20 || y > 420) {
            vm_ent_clear(vm, s_hom[i]);
            s_hom[i] = -1;
            continue;
        }
        vm_ent_place(vm, s_hom[i], (int16_t)x, (int16_t)y, -4, 4, -4, 4);
    }
}

static void compact_hom(void)
{
    int i;
    int w = 0;

    for (i = 0; i < s_nhom; i++) {
        if (s_hom[i] >= 0) {
            s_hom[w] = s_hom[i];
            s_hom_from[w] = s_hom_from[i];
            s_hom_age[w] = s_hom_age[i];
            w++;
        }
    }
    s_nhom = w;
}

static int homing_from(int ti)
{
    int i;

    for (i = 0; i < s_nhom; i++) {
        if (s_hom[i] >= 0 && s_hom_from[i] == ti) {
            return 1;
        }
    }
    return 0;
}

static void fire_homing(RbVm *vm)
{
    int n = tent_alive_n();
    int cap = n >= 4 ? 1 : n == 3 ? 2 : n == 2 ? 2 : n == 1 ? 4 : 0;
    int gap = n >= 4 ? 22 : n == 3 ? 16 : n == 2 ? 14 : 6;
    int ti = -1;
    int k;
    int x;
    int y;
    int id;

    if (n == 0 || s_phase != PH_FIGHT) {
        return;
    }
    compact_hom();
    if (s_fire_gap > 0) {
        s_fire_gap--;
    }
    if (s_nhom >= cap || s_fire_gap > 0) {
        return;
    }
    if (n == 2) {
        for (k = 0; k < n; k++) {
            int t = alive_tent(k);

            if (t != s_dash_i || !s_dash) {
                if (!homing_from(t)) {
                    ti = t;
                    break;
                }
            }
        }
    } else {
        for (k = 0; k < 6 && ti < 0; k++) {
            int t = alive_tent(krand(n));

            if (!(s_dash && t == s_dash_i)) {
                ti = t;
            }
        }
    }
    if (ti < 0) {
        return;
    }
    x = s_px[ti][KR_SEGS - 1];
    y = s_py[ti][KR_SEGS - 1];
    id = vm_spawn_hold(vm, 0x0080u, (int16_t)x, (int16_t)y, KR_HOLD, -4, 4, -4, 4);
    if (id < 0 || s_nhom >= 6) {
        return;
    }
    s_hom[s_nhom] = id;
    s_hom_from[s_nhom] = ti;
    s_hom_age[s_nhom] = 0;
    s_nhom++;
    s_fire_gap = gap;
}

static int is_player_shot(const RbEntView *v)
{
    if (!v || v->kind == 0x0012u || v->kind == 0x0014u) {
        return 0;
    }
    if (v->origin == 0x62E3u || v->origin == 0x62E9u) {
        return 0;
    }
    if (v->type & 0x1000u) {
        return 1;
    }
    if (v->origin == 0x6331u || v->origin == 0x6498u ||
        v->origin == 0x6337u || v->origin == 0x649Eu) {
        return 1;
    }
    if ((v->type & 0x0004u) && v->seg == 26u) {
        return 1;
    }
    if (v->origin >= 0x6300u && v->origin < 0x6700u && (v->type & 0x00FFu) != 0) {
        return 1;
    }
    return 0;
}

static int near_seg(int x, int y, int cx, int cy, int r)
{
    int dx = x - cx;
    int dy = y - cy;

    return dx * dx + dy * dy <= r * r;
}

static int overlap(const RbEntView *a, const RbEntView *b)
{
    int al = a->x + a->bl;
    int ar = a->x + a->br;
    int at = a->y + a->bt;
    int ab = a->y + a->bb;
    int bl = b->x + b->bl;
    int br = b->x + b->br;
    int bt = b->y + b->bt;
    int bb = b->y + b->bb;

    if (al >= br || ar < bl || at >= bb || ab < bt) {
        return 0;
    }
    return 1;
}

static void apply_damage(RbVm *vm)
{
    int credited[KR_TENT][KR_SEGS];
    int hull_hit = 0;
    int i;
    int s;
    int sh;

    memset(credited, 0, sizeof credited);
    for (i = 0; i < KR_TENT; i++) {
        if (s_thp[i] <= 0) {
            continue;
        }
        for (s = 0; s < KR_SEGS; s++) {
            RbEntView v;
            int id = s_slot[i][s];

            if (id < 0 || !vm_ent_view(vm, id, &v)) {
                continue;
            }
            if (v.hp < KR_HOLD) {
                int d = KR_HOLD - (int)v.hp;

                if (d > 48) {
                    d = 8;
                }
                s_thp[i] -= d;
                credited[i][s] = 1;
            }
            vm_ent_set_hp(vm, id, KR_HOLD);
        }
    }
    if (s_hull_slot >= 0) {
        RbEntView v;

        if (vm_ent_view(vm, s_hull_slot, &v) && v.hp < KR_HOLD) {
            int d = KR_HOLD - (int)v.hp;

            if (d > 48) {
                d = 8;
            }
            s_khp -= d;
            hull_hit = 1;
        }
        if (s_hull_slot >= 0) {
            vm_ent_set_hp(vm, s_hull_slot, KR_HOLD);
        }
    }
    for (sh = 0; sh < 256; sh++) {
        RbEntView shot;
        int hit_i = -1;
        int hit_s = -1;

        if (!vm_ent_view(vm, sh, &shot) || !is_player_shot(&shot)) {
            continue;
        }
        for (i = 0; i < KR_TENT && hit_i < 0; i++) {
            if (s_thp[i] <= 0) {
                continue;
            }
            for (s = 0; s < KR_SEGS; s++) {
                int rad = (s + 1 == KR_SEGS) ? 18 : 12;
                RbEntView part;
                int id = s_slot[i][s];

                if (near_seg(shot.x, shot.y, s_px[i][s], s_py[i][s], rad)) {
                    hit_i = i;
                    hit_s = s;
                    break;
                }
                if (id >= 0 && vm_ent_view(vm, id, &part) && overlap(&shot, &part)) {
                    hit_i = i;
                    hit_s = s;
                    break;
                }
            }
        }
        if (hit_i >= 0) {
            if (!credited[hit_i][hit_s]) {
                s_thp[hit_i] -= 12;
                credited[hit_i][hit_s] = 1;
                vm_add_score(vm, 10);
            }
            vm_ent_clear(vm, sh);
            continue;
        }
        if (s_hull_slot >= 0 && tent_alive_n() == 0) {
            RbEntView hull;

            if (vm_ent_view(vm, s_hull_slot, &hull) && overlap(&shot, &hull)) {
                if (!hull_hit) {
                    s_khp -= 6;
                    hull_hit = 1;
                }
                vm_ent_clear(vm, sh);
            }
        }
    }
    for (i = 0; i < KR_TENT; i++) {
        if (s_thp[i] <= 0 && s_slot[i][0] >= 0) {
            s_boom[i] = 18;
            s_bx[i] = s_px[i][KR_SEGS - 1];
            s_by[i] = s_py[i][KR_SEGS - 1];
            for (s = 0; s < KR_SEGS; s++) {
                if (s_slot[i][s] >= 0) {
                    vm_ent_clear(vm, s_slot[i][s]);
                    s_slot[i][s] = -1;
                }
            }
            vm_add_score(vm, 200);
            sys_play_sound(0x25, 160);
            if (s_dash && s_dash_i == i) {
                s_dash = 0;
            }
        }
    }
}

static void bullets_vs_ship(RbVm *vm)
{
    int sx = 0;
    int sy = 0;
    int x0, y0, x1, y1;
    int i;

    if (!vm_ship_pos(vm, &sx, &sy)) {
        return;
    }
    if (!vm_ship_box(vm, &x0, &y0, &x1, &y1)) {
        x0 = sx - 6;
        x1 = sx + 6;
        y0 = sy - 6;
        y1 = sy + 6;
    }
    for (i = 0; i < s_nhom; i++) {
        RbEntView v;

        if (s_hom[i] < 0 || !vm_ent_view(vm, s_hom[i], &v)) {
            continue;
        }
        if (v.x + 4 >= x0 && v.x - 4 <= x1 && v.y + 4 >= y0 && v.y - 4 <= y1) {
            vm_ent_clear(vm, s_hom[i]);
            s_hom[i] = -1;
        }
    }
}

static void cycle_begin(RbVm *vm)
{
    vm_clear_haste(vm);
    s_gen = -1;
    s_cyc_t = 0;
    if (s_ncyc > 0) {
        return;
    }
    if (s_npat > 0) {
        Pat *p = &s_pat[s_cyc_i % s_npat];

        s_gen = vm_spawn_haste(vm, 6, p->ip, p->type, (int16_t)s_hx, (int16_t)s_hy);
    }
}

static void replay_cycle(RbVm *vm)
{
    Cycle *c;
    int dur;
    int now;
    int prev;
    int i;
    int idx = 0;
    int seen = 0;

    if (s_ncyc <= 0) {
        return;
    }
    for (i = 0; i < KR_CYC_N; i++) {
        if (s_cyc[i].n >= 3) {
            if (seen == (s_cyc_i % s_ncyc)) {
                idx = i;
                break;
            }
            seen++;
        }
    }
    c = &s_cyc[idx];
    dur = c->dur > 30 ? (int)c->dur : 30;
    now = (s_cyc_t * 2) % dur;
    prev = s_cyc_t == 0 ? -1 : ((s_cyc_t - 1) * 2) % dur;
    for (i = 0; i < c->n; i++) {
        int t = (int)c->sh[i].t;
        int fire = 0;

        if (prev < 0) {
            fire = t == 0;
        } else if (now >= prev) {
            fire = t > prev && t <= now;
        } else {
            fire = t > prev || t <= now;
        }
        if (fire) {
            vm_spawn_snap(vm, c->sh[i].raw, c->sh[i].cof, c->sh[i].ip,
                          c->sh[i].origin,
                          (int16_t)(s_hx + c->sh[i].dx),
                          (int16_t)(s_hy + c->sh[i].dy));
        }
    }
}


static void start_fight(RbVm *vm)
{
    int base;
    int i;

    pick_halb();
    scan_patterns(vm);
    close_cycle();
    s_ncyc = count_cycles();
    vm_set_kraken_take(vm, 0);
    for (i = 0; i < 256; i++) {
        RbEntView v;

        if (vm_ent_view(vm, i, &v) && (v.kind == 0x0012u || v.origin == 0x4F6Eu ||
                                       v.origin == 0x4F74u)) {
            vm_ent_unhide(vm, i);
        }
    }
    vm_kraken_isolate(vm);
    vm_redraw(vm);
    slots_init();
    base = s_boss_hp >= 64 ? s_boss_hp : 480;
    s_khp_max = base;
    s_khp = base;
    s_tmax = base / 2;
    if (s_tmax < 32) {
        s_tmax = 32;
    }
    for (i = 0; i < KR_TENT; i++) {
        s_thp[i] = s_tmax;
        s_boom[i] = 0;
    }
    s_hx = 320;
    s_hy = 160;
    s_phase = PH_LEAVE;
    s_timer = 0;
    s_noship = 0;
    s_big = 0;
    s_cyc_i = 0;
    s_cyc_t = 0;
    sys_play_music(8);
#if KRAKEN_DEBUG
    s_arm = 0;
#endif
}

static void stop_fight(RbVm *vm, int ending)
{
    clear_parts(vm);
    s_phase = PH_OFF;
    if (ending) {
        vm_add_score(vm, 2000);
        sys_play_sound(0x1E, 200);
        vm_force_ending(vm);
    }
}

void kraken_before(RbVm *vm, int deaths, int fair, int loop, int stage, int paused)
{
    int shipx = 320;
    int shipy = 300;
    int stretch = 256;

    s_deaths = deaths;
    s_fair = fair;
    s_loop = loop;
    s_stage = stage;
    if (!vm) {
        return;
    }
    if (s_phase == PH_OFF) {
        vm_set_kraken_take(vm, qualified(vm) && stage == 5 && loop >= 2);
        return;
    }
    if (paused) {
        place_parts(vm);
        return;
    }
    if (s_phase == PH_RETURN) {
        stretch = s_timer * 256 / 80;
        if (stretch > 256) {
            stretch = 256;
        }
    } else if (s_phase != PH_FIGHT) {
        stretch = 0;
    }
    if (vm_ship_pos(vm, &shipx, &shipy) != 1) {
        shipx = 320;
        shipy = 300;
    }
    if (s_phase == PH_LEAVE) {
        s_hy = 180 - s_timer * 3;
        s_hx = 320;
    } else if (s_phase == PH_WAIT) {
        s_hy = -40;
        s_hx = 320;
    } else if (s_phase == PH_RETURN) {
        s_hx = 320;
        s_hy = -30 + s_timer * (96 + 30) / 80;
    }
    if (s_phase == PH_RETURN || s_phase == PH_FIGHT) {
        if (stretch < 8) {
            stretch = 8;
        }
        pose_at(shipx, shipy, stretch);
    }
    if (s_phase == PH_FIGHT) {
        place_parts(vm);
        if (s_gen >= 0) {
            RbEntView v;

            if (!vm_ent_view(vm, s_gen, &v)) {
                s_gen = -1;
            } else {
                vm_ent_place(vm, s_gen, (int16_t)s_hx, (int16_t)s_hy, 0, 0, 0, 0);
            }
        }
    }
}

void kraken_after(RbVm *vm, int stage, int paused)
{
    int i;

    (void)stage;
    if (!vm) {
        return;
    }
    if (s_fair) {
        medal_tick(vm);
    }
    watch_world(vm, s_stage);
    scan_patterns(vm);
    if (s_phase == PH_OFF) {
        if (vm_take_kraken(vm)) {
            start_fight(vm);
        }
#if KRAKEN_DEBUG
        else if (s_arm) {
            int slot = -1;
            int shown = 0;

            s_dbg_f++;
            for (i = 0; i < 256; i++) {
                RbEntView v;

                if (vm_ent_view(vm, i, &v) && v.kind == 0x0012u) {
                    slot = i;
                    if ((v.type & 0x0200u) == 0 && (v.type & 0x00FFu) != 0) {
                        shown = 1;
                    }
                }
            }
            /* The stage has to paint its own background first. A timeout
             * used to begin the fight on black and then stop that
             * painting forever. */
            if (slot >= 0 && shown && s_dbg_f > 45 &&
                gfx_bg_lit(vm_gfx(vm)) >= 24) {
                start_fight(vm);
            }
        }
#endif
        return;
    }
    /* Drop stage actors every frame so a director cannot put them back.
     * The ship, its shots, and Kraken's own parts are kept. */
    vm_kraken_isolate(vm);
    if (paused) {
        vm_redraw(vm);
        return;
    }
    {
        int have = 0;
        int k;

        for (k = 0; k < 256; k++) {
            RbEntView v;

            if (vm_ent_view(vm, k, &v) && v.kind == 0x0012u) {
                have = 1;
                vm_ent_unhide(vm, k);
            }
        }
        if (!have) {
            if (++s_noship > 180) {
                stop_fight(vm, 0);
            }
            vm_redraw(vm);
            return;
        }
    }
    s_noship = 0;
    s_timer++;
    if (s_phase == PH_LEAVE && s_timer >= 90) {
        s_phase = PH_WAIT;
        s_timer = 0;
    } else if (s_phase == PH_WAIT && s_timer >= 55) {
        s_phase = PH_RETURN;
        s_timer = 0;
        s_hx = 320;
        s_hy = -30;
    } else if (s_phase == PH_RETURN && s_timer >= 80) {
        s_phase = PH_FIGHT;
        s_timer = 0;
        s_hy = 96;
        spawn_parts(vm);
        cycle_begin(vm);
    } else if (s_phase == PH_FIGHT) {
        spawn_parts(vm);
        apply_damage(vm);
        bullets_vs_ship(vm);
        fire_homing(vm);
        if (s_dash) {
            if (++s_dash_t >= 76) {
                s_dash = 0;
            }
        } else if (s_dash_wait > 0) {
            s_dash_wait--;
        } else if (tent_alive_n() > 0) {
            s_dash = 1;
            s_dash_t = 0;
            s_dash_i = alive_tent(krand(tent_alive_n()));
            s_dash_wait = KR_CYCLE;
        }
        if (s_gen >= 0) {
            RbEntView v;

            if (vm_ent_view(vm, s_gen, &v)) {
                vm_ent_place(vm, s_gen, (int16_t)s_hx, (int16_t)s_hy, 0, 0, 0, 0);
            }
        }
        replay_cycle(vm);
        s_cyc_t++;
        if (s_cyc_t >= KR_CYCLE) {
            s_cyc_i++;
            cycle_begin(vm);
        }
        if (s_khp <= 0) {
            s_phase = PH_DEAD;
            s_timer = 0;
            s_big = 28;
            clear_parts(vm);
        }
    } else if (s_phase == PH_DEAD) {
        if (s_big > 0) {
            s_big--;
        }
        if (s_timer >= 50) {
            stop_fight(vm, 1);
        }
    }
    for (i = 0; i < KR_TENT; i++) {
        if (s_boom[i] > 0) {
            s_boom[i]--;
        }
    }
    if (s_phase != PH_OFF) {
        vm_redraw(vm);
    }
}

static void put_px(uint32_t *px, int w, int h, int x, int y, uint32_t c)
{
    if (!c || x < 80 || y < 0 || x >= 560 || y >= h || x >= w) {
        return;
    }
    px[(size_t)y * (size_t)w + (size_t)x] = c;
}

#define METAL_OUT 0xA8B0BCFFu
#define METAL_SH  0xD5DCE6FFu
#define METAL_LIT 0xF7F9FCFF
#define METAL_HI  0xFFFFFFFF
#define JOINT_RED 0xE02820FFu
#define ORB_HALO  0xFF58C8FFu
#define ORB_BODY  0xFF8AD8FFu
#define ORB_CORE  0xFFF8FFFFu

static void blit_hull(uint32_t *px, int w, int h, int cx, int cy)
{
    int yy;
    int xx;

    /* Silver stand-in core. The real hull sprite is not guessed. */
    for (yy = -48; yy <= 40; yy++) {
        int half = 9;

        if (yy < -32) {
            half = 6;
        } else if (yy > 22) {
            half = 7 - (yy - 22) / 5;
        }
        if (half < 4) {
            half = 4;
        }
        for (xx = -half; xx <= half; xx++) {
            int edge = xx == -half || xx == half || yy == -48 || yy == 40;
            uint32_t col = edge ? METAL_OUT : (xx >= half - 3 ? METAL_HI : METAL_LIT);

            put_px(px, w, h, cx + xx, cy + yy, col);
        }
    }
}

static void draw_seg(uint32_t *px, int w, int h, int x0, int y0, int x1, int y1,
                     int thick, int joint)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int steps = dx < 0 ? -dx : dx;
    int sy = dy < 0 ? -dy : dy;
    int s;
    if (sy > steps) {
        steps = sy;
    }
    if (steps < 1) {
        steps = 1;
    }
    for (s = 0; s <= steps; s++) {
        int x = x0 + dx * s / steps;
        int y = y0 + dy * s / steps;
        int rad = thick / 2;
        int oy;
        int ox;

        if (rad < 2) {
            rad = 2;
        }
        for (oy = -rad - 1; oy <= rad + 1; oy++) {
            for (ox = -rad - 1; ox <= rad + 1; ox++) {
                int d2 = ox * ox + oy * oy;
                int side;
                uint32_t col;

                if (d2 > (rad + 1) * (rad + 1)) {
                    continue;
                }
                /* Upper-left of each disc is the white band. A direction
                 * test collapsed to 0 here and the whole arm went flat. */
                side = ox + oy;
                if (d2 > rad * rad) {
                    col = METAL_OUT;
                } else if (joint && s >= steps - 2) {
                    col = JOINT_RED;
                } else if (side < -(rad / 2)) {
                    col = METAL_HI;
                } else if (side > rad - 1) {
                    col = METAL_SH;
                } else {
                    col = METAL_LIT;
                }
                put_px(px, w, h, x + ox, y + oy, col);
            }
        }
    }
}

static void draw_orb(uint32_t *px, int w, int h, int cx, int cy)
{
    int y;
    int x;
    int a;
    const int rad = 11;

    for (y = -rad - 2; y <= rad + 2; y++) {
        for (x = -rad - 2; x <= rad + 2; x++) {
            int d2 = x * x + y * y;
            uint32_t col;

            if (d2 > (rad + 2) * (rad + 2)) {
                continue;
            }
            if (d2 >= rad * rad) {
                col = METAL_OUT;
            } else if (d2 <= 9) {
                col = ORB_CORE;
            } else if (d2 <= 36) {
                col = ORB_BODY;
            } else {
                col = ORB_HALO;
            }
            put_px(px, w, h, cx + x, cy + y, col);
        }
    }
    for (a = 0; a < 256; a += 21) {
        int hx = cx + kcos(a) * 16 / 256;
        int hy = cy + ksin(a) * 16 / 256;

        put_px(px, w, h, hx, hy, ORB_CORE);
        put_px(px, w, h, hx + 1, hy, ORB_HALO);
        put_px(px, w, h, hx, hy + 1, ORB_BODY);
    }
}

static uint32_t pal(const RbGfx *gfx, int idx, uint32_t fallback)
{
    uint32_t c;

    if (!gfx) {
        return fallback;
    }
    c = gfx_index_rgba(gfx, idx);
    return c ? c : fallback;
}

static void draw_boom(uint32_t *px, int w, int h, const RbGfx *gfx, int cx, int cy, int t, int big)
{
    int r = (18 - t) * (big ? 4 : 2);
    int a;
    uint32_t c = pal(gfx, 11, 0xFF6060FFu);
    uint32_t core = pal(gfx, 15, 0xFFFFFFFFu);

    if (t <= 0) {
        return;
    }
    for (a = 0; a < 256; a += 8) {
        put_px(px, w, h, cx + kcos(a) * r / 256, cy + ksin(a) * r / 256, c);
        put_px(px, w, h, cx + kcos(a) * r / 512, cy + ksin(a) * r / 512, core);
    }
}

static void draw_name(uint32_t *px, int w, int h, const RbGfx *gfx, int cx, int cy)
{
    static const uint8_t gly[][7] = {
        {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11}, /* K */
        {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11}, /* R */
        {0x0E, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11}, /* A */
        {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11}, /* K */
        {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F}, /* E */
        {0x11, 0x19, 0x15, 0x13, 0x11, 0x11, 0x11}  /* N */
    };
    int i;
    uint32_t c = pal(gfx, 14, 0xFFF0A0FFu);
    int left = cx - 6 * 8;
    for (i = 0; i < 6; i++) {
        int row;

        for (row = 0; row < 7; row++) {
            int col;

            for (col = 0; col < 5; col++) {
                if (gly[i][row] & (0x10u >> col)) {
                    put_px(px, w, h, left + i * 16 + col * 2, cy + row * 2, c);
                    put_px(px, w, h, left + i * 16 + col * 2 + 1, cy + row * 2, c);
                    put_px(px, w, h, left + i * 16 + col * 2, cy + row * 2 + 1, c);
                    put_px(px, w, h, left + i * 16 + col * 2 + 1, cy + row * 2 + 1, c);
                }
            }
        }
    }
}

static void draw_arm(uint32_t *px, int w, int h, int ti)
{
    int s;
    int x0 = s_hx + SOCK_X[ti];
    int y0 = s_hy + SOCK_Y[ti];

    for (s = 0; s < KR_SEGS; s++) {
        int x1 = s_px[ti][s];
        int y1 = s_py[ti][s];
        int thick = 30 - s * 2;

        if (thick < 16) {
            thick = 16;
        }
        if (s + 1 == KR_SEGS) {
            draw_seg(px, w, h, x0, y0, x1, y1, thick, 1);
            draw_orb(px, w, h, x1, y1);
        } else {
            draw_seg(px, w, h, x0, y0, x1, y1, thick, 1);
        }
        x0 = x1;
        y0 = y1;
    }
}

void kraken_draw(RbVm *vm, const RbGfx *gfx, uint32_t *px, int w, int h)
{
    int i;

    (void)gfx;
    if (!px || s_phase == PH_OFF || w < 640 || h < 400) {
        return;
    }
    if (s_phase == PH_LEAVE) {
        blit_hull(px, w, h, s_hx, s_hy);
        return;
    }
    if (s_phase == PH_WAIT || (s_phase == PH_RETURN && s_timer < 40)) {
        draw_name(px, w, h, gfx, 320, 188);
    }
    if (s_phase == PH_WAIT) {
        return;
    }
    for (i = 0; i < KR_TENT; i++) {
        if (s_phase == PH_FIGHT && s_thp[i] <= 0) {
            draw_boom(px, w, h, gfx, s_bx[i], s_by[i], s_boom[i], 0);
            continue;
        }
        if (s_phase == PH_RETURN || s_phase == PH_FIGHT) {
            draw_arm(px, w, h, i);
        }
    }
    /* Hull stays up after the arms are gone. It is the thing you shoot. */
    if (s_phase == PH_RETURN || s_phase == PH_FIGHT || s_phase == PH_DEAD) {
        blit_hull(px, w, h, s_hx, s_hy);
    }
    for (i = 0; i < s_nhom; i++) {
        RbEntView v;

        if (!vm || s_hom[i] < 0 || !vm_ent_view(vm, s_hom[i], &v)) {
            continue;
        }
        draw_orb(px, w, h, v.x, v.y);
    }
    if (s_big > 0) {
        draw_boom(px, w, h, gfx, s_hx, s_hy, s_big > 18 ? s_big - 10 : s_big, 1);
    }
}
