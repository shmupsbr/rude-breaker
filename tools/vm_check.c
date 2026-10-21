#include "assets.h"
#include "vm.h"

#include <stdio.h>
#include <string.h>

static void dump(const char *tag, const RbVmInfo *info)
{
    printf("%s chunk=%u cell=%u spd=%d scroll=%d lives=%d ship=%d,%d "
           "score=%u alive=%u spawns=%u faults=%u op_miss=%u nat_miss=%u "
           "miss=%02X@%04X nat=%04X aim=%u ship_nat=%u opt=%u loads=%u rec=%u hdr=%u\n",
           tag,
           (unsigned)info->chunk, (unsigned)info->cell, (int)info->speed,
           (int)info->scroll_y, (int)info->lives,
           (int)info->ship_x, (int)info->ship_y,
           (unsigned)info->score, (unsigned)info->alive, (unsigned)info->spawns,
           (unsigned)info->step_faults, (unsigned)info->op_miss,
           (unsigned)info->nat_miss, info->miss_op, info->miss_ip,
           info->nat_miss_ip, (unsigned)info->aim_ops,
           (unsigned)info->ship_natives, (unsigned)info->option_natives,
           (unsigned)info->loads, (unsigned)info->load_record,
           (unsigned)info->load_header);
}

static void sprite_bits(RbVm *vm, int *shot, int *opt, int *form)
{
    RbVmSprite spr[256];
    int n = vm_collect_sprites(vm, spr, 256);
    int i;

    *shot = *opt = *form = 0;
    for (i = 0; i < n; i++) {
        if (spr[i].type & 0x00C0u) {
            (*shot)++;
        }
        if (spr[i].type & 0x0200u) {
            (*opt)++;
        }
        if (spr[i].type & 0x1000u) {
            (*form)++;
        }
    }
}

static void run_stage(RbVm *vm, uint16_t stage, int frames, uint16_t buttons)
{
    RbInput in;
    RbVmInfo info;
    int f;
    int prev_spd = -999;
    int cells = 0;
    uint16_t prev_cell = 0xFFFFu;
    int saw_form = 0;
    int saw_kids = 0;
    int shot = 0, opt = 0, form = 0;
    int max_shot = 0;

    memset(&in, 0, sizeof in);
    in.buttons = buttons;
    vm_reset_play(vm, stage);
    for (f = 0; f < frames; f++) {
        vm_set_input(vm, &in);
        vm_frame(vm);
        vm_info(vm, &info);
        if ((int)info.speed != prev_spd && f < 400) {
            printf("s%u f%d speed %d -> %d cell %u\n",
                   (unsigned)stage, f + 1, prev_spd, (int)info.speed,
                   (unsigned)info.cell);
            prev_spd = (int)info.speed;
        }
        if (info.cell != prev_cell) {
            cells++;
            prev_cell = info.cell;
        }
        if (!saw_form && vm_count_origin(vm, 0x29E1) > 0) {
            saw_form = f + 1;
        }
        if (!saw_kids && vm_count_origin(vm, 0x353A) > 0 &&
            vm_count_origin(vm, 0x36E0) > 0 &&
            vm_count_origin(vm, 0x31B2) > 0 &&
            vm_count_origin(vm, 0x3376) > 0 &&
            vm_count_origin(vm, 0x2FA0) > 0) {
            saw_kids = f + 1;
        }
        sprite_bits(vm, &shot, &opt, &form);
        if (shot > max_shot) {
            max_shot = shot;
        }
        if (f == 1 || f == 161 || f == frames - 1) {
            char tag[40];

            snprintf(tag, sizeof tag, "s%u f%d btn %04X",
                     (unsigned)stage, f + 1, buttons);
            dump(tag, &info);
            printf("  sprites shot=%d opt=%d form=%d "
                   "origin 6331=%d 6498=%d proxy=%d 5CF4=%d 29E1=%d "
                   "kids %d %d %d %d %d\n",
                   shot, opt, form,
                   vm_count_origin(vm, 0x6331),
                   vm_count_origin(vm, 0x6498),
                   vm_count_origin(vm, 0x5641),
                   vm_count_origin(vm, 0x5CF4),
                   vm_count_origin(vm, 0x29E1),
                   vm_count_origin(vm, 0x353A),
                   vm_count_origin(vm, 0x36E0),
                   vm_count_origin(vm, 0x31B2),
                   vm_count_origin(vm, 0x3376),
                   vm_count_origin(vm, 0x2FA0));
        }
    }
    printf("summary stage %u frames=%d cell_changes=%d form_at=%d kids_at=%d max_shot=%d\n",
           (unsigned)stage, frames, cells, saw_form, saw_kids, max_shot);
}

int main(int argc, char **argv)
{
    const char *dir = argc > 1 ? argv[1] : "assets/raw/RUDEBR";
    RbAssets assets;
    RbVm *vm = NULL;
    int rc;

    if (assets_load(&assets, dir) != 0) {
        fprintf(stderr, "assets load failed: %s\n", dir);
        return 1;
    }
    if (vm_create(&vm, &assets) != 0) {
        fprintf(stderr, "vm_create failed\n");
        assets_free(&assets);
        return 1;
    }
    rc = vm_self_test(vm);
    printf("self_test=%d\n", rc);
    if (rc != 0) {
        vm_destroy(vm);
        assets_free(&assets);
        return rc;
    }
    run_stage(vm, 1, 2500, 0);
    /* Fire keeps the ship alive until the timeline spawns 0x29E1.
     * An idle ship takes the death script and loads the clear first. */
    {
        RbInput in;
        RbVmInfo info;
        int f;
        int form_at = 0;
        int kids_at = 0;
        int clear_at = 0;

        memset(&in, 0, sizeof in);
        in.buttons = RB_BTN_A;
        vm_reset_play(vm, 1);
        for (f = 0; f < 1800; f++) {
            vm_set_input(vm, &in);
            vm_frame(vm);
            if (!form_at && vm_count_origin(vm, 0x29E1) > 0) {
                form_at = f + 1;
            }
            if (!kids_at && vm_count_origin(vm, 0x353A) > 0 &&
                vm_count_origin(vm, 0x36E0) > 0 &&
                vm_count_origin(vm, 0x31B2) > 0 &&
                vm_count_origin(vm, 0x3376) > 0 &&
                vm_count_origin(vm, 0x2FA0) > 0) {
                kids_at = f + 1;
            }
            vm_info(vm, &info);
            if (info.chunk != 2) {
                clear_at = f + 1;
                break;
            }
        }
        printf("fire-hold form_at=%d kids_at=%d clear_at=%d chunk=%u score=%u\n",
               form_at, kids_at, clear_at, (unsigned)info.chunk,
               (unsigned)info.score);
        if (!(form_at > 0 && kids_at > 0 && clear_at > kids_at)) {
            fprintf(stderr, "stage 1 formation did not spawn before the clear\n");
            vm_destroy(vm);
            assets_free(&assets);
            return 1;
        }
    }
    {
        RbInput in;
        RbVmInfo info;
        int f;
        uint32_t loads0;

        vm_info(vm, &info);
        loads0 = info.loads;
        memset(&in, 0, sizeof in);
        in.buttons = RB_BTN_A;
        for (f = 0; f < 2000; f++) {
            vm_set_input(vm, &in);
            vm_frame(vm);
        }
        vm_info(vm, &info);
        printf("after clear + A: chunk=%u loads=%u (was %u) rec=%u hdr=%u lives=%d\n",
               (unsigned)info.chunk, (unsigned)info.loads, (unsigned)loads0,
               (unsigned)info.load_record, (unsigned)info.load_header,
               (int)info.lives);
    }
    {
        RbInput in;
        RbVmInfo info;
        int f;
        uint32_t opt0;

        memset(&in, 0, sizeof in);
        vm_reset_play(vm, 1);
        for (f = 0; f < 40; f++) {
            in.buttons = (f >= 8) ? RB_BTN_B : 0;
            if (f >= 12) {
                in.buttons |= RB_BTN_RIGHT;
            }
            vm_set_input(vm, &in);
            vm_frame(vm);
            if (f >= 11 && f <= 20) {
                vm_info(vm, &info);
                printf("  move f%d ship %d opt %u\n", f + 1,
                       (int)info.ship_x, (unsigned)info.option_natives);
            }
        }
        vm_info(vm, &info);
        opt0 = info.option_natives;
        printf("mode-button after 40: opt=%u ship=%d,%d lives=%d\n",
               (unsigned)opt0, (int)info.ship_x, (int)info.ship_y,
               (int)info.lives);
    }
    run_stage(vm, 3, 1200, 0);
    run_stage(vm, 5, 1200, 0);
    vm_destroy(vm);
    assets_free(&assets);
    return 0;
}
