/*
 * Ghidra 12.1.4 headless decompile.
 * Program: sgc_game.bin
 * Language: x86:LE:16:Real Mode
 * Image base: 0x100 (.COM load address)
 * Pseudo-C is a guide. Trust docs/re/MAP.md where they disagree.
 */

/* 0000:0100  com_entry */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16far com_entry(void)

{
  int *piVar1;
  int *piVar2;
  code *pcVar3;
  int *piVar4;
  byte bVar5;
  int iVar6;
  int *piVar7;
  int *piVar8;
  undefined2 unaff_CS;
  int unaff_SS;
  undefined1 uVar9;
  
  piVar4 = _DAT_0000_0081;
  DAT_0000_0104 = _DAT_0000_0081;
  piVar7 = (int *)0x73e;
  iVar6 = 3;
  piVar8 = _DAT_0000_0081;
  do {
    if (iVar6 == 0) break;
    iVar6 = iVar6 + -1;
    piVar2 = piVar8;
    piVar8 = piVar8 + 1;
    piVar1 = piVar7;
    piVar7 = piVar7 + 1;
  } while (*piVar1 == *piVar2);
  uVar9 = 0;
  if (iVar6 != 0) {
    pcVar3 = (code *)swi(0x21);
    (*pcVar3)();
    return 1;
  }
  _DAT_0000_0081[7] = unaff_SS;
  iVar6 = piVar4[0x28];
  *(undefined2 *)(iVar6 + 4) = 0x71e;
  *(undefined2 *)(iVar6 + 6) = 0x727;
  *(undefined2 *)(iVar6 + 8) = 0x732;
  init_play_state();
  pcVar3 = (code *)swi(0x21);
  iVar6 = (*pcVar3)();
  piVar4[0xc] = iVar6;
  if (!(bool)uVar9) {
    pcVar3 = (code *)swi(0x21);
    iVar6 = (*pcVar3)();
    piVar4[0xb] = iVar6;
    if (!(bool)uVar9) {
      pcVar3 = (code *)swi(0x21);
      iVar6 = (*pcVar3)();
      piVar4[0xd] = iVar6;
      if (!(bool)uVar9) {
        pcVar3 = (code *)swi(0x21);
        iVar6 = (*pcVar3)();
        piVar4[0xe] = iVar6;
        if (!(bool)uVar9) {
          pcVar3 = (code *)swi(0x21);
          iVar6 = (*pcVar3)();
          piVar4[0xf] = iVar6;
          if (!(bool)uVar9) {
            pcVar3 = (code *)swi(0x21);
            iVar6 = (*pcVar3)();
            piVar4[0x10] = iVar6;
            if (!(bool)uVar9) {
              pcVar3 = (code *)swi(0x21);
              iVar6 = (*pcVar3)();
              piVar4[0x11] = iVar6;
              if (!(bool)uVar9) {
                pcVar3 = (code *)swi(0x21);
                iVar6 = (*pcVar3)();
                piVar4[0x12] = iVar6;
                if (!(bool)uVar9) {
                  pcVar3 = (code *)swi(0x21);
                  iVar6 = (*pcVar3)();
                  piVar4[0x13] = iVar6;
                  if (!(bool)uVar9) {
                    out(0xa6,*(byte *)(piVar4 + 0x5d));
                    bVar5 = *(byte *)(piVar4 + 0x5d) ^ 1;
                    *(byte *)(piVar4 + 0x5d) = bVar5;
                    out(0xa4,bVar5);
                    rng_seed();
                    run_game();
                    goto LAB_0000_0840;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  pcVar3 = (code *)swi(0x15);
  (*pcVar3)();
LAB_0000_0840:
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  return 0;
}



/* 0000:0764  game_entry */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16far game_entry(void)

{
  int *piVar1;
  int *piVar2;
  code *pcVar3;
  int *piVar4;
  byte bVar5;
  int iVar6;
  int *piVar7;
  int *piVar8;
  undefined2 unaff_CS;
  int unaff_SS;
  undefined1 uVar9;
  
  piVar4 = _DAT_0000_0081;
  DAT_0000_0104 = _DAT_0000_0081;
  piVar7 = (int *)0x73e;
  iVar6 = 3;
  piVar8 = _DAT_0000_0081;
  do {
    if (iVar6 == 0) break;
    iVar6 = iVar6 + -1;
    piVar2 = piVar8;
    piVar8 = piVar8 + 1;
    piVar1 = piVar7;
    piVar7 = piVar7 + 1;
  } while (*piVar1 == *piVar2);
  uVar9 = 0;
  if (iVar6 != 0) {
    pcVar3 = (code *)swi(0x21);
    (*pcVar3)();
    return 1;
  }
  _DAT_0000_0081[7] = unaff_SS;
  iVar6 = piVar4[0x28];
  *(undefined2 *)(iVar6 + 4) = 0x71e;
  *(undefined2 *)(iVar6 + 6) = 0x727;
  *(undefined2 *)(iVar6 + 8) = 0x732;
  init_play_state();
  pcVar3 = (code *)swi(0x21);
  iVar6 = (*pcVar3)();
  piVar4[0xc] = iVar6;
  if (!(bool)uVar9) {
    pcVar3 = (code *)swi(0x21);
    iVar6 = (*pcVar3)();
    piVar4[0xb] = iVar6;
    if (!(bool)uVar9) {
      pcVar3 = (code *)swi(0x21);
      iVar6 = (*pcVar3)();
      piVar4[0xd] = iVar6;
      if (!(bool)uVar9) {
        pcVar3 = (code *)swi(0x21);
        iVar6 = (*pcVar3)();
        piVar4[0xe] = iVar6;
        if (!(bool)uVar9) {
          pcVar3 = (code *)swi(0x21);
          iVar6 = (*pcVar3)();
          piVar4[0xf] = iVar6;
          if (!(bool)uVar9) {
            pcVar3 = (code *)swi(0x21);
            iVar6 = (*pcVar3)();
            piVar4[0x10] = iVar6;
            if (!(bool)uVar9) {
              pcVar3 = (code *)swi(0x21);
              iVar6 = (*pcVar3)();
              piVar4[0x11] = iVar6;
              if (!(bool)uVar9) {
                pcVar3 = (code *)swi(0x21);
                iVar6 = (*pcVar3)();
                piVar4[0x12] = iVar6;
                if (!(bool)uVar9) {
                  pcVar3 = (code *)swi(0x21);
                  iVar6 = (*pcVar3)();
                  piVar4[0x13] = iVar6;
                  if (!(bool)uVar9) {
                    out(0xa6,*(byte *)(piVar4 + 0x5d));
                    bVar5 = *(byte *)(piVar4 + 0x5d) ^ 1;
                    *(byte *)(piVar4 + 0x5d) = bVar5;
                    out(0xa4,bVar5);
                    rng_seed();
                    run_game();
                    goto LAB_0000_0840;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  pcVar3 = (code *)swi(0x15);
  (*pcVar3)();
LAB_0000_0840:
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  return 0;
}



/* 0000:088b  run_game */

void run_game(void)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 uVar3;
  int unaff_BP;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  load_rank_state();
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x40);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  init_play_state();
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x32);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  uVar1 = *(undefined2 *)(unaff_BP + 0x14);
  iVar2 = *(int *)(unaff_BP + 0x52);
  entity_zero();
  entity_alloc();
  uVar3 = *(undefined2 *)(unaff_BP + 0x18);
  *(undefined2 *)(iVar2 + 10) = uVar3;
  *(undefined2 *)(iVar2 + 8) = *(undefined2 *)0x0;
  frame_loop();
  return;
}



/* 0000:08e6  FUN_0000_08e6 */

void FUN_0000_08e6(void)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 uVar3;
  byte bVar4;
  int unaff_BP;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  init_play_state();
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x32);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  uVar1 = *(undefined2 *)(unaff_BP + 0x14);
  iVar2 = *(int *)(unaff_BP + 0x52);
  entity_zero();
  entity_alloc();
  uVar3 = *(undefined2 *)(unaff_BP + 0x18);
  *(undefined2 *)(iVar2 + 10) = uVar3;
  *(undefined2 *)(iVar2 + 8) = *(undefined2 *)0x2;
  do {
    while( true ) {
      if ((*(uint *)(unaff_BP + 0x6e) & 0x8000) != 0) {
        quit_path();
        return;
      }
      *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x40);
      (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
      apply_debug_keys();
      pause_if_flag();
      debug_hud();
      entity_dispatch();
      out(0x6c,0);
      if (*(char *)(unaff_BP + 0xc0) != '\0') break;
LAB_0000_09ce:
      if ((*(uint *)(unaff_BP + 0x72) & 0x4000) == 0) {
        *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x3c);
        (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
        bVar4 = in(0x60);
        while ((bVar4 & 0x20) != 0) {
          bVar4 = in(0x60);
        }
        if ((*(uint *)(unaff_BP + 0x72) & 0x1000) == 0) {
          bVar4 = in(10);
          out(10,bVar4 | 0x10);
        }
        do {
          bVar4 = in(0x60);
        } while ((bVar4 & 0x20) == 0);
      }
      crtc_set_scroll();
      out(0xa6,*(byte *)(unaff_BP + 0xba));
      bVar4 = *(byte *)(unaff_BP + 0xba) ^ 1;
      *(byte *)(unaff_BP + 0xba) = bVar4;
      out(0xa4,bVar4);
      if ((*(uint *)(unaff_BP + 0x72) & 0x1000) == 0) {
        bVar4 = in(10);
        out(10,bVar4 & 0xef);
      }
      *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x34);
      (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
      if ((*(uint *)(unaff_BP + 0x72) & 0x2000) != 0) {
        out(0x6c,0x10);
      }
    }
    if (*(char *)(unaff_BP + 0xc1) == '\0') {
      *(undefined1 *)(unaff_BP + 0xc1) = *(undefined1 *)(unaff_BP + 0xc0);
      goto LAB_0000_09ce;
    }
    if ((*(uint *)(unaff_BP + 0x72) & 0x4000) == 0) {
      *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x3c);
      (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
      bVar4 = in(0x60);
      while ((bVar4 & 0x20) != 0) {
        bVar4 = in(0x60);
      }
      if ((*(uint *)(unaff_BP + 0x72) & 0x1000) == 0) {
        bVar4 = in(10);
        out(10,bVar4 | 0x10);
      }
      do {
        bVar4 = in(0x60);
      } while ((bVar4 & 0x20) == 0);
    }
    if ((*(uint *)(unaff_BP + 0x72) & 0x1000) == 0) {
      bVar4 = in(10);
      out(10,bVar4 & 0xef);
    }
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x34);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
    *(char *)(unaff_BP + 0xc1) = *(char *)(unaff_BP + 0xc1) + -1;
  } while( true );
}



/* 0000:0933  frame_loop */

void frame_loop(void)

{
  byte bVar1;
  int unaff_BP;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  do {
    while( true ) {
      if ((*(uint *)(unaff_BP + 0x6e) & 0x8000) != 0) {
        quit_path();
        return;
      }
      *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x40);
      (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
      apply_debug_keys();
      pause_if_flag();
      debug_hud();
      entity_dispatch();
      out(0x6c,0);
      if (*(char *)(unaff_BP + 0xc0) != '\0') break;
LAB_0000_09ce:
      if ((*(uint *)(unaff_BP + 0x72) & 0x4000) == 0) {
        *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x3c);
        (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
        bVar1 = in(0x60);
        while ((bVar1 & 0x20) != 0) {
          bVar1 = in(0x60);
        }
        if ((*(uint *)(unaff_BP + 0x72) & 0x1000) == 0) {
          bVar1 = in(10);
          out(10,bVar1 | 0x10);
        }
        do {
          bVar1 = in(0x60);
        } while ((bVar1 & 0x20) == 0);
      }
      crtc_set_scroll();
      out(0xa6,*(byte *)(unaff_BP + 0xba));
      bVar1 = *(byte *)(unaff_BP + 0xba) ^ 1;
      *(byte *)(unaff_BP + 0xba) = bVar1;
      out(0xa4,bVar1);
      if ((*(uint *)(unaff_BP + 0x72) & 0x1000) == 0) {
        bVar1 = in(10);
        out(10,bVar1 & 0xef);
      }
      *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x34);
      (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
      if ((*(uint *)(unaff_BP + 0x72) & 0x2000) != 0) {
        out(0x6c,0x10);
      }
    }
    if (*(char *)(unaff_BP + 0xc1) == '\0') {
      *(undefined1 *)(unaff_BP + 0xc1) = *(undefined1 *)(unaff_BP + 0xc0);
      goto LAB_0000_09ce;
    }
    if ((*(uint *)(unaff_BP + 0x72) & 0x4000) == 0) {
      *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x3c);
      (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
      bVar1 = in(0x60);
      while ((bVar1 & 0x20) != 0) {
        bVar1 = in(0x60);
      }
      if ((*(uint *)(unaff_BP + 0x72) & 0x1000) == 0) {
        bVar1 = in(10);
        out(10,bVar1 | 0x10);
      }
      do {
        bVar1 = in(0x60);
      } while ((bVar1 & 0x20) == 0);
    }
    if ((*(uint *)(unaff_BP + 0x72) & 0x1000) == 0) {
      bVar1 = in(10);
      out(10,bVar1 & 0xef);
    }
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x34);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
    *(char *)(unaff_BP + 0xc1) = *(char *)(unaff_BP + 0xc1) + -1;
  } while( true );
}



/* 0000:0a36  quit_path */

void __cdecl16near quit_path(void)

{
  undefined2 *puVar1;
  code *pcVar2;
  byte bVar3;
  int iVar4;
  int unaff_BP;
  undefined2 *puVar5;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  draw_text();
  out(0x74,0x10);
  *(uint *)(unaff_BP + 0x72) = *(uint *)(unaff_BP + 0x72) & 0x7fff;
  puVar5 = (undefined2 *)*(undefined2 *)(unaff_BP + 0x52);
  entity_zero();
  if (*(char *)(unaff_BP + 0xbc) == '\0') {
    FUN_0000_08e6();
    return;
  }
  out(0x6c,0);
  if ((*(uint *)(unaff_BP + 0x72) & 0x1000) == 0) {
    bVar3 = in(10);
    out(10,bVar3 & 0xef);
  }
  ranking_exit();
  if ((*(byte *)(unaff_BP + 0xb9) & 0x80) == 0) {
    return;
  }
  pcVar2 = (code *)swi(0x18);
  (*pcVar2)();
  out(0xa8,0);
  out(0xaa,0);
  out(0xac,0);
  out(0xae,0);
  out(0x7c,0xc0);
  out(0x7e,0);
  out(0x7e,0);
  out(0x7e,0);
  out(0x7e,0);
  out(0xa4,1);
  out(0xa6,1);
  for (iVar4 = 16000; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    *puVar1 = 0xffff;
  }
  out(0xa4,0);
  out(0xa6,0);
  for (iVar4 = 16000; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    *puVar1 = 0xffff;
  }
  out(0x7c,0);
  draw_text();
  draw_text();
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x3e);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  pcVar2 = (code *)swi(0x18);
  (*pcVar2)();
  FUN_0000_08e6();
  return;
}



/* 0000:0bd9  pause_if_flag */

void __cdecl16near pause_if_flag(void)

{
  undefined2 *puVar1;
  undefined **ppuVar2;
  undefined2 *puVar3;
  int iVar4;
  int unaff_BP;
  undefined2 *puVar5;
  undefined **ppuVar6;
  undefined2 *puVar7;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  if ((*(uint *)(unaff_BP + 0x72) & 0x8000) == 0) {
    return;
  }
  puVar5 = (undefined2 *)0xe74;
  puVar7 = (undefined2 *)0xca8;
  for (iVar4 = 10; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar3 = puVar7;
    puVar7 = puVar7 + 1;
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    *puVar3 = *puVar1;
  }
  ppuVar6 = (undefined **)&PTR_vm_invalid_0000_2e74;
  puVar5 = (undefined2 *)0xcbc;
  for (iVar4 = 10; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    ppuVar2 = ppuVar6;
    ppuVar6 = ppuVar6 + 1;
    *puVar1 = *ppuVar2;
  }
  while( true ) {
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x3c);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(unaff_CS);
    if ((*(uint *)(unaff_BP + 0x6e) & 0x8000) != 0) break;
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x40);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(unaff_CS);
    apply_debug_keys();
    debug_hud();
    if ((*(uint *)(unaff_BP + 0x72) & 0x8000) == 0) break;
    DAT_0000_0ca6 = DAT_0000_0ca6 + 1;
    unaff_CS = 0;
    if ((DAT_0000_0ca6 & 1) == 0) {
      draw_text();
    }
    else {
      draw_text();
    }
    draw_text();
  }
  puVar5 = (undefined2 *)0xca8;
  puVar7 = (undefined2 *)0xe74;
  for (iVar4 = 10; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar3 = puVar7;
    puVar7 = puVar7 + 1;
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    *puVar3 = *puVar1;
  }
  puVar5 = (undefined2 *)0xcbc;
  ppuVar6 = (undefined **)&PTR_vm_invalid_0000_2e74;
  for (iVar4 = 10; iVar4 != 0; iVar4 = iVar4 + -1) {
    ppuVar2 = ppuVar6;
    ppuVar6 = ppuVar6 + 1;
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    *ppuVar2 = (undefined *)*puVar1;
  }
  if ((*(uint *)(unaff_BP + 0x6e) & 0x8000) == 0) {
    do {
      *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x3c);
      (*(code *)*(undefined2 *)(unaff_BP + 0xc))(unaff_CS);
      if ((*(uint *)(unaff_BP + 0x6e) & 0x8000) != 0) {
        return;
      }
      *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x40);
      (*(code *)*(undefined2 *)(unaff_BP + 0xc))(unaff_CS);
      apply_debug_keys();
      debug_hud();
    } while ((*(uint *)(unaff_BP + 0x6c) & 0x80) != 0);
  }
  return;
}



/* 0000:0ce6  apply_debug_keys */

void __cdecl16near apply_debug_keys(void)

{
  uint uVar1;
  byte bVar2;
  uint uVar3;
  int unaff_BP;
  undefined2 unaff_SS;
  
  if ((*(uint *)(unaff_BP + 0x72) & 0x8000) != 0) {
    FUN_0000_0d62();
  }
  uVar3 = *(uint *)(unaff_BP + 0x72);
  uVar1 = *(uint *)(unaff_BP + 0x6e);
  if ((uVar1 & 0x80) != 0) {
    uVar3 = uVar3 ^ 0x8000;
  }
  if ((uVar3 & 0x800) != 0) {
    if ((uVar1 & 0x1000) != 0) {
      uVar3 = uVar3 ^ 0x100;
      *(undefined1 *)(unaff_BP + 0xc4) = 1;
    }
    if ((uVar1 & 0x800) != 0) {
      uVar3 = uVar3 ^ 0x2000;
    }
    if ((uVar1 & 0x100) != 0) {
      uVar3 = uVar3 ^ 0x1000;
    }
    uVar3 = uVar3 & 0xbfff;
    if ((*(uint *)(unaff_BP + 0x6c) & 0x2000) != 0) {
      uVar3 = uVar3 | 0x4000;
    }
  }
  *(uint *)(unaff_BP + 0x72) = uVar3;
  if ((uVar3 & 0x800) != 0) {
    uVar3 = *(uint *)(unaff_BP + 0x6e);
    if ((uVar3 & 0x400) != 0) {
      *(undefined1 *)(unaff_BP + 0xcc) = 1;
    }
    if ((uVar3 & 0x200) != 0) {
      bVar2 = *(char *)(unaff_BP + 0xc0) + 1U & 3;
      *(byte *)(unaff_BP + 0xc0) = bVar2;
      *(byte *)(unaff_BP + 0xc1) = bVar2;
    }
  }
  return;
}



/* 0000:0d62  FUN_0000_0d62 */

void __cdecl16near FUN_0000_0d62(void)

{
  byte bVar1;
  byte bVar2;
  int unaff_BP;
  undefined2 unaff_SS;
  
  if ((*(uint *)(unaff_BP + 0x72) & 0x800) == 0) {
    bVar1 = *(byte *)(unaff_BP + 0xc3);
    bVar2 = *(byte *)(unaff_BP + 0xc2);
    if ((bVar1 & 0x10) == 0) {
      if ((bVar1 & 8) == 0) {
        if ((bVar1 & 4) == 0) {
          if ((bVar1 & 2) == 0) {
            if ((bVar1 & 1) == 0) {
              if ((bVar2 & 8) != 0) {
                *(byte *)(unaff_BP + 0xc3) = *(byte *)(unaff_BP + 0xc3) | 1;
                return;
              }
            }
            else if ((bVar2 & 2) != 0) {
              *(byte *)(unaff_BP + 0xc3) = *(byte *)(unaff_BP + 0xc3) | 2;
              return;
            }
          }
          else if ((bVar2 & 4) != 0) {
            *(byte *)(unaff_BP + 0xc3) = *(byte *)(unaff_BP + 0xc3) | 4;
            return;
          }
        }
        else if ((bVar2 & 0x10) != 0) {
          *(byte *)(unaff_BP + 0xc3) = *(byte *)(unaff_BP + 0xc3) | 8;
          return;
        }
      }
      else if ((bVar2 & 1) != 0) {
        *(byte *)(unaff_BP + 0xc3) = *(byte *)(unaff_BP + 0xc3) | 0x10;
        return;
      }
    }
    else if ((bVar2 & 0x10) != 0) {
      *(undefined1 *)(unaff_BP + 0xc3) = 0;
      *(uint *)(unaff_BP + 0x72) = *(uint *)(unaff_BP + 0x72) | 0x800;
      return;
    }
    if (bVar2 != 0) {
      *(undefined1 *)(unaff_BP + 0xc3) = 0;
    }
    return;
  }
  if (((*(uint *)(unaff_BP + 0x6e) & 0x3f00) == 0) && (*(char *)(unaff_BP + 0xc2) == '\0')) {
    return;
  }
  *(uint *)(unaff_BP + 0x72) = *(uint *)(unaff_BP + 0x72) & 0xf7ff;
  return;
}



/* 0000:0dec  init_play_state */

void __cdecl16near init_play_state(void)

{
  int unaff_BP;
  undefined2 unaff_SS;
  
  *(undefined2 *)(unaff_BP + 0x6e) = 0;
  *(undefined2 *)(unaff_BP + 0x74) = 0;
  *(undefined2 *)(unaff_BP + 0x76) = 0;
  *(undefined2 *)(unaff_BP + 0x7a) = 0;
  *(undefined2 *)(unaff_BP + 0x7c) = 0;
  *(undefined2 *)(unaff_BP + 0x7e) = 0x1e0;
  *(undefined2 *)(unaff_BP + 0x82) = 2;
  *(undefined1 *)(unaff_BP + 0xb7) = 0;
  *(undefined1 *)(unaff_BP + 0xbb) = 0xff;
  *(undefined1 *)(unaff_BP + 0xbc) = 1;
  return;
}



/* 0000:0e20  rng_seed */

void __cdecl16near rng_seed(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(0x1c);
  (*pcVar1)();
  DAT_0000_0e7e = DAT_0000_0e48 + DAT_0000_0e4a + DAT_0000_0e4c;
  DAT_0000_0e7e = rng_mix();
  return;
}



/* 0000:0e4e  rng_mix */

void __cdecl16near rng_mix(void)

{
  undefined2 uVar1;
  
  uVar1 = FUN_0000_0e5d();
  FUN_0000_0e5d(uVar1);
  return;
}



/* 0000:0e5d  FUN_0000_0e5d */

undefined2 __cdecl16near FUN_0000_0e5d(void)

{
  uint uVar1;
  char cVar2;
  uint uVar3;
  int iVar4;
  int unaff_BP;
  undefined2 unaff_SS;
  
  uVar1 = *(uint *)(unaff_BP + 0x5c);
  uVar3 = DAT_0000_0e7e + *(int *)(unaff_BP + 100);
  iVar4 = uVar3 + uVar1;
  iVar4 = ((int)(CONCAT12(CARRY2(uVar3,uVar1),CONCAT11((char)uVar1,(char)iVar4)) >> 1) -
          (CONCAT11((char)(uVar1 >> 8),(char)((uint)iVar4 >> 8)) << 1 | (uint)((int)uVar1 < 0))) -
          (uint)((int)uVar1 < 0);
  DAT_0000_0e7e = DAT_0000_0e7e + iVar4;
  cVar2 = (char)((uint)iVar4 >> 8);
  return CONCAT11(cVar2,(char)iVar4 + cVar2);
}



/* 0000:0e80  sincos */

void __cdecl16near sincos(double __x,double *__sinx,double *__cosx)

{
  byte in_BL;
  
  if ((in_BL & 0xc0) == 0) {
    return;
  }
  if ((in_BL & 0x80) != 0) {
    if ((in_BL & 0x40) != 0) {
      return;
    }
    return;
  }
  return;
}



/* 0000:0ec6  aim_helper */

int __cdecl16near aim_helper(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int in_CX;
  int iVar4;
  int in_DX;
  uint *puVar5;
  int unaff_SI;
  undefined2 unaff_DS;
  
  iVar1 = *(int *)(unaff_SI + 0x24);
  iVar2 = *(int *)(unaff_SI + 0x22);
  if (in_DX <= iVar1) {
    if (iVar2 <= in_CX) {
      iVar3 = -0x4100;
      puVar5 = (uint *)&DAT_0000_18c8;
      iVar4 = 0x40;
      do {
        if ((uint)(CONCAT22((uint)(in_CX - iVar2) >> 8,(in_CX - iVar2) * 0x100) /
                  (ulong)(uint)(iVar1 - in_DX)) < *puVar5) {
          return iVar3;
        }
        puVar5 = puVar5 + 1;
        iVar3 = iVar3 + 0x100;
        iVar4 = iVar4 + -1;
      } while (iVar4 != 0);
      return iVar3;
    }
    iVar3 = -0x4000;
    puVar5 = (uint *)&DAT_0000_18c8;
    iVar4 = 0x40;
    do {
      if ((uint)(CONCAT22((uint)(iVar2 - in_CX) >> 8,(iVar2 - in_CX) * 0x100) /
                (ulong)(uint)(iVar1 - in_DX)) < *puVar5) {
        return iVar3;
      }
      puVar5 = puVar5 + 1;
      iVar3 = iVar3 + -0x100;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    return iVar3;
  }
  if (iVar2 < in_CX) {
    iVar3 = 0x4000;
    puVar5 = (uint *)&DAT_0000_18c8;
    iVar4 = 0x40;
    do {
      if ((uint)(CONCAT22((uint)(in_CX - iVar2) >> 8,(in_CX - iVar2) * 0x100) /
                (ulong)(uint)(in_DX - iVar1)) < *puVar5) {
        return iVar3;
      }
      puVar5 = puVar5 + 1;
      iVar3 = iVar3 + -0x100;
      iVar4 = iVar4 + -1;
    } while (iVar4 != 0);
    return iVar3;
  }
  iVar3 = 0x3f00;
  puVar5 = (uint *)&DAT_0000_18c8;
  iVar4 = 0x40;
  do {
    if ((uint)(CONCAT22((uint)(iVar2 - in_CX) >> 8,(iVar2 - in_CX) * 0x100) /
              (ulong)(uint)(in_DX - iVar1)) < *puVar5) {
      return iVar3;
    }
    puVar5 = puVar5 + 1;
    iVar3 = iVar3 + 0x100;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  return iVar3;
}



/* 0000:0f88  aim_helper_2 */

int __cdecl16near aim_helper_2(void)

{
  uint uVar1;
  int unaff_SI;
  undefined2 unaff_DS;
  
  uVar1 = aim_helper();
  uVar1 = ((uVar1 >> 8) + 0x100) - (*(uint *)(unaff_SI + 0x2e) >> 8) & 0xff;
  if (uVar1 < 0x80) {
    return *(int *)(unaff_SI + 0x2e) + *(int *)(unaff_SI + 0x30);
  }
  if (uVar1 < 0x81) {
    return 0;
  }
  return *(int *)(unaff_SI + 0x2e) - *(int *)(unaff_SI + 0x30);
}



/* 0000:0fb9  crtc_set_scroll */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16near crtc_set_scroll(void)

{
  byte bVar1;
  int iVar2;
  int iVar3;
  uint uVar4;
  byte bVar5;
  int unaff_BP;
  undefined2 unaff_SS;
  
  _DAT_0000_1088 = 0;
  DAT_0000_108c = 400;
  iVar2 = *(int *)(unaff_BP + 0x74) - *(int *)(unaff_BP + 0x84);
  DAT_0000_108a = iVar2 + 1;
  if (DAT_0000_108a < 0) {
    DAT_0000_108a = iVar2 + 0x191;
  }
  if (399 < DAT_0000_108a) {
    DAT_0000_108a = DAT_0000_108a + -400;
  }
  iVar2 = *(int *)(unaff_BP + 0x76) + *(int *)(unaff_BP + 0x84);
  iVar3 = iVar2 + -1;
  if (iVar3 < 0) {
    iVar3 = iVar2 + 399;
  }
  if (399 < iVar3) {
    iVar3 = iVar3 + -400;
  }
  DAT_0000_1086 = iVar3 * 0x28;
  *(undefined2 *)(unaff_BP + 0x84) = *(undefined2 *)(unaff_BP + 0x82);
  out(0xa2,0x70);
  out(0xa0,(undefined1)DAT_0000_1086);
  out(0xa2,0x71);
  out(0xa0,DAT_0000_1086._1_1_);
  out(0xa2,0x72);
  out(0xa0,(char)(DAT_0000_108a << 4));
  out(0xa2,0x73);
  uVar4 = CONCAT11(*(undefined1 *)(unaff_BP + 0xb9),(char)((uint)(DAT_0000_108a << 4) >> 8)) &
          0x40ff;
  out(0xa0,(byte)uVar4 | (byte)(uVar4 >> 8));
  out(0xa2,0x74);
  out(0xa0,DAT_0000_1088);
  out(0xa2,0x75);
  out(0xa0,DAT_0000_1089);
  out(0xa2,0x76);
  out(0xa0,(char)(DAT_0000_108c << 4));
  out(0xa2,0x77);
  uVar4 = CONCAT11(*(undefined1 *)(unaff_BP + 0xb9),(char)((uint)(DAT_0000_108c << 4) >> 8)) &
          0x40ff;
  bVar5 = (byte)(uVar4 >> 8);
  bVar1 = (byte)uVar4 | bVar5;
  out(0xa0,bVar1);
  return CONCAT11(bVar5,bVar1);
}



/* 0000:108e  scroll_row_update */

void __cdecl16near scroll_row_update(void)

{
  uint *puVar1;
  undefined2 uVar2;
  uint uVar3;
  uint *puVar4;
  int iVar5;
  int unaff_BP;
  uint *puVar6;
  undefined2 unaff_SS;
  
  uVar2 = *(undefined2 *)(unaff_BP + 0x16);
  uVar3 = *(uint *)(unaff_BP + 0x76);
  if (399 < uVar3) {
    uVar3 = uVar3 - 400;
  }
  puVar4 = (uint *)((uVar3 >> 4) * 0x3c + 0x106);
  puVar6 = (uint *)((*(uint *)(unaff_BP + 0x7a) >> 4) * 0x3c);
  iVar5 = 0x1e;
  do {
    puVar1 = puVar6;
    puVar6 = puVar6 + 1;
    *puVar4 = *puVar1 | 0x8000;
    puVar4 = puVar4 + 1;
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
  return;
}



/* 0000:10d2  compose_sprites */

void __cdecl16near compose_sprites(void)

{
  int iVar1;
  int iVar2;
  uint *puVar3;
  
  puVar3 = (uint *)&DAT_0000_0106;
  iVar2 = 0x19;
  do {
    iVar1 = 0x1e;
    do {
      if ((*puVar3 & 0xc000) != 0) {
        FUN_0000_4b21();
        *puVar3 = (*puVar3 & 0x8000) >> 1 | *puVar3 & 0x3fff;
      }
      puVar3 = puVar3 + 1;
      iVar1 = iVar1 + -1;
    } while (iVar1 != 0);
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  return;
}



/* 0000:1132  draw_entity */

void __cdecl16near draw_entity(void)

{
  int iVar1;
  char cVar2;
  char cVar3;
  int iVar4;
  uint uVar5;
  byte bVar7;
  uint uVar6;
  undefined1 uVar8;
  char cVar9;
  int iVar10;
  char cVar11;
  uint *puVar12;
  int unaff_BP;
  int unaff_SI;
  uint uVar13;
  uint uVar14;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  iVar4 = *(int *)(unaff_SI + 0x22) + (uint)*(byte *)(unaff_SI + 0x4c) * -8;
  iVar10 = *(int *)(unaff_SI + 0x24) + (uint)*(byte *)(unaff_SI + 0x4d) * -8;
  iVar1 = iVar10 + *(int *)(unaff_BP + 0x76);
  if (399 < iVar1) {
    iVar1 = iVar1 + -400;
  }
  cVar11 = (char)(iVar10 >> 4);
  uVar8 = (undefined1)(iVar4 >> 4);
  DAT_0000_12c3 = (byte)iVar4 & *(byte *)(unaff_SI + 0x4e);
  DAT_0000_12c4 = *(undefined1 *)(unaff_SI + 0x4f);
  cVar3 = (char)(iVar1 >> 4);
  uVar13 = (iVar4 >> 3) + iVar1 * 0x50;
  iVar1 = *(int *)(unaff_SI + 0x38);
  for (cVar9 = *(char *)(unaff_SI + 0x4d); cVar9 != '\0'; cVar9 = cVar9 + -1) {
    uVar5 = CONCAT11(uVar8,cVar3);
    if (cVar3 < '\0') {
      uVar5 = CONCAT11(uVar8,cVar3 + '\x19');
    }
    else if ('\x18' < cVar3) {
      uVar5 = CONCAT11(uVar8,cVar3 + -0x19);
    }
    if (cVar11 < '\0') {
      iVar1 = iVar1 + (uint)*(byte *)(unaff_SI + 0x4c);
    }
    else if (cVar11 < '\x18') {
      uVar6 = uVar5;
      uVar14 = uVar13;
      for (cVar3 = *(char *)(unaff_SI + 0x4c); cVar3 != '\0'; cVar3 = cVar3 + -1) {
        bVar7 = (byte)(uVar6 >> 8);
        if (('\x03' < (char)bVar7) && (bVar7 < 0x23)) {
          if (31999 < uVar14) {
            uVar14 = uVar14 + 0x8300;
          }
          FUN_0000_481b(uVar6,unaff_ES,unaff_DS,unaff_SI,iVar1);
          cVar2 = (char)uVar6;
          bVar7 = (byte)(uVar6 >> 8);
          puVar12 = (uint *)((uVar6 & 0xff) * 0x3c + 0xfc + (uint)bVar7 * 2);
          if ((char)bVar7 < '\x05') {
            if (cVar3 == '\x01') {
              puVar12[1] = puVar12[1] | 0x8000;
              if ('\x17' < cVar2) {
                puVar12 = puVar12 + -0x2ee;
              }
              puVar12[0x1f] = puVar12[0x1f] | 0x8000;
            }
            else {
              puVar12[0x1f] = puVar12[0x1f] | 0x8000;
              if ('\x17' < cVar2) {
                puVar12 = puVar12 + -0x2ee;
              }
              puVar12[0x3d] = puVar12[0x3d] | 0x8000;
            }
          }
          else if (cVar3 == '\x01') {
            *puVar12 = *puVar12 | 0x8000;
            puVar12[1] = puVar12[1] | 0x8000;
            if ('\x17' < cVar2) {
              puVar12 = puVar12 + -0x2ee;
            }
            puVar12[0x1e] = puVar12[0x1e] | 0x8000;
            puVar12[0x1f] = puVar12[0x1f] | 0x8000;
          }
          else {
            *puVar12 = *puVar12 | 0x8000;
            if ('\x17' < cVar2) {
              puVar12 = puVar12 + -0x2ee;
            }
            puVar12[0x1e] = puVar12[0x1e] | 0x8000;
          }
        }
        uVar14 = uVar14 + 2;
        iVar1 = iVar1 + 1;
        uVar6 = CONCAT11((char)(uVar6 >> 8) + '\x01',(char)uVar6);
      }
    }
    uVar8 = (undefined1)(uVar5 >> 8);
    cVar3 = (char)uVar5 + '\x01';
    cVar11 = cVar11 + '\x01';
    uVar13 = uVar13 + 0x500;
  }
  return;
}



/* 0000:12c5  draw_text */

void __cdecl16near draw_text(void)

{
  char cVar1;
  undefined1 uVar2;
  char cVar3;
  byte bVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  int unaff_BP;
  uint *unaff_SI;
  uint *puVar8;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  uVar7 = DAT_0000_1410;
  do {
    while( true ) {
      while( true ) {
        while( true ) {
          while( true ) {
            while( true ) {
              while( true ) {
                while( true ) {
                  while( true ) {
                    puVar8 = (uint *)((int)unaff_SI + 1);
                    bVar4 = (byte)*unaff_SI;
                    if (0xef < bVar4) break;
                    uVar5 = (uint)bVar4;
                    unaff_SI = (uint *)((int)unaff_SI + 3);
                    uVar6 = *puVar8;
                    do {
                      *DAT_0000_140e = uVar6 & 0xff;
                      DAT_0000_140e[0x1000] = uVar7;
                      DAT_0000_140e[1] = uVar6 >> 8;
                      DAT_0000_140e[0x1001] = uVar7;
                      DAT_0000_140e = DAT_0000_140e + 2;
                      uVar5 = uVar5 - 1;
                    } while (uVar5 != 0);
                  }
                  if (bVar4 != 0xf8) break;
                  cVar1 = (char)*puVar8;
                  uVar2 = (undefined1)(*puVar8 >> 8);
                  *DAT_0000_140e = CONCAT11(uVar2,cVar1 + -0x20);
                  DAT_0000_140e[0x1000] = uVar7;
                  DAT_0000_140e[1] = CONCAT11(uVar2,cVar1 + '`');
                  DAT_0000_140e[0x1001] = uVar7;
                  DAT_0000_140e = DAT_0000_140e + 2;
                  unaff_SI = (uint *)((int)unaff_SI + 3);
                }
                if (bVar4 != 0xf9) break;
                while( true ) {
                  cVar1 = (char)*puVar8;
                  if (cVar1 == -1) break;
                  cVar3 = (char)(*puVar8 >> 8);
                  bVar4 = cVar3 - 0x1f;
                  if (-1 < (char)bVar4) {
                    bVar4 = cVar3 + -0x41 + (bVar4 < 0x61);
                  }
                  uVar6 = CONCAT11(bVar4 + 0xa1,cVar1 * '\x02' + '\x1f' + (0x5e < bVar4)) & 0x7f7f;
                  cVar1 = (char)uVar6;
                  uVar2 = (undefined1)(uVar6 >> 8);
                  *DAT_0000_140e = CONCAT11(uVar2,cVar1 + -0x20);
                  DAT_0000_140e[0x1000] = uVar7;
                  DAT_0000_140e[1] = CONCAT11(uVar2,cVar1 + '`');
                  DAT_0000_140e[0x1001] = uVar7;
                  DAT_0000_140e = DAT_0000_140e + 2;
                  puVar8 = puVar8 + 1;
                }
                unaff_SI = (uint *)((int)puVar8 + 1);
              }
              if (bVar4 != 0xfa) break;
              DAT_0000_140e =
                   (uint *)((uint)(byte)*puVar8 * 2 + (uint)(byte)unaff_SI[1] * 0x20 +
                           (uint)(byte)unaff_SI[1] * 0x80);
              unaff_SI = (uint *)((int)unaff_SI + 3);
              DAT_0000_140c = DAT_0000_140e;
            }
            if (bVar4 != 0xfb) break;
            uVar6 = (uint)*(byte *)((uint)(byte)unaff_SI[1] + unaff_BP);
            DAT_0000_140e =
                 (uint *)((uint)*(byte *)((uint)(byte)*puVar8 + unaff_BP) * 2 + uVar6 * 0x20 +
                         uVar6 * 0x80);
            unaff_SI = (uint *)((int)unaff_SI + 3);
            DAT_0000_140c = DAT_0000_140e;
          }
          if (bVar4 != 0xfc) break;
          uVar7 = CONCAT11((char)(uVar7 >> 8),(byte)*puVar8);
          unaff_SI = unaff_SI + 1;
          DAT_0000_1410 = uVar7;
        }
        if (bVar4 != 0xfd) break;
        unaff_SI = unaff_SI + 1;
        uVar6 = (uint)(byte)*puVar8;
        do {
          DAT_0000_140e[0x1000] = uVar7;
          DAT_0000_140e[0x1001] = uVar7;
          DAT_0000_140e = DAT_0000_140e + 2;
          uVar6 = uVar6 - 1;
        } while (uVar6 != 0);
      }
      unaff_SI = puVar8;
      if (bVar4 != 0xfe) break;
      DAT_0000_140e = DAT_0000_140c + 0x50;
      DAT_0000_140c = DAT_0000_140e;
    }
  } while (bVar4 != 0xff);
  return;
}



/* 0000:1412  FUN_0000_1412 */

void __cdecl16near FUN_0000_1412(void)

{
  FUN_0000_1554();
  FUN_0000_1554();
  return;
}



/* 0000:143f  FUN_0000_143f */

void __cdecl16near FUN_0000_143f(void)

{
  FUN_0000_1554();
  FUN_0000_1554();
  FUN_0000_1554();
  FUN_0000_1554();
  FUN_0000_1554();
  return;
}



/* 0000:1494  FUN_0000_1494 */

void __cdecl16near FUN_0000_1494(void)

{
  ulong uVar1;
  uint in_AX;
  uint in_DX;
  
  if ((0x5f4 < in_DX) && ((0x5f5 < in_DX || (0xe0f6 < in_AX)))) {
    in_DX = 0x5f5;
    in_AX = 0xe0f6;
  }
  uVar1 = (ulong)in_DX % 10000 << 0x10 | (ulong)in_AX;
  DAT_0000_1550 = (undefined2)(uVar1 / 10000);
  DAT_0000_1552 = (undefined2)(uVar1 % 10000);
  DAT_0000_154c = in_AX;
  DAT_0000_154e = in_DX;
  FUN_0000_1554();
  FUN_0000_1554();
  FUN_0000_1554();
  FUN_0000_1554();
  FUN_0000_1554();
  FUN_0000_1554();
  FUN_0000_1554();
  FUN_0000_1554();
  return;
}



/* 0000:1554  FUN_0000_1554 */

uint __cdecl16near FUN_0000_1554(void)

{
  uint in_AX;
  uint uVar1;
  char in_BL;
  uint unaff_SI;
  uint *unaff_DI;
  undefined2 unaff_ES;
  
  uVar1 = in_AX;
  if ((in_BL == '\0') && (uVar1 = in_AX & 0xff, (char)in_AX == '\0')) {
    *unaff_DI = (uint)(undefined1 *)&LAB_0000_2020;
    unaff_DI[1] = (uint)(undefined1 *)&LAB_0000_2020;
    unaff_DI[0x1000] = unaff_SI;
    unaff_DI[0x1001] = unaff_SI;
  }
  else {
    uVar1 = CONCAT11((char)uVar1 + '!',0x56);
    *unaff_DI = uVar1;
    unaff_DI[1] = uVar1 | 0x80;
    unaff_DI[0x1000] = unaff_SI;
    unaff_DI[0x1001] = unaff_SI;
  }
  return in_AX;
}



/* 0000:1592  FUN_0000_1592 */

undefined4 __cdecl16near FUN_0000_1592(void)

{
  code *pcVar1;
  undefined1 extraout_AH;
  int in_CX;
  int extraout_DX;
  
  out(0x68,0xb);
  do {
    pcVar1 = (code *)swi(0x18);
    (*pcVar1)();
    in_CX = in_CX + -1;
  } while (in_CX != 0);
  out(0x68,10);
  return CONCAT22(extraout_DX + 1,CONCAT11(extraout_AH,10));
}



/* 0000:15b6  ranking_exit */

void __cdecl16near ranking_exit(void)

{
  undefined2 uVar1;
  undefined2 *puVar2;
  undefined2 *puVar3;
  int iVar4;
  int unaff_BP;
  undefined2 *puVar5;
  undefined2 *puVar6;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  
  uVar1 = *(undefined2 *)(unaff_BP + 0x24);
  puVar5 = (undefined2 *)*(undefined2 *)(unaff_BP + 0x56);
  puVar6 = (undefined2 *)0xa;
  for (iVar4 = 0x6e; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar3 = puVar6;
    puVar6 = puVar6 + 1;
    puVar2 = puVar5;
    puVar5 = puVar5 + 1;
    *puVar3 = *puVar2;
  }
  *(undefined2 *)0x0 = *(undefined2 *)(unaff_BP + 0x72);
  *(undefined2 *)0x2 = *(undefined2 *)(unaff_BP + 0xb2);
  *(undefined2 *)0x4 = *(undefined2 *)(unaff_BP + 0x62);
  *(undefined1 *)0x6 = *(undefined1 *)(unaff_BP + 199);
  *(undefined1 *)0x7 = *(undefined1 *)(unaff_BP + 0xc9);
  *(byte *)0x8 = *(byte *)(unaff_BP + 0xb9) & 4;
  *(undefined1 *)0x9 = *(undefined1 *)(unaff_BP + 0xc0);
  puVar5 = (undefined2 *)*(undefined2 *)(*(int *)(unaff_BP + 0x50) + 6);
  puVar6 = (undefined2 *)*(undefined2 *)(unaff_BP + 0x4e);
  for (iVar4 = 0x28; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar3 = puVar6;
    puVar6 = puVar6 + 1;
    puVar2 = puVar5;
    puVar5 = puVar5 + 1;
    *puVar3 = *puVar2;
  }
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x2a);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  *(undefined2 *)0x1824 = 0;
  return;
}



/* 0000:1648  load_rank_state */

void __cdecl16near load_rank_state(void)

{
  undefined2 uVar1;
  undefined2 *puVar2;
  undefined2 *puVar3;
  int iVar4;
  int unaff_BP;
  undefined2 *puVar5;
  undefined2 *puVar6;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined1 in_CF;
  
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x32);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  if ((bool)in_CF) {
    puVar5 = (undefined2 *)0x1948;
    puVar6 = (undefined2 *)*(undefined2 *)(unaff_BP + 0x56);
    for (iVar4 = 0x6e; iVar4 != 0; iVar4 = iVar4 + -1) {
      puVar3 = puVar6;
      puVar6 = puVar6 + 1;
      puVar2 = puVar5;
      puVar5 = puVar5 + 1;
      *puVar3 = *puVar2;
    }
    *(undefined2 *)(unaff_BP + 0x72) = 0;
    *(undefined2 *)(unaff_BP + 0xb2) = 2;
    *(undefined2 *)(unaff_BP + 0x62) = 2;
    *(undefined1 *)(unaff_BP + 199) = 1;
    *(undefined1 *)(unaff_BP + 0xc9) = 7;
    *(byte *)(unaff_BP + 0xb9) = *(byte *)(unaff_BP + 0xb9) & 0xfb;
    *(undefined1 *)(unaff_BP + 0xc0) = 0;
    *(undefined1 *)(unaff_BP + 0xc1) = 0;
    puVar5 = (undefined2 *)*(int *)(unaff_BP + 0x56);
    *(undefined2 *)(unaff_BP + 0x8a) = *puVar5;
    *(undefined2 *)(unaff_BP + 0x8c) = puVar5[1];
  }
  else {
    uVar1 = *(undefined2 *)(unaff_BP + 0x24);
    puVar5 = (undefined2 *)0xa;
    puVar6 = (undefined2 *)*(undefined2 *)(unaff_BP + 0x56);
    for (iVar4 = 0x6e; iVar4 != 0; iVar4 = iVar4 + -1) {
      puVar3 = puVar6;
      puVar6 = puVar6 + 1;
      puVar2 = puVar5;
      puVar5 = puVar5 + 1;
      *puVar3 = *puVar2;
    }
    *(uint *)(unaff_BP + 0x72) = *(uint *)0x0 & 0x7fff;
    *(undefined2 *)(unaff_BP + 0xb2) = *(undefined2 *)0x2;
    *(undefined2 *)(unaff_BP + 0x62) = *(undefined2 *)0x4;
    *(undefined1 *)(unaff_BP + 199) = *(undefined1 *)0x6;
    *(undefined1 *)(unaff_BP + 0xc9) = *(undefined1 *)0x7;
    *(byte *)(unaff_BP + 0xb9) = *(byte *)(unaff_BP + 0xb9) | *(byte *)0x8;
    *(undefined1 *)(unaff_BP + 0xc0) = *(undefined1 *)0x9;
    *(undefined1 *)(unaff_BP + 0xc1) = 0;
    puVar5 = (undefined2 *)*(int *)(unaff_BP + 0x56);
    *(undefined2 *)(unaff_BP + 0x8a) = *puVar5;
    *(undefined2 *)(unaff_BP + 0x8c) = puVar5[1];
  }
  return;
}



/* 0000:1717  debug_hud */

void __cdecl16near debug_hud(void)

{
  int unaff_BP;
  undefined2 unaff_SS;
  
  if ((*(uint *)(unaff_BP + 0x72) & 0x2000) != 0) {
    FUN_0000_43c0();
    FUN_0000_43c0();
    FUN_0000_43c0();
    FUN_0000_43c0();
    FUN_0000_43c0();
    FUN_0000_43c0();
    FUN_0000_4380();
    FUN_0000_4380();
    FUN_0000_4380();
    FUN_0000_43c0();
    FUN_0000_1826();
    FUN_0000_43c0();
    FUN_0000_43c0();
    FUN_0000_43c0();
    FUN_0000_4380();
    FUN_0000_4380();
    FUN_0000_43c0();
    FUN_0000_4380();
    FUN_0000_4380();
  }
  return;
}



/* 0000:1826  FUN_0000_1826 */

int __cdecl16near FUN_0000_1826(void)

{
  int iVar1;
  int iVar2;
  int unaff_BP;
  int *piVar3;
  undefined2 unaff_SS;
  
  piVar3 = (int *)*(undefined2 *)(unaff_BP + 0x52);
  iVar1 = 0x100;
  iVar2 = 0;
  do {
    if (*piVar3 != 0) {
      iVar2 = iVar2 + 1;
    }
    piVar3 = piVar3 + 0x29;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return iVar2;
}



/* 0000:1a30  entity_dispatch */

void __cdecl16near entity_dispatch(void)

{
  int in_CX;
  int in_BX;
  int *unaff_SI;
  undefined2 unaff_DS;
  
  do {
    if (*unaff_SI != 0) {
      (*(code *)unaff_SI[1])();
    }
    unaff_SI = (int *)((int)unaff_SI + in_BX);
    in_CX = in_CX + -1;
  } while (in_CX != 0);
  return;
}



/* 0000:1a41  entity_alloc */

void __cdecl16near entity_alloc(void)

{
  int in_AX;
  int in_CX;
  int in_DX;
  int in_BX;
  int *unaff_SI;
  undefined2 unaff_DS;
  
  do {
    if (*unaff_SI == 0) {
      *unaff_SI = in_AX;
      unaff_SI[1] = in_DX;
      return;
    }
    unaff_SI = (int *)((int)unaff_SI + in_BX);
    in_CX = in_CX + -1;
  } while (in_CX != 0);
  return;
}



/* 0000:1a53  entity_alloc_back */

void __cdecl16near entity_alloc_back(void)

{
  int in_AX;
  int in_CX;
  int iVar1;
  int in_DX;
  int in_BX;
  int unaff_SI;
  int *piVar2;
  undefined2 unaff_DS;
  
  iVar1 = in_CX + -1;
  piVar2 = (int *)(unaff_SI + in_BX * iVar1);
  do {
    if (*piVar2 == 0) {
      *piVar2 = in_AX;
      piVar2[1] = in_DX;
      return;
    }
    piVar2 = (int *)((int)piVar2 - in_BX);
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  return;
}



/* 0000:1a70  FUN_0000_1a70 */

void FUN_0000_1a70(void)

{
  undefined2 in_AX;
  int unaff_SI;
  undefined2 unaff_DS;
  undefined2 in_stack_00000000;
  
  *(undefined2 *)(unaff_SI + 6) = in_AX;
  *(undefined2 *)(unaff_SI + 2) = in_stack_00000000;
                    /* WARNING: Could not recover jumptable at 0x00001a76. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)*(undefined2 *)(unaff_SI + 2))();
  return;
}



/* 0000:1a79  FUN_0000_1a79 */

void __cdecl16near FUN_0000_1a79(void)

{
  int *piVar1;
  int unaff_SI;
  undefined2 unaff_DS;
  undefined2 in_stack_00000000;
  
  if (*(int *)(unaff_SI + 6) != 0) {
    piVar1 = (int *)(unaff_SI + 6);
    *piVar1 = *piVar1 + -1;
    if (*piVar1 != 0) {
      return;
    }
  }
  *(undefined2 *)(unaff_SI + 2) = in_stack_00000000;
  return;
}



/* 0000:1a8a  entity_clear_matching */

void __cdecl16near entity_clear_matching(void)

{
  uint *puVar1;
  int iVar2;
  uint in_DX;
  int iVar3;
  uint *unaff_SI;
  uint *puVar4;
  undefined2 unaff_DS;
  
  iVar3 = 0x100;
  do {
    if ((*unaff_SI & in_DX) != 0) {
      puVar4 = unaff_SI;
      for (iVar2 = 0x29; iVar2 != 0; iVar2 = iVar2 + -1) {
        puVar1 = puVar4;
        puVar4 = puVar4 + 1;
        *puVar1 = 0;
      }
    }
    unaff_SI = unaff_SI + 0x29;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  return;
}



/* 0000:1aad  entity_clear_one */

void __cdecl16near entity_clear_one(void)

{
  undefined2 *puVar1;
  int iVar2;
  undefined2 *unaff_SI;
  undefined2 unaff_DS;
  
  for (iVar2 = 0x29; iVar2 != 0; iVar2 = iVar2 + -1) {
    puVar1 = unaff_SI;
    unaff_SI = unaff_SI + 1;
    *puVar1 = 0;
  }
  return;
}



/* 0000:1abf  FUN_0000_1abf */

void __cdecl16near FUN_0000_1abf(void)

{
  undefined2 *puVar1;
  int in_CX;
  uint uVar2;
  uint in_BX;
  undefined2 *unaff_SI;
  undefined2 *unaff_DI;
  undefined2 unaff_DS;
  
  do {
    uVar2 = in_BX >> 1;
    if (unaff_SI == unaff_DI) {
      unaff_DI = unaff_DI + (in_BX >> 1);
    }
    else {
      for (; uVar2 != 0; uVar2 = uVar2 - 1) {
        puVar1 = unaff_DI;
        unaff_DI = unaff_DI + 1;
        *puVar1 = 0;
      }
    }
    in_CX = in_CX + -1;
  } while (in_CX != 0);
  return;
}



/* 0000:1ade  entity_zero */

void __cdecl16near entity_zero(void)

{
  undefined2 *puVar1;
  int in_CX;
  undefined2 *unaff_DI;
  undefined2 unaff_ES;
  
  for (; in_CX != 0; in_CX = in_CX + -1) {
    puVar1 = unaff_DI;
    unaff_DI = unaff_DI + 1;
    *puVar1 = 0;
  }
  return;
}



/* 0000:24c7  FUN_0000_24c7 */

void FUN_0000_24c7(void)

{
  uint in_AX;
  uint in_CX;
  uint uVar1;
  uint uVar2;
  int iVar3;
  undefined1 *puVar4;
  undefined2 unaff_DS;
  
  uVar1 = in_CX & 0x7fff;
  if ((0x853f < in_AX) && (in_AX < 0x86a0)) {
    uVar1 = uVar1 | 0x8000;
  }
  uVar2 = uVar1;
  FUN_0000_2d70();
  if ((uVar2 & 0x8000) != 0) {
    iVar3 = 0x10;
    puVar4 = (undefined1 *)0x2da5;
    do {
      *puVar4 = 0;
      puVar4 = puVar4 + 2;
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
  }
  if ((uVar1 & 0x3000) != 0) {
    if ((uVar1 & 0x2000) == 0) {
      FUN_0000_259c();
    }
    else {
      FUN_0000_25c1();
    }
  }
  if ((uVar1 & 0x300) == 0) {
    if ((uVar1 & 0xc00) == 0) {
      FUN_0000_25dc();
      return;
    }
    if ((uVar1 & 0x400) == 0) {
      FUN_0000_2606();
      return;
    }
    FUN_0000_2643();
    return;
  }
  if ((uVar1 & 0x200) == 0) {
    if ((uVar1 & 0xc00) == 0) {
      FUN_0000_268a();
      return;
    }
    if ((uVar1 & 0x400) == 0) {
      FUN_0000_26b9();
      return;
    }
    FUN_0000_2703();
    return;
  }
  if ((uVar1 & 0x100) != 0) {
    if ((uVar1 & 0xc00) == 0) {
      FUN_0000_2a51();
      return;
    }
    if ((uVar1 & 0x400) == 0) {
      FUN_0000_2ad0();
      return;
    }
    FUN_0000_2bcf();
    return;
  }
  if ((uVar1 & 0xc00) == 0) {
    FUN_0000_2761();
    return;
  }
  if ((uVar1 & 0x400) == 0) {
    FUN_0000_27db();
    return;
  }
  FUN_0000_28c7();
  return;
}



/* 0000:256f  FUN_0000_256f */

undefined1 __cdecl16near FUN_0000_256f(void)

{
  out(0x7c,0);
  return 0;
}



/* 0000:2576  FUN_0000_2576 */

undefined3 __cdecl16near FUN_0000_2576(void)

{
  byte bVar1;
  byte in_DL;
  
  out(0x7c,0xc0);
  out(0x7e,-(in_DL & 1));
  out(0x7e,-(in_DL >> 1 & 1));
  out(0x7e,-(in_DL >> 2 & 1));
  bVar1 = -(in_DL >> 3 & 1);
  out(0x7e,bVar1);
  return CONCAT12(in_DL >> 4,(uint)bVar1);
}



/* 0000:259c  FUN_0000_259c */

void __cdecl16near FUN_0000_259c(void)

{
  uint uVar1;
  int iVar2;
  char cVar3;
  uint *puVar4;
  undefined2 unaff_DS;
  
  iVar2 = 0x10;
  puVar4 = (uint *)0x2da4;
  do {
    uVar1 = *puVar4;
    cVar3 = (char)(uVar1 >> 8);
    *puVar4 = uVar1 | CONCAT11(cVar3 << 1,(byte)((uVar1 & 0xff) << 1) | (int)uVar1 < 0) &
                      (~CONCAT11((char)((CONCAT11((byte)uVar1,cVar3) & 0x1ff) >> 1),(byte)uVar1 >> 1
                                ) | uVar1);
    iVar2 = iVar2 + -1;
    puVar4 = puVar4 + 1;
  } while (iVar2 != 0);
  return;
}



/* 0000:25c1  FUN_0000_25c1 */

void __cdecl16near FUN_0000_25c1(void)

{
  uint uVar1;
  int iVar2;
  uint *puVar3;
  undefined2 unaff_DS;
  
  iVar2 = 0x10;
  puVar3 = (uint *)0x2da4;
  do {
    uVar1 = *puVar3;
    *puVar3 = CONCAT11((char)(uVar1 >> 8) << 1,(byte)((uVar1 & 0xff) << 1) | (int)uVar1 < 0) | uVar1
    ;
    iVar2 = iVar2 + -1;
    puVar3 = puVar3 + 1;
  } while (iVar2 != 0);
  return;
}



/* 0000:25dc  FUN_0000_25dc */

void FUN_0000_25dc(void)

{
  undefined2 *puVar1;
  uint in_CX;
  int iVar2;
  undefined2 *puVar3;
  undefined2 *unaff_DI;
  undefined2 unaff_DS;
  
  FUN_0000_2576();
  puVar3 = (undefined2 *)0x2da4;
  iVar2 = 0x10;
  do {
    puVar1 = puVar3;
    puVar3 = puVar3 + 1;
    *unaff_DI = *puVar1;
    unaff_DI = unaff_DI + 0x28;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  if ((in_CX & 0x8000) == 0) {
    FUN_0000_256f();
    return;
  }
  FUN_0000_256f();
  return;
}



/* 0000:2606  FUN_0000_2606 */

void FUN_0000_2606(void)

{
  uint *puVar1;
  uint uVar2;
  byte bVar3;
  int iVar4;
  uint *puVar5;
  int unaff_DI;
  undefined2 unaff_DS;
  
  FUN_0000_2576();
  puVar5 = (uint *)0x2da4;
  iVar4 = 0x10;
  do {
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    bVar3 = (byte)*puVar1;
    uVar2 = *puVar1 >> 8;
    *(undefined1 *)(unaff_DI + 0x4f) = 0;
    *(undefined2 *)(unaff_DI + 0x50) =
         CONCAT11((char)(((uint)(bVar3 >> 1 & 1) << 8 | ((uint)(bVar3 & 1) << 8 | uVar2) >> 1) >> 1)
                  ,bVar3 >> 2);
    *(undefined1 *)(unaff_DI + 0x52) =
         (char)(CONCAT11((uVar2 & 2) != 0,((uVar2 & 1) != 0) << 7) >> 1);
    unaff_DI = unaff_DI + 0x50;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  FUN_0000_25dc();
  return;
}



/* 0000:2643  FUN_0000_2643 */

void FUN_0000_2643(void)

{
  uint *puVar1;
  uint uVar2;
  char cVar3;
  int iVar4;
  uint *puVar5;
  uint *unaff_DI;
  undefined2 unaff_DS;
  
  FUN_0000_2576();
  puVar5 = (uint *)0x2da4;
  iVar4 = 0x10;
  do {
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    uVar2 = *puVar1;
    unaff_DI[-0x28] = uVar2;
    unaff_DI[0x28] = uVar2;
    cVar3 = (char)(uVar2 >> 8);
    *(bool *)((int)unaff_DI + -1) = (uVar2 & 0x80) != 0;
    *unaff_DI = CONCAT11((char)((CONCAT11((byte)uVar2,cVar3) & 0x1ff) >> 1),(byte)uVar2 >> 1) |
                CONCAT11(cVar3 << 1,(byte)((uVar2 & 0xff) << 1) | (int)uVar2 < 0);
    *(char *)(unaff_DI + 1) = ((uVar2 & 0x100) != 0) << 7;
    unaff_DI = unaff_DI + 0x28;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  FUN_0000_25dc();
  return;
}



/* 0000:268a  FUN_0000_268a */

void FUN_0000_268a(void)

{
  undefined2 *puVar1;
  undefined2 uVar2;
  uint in_CX;
  int iVar3;
  undefined2 *puVar4;
  undefined2 *unaff_DI;
  undefined2 unaff_DS;
  
  FUN_0000_2576();
  puVar4 = (undefined2 *)0x2da4;
  iVar3 = 0x10;
  do {
    puVar1 = puVar4;
    puVar4 = puVar4 + 1;
    uVar2 = *puVar1;
    *unaff_DI = uVar2;
    unaff_DI[0x28] = uVar2;
    unaff_DI = unaff_DI + 0x50;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  if ((in_CX & 0x8000) == 0) {
    FUN_0000_256f();
    return;
  }
  FUN_0000_256f();
  return;
}



/* 0000:26b9  FUN_0000_26b9 */

void FUN_0000_26b9(void)

{
  uint *puVar1;
  char cVar2;
  uint uVar3;
  byte bVar4;
  undefined2 uVar5;
  int iVar6;
  uint *puVar7;
  int unaff_DI;
  undefined2 unaff_DS;
  
  FUN_0000_2576();
  puVar7 = (uint *)0x2da4;
  iVar6 = 0x10;
  do {
    puVar1 = puVar7;
    puVar7 = puVar7 + 1;
    bVar4 = (byte)*puVar1;
    uVar3 = *puVar1 >> 8;
    uVar5 = CONCAT11((char)(((uint)(bVar4 & 1) << 8 | uVar3) >> 1),bVar4 >> 1);
    cVar2 = ((uVar3 & 1) != 0) << 7;
    *(undefined1 *)(unaff_DI + 0x9f) = 0;
    *(undefined1 *)(unaff_DI + 0xf9) = 0;
    *(undefined2 *)(unaff_DI + 0xa0) = uVar5;
    *(undefined2 *)(unaff_DI + 0xf0) = uVar5;
    *(char *)(unaff_DI + 0xa2) = cVar2;
    *(char *)(unaff_DI + 0xf2) = cVar2;
    unaff_DI = unaff_DI + 0xa0;
    iVar6 = iVar6 + -1;
  } while (iVar6 != 0);
  FUN_0000_268a();
  return;
}



/* 0000:2703  FUN_0000_2703 */

void FUN_0000_2703(void)

{
  uint *puVar1;
  char cVar2;
  bool bVar3;
  char cVar5;
  uint uVar4;
  int iVar6;
  uint *puVar7;
  uint *unaff_DI;
  undefined2 unaff_DS;
  
  FUN_0000_2576();
  puVar7 = (uint *)0x2da4;
  iVar6 = 0x10;
  do {
    puVar1 = puVar7;
    puVar7 = puVar7 + 1;
    uVar4 = *puVar1;
    unaff_DI[-0x50] = uVar4;
    unaff_DI[-0x28] = uVar4;
    unaff_DI[0x28] = uVar4;
    unaff_DI[0x50] = uVar4;
    cVar5 = (char)(uVar4 >> 8);
    cVar2 = ((uVar4 & 0x100) != 0) << 7;
    bVar3 = (uVar4 & 0x80) != 0;
    uVar4 = CONCAT11((char)((CONCAT11((byte)uVar4,cVar5) & 0x1ff) >> 1),(byte)uVar4 >> 1) |
            CONCAT11(cVar5 << 1,(byte)((uVar4 & 0xff) << 1) | (int)uVar4 < 0);
    *(bool *)((int)unaff_DI + -1) = bVar3;
    *(bool *)((int)unaff_DI + 0x4f) = bVar3;
    *unaff_DI = uVar4;
    unaff_DI[0x28] = uVar4;
    *(char *)(unaff_DI + 1) = cVar2;
    *(char *)(unaff_DI + 0x29) = cVar2;
    unaff_DI = unaff_DI + 0x50;
    iVar6 = iVar6 + -1;
  } while (iVar6 != 0);
  FUN_0000_268a();
  return;
}



/* 0000:2761  FUN_0000_2761 */

void FUN_0000_2761(void)

{
  char *pcVar1;
  char cVar2;
  int iVar3;
  uint in_CX;
  int iVar4;
  int iVar5;
  char *pcVar6;
  undefined2 *unaff_DI;
  undefined2 *puVar7;
  undefined2 unaff_DS;
  
  FUN_0000_2576();
  pcVar6 = (char *)0x2da4;
  iVar4 = 0x10;
  do {
    iVar5 = 2;
    do {
      puVar7 = unaff_DI;
      pcVar1 = pcVar6;
      pcVar6 = pcVar6 + 1;
      cVar2 = *pcVar1;
      iVar3 = (((((((((((((((uint)(cVar2 < '\0') << 1 | (uint)(cVar2 < '\0')) << 1 |
                          (uint)((char)(cVar2 << 1) < '\0')) << 1 |
                         (uint)((char)(cVar2 << 1) < '\0')) << 1 | (uint)((char)(cVar2 << 2) < '\0')
                        ) << 1 | (uint)((char)(cVar2 << 2) < '\0')) << 1 |
                      (uint)((char)(cVar2 << 3) < '\0')) << 1 | (uint)((char)(cVar2 << 3) < '\0'))
                     << 1 | (uint)((char)(cVar2 << 4) < '\0')) << 1 |
                   (uint)((char)(cVar2 << 4) < '\0')) << 1 | (uint)((char)(cVar2 << 5) < '\0')) << 1
                 | (uint)((char)(cVar2 << 5) < '\0')) << 1 | (uint)((char)(cVar2 << 6) < '\0')) << 1
               | (uint)((char)(cVar2 << 6) < '\0')) << 1 | (uint)((char)(cVar2 << 7) < '\0')) << 1;
      *puVar7 = CONCAT11((byte)iVar3 | (char)(cVar2 << 7) < '\0',(char)((uint)iVar3 >> 8));
      iVar5 = iVar5 + -1;
      unaff_DI = puVar7 + 1;
    } while (iVar5 != 0);
    unaff_DI = puVar7 + 0x27;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  if ((in_CX & 0x8000) == 0) {
    FUN_0000_256f();
    return;
  }
  FUN_0000_256f();
  return;
}



/* 0000:27db  FUN_0000_27db */

void FUN_0000_27db(void)

{
  uint *puVar1;
  int iVar2;
  char cVar3;
  uint uVar4;
  byte bVar5;
  char cVar7;
  int iVar8;
  uint *puVar9;
  int unaff_DI;
  undefined2 unaff_DS;
  byte bVar6;
  
  FUN_0000_2576();
  puVar9 = (uint *)0x2da4;
  iVar8 = 0x10;
  do {
    puVar1 = puVar9;
    puVar9 = puVar9 + 1;
    bVar6 = (byte)*puVar1;
    uVar4 = *puVar1 >> 8;
    bVar5 = bVar6 >> 1;
    cVar7 = (char)(((uint)(bVar6 & 1) << 8 | uVar4) >> 1);
    cVar3 = ((uVar4 & 1) != 0) << 7;
    iVar2 = (((((((((((((uint)((char)(bVar5 << 1) < '\0') << 1 | (uint)((char)(bVar5 << 1) < '\0'))
                       << 1 | (uint)((char)(bVar5 << 2) < '\0')) << 1 |
                     (uint)((char)(bVar5 << 2) < '\0')) << 1 | (uint)((char)(bVar5 << 3) < '\0')) <<
                    1 | (uint)((char)(bVar5 << 3) < '\0')) << 1 | (uint)((char)(bVar5 << 4) < '\0'))
                  << 1 | (uint)((char)(bVar5 << 4) < '\0')) << 1 | (uint)((char)(bVar5 << 5) < '\0')
                ) << 1 | (uint)((char)(bVar5 << 5) < '\0')) << 1 | (uint)((char)(bVar5 << 6) < '\0')
              ) << 1 | (uint)((char)(bVar5 << 6) < '\0')) << 1 | (uint)((char)(bVar5 << 7) < '\0'))
            << 1;
    *(undefined2 *)(unaff_DI + 0x50) =
         CONCAT11((byte)iVar2 | (char)(bVar5 << 7) < '\0',(char)((uint)iVar2 >> 8));
    iVar2 = (((((((((((((((uint)(cVar7 < '\0') << 1 | (uint)(cVar7 < '\0')) << 1 |
                        (uint)((char)(cVar7 << 1) < '\0')) << 1 | (uint)((char)(cVar7 << 1) < '\0'))
                       << 1 | (uint)((char)(cVar7 << 2) < '\0')) << 1 |
                     (uint)((char)(cVar7 << 2) < '\0')) << 1 | (uint)((char)(cVar7 << 3) < '\0')) <<
                    1 | (uint)((char)(cVar7 << 3) < '\0')) << 1 | (uint)((char)(cVar7 << 4) < '\0'))
                  << 1 | (uint)((char)(cVar7 << 4) < '\0')) << 1 | (uint)((char)(cVar7 << 5) < '\0')
                ) << 1 | (uint)((char)(cVar7 << 5) < '\0')) << 1 | (uint)((char)(cVar7 << 6) < '\0')
              ) << 1 | (uint)((char)(cVar7 << 6) < '\0')) << 1 | (uint)((char)(cVar7 << 7) < '\0'))
            << 1;
    *(undefined2 *)(unaff_DI + 0x52) =
         CONCAT11((byte)iVar2 | (char)(cVar7 << 7) < '\0',(char)((uint)iVar2 >> 8));
    *(char *)(unaff_DI + 0x54) = ((cVar3 < '\0') << 1 | cVar3 < '\0') << 6;
    unaff_DI = unaff_DI + 0x50;
    iVar8 = iVar8 + -1;
  } while (iVar8 != 0);
  FUN_0000_2761();
  return;
}



/* 0000:28c7  FUN_0000_28c7 */

void FUN_0000_28c7(void)

{
  uint *puVar1;
  int iVar2;
  char cVar3;
  char cVar4;
  uint uVar6;
  int iVar7;
  uint *puVar8;
  undefined2 *unaff_DI;
  undefined2 unaff_DS;
  uint uVar5;
  
  FUN_0000_2576();
  puVar8 = (uint *)0x2da4;
  iVar7 = 0x10;
  do {
    puVar1 = puVar8;
    puVar8 = puVar8 + 1;
    uVar6 = *puVar1;
    cVar4 = (char)uVar6;
    uVar5 = CONCAT11(cVar4,(char)(uVar6 >> 8));
    cVar3 = ((uVar6 & 0x100) != 0) << 7;
    uVar6 = (uVar5 & 0x7fff) >> 1 | uVar5 | uVar5 << 1;
    *(undefined1 *)((int)unaff_DI + -1) =
         (char)(((uint)(cVar4 < '\0') << 8 | (uint)(cVar4 < '\0') << 7) >> 7);
    cVar4 = (char)(uVar6 >> 8);
    iVar2 = (((((((((((((((uint)((int)uVar6 < 0) << 1 | (uint)((int)uVar6 < 0)) << 1 |
                        (uint)((char)(cVar4 << 1) < '\0')) << 1 | (uint)((char)(cVar4 << 1) < '\0'))
                       << 1 | (uint)((char)(cVar4 << 2) < '\0')) << 1 |
                     (uint)((char)(cVar4 << 2) < '\0')) << 1 | (uint)((char)(cVar4 << 3) < '\0')) <<
                    1 | (uint)((char)(cVar4 << 3) < '\0')) << 1 | (uint)((char)(cVar4 << 4) < '\0'))
                  << 1 | (uint)((char)(cVar4 << 4) < '\0')) << 1 | (uint)((char)(cVar4 << 5) < '\0')
                ) << 1 | (uint)((char)(cVar4 << 5) < '\0')) << 1 | (uint)((char)(cVar4 << 6) < '\0')
              ) << 1 | (uint)((char)(cVar4 << 6) < '\0')) << 1 | (uint)((char)(cVar4 << 7) < '\0'))
            << 1;
    *unaff_DI = CONCAT11((byte)iVar2 | (char)(cVar4 << 7) < '\0',(char)((uint)iVar2 >> 8));
    cVar4 = (char)uVar6;
    iVar2 = (((((((((((((((uint)(cVar4 < '\0') << 1 | (uint)(cVar4 < '\0')) << 1 |
                        (uint)((char)(cVar4 << 1) < '\0')) << 1 | (uint)((char)(cVar4 << 1) < '\0'))
                       << 1 | (uint)((char)(cVar4 << 2) < '\0')) << 1 |
                     (uint)((char)(cVar4 << 2) < '\0')) << 1 | (uint)((char)(cVar4 << 3) < '\0')) <<
                    1 | (uint)((char)(cVar4 << 3) < '\0')) << 1 | (uint)((char)(cVar4 << 4) < '\0'))
                  << 1 | (uint)((char)(cVar4 << 4) < '\0')) << 1 | (uint)((char)(cVar4 << 5) < '\0')
                ) << 1 | (uint)((char)(cVar4 << 5) < '\0')) << 1 | (uint)((char)(cVar4 << 6) < '\0')
              ) << 1 | (uint)((char)(cVar4 << 6) < '\0')) << 1 | (uint)((char)(cVar4 << 7) < '\0'))
            << 1;
    unaff_DI[1] = CONCAT11((byte)iVar2 | (char)(cVar4 << 7) < '\0',(char)((uint)iVar2 >> 8));
    *(char *)(unaff_DI + 2) = ((cVar3 < '\0') << 1 | cVar3 < '\0') << 6;
    unaff_DI = unaff_DI + 0x28;
    iVar7 = iVar7 + -1;
  } while (iVar7 != 0);
  FUN_0000_29f4();
  FUN_0000_29f4();
  FUN_0000_2761();
  return;
}



/* 0000:29f4  FUN_0000_29f4 */

void __cdecl16near FUN_0000_29f4(void)

{
  char *pcVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  char *pcVar6;
  undefined2 *unaff_DI;
  undefined2 *puVar7;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  pcVar6 = (char *)0x2da4;
  iVar4 = 0x10;
  do {
    iVar5 = 2;
    do {
      puVar7 = unaff_DI;
      pcVar1 = pcVar6;
      pcVar6 = pcVar6 + 1;
      cVar2 = *pcVar1;
      iVar3 = (((((((((((((((uint)(cVar2 < '\0') << 1 | (uint)(cVar2 < '\0')) << 1 |
                          (uint)((char)(cVar2 << 1) < '\0')) << 1 |
                         (uint)((char)(cVar2 << 1) < '\0')) << 1 | (uint)((char)(cVar2 << 2) < '\0')
                        ) << 1 | (uint)((char)(cVar2 << 2) < '\0')) << 1 |
                      (uint)((char)(cVar2 << 3) < '\0')) << 1 | (uint)((char)(cVar2 << 3) < '\0'))
                     << 1 | (uint)((char)(cVar2 << 4) < '\0')) << 1 |
                   (uint)((char)(cVar2 << 4) < '\0')) << 1 | (uint)((char)(cVar2 << 5) < '\0')) << 1
                 | (uint)((char)(cVar2 << 5) < '\0')) << 1 | (uint)((char)(cVar2 << 6) < '\0')) << 1
               | (uint)((char)(cVar2 << 6) < '\0')) << 1 | (uint)((char)(cVar2 << 7) < '\0')) << 1;
      *puVar7 = CONCAT11((byte)iVar3 | (char)(cVar2 << 7) < '\0',(char)((uint)iVar3 >> 8));
      iVar5 = iVar5 + -1;
      unaff_DI = puVar7 + 1;
    } while (iVar5 != 0);
    unaff_DI = puVar7 + 0x27;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  return;
}



/* 0000:2a51  FUN_0000_2a51 */

void FUN_0000_2a51(void)

{
  char *pcVar1;
  char cVar2;
  int iVar3;
  undefined2 uVar4;
  uint in_CX;
  int iVar5;
  int iVar6;
  char *pcVar7;
  undefined2 *unaff_DI;
  undefined2 *puVar8;
  undefined2 unaff_DS;
  
  FUN_0000_2576();
  pcVar7 = (char *)0x2da4;
  iVar5 = 0x10;
  do {
    iVar6 = 2;
    do {
      puVar8 = unaff_DI;
      pcVar1 = pcVar7;
      pcVar7 = pcVar7 + 1;
      cVar2 = *pcVar1;
      iVar3 = (((((((((((((((uint)(cVar2 < '\0') << 1 | (uint)(cVar2 < '\0')) << 1 |
                          (uint)((char)(cVar2 << 1) < '\0')) << 1 |
                         (uint)((char)(cVar2 << 1) < '\0')) << 1 | (uint)((char)(cVar2 << 2) < '\0')
                        ) << 1 | (uint)((char)(cVar2 << 2) < '\0')) << 1 |
                      (uint)((char)(cVar2 << 3) < '\0')) << 1 | (uint)((char)(cVar2 << 3) < '\0'))
                     << 1 | (uint)((char)(cVar2 << 4) < '\0')) << 1 |
                   (uint)((char)(cVar2 << 4) < '\0')) << 1 | (uint)((char)(cVar2 << 5) < '\0')) << 1
                 | (uint)((char)(cVar2 << 5) < '\0')) << 1 | (uint)((char)(cVar2 << 6) < '\0')) << 1
               | (uint)((char)(cVar2 << 6) < '\0')) << 1 | (uint)((char)(cVar2 << 7) < '\0')) << 1;
      uVar4 = CONCAT11((byte)iVar3 | (char)(cVar2 << 7) < '\0',(char)((uint)iVar3 >> 8));
      *puVar8 = uVar4;
      puVar8[0x28] = uVar4;
      iVar6 = iVar6 + -1;
      unaff_DI = puVar8 + 1;
    } while (iVar6 != 0);
    unaff_DI = puVar8 + 0x4f;
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
  if ((in_CX & 0x8000) == 0) {
    FUN_0000_256f();
    return;
  }
  FUN_0000_256f();
  return;
}



/* 0000:2ad0  FUN_0000_2ad0 */

void FUN_0000_2ad0(void)

{
  uint *puVar1;
  int iVar2;
  char cVar3;
  uint uVar4;
  byte bVar5;
  char cVar8;
  undefined2 uVar7;
  int iVar9;
  uint *puVar10;
  int unaff_DI;
  undefined2 unaff_DS;
  byte bVar6;
  
  FUN_0000_2576();
  puVar10 = (uint *)0x2da4;
  iVar9 = 0x10;
  do {
    puVar1 = puVar10;
    puVar10 = puVar10 + 1;
    bVar6 = (byte)*puVar1;
    uVar4 = *puVar1 >> 8;
    bVar5 = bVar6 >> 1;
    cVar8 = (char)(((uint)(bVar6 & 1) << 8 | uVar4) >> 1);
    cVar3 = ((uVar4 & 1) != 0) << 7;
    iVar2 = (((((((((((((uint)((char)(bVar5 << 1) < '\0') << 1 | (uint)((char)(bVar5 << 1) < '\0'))
                       << 1 | (uint)((char)(bVar5 << 2) < '\0')) << 1 |
                     (uint)((char)(bVar5 << 2) < '\0')) << 1 | (uint)((char)(bVar5 << 3) < '\0')) <<
                    1 | (uint)((char)(bVar5 << 3) < '\0')) << 1 | (uint)((char)(bVar5 << 4) < '\0'))
                  << 1 | (uint)((char)(bVar5 << 4) < '\0')) << 1 | (uint)((char)(bVar5 << 5) < '\0')
                ) << 1 | (uint)((char)(bVar5 << 5) < '\0')) << 1 | (uint)((char)(bVar5 << 6) < '\0')
              ) << 1 | (uint)((char)(bVar5 << 6) < '\0')) << 1 | (uint)((char)(bVar5 << 7) < '\0'))
            << 1;
    uVar7 = CONCAT11((byte)iVar2 | (char)(bVar5 << 7) < '\0',(char)((uint)iVar2 >> 8));
    *(undefined2 *)(unaff_DI + 0xa0) = uVar7;
    *(undefined2 *)(unaff_DI + 0xf0) = uVar7;
    iVar2 = (((((((((((((((uint)(cVar8 < '\0') << 1 | (uint)(cVar8 < '\0')) << 1 |
                        (uint)((char)(cVar8 << 1) < '\0')) << 1 | (uint)((char)(cVar8 << 1) < '\0'))
                       << 1 | (uint)((char)(cVar8 << 2) < '\0')) << 1 |
                     (uint)((char)(cVar8 << 2) < '\0')) << 1 | (uint)((char)(cVar8 << 3) < '\0')) <<
                    1 | (uint)((char)(cVar8 << 3) < '\0')) << 1 | (uint)((char)(cVar8 << 4) < '\0'))
                  << 1 | (uint)((char)(cVar8 << 4) < '\0')) << 1 | (uint)((char)(cVar8 << 5) < '\0')
                ) << 1 | (uint)((char)(cVar8 << 5) < '\0')) << 1 | (uint)((char)(cVar8 << 6) < '\0')
              ) << 1 | (uint)((char)(cVar8 << 6) < '\0')) << 1 | (uint)((char)(cVar8 << 7) < '\0'))
            << 1;
    uVar7 = CONCAT11((byte)iVar2 | (char)(cVar8 << 7) < '\0',(char)((uint)iVar2 >> 8));
    *(undefined2 *)(unaff_DI + 0xa2) = uVar7;
    *(undefined2 *)(unaff_DI + 0xf2) = uVar7;
    cVar3 = ((cVar3 < '\0') << 1 | cVar3 < '\0') << 6;
    *(char *)(unaff_DI + 0xa4) = cVar3;
    *(char *)(unaff_DI + 0xf4) = cVar3;
    unaff_DI = unaff_DI + 0xa0;
    iVar9 = iVar9 + -1;
  } while (iVar9 != 0);
  FUN_0000_2a51();
  return;
}



/* 0000:2bcf  FUN_0000_2bcf */

void FUN_0000_2bcf(void)

{
  uint *puVar1;
  int iVar2;
  char cVar3;
  undefined1 uVar4;
  char cVar5;
  undefined2 uVar7;
  uint uVar8;
  int iVar9;
  uint *puVar10;
  undefined2 *unaff_DI;
  undefined2 unaff_DS;
  uint uVar6;
  
  FUN_0000_2576();
  puVar10 = (uint *)0x2da4;
  iVar9 = 0x10;
  do {
    puVar1 = puVar10;
    puVar10 = puVar10 + 1;
    uVar8 = *puVar1;
    cVar5 = (char)uVar8;
    uVar6 = CONCAT11(cVar5,(char)(uVar8 >> 8));
    cVar3 = ((uVar8 & 0x100) != 0) << 7;
    uVar8 = (uVar6 & 0x7fff) >> 1 | uVar6 | uVar6 << 1;
    uVar4 = (undefined1)(((uint)(cVar5 < '\0') << 8 | (uint)(cVar5 < '\0') << 7) >> 7);
    *(undefined1 *)((int)unaff_DI + -1) = uVar4;
    *(undefined1 *)((int)unaff_DI + 0x4f) = uVar4;
    cVar5 = (char)(uVar8 >> 8);
    iVar2 = (((((((((((((((uint)((int)uVar8 < 0) << 1 | (uint)((int)uVar8 < 0)) << 1 |
                        (uint)((char)(cVar5 << 1) < '\0')) << 1 | (uint)((char)(cVar5 << 1) < '\0'))
                       << 1 | (uint)((char)(cVar5 << 2) < '\0')) << 1 |
                     (uint)((char)(cVar5 << 2) < '\0')) << 1 | (uint)((char)(cVar5 << 3) < '\0')) <<
                    1 | (uint)((char)(cVar5 << 3) < '\0')) << 1 | (uint)((char)(cVar5 << 4) < '\0'))
                  << 1 | (uint)((char)(cVar5 << 4) < '\0')) << 1 | (uint)((char)(cVar5 << 5) < '\0')
                ) << 1 | (uint)((char)(cVar5 << 5) < '\0')) << 1 | (uint)((char)(cVar5 << 6) < '\0')
              ) << 1 | (uint)((char)(cVar5 << 6) < '\0')) << 1 | (uint)((char)(cVar5 << 7) < '\0'))
            << 1;
    uVar7 = CONCAT11((byte)iVar2 | (char)(cVar5 << 7) < '\0',(char)((uint)iVar2 >> 8));
    *unaff_DI = uVar7;
    unaff_DI[0x28] = uVar7;
    cVar5 = (char)uVar8;
    iVar2 = (((((((((((((((uint)(cVar5 < '\0') << 1 | (uint)(cVar5 < '\0')) << 1 |
                        (uint)((char)(cVar5 << 1) < '\0')) << 1 | (uint)((char)(cVar5 << 1) < '\0'))
                       << 1 | (uint)((char)(cVar5 << 2) < '\0')) << 1 |
                     (uint)((char)(cVar5 << 2) < '\0')) << 1 | (uint)((char)(cVar5 << 3) < '\0')) <<
                    1 | (uint)((char)(cVar5 << 3) < '\0')) << 1 | (uint)((char)(cVar5 << 4) < '\0'))
                  << 1 | (uint)((char)(cVar5 << 4) < '\0')) << 1 | (uint)((char)(cVar5 << 5) < '\0')
                ) << 1 | (uint)((char)(cVar5 << 5) < '\0')) << 1 | (uint)((char)(cVar5 << 6) < '\0')
              ) << 1 | (uint)((char)(cVar5 << 6) < '\0')) << 1 | (uint)((char)(cVar5 << 7) < '\0'))
            << 1;
    uVar7 = CONCAT11((byte)iVar2 | (char)(cVar5 << 7) < '\0',(char)((uint)iVar2 >> 8));
    unaff_DI[1] = uVar7;
    unaff_DI[0x29] = uVar7;
    cVar3 = ((cVar3 < '\0') << 1 | cVar3 < '\0') << 6;
    *(char *)(unaff_DI + 2) = cVar3;
    *(char *)(unaff_DI + 0x2a) = cVar3;
    unaff_DI = unaff_DI + 0x50;
    iVar9 = iVar9 + -1;
  } while (iVar9 != 0);
  FUN_0000_2d0e();
  FUN_0000_2d0e();
  FUN_0000_2a51();
  return;
}



/* 0000:2d0e  FUN_0000_2d0e */

void __cdecl16near FUN_0000_2d0e(void)

{
  char *pcVar1;
  char cVar2;
  int iVar3;
  undefined2 uVar4;
  int iVar5;
  int iVar6;
  char *pcVar7;
  undefined2 *unaff_DI;
  undefined2 *puVar8;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  pcVar7 = (char *)0x2da4;
  iVar5 = 0x10;
  do {
    iVar6 = 2;
    do {
      puVar8 = unaff_DI;
      pcVar1 = pcVar7;
      pcVar7 = pcVar7 + 1;
      cVar2 = *pcVar1;
      iVar3 = (((((((((((((((uint)(cVar2 < '\0') << 1 | (uint)(cVar2 < '\0')) << 1 |
                          (uint)((char)(cVar2 << 1) < '\0')) << 1 |
                         (uint)((char)(cVar2 << 1) < '\0')) << 1 | (uint)((char)(cVar2 << 2) < '\0')
                        ) << 1 | (uint)((char)(cVar2 << 2) < '\0')) << 1 |
                      (uint)((char)(cVar2 << 3) < '\0')) << 1 | (uint)((char)(cVar2 << 3) < '\0'))
                     << 1 | (uint)((char)(cVar2 << 4) < '\0')) << 1 |
                   (uint)((char)(cVar2 << 4) < '\0')) << 1 | (uint)((char)(cVar2 << 5) < '\0')) << 1
                 | (uint)((char)(cVar2 << 5) < '\0')) << 1 | (uint)((char)(cVar2 << 6) < '\0')) << 1
               | (uint)((char)(cVar2 << 6) < '\0')) << 1 | (uint)((char)(cVar2 << 7) < '\0')) << 1;
      uVar4 = CONCAT11((byte)iVar3 | (char)(cVar2 << 7) < '\0',(char)((uint)iVar3 >> 8));
      *puVar8 = uVar4;
      puVar8[0x28] = uVar4;
      iVar6 = iVar6 + -1;
      unaff_DI = puVar8 + 1;
    } while (iVar6 != 0);
    unaff_DI = puVar8 + 0x4f;
    iVar5 = iVar5 + -1;
  } while (iVar5 != 0);
  return;
}



/* 0000:2d70  FUN_0000_2d70 */

undefined2 __cdecl16near FUN_0000_2d70(void)

{
  undefined1 uVar1;
  byte bVar2;
  int iVar3;
  undefined1 in_DL;
  char in_DH;
  undefined1 *puVar4;
  undefined2 unaff_DS;
  
  out(0x68,0xb);
  out(0xa3,in_DH + -0x20);
  out(0xa1,in_DL);
  puVar4 = (undefined1 *)0x2da4;
  iVar3 = 0x10;
  bVar2 = 0;
  do {
    out(0xa5,bVar2 | 0x20);
    uVar1 = in(0xa9);
    *puVar4 = uVar1;
    out(0xa5,bVar2);
    uVar1 = in(0xa9);
    puVar4[1] = uVar1;
    puVar4 = puVar4 + 2;
    bVar2 = bVar2 + 1;
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  out(0x68,10);
  return CONCAT11(bVar2,10);
}



/* 0000:2fd0  vm_execute */

void vm_execute(undefined2 *param_1)

{
  int *piVar1;
  uint *puVar2;
  byte *pbVar3;
  code *pcVar4;
  undefined1 uVar5;
  uint *puVar6;
  undefined2 uVar7;
  uint uVar8;
  int iVar9;
  int iVar10;
  byte bVar11;
  int iVar12;
  uint uVar13;
  int unaff_BP;
  uint *unaff_SI;
  uint *puVar14;
  undefined1 *puVar15;
  byte *pbVar16;
  byte *pbVar17;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  bool bVar18;
  uint uVar19;
  
  FUN_0000_1a70();
  if (*(char *)(unaff_BP + 0xb7) != '\0') {
    return;
  }
  uVar19 = unaff_SI[5];
  puVar15 = (undefined1 *)unaff_SI[4];
  uVar5 = *puVar15;
  puVar14 = (uint *)(puVar15 + 1);
  unaff_SI[4] = (uint)puVar14;
  bVar18 = false;
  switch(uVar5) {
  case 0:
    do {
      FUN_0000_1a70();
      FUN_0000_1a79();
    } while( true );
  case 1:
    entity_clear_one();
    return;
  case 2:
    FUN_0000_1abf();
    vm_fetch();
    return;
  case 3:
    *(uint *)(unaff_BP + 0x6c) = *(uint *)(unaff_BP + 0x6c) | 0x8000;
    *(uint *)(unaff_BP + 0x6e) = *(uint *)(unaff_BP + 0x6e) | 0x8000;
    return;
  case 4:
    return;
  case 5:
    unaff_SI[4] = *puVar14;
    vm_fetch();
    return;
  case 6:
    unaff_SI[4] = (uint)(puVar15 + 3);
    FUN_0000_1a70();
    FUN_0000_1a79();
    vm_fetch();
    return;
  case 7:
    FUN_0000_1a70();
    if ((*(uint *)(unaff_BP + 0x6e) & 0x3f) == 0) {
      unaff_SI[3] = unaff_SI[3] + 1;
    }
    FUN_0000_1a79();
    vm_fetch();
    return;
  case 8:
    uVar19 = *puVar14;
    uVar8 = unaff_SI[6];
    unaff_SI[6] = unaff_SI[6] + 2;
    *(undefined2 *)((int)unaff_SI + uVar8 + 0xe) = puVar15 + 3;
    unaff_SI[4] = uVar19;
    vm_fetch();
    return;
  case 9:
    unaff_SI[6] = unaff_SI[6] - 2;
    unaff_SI[4] = *(uint *)((int)unaff_SI + unaff_SI[6] + 0xe);
    vm_fetch();
    return;
  case 10:
    uVar19 = *puVar14;
    unaff_SI[4] = (uint)(puVar15 + 3);
    uVar8 = unaff_SI[6];
    *(uint *)((int)unaff_SI + uVar8 + 0xe) = (uint)(puVar15 + 3);
    *(uint *)((int)unaff_SI + uVar8 + 0x10) = uVar19;
    unaff_SI[6] = unaff_SI[6] + 4;
    vm_fetch();
    return;
  case 0xb:
    uVar19 = unaff_SI[6];
    piVar1 = (int *)((int)unaff_SI + uVar19 + 0xc);
    *piVar1 = *piVar1 + -1;
    if (*piVar1 == 0) {
      unaff_SI[6] = unaff_SI[6] - 4;
      unaff_SI[4] = (uint)puVar14;
      vm_fetch();
      return;
    }
    unaff_SI[4] = *(uint *)((int)unaff_SI + uVar19 + 10);
    vm_fetch();
    return;
  case 0xc:
    *(uint *)(unaff_BP + 0xc) = *puVar14;
    *(uint *)(unaff_BP + 0xe) = uVar19;
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0,uVar19);
    *(undefined2 *)(unaff_BP + 0xe) = unaff_SS;
    unaff_SI[4] = unaff_SI[4] + 2;
    vm_fetch();
    return;
  case 0xd:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x32);
    puVar6 = (uint *)((uint)(byte)puVar15[4] << 1);
    uVar19 = *(uint *)((uint)(byte)*puVar14 + unaff_BP);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[5] = uVar19;
    unaff_SI[4] = *puVar6;
    vm_fetch();
    return;
  case 0xe:
    *(uint *)(unaff_BP + 0x60) = *puVar14;
    unaff_SI[4] = unaff_SI[4] + 2;
    vm_fetch();
    return;
  case 0xf:
    *(undefined2 *)(unaff_BP + 0x60) = *(undefined2 *)(unaff_BP + 0x62);
    vm_fetch();
    return;
  case 0x10:
    uVar19 = *puVar14;
    *(uint *)(unaff_BP + 0x60) = uVar19;
    *(uint *)(unaff_BP + 0x62) = uVar19;
    unaff_SI[4] = unaff_SI[4] + 2;
    vm_fetch();
    return;
  case 0x11:
    uVar5 = (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3bae))();
    *(undefined1 *)param_1 = uVar5;
    unaff_SI[4] = unaff_SI[4] + 3;
    vm_fetch();
    return;
  case 0x12:
    uVar5 = (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3bae))();
    *(undefined1 *)param_1 = uVar5;
    unaff_SI[4] = unaff_SI[4] + 3;
    vm_fetch();
    return;
  case 0x13:
    bVar11 = (byte)*puVar14;
    uVar8 = *(uint *)(puVar15 + 2);
    puVar15 = puVar15 + 4;
    do {
      if (bVar11 == 0) {
        unaff_SI[4] = uVar8;
        vm_fetch();
        return;
      }
      pbVar17 = puVar15 + 1;
      bVar18 = (undefined1 *)0xfffc < puVar15;
      puVar15 = puVar15 + 3;
      bVar11 = bVar11 - 1;
      (*(code *)*(undefined2 *)(*pbVar17 + 0x3cb8))();
    } while (!bVar18);
    unaff_SI[4] = (uint)(puVar15 + (uint)bVar11 * 3);
    vm_fetch();
    return;
  case 0x14:
    bVar11 = (byte)*puVar14;
    uVar8 = *(uint *)(puVar15 + 2);
    puVar15 = puVar15 + 4;
    do {
      if (bVar11 == 0) {
        uVar19 = unaff_SI[6];
        unaff_SI[6] = unaff_SI[6] + 2;
        *(undefined2 *)((int)unaff_SI + uVar19 + 0xe) = puVar15;
        unaff_SI[4] = uVar8;
        vm_fetch();
        return;
      }
      pbVar17 = puVar15 + 1;
      bVar18 = (undefined1 *)0xfffc < puVar15;
      puVar15 = puVar15 + 3;
      bVar11 = bVar11 - 1;
      (*(code *)*(undefined2 *)(*pbVar17 + 0x3cb8))();
    } while (!bVar18);
    unaff_SI[4] = (uint)(puVar15 + (uint)bVar11 * 3);
    vm_fetch();
    return;
  case 0x15:
    puVar6 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
    entity_alloc();
    if (!bVar18) {
      puVar6[5] = uVar19;
      puVar6[4] = *puVar14;
      *puVar6 = *puVar6 | *(uint *)(puVar15 + 3);
    }
    unaff_SI[4] = unaff_SI[4] + 4;
    vm_fetch();
    return;
  case 0x16:
    bVar18 = false;
    entity_alloc();
    if (!bVar18) {
      unaff_SI[5] = uVar19;
      unaff_SI[4] = *puVar14;
      *unaff_SI = *unaff_SI | *(uint *)(puVar15 + 3);
    }
    unaff_SI[4] = unaff_SI[4] + 4;
    vm_fetch();
    return;
  case 0x17:
    puVar6 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
    entity_alloc_back();
    if (!bVar18) {
      puVar6[5] = uVar19;
      puVar6[4] = *puVar14;
      *puVar6 = *puVar6 | *(uint *)(puVar15 + 3);
    }
    unaff_SI[4] = unaff_SI[4] + 4;
    vm_fetch();
    return;
  case 0x18:
    bVar18 = CARRY2(*(int *)(puVar15 + 5) * 0x52,*(uint *)(unaff_BP + 0x52));
    puVar6 = (uint *)(*(int *)(puVar15 + 5) * 0x52 + *(uint *)(unaff_BP + 0x52));
    entity_alloc();
    if (!bVar18) {
      puVar6[5] = uVar19;
      puVar6[4] = *puVar14;
      *puVar6 = *puVar6 | *(uint *)(puVar15 + 3);
    }
    unaff_SI[4] = unaff_SI[4] + 6;
    vm_fetch();
    return;
  case 0x19:
    uVar8 = *(int *)(puVar15 + 5) * 0x52;
    uVar13 = (int)unaff_SI - *(int *)(unaff_BP + 0x52);
    if (uVar8 < uVar13) {
      bVar18 = 0x100 < uVar13 / 0x52;
      puVar6 = unaff_SI;
    }
    else {
      bVar18 = CARRY2(uVar8,*(uint *)(unaff_BP + 0x52));
      puVar6 = (uint *)(uVar8 + *(uint *)(unaff_BP + 0x52));
    }
    entity_alloc();
    if (!bVar18) {
      puVar6[5] = uVar19;
      puVar6[4] = *puVar14;
      *puVar6 = *puVar6 | *(uint *)(puVar15 + 3);
    }
    unaff_SI[4] = unaff_SI[4] + 6;
    vm_fetch();
    return;
  case 0x1a:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      unaff_SI[4] = *(uint *)(puVar15 + 4);
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x1b:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      unaff_SI[4] = *(uint *)(puVar15 + 5);
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 6;
    vm_fetch();
    return;
  case 0x1c:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      unaff_SI[4] = *(uint *)(puVar15 + 4);
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x1d:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      unaff_SI[4] = *(uint *)(puVar15 + 4);
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x1e:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      uVar19 = *(uint *)(puVar15 + 4);
      uVar8 = unaff_SI[6];
      unaff_SI[6] = unaff_SI[6] + 2;
      *(undefined2 *)((int)unaff_SI + uVar8 + 0xe) = puVar15 + 6;
      unaff_SI[4] = uVar19;
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x1f:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      uVar19 = *(uint *)(puVar15 + 5);
      uVar8 = unaff_SI[6];
      unaff_SI[6] = unaff_SI[6] + 2;
      *(undefined2 *)((int)unaff_SI + uVar8 + 0xe) = puVar15 + 7;
      unaff_SI[4] = uVar19;
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 6;
    vm_fetch();
    return;
  case 0x20:
    while( true ) {
      puVar2 = puVar14;
      puVar14 = (uint *)((int)puVar14 + 1);
      if ((byte)*puVar2 == 0xff) break;
      (*(code *)*(undefined2 *)((byte)*puVar2 + 0x35d8))();
    }
    unaff_SI[4] = (uint)puVar14;
    vm_fetch();
    return;
  case 0x21:
    uVar8 = *puVar14;
    pbVar17 = puVar15 + 3;
    puVar14 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
    iVar12 = 0x100;
    do {
      pbVar16 = pbVar17;
      if ((*puVar14 & uVar8) != 0) {
        while( true ) {
          if (*pbVar16 == 0xff) break;
          (*(code *)*(undefined2 *)(*pbVar16 + 0x35d8))();
          pbVar16 = pbVar16 + 1;
        }
      }
      puVar14 = puVar14 + 0x29;
      iVar12 = iVar12 + -1;
    } while (iVar12 != 0);
    while( true ) {
      if (*pbVar17 == 0xff) break;
      pbVar17 = pbVar17 + 1 + *(int *)((undefined *)&DAT_0000_3604 + *pbVar17);
    }
    unaff_SI[4] = (uint)(pbVar17 + 1);
    vm_fetch();
    return;
  case 0x22:
    pbVar17 = puVar15 + 3;
    puVar6 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
    iVar12 = 0x100;
    do {
      if ((*puVar6 & *puVar14) != 0) {
        while( true ) {
          pbVar3 = pbVar17;
          pbVar17 = pbVar17 + 1;
          if (*pbVar3 == 0xff) break;
          (*(code *)*(undefined2 *)(*pbVar3 + 0x35d8))();
        }
        unaff_SI[4] = (uint)pbVar17;
        vm_fetch();
        return;
      }
      puVar6 = puVar6 + 0x29;
      iVar12 = iVar12 + -1;
    } while (iVar12 != 0);
    while( true ) {
      if (*pbVar17 == 0xff) break;
      pbVar17 = pbVar17 + 1 + *(int *)((undefined *)&DAT_0000_3604 + *pbVar17);
    }
    unaff_SI[4] = (uint)(pbVar17 + 1);
    vm_fetch();
    return;
  case 0x23:
    uVar8 = *puVar14;
    pbVar17 = puVar15 + 3;
    iVar12 = *(int *)(unaff_BP + 0x52);
    iVar9 = 0x100;
    do {
      pbVar16 = pbVar17;
      if (uVar8 == *(uint *)(iVar12 + 4)) {
        while( true ) {
          if (*pbVar16 == 0xff) break;
          (*(code *)*(undefined2 *)(*pbVar16 + 0x35d8))();
          pbVar16 = pbVar16 + 1;
        }
      }
      iVar12 = iVar12 + 0x52;
      iVar9 = iVar9 + -1;
    } while (iVar9 != 0);
    while( true ) {
      if (*pbVar17 == 0xff) break;
      pbVar17 = pbVar17 + 1 + *(int *)((undefined *)&DAT_0000_3604 + *pbVar17);
    }
    unaff_SI[4] = (uint)(pbVar17 + 1);
    vm_fetch();
    return;
  case 0x24:
    pbVar17 = puVar15 + 3;
    iVar12 = *(int *)(unaff_BP + 0x52);
    iVar9 = 0x100;
    do {
      if (*puVar14 == *(uint *)(iVar12 + 4)) {
        while( true ) {
          pbVar3 = pbVar17;
          pbVar17 = pbVar17 + 1;
          if (*pbVar3 == 0xff) break;
          (*(code *)*(undefined2 *)(*pbVar3 + 0x35d8))();
        }
        unaff_SI[4] = (uint)pbVar17;
        vm_fetch();
        return;
      }
      iVar12 = iVar12 + 0x52;
      iVar9 = iVar9 + -1;
    } while (iVar9 != 0);
    while( true ) {
      if (*pbVar17 == 0xff) break;
      pbVar17 = pbVar17 + 1 + *(int *)((undefined *)&DAT_0000_3604 + *pbVar17);
    }
    unaff_SI[4] = (uint)(pbVar17 + 1);
    vm_fetch();
    return;
  case 0x25:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      unaff_SI[4] = *(uint *)(puVar15 + 4);
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x26:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      unaff_SI[4] = *(uint *)(puVar15 + 5);
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 6;
    vm_fetch();
    return;
  case 0x27:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      unaff_SI[4] = *(uint *)(puVar15 + 4);
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x28:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      unaff_SI[4] = *(uint *)(puVar15 + 4);
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x29:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      uVar19 = *(uint *)(puVar15 + 4);
      uVar8 = unaff_SI[6];
      unaff_SI[6] = unaff_SI[6] + 2;
      *(undefined2 *)((int)unaff_SI + uVar8 + 0xe) = puVar15 + 6;
      unaff_SI[4] = uVar19;
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x2a:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      uVar19 = *(uint *)(puVar15 + 5);
      uVar8 = unaff_SI[6];
      unaff_SI[6] = unaff_SI[6] + 2;
      *(undefined2 *)((int)unaff_SI + uVar8 + 0xe) = puVar15 + 7;
      unaff_SI[4] = uVar19;
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 6;
    vm_fetch();
    return;
  case 0x2b:
    uVar5 = (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3bae))();
    *(undefined1 *)param_1 = uVar5;
    unaff_SI[4] = unaff_SI[4] + 3;
    vm_fetch();
    return;
  case 0x2c:
    uVar7 = (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3c2a))();
    *param_1 = uVar7;
    unaff_SI[4] = unaff_SI[4] + 4;
    vm_fetch();
    return;
  case 0x2d:
    uVar5 = (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3bae))();
    *(undefined1 *)param_1 = uVar5;
    unaff_SI[4] = unaff_SI[4] + 3;
    vm_fetch();
    return;
  case 0x2e:
    uVar7 = (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3c2a))();
    *param_1 = uVar7;
    unaff_SI[4] = unaff_SI[4] + 3;
    vm_fetch();
    return;
  case 0x2f:
    uVar5 = (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3bae))();
    *(undefined1 *)param_1 = uVar5;
    unaff_SI[4] = unaff_SI[4] + 3;
    vm_fetch();
    return;
  case 0x30:
    uVar7 = (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3c2a))();
    *param_1 = uVar7;
    unaff_SI[4] = unaff_SI[4] + 3;
    vm_fetch();
    return;
  case 0x31:
    out(0xa6,*(undefined1 *)(unaff_BP + 0xba));
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x32);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    out(0xa6,*(byte *)(unaff_BP + 0xba) ^ 1);
    uVar7 = *(undefined2 *)(unaff_BP + 8);
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x30);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 2;
    vm_fetch();
    return;
  case 0x32:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x32);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x33:
    FUN_0000_1592();
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x34:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x46);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x35:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x46);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 4;
    vm_fetch();
    return;
  case 0x36:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x48);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 4;
    vm_fetch();
    return;
  case 0x37:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x48);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 4;
    vm_fetch();
    return;
  case 0x38:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x4a);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 4;
    vm_fetch();
    return;
  case 0x39:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x4a);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 4;
    vm_fetch();
    return;
  case 0x3a:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x42);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0,uVar19);
    unaff_SI[4] = unaff_SI[4] + 0xc;
    vm_fetch();
    return;
  case 0x3b:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x42);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0,uVar19);
    unaff_SI[4] = unaff_SI[4] + 0xb;
    vm_fetch();
    return;
  case 0x3c:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x44);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
    unaff_SI[4] = unaff_SI[4] + 6;
    vm_fetch();
    return;
  case 0x3d:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x44);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
    unaff_SI[4] = unaff_SI[4] + 6;
    vm_fetch();
    return;
  case 0x3e:
    iVar12 = *(int *)(unaff_BP + 0x52);
    entity_alloc();
    if (!bVar18) {
      *(uint *)(iVar12 + 0x22) = *puVar14;
      *(uint *)(iVar12 + 0x24) = uVar19;
    }
    unaff_SI[4] = unaff_SI[4] + 2;
    vm_fetch();
    return;
  case 0x3f:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x36);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 2;
    vm_fetch();
    return;
  case 0x40:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x3a);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 2;
    vm_fetch();
    return;
  case 0x41:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x38);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0,uVar19);
    unaff_SI[4] = unaff_SI[4] + 1;
    vm_fetch();
    return;
  case 0x42:
    break;
  case 0x43:
    unaff_SI[4] = unaff_SI[4] + 1;
    if ((*(byte *)(unaff_BP + 0xb9) & 4) == 0) {
      pcVar4 = (code *)swi(0x7f);
      (*pcVar4)();
      vm_fetch();
      return;
    }
    vm_fetch();
    return;
  case 0x44:
    unaff_SI[4] = unaff_SI[4] + 1;
    if ((*(byte *)(unaff_BP + 0xb9) & 4) == 0) {
      pcVar4 = (code *)swi(0x7f);
      (*pcVar4)();
      vm_fetch();
      return;
    }
    vm_fetch();
    return;
  case 0x45:
    *(undefined1 *)(unaff_BP + 0xbb) = 0xff;
    pcVar4 = (code *)swi(0x7f);
    (*pcVar4)();
    vm_fetch();
    return;
  case 0x46:
    *(undefined1 *)(unaff_BP + 0xbb) = 0xff;
    pcVar4 = (code *)swi(0x7f);
    (*pcVar4)();
    unaff_SI[4] = unaff_SI[4] + 1;
    vm_fetch();
    return;
  case 0x47:
    draw_text();
    unaff_SI[4] = unaff_SI[4] + 2;
    vm_fetch();
    return;
  case 0x48:
    FUN_0000_1412();
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x49:
    FUN_0000_143f();
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x4a:
    FUN_0000_1494();
    unaff_SI[4] = unaff_SI[4] + 6;
    vm_fetch();
    return;
  case 0x4b:
    iVar9 = *(int *)(unaff_BP + 0x7e) * 0x10;
    iVar12 = *(int *)(unaff_BP + 0x7a) - *(int *)(unaff_BP + 0x82);
    if (iVar12 < 0) {
      iVar12 = iVar12 + iVar9;
    }
    if (iVar9 <= iVar12) {
      iVar12 = iVar12 + *(int *)(unaff_BP + 0x7e) * -0x10;
    }
    *(int *)(unaff_BP + 0x7a) = iVar12;
    iVar12 = *(int *)(unaff_BP + 0x82) + *(int *)(unaff_BP + 0x74);
    if (iVar12 < 1) {
      iVar12 = iVar12 + 400;
    }
    for (; 400 < iVar12; iVar12 = iVar12 + -400) {
    }
    *(int *)(unaff_BP + 0x74) = iVar12;
    *(int *)(unaff_BP + 0x76) = 400 - iVar12;
    *(undefined1 *)(unaff_BP + 0xbe) = 0;
    uVar19 = *(uint *)(unaff_BP + 0x7c);
    uVar8 = *(uint *)(unaff_BP + 0x7a) >> 4;
    *(uint *)(unaff_BP + 0x7c) = uVar8;
    if (uVar8 != uVar19) {
      *(undefined1 *)(unaff_BP + 0xbe) = 1;
      scroll_row_update();
    }
    vm_fetch();
    return;
  case 0x4c:
    if (*(char *)(unaff_BP + 0xc1) == '\0') {
      compose_sprites();
      vm_fetch();
      return;
    }
    vm_fetch();
    return;
  case 0x4d:
    *(undefined1 *)(unaff_BP + 0xc1) = 0;
    iVar12 = 0x19;
    do {
      iVar9 = *(int *)(unaff_BP + 0x74) + 0x10;
      if (400 < iVar9) {
        iVar9 = *(int *)(unaff_BP + 0x74) + -0x180;
      }
      *(int *)(unaff_BP + 0x74) = iVar9;
      *(int *)(unaff_BP + 0x76) = 400 - iVar9;
      iVar9 = *(int *)(unaff_BP + 0x7e);
      iVar10 = *(int *)(unaff_BP + 0x7c) + -1;
      if (iVar9 <= iVar10) {
        iVar10 = iVar10 - iVar9;
      }
      if (iVar10 < 0) {
        iVar10 = iVar10 + iVar9;
      }
      *(int *)(unaff_BP + 0x7c) = iVar10;
      *(int *)(unaff_BP + 0x7a) = iVar10 << 4;
      scroll_row_update();
      iVar12 = iVar12 + -1;
    } while (iVar12 != 0);
    vm_fetch();
    return;
  case 0x4e:
    if (*(char *)(unaff_BP + 0xc1) == '\0') {
      puVar14 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
      iVar12 = 0x100;
      do {
        if (((*puVar14 & 0xff) != 0) && ((*puVar14 & 0x200) == 0)) {
          draw_entity();
        }
        puVar14 = puVar14 + 0x29;
        iVar12 = iVar12 + -1;
      } while (iVar12 != 0);
      vm_fetch();
      return;
    }
    vm_fetch();
    return;
  case 0x4f:
    uVar8 = *puVar14;
    pbVar17 = puVar15 + 3;
    puVar14 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
    iVar12 = 0x100;
    do {
      if ((((*puVar14 & uVar8) != 0) && ((*puVar14 & 0x100) == 0)) &&
         (((int)(unaff_SI[0x21] + unaff_SI[0x11]) < (int)(puVar14[0x22] + puVar14[0x11]) &&
          (((((int)(puVar14[0x21] + puVar14[0x11]) <= (int)(unaff_SI[0x22] + unaff_SI[0x11]) &&
             ((int)(unaff_SI[0x23] + unaff_SI[0x12]) < (int)(puVar14[0x24] + puVar14[0x12]))) &&
            (0x1f < (int)(puVar14[0x24] + puVar14[0x12]))) &&
           (pbVar16 = pbVar17,
           (int)(puVar14[0x23] + puVar14[0x12]) <= (int)(unaff_SI[0x24] + unaff_SI[0x12]))))))) {
        while( true ) {
          if (*pbVar16 == 0xff) break;
          (*(code *)*(undefined2 *)(*pbVar16 + 0x35d8))();
          pbVar16 = pbVar16 + 1;
        }
        unaff_SI[0x1e] = unaff_SI[0x1e] - 1;
      }
      puVar14 = puVar14 + 0x29;
      iVar12 = iVar12 + -1;
    } while (iVar12 != 0);
    while( true ) {
      if (*pbVar17 == 0xff) break;
      pbVar17 = pbVar17 + 1 + *(int *)((undefined *)&DAT_0000_3604 + *pbVar17);
    }
    unaff_SI[4] = (uint)(pbVar17 + 1);
    vm_fetch();
    return;
  case 0x50:
    pbVar17 = puVar15 + 3;
    puVar6 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
    iVar12 = 0x100;
    while (((((*puVar6 & 0x100) != 0 || ((*puVar14 & *puVar6) == 0)) ||
            ((int)(puVar6[0x22] + puVar6[0x11]) <= (int)(unaff_SI[0x21] + unaff_SI[0x11]))) ||
           ((((int)(unaff_SI[0x22] + unaff_SI[0x11]) < (int)(puVar6[0x21] + puVar6[0x11]) ||
             ((int)(puVar6[0x24] + puVar6[0x12]) <= (int)(unaff_SI[0x23] + unaff_SI[0x12]))) ||
            (((int)(puVar6[0x24] + puVar6[0x12]) < 0x20 ||
             ((int)(unaff_SI[0x24] + unaff_SI[0x12]) < (int)(puVar6[0x23] + puVar6[0x12])))))))) {
      puVar6 = puVar6 + 0x29;
      iVar12 = iVar12 + -1;
      if (iVar12 == 0) {
        while( true ) {
          if (*pbVar17 == 0xff) break;
          pbVar17 = pbVar17 + 1 + *(int *)((undefined *)&DAT_0000_3604 + *pbVar17);
        }
        unaff_SI[4] = (uint)(pbVar17 + 1);
        vm_fetch();
        return;
      }
    }
    unaff_SI[0x1e] = unaff_SI[0x1e] - 1;
    while( true ) {
      pbVar3 = pbVar17;
      pbVar17 = pbVar17 + 1;
      if (*pbVar3 == 0xff) break;
      (*(code *)*(undefined2 *)(*pbVar3 + 0x35d8))();
    }
    unaff_SI[4] = (uint)pbVar17;
    vm_fetch();
    return;
  default:
    entity_alloc();
    do {
      FUN_0000_1a70();
      FUN_0000_1a79();
    } while( true );
  }
  unaff_SI[4] = unaff_SI[4] + 1;
  if ((*(byte *)(unaff_BP + 0xb9) & 4) != 0) {
    *(undefined1 *)(unaff_BP + 0xbb) = 0xff;
    vm_fetch();
    return;
  }
  if ((char)*puVar14 != *(char *)(unaff_BP + 0xbb)) {
    *(char *)(unaff_BP + 0xbb) = (char)*puVar14;
    pcVar4 = (code *)swi(0x7f);
    (*pcVar4)();
    vm_fetch();
    return;
  }
  vm_fetch();
  return;
}



/* 0000:2fd6  vm_fetch */

void __cdecl16near vm_fetch(void)

{
  int *piVar1;
  uint *puVar2;
  byte *pbVar3;
  code *pcVar4;
  undefined1 uVar5;
  uint *puVar6;
  undefined2 uVar7;
  uint uVar8;
  int iVar9;
  int iVar10;
  byte bVar11;
  int iVar12;
  uint uVar13;
  int unaff_BP;
  uint *unaff_SI;
  uint *puVar14;
  undefined1 *puVar15;
  byte *pbVar16;
  byte *pbVar17;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  bool bVar18;
  undefined2 *in_stack_00000000;
  uint uVar19;
  
  if (*(char *)(unaff_BP + 0xb7) != '\0') {
    return;
  }
  uVar19 = unaff_SI[5];
  puVar15 = (undefined1 *)unaff_SI[4];
  uVar5 = *puVar15;
  puVar14 = (uint *)(puVar15 + 1);
  unaff_SI[4] = (uint)puVar14;
  bVar18 = false;
  switch(uVar5) {
  case 0:
    do {
      FUN_0000_1a70();
      FUN_0000_1a79();
    } while( true );
  case 1:
    entity_clear_one();
    return;
  case 2:
    FUN_0000_1abf(puVar14);
    vm_fetch();
    return;
  case 3:
    *(uint *)(unaff_BP + 0x6c) = *(uint *)(unaff_BP + 0x6c) | 0x8000;
    *(uint *)(unaff_BP + 0x6e) = *(uint *)(unaff_BP + 0x6e) | 0x8000;
    return;
  case 4:
    return;
  case 5:
    unaff_SI[4] = *puVar14;
    vm_fetch();
    return;
  case 6:
    unaff_SI[4] = (uint)(puVar15 + 3);
    FUN_0000_1a70();
    FUN_0000_1a79();
    vm_fetch();
    return;
  case 7:
    FUN_0000_1a70();
    if ((*(uint *)(unaff_BP + 0x6e) & 0x3f) == 0) {
      unaff_SI[3] = unaff_SI[3] + 1;
    }
    FUN_0000_1a79();
    vm_fetch();
    return;
  case 8:
    uVar19 = *puVar14;
    uVar8 = unaff_SI[6];
    unaff_SI[6] = unaff_SI[6] + 2;
    *(undefined2 *)((int)unaff_SI + uVar8 + 0xe) = puVar15 + 3;
    unaff_SI[4] = uVar19;
    vm_fetch();
    return;
  case 9:
    unaff_SI[6] = unaff_SI[6] - 2;
    unaff_SI[4] = *(uint *)((int)unaff_SI + unaff_SI[6] + 0xe);
    vm_fetch();
    return;
  case 10:
    uVar19 = *puVar14;
    unaff_SI[4] = (uint)(puVar15 + 3);
    uVar8 = unaff_SI[6];
    *(uint *)((int)unaff_SI + uVar8 + 0xe) = (uint)(puVar15 + 3);
    *(uint *)((int)unaff_SI + uVar8 + 0x10) = uVar19;
    unaff_SI[6] = unaff_SI[6] + 4;
    vm_fetch();
    return;
  case 0xb:
    uVar19 = unaff_SI[6];
    piVar1 = (int *)((int)unaff_SI + uVar19 + 0xc);
    *piVar1 = *piVar1 + -1;
    if (*piVar1 == 0) {
      unaff_SI[6] = unaff_SI[6] - 4;
      unaff_SI[4] = (uint)puVar14;
      vm_fetch();
      return;
    }
    unaff_SI[4] = *(uint *)((int)unaff_SI + uVar19 + 10);
    vm_fetch();
    return;
  case 0xc:
    *(uint *)(unaff_BP + 0xc) = *puVar14;
    *(uint *)(unaff_BP + 0xe) = uVar19;
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0,uVar19);
    *(undefined2 *)(unaff_BP + 0xe) = unaff_SS;
    unaff_SI[4] = unaff_SI[4] + 2;
    vm_fetch();
    return;
  case 0xd:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x32);
    puVar6 = (uint *)((uint)(byte)puVar15[4] << 1);
    uVar19 = *(uint *)((uint)(byte)*puVar14 + unaff_BP);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[5] = uVar19;
    unaff_SI[4] = *puVar6;
    vm_fetch();
    return;
  case 0xe:
    *(uint *)(unaff_BP + 0x60) = *puVar14;
    unaff_SI[4] = unaff_SI[4] + 2;
    vm_fetch();
    return;
  case 0xf:
    *(undefined2 *)(unaff_BP + 0x60) = *(undefined2 *)(unaff_BP + 0x62);
    vm_fetch();
    return;
  case 0x10:
    uVar19 = *puVar14;
    *(uint *)(unaff_BP + 0x60) = uVar19;
    *(uint *)(unaff_BP + 0x62) = uVar19;
    unaff_SI[4] = unaff_SI[4] + 2;
    vm_fetch();
    return;
  case 0x11:
    uVar5 = (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3bae))
                      ((uint)(byte)*puVar14 + *(int *)(unaff_BP + 0x54));
    *(undefined1 *)in_stack_00000000 = uVar5;
    unaff_SI[4] = unaff_SI[4] + 3;
    vm_fetch();
    return;
  case 0x12:
    uVar5 = (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3bae))
                      ((uint)(byte)*puVar14 + *(int *)(unaff_BP + 0x54));
    *(undefined1 *)in_stack_00000000 = uVar5;
    unaff_SI[4] = unaff_SI[4] + 3;
    vm_fetch();
    return;
  case 0x13:
    bVar11 = (byte)*puVar14;
    uVar8 = *(uint *)(puVar15 + 2);
    puVar15 = puVar15 + 4;
    do {
      if (bVar11 == 0) {
        unaff_SI[4] = uVar8;
        vm_fetch();
        return;
      }
      pbVar17 = puVar15 + 1;
      bVar18 = (undefined1 *)0xfffc < puVar15;
      puVar15 = puVar15 + 3;
      bVar11 = bVar11 - 1;
      (*(code *)*(undefined2 *)(*pbVar17 + 0x3cb8))();
    } while (!bVar18);
    unaff_SI[4] = (uint)(puVar15 + (uint)bVar11 * 3);
    vm_fetch();
    return;
  case 0x14:
    bVar11 = (byte)*puVar14;
    uVar8 = *(uint *)(puVar15 + 2);
    puVar15 = puVar15 + 4;
    do {
      if (bVar11 == 0) {
        uVar19 = unaff_SI[6];
        unaff_SI[6] = unaff_SI[6] + 2;
        *(undefined2 *)((int)unaff_SI + uVar19 + 0xe) = puVar15;
        unaff_SI[4] = uVar8;
        vm_fetch();
        return;
      }
      pbVar17 = puVar15 + 1;
      bVar18 = (undefined1 *)0xfffc < puVar15;
      puVar15 = puVar15 + 3;
      bVar11 = bVar11 - 1;
      (*(code *)*(undefined2 *)(*pbVar17 + 0x3cb8))();
    } while (!bVar18);
    unaff_SI[4] = (uint)(puVar15 + (uint)bVar11 * 3);
    vm_fetch();
    return;
  case 0x15:
    puVar6 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
    entity_alloc();
    if (!bVar18) {
      puVar6[5] = uVar19;
      puVar6[4] = *puVar14;
      *puVar6 = *puVar6 | *(uint *)(puVar15 + 3);
    }
    unaff_SI[4] = unaff_SI[4] + 4;
    vm_fetch();
    return;
  case 0x16:
    bVar18 = false;
    puVar6 = unaff_SI;
    entity_alloc();
    if (!bVar18) {
      unaff_SI[5] = uVar19;
      unaff_SI[4] = *puVar14;
      *unaff_SI = *unaff_SI | *(uint *)(puVar15 + 3);
    }
    puVar6[4] = puVar6[4] + 4;
    vm_fetch();
    return;
  case 0x17:
    puVar6 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
    entity_alloc_back();
    if (!bVar18) {
      puVar6[5] = uVar19;
      puVar6[4] = *puVar14;
      *puVar6 = *puVar6 | *(uint *)(puVar15 + 3);
    }
    unaff_SI[4] = unaff_SI[4] + 4;
    vm_fetch();
    return;
  case 0x18:
    bVar18 = CARRY2(*(int *)(puVar15 + 5) * 0x52,*(uint *)(unaff_BP + 0x52));
    puVar6 = (uint *)(*(int *)(puVar15 + 5) * 0x52 + *(uint *)(unaff_BP + 0x52));
    entity_alloc();
    if (!bVar18) {
      puVar6[5] = uVar19;
      puVar6[4] = *puVar14;
      *puVar6 = *puVar6 | *(uint *)(puVar15 + 3);
    }
    unaff_SI[4] = unaff_SI[4] + 6;
    vm_fetch();
    return;
  case 0x19:
    uVar8 = *(int *)(puVar15 + 5) * 0x52;
    uVar13 = (int)unaff_SI - *(int *)(unaff_BP + 0x52);
    if (uVar8 < uVar13) {
      bVar18 = 0x100 < uVar13 / 0x52;
      puVar6 = unaff_SI;
    }
    else {
      bVar18 = CARRY2(uVar8,*(uint *)(unaff_BP + 0x52));
      puVar6 = (uint *)(uVar8 + *(uint *)(unaff_BP + 0x52));
    }
    entity_alloc();
    if (!bVar18) {
      puVar6[5] = uVar19;
      puVar6[4] = *puVar14;
      *puVar6 = *puVar6 | *(uint *)(puVar15 + 3);
    }
    unaff_SI[4] = unaff_SI[4] + 6;
    vm_fetch();
    return;
  case 0x1a:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      unaff_SI[4] = *(uint *)(puVar15 + 4);
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x1b:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      unaff_SI[4] = *(uint *)(puVar15 + 5);
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 6;
    vm_fetch();
    return;
  case 0x1c:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      unaff_SI[4] = *(uint *)(puVar15 + 4);
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x1d:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      unaff_SI[4] = *(uint *)(puVar15 + 4);
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x1e:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      uVar19 = *(uint *)(puVar15 + 4);
      uVar8 = unaff_SI[6];
      unaff_SI[6] = unaff_SI[6] + 2;
      *(undefined2 *)((int)unaff_SI + uVar8 + 0xe) = puVar15 + 6;
      unaff_SI[4] = uVar19;
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x1f:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      uVar19 = *(uint *)(puVar15 + 5);
      uVar8 = unaff_SI[6];
      unaff_SI[6] = unaff_SI[6] + 2;
      *(undefined2 *)((int)unaff_SI + uVar8 + 0xe) = puVar15 + 7;
      unaff_SI[4] = uVar19;
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 6;
    vm_fetch();
    return;
  case 0x20:
    while( true ) {
      puVar2 = puVar14;
      puVar14 = (uint *)((int)puVar14 + 1);
      if ((byte)*puVar2 == 0xff) break;
      (*(code *)*(undefined2 *)((byte)*puVar2 + 0x35d8))();
    }
    unaff_SI[4] = (uint)puVar14;
    vm_fetch();
    return;
  case 0x21:
    uVar8 = *puVar14;
    pbVar17 = puVar15 + 3;
    puVar14 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
    iVar12 = 0x100;
    do {
      pbVar16 = pbVar17;
      if ((*puVar14 & uVar8) != 0) {
        while( true ) {
          if (*pbVar16 == 0xff) break;
          (*(code *)*(undefined2 *)(*pbVar16 + 0x35d8))();
          pbVar16 = pbVar16 + 1;
        }
      }
      puVar14 = puVar14 + 0x29;
      iVar12 = iVar12 + -1;
    } while (iVar12 != 0);
    while( true ) {
      if (*pbVar17 == 0xff) break;
      pbVar17 = pbVar17 + 1 + *(int *)((undefined *)&DAT_0000_3604 + *pbVar17);
    }
    unaff_SI[4] = (uint)(pbVar17 + 1);
    vm_fetch();
    return;
  case 0x22:
    pbVar17 = puVar15 + 3;
    puVar6 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
    iVar12 = 0x100;
    do {
      if ((*puVar6 & *puVar14) != 0) {
        while( true ) {
          pbVar3 = pbVar17;
          pbVar17 = pbVar17 + 1;
          if (*pbVar3 == 0xff) break;
          (*(code *)*(undefined2 *)(*pbVar3 + 0x35d8))();
        }
        unaff_SI[4] = (uint)pbVar17;
        vm_fetch();
        return;
      }
      puVar6 = puVar6 + 0x29;
      iVar12 = iVar12 + -1;
    } while (iVar12 != 0);
    while( true ) {
      if (*pbVar17 == 0xff) break;
      pbVar17 = pbVar17 + 1 + *(int *)((undefined *)&DAT_0000_3604 + *pbVar17);
    }
    unaff_SI[4] = (uint)(pbVar17 + 1);
    vm_fetch();
    return;
  case 0x23:
    uVar8 = *puVar14;
    pbVar17 = puVar15 + 3;
    iVar12 = *(int *)(unaff_BP + 0x52);
    iVar9 = 0x100;
    do {
      pbVar16 = pbVar17;
      if (uVar8 == *(uint *)(iVar12 + 4)) {
        while( true ) {
          if (*pbVar16 == 0xff) break;
          (*(code *)*(undefined2 *)(*pbVar16 + 0x35d8))();
          pbVar16 = pbVar16 + 1;
        }
      }
      iVar12 = iVar12 + 0x52;
      iVar9 = iVar9 + -1;
    } while (iVar9 != 0);
    while( true ) {
      if (*pbVar17 == 0xff) break;
      pbVar17 = pbVar17 + 1 + *(int *)((undefined *)&DAT_0000_3604 + *pbVar17);
    }
    unaff_SI[4] = (uint)(pbVar17 + 1);
    vm_fetch();
    return;
  case 0x24:
    pbVar17 = puVar15 + 3;
    iVar12 = *(int *)(unaff_BP + 0x52);
    iVar9 = 0x100;
    do {
      if (*puVar14 == *(uint *)(iVar12 + 4)) {
        while( true ) {
          pbVar3 = pbVar17;
          pbVar17 = pbVar17 + 1;
          if (*pbVar3 == 0xff) break;
          (*(code *)*(undefined2 *)(*pbVar3 + 0x35d8))();
        }
        unaff_SI[4] = (uint)pbVar17;
        vm_fetch();
        return;
      }
      iVar12 = iVar12 + 0x52;
      iVar9 = iVar9 + -1;
    } while (iVar9 != 0);
    while( true ) {
      if (*pbVar17 == 0xff) break;
      pbVar17 = pbVar17 + 1 + *(int *)((undefined *)&DAT_0000_3604 + *pbVar17);
    }
    unaff_SI[4] = (uint)(pbVar17 + 1);
    vm_fetch();
    return;
  case 0x25:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      unaff_SI[4] = *(uint *)(puVar15 + 4);
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x26:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      unaff_SI[4] = *(uint *)(puVar15 + 5);
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 6;
    vm_fetch();
    return;
  case 0x27:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      unaff_SI[4] = *(uint *)(puVar15 + 4);
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x28:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      unaff_SI[4] = *(uint *)(puVar15 + 4);
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x29:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      uVar19 = *(uint *)(puVar15 + 4);
      uVar8 = unaff_SI[6];
      unaff_SI[6] = unaff_SI[6] + 2;
      *(undefined2 *)((int)unaff_SI + uVar8 + 0xe) = puVar15 + 6;
      unaff_SI[4] = uVar19;
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x2a:
    bVar18 = 0xc347 < (byte)puVar15[2];
    (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3cb8))();
    if (!bVar18) {
      uVar19 = *(uint *)(puVar15 + 5);
      uVar8 = unaff_SI[6];
      unaff_SI[6] = unaff_SI[6] + 2;
      *(undefined2 *)((int)unaff_SI + uVar8 + 0xe) = puVar15 + 7;
      unaff_SI[4] = uVar19;
      vm_fetch();
      return;
    }
    unaff_SI[4] = unaff_SI[4] + 6;
    vm_fetch();
    return;
  case 0x2b:
    uVar5 = (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3bae))((uint)(byte)*puVar14 + unaff_BP);
    *(undefined1 *)in_stack_00000000 = uVar5;
    unaff_SI[4] = unaff_SI[4] + 3;
    vm_fetch();
    return;
  case 0x2c:
    uVar7 = (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3c2a))((uint)(byte)*puVar14 + unaff_BP);
    *in_stack_00000000 = uVar7;
    unaff_SI[4] = unaff_SI[4] + 4;
    vm_fetch();
    return;
  case 0x2d:
    uVar5 = (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3bae))((uint)(byte)*puVar14 + unaff_BP);
    *(undefined1 *)in_stack_00000000 = uVar5;
    unaff_SI[4] = unaff_SI[4] + 3;
    vm_fetch();
    return;
  case 0x2e:
    uVar7 = (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3c2a))((uint)(byte)*puVar14 + unaff_BP);
    *in_stack_00000000 = uVar7;
    unaff_SI[4] = unaff_SI[4] + 3;
    vm_fetch();
    return;
  case 0x2f:
    uVar5 = (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3bae))((uint)(byte)*puVar14 + unaff_BP);
    *(undefined1 *)in_stack_00000000 = uVar5;
    unaff_SI[4] = unaff_SI[4] + 3;
    vm_fetch();
    return;
  case 0x30:
    uVar7 = (*(code *)*(undefined2 *)((byte)puVar15[2] + 0x3c2a))((uint)(byte)*puVar14 + unaff_BP);
    *in_stack_00000000 = uVar7;
    unaff_SI[4] = unaff_SI[4] + 3;
    vm_fetch();
    return;
  case 0x31:
    out(0xa6,*(undefined1 *)(unaff_BP + 0xba));
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x32);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    out(0xa6,*(byte *)(unaff_BP + 0xba) ^ 1);
    uVar7 = *(undefined2 *)(unaff_BP + 8);
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x30);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 2;
    vm_fetch();
    return;
  case 0x32:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x32);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x33:
    FUN_0000_1592();
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x34:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x46);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x35:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x46);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 4;
    vm_fetch();
    return;
  case 0x36:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x48);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 4;
    vm_fetch();
    return;
  case 0x37:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x48);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 4;
    vm_fetch();
    return;
  case 0x38:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x4a);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 4;
    vm_fetch();
    return;
  case 0x39:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x4a);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 4;
    vm_fetch();
    return;
  case 0x3a:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x42);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0,uVar19);
    unaff_SI[4] = unaff_SI[4] + 0xc;
    vm_fetch();
    return;
  case 0x3b:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x42);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0,uVar19);
    unaff_SI[4] = unaff_SI[4] + 0xb;
    vm_fetch();
    return;
  case 0x3c:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x44);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 6;
    vm_fetch();
    return;
  case 0x3d:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x44);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 6;
    vm_fetch();
    return;
  case 0x3e:
    iVar12 = *(int *)(unaff_BP + 0x52);
    entity_alloc();
    if (!bVar18) {
      *(uint *)(iVar12 + 0x22) = *puVar14;
      *(uint *)(iVar12 + 0x24) = uVar19;
    }
    unaff_SI[4] = unaff_SI[4] + 2;
    vm_fetch();
    return;
  case 0x3f:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x36);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 2;
    vm_fetch();
    return;
  case 0x40:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x3a);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    unaff_SI[4] = unaff_SI[4] + 2;
    vm_fetch();
    return;
  case 0x41:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x38);
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0,uVar19);
    unaff_SI[4] = unaff_SI[4] + 1;
    vm_fetch();
    return;
  case 0x42:
    break;
  case 0x43:
    unaff_SI[4] = unaff_SI[4] + 1;
    if ((*(byte *)(unaff_BP + 0xb9) & 4) == 0) {
      pcVar4 = (code *)swi(0x7f);
      (*pcVar4)();
      vm_fetch();
      return;
    }
    vm_fetch();
    return;
  case 0x44:
    unaff_SI[4] = unaff_SI[4] + 1;
    if ((*(byte *)(unaff_BP + 0xb9) & 4) == 0) {
      pcVar4 = (code *)swi(0x7f);
      (*pcVar4)();
      vm_fetch();
      return;
    }
    vm_fetch();
    return;
  case 0x45:
    *(undefined1 *)(unaff_BP + 0xbb) = 0xff;
    pcVar4 = (code *)swi(0x7f);
    (*pcVar4)();
    vm_fetch();
    return;
  case 0x46:
    *(undefined1 *)(unaff_BP + 0xbb) = 0xff;
    pcVar4 = (code *)swi(0x7f);
    (*pcVar4)();
    unaff_SI[4] = unaff_SI[4] + 1;
    vm_fetch();
    return;
  case 0x47:
    draw_text();
    unaff_SI[4] = unaff_SI[4] + 2;
    vm_fetch();
    return;
  case 0x48:
    FUN_0000_1412();
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x49:
    FUN_0000_143f();
    unaff_SI[4] = unaff_SI[4] + 5;
    vm_fetch();
    return;
  case 0x4a:
    FUN_0000_1494();
    unaff_SI[4] = unaff_SI[4] + 6;
    vm_fetch();
    return;
  case 0x4b:
    iVar9 = *(int *)(unaff_BP + 0x7e) * 0x10;
    iVar12 = *(int *)(unaff_BP + 0x7a) - *(int *)(unaff_BP + 0x82);
    if (iVar12 < 0) {
      iVar12 = iVar12 + iVar9;
    }
    if (iVar9 <= iVar12) {
      iVar12 = iVar12 + *(int *)(unaff_BP + 0x7e) * -0x10;
    }
    *(int *)(unaff_BP + 0x7a) = iVar12;
    iVar12 = *(int *)(unaff_BP + 0x82) + *(int *)(unaff_BP + 0x74);
    if (iVar12 < 1) {
      iVar12 = iVar12 + 400;
    }
    for (; 400 < iVar12; iVar12 = iVar12 + -400) {
    }
    *(int *)(unaff_BP + 0x74) = iVar12;
    *(int *)(unaff_BP + 0x76) = 400 - iVar12;
    *(undefined1 *)(unaff_BP + 0xbe) = 0;
    uVar19 = *(uint *)(unaff_BP + 0x7c);
    uVar8 = *(uint *)(unaff_BP + 0x7a) >> 4;
    *(uint *)(unaff_BP + 0x7c) = uVar8;
    if (uVar8 != uVar19) {
      *(undefined1 *)(unaff_BP + 0xbe) = 1;
      scroll_row_update();
    }
    vm_fetch();
    return;
  case 0x4c:
    if (*(char *)(unaff_BP + 0xc1) == '\0') {
      compose_sprites();
      vm_fetch();
      return;
    }
    vm_fetch();
    return;
  case 0x4d:
    *(undefined1 *)(unaff_BP + 0xc1) = 0;
    iVar12 = 0x19;
    do {
      iVar9 = *(int *)(unaff_BP + 0x74) + 0x10;
      if (400 < iVar9) {
        iVar9 = *(int *)(unaff_BP + 0x74) + -0x180;
      }
      *(int *)(unaff_BP + 0x74) = iVar9;
      *(int *)(unaff_BP + 0x76) = 400 - iVar9;
      iVar9 = *(int *)(unaff_BP + 0x7e);
      iVar10 = *(int *)(unaff_BP + 0x7c) + -1;
      if (iVar9 <= iVar10) {
        iVar10 = iVar10 - iVar9;
      }
      if (iVar10 < 0) {
        iVar10 = iVar10 + iVar9;
      }
      *(int *)(unaff_BP + 0x7c) = iVar10;
      *(int *)(unaff_BP + 0x7a) = iVar10 << 4;
      scroll_row_update();
      iVar12 = iVar12 + -1;
    } while (iVar12 != 0);
    vm_fetch();
    return;
  case 0x4e:
    if (*(char *)(unaff_BP + 0xc1) == '\0') {
      puVar14 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
      iVar12 = 0x100;
      do {
        if (((*puVar14 & 0xff) != 0) && ((*puVar14 & 0x200) == 0)) {
          draw_entity();
        }
        puVar14 = puVar14 + 0x29;
        iVar12 = iVar12 + -1;
      } while (iVar12 != 0);
      vm_fetch();
      return;
    }
    vm_fetch();
    return;
  case 0x4f:
    uVar8 = *puVar14;
    pbVar17 = puVar15 + 3;
    puVar14 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
    iVar12 = 0x100;
    do {
      if ((((*puVar14 & uVar8) != 0) && ((*puVar14 & 0x100) == 0)) &&
         (((int)(unaff_SI[0x21] + unaff_SI[0x11]) < (int)(puVar14[0x22] + puVar14[0x11]) &&
          (((((int)(puVar14[0x21] + puVar14[0x11]) <= (int)(unaff_SI[0x22] + unaff_SI[0x11]) &&
             ((int)(unaff_SI[0x23] + unaff_SI[0x12]) < (int)(puVar14[0x24] + puVar14[0x12]))) &&
            (0x1f < (int)(puVar14[0x24] + puVar14[0x12]))) &&
           (pbVar16 = pbVar17,
           (int)(puVar14[0x23] + puVar14[0x12]) <= (int)(unaff_SI[0x24] + unaff_SI[0x12]))))))) {
        while( true ) {
          if (*pbVar16 == 0xff) break;
          (*(code *)*(undefined2 *)(*pbVar16 + 0x35d8))();
          pbVar16 = pbVar16 + 1;
        }
        unaff_SI[0x1e] = unaff_SI[0x1e] - 1;
      }
      puVar14 = puVar14 + 0x29;
      iVar12 = iVar12 + -1;
    } while (iVar12 != 0);
    while( true ) {
      if (*pbVar17 == 0xff) break;
      pbVar17 = pbVar17 + 1 + *(int *)((undefined *)&DAT_0000_3604 + *pbVar17);
    }
    unaff_SI[4] = (uint)(pbVar17 + 1);
    vm_fetch();
    return;
  case 0x50:
    pbVar17 = puVar15 + 3;
    puVar6 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
    iVar12 = 0x100;
    while (((((*puVar6 & 0x100) != 0 || ((*puVar14 & *puVar6) == 0)) ||
            ((int)(puVar6[0x22] + puVar6[0x11]) <= (int)(unaff_SI[0x21] + unaff_SI[0x11]))) ||
           ((((int)(unaff_SI[0x22] + unaff_SI[0x11]) < (int)(puVar6[0x21] + puVar6[0x11]) ||
             ((int)(puVar6[0x24] + puVar6[0x12]) <= (int)(unaff_SI[0x23] + unaff_SI[0x12]))) ||
            (((int)(puVar6[0x24] + puVar6[0x12]) < 0x20 ||
             ((int)(unaff_SI[0x24] + unaff_SI[0x12]) < (int)(puVar6[0x23] + puVar6[0x12])))))))) {
      puVar6 = puVar6 + 0x29;
      iVar12 = iVar12 + -1;
      if (iVar12 == 0) {
        while( true ) {
          if (*pbVar17 == 0xff) break;
          pbVar17 = pbVar17 + 1 + *(int *)((undefined *)&DAT_0000_3604 + *pbVar17);
        }
        unaff_SI[4] = (uint)(pbVar17 + 1);
        vm_fetch();
        return;
      }
    }
    unaff_SI[0x1e] = unaff_SI[0x1e] - 1;
    while( true ) {
      pbVar3 = pbVar17;
      pbVar17 = pbVar17 + 1;
      if (*pbVar3 == 0xff) break;
      (*(code *)*(undefined2 *)(*pbVar3 + 0x35d8))();
    }
    unaff_SI[4] = (uint)pbVar17;
    vm_fetch();
    return;
  default:
    entity_alloc();
    do {
      FUN_0000_1a70();
      FUN_0000_1a79();
    } while( true );
  }
  unaff_SI[4] = unaff_SI[4] + 1;
  if ((*(byte *)(unaff_BP + 0xb9) & 4) != 0) {
    *(undefined1 *)(unaff_BP + 0xbb) = 0xff;
    vm_fetch();
    return;
  }
  if ((char)*puVar14 != *(char *)(unaff_BP + 0xbb)) {
    *(char *)(unaff_BP + 0xbb) = (char)*puVar14;
    pcVar4 = (code *)swi(0x7f);
    (*pcVar4)();
    vm_fetch();
    return;
  }
  vm_fetch();
  return;
}



/* 0000:2ff4  vm_invalid */

void vm_invalid(void)

{
  entity_alloc();
  do {
    FUN_0000_1a70();
    FUN_0000_1a79();
  } while( true );
}



/* 0000:3020  vm_op_0 */

void vm_op_0(void)

{
  do {
    FUN_0000_1a70();
    FUN_0000_1a79();
  } while( true );
}



/* 0000:302b  vm_op_1 */

void __cdecl16near vm_op_1(void)

{
  entity_clear_one();
  return;
}



/* 0000:302f  vm_op_2 */

void vm_op_2(void)

{
  FUN_0000_1abf();
  vm_fetch();
  return;
}



/* 0000:303f  vm_op_3 */

void __cdecl16near vm_op_3(void)

{
  int unaff_BP;
  undefined2 unaff_SS;
  
  *(uint *)(unaff_BP + 0x6c) = *(uint *)(unaff_BP + 0x6c) | 0x8000;
  *(uint *)(unaff_BP + 0x6e) = *(uint *)(unaff_BP + 0x6e) | 0x8000;
  return;
}



/* 0000:304a  vm_op_4 */

void __cdecl16near vm_op_4(void)

{
  return;
}



/* 0000:304b  vm_op_5 */

void vm_op_5(void)

{
  int unaff_SI;
  undefined2 *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_SI + 8) = *unaff_DI;
  vm_fetch();
  return;
}



/* 0000:3053  vm_op_6 */

void vm_op_6(void)

{
  int unaff_SI;
  int unaff_DI;
  undefined2 unaff_DS;
  
  *(int *)(unaff_SI + 8) = unaff_DI + 2;
  FUN_0000_1a70();
  FUN_0000_1a79();
  vm_fetch();
  return;
}



/* 0000:3065  vm_op_7 */

void vm_op_7(void)

{
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  FUN_0000_1a70();
  if ((*(uint *)(unaff_BP + 0x6e) & 0x3f) == 0) {
    *(int *)(unaff_SI + 6) = *(int *)(unaff_SI + 6) + 1;
  }
  FUN_0000_1a79();
  vm_fetch();
  return;
}



/* 0000:307b  vm_op_8 */

void vm_op_8(void)

{
  undefined2 uVar1;
  int iVar2;
  int unaff_SI;
  undefined2 *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  uVar1 = *unaff_DI;
  iVar2 = *(int *)(unaff_SI + 0xc);
  *(int *)(unaff_SI + 0xc) = *(int *)(unaff_SI + 0xc) + 2;
  *(undefined2 *)(iVar2 + unaff_SI + 0xe) = unaff_DI + 1;
  *(undefined2 *)(unaff_SI + 8) = uVar1;
  vm_fetch();
  return;
}



/* 0000:3091  vm_op_9 */

void vm_op_9(void)

{
  int unaff_SI;
  undefined2 unaff_DS;
  
  *(int *)(unaff_SI + 0xc) = *(int *)(unaff_SI + 0xc) + -2;
  *(undefined2 *)(unaff_SI + 8) = *(undefined2 *)(*(int *)(unaff_SI + 0xc) + unaff_SI + 0xe);
  vm_fetch();
  return;
}



/* 0000:30a1  vm_op_a */

void vm_op_a(void)

{
  undefined2 uVar1;
  int iVar2;
  int unaff_SI;
  undefined2 *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  uVar1 = *unaff_DI;
  *(undefined2 *)(unaff_SI + 8) = unaff_DI + 1;
  iVar2 = *(int *)(unaff_SI + 0xc);
  *(undefined2 *)(iVar2 + unaff_SI + 0xe) = unaff_DI + 1;
  *(undefined2 *)(iVar2 + unaff_SI + 0x10) = uVar1;
  *(int *)(unaff_SI + 0xc) = *(int *)(unaff_SI + 0xc) + 4;
  vm_fetch();
  return;
}



/* 0000:30bd  vm_op_b */

void vm_op_b(void)

{
  int *piVar1;
  int iVar2;
  int unaff_SI;
  undefined2 unaff_DI;
  undefined2 unaff_DS;
  
  iVar2 = *(int *)(unaff_SI + 0xc) + -2;
  piVar1 = (int *)(iVar2 + unaff_SI + 0xe);
  *piVar1 = *piVar1 + -1;
  if (*piVar1 != 0) {
    *(undefined2 *)(unaff_SI + 8) = *(undefined2 *)(iVar2 + unaff_SI + 0xc);
    vm_fetch();
    return;
  }
  *(int *)(unaff_SI + 0xc) = *(int *)(unaff_SI + 0xc) + -4;
  *(undefined2 *)(unaff_SI + 8) = unaff_DI;
  vm_fetch();
  return;
}



/* 0000:30de  vm_op_d */

void vm_op_d(void)

{
  byte bVar1;
  undefined2 uVar2;
  int unaff_BP;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x32);
  bVar1 = unaff_DI[3];
  uVar2 = *(undefined2 *)((uint)*unaff_DI + unaff_BP);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  *(undefined2 *)(unaff_SI + 10) = uVar2;
  *(undefined2 *)(unaff_SI + 8) = *(undefined2 *)((uint)bVar1 << 1);
  vm_fetch();
  return;
}



/* 0000:311d  vm_op_e */

void vm_op_e(void)

{
  int unaff_BP;
  int unaff_SI;
  undefined2 *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_BP + 0x60) = *unaff_DI;
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 2;
  vm_fetch();
  return;
}



/* 0000:312a  vm_op_f */

void vm_op_f(void)

{
  int unaff_BP;
  undefined2 unaff_SS;
  
  *(undefined2 *)(unaff_BP + 0x60) = *(undefined2 *)(unaff_BP + 0x62);
  vm_fetch();
  return;
}



/* 0000:3133  vm_op_10 */

void vm_op_10(void)

{
  undefined2 uVar1;
  int unaff_BP;
  int unaff_SI;
  undefined2 *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  uVar1 = *unaff_DI;
  *(undefined2 *)(unaff_BP + 0x60) = uVar1;
  *(undefined2 *)(unaff_BP + 0x62) = uVar1;
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 2;
  vm_fetch();
  return;
}



/* 0000:3143  vm_op_11 */

void vm_op_11(void)

{
  undefined1 uVar1;
  int unaff_BP;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined1 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3bae))
                    ((uint)*unaff_DI + *(int *)(unaff_BP + 0x54));
  *in_stack_00000000 = uVar1;
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 3;
  vm_fetch();
  return;
}



/* 0000:316b  vm_op_12 */

void vm_op_12(void)

{
  undefined1 uVar1;
  int unaff_BP;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined1 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3bae))
                    ((uint)*unaff_DI + *(int *)(unaff_BP + 0x54));
  *in_stack_00000000 = uVar1;
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 3;
  vm_fetch();
  return;
}



/* 0000:319b  vm_op_13 */

void vm_op_13(void)

{
  byte *pbVar1;
  undefined2 uVar2;
  byte bVar3;
  int unaff_SI;
  byte *unaff_DI;
  byte *pbVar4;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  bool bVar5;
  
  bVar3 = *unaff_DI;
  uVar2 = *(undefined2 *)(unaff_DI + 1);
  pbVar4 = unaff_DI + 3;
  do {
    if (bVar3 == 0) {
      *(undefined2 *)(unaff_SI + 8) = uVar2;
      vm_fetch();
      return;
    }
    pbVar1 = pbVar4 + 1;
    bVar5 = (byte *)0xfffc < pbVar4;
    pbVar4 = pbVar4 + 3;
    bVar3 = bVar3 - 1;
    (*(code *)*(undefined2 *)(*pbVar1 + 0x3cb8))();
  } while (!bVar5);
  *(undefined2 *)(unaff_SI + 8) = pbVar4 + (uint)bVar3 * 3;
  vm_fetch();
  return;
}



/* 0000:31e6  vm_op_14 */

void vm_op_14(void)

{
  byte *pbVar1;
  undefined2 uVar2;
  int iVar3;
  byte bVar4;
  int unaff_SI;
  byte *unaff_DI;
  byte *pbVar5;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  bool bVar6;
  
  bVar4 = *unaff_DI;
  uVar2 = *(undefined2 *)(unaff_DI + 1);
  pbVar5 = unaff_DI + 3;
  do {
    if (bVar4 == 0) {
      iVar3 = *(int *)(unaff_SI + 0xc);
      *(int *)(unaff_SI + 0xc) = *(int *)(unaff_SI + 0xc) + 2;
      *(undefined2 *)(iVar3 + unaff_SI + 0xe) = pbVar5;
      *(undefined2 *)(unaff_SI + 8) = uVar2;
      vm_fetch();
      return;
    }
    pbVar1 = pbVar5 + 1;
    bVar6 = (byte *)0xfffc < pbVar5;
    pbVar5 = pbVar5 + 3;
    bVar4 = bVar4 - 1;
    (*(code *)*(undefined2 *)(*pbVar1 + 0x3cb8))();
  } while (!bVar6);
  *(undefined2 *)(unaff_SI + 8) = pbVar5 + (uint)bVar4 * 3;
  vm_fetch();
  return;
}



/* 0000:323b  vm_op_15 */

void vm_op_15(void)

{
  uint *puVar1;
  int unaff_BP;
  int unaff_SI;
  uint *unaff_DI;
  uint unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined1 in_CF;
  
  puVar1 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
  entity_alloc();
  if (!(bool)in_CF) {
    puVar1[5] = unaff_ES;
    puVar1[4] = *unaff_DI;
    *puVar1 = *puVar1 | unaff_DI[1];
  }
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 4;
  vm_fetch();
  return;
}



/* 0000:3267  vm_op_17 */

void vm_op_17(void)

{
  uint *puVar1;
  int unaff_BP;
  int unaff_SI;
  uint *unaff_DI;
  uint unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined1 in_CF;
  
  puVar1 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
  entity_alloc_back();
  if (!(bool)in_CF) {
    puVar1[5] = unaff_ES;
    puVar1[4] = *unaff_DI;
    *puVar1 = *puVar1 | unaff_DI[1];
  }
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 4;
  vm_fetch();
  return;
}



/* 0000:3293  vm_op_16 */

void vm_op_16(void)

{
  uint *unaff_SI;
  uint *unaff_DI;
  uint unaff_ES;
  undefined2 unaff_DS;
  bool bVar1;
  
  bVar1 = false;
  entity_alloc();
  if (!bVar1) {
    unaff_SI[5] = unaff_ES;
    unaff_SI[4] = *unaff_DI;
    *unaff_SI = *unaff_SI | unaff_DI[1];
  }
  unaff_SI[4] = unaff_SI[4] + 4;
  vm_fetch();
  return;
}



/* 0000:32cb  vm_op_19 */

void vm_op_19(void)

{
  uint uVar1;
  uint uVar2;
  int unaff_BP;
  uint *unaff_SI;
  uint *puVar3;
  uint *unaff_DI;
  uint unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  bool bVar4;
  
  uVar1 = unaff_DI[2] * 0x52;
  uVar2 = (int)unaff_SI - *(int *)(unaff_BP + 0x52);
  if (uVar1 < uVar2) {
    bVar4 = 0x100 < uVar2 / 0x52;
    puVar3 = unaff_SI;
  }
  else {
    bVar4 = CARRY2(uVar1,*(uint *)(unaff_BP + 0x52));
    puVar3 = (uint *)(uVar1 + *(uint *)(unaff_BP + 0x52));
  }
  entity_alloc();
  if (!bVar4) {
    puVar3[5] = unaff_ES;
    puVar3[4] = *unaff_DI;
    *puVar3 = *puVar3 | unaff_DI[1];
  }
  unaff_SI[4] = unaff_SI[4] + 6;
  vm_fetch();
  return;
}



/* 0000:3316  vm_op_18 */

void vm_op_18(void)

{
  uint *puVar1;
  int unaff_BP;
  int unaff_SI;
  uint *unaff_DI;
  uint unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  bool bVar2;
  
  bVar2 = CARRY2(unaff_DI[2] * 0x52,*(uint *)(unaff_BP + 0x52));
  puVar1 = (uint *)(unaff_DI[2] * 0x52 + *(uint *)(unaff_BP + 0x52));
  entity_alloc();
  if (!bVar2) {
    puVar1[5] = unaff_ES;
    puVar1[4] = *unaff_DI;
    *puVar1 = *puVar1 | unaff_DI[1];
  }
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 6;
  vm_fetch();
  return;
}



/* 0000:334c  vm_op_1a */

void vm_op_1a(void)

{
  int unaff_SI;
  int unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  bool bVar1;
  
  bVar1 = 0xc347 < *(byte *)(unaff_DI + 1);
  (*(code *)*(undefined2 *)(*(byte *)(unaff_DI + 1) + 0x3cb8))();
  if (bVar1) {
    *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 5;
    vm_fetch();
    return;
  }
  *(undefined2 *)(unaff_SI + 8) = *(undefined2 *)(unaff_DI + 3);
  vm_fetch();
  return;
}



/* 0000:337d  vm_op_1b */

void vm_op_1b(void)

{
  int unaff_SI;
  int unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  bool bVar1;
  
  bVar1 = 0xc347 < *(byte *)(unaff_DI + 1);
  (*(code *)*(undefined2 *)(*(byte *)(unaff_DI + 1) + 0x3cb8))();
  if (bVar1) {
    *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 6;
    vm_fetch();
    return;
  }
  *(undefined2 *)(unaff_SI + 8) = *(undefined2 *)(unaff_DI + 4);
  vm_fetch();
  return;
}



/* 0000:33aa  vm_op_1c */

void vm_op_1c(void)

{
  int unaff_SI;
  int unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  bool bVar1;
  
  bVar1 = 0xc347 < *(byte *)(unaff_DI + 1);
  (*(code *)*(undefined2 *)(*(byte *)(unaff_DI + 1) + 0x3cb8))();
  if (bVar1) {
    *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 5;
    vm_fetch();
    return;
  }
  *(undefined2 *)(unaff_SI + 8) = *(undefined2 *)(unaff_DI + 3);
  vm_fetch();
  return;
}



/* 0000:33e1  vm_op_1d */

void vm_op_1d(void)

{
  int unaff_SI;
  int unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  bool bVar1;
  
  bVar1 = 0xc347 < *(byte *)(unaff_DI + 1);
  (*(code *)*(undefined2 *)(*(byte *)(unaff_DI + 1) + 0x3cb8))();
  if (bVar1) {
    *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 5;
    vm_fetch();
    return;
  }
  *(undefined2 *)(unaff_SI + 8) = *(undefined2 *)(unaff_DI + 3);
  vm_fetch();
  return;
}



/* 0000:3414  vm_op_1e */

void vm_op_1e(void)

{
  undefined2 uVar1;
  int iVar2;
  int unaff_SI;
  int unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  bool bVar3;
  
  bVar3 = 0xc347 < *(byte *)(unaff_DI + 1);
  (*(code *)*(undefined2 *)(*(byte *)(unaff_DI + 1) + 0x3cb8))();
  if (bVar3) {
    *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 5;
    vm_fetch();
    return;
  }
  uVar1 = *(undefined2 *)(unaff_DI + 3);
  iVar2 = *(int *)(unaff_SI + 0xc);
  *(int *)(unaff_SI + 0xc) = *(int *)(unaff_SI + 0xc) + 2;
  *(int *)(iVar2 + unaff_SI + 0xe) = unaff_DI + 5;
  *(undefined2 *)(unaff_SI + 8) = uVar1;
  vm_fetch();
  return;
}



/* 0000:3452  vm_op_1f */

void vm_op_1f(void)

{
  undefined2 uVar1;
  int iVar2;
  int unaff_SI;
  int unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  bool bVar3;
  
  bVar3 = 0xc347 < *(byte *)(unaff_DI + 1);
  (*(code *)*(undefined2 *)(*(byte *)(unaff_DI + 1) + 0x3cb8))();
  if (bVar3) {
    *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 6;
    vm_fetch();
    return;
  }
  uVar1 = *(undefined2 *)(unaff_DI + 4);
  iVar2 = *(int *)(unaff_SI + 0xc);
  *(int *)(unaff_SI + 0xc) = *(int *)(unaff_SI + 0xc) + 2;
  *(int *)(iVar2 + unaff_SI + 0xe) = unaff_DI + 6;
  *(undefined2 *)(unaff_SI + 8) = uVar1;
  vm_fetch();
  return;
}



/* 0000:348c  vm_op_20 */

void vm_op_20(void)

{
  byte *pbVar1;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  while( true ) {
    pbVar1 = unaff_DI;
    unaff_DI = unaff_DI + 1;
    if (*pbVar1 == 0xff) break;
    (*(code *)*(undefined2 *)(*pbVar1 + 0x35d8))();
  }
  *(undefined2 *)(unaff_SI + 8) = unaff_DI;
  vm_fetch();
  return;
}



/* 0000:34a6  vm_op_21 */

void vm_op_21(void)

{
  uint uVar1;
  int iVar2;
  int unaff_BP;
  int unaff_SI;
  uint *puVar3;
  uint *unaff_DI;
  uint *puVar4;
  uint *puVar5;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  uVar1 = *unaff_DI;
  puVar5 = unaff_DI + 1;
  puVar3 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
  iVar2 = 0x100;
  do {
    puVar4 = puVar5;
    if ((*puVar3 & uVar1) != 0) {
      while( true ) {
        if ((byte)*puVar4 == 0xff) break;
        (*(code *)*(undefined2 *)((byte)*puVar4 + 0x35d8))();
        puVar4 = (uint *)((int)puVar4 + 1);
      }
    }
    puVar3 = puVar3 + 0x29;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  while( true ) {
    if ((byte)*puVar5 == 0xff) break;
    puVar5 = (uint *)((byte *)((int)puVar5 + 1) +
                     *(int *)((undefined *)&DAT_0000_3604 + (byte)*puVar5));
  }
  *(undefined2 *)(unaff_SI + 8) = (byte *)((int)puVar5 + 1);
  vm_fetch();
  return;
}



/* 0000:34f1  vm_op_22 */

void vm_op_22(void)

{
  uint *puVar1;
  int iVar2;
  uint *puVar3;
  int unaff_BP;
  int unaff_SI;
  uint *unaff_DI;
  uint *puVar4;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  puVar4 = unaff_DI + 1;
  puVar3 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
  iVar2 = 0x100;
  do {
    if ((*puVar3 & *unaff_DI) != 0) {
      while( true ) {
        puVar1 = puVar4;
        puVar4 = (uint *)((int)puVar4 + 1);
        if ((byte)*puVar1 == 0xff) break;
        (*(code *)*(undefined2 *)((byte)*puVar1 + 0x35d8))();
      }
      *(undefined2 *)(unaff_SI + 8) = puVar4;
      vm_fetch();
      return;
    }
    puVar3 = puVar3 + 0x29;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  while( true ) {
    if ((byte)*puVar4 == 0xff) break;
    puVar4 = (uint *)((byte *)((int)puVar4 + 1) +
                     *(int *)((undefined *)&DAT_0000_3604 + (byte)*puVar4));
  }
  *(undefined2 *)(unaff_SI + 8) = (byte *)((int)puVar4 + 1);
  vm_fetch();
  return;
}



/* 0000:353e  vm_op_23 */

void vm_op_23(void)

{
  int iVar1;
  int iVar2;
  int unaff_BP;
  int unaff_SI;
  int iVar3;
  int *unaff_DI;
  byte *pbVar4;
  byte *pbVar5;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  iVar1 = *unaff_DI;
  pbVar5 = (byte *)(unaff_DI + 1);
  iVar3 = *(int *)(unaff_BP + 0x52);
  iVar2 = 0x100;
  do {
    pbVar4 = pbVar5;
    if (iVar1 == *(int *)(iVar3 + 4)) {
      while( true ) {
        if (*pbVar4 == 0xff) break;
        (*(code *)*(undefined2 *)(*pbVar4 + 0x35d8))();
        pbVar4 = pbVar4 + 1;
      }
    }
    iVar3 = iVar3 + 0x52;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  while( true ) {
    if (*pbVar5 == 0xff) break;
    pbVar5 = pbVar5 + 1 + *(int *)((undefined *)&DAT_0000_3604 + *pbVar5);
  }
  *(undefined2 *)(unaff_SI + 8) = pbVar5 + 1;
  vm_fetch();
  return;
}



/* 0000:358a  vm_op_24 */

void vm_op_24(void)

{
  byte *pbVar1;
  int iVar2;
  int iVar3;
  int unaff_BP;
  int unaff_SI;
  int *unaff_DI;
  byte *pbVar4;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  pbVar4 = (byte *)(unaff_DI + 1);
  iVar3 = *(int *)(unaff_BP + 0x52);
  iVar2 = 0x100;
  do {
    if (*unaff_DI == *(int *)(iVar3 + 4)) {
      while( true ) {
        pbVar1 = pbVar4;
        pbVar4 = pbVar4 + 1;
        if (*pbVar1 == 0xff) break;
        (*(code *)*(undefined2 *)(*pbVar1 + 0x35d8))();
      }
      *(undefined2 *)(unaff_SI + 8) = pbVar4;
      vm_fetch();
      return;
    }
    iVar3 = iVar3 + 0x52;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  while( true ) {
    if (*pbVar4 == 0xff) break;
    pbVar4 = pbVar4 + 1 + *(int *)((undefined *)&DAT_0000_3604 + *pbVar4);
  }
  *(undefined2 *)(unaff_SI + 8) = pbVar4 + 1;
  vm_fetch();
  return;
}



/* 0000:3630  vm_sub_0 */

void __cdecl16near vm_sub_0(void)

{
  undefined1 uVar1;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  undefined1 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3bae))((uint)*unaff_DI + unaff_SI);
  *in_stack_00000000 = uVar1;
  return;
}



/* 0000:3652  vm_sub_1 */

void __cdecl16near vm_sub_1(void)

{
  undefined2 uVar1;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  undefined2 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3c2a))((uint)*unaff_DI + unaff_SI);
  *in_stack_00000000 = uVar1;
  return;
}



/* 0000:3674  vm_sub_2 */

void __cdecl16near vm_sub_2(void)

{
  undefined1 uVar1;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  undefined1 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3bae))((uint)*unaff_DI + unaff_SI);
  *in_stack_00000000 = uVar1;
  return;
}



/* 0000:369c  vm_sub_3 */

void __cdecl16near vm_sub_3(void)

{
  undefined2 uVar1;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  undefined2 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3c2a))((uint)*unaff_DI + unaff_SI);
  *in_stack_00000000 = uVar1;
  return;
}



/* 0000:36c4  vm_sub_4 */

void __cdecl16near vm_sub_4(void)

{
  undefined1 uVar1;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  undefined1 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3bae))((uint)*unaff_DI + unaff_SI);
  *in_stack_00000000 = uVar1;
  return;
}



/* 0000:36ed  vm_sub_5 */

void __cdecl16near vm_sub_5(void)

{
  undefined2 uVar1;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  undefined2 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3c2a))((uint)*unaff_DI + unaff_SI);
  *in_stack_00000000 = uVar1;
  return;
}



/* 0000:3716  vm_sub_6 */

void __cdecl16near vm_sub_6(void)

{
  undefined1 uVar1;
  int unaff_BP;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined1 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3bae))((uint)*unaff_DI + unaff_BP);
  *in_stack_00000000 = uVar1;
  return;
}



/* 0000:373a  vm_sub_7 */

void __cdecl16near vm_sub_7(void)

{
  undefined2 uVar1;
  int unaff_BP;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3c2a))((uint)*unaff_DI + unaff_BP);
  *in_stack_00000000 = uVar1;
  return;
}



/* 0000:375e  vm_sub_8 */

void __cdecl16near vm_sub_8(void)

{
  undefined1 uVar1;
  int unaff_BP;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined1 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3bae))((uint)*unaff_DI + unaff_BP);
  *in_stack_00000000 = uVar1;
  return;
}



/* 0000:3788  vm_sub_9 */

void __cdecl16near vm_sub_9(void)

{
  undefined2 uVar1;
  int unaff_BP;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3c2a))((uint)*unaff_DI + unaff_BP);
  *in_stack_00000000 = uVar1;
  return;
}



/* 0000:37b2  vm_sub_a */

void __cdecl16near vm_sub_a(void)

{
  undefined1 uVar1;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  undefined1 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3bae))((uint)*unaff_DI + unaff_SI);
  *in_stack_00000000 = uVar1;
  return;
}



/* 0000:37db  vm_sub_b */

void __cdecl16near vm_sub_b(void)

{
  undefined2 uVar1;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  undefined2 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3c2a))((uint)*unaff_DI + unaff_SI);
  *in_stack_00000000 = uVar1;
  return;
}



/* 0000:3804  vm_sub_c */

void __cdecl16near vm_sub_c(void)

{
  undefined1 uVar1;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  undefined1 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3bae))((uint)*unaff_DI + unaff_SI);
  *in_stack_00000000 = uVar1;
  return;
}



/* 0000:3833  vm_sub_d */

void __cdecl16near vm_sub_d(void)

{
  undefined2 uVar1;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  undefined2 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3c2a))((uint)*unaff_DI + unaff_SI);
  *in_stack_00000000 = uVar1;
  return;
}



/* 0000:3862  vm_sub_e */

void __cdecl16near vm_sub_e(void)

{
  return;
}



/* 0000:3866  vm_sub_f */

void __cdecl16near vm_sub_f(undefined6 param_1,double *param_2,double *param_3)

{
  undefined2 uVar1;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  undefined2 *in_stack_00000000;
  
  sincos((double)CONCAT62(param_1,in_stack_00000000),param_2,param_3);
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3c2a))((uint)*unaff_DI + unaff_SI);
  *in_stack_00000000 = uVar1;
  return;
}



/* 0000:38a3  vm_sub_10 */

void __cdecl16near vm_sub_10(void)

{
  return;
}



/* 0000:38a7  vm_sub_11 */

void __cdecl16near vm_sub_11(undefined6 param_1,double *param_2,double *param_3)

{
  undefined2 uVar1;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  undefined2 *in_stack_00000000;
  
  sincos((double)CONCAT62(param_1,in_stack_00000000),param_2,param_3);
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3c2a))((uint)*unaff_DI + unaff_SI);
  *in_stack_00000000 = uVar1;
  return;
}



/* 0000:38e8  vm_sub_12 */

void __cdecl16near vm_sub_12(void)

{
  return;
}



/* 0000:38ec  vm_sub_13 */

void __cdecl16near vm_sub_13(void)

{
  undefined2 uVar1;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  undefined2 *in_stack_00000000;
  
  aim_helper();
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3c2a))((uint)*unaff_DI + unaff_SI);
  *in_stack_00000000 = uVar1;
  return;
}



/* 0000:3925  vm_sub_14 */

void __cdecl16near vm_sub_14(void)

{
  return;
}



/* 0000:3929  vm_sub_15 */

void __cdecl16near vm_sub_15(void)

{
  undefined2 uVar1;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  undefined2 *in_stack_00000000;
  
  aim_helper_2();
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3c2a))((uint)*unaff_DI + unaff_SI);
  *in_stack_00000000 = uVar1;
  return;
}



/* 0000:3962  vm_op_25 */

void vm_op_25(void)

{
  int unaff_SI;
  int unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  bool bVar1;
  
  bVar1 = 0xc347 < *(byte *)(unaff_DI + 1);
  (*(code *)*(undefined2 *)(*(byte *)(unaff_DI + 1) + 0x3cb8))();
  if (bVar1) {
    *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 5;
    vm_fetch();
    return;
  }
  *(undefined2 *)(unaff_SI + 8) = *(undefined2 *)(unaff_DI + 3);
  vm_fetch();
  return;
}



/* 0000:3994  vm_op_26 */

void vm_op_26(void)

{
  int unaff_SI;
  int unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  bool bVar1;
  
  bVar1 = 0xc347 < *(byte *)(unaff_DI + 1);
  (*(code *)*(undefined2 *)(*(byte *)(unaff_DI + 1) + 0x3cb8))();
  if (bVar1) {
    *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 6;
    vm_fetch();
    return;
  }
  *(undefined2 *)(unaff_SI + 8) = *(undefined2 *)(unaff_DI + 4);
  vm_fetch();
  return;
}



/* 0000:39c2  vm_op_27 */

void vm_op_27(void)

{
  int unaff_SI;
  int unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  bool bVar1;
  
  bVar1 = 0xc347 < *(byte *)(unaff_DI + 1);
  (*(code *)*(undefined2 *)(*(byte *)(unaff_DI + 1) + 0x3cb8))();
  if (bVar1) {
    *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 5;
    vm_fetch();
    return;
  }
  *(undefined2 *)(unaff_SI + 8) = *(undefined2 *)(unaff_DI + 3);
  vm_fetch();
  return;
}



/* 0000:39fb  vm_op_28 */

void vm_op_28(void)

{
  int unaff_SI;
  int unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  bool bVar1;
  
  bVar1 = 0xc347 < *(byte *)(unaff_DI + 1);
  (*(code *)*(undefined2 *)(*(byte *)(unaff_DI + 1) + 0x3cb8))();
  if (bVar1) {
    *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 5;
    vm_fetch();
    return;
  }
  *(undefined2 *)(unaff_SI + 8) = *(undefined2 *)(unaff_DI + 3);
  vm_fetch();
  return;
}



/* 0000:3a30  vm_op_29 */

void vm_op_29(void)

{
  undefined2 uVar1;
  int iVar2;
  int unaff_SI;
  int unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  bool bVar3;
  
  bVar3 = 0xc347 < *(byte *)(unaff_DI + 1);
  (*(code *)*(undefined2 *)(*(byte *)(unaff_DI + 1) + 0x3cb8))();
  if (bVar3) {
    *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 5;
    vm_fetch();
    return;
  }
  uVar1 = *(undefined2 *)(unaff_DI + 3);
  iVar2 = *(int *)(unaff_SI + 0xc);
  *(int *)(unaff_SI + 0xc) = *(int *)(unaff_SI + 0xc) + 2;
  *(int *)(iVar2 + unaff_SI + 0xe) = unaff_DI + 5;
  *(undefined2 *)(unaff_SI + 8) = uVar1;
  vm_fetch();
  return;
}



/* 0000:3a6f  vm_op_2a */

void vm_op_2a(void)

{
  undefined2 uVar1;
  int iVar2;
  int unaff_SI;
  int unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  bool bVar3;
  
  bVar3 = 0xc347 < *(byte *)(unaff_DI + 1);
  (*(code *)*(undefined2 *)(*(byte *)(unaff_DI + 1) + 0x3cb8))();
  if (bVar3) {
    *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 6;
    vm_fetch();
    return;
  }
  uVar1 = *(undefined2 *)(unaff_DI + 4);
  iVar2 = *(int *)(unaff_SI + 0xc);
  *(int *)(unaff_SI + 0xc) = *(int *)(unaff_SI + 0xc) + 2;
  *(int *)(iVar2 + unaff_SI + 0xe) = unaff_DI + 6;
  *(undefined2 *)(unaff_SI + 8) = uVar1;
  vm_fetch();
  return;
}



/* 0000:3aaa  vm_op_2b */

void vm_op_2b(void)

{
  undefined1 uVar1;
  int unaff_BP;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined1 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3bae))((uint)*unaff_DI + unaff_BP);
  *in_stack_00000000 = uVar1;
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 3;
  vm_fetch();
  return;
}



/* 0000:3ad1  vm_op_2c */

void vm_op_2c(void)

{
  undefined2 uVar1;
  int unaff_BP;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined2 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3c2a))((uint)*unaff_DI + unaff_BP);
  *in_stack_00000000 = uVar1;
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 4;
  vm_fetch();
  return;
}



/* 0000:3af8  vm_op_2d */

void vm_op_2d(void)

{
  undefined1 uVar1;
  int unaff_BP;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined1 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3bae))((uint)*unaff_DI + unaff_BP);
  *in_stack_00000000 = uVar1;
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 3;
  vm_fetch();
  return;
}



/* 0000:3b26  vm_op_2e */

void vm_op_2e(void)

{
  undefined2 uVar1;
  int unaff_BP;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined2 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3c2a))((uint)*unaff_DI + unaff_BP);
  *in_stack_00000000 = uVar1;
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 3;
  vm_fetch();
  return;
}



/* 0000:3b54  vm_op_2f */

void vm_op_2f(void)

{
  undefined1 uVar1;
  int unaff_BP;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined1 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3bae))((uint)*unaff_DI + unaff_BP);
  *in_stack_00000000 = uVar1;
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 3;
  vm_fetch();
  return;
}



/* 0000:3b81  vm_op_30 */

void vm_op_30(void)

{
  undefined2 uVar1;
  int unaff_BP;
  int unaff_SI;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined2 *in_stack_00000000;
  
  uVar1 = (*(code *)*(undefined2 *)(unaff_DI[1] + 0x3c2a))((uint)*unaff_DI + unaff_BP);
  *in_stack_00000000 = uVar1;
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 3;
  vm_fetch();
  return;
}



/* 0000:3bd0  alu_byte_2 */

void __cdecl16near alu_byte_2(void)

{
  return;
}



/* 0000:3bd3  alu_byte_1 */

char __cdecl16near alu_byte_1(void)

{
  char in_AH;
  
  return -in_AH;
}



/* 0000:3bd8  alu_byte_7 */

char __cdecl16near alu_byte_7(void)

{
  byte in_AL;
  byte in_AH;
  int unaff_BP;
  undefined2 unaff_SS;
  
  *(byte *)(unaff_BP + 0xbd) = *(byte *)(unaff_BP + 0xbd) << 1 | CARRY1(in_AL,in_AH);
  return in_AL + in_AH;
}



/* 0000:3bdf  alu_byte_8 */

char __cdecl16near alu_byte_8(void)

{
  byte in_AL;
  byte in_AH;
  int unaff_BP;
  undefined2 unaff_SS;
  
  *(byte *)(unaff_BP + 0xbd) = *(byte *)(unaff_BP + 0xbd) << 1 | in_AL < in_AH;
  return in_AL - in_AH;
}



/* 0000:3be6  alu_byte_5 */

char __cdecl16near alu_byte_5(void)

{
  uint uVar1;
  uint uVar2;
  bool bVar3;
  byte in_AL;
  byte in_AH;
  int unaff_BP;
  undefined2 unaff_SS;
  undefined1 in_CF;
  
  uVar1 = CONCAT11(in_CF,*(undefined1 *)(unaff_BP + 0xbd));
  uVar2 = uVar1 >> 1;
  *(undefined1 *)(unaff_BP + 0xbd) = (char)uVar2;
  bVar3 = ((uVar2 | uVar1 << 8) & 0x100) != 0;
  *(byte *)(unaff_BP + 0xbd) =
       *(byte *)(unaff_BP + 0xbd) << 1 | (CARRY1(in_AL,in_AH) || CARRY1(in_AL + in_AH,bVar3));
  return in_AL + in_AH + bVar3;
}



/* 0000:3bf1  alu_byte_6 */

char __cdecl16near alu_byte_6(void)

{
  uint uVar1;
  uint uVar2;
  bool bVar3;
  byte in_AL;
  byte in_AH;
  int unaff_BP;
  undefined2 unaff_SS;
  undefined1 in_CF;
  
  uVar1 = CONCAT11(in_CF,*(undefined1 *)(unaff_BP + 0xbd));
  uVar2 = uVar1 >> 1;
  *(undefined1 *)(unaff_BP + 0xbd) = (char)uVar2;
  bVar3 = ((uVar2 | uVar1 << 8) & 0x100) != 0;
  *(byte *)(unaff_BP + 0xbd) =
       *(byte *)(unaff_BP + 0xbd) << 1 | (in_AL < in_AH || (byte)(in_AL - in_AH) < bVar3);
  return (in_AL - in_AH) - bVar3;
}



/* 0000:3bfc  alu_byte_9 */

byte __cdecl16near alu_byte_9(void)

{
  byte in_AL;
  byte in_AH;
  
  return in_AL & in_AH;
}



/* 0000:3bff  alu_byte_a */

byte __cdecl16near alu_byte_a(void)

{
  byte in_AL;
  byte in_AH;
  
  return in_AL | in_AH;
}



/* 0000:3c02  alu_byte_b */

byte __cdecl16near alu_byte_b(void)

{
  byte in_AL;
  byte in_AH;
  
  return in_AL ^ in_AH;
}



/* 0000:3c05  alu_byte_c */

int __cdecl16near alu_byte_c(void)

{
  uint in_AX;
  
  return (in_AX & 0xff) * (in_AX >> 8);
}



/* 0000:3c08  alu_byte_d */

undefined2 __cdecl16near alu_byte_d(void)

{
  uint in_AX;
  
  return CONCAT11((char)((in_AX & 0xff) % (in_AX >> 8)),(char)((in_AX & 0xff) / (in_AX >> 8)));
}



/* 0000:3c0f  alu_byte_e */

undefined2 __cdecl16near alu_byte_e(void)

{
  uint uVar1;
  undefined1 uVar2;
  byte extraout_DL;
  
  uVar1 = rng_mix();
  uVar2 = (undefined1)((uVar1 & 0xff) % (uint)extraout_DL);
  return CONCAT11(uVar2,uVar2);
}



/* 0000:3c1b  alu_byte_f */

char __cdecl16near alu_byte_f(void)

{
  char in_AL;
  byte in_AH;
  
  return in_AL << (in_AH & 0x1f);
}



/* 0000:3c22  alu_byte_10 */

char __cdecl16near alu_byte_10(void)

{
  char in_AL;
  byte in_AH;
  
  return in_AL >> (in_AH & 0x1f);
}



/* 0000:3c4c  alu_word_0 */

void __cdecl16near alu_word_0(void)

{
  return;
}



/* 0000:3c4f  alu_word_1 */

int __cdecl16near alu_word_1(void)

{
  int in_DX;
  
  return -in_DX;
}



/* 0000:3c54  alu_word_2 */

int __cdecl16near alu_word_2(void)

{
  char in_DL;
  
  return (int)in_DL;
}



/* 0000:3c58  alu_word_3 */

int __cdecl16near alu_word_3(void)

{
  uint in_AX;
  uint in_DX;
  int unaff_BP;
  undefined2 unaff_SS;
  
  *(byte *)(unaff_BP + 0xbd) = *(byte *)(unaff_BP + 0xbd) << 1 | CARRY2(in_AX,in_DX);
  return in_AX + in_DX;
}



/* 0000:3c5f  alu_word_4 */

int __cdecl16near alu_word_4(void)

{
  uint in_AX;
  uint in_DX;
  int unaff_BP;
  undefined2 unaff_SS;
  
  *(byte *)(unaff_BP + 0xbd) = *(byte *)(unaff_BP + 0xbd) << 1 | in_AX < in_DX;
  return in_AX - in_DX;
}



/* 0000:3c66  alu_word_5 */

int __cdecl16near alu_word_5(void)

{
  uint uVar1;
  uint uVar2;
  uint in_AX;
  uint in_DX;
  int unaff_BP;
  undefined2 unaff_SS;
  undefined1 in_CF;
  
  uVar1 = CONCAT11(in_CF,*(undefined1 *)(unaff_BP + 0xbd));
  uVar2 = uVar1 >> 1;
  *(undefined1 *)(unaff_BP + 0xbd) = (char)uVar2;
  uVar1 = (uint)(((uVar2 | uVar1 << 8) & 0x100) != 0);
  *(byte *)(unaff_BP + 0xbd) =
       *(byte *)(unaff_BP + 0xbd) << 1 | (CARRY2(in_AX,in_DX) || CARRY2(in_AX + in_DX,uVar1));
  return in_AX + in_DX + uVar1;
}



/* 0000:3c71  alu_word_6 */

int __cdecl16near alu_word_6(void)

{
  uint uVar1;
  uint uVar2;
  uint in_AX;
  uint in_DX;
  int unaff_BP;
  undefined2 unaff_SS;
  undefined1 in_CF;
  
  uVar1 = CONCAT11(in_CF,*(undefined1 *)(unaff_BP + 0xbd));
  uVar2 = uVar1 >> 1;
  *(undefined1 *)(unaff_BP + 0xbd) = (char)uVar2;
  uVar1 = (uint)(((uVar2 | uVar1 << 8) & 0x100) != 0);
  *(byte *)(unaff_BP + 0xbd) =
       *(byte *)(unaff_BP + 0xbd) << 1 | (in_AX < in_DX || in_AX - in_DX < uVar1);
  return (in_AX - in_DX) - uVar1;
}



/* 0000:3c7c  alu_word_7 */

int __cdecl16near alu_word_7(void)

{
  uint in_AX;
  char in_DL;
  int unaff_BP;
  undefined2 unaff_SS;
  
  *(byte *)(unaff_BP + 0xbd) = *(byte *)(unaff_BP + 0xbd) << 1 | CARRY2((int)in_DL,in_AX);
  return (int)in_DL + in_AX;
}



/* 0000:3c85  alu_word_8 */

int __cdecl16near alu_word_8(void)

{
  uint in_AX;
  char in_DL;
  int unaff_BP;
  undefined2 unaff_SS;
  
  *(byte *)(unaff_BP + 0xbd) = *(byte *)(unaff_BP + 0xbd) << 1 | in_AX < (uint)(int)in_DL;
  return in_AX - (int)in_DL;
}



/* 0000:3c8f  alu_word_9 */

uint __cdecl16near alu_word_9(void)

{
  uint in_AX;
  uint in_DX;
  
  return in_AX & in_DX;
}



/* 0000:3c92  alu_word_a */

uint __cdecl16near alu_word_a(void)

{
  uint in_AX;
  uint in_DX;
  
  return in_AX | in_DX;
}



/* 0000:3c95  alu_word_b */

uint __cdecl16near alu_word_b(void)

{
  uint in_AX;
  uint in_DX;
  
  return in_AX ^ in_DX;
}



/* 0000:3c98  alu_word_c */

long __cdecl16near alu_word_c(void)

{
  uint in_AX;
  uint in_DX;
  
  return (ulong)in_AX * (ulong)in_DX;
}



/* 0000:3c9b  alu_word_d */

uint __cdecl16near alu_word_d(void)

{
  uint in_AX;
  uint in_DX;
  
  return in_AX / in_DX;
}



/* 0000:3ca2  alu_word_e */

uint __cdecl16near alu_word_e(void)

{
  undefined4 uVar1;
  
  uVar1 = rng_mix();
  return (uint)uVar1 % (uint)((ulong)uVar1 >> 0x10);
}



/* 0000:3cae  alu_word_f */

int __cdecl16near alu_word_f(void)

{
  int in_AX;
  byte in_DL;
  
  return in_AX << (in_DL & 0x1f);
}



/* 0000:3cb3  alu_word_10 */

int __cdecl16near alu_word_10(void)

{
  int in_AX;
  byte in_DL;
  
  return in_AX >> (in_DL & 0x1f);
}



/* 0000:3cd2  alu_cmp_0 */

void __cdecl16near alu_cmp_0(void)

{
  int in_AX;
  int in_DX;
  
  if (in_AX != in_DX) {
    return;
  }
  return;
}



/* 0000:3cd8  alu_cmp_1 */

void __cdecl16near alu_cmp_1(void)

{
  int in_AX;
  int in_DX;
  
  if (in_AX == in_DX) {
    return;
  }
  return;
}



/* 0000:3cde  alu_cmp_2 */

void __cdecl16near alu_cmp_2(void)

{
  uint in_AX;
  uint in_DX;
  
  if (in_AX <= in_DX) {
    return;
  }
  return;
}



/* 0000:3ce4  alu_cmp_3 */

void __cdecl16near alu_cmp_3(void)

{
  uint in_AX;
  uint in_DX;
  
  if (in_DX <= in_AX) {
    return;
  }
  return;
}



/* 0000:3cea  alu_cmp_4 */

void __cdecl16near alu_cmp_4(void)

{
  int in_AX;
  int in_DX;
  
  if (in_AX <= in_DX) {
    return;
  }
  return;
}



/* 0000:3cf0  alu_cmp_5 */

void __cdecl16near alu_cmp_5(void)

{
  int in_AX;
  int in_DX;
  
  if (in_DX <= in_AX) {
    return;
  }
  return;
}



/* 0000:3cf6  alu_cmp_6 */

void __cdecl16near alu_cmp_6(void)

{
  uint in_AX;
  uint in_DX;
  
  if (in_AX < in_DX) {
    return;
  }
  return;
}



/* 0000:3cfc  alu_cmp_7 */

void __cdecl16near alu_cmp_7(void)

{
  uint in_AX;
  uint in_DX;
  
  if (in_DX < in_AX) {
    return;
  }
  return;
}



/* 0000:3d02  alu_cmp_8 */

void __cdecl16near alu_cmp_8(void)

{
  int in_AX;
  int in_DX;
  
  if (in_AX < in_DX) {
    return;
  }
  return;
}



/* 0000:3d08  alu_cmp_9 */

void __cdecl16near alu_cmp_9(void)

{
  int in_AX;
  int in_DX;
  
  if (in_DX < in_AX) {
    return;
  }
  return;
}



/* 0000:3d0e  alu_cmp_a */

void __cdecl16near alu_cmp_a(void)

{
  uint in_AX;
  uint in_DX;
  
  if ((in_DX & in_AX) != 0) {
    return;
  }
  return;
}



/* 0000:3d14  alu_cmp_b */

void __cdecl16near alu_cmp_b(void)

{
  uint in_AX;
  uint in_DX;
  
  if ((in_DX & in_AX) == 0) {
    return;
  }
  return;
}



/* 0000:3d1a  vm_op_31 */

void vm_op_31(void)

{
  undefined2 uVar1;
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  out(0xa6,*(undefined1 *)(unaff_BP + 0xba));
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x32);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  out(0xa6,*(byte *)(unaff_BP + 0xba) ^ 1);
  uVar1 = *(undefined2 *)(unaff_BP + 8);
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x30);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 2;
  vm_fetch();
  return;
}



/* 0000:3d5b  vm_op_32 */

void vm_op_32(void)

{
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x32);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 5;
  vm_fetch();
  return;
}



/* 0000:3d84  vm_op_33 */

void vm_op_33(void)

{
  int unaff_SI;
  undefined2 unaff_DS;
  
  FUN_0000_1592();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 5;
  vm_fetch();
  return;
}



/* 0000:3da8  vm_op_34 */

void vm_op_34(void)

{
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x46);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 5;
  vm_fetch();
  return;
}



/* 0000:3dcb  vm_op_35 */

void vm_op_35(void)

{
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x46);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 4;
  vm_fetch();
  return;
}



/* 0000:3e10  vm_op_36 */

void vm_op_36(void)

{
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x48);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 4;
  vm_fetch();
  return;
}



/* 0000:3e2f  vm_op_37 */

void vm_op_37(void)

{
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x48);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 4;
  vm_fetch();
  return;
}



/* 0000:3e6f  vm_op_38 */

void vm_op_38(void)

{
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x4a);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 4;
  vm_fetch();
  return;
}



/* 0000:3e8e  vm_op_39 */

void vm_op_39(void)

{
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x4a);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 4;
  vm_fetch();
  return;
}



/* 0000:3ece  vm_op_3a */

void vm_op_3a(void)

{
  int unaff_BP;
  int unaff_SI;
  int unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x42);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 0xc;
  vm_fetch();
  return;
}



/* 0000:3f0a  vm_op_3b */

void vm_op_3b(void)

{
  int unaff_BP;
  int unaff_SI;
  int unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x42);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 0xb;
  vm_fetch();
  return;
}



/* 0000:3f63  vm_op_3c */

void vm_op_3c(void)

{
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x44);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 6;
  vm_fetch();
  return;
}



/* 0000:3f84  vm_op_3d */

void vm_op_3d(void)

{
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x44);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 6;
  vm_fetch();
  return;
}



/* 0000:3fc6  vm_op_3e */

void vm_op_3e(void)

{
  int iVar1;
  int unaff_BP;
  int unaff_SI;
  undefined2 *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined1 in_CF;
  
  iVar1 = *(int *)(unaff_BP + 0x52);
  entity_alloc();
  if (!(bool)in_CF) {
    *(undefined2 *)(iVar1 + 0x22) = *unaff_DI;
    *(undefined2 *)(iVar1 + 0x24) = unaff_ES;
  }
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 2;
  vm_fetch();
  return;
}



/* 0000:3fec  vm_op_c */

void vm_op_c(void)

{
  int unaff_BP;
  int unaff_SI;
  undefined2 *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_BP + 0xc) = *unaff_DI;
  *(undefined2 *)(unaff_BP + 0xe) = unaff_ES;
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  *(undefined2 *)(unaff_BP + 0xe) = unaff_SS;
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 2;
  vm_fetch();
  return;
}



/* 0000:4008  vm_op_3f */

void vm_op_3f(void)

{
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x36);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 2;
  vm_fetch();
  return;
}



/* 0000:4023  vm_op_40 */

void vm_op_40(void)

{
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x3a);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 2;
  vm_fetch();
  return;
}



/* 0000:403e  vm_op_41 */

void vm_op_41(void)

{
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x38);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 1;
  vm_fetch();
  return;
}



/* 0000:4054  vm_op_42 */

void vm_op_42(void)

{
  code *pcVar1;
  int unaff_BP;
  int unaff_SI;
  char *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 1;
  if ((*(byte *)(unaff_BP + 0xb9) & 4) != 0) {
    *(undefined1 *)(unaff_BP + 0xbb) = 0xff;
    vm_fetch();
    return;
  }
  if (*unaff_DI == *(char *)(unaff_BP + 0xbb)) {
    vm_fetch();
    return;
  }
  *(char *)(unaff_BP + 0xbb) = *unaff_DI;
  pcVar1 = (code *)swi(0x7f);
  (*pcVar1)();
  vm_fetch();
  return;
}



/* 0000:407d  vm_op_43 */

void vm_op_43(void)

{
  code *pcVar1;
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 1;
  if ((*(byte *)(unaff_BP + 0xb9) & 4) != 0) {
    vm_fetch();
    return;
  }
  pcVar1 = (code *)swi(0x7f);
  (*pcVar1)();
  vm_fetch();
  return;
}



/* 0000:4094  vm_op_44 */

void vm_op_44(void)

{
  code *pcVar1;
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 1;
  if ((*(byte *)(unaff_BP + 0xb9) & 4) != 0) {
    vm_fetch();
    return;
  }
  pcVar1 = (code *)swi(0x7f);
  (*pcVar1)();
  vm_fetch();
  return;
}



/* 0000:40b2  vm_op_45 */

void vm_op_45(void)

{
  code *pcVar1;
  int unaff_BP;
  undefined2 unaff_SS;
  
  *(undefined1 *)(unaff_BP + 0xbb) = 0xff;
  pcVar1 = (code *)swi(0x7f);
  (*pcVar1)();
  vm_fetch();
  return;
}



/* 0000:40be  vm_op_46 */

void vm_op_46(void)

{
  code *pcVar1;
  int unaff_BP;
  int unaff_SI;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined1 *)(unaff_BP + 0xbb) = 0xff;
  pcVar1 = (code *)swi(0x7f);
  (*pcVar1)();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 1;
  vm_fetch();
  return;
}



/* 0000:40d0  vm_op_47 */

void vm_op_47(void)

{
  int unaff_SI;
  undefined2 unaff_DS;
  
  draw_text();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 2;
  vm_fetch();
  return;
}



/* 0000:40e5  vm_op_48 */

void vm_op_48(void)

{
  int unaff_SI;
  undefined2 unaff_DS;
  
  FUN_0000_1412();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 5;
  vm_fetch();
  return;
}



/* 0000:4101  vm_op_49 */

void vm_op_49(void)

{
  int unaff_SI;
  undefined2 unaff_DS;
  
  FUN_0000_143f();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 5;
  vm_fetch();
  return;
}



/* 0000:411d  vm_op_4a */

void vm_op_4a(void)

{
  int unaff_SI;
  undefined2 unaff_DS;
  
  FUN_0000_1494();
  *(int *)(unaff_SI + 8) = *(int *)(unaff_SI + 8) + 6;
  vm_fetch();
  return;
}



/* 0000:4144  vm_op_4b */

void vm_op_4b(void)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  int iVar4;
  int unaff_BP;
  undefined2 unaff_SS;
  
  iVar4 = *(int *)(unaff_BP + 0x7e) * 0x10;
  iVar2 = *(int *)(unaff_BP + 0x7a) - *(int *)(unaff_BP + 0x82);
  if (iVar2 < 0) {
    iVar2 = iVar2 + iVar4;
  }
  if (iVar4 <= iVar2) {
    iVar2 = iVar2 + *(int *)(unaff_BP + 0x7e) * -0x10;
  }
  *(int *)(unaff_BP + 0x7a) = iVar2;
  iVar2 = *(int *)(unaff_BP + 0x82) + *(int *)(unaff_BP + 0x74);
  if (iVar2 < 1) {
    iVar2 = iVar2 + 400;
  }
  for (; 400 < iVar2; iVar2 = iVar2 + -400) {
  }
  *(int *)(unaff_BP + 0x74) = iVar2;
  *(int *)(unaff_BP + 0x76) = 400 - iVar2;
  *(undefined1 *)(unaff_BP + 0xbe) = 0;
  uVar1 = *(uint *)(unaff_BP + 0x7c);
  uVar3 = *(uint *)(unaff_BP + 0x7a) >> 4;
  *(uint *)(unaff_BP + 0x7c) = uVar3;
  if (uVar3 != uVar1) {
    *(undefined1 *)(unaff_BP + 0xbe) = 1;
    scroll_row_update();
  }
  vm_fetch();
  return;
}



/* 0000:41a7  vm_op_4c */

void vm_op_4c(void)

{
  int unaff_BP;
  undefined2 unaff_SS;
  
  if (*(char *)(unaff_BP + 0xc1) != '\0') {
    vm_fetch();
    return;
  }
  compose_sprites();
  vm_fetch();
  return;
}



/* 0000:41b7  vm_op_4d */

void vm_op_4d(void)

{
  int iVar1;
  int iVar2;
  int iVar3;
  int unaff_BP;
  undefined2 unaff_SS;
  
  *(undefined1 *)(unaff_BP + 0xc1) = 0;
  iVar3 = 0x19;
  do {
    iVar1 = *(int *)(unaff_BP + 0x74) + 0x10;
    if (400 < iVar1) {
      iVar1 = *(int *)(unaff_BP + 0x74) + -0x180;
    }
    *(int *)(unaff_BP + 0x74) = iVar1;
    *(int *)(unaff_BP + 0x76) = 400 - iVar1;
    iVar1 = *(int *)(unaff_BP + 0x7e);
    iVar2 = *(int *)(unaff_BP + 0x7c) + -1;
    if (iVar1 <= iVar2) {
      iVar2 = iVar2 - iVar1;
    }
    if (iVar2 < 0) {
      iVar2 = iVar2 + iVar1;
    }
    *(int *)(unaff_BP + 0x7c) = iVar2;
    *(int *)(unaff_BP + 0x7a) = iVar2 << 4;
    scroll_row_update();
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  vm_fetch();
  return;
}



/* 0000:41fd  vm_op_4e */

void vm_op_4e(void)

{
  int iVar1;
  int unaff_BP;
  uint *puVar2;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  if (*(char *)(unaff_BP + 0xc1) != '\0') {
    vm_fetch();
    return;
  }
  puVar2 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
  iVar1 = 0x100;
  do {
    if (((*puVar2 & 0xff) != 0) && ((*puVar2 & 0x200) == 0)) {
      draw_entity();
    }
    puVar2 = puVar2 + 0x29;
    iVar1 = iVar1 + -1;
  } while (iVar1 != 0);
  vm_fetch();
  return;
}



/* 0000:422a  vm_op_4f */

void vm_op_4f(void)

{
  uint uVar1;
  int iVar2;
  uint *puVar3;
  int unaff_BP;
  int unaff_SI;
  uint *unaff_DI;
  uint *puVar4;
  uint *puVar5;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  uVar1 = *unaff_DI;
  puVar4 = unaff_DI + 1;
  puVar3 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
  iVar2 = 0x100;
  do {
    if (((((*puVar3 & uVar1) != 0) && ((*puVar3 & 0x100) == 0)) &&
        (*(int *)(unaff_SI + 0x42) + *(int *)(unaff_SI + 0x22) < (int)(puVar3[0x22] + puVar3[0x11]))
        ) && ((((int)(puVar3[0x21] + puVar3[0x11]) <=
                *(int *)(unaff_SI + 0x44) + *(int *)(unaff_SI + 0x22) &&
               (*(int *)(unaff_SI + 0x46) + *(int *)(unaff_SI + 0x24) <
                (int)(puVar3[0x24] + puVar3[0x12]))) &&
              ((0x1f < (int)(puVar3[0x24] + puVar3[0x12]) &&
               (puVar5 = puVar4,
               (int)(puVar3[0x23] + puVar3[0x12]) <=
               *(int *)(unaff_SI + 0x48) + *(int *)(unaff_SI + 0x24))))))) {
      while( true ) {
        if ((byte)*puVar5 == 0xff) break;
        (*(code *)*(undefined2 *)((byte)*puVar5 + 0x35d8))();
        puVar5 = (uint *)((int)puVar5 + 1);
      }
      *(int *)(unaff_SI + 0x3c) = *(int *)(unaff_SI + 0x3c) + -1;
    }
    puVar3 = puVar3 + 0x29;
    iVar2 = iVar2 + -1;
  } while (iVar2 != 0);
  while( true ) {
    if ((byte)*puVar4 == 0xff) break;
    puVar4 = (uint *)((byte *)((int)puVar4 + 1) +
                     *(int *)((undefined *)&DAT_0000_3604 + (byte)*puVar4));
  }
  *(undefined2 *)(unaff_SI + 8) = (byte *)((int)puVar4 + 1);
  vm_fetch();
  return;
}



/* 0000:42d1  vm_op_50 */

void vm_op_50(void)

{
  uint *puVar1;
  int iVar2;
  uint *puVar3;
  int unaff_BP;
  int unaff_SI;
  uint *unaff_DI;
  uint *puVar4;
  undefined2 unaff_ES;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  puVar4 = unaff_DI + 1;
  puVar3 = (uint *)*(undefined2 *)(unaff_BP + 0x52);
  iVar2 = 0x100;
  while (((((*puVar3 & 0x100) != 0 || ((*unaff_DI & *puVar3) == 0)) ||
          ((int)(puVar3[0x22] + puVar3[0x11]) <=
           *(int *)(unaff_SI + 0x42) + *(int *)(unaff_SI + 0x22))) ||
         (((*(int *)(unaff_SI + 0x44) + *(int *)(unaff_SI + 0x22) <
            (int)(puVar3[0x21] + puVar3[0x11]) ||
           ((int)(puVar3[0x24] + puVar3[0x12]) <=
            *(int *)(unaff_SI + 0x46) + *(int *)(unaff_SI + 0x24))) ||
          (((int)(puVar3[0x24] + puVar3[0x12]) < 0x20 ||
           (*(int *)(unaff_SI + 0x48) + *(int *)(unaff_SI + 0x24) <
            (int)(puVar3[0x23] + puVar3[0x12])))))))) {
    puVar3 = puVar3 + 0x29;
    iVar2 = iVar2 + -1;
    if (iVar2 == 0) {
      while( true ) {
        if ((byte)*puVar4 == 0xff) break;
        puVar4 = (uint *)((byte *)((int)puVar4 + 1) +
                         *(int *)((undefined *)&DAT_0000_3604 + (byte)*puVar4));
      }
      *(undefined2 *)(unaff_SI + 8) = (byte *)((int)puVar4 + 1);
      vm_fetch();
      return;
    }
  }
  *(int *)(unaff_SI + 0x3c) = *(int *)(unaff_SI + 0x3c) + -1;
  while( true ) {
    puVar1 = puVar4;
    puVar4 = (uint *)((int)puVar4 + 1);
    if ((byte)*puVar1 == 0xff) break;
    (*(code *)*(undefined2 *)((byte)*puVar1 + 0x35d8))();
  }
  *(undefined2 *)(unaff_SI + 8) = puVar4;
  vm_fetch();
  return;
}



/* 0000:4380  FUN_0000_4380 */

void __cdecl16near FUN_0000_4380(void)

{
  undefined1 in_DL;
  undefined1 in_DH;
  undefined1 *in_BX;
  
  *in_BX = in_DH;
  in_BX[2] = in_DL;
  in_BX[4] = 0x3a;
  in_BX[6] = 0x20;
  *(undefined2 *)(in_BX + 0x2000) = 0xe1;
  *(undefined2 *)(in_BX + 0x2002) = 0xe1;
  *(undefined2 *)(in_BX + 0x2004) = 0xe1;
  *(undefined2 *)(in_BX + 0x2006) = 0xe1;
  FUN_0000_4426();
  return;
}



/* 0000:43c0  FUN_0000_43c0 */

void __cdecl16near FUN_0000_43c0(void)

{
  undefined1 in_DL;
  undefined1 in_DH;
  undefined1 *in_BX;
  
  *in_BX = in_DH;
  in_BX[2] = in_DL;
  in_BX[4] = 0x3a;
  *(undefined2 *)(in_BX + 0x2000) = 0xe1;
  *(undefined2 *)(in_BX + 0x2002) = 0xe1;
  *(undefined2 *)(in_BX + 0x2004) = 0xe1;
  FUN_0000_44af();
  return;
}



/* 0000:4426  FUN_0000_4426 */

undefined4 __cdecl16near FUN_0000_4426(void)

{
  byte bVar1;
  uint in_AX;
  undefined2 in_DX;
  uint uVar2;
  char *in_BX;
  
  bVar1 = 0;
  for (uVar2 = in_AX; 0xfff < uVar2; uVar2 = uVar2 - 0x1000) {
    bVar1 = bVar1 + 1;
  }
  if (9 < bVar1) {
    bVar1 = bVar1 + 7;
  }
  *in_BX = bVar1 + 0x30;
  (in_BX + 0x2000)[0] = -0x1f;
  (in_BX + 0x2000)[1] = '\0';
  bVar1 = 0;
  for (; 0xff < uVar2; uVar2 = uVar2 - 0x100) {
    bVar1 = bVar1 + 1;
  }
  if (9 < bVar1) {
    bVar1 = bVar1 + 7;
  }
  in_BX[2] = bVar1 + 0x30;
  (in_BX + 0x2002)[0] = -0x1f;
  (in_BX + 0x2002)[1] = '\0';
  bVar1 = 0;
  for (; 0xf < uVar2; uVar2 = uVar2 - 0x10) {
    bVar1 = bVar1 + 1;
  }
  if (9 < bVar1) {
    bVar1 = bVar1 + 7;
  }
  in_BX[4] = bVar1 + 0x30;
  (in_BX + 0x2004)[0] = -0x1f;
  (in_BX + 0x2004)[1] = '\0';
  bVar1 = (byte)uVar2;
  if (9 < bVar1) {
    bVar1 = bVar1 + 7;
  }
  in_BX[6] = bVar1 + 0x30;
  (in_BX + 0x2006)[0] = -0x1f;
  (in_BX + 0x2006)[1] = '\0';
  return CONCAT22(in_DX,in_AX);
}



/* 0000:44af  FUN_0000_44af */

undefined4 __cdecl16near FUN_0000_44af(void)

{
  char cVar1;
  uint in_AX;
  undefined2 in_DX;
  uint uVar2;
  char *in_BX;
  
  cVar1 = '0';
  for (uVar2 = in_AX; 9999 < uVar2; uVar2 = uVar2 + 0xd8f0) {
    cVar1 = cVar1 + '\x01';
  }
  *in_BX = cVar1;
  (in_BX + 0x2000)[0] = -0x1f;
  (in_BX + 0x2000)[1] = '\0';
  cVar1 = '0';
  for (; 999 < uVar2; uVar2 = uVar2 - 1000) {
    cVar1 = cVar1 + '\x01';
  }
  in_BX[2] = cVar1;
  (in_BX + 0x2002)[0] = -0x1f;
  (in_BX + 0x2002)[1] = '\0';
  cVar1 = '0';
  for (; 99 < uVar2; uVar2 = uVar2 - 100) {
    cVar1 = cVar1 + '\x01';
  }
  in_BX[4] = cVar1;
  (in_BX + 0x2004)[0] = -0x1f;
  (in_BX + 0x2004)[1] = '\0';
  cVar1 = '0';
  for (; 9 < uVar2; uVar2 = uVar2 - 10) {
    cVar1 = cVar1 + '\x01';
  }
  in_BX[6] = cVar1;
  (in_BX + 0x2006)[0] = -0x1f;
  (in_BX + 0x2006)[1] = '\0';
  in_BX[8] = (char)uVar2 + '0';
  (in_BX + 0x2008)[0] = -0x1f;
  (in_BX + 0x2008)[1] = '\0';
  return CONCAT22(in_DX,in_AX);
}



/* 0000:481b  FUN_0000_481b */

undefined2 __cdecl16near FUN_0000_481b(void)

{
  undefined2 *puVar1;
  undefined1 uVar2;
  byte bVar4;
  uint uVar3;
  byte in_CL;
  byte in_CH;
  char cVar5;
  undefined2 *unaff_SI;
  undefined2 *unaff_DI;
  undefined2 *puVar6;
  undefined2 *puVar7;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  if (in_CH != 2) {
    out(0x7c,0xc0);
    uVar2 = 0;
    out(0x7e,0);
    out(0x7e,0);
    out(0x7e,0);
    if ((in_CH & 1) == 0) {
      uVar2 = 0xff;
    }
    out(0x7e,uVar2);
    if (0x7800 < (int)unaff_DI) {
      cVar5 = '\x10';
      do {
        puVar7 = unaff_DI;
        if ((undefined2 *)0x7cff < unaff_DI) {
          puVar7 = unaff_DI + -16000;
        }
        puVar1 = unaff_SI;
        unaff_SI = unaff_SI + 1;
        bVar4 = (byte)((uint)*puVar1 >> 8);
        uVar3 = CONCAT11((char)*puVar1,bVar4) >> (in_CL & 0x1f);
        *puVar7 = CONCAT11((char)uVar3,(char)(uVar3 >> 8));
        *(char *)(puVar7 + 1) = (char)(((uint)bVar4 << 8) >> (in_CL & 0x1f));
        unaff_DI = puVar7 + 0x28;
        cVar5 = cVar5 + -1;
      } while (cVar5 != '\0');
      puVar7 = puVar7 + 0x3c28;
      out(0x7e,0xff);
      out(0x7e,0xff);
      out(0x7e,0xff);
      out(0x7c,0xce);
      cVar5 = '\x10';
      do {
        puVar6 = puVar7;
        if ((undefined2 *)0x7cff < puVar7) {
          puVar6 = puVar7 + -16000;
        }
        puVar1 = unaff_SI;
        unaff_SI = unaff_SI + 1;
        bVar4 = (byte)((uint)*puVar1 >> 8);
        uVar3 = CONCAT11((char)*puVar1,bVar4) >> (in_CL & 0x1f);
        *puVar6 = CONCAT11((char)uVar3,(char)(uVar3 >> 8));
        *(char *)(puVar6 + 1) = (char)(((uint)bVar4 << 8) >> (in_CL & 0x1f));
        puVar7 = puVar6 + 0x28;
        cVar5 = cVar5 + -1;
      } while (cVar5 != '\0');
      puVar6 = puVar6 + 0x3c28;
      out(0x7c,0xcd);
      cVar5 = '\x10';
      do {
        puVar7 = puVar6;
        if ((undefined2 *)0x7cff < puVar6) {
          puVar7 = puVar6 + -16000;
        }
        puVar1 = unaff_SI;
        unaff_SI = unaff_SI + 1;
        bVar4 = (byte)((uint)*puVar1 >> 8);
        uVar3 = CONCAT11((char)*puVar1,bVar4) >> (in_CL & 0x1f);
        *puVar7 = CONCAT11((char)uVar3,(char)(uVar3 >> 8));
        *(char *)(puVar7 + 1) = (char)(((uint)bVar4 << 8) >> (in_CL & 0x1f));
        puVar6 = puVar7 + 0x28;
        cVar5 = cVar5 + -1;
      } while (cVar5 != '\0');
      puVar7 = puVar7 + 0x3c28;
      out(0x7c,0xcb);
      cVar5 = '\x10';
      do {
        if ((undefined2 *)0x7cff < puVar7) {
          puVar7 = puVar7 + -16000;
        }
        puVar1 = unaff_SI;
        unaff_SI = unaff_SI + 1;
        bVar4 = (byte)((uint)*puVar1 >> 8);
        uVar3 = CONCAT11((char)*puVar1,bVar4) >> (in_CL & 0x1f);
        *puVar7 = CONCAT11((char)uVar3,(char)(uVar3 >> 8));
        *(char *)(puVar7 + 1) = (char)(((uint)bVar4 << 8) >> (in_CL & 0x1f));
        puVar7 = puVar7 + 0x28;
        cVar5 = cVar5 + -1;
      } while (cVar5 != '\0');
      out(0x7c,0);
      return 0;
    }
    if (in_CL != 0) {
      cVar5 = '\x10';
      do {
        puVar7 = unaff_DI;
        puVar1 = unaff_SI;
        unaff_SI = unaff_SI + 1;
        bVar4 = (byte)((uint)*puVar1 >> 8);
        uVar3 = CONCAT11((char)*puVar1,bVar4) >> (in_CL & 0x1f);
        *puVar7 = CONCAT11((char)uVar3,(char)(uVar3 >> 8));
        *(char *)(puVar7 + 1) = (char)(((uint)bVar4 << 8) >> (in_CL & 0x1f));
        cVar5 = cVar5 + -1;
        unaff_DI = puVar7 + 0x28;
      } while (cVar5 != '\0');
      out(0x7e,0xff);
      out(0x7e,0xff);
      out(0x7e,0xff);
      out(0x7c,0xce);
      cVar5 = '\x10';
      puVar7 = puVar7 + -600;
      do {
        puVar6 = puVar7;
        puVar1 = unaff_SI;
        unaff_SI = unaff_SI + 1;
        bVar4 = (byte)((uint)*puVar1 >> 8);
        uVar3 = CONCAT11((char)*puVar1,bVar4) >> (in_CL & 0x1f);
        *puVar6 = CONCAT11((char)uVar3,(char)(uVar3 >> 8));
        *(char *)(puVar6 + 1) = (char)(((uint)bVar4 << 8) >> (in_CL & 0x1f));
        cVar5 = cVar5 + -1;
        puVar7 = puVar6 + 0x28;
      } while (cVar5 != '\0');
      out(0x7c,0xcd);
      cVar5 = '\x10';
      puVar7 = puVar6 + -600;
      do {
        puVar6 = puVar7;
        puVar1 = unaff_SI;
        unaff_SI = unaff_SI + 1;
        bVar4 = (byte)((uint)*puVar1 >> 8);
        uVar3 = CONCAT11((char)*puVar1,bVar4) >> (in_CL & 0x1f);
        *puVar6 = CONCAT11((char)uVar3,(char)(uVar3 >> 8));
        *(char *)(puVar6 + 1) = (char)(((uint)bVar4 << 8) >> (in_CL & 0x1f));
        cVar5 = cVar5 + -1;
        puVar7 = puVar6 + 0x28;
      } while (cVar5 != '\0');
      puVar6 = puVar6 + -600;
      out(0x7c,0xcb);
      cVar5 = '\x10';
      do {
        puVar1 = unaff_SI;
        unaff_SI = unaff_SI + 1;
        bVar4 = (byte)((uint)*puVar1 >> 8);
        uVar3 = CONCAT11((char)*puVar1,bVar4) >> (in_CL & 0x1f);
        *puVar6 = CONCAT11((char)uVar3,(char)(uVar3 >> 8));
        *(char *)(puVar6 + 1) = (char)(((uint)bVar4 << 8) >> (in_CL & 0x1f));
        puVar6 = puVar6 + 0x28;
        cVar5 = cVar5 + -1;
      } while (cVar5 != '\0');
      out(0x7c,0);
      return 0;
    }
    *unaff_DI = *unaff_SI;
    unaff_DI[0x28] = unaff_SI[1];
    unaff_DI[0x50] = unaff_SI[2];
    unaff_DI[0x78] = unaff_SI[3];
    unaff_DI[0xa0] = unaff_SI[4];
    unaff_DI[200] = unaff_SI[5];
    unaff_DI[0xf0] = unaff_SI[6];
    unaff_DI[0x118] = unaff_SI[7];
    unaff_DI[0x140] = unaff_SI[8];
    unaff_DI[0x168] = unaff_SI[9];
    unaff_DI[400] = unaff_SI[10];
    unaff_DI[0x1b8] = unaff_SI[0xb];
    unaff_DI[0x1e0] = unaff_SI[0xc];
    unaff_DI[0x208] = unaff_SI[0xd];
    unaff_DI[0x230] = unaff_SI[0xe];
    unaff_DI[600] = unaff_SI[0xf];
    out(0x7e,0xff);
    out(0x7e,0xff);
    out(0x7e,0xff);
    out(0x7c,0xce);
    *unaff_DI = unaff_SI[0x10];
    unaff_DI[0x28] = unaff_SI[0x11];
    unaff_DI[0x50] = unaff_SI[0x12];
    unaff_DI[0x78] = unaff_SI[0x13];
    unaff_DI[0xa0] = unaff_SI[0x14];
    unaff_DI[200] = unaff_SI[0x15];
    unaff_DI[0xf0] = unaff_SI[0x16];
    unaff_DI[0x118] = unaff_SI[0x17];
    unaff_DI[0x140] = unaff_SI[0x18];
    unaff_DI[0x168] = unaff_SI[0x19];
    unaff_DI[400] = unaff_SI[0x1a];
    unaff_DI[0x1b8] = unaff_SI[0x1b];
    unaff_DI[0x1e0] = unaff_SI[0x1c];
    unaff_DI[0x208] = unaff_SI[0x1d];
    unaff_DI[0x230] = unaff_SI[0x1e];
    unaff_DI[600] = unaff_SI[0x1f];
    out(0x7c,0xcd);
    *unaff_DI = unaff_SI[0x20];
    unaff_DI[0x28] = unaff_SI[0x21];
    unaff_DI[0x50] = unaff_SI[0x22];
    unaff_DI[0x78] = unaff_SI[0x23];
    unaff_DI[0xa0] = unaff_SI[0x24];
    unaff_DI[200] = unaff_SI[0x25];
    unaff_DI[0xf0] = unaff_SI[0x26];
    unaff_DI[0x118] = unaff_SI[0x27];
    unaff_DI[0x140] = unaff_SI[0x28];
    unaff_DI[0x168] = unaff_SI[0x29];
    unaff_DI[400] = unaff_SI[0x2a];
    unaff_DI[0x1b8] = unaff_SI[0x2b];
    unaff_DI[0x1e0] = unaff_SI[0x2c];
    unaff_DI[0x208] = unaff_SI[0x2d];
    unaff_DI[0x230] = unaff_SI[0x2e];
    unaff_DI[600] = unaff_SI[0x2f];
    out(0x7c,0xcb);
    *unaff_DI = unaff_SI[0x30];
    unaff_DI[0x28] = unaff_SI[0x31];
    unaff_DI[0x50] = unaff_SI[0x32];
    unaff_DI[0x78] = unaff_SI[0x33];
    unaff_DI[0xa0] = unaff_SI[0x34];
    unaff_DI[200] = unaff_SI[0x35];
    unaff_DI[0xf0] = unaff_SI[0x36];
    unaff_DI[0x118] = unaff_SI[0x37];
    unaff_DI[0x140] = unaff_SI[0x38];
    unaff_DI[0x168] = unaff_SI[0x39];
    unaff_DI[400] = unaff_SI[0x3a];
    unaff_DI[0x1b8] = unaff_SI[0x3b];
    unaff_DI[0x1e0] = unaff_SI[0x3c];
    unaff_DI[0x208] = unaff_SI[0x3d];
    unaff_DI[0x230] = unaff_SI[0x3e];
    unaff_DI[600] = unaff_SI[0x3f];
    out(0x7c,0);
    return 0;
  }
  out(0x7c,0xc0);
  out(0x7e,0xff);
  out(0x7e,0xff);
  out(0x7e,0);
  out(0x7e,0xff);
  if (0x7800 < (int)unaff_DI) {
    cVar5 = '\x10';
    do {
      if ((undefined2 *)0x7cff < unaff_DI) {
        unaff_DI = unaff_DI + -16000;
      }
      puVar1 = unaff_SI;
      unaff_SI = unaff_SI + 1;
      bVar4 = (byte)((uint)*puVar1 >> 8);
      uVar3 = CONCAT11((char)*puVar1,bVar4) >> (in_CL & 0x1f);
      *unaff_DI = CONCAT11((char)uVar3,(char)(uVar3 >> 8));
      *(char *)(unaff_DI + 1) = (char)(((uint)bVar4 << 8) >> (in_CL & 0x1f));
      unaff_DI = unaff_DI + 0x28;
      cVar5 = cVar5 + -1;
    } while (cVar5 != '\0');
    out(0x7c,0);
    return 0;
  }
  if (in_CL == 0) {
    *unaff_DI = *unaff_SI;
    unaff_DI[0x28] = unaff_SI[1];
    unaff_DI[0x50] = unaff_SI[2];
    unaff_DI[0x78] = unaff_SI[3];
    unaff_DI[0xa0] = unaff_SI[4];
    unaff_DI[200] = unaff_SI[5];
    unaff_DI[0xf0] = unaff_SI[6];
    unaff_DI[0x118] = unaff_SI[7];
    unaff_DI[0x140] = unaff_SI[8];
    unaff_DI[0x168] = unaff_SI[9];
    unaff_DI[400] = unaff_SI[10];
    unaff_DI[0x1b8] = unaff_SI[0xb];
    unaff_DI[0x1e0] = unaff_SI[0xc];
    unaff_DI[0x208] = unaff_SI[0xd];
    unaff_DI[0x230] = unaff_SI[0xe];
    unaff_DI[600] = unaff_SI[0xf];
    out(0x7c,0);
    return 0;
  }
  cVar5 = '\x10';
  do {
    puVar1 = unaff_SI;
    unaff_SI = unaff_SI + 1;
    bVar4 = (byte)((uint)*puVar1 >> 8);
    uVar3 = CONCAT11((char)*puVar1,bVar4) >> (in_CL & 0x1f);
    *unaff_DI = CONCAT11((char)uVar3,(char)(uVar3 >> 8));
    *(char *)(unaff_DI + 1) = (char)(((uint)bVar4 << 8) >> (in_CL & 0x1f));
    unaff_DI = unaff_DI + 0x28;
    cVar5 = cVar5 + -1;
  } while (cVar5 != '\0');
  out(0x7c,0);
  return 0;
}



/* 0000:4b21  FUN_0000_4b21 */

void __cdecl16near FUN_0000_4b21(void)

{
  undefined2 *unaff_SI;
  undefined2 *unaff_DI;
  undefined2 unaff_DS;
  
  *unaff_DI = *unaff_SI;
  unaff_DI[0x28] = unaff_SI[1];
  unaff_DI[0x50] = unaff_SI[2];
  unaff_DI[0x78] = unaff_SI[3];
  unaff_DI[0xa0] = unaff_SI[4];
  unaff_DI[200] = unaff_SI[5];
  unaff_DI[0xf0] = unaff_SI[6];
  unaff_DI[0x118] = unaff_SI[7];
  unaff_DI[0x140] = unaff_SI[8];
  unaff_DI[0x168] = unaff_SI[9];
  unaff_DI[400] = unaff_SI[10];
  unaff_DI[0x1b8] = unaff_SI[0xb];
  unaff_DI[0x1e0] = unaff_SI[0xc];
  unaff_DI[0x208] = unaff_SI[0xd];
  unaff_DI[0x230] = unaff_SI[0xe];
  unaff_DI[600] = unaff_SI[0xf];
  *unaff_DI = unaff_SI[0x10];
  unaff_DI[0x28] = unaff_SI[0x11];
  unaff_DI[0x50] = unaff_SI[0x12];
  unaff_DI[0x78] = unaff_SI[0x13];
  unaff_DI[0xa0] = unaff_SI[0x14];
  unaff_DI[200] = unaff_SI[0x15];
  unaff_DI[0xf0] = unaff_SI[0x16];
  unaff_DI[0x118] = unaff_SI[0x17];
  unaff_DI[0x140] = unaff_SI[0x18];
  unaff_DI[0x168] = unaff_SI[0x19];
  unaff_DI[400] = unaff_SI[0x1a];
  unaff_DI[0x1b8] = unaff_SI[0x1b];
  unaff_DI[0x1e0] = unaff_SI[0x1c];
  unaff_DI[0x208] = unaff_SI[0x1d];
  unaff_DI[0x230] = unaff_SI[0x1e];
  unaff_DI[600] = unaff_SI[0x1f];
  unaff_DI[-0x4000] = unaff_SI[0x20];
  unaff_DI[-0x3fd8] = unaff_SI[0x21];
  unaff_DI[-0x3fb0] = unaff_SI[0x22];
  unaff_DI[-0x3f88] = unaff_SI[0x23];
  unaff_DI[-0x3f60] = unaff_SI[0x24];
  unaff_DI[-0x3f38] = unaff_SI[0x25];
  unaff_DI[-0x3f10] = unaff_SI[0x26];
  unaff_DI[-0x3ee8] = unaff_SI[0x27];
  unaff_DI[-0x3ec0] = unaff_SI[0x28];
  unaff_DI[-0x3e98] = unaff_SI[0x29];
  unaff_DI[-0x3e70] = unaff_SI[0x2a];
  unaff_DI[-0x3e48] = unaff_SI[0x2b];
  unaff_DI[-0x3e20] = unaff_SI[0x2c];
  unaff_DI[-0x3df8] = unaff_SI[0x2d];
  unaff_DI[-0x3dd0] = unaff_SI[0x2e];
  unaff_DI[-0x3da8] = unaff_SI[0x2f];
  *unaff_DI = 0;
  unaff_DI[0x28] = 0;
  unaff_DI[0x50] = 0;
  unaff_DI[0x78] = 0;
  unaff_DI[0xa0] = 0;
  unaff_DI[200] = 0;
  unaff_DI[0xf0] = 0;
  unaff_DI[0x118] = 0;
  unaff_DI[0x140] = 0;
  unaff_DI[0x168] = 0;
  unaff_DI[400] = 0;
  unaff_DI[0x1b8] = 0;
  unaff_DI[0x1e0] = 0;
  unaff_DI[0x208] = 0;
  unaff_DI[0x230] = 0;
  unaff_DI[600] = 0;
  return;
}


