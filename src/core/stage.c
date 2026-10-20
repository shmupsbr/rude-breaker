/**
 * Stage director, ported from the script entity at chunk 0x79CB (stages
 * 1-4) / 0x79DD (stage 5) and the timeline entity the stage root spawns
 * after 17 frames (chunk 0x81CF and the matching sites in later chunks).
 *
 * Scroll and the speed regulator are opcode 0x4B plus the tail at 0x8184.
 * Cell waves and the timeline come from tools/gen_stage_tables.py.
 * The stream is child 0x4848. It is spawned when the director's
 * counter (+0x34) is a multiple of 512, and that counter increments
 * on every loop frame. Opcode 0x4D runs once before the loop and
 * steps the map 25 cells. The cell-edge object is 0x40A3.
 *
 * RNG is game-image 0xE4E / 0xE5D. The original seeds it from INT 1Ch;
 * this port uses the fixed seed 0x1234 so a stage is reproducible.
 */
#include "stage.h"
#include "stage_data.h"

#include <string.h>

#define STREAM_TYPE 0x8020u
#define POPUP_TYPE  0x8010u
#define TIMELINE_ARM 17u

typedef struct RbDir {
    uint16_t sub;       /* [bp+0x7A] */
    uint16_t cell;      /* [bp+0x7C] */
    uint16_t speed;     /* [bp+0x82] */
    uint16_t target;    /* [bp+0x86] */
    uint16_t acc;       /* [bp+0x80] */
    uint16_t scroll_y;  /* [bp+0x74] */
    uint16_t counted;   /* [bp+0xAE] */
    uint16_t ev;
    uint16_t delay;
    uint16_t stage;     /* 1..5 */
    uint8_t  carry;     /* [bp+0xBD] */
    uint8_t  phase;     /* [bp+0xC8] */
    uint8_t  arm;
    uint8_t  delay_armed;
    uint8_t  wait_gate;
    uint8_t  timeline_done;
    uint8_t  terminal;
    uint8_t  finished;
    uint16_t rng_e7e;
    uint16_t rng_5c;
    uint16_t rng_64;
    uint16_t stream_n;  /* director [si+0x34]; spawn when this & 0x1FF == 0 */
    uint8_t  rng_cf;
    uint8_t  primed;    /* 0 until opcode 0x4D has run */
} RbDir;

static RbDir g_dir;

static uint8_t rng_byte(RbDir *s)
{
    uint16_t ax = s->rng_5c;
    uint16_t bx = s->rng_e7e;
    uint8_t al;
    uint8_t ah;
    uint8_t bl;
    uint8_t bh;
    uint8_t cf;
    uint8_t rcr_cf;
    uint8_t rol_cf;
    uint32_t wide;

    bx = (uint16_t)(bx + s->rng_64);
    bx = (uint16_t)(bx + ax);
    al = (uint8_t)ax;
    ah = (uint8_t)(ax >> 8);
    bl = (uint8_t)bx;
    bh = (uint8_t)(bx >> 8);
    /* XCHG BH, AL */
    cf = bh;
    bh = al;
    al = cf;
    ax = (uint16_t)(al | ((uint16_t)ah << 8));
    bx = (uint16_t)(bl | ((uint16_t)bh << 8));

    rcr_cf = (uint8_t)(bx & 1u);
    bx = (uint16_t)((bx >> 1) | (s->rng_cf ? 0x8000u : 0u));

    rol_cf = (uint8_t)((ax >> 15) & 1u);
    ax = (uint16_t)((ax << 1) | rcr_cf);

    /* SBB BX, AX */
    wide = (uint32_t)bx - (uint32_t)ax - (uint32_t)rol_cf;
    bx = (uint16_t)wide;
    /* SUB BX, AX (replaces CF) */
    wide = (uint32_t)bx - (uint32_t)ax;
    bx = (uint16_t)wide;
    /* ADD AX, BX */
    wide = (uint32_t)ax + (uint32_t)bx;
    ax = (uint16_t)wide;
    s->rng_e7e = (uint16_t)(s->rng_e7e + ax);
    al = (uint8_t)ax;
    ah = (uint8_t)(ax >> 8);
    al = (uint8_t)(al + ah);
    s->rng_cf = (uint8_t)(((uint16_t)(uint8_t)ax + (uint16_t)ah) > 0xFFu);
    return al;
}

static uint16_t rng_mix(RbDir *s)
{
    uint8_t hi = rng_byte(s);
    uint8_t lo = rng_byte(s);

    return (uint16_t)(((uint16_t)hi << 8) | lo);
}

static uint16_t rng_mod(RbDir *s, uint16_t mod)
{
    if (mod == 0u) {
        return 0;
    }
    return (uint16_t)(rng_mix(s) % mod);
}

static uint8_t rcl_byte(uint8_t bd, uint8_t bit_in, uint8_t *cf_out)
{
    *cf_out = (uint8_t)((bd >> 7) & 1u);
    return (uint8_t)((bd << 1) | (bit_in & 1u));
}

static void alu_add(uint16_t *dst, uint16_t imm)
{
    uint32_t sum = (uint32_t)(*dst) + (uint32_t)imm;
    uint8_t cf;

    g_dir.carry = rcl_byte(g_dir.carry, (uint8_t)(sum > 0xFFFFu), &cf);
    *dst = (uint16_t)sum;
    (void)cf;
}

static uint8_t alu_sub(uint16_t *dst, uint16_t imm)
{
    uint8_t borrow = (uint8_t)(*dst < imm);
    uint8_t cf;

    *dst = (uint16_t)(*dst - imm);
    g_dir.carry = rcl_byte(g_dir.carry, borrow, &cf);
    return cf;
}

static void alu_sbb0(uint16_t *dst, uint8_t cf_in)
{
    uint8_t bit = (uint8_t)(g_dir.carry & 1u);
    uint16_t cur = *dst;
    uint16_t res;
    uint8_t sbb_cf;
    uint8_t ignored;

    g_dir.carry = (uint8_t)((g_dir.carry >> 1) | (cf_in << 7));
    res = (uint16_t)(cur - bit);
    sbb_cf = (uint8_t)(cur < bit);
    g_dir.carry = rcl_byte(g_dir.carry, sbb_cf, &ignored);
    *dst = res;
}

static void alu_adc0(uint16_t *dst, uint8_t cf_in)
{
    uint8_t bit = (uint8_t)(g_dir.carry & 1u);
    uint32_t sum;
    uint8_t ignored;

    g_dir.carry = (uint8_t)((g_dir.carry >> 1) | (cf_in << 7));
    sum = (uint32_t)(*dst) + (uint32_t)bit;
    g_dir.carry = rcl_byte(g_dir.carry, (uint8_t)(sum > 0xFFFFu), &ignored);
    *dst = (uint16_t)sum;
}

static void scroll_step(void)
{
    uint16_t width = 0x1E0u;
    uint16_t mod = (uint16_t)(width << 4);
    int16_t sub = (int16_t)g_dir.sub;
    int16_t spd = (int16_t)g_dir.speed;
    uint16_t sy;
    uint16_t cell;
    uint16_t prev;

    sub = (int16_t)(sub - spd);
    if (sub < 0) {
        sub = (int16_t)(sub + (int16_t)mod);
    }
    if ((uint16_t)sub >= mod) {
        sub = (int16_t)((uint16_t)sub - mod);
    }
    g_dir.sub = (uint16_t)sub;

    sy = (uint16_t)(g_dir.scroll_y + g_dir.speed);
    if ((int16_t)sy <= 0 && g_dir.speed != 0u && g_dir.scroll_y == 0u) {
        /* speed is non-negative in every decoded stage; the signed
         * `JG` wrap in 0x4168 only fires for a negative sum. */
    }
    while (sy > 0x190u) {
        sy = (uint16_t)(sy - 0x190u);
    }
    g_dir.scroll_y = sy;

    prev = g_dir.cell;
    cell = (uint16_t)(g_dir.sub >> 4);
    g_dir.cell = cell;
    (void)prev;
}

static int cell_changed_to(uint16_t prev)
{
    return g_dir.cell != prev;
}

static int match_cell(const RbPhaseRule *rule)
{
    if (rule->match_op == RB_MATCH_ALWAYS) {
        return 1;
    }
    if (rule->match_op == RB_MATCH_EQ) {
        return g_dir.cell == rule->match_cell;
    }
    if (rule->match_op == RB_MATCH_LE) {
        return g_dir.cell <= rule->match_cell;
    }
    if (rule->match_op == RB_MATCH_GE) {
        return g_dir.cell >= rule->match_cell;
    }
    return 0;
}

static const RbStageInfo *info_of(void)
{
    uint16_t i = g_dir.stage;

    if (i < 1u || i > RB_STAGE_COUNT) {
        i = 1u;
    }
    return &rb_stages[i - 1u];
}

static void spawn_def(RbEnemyPool *pool, uint16_t index, int16_t x_override, int use_override)
{
    const RbStageInfo *info = info_of();
    const RbSpawnDef *def;
    RbEnemySpec spec;
    int16_t x;

    if (index >= info->spawn_count) {
        return;
    }
    def = &info->spawns[index];
    if (use_override) {
        x = x_override;
    } else if (def->x_mod != 0u) {
        x = (int16_t)((int16_t)rng_mod(&g_dir, def->x_mod) + def->x_add);
    } else {
        x = def->x;
    }
    memset(&spec, 0, sizeof(spec));
    spec.x = x;
    spec.y = def->y;
    spec.hp = def->hp;
    spec.vx = def->vx;
    spec.vy = def->vy;
    spec.ax = def->ax;
    spec.ay = def->ay;
    spec.angle = def->angle;
    spec.spin = def->spin;
    spec.speed = def->speed;
    spec.box_l = def->box_l;
    spec.box_r = def->box_r;
    spec.box_t = def->box_t;
    spec.box_b = def->box_b;
    spec.type = def->type;
    spec.mover = def->mover;
    spec.counted = def->counted;
    if (enemy_spawn_spec(pool, &spec) >= 0 && spec.counted) {
        g_dir.counted++;
    }
}

static void prelude_4d(void)
{
    uint16_t n;

    /* Opcode 0x4D (0x41B7): 25 steps. scroll_y += 16, wrap above 0x190.
     * cell -= 1 with the signed wrap on [bp+0x7E]. No wave check. */
    for (n = 0; n < 0x19u; n++) {
        int16_t cell = (int16_t)g_dir.cell;
        uint16_t sy = (uint16_t)(g_dir.scroll_y + 0x10u);

        if (sy > 0x190u) {
            sy = (uint16_t)(sy - 0x190u);
        }
        g_dir.scroll_y = sy;
        cell = (int16_t)(cell - 1);
        if (cell >= 0x1E0) {
            cell = (int16_t)(cell - 0x1E0);
        }
        if (cell < 0) {
            cell = (int16_t)(cell + 0x1E0);
        }
        g_dir.cell = (uint16_t)cell;
        g_dir.sub = (uint16_t)(g_dir.cell << 4);
    }
}

static void spawn_stream(RbEnemyPool *pool)
{
    RbEnemySpec spec;

    /* 1B at the stream site: spawn only when ([si+0x34] & 0x1FF) == 0.
     * The add of 1 runs on the spawn frame and on the skipped frames. */
    if ((g_dir.stream_n & 0x1FFu) != 0u) {
        g_dir.stream_n++;
        return;
    }
    g_dir.stream_n++;

    memset(&spec, 0, sizeof(spec));
    /* Child 0x4848: X = (rng % 0x160) + 0x90, Y = 0,
     * vx = (rng % 1) - 0x80, vy = 0x0300, ay = -2, hp = 20, box ±16. */
    spec.x = (int16_t)(rng_mod(&g_dir, 0x160u) + 0x90u);
    spec.y = 0;
    spec.hp = 20;
    spec.vx = (uint16_t)(rng_mod(&g_dir, 1u) - 0x80u);
    spec.vy = 0x0300u;
    spec.ay = 0xFFFEu;
    spec.type = STREAM_TYPE;
    spec.mover = RB_MOVER_STREAM;
    spec.box_l = -16;
    spec.box_r = 16;
    spec.box_t = -16;
    spec.box_b = 16;
    (void)enemy_spawn_spec(pool, &spec);
}

static void spawn_popup(RbEnemyPool *pool)
{
    RbEnemySpec spec;
    uint16_t roll;

    memset(&spec, 0, sizeof(spec));
    roll = rng_mod(&g_dir, 3u);
    spec.x = (int16_t)(rng_mod(&g_dir, 0x1E0u) + 0x50u);
    spec.y = 0;
    spec.hp = 1;
    spec.vy = (uint16_t)((roll << 7) + 0x80u);
    spec.type = POPUP_TYPE;
    spec.mover = RB_MOVER_STREAM;
    spec.box_l = -4;
    spec.box_r = 4;
    spec.box_t = -4;
    spec.box_b = 4;
    (void)enemy_spawn_spec(pool, &spec);
}

static void apply_phase(void)
{
    const RbStageInfo *info = info_of();
    uint8_t r;

    for (r = 0; r < info->rule_count; r++) {
        const RbPhaseRule *rule = &info->rules[r];

        if (rule->phase != g_dir.phase) {
            continue;
        }
        if (rule->enter_speed != RB_SPEED_KEEP) {
            g_dir.target = rule->enter_speed;
        }
        if (rule->enter_next != RB_PHASE_KEEP) {
            g_dir.phase = rule->enter_next;
        }
        if (match_cell(rule)) {
            if (rule->match_speed != RB_SPEED_KEEP) {
                g_dir.target = rule->match_speed;
            }
            if (rule->rewind_add != 0u) {
                g_dir.sub = (uint16_t)((g_dir.sub & 0x000Fu) + rule->rewind_add);
                g_dir.cell = (uint16_t)(g_dir.sub >> 4);
            }
            if (rule->next_phase != RB_PHASE_KEEP) {
                g_dir.phase = rule->next_phase;
            }
            if (r + 1u == info->rule_count && rule->match_op != RB_MATCH_ALWAYS) {
                g_dir.terminal = 1;
            }
        }
        break;
    }
}

static void spawn_waves(RbEnemyPool *pool)
{
    const RbStageInfo *info = info_of();
    uint16_t i;

    for (i = 0; i < info->wave_count; i++) {
        if (info->waves[i].cell == g_dir.cell) {
            spawn_def(pool, info->waves[i].spawn, 0, 0);
        }
    }
}

static void regulate_speed(void)
{
    uint16_t marker = 0;
    uint16_t ac;
    uint8_t cf;
    int16_t signed_ac;

    /* [bp+0x88]++ is an ADD that rotates the carry byte. The counter
     * itself is not read by the spawns. */
    marker = 0;
    alu_add(&marker, 1u);

    ac = g_dir.speed;
    /* MOV into [bp+0xAC], then SUB the target. */
    cf = alu_sub(&ac, g_dir.target);
    (void)cf;
    signed_ac = (int16_t)ac;
    if (signed_ac == 0) {
        return;
    }
    if (signed_ac > 0) {
        cf = alu_sub(&g_dir.acc, 0x1000u);
        alu_sbb0(&g_dir.speed, cf);
    } else {
        uint8_t add_cf;
        uint32_t sum = (uint32_t)g_dir.acc + 0x1000u;

        g_dir.acc = (uint16_t)sum;
        add_cf = 0;
        g_dir.carry = rcl_byte(g_dir.carry, (uint8_t)(sum > 0xFFFFu), &add_cf);
        alu_adc0(&g_dir.speed, add_cf);
    }
}

static void run_timeline(RbEnemyPool *pool)
{
    const RbStageInfo *info = info_of();

    if (g_dir.timeline_done) {
        return;
    }
    if (g_dir.arm > 0u) {
        g_dir.arm--;
        return;
    }
    if (g_dir.wait_gate) {
        if (g_dir.counted != 0u) {
            return;
        }
        g_dir.wait_gate = 0;
    }

    for (;;) {
        const RbStageEvent *ev;

        if (g_dir.ev >= info->event_count) {
            g_dir.timeline_done = 1;
            return;
        }
        ev = &info->events[g_dir.ev];
        if (!g_dir.delay_armed) {
            g_dir.delay = ev->delay;
            g_dir.delay_armed = 1;
        }
        if (g_dir.delay > 0u) {
            g_dir.delay--;
            return;
        }
        g_dir.delay_armed = 0;
        g_dir.ev++;
        if (ev->kind == RB_EV_GATE) {
            g_dir.wait_gate = 1;
            return;
        }
        if (ev->kind == RB_EV_PHASE) {
            g_dir.phase = (uint8_t)ev->arg;
            continue;
        }
        spawn_def(pool, ev->arg, 0, 0);
    }
}

void stage_begin(uint16_t stage_index)
{
    memset(&g_dir, 0, sizeof(g_dir));
    if (stage_index < 1u || stage_index > RB_STAGE_COUNT) {
        stage_index = 1u;
    }
    g_dir.stage = stage_index;
    /* Init block at chunk 0x7959 (stage 5's copy is the same constants). */
    g_dir.sub = 0x1E00u;
    g_dir.cell = 0x1E0u;
    g_dir.speed = 15u;
    g_dir.target = 4u;
    g_dir.scroll_y = 0;
    g_dir.phase = 0;
    g_dir.arm = TIMELINE_ARM;
    g_dir.rng_e7e = 0x1234u;
    g_dir.rng_cf = 0;
}

void stage_note_despawn(uint16_t counted)
{
    if (counted > g_dir.counted) {
        g_dir.counted = 0;
    } else {
        g_dir.counted = (uint16_t)(g_dir.counted - counted);
    }
}

void stage_tick(RbEnemyPool *pool)
{
    uint16_t prev = g_dir.cell;
    int changed;

    if (!g_dir.primed) {
        /* Director's first frame is opcode 0x4D and a yield. The stage
         * root is in its arm on that same frame, so the timeline waits. */
        prelude_4d();
        g_dir.primed = 1;
        if (g_dir.arm > 0u) {
            g_dir.arm--;
        }
        return;
    }

    scroll_step();
    changed = cell_changed_to(prev);
    if (changed) {
        apply_phase();
        spawn_waves(pool);
        if (info_of()->popup && (g_dir.cell < 0x70u || g_dir.cell > 0x130u)) {
            spawn_popup(pool);
        }
    }
    regulate_speed();
    spawn_stream(pool);
    run_timeline(pool);
    if (g_dir.timeline_done && g_dir.counted == 0u && g_dir.terminal) {
        g_dir.finished = 1;
    }
}

int stage_finished(void)
{
    return g_dir.finished;
}

uint16_t stage_index(void)
{
    return g_dir.stage;
}

uint16_t stage_cell(void)
{
    return g_dir.cell;
}

int16_t stage_scroll_y(void)
{
    return (int16_t)g_dir.scroll_y;
}

int16_t stage_scroll_speed(void)
{
    return (int16_t)g_dir.speed;
}
