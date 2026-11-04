/* cfg_check: rudebreaker.cfg round trips without game data.
 *
 * Builds game.c into this program, writes cfg files beside it and
 * checks that an old cfg (an earlier build), a new one, and lines this
 * build does not know all load; that save then load keeps the values;
 * and that RESTORE DEFAULTS and ORIGINAL PRESET leave RBRANK.DAT as it
 * was (no VM: nothing is rewritten, nothing is removed).
 *
 *   make cfg-check   (runs it) */
#include "../src/core/game.c"

static int g_fail;

#define CHECK(c) do { if (!(c)) { fprintf(stderr, "cfg_check: FAIL %s:%d %s\n", \
    __FILE__, __LINE__, #c); g_fail = 1; } } while (0)

static void put(const char *path, const char *text)
{
    FILE *fp = fopen(path, "wb");

    if (!fp) {
        fprintf(stderr, "cfg_check: cannot write %s\n", path);
        exit(2);
    }
    fputs(text, fp);
    fclose(fp);
}

static long fsize(const char *path)
{
    FILE *fp = fopen(path, "rb");
    long n;

    if (!fp) {
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    n = ftell(fp);
    fclose(fp);
    return n;
}

int main(void)
{
    static RbGame g;
    char dir[1024], cfg[1200], rank[1200], buf[4096];
    uint8_t blob[230];
    FILE *fp;
    size_t n;
    int i;

    if (rb_exe_dir(dir, sizeof dir) != 0) {
        fprintf(stderr, "cfg_check: no exe dir\n");
        return 2;
    }
    snprintf(g.asset_dir, sizeof g.asset_dir, "%s", dir);
    snprintf(cfg, sizeof cfg, "%s/rudebreaker.cfg", dir);
    snprintf(rank, sizeof rank, "%s/RBRANK.DAT", dir);
    for (i = 0; i < (int)sizeof blob; i++) {
        blob[i] = (uint8_t)(i * 7 + 3);
    }
    fp = fopen(rank, "wb");
    if (!fp) {
        return 2;
    }
    fwrite(blob, 1, sizeof blob, fp);
    fclose(fp);

    /* 1. An old cfg (no iframes / master_volume / joystick lines) plus
     * lines no build knows: loads, new options take their defaults. */
    put(cfg, "bullets=pink\ndefaults9r=1\npalette=modern\ntraining=off\n"
             "rank=on\npanels=off\ndefaults=9q\nloop=on\n"
             "sound_channels=extra\ndefaults9zf=1\nrank_mode=classic\n"
             "attract=off\nhitbox=off\nfullscreen=off\n"
             "some_future_key=42\n# a comment\n\nweird line without equals\n");
    load_cfg(&g);
    CHECK(g_loop_on == 1);
    CHECK(g_sndx == 1);
    CHECK(g_attract_on == 0);
    CHECK(g_aura_on == 3);     /* NEW - BLUE: the 0.10.1 default */
    CHECK(g_mvol == 100);
    CHECK(g_joy_off_saved == 0);
    CHECK(g_scaling == 1);     /* SHARP: the 0.10.1 default */
    CHECK(g_vu_style == 0);

    /* 2. A new cfg: the new keys load. */
    put(cfg, "loop=off\niframes=new\nmaster_volume=40\njoystick=off\n"
             "master_volume_typo=5\n");
    load_cfg(&g);
    CHECK(g_loop_on == 0);
    CHECK(g_aura_on == 1);
    CHECK(g_mvol == 40);
    CHECK(g_joy_off_saved == 1);
    /* 2b. I-FRAMES colours (new = fire), VU style, 0.10.1 scaling. */
    put(cfg, "iframes=lime\nvu=spectrum\n");
    load_cfg(&g);
    CHECK(g_aura_on == 2 && g_vu_style == 1);
    put(cfg, "iframes=blue\n");
    load_cfg(&g);
    CHECK(g_aura_on == 3 && g_vu_style == 0);
    put(cfg, "iframes=fire\n");
    load_cfg(&g);
    CHECK(g_aura_on == 1);
    g_aura_on = 3;
    g_vu_style = 1;
    save_cfg(&g);
    g_aura_on = 0;
    g_vu_style = 0;
    load_cfg(&g);
    CHECK(g_aura_on == 3 && g_vu_style == 1);
    put(cfg, "master_volume=900\n");
    load_cfg(&g);
    CHECK(g_mvol == 100);

    /* 3. Save then load keeps the values. */
    g_aura_on = 1;
    g_mvol = 70;
    g_loop_on = 1;
    g_joy_lockout = 1;
    save_cfg(&g);
    g_aura_on = 0;
    g_mvol = 0;
    g_loop_on = 0;
    load_cfg(&g);
    CHECK(g_aura_on == 1);
    CHECK(g_mvol == 70);
    CHECK(g_loop_on == 1);
    CHECK(g_joy_off_saved == 1);

    /* 3b. SOUND output shaping: parse, clamp, round trip. */
    put(cfg, "balance=-4\noutput_profile=home_theater\ncustom_eq=1,-2,3,99,-99,0,0,0,0,12\n");
    load_cfg(&g);
    CHECK(g_bal == -4 && g_eqp == EQP_HT);
    CHECK(g_eqc[0] == 1 && g_eqc[1] == -2 && g_eqc[3] == 12 && g_eqc[4] == -12 && g_eqc[9] == 12);
    put(cfg, "balance=55\noutput_profile=nonsense\n");
    load_cfg(&g);
    CHECK(g_bal == 10 && g_eqp == EQP_FLAT);
    g_bal = 3;
    g_eqp = EQP_CUSTOM;
    g_eqc[5] = -7;
    save_cfg(&g);
    g_bal = 0;
    g_eqp = 0;
    g_eqc[5] = 0;
    load_cfg(&g);
    CHECK(g_bal == 3 && g_eqp == EQP_CUSTOM && g_eqc[5] == -7);
    {
        /* FLAT and centred leaves the samples as they were. */
        EqChain c;
        int16_t a[512], b[512];
        int k;

        for (k = 0; k < 512; k++) {
            a[k] = b[k] = (int16_t)((k * 7919) % 30000 - 15000);
        }
        eq_design(&c, 44100, EQP_FLAT, NULL, 0);
        CHECK(c.bypass == 1);
        eq_run(&c, b, 256);
        CHECK(memcmp(a, b, sizeof a) == 0);
        eq_design(&c, 44100, EQP_FLAT, NULL, 10);
        CHECK(c.bypass == 0);
        eq_run(&c, b, 256);
        CHECK(b[0] == 0 && b[1] == a[1]);
        for (k = 0; k < EQP_N; k++) {
            CHECK(eq_profile_from_key(eq_profile_key(k)) == k);
        }
    }

    /* 4. preset=original turns the port's additions off. */
    put(cfg, "loop=on\niframes=new\nsound_channels=extra\npreset=original\n");
    load_cfg(&g);
    CHECK(g_cfg_preset == 1);
    original_preset(&g);
    CHECK(g_loop_on == 0 && g_aura_on == 0 && g_sndx == 0 && g_rank_on == 0);
    CHECK(g_panels_on == 0 && g_bullet_c == 0 && g_modern == 0);
    CHECK(g_bal == 0 && g_eqp == EQP_FLAT && g_eqc[5] == 0);

    /* 5. RESTORE DEFAULTS: options back, RBRANK.DAT still there and the same. */
    restore_defaults(&g);
    CHECK(g_aura_on == 3);
    CHECK(g_scaling == 1 && g_vu_style == 0);
    CHECK(g_mvol == 100);
    CHECK(g_bal == 0 && g_eqp == EQP_FLAT);
    CHECK(fsize(rank) == (long)sizeof blob);
    fp = fopen(rank, "rb");
    CHECK(fp != NULL);
    if (fp) {
        n = fread(buf, 1, sizeof buf, fp);
        fclose(fp);
        CHECK(n == sizeof blob && memcmp(buf, blob, sizeof blob) == 0);
    }
    CHECK(fsize(cfg) > 0);

    remove(cfg);
    remove(rank);
    if (g_fail) {
        return 1;
    }
    printf("cfg_check: ok\n");
    return 0;
}
