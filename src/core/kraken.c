/**
 * Kraken. Host fight drawn over the stage the ship is already in.
 *
 * The stage 5 boss falls off the bottom while it breaks apart, then
 * the game's own white flash plays. If this is not the true last
 * fight, the run continues. If it is, the central part rises back
 * after three seconds, puts out four arms, and the fight starts.
 * Beating it explodes that part in place. The hull is the stage 5
 * boss core (chunk 6, bank 36, pattern 41223, 3x5).
 */
#include "kraken.h"
#include "kraken_hit.h"
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
    PH_FALL,
    PH_FLASH,
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
static int s_d1;
static int s_d1_ok;
static KrLoops s_loops;
static int s_death0;
static int s_tracked;
static int s_have_track;
static int s_run_real;
static int s_hx, s_hy;
static int s_khp, s_khp_max;
static int s_thp[KR_TENT];
static int s_tmax;
static int s_hit;
#define CORE_CH   6
#define CORE_BANK 36
#define CORE_PAT  41223
#define CORE_TW   3
#define CORE_TH   5
static uint32_t s_core[CORE_TW * 16 * CORE_TH * 16];
static int s_core_ok;
#if KRAKEN_DEBUG
static int s_hull_said;
#endif
static int s_drop;
static int s_drop_dy;
static int s_drop_done;
static int s_saw_fort;
static int s_quiet;
static int s_applied[256];
static int16_t s_last_y[256];
static uint16_t s_drop_org[256];
static uint8_t s_pshot[256];
static int16_t s_px0[256];
static int16_t s_py0[256];
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
static int s_deaths, s_loop, s_stage, s_black;
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

static int loop_need(const RbVm *vm)
{
    int i;
    int n = 0;

    for (i = 1; i <= 5; i++) {
        int s = vm ? vm_medal_spots(vm, i) : -1;

        n += s < 0 ? 10 : s;
    }
    return n;
}

static void sync_loops(const RbVm *vm)
{
    int died = s_deaths - s_death0;

    s_loops.need = loop_need(vm);
    s_loops.deaths = died < 0 ? 0 : died;
}

/* The loop number moved on, so the one just played is finished.
 * A clean loop is kept. A death or a missed medal drops only that loop. */
static void note_loop(const RbVm *vm)
{
    if (!s_have_track) {
        s_tracked = s_loop;
        s_death0 = s_deaths;
        s_have_track = 1;
        s_loops.need = loop_need(vm);
        return;
    }
    if (s_loop == s_tracked) {
        return;
    }
    sync_loops(vm);
    if (s_run_real && s_black) {
        kr_finish_loop(&s_loops);
    } else {
        s_loops.medals = 0;
        s_loops.deaths = 0;
    }
    s_d1_ok = 0;
    s_death0 = s_deaths;
    s_tracked = s_loop;
    s_loops.deaths = 0;
}

static int qualified(const RbVm *vm)
{
    /* Debug entry starts the fight itself. It must not also swallow
     * the stage's own loads, or the picture never comes up. */
    if (!s_black || !s_run_real || !vm || s_stage != 5) {
        return 0;
    }
    sync_loops(vm);
    return kr_tlb_ready(&s_loops);
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
        s_loops.medals += d1 - s_d1;
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
    memset(&s_loops, 0, sizeof s_loops);
    s_death0 = 0;
    s_tracked = 0;
    s_have_track = 0;
    s_run_real = 0;
    s_black = 0;
    s_d1_ok = 0;
    s_khp = 0;
    s_noship = 0;
    s_big = 0;
    s_cyc_i = 0;
    s_cyc_t = 0;
    s_drop = 0;
    s_drop_dy = 0;
    s_drop_done = 0;
    s_saw_fort = 0;
    s_quiet = 0;
    memset(s_applied, 0, sizeof s_applied);
    memset(s_last_y, 0, sizeof s_last_y);
    memset(s_drop_org, 0, sizeof s_drop_org);
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
    /* No hold for the core. A script-visible body here was eating the
     * shot stream after the arms died, so only the geometry test hits it. */
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
    if (!v) {
        return 0;
    }
    return kr_is_projectile(v->type, v->kind, v->origin, v->seg, v->w, v->h);
}

static void note_shots(RbVm *vm)
{
    int i;

    memset(s_pshot, 0, sizeof s_pshot);
    if (!vm) {
        return;
    }
    for (i = 0; i < 256; i++) {
        RbEntView v;

        if (!vm_ent_view(vm, i, &v) || !is_player_shot(&v)) {
            continue;
        }
        s_pshot[i] = 1;
        s_px0[i] = v.x;
        s_py0[i] = v.y;
    }
}

/* Distance from a point to a segment, squared. */
static int seg_dist2(int px, int py, int ax, int ay, int bx, int by)
{
    int abx = bx - ax;
    int aby = by - ay;
    int apx = px - ax;
    int apy = py - ay;
    int ab2 = abx * abx + aby * aby;
    int t;
    int dx;
    int dy;

    if (ab2 <= 0) {
        return apx * apx + apy * apy;
    }
    t = apx * abx + apy * aby;
    if (t < 0) {
        t = 0;
    } else if (t > ab2) {
        t = ab2;
    }
    dx = apx - (int)(((long)abx * t) / ab2);
    dy = apy - (int)(((long)aby * t) / ab2);
    return dx * dx + dy * dy;
}

static int path_arm(int x0, int y0, int x1, int y1)
{
    int best = -1;
    int best_d = 0x3fffffff;
    int i;
    int s;
    int k;
    int dx = x1 - x0;
    int dy = y1 - y0;
    int steps = dx < 0 ? -dx : dx;
    int sy = dy < 0 ? -dy : dy;

    if (sy > steps) {
        steps = sy;
    }
    steps /= 6;
    if (steps < 1) {
        steps = 1;
    }
    if (steps > 16) {
        steps = 16;
    }
    for (i = 0; i < KR_TENT; i++) {
        if (s_thp[i] <= 0) {
            continue;
        }
        for (s = 0; s < KR_SEGS; s++) {
            int ax = s == 0 ? s_hx + SOCK_X[i] : s_px[i][s - 1];
            int ay = s == 0 ? s_hy + SOCK_Y[i] : s_py[i][s - 1];
            int bx = s_px[i][s];
            int by = s_py[i][s];
            int rad = (s + 1 == KR_SEGS) ? 22 : 16;

            for (k = 0; k <= steps; k++) {
                int x = x0 + dx * k / steps;
                int y = y0 + dy * k / steps;
                int d = seg_dist2(x, y, ax, ay, bx, by);

                if (d <= rad * rad && d < best_d) {
                    best_d = d;
                    best = i;
                }
            }
        }
    }
    return best;
}

static int on_core(int x, int y)
{
    return kr_core_hit(s_hx, s_hy, x, y);
}

static void note_arm_hit(RbVm *vm, int arm)
{
    int d = s_hit > 0 ? s_hit : 8;

    s_thp[arm] -= d;
    if (s_thp[arm] < 0) {
        s_thp[arm] = 0;
    }
    vm_add_score(vm, 10);
#if KRAKEN_DEBUG
    fprintf(stderr, "kraken hit arm=%d hp=%d/%d\n", arm, s_thp[arm], s_tmax);
    fflush(stderr);
#else
    (void)arm;
#endif
}

static void apply_damage(RbVm *vm)
{
    int i;
    int s;
    int sh;

    for (sh = 0; sh < 256; sh++) {
        RbEntView shot;
        int alive = vm_ent_view(vm, sh, &shot);
        int x0;
        int y0;
        int x1;
        int y1;
        int arm;
        int k;
        int steps;

        if (alive && !is_player_shot(&shot)) {
            continue;
        }
        if (!alive && !s_pshot[sh]) {
            continue;
        }
        if (alive) {
            x1 = shot.x;
            y1 = shot.y;
            if (s_pshot[sh]) {
                x0 = s_px0[sh];
                y0 = s_py0[sh];
            } else {
                x0 = x1;
                y0 = y1;
            }
        } else {
            x0 = x1 = s_px0[sh];
            y0 = y1 = s_py0[sh];
        }
        arm = path_arm(x0, y0, x1, y1);
        if (arm >= 0) {
            note_arm_hit(vm, arm);
            if (alive) {
                vm_ent_clear(vm, sh);
            }
            continue;
        }
        if (tent_alive_n() > 0) {
            continue;
        }
        steps = 1;
        {
            int adx = x1 - x0;
            int ady = y1 - y0;
            int n = adx < 0 ? -adx : adx;
            int m = ady < 0 ? -ady : ady;

            if (m > n) {
                n = m;
            }
            steps = n / 6;
            if (steps < 1) {
                steps = 1;
            }
        }
        for (k = 0; k <= steps; k++) {
            int x = x0 + (x1 - x0) * k / steps;
            int y = y0 + (y1 - y0) * k / steps;

            if (!on_core(x, y)) {
                continue;
            }
            s_khp -= s_hit > 0 ? s_hit : 8;
            if (s_khp < 0) {
                s_khp = 0;
            }
            vm_add_score(vm, 10);
#if KRAKEN_DEBUG
            fprintf(stderr, "kraken hit hull hp=%d/%d\n", s_khp, s_khp_max);
            fflush(stderr);
#endif
            if (alive) {
                vm_ent_clear(vm, sh);
            }
            break;
        }
    }
    for (i = 0; i < KR_TENT; i++) {
        int any = 0;

        if (s_thp[i] > 0) {
            continue;
        }
        for (s = 0; s < KR_SEGS; s++) {
            if (s_slot[i][s] >= 0) {
                any = 1;
            }
        }
        if (!any) {
            continue;
        }
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
#if KRAKEN_DEBUG
        fprintf(stderr, "kraken arm=%d down\n", i);
        fflush(stderr);
#endif
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


static int drop_skip(const RbEntView *v)
{
    if (!v) {
        return 1;
    }
    if (v->kind == 0x0012u || v->kind == 0x0014u) {
        return 1;
    }
    if (is_player_shot(v)) {
        return 1;
    }
    if ((v->w == 0 || v->h == 0) && !is_fort(v->origin)) {
        return 1;
    }
    return 0;
}

/* Push the stage 5 boss and the bursts spawned on it down the screen.
 * A new sprite takes the whole offset at once, so a blast that the
 * script plants on the boss's old spot appears where the boss is now,
 * already on its way off the bottom. */
static void drop_world(RbVm *vm)
{
    int i;

    s_drop_dy += 12;
    if (s_drop_dy > 640) {
        s_drop_dy = 640;
    }
    for (i = 0; i < 256; i++) {
        RbEntView v;
        int dy;
        int y;

        if (!vm_ent_view(vm, i, &v)) {
            s_drop_org[i] = 0;
            s_applied[i] = 0;
            continue;
        }
        if (v.origin != s_drop_org[i]) {
            s_drop_org[i] = v.origin;
            s_applied[i] = 0;
        }
        if (drop_skip(&v)) {
            continue;
        }
        /* The script wrote a fresh absolute Y. Apply the full fall
         * again or the body snaps back to the middle. */
        if (s_applied[i] > 0 && (int)v.y < (int)s_last_y[i] - 24) {
            s_applied[i] = 0;
        }
        dy = s_drop_dy - s_applied[i];
        if (dy == 0) {
            continue;
        }
        y = (int)v.y + dy;
        if (y > 560) {
            y = 560;
        }
        vm_ent_place(vm, i, v.x, (int16_t)y, v.bl, v.br, v.bt, v.bb);
        s_applied[i] = s_drop_dy;
        s_last_y[i] = (int16_t)y;
    }
}

static void watch_drop(RbVm *vm, int stage)
{
    RbVmInfo info;
    int i;
    int living = 0;
    int dead = 0;

    if (!vm || !s_black || s_phase != PH_OFF) {
        return;
    }
    vm_info(vm, &info);
    if (info.chunk != 6 || stage != 5) {
        s_drop = 0;
        s_drop_dy = 0;
        s_drop_done = 0;
        s_saw_fort = 0;
        s_quiet = 0;
        return;
    }
    for (i = 0; i < 256; i++) {
        RbEntView v;

        /* Kind 0041 is the core. It keeps fighting after the pylons
         * (kinds 0042-0045) are gone. The fall starts at its hp 0. */
        if (!vm_ent_view(vm, i, &v) || v.kind != 0x0041u) {
            continue;
        }
        if (v.hp > 0 && !(v.type & 0x0400u)) {
            living++;
            s_saw_fort = 1;
        } else {
            dead++;
        }
    }
    if (living > 0) {
        s_quiet = 0;
        /* A restarted stage 5 brings the core back. Allow another fall. */
        if (s_drop_done && !s_drop) {
            s_drop_done = 0;
        }
    } else if (s_saw_fort && dead && !s_drop && !s_drop_done) {
        s_quiet = 2;
    } else if (s_saw_fort && !s_drop_done) {
        s_quiet++;
    }
    /* hp 0 starts at once. A vanished core waits two frames so a
     * one-frame gap during the solo fight does not drop it. */
    if (s_saw_fort && s_quiet >= 2 && !s_drop && !s_drop_done) {
        s_drop = 1;
        s_drop_dy = 0;
        memset(s_applied, 0, sizeof s_applied);
        memset(s_last_y, 0, sizeof s_last_y);
        memset(s_drop_org, 0, sizeof s_drop_org);
    }
    if (!s_drop || s_drop_done) {
        return;
    }
    drop_world(vm);
    vm_redraw(vm);
    if (s_drop_dy >= 480) {
        s_drop = 0;
        s_drop_done = 1;
    }
}

static void ship_ready(RbVm *vm)
{
    int i;

    for (i = 0; i < 256; i++) {
        RbEntView v;

        if (!vm_ent_view(vm, i, &v)) {
            continue;
        }
        if (v.kind != 0x0012u && v.kind != 0x0014u) {
            continue;
        }
        if (v.y >= 200) {
            continue;
        }
        vm_ent_place(vm, i, v.x, 330, v.bl, v.br, v.bt, v.bb);
        vm_ent_unhide(vm, i);
    }
}

static int core_opaque(const RbGfx *gfx, int index)
{
    uint32_t pix[256];
    int i;
    int n = 0;

    if (!gfx || index < 0 || !gfx_sprite_rgba(gfx, CORE_BANK, index, pix)) {
        return 0;
    }
    for (i = 0; i < 256; i++) {
        if (pix[i] != 0) {
            n++;
        }
    }
    return n;
}

static void hull_say(const char *msg)
{
#if KRAKEN_DEBUG
    if (s_hull_said == 2) {
        return;
    }
    fprintf(stderr, "kraken hull: %s\n", msg);
    fflush(stderr);
    s_hull_said = 1;
#else
    (void)msg;
#endif
}

/* Stage 5 core, the frame it uses after the side pylons are gone.
 * Chunk 6 holds the pattern bytes. Bank 36 is the sprite sheet the
 * stage binds with opcode 0x32. */
static int load_core(RbVm *vm)
{
    const uint8_t *script;
    uint32_t n = 0;
    uint8_t cells[CORE_TW * CORE_TH];
    const RbGfx *gfx;
    int rec = -1;
    int solid;
    int k;
    uint32_t i;
    int row;
    int col;
    int opaque = 0;

    if (s_core_ok || !vm) {
        return s_core_ok;
    }
    if (!vm_ensure_record(vm, CORE_CH)) {
        hull_say("decode failed: chunk 6 not loaded");
        return 0;
    }
    script = vm_chunk_bytes(vm, CORE_CH, &n);
    if (!script || n < (uint32_t)CORE_PAT + sizeof cells) {
        hull_say("decode failed: pattern 41223 past end of chunk 6");
        return 0;
    }
    memcpy(cells, script + CORE_PAT, sizeof cells);
    gfx = vm_gfx(vm);
    /* Stage 5 has usually bound bank 36 already. Keep that sheet if
     * the core cells draw. Rebinding is only for the title jump. */
    solid = 0;
    for (k = 0; k < (int)sizeof cells; k++) {
        if (core_opaque(gfx, cells[k]) >= 8) {
            solid++;
        }
    }
    if (solid >= 3) {
        rec = 0;
    }
    for (i = 0; i + 3u < n && rec < 0; i++) {
        if (script[i] != 0x32u || script[i + 1] != (uint8_t)CORE_BANK) {
            continue;
        }
        if (!vm_bind_record(vm, (uint8_t)CORE_BANK, script[i + 2])) {
            continue;
        }
        solid = 0;
        for (k = 0; k < (int)sizeof cells; k++) {
            if (core_opaque(gfx, cells[k]) >= 8) {
                solid++;
            }
        }
        if (solid >= 3) {
            rec = script[i + 2];
        }
    }
    if (rec < 0) {
        hull_say("decode failed: bank 36 has no core cells");
        return 0;
    }
    memset(s_core, 0, sizeof s_core);
    for (row = 0; row < CORE_TH; row++) {
        for (col = 0; col < CORE_TW; col++) {
            uint32_t pix[256];
            int y;
            int x;
            int index = cells[row * CORE_TW + col];

            if (!gfx_sprite_rgba(gfx, CORE_BANK, index, pix)) {
                continue;
            }
            for (y = 0; y < 16; y++) {
                for (x = 0; x < 16; x++) {
                    uint32_t p = pix[y * 16 + x];

                    if (p == 0) {
                        continue;
                    }
                    s_core[(row * 16 + y) * (CORE_TW * 16) + col * 16 + x] = p;
                    opaque++;
                }
            }
        }
    }
    if (opaque < 40) {
        hull_say("decode failed: core frame is empty");
        return 0;
    }
    s_core_ok = 1;
#if KRAKEN_DEBUG
    fprintf(stderr, "kraken hull: chunk=6 bank=36 pat=41223 rec=%d 3x5\n", rec);
    fflush(stderr);
    s_hull_said = 2;
#else
    (void)rec;
#endif
    return 1;
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
    /* Half of Kraken's life per arm, and a hit is a fixed slice of that
     * so the arm can actually be shot down. A raw boss word in the
     * tens of thousands made the arms unkillable. */
    base = 480;
    if (s_boss_hp >= 64 && s_boss_hp <= 960) {
        base = s_boss_hp;
    }
    s_khp_max = base;
    s_khp = base;
    s_tmax = base / 2;
    if (s_tmax < 32) {
        s_tmax = 32;
    }
    s_hit = kr_shot_damage(base);
    for (i = 0; i < KR_TENT; i++) {
        s_thp[i] = s_tmax;
        s_boom[i] = 0;
    }
    s_drop = 0;
    s_drop_done = 1;
    s_hx = 320;
    s_hy = 440;
    s_timer = 0;
#if KRAKEN_DEBUG
    /* F9 plays the whole appearance: the core falls off, the white
     * flash, the three-second wait, then the rise and the arms. */
    if (s_arm) {
        s_phase = PH_FALL;
        s_hy = 72;
    } else
#endif
    {
        s_phase = PH_WAIT;
    }
    ship_ready(vm);
    s_noship = 0;
    s_big = 0;
    s_cyc_i = 0;
    s_cyc_t = 0;
    sys_play_music(8);
    load_core(vm);
#if KRAKEN_DEBUG
    s_arm = 0;
    fprintf(stderr, "kraken hp core=%d arm=%d hit=%d\n", s_khp, s_tmax, s_hit);
    fflush(stderr);
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

void kraken_before(RbVm *vm, int deaths, int fair, int loop, int stage, int paused,
                   int black)
{
    int shipx = 320;
    int shipy = 300;
    int stretch = 256;

    s_deaths = deaths;
    s_black = black;
    s_loop = loop;
    s_stage = stage;
    /* A real Black Label run from stage 1. A later death or continue
     * does not clear this; it only spoils the loop in progress. */
    if (black && fair && loop <= 1 && stage <= 1) {
        s_run_real = 1;
    }
    if (!vm) {
        return;
    }
    note_loop(vm);
    if (s_phase == PH_OFF) {
        vm_set_kraken_take(vm, qualified(vm));
        return;
    }
    if (paused) {
        place_parts(vm);
        return;
    }
    if (s_phase == PH_RETURN) {
        /* The central part rises first. The arms come out after it arrives. */
        if (s_timer < 50) {
            stretch = 0;
        } else {
            stretch = (s_timer - 50) * 256 / 30;
            if (stretch > 256) {
                stretch = 256;
            }
        }
    } else if (s_phase != PH_FIGHT) {
        stretch = 0;
    }
    if (vm_ship_pos(vm, &shipx, &shipy) != 1) {
        shipx = 320;
        shipy = 300;
    }
    if (s_phase == PH_FALL) {
        s_hx = 320;
        s_hy += 12;
    } else if (s_phase == PH_WAIT || s_phase == PH_FLASH) {
        s_hy = 440;
        s_hx = 320;
    } else if (s_phase == PH_RETURN) {
        s_hx = 320;
        if (s_timer < 50) {
            s_hy = 440 - s_timer * (440 - 96) / 50;
        } else {
            s_hy = 96;
        }
    }
    if (s_phase == PH_FIGHT || (s_phase == PH_RETURN && stretch > 0)) {
        if (stretch < 8) {
            stretch = 8;
        }
        pose_at(shipx, shipy, stretch);
    }
    if (s_phase == PH_FIGHT) {
        note_shots(vm);
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

    if (!vm) {
        return;
    }
    if (s_black && s_run_real) {
        medal_tick(vm);
    }
    watch_world(vm, s_stage);
    scan_patterns(vm);
    if (s_phase == PH_OFF) {
        watch_drop(vm, stage);
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
    if (s_phase == PH_FALL && s_hy > 460) {
        s_phase = PH_FLASH;
        s_timer = 0;
    } else if (s_phase == PH_FLASH && s_timer >= 24) {
        s_phase = PH_WAIT;
        s_timer = 0;
    } else if (s_phase == PH_WAIT && s_timer >= (3 * 92) / 3) {
        s_phase = PH_RETURN;
        s_timer = 0;
        s_hx = 320;
        s_hy = 440;
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
            /* In the middle of the screen, the way the stage boss used to die. */
            s_hx = 320;
            s_hy = 150;
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

#define METAL_OUT 0x5A6570FFu
#define METAL_SH  0xD8E0E8FFu
#define METAL_LIT 0xF5F8FCFF
#define METAL_HI  0xFFFFFFFF
#define JOINT_RED 0xE03830FFu
#define ORB_HALO  0xFF58C8FFu
#define ORB_BODY  0xFF8AD8FFu
#define ORB_CORE  0xFFF8FFFFu

static void blit_hull(uint32_t *px, int w, int h, int cx, int cy)
{
    int yy;
    int xx;

    if (s_core_ok) {
        int x0 = cx - (CORE_TW * 16) / 2;
        int y0 = cy - (CORE_TH * 16) / 2;
        int y;
        int x;

        for (y = 0; y < CORE_TH * 16; y++) {
            for (x = 0; x < CORE_TW * 16; x++) {
                put_px(px, w, h, x0 + x, y0 + y,
                       s_core[y * (CORE_TW * 16) + x]);
            }
        }
        return;
    }
    /* Shown only until chunk 6 / bank 36 can be decoded. */
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
                     int thick)
{
    int dx = x1 - x0;
    int dy = y1 - y0;
    int steps = dx < 0 ? -dx : dx;
    int sy = dy < 0 ? -dy : dy;
    int s;
    int rad = thick / 2;
    int stop;

    if (sy > steps) {
        steps = sy;
    }
    if (steps < 1) {
        steps = 1;
    }
    if (rad < 3) {
        rad = 3;
    }
    /* Stop short of the next joint so a dark gap separates the plates. */
    stop = steps - (rad / 2 + 2);
    if (stop < steps / 2) {
        stop = steps / 2;
    }
    for (s = 0; s <= stop; s++) {
        int x = x0 + dx * s / steps;
        int y = y0 + dy * s / steps;
        int oy;
        int ox;

        for (oy = -rad - 1; oy <= rad + 1; oy++) {
            for (ox = -rad - 1; ox <= rad + 1; ox++) {
                int d2 = ox * ox + oy * oy;
                uint32_t col;

                if (d2 > (rad + 1) * (rad + 1)) {
                    continue;
                }
                if (d2 > rad * rad) {
                    col = METAL_OUT;
                } else if (oy < -(rad / 5)) {
                    col = METAL_HI;
                } else if (oy > rad / 2) {
                    col = METAL_SH;
                } else {
                    col = METAL_LIT;
                }
                put_px(px, w, h, x + ox, y + oy, col);
            }
        }
    }
}

static void draw_joint(uint32_t *px, int w, int h, int cx, int cy, int rad)
{
    int y;
    int x;
    int outer = rad / 3 + 2;
    int inner;

    if (outer < 3) {
        outer = 3;
    }
    if (outer > 5) {
        outer = 5;
    }
    inner = outer - 2;
    if (inner < 1) {
        inner = 1;
    }
    for (y = -outer; y <= outer; y++) {
        for (x = -outer; x <= outer; x++) {
            int d2 = x * x + y * y;

            if (d2 > outer * outer || d2 < inner * inner) {
                continue;
            }
            put_px(px, w, h, cx + x, cy + y, d2 > (outer - 1) * (outer - 1) ? METAL_OUT : JOINT_RED);
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
        int thick = 20 - s;

        if (thick < 11) {
            thick = 11;
        }
        draw_seg(px, w, h, x0, y0, x1, y1, thick);
        if (s + 1 == KR_SEGS) {
            draw_orb(px, w, h, x1, y1);
        } else {
            draw_joint(px, w, h, x1, y1, thick);
        }
        x0 = x1;
        y0 = y1;
    }
}

static void draw_flash(uint32_t *px, int w, int h, int t)
{
    int y;
    int x;

    if (t < 8 || t > 22) {
        return;
    }
    for (y = 0; y < h; y++) {
        for (x = 80; x < 560 && x < w; x++) {
            if (t < 16 || ((x + y) & 3) >= (t - 16)) {
                px[(size_t)y * (size_t)w + (size_t)x] = 0xFFFFFFFFu;
            }
        }
    }
}

void kraken_draw(RbVm *vm, const RbGfx *gfx, uint32_t *px, int w, int h)
{
    int i;

    (void)gfx;
    if (!px || s_phase == PH_OFF || w < 640 || h < 400) {
        return;
    }
    if (vm && !s_core_ok) {
        load_core(vm);
    }
    if (s_phase == PH_FALL) {
        blit_hull(px, w, h, s_hx, s_hy);
        draw_boom(px, w, h, gfx, s_hx, s_hy - 20, (s_timer % 12) + 4, 0);
        draw_boom(px, w, h, gfx, s_hx - 16, s_hy + 10, (s_timer % 10) + 2, 0);
        draw_boom(px, w, h, gfx, s_hx + 14, s_hy, (s_timer % 8) + 3, 0);
        return;
    }
    if (s_phase == PH_FLASH) {
        draw_flash(px, w, h, s_timer < 8 ? 8 : s_timer);
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
        if (s_phase == PH_FIGHT || (s_phase == PH_RETURN && s_timer >= 50)) {
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
    if (s_phase == PH_DEAD) {
        draw_flash(px, w, h, s_timer);
    }
}
