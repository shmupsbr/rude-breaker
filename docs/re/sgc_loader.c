/*
 * Ghidra 12.1.4 headless decompile.
 * Program: sgc_loader.bin
 * Language: x86:LE:16:Real Mode
 * Image base: 0x100 (.COM load address)
 * Pseudo-C is a guide. Trust docs/re/MAP.md where they disagree.
 */

/* 0000:0100  com_entry */

/* WARNING: Stack frame is not setup normally: Input value of stackpointer is not used */
/* WARNING: This function may have set the stack pointer */
/* WARNING: Removing unreachable block (ram,0x00005851) */

void com_entry(void)

{
  undefined2 *puVar1;
  undefined2 uVar2;
  code *pcVar3;
  code *pcVar4;
  int iVar5;
  undefined2 uVar6;
  uint uVar7;
  undefined2 *puVar8;
  undefined2 *puVar9;
  undefined2 *puVar10;
  undefined2 *puVar11;
  undefined2 *puVar12;
  undefined2 unaff_CS;
  undefined2 uVar13;
  bool bVar14;
  undefined1 uVar15;
  
  *(undefined2 *)0x348 = 0x5846;
  init_bp_pointers();
  puVar8 = (undefined2 *)0x34a;
  bVar14 = true;
  pcVar4 = (code *)swi(0x21);
  (*pcVar4)();
  if (bVar14) goto LAB_0000_59ff;
  pcVar4 = (code *)swi(0x21);
  (*pcVar4)();
  uVar7 = 0xa000;
  pcVar4 = (code *)swi(0x21);
  iVar5 = (*pcVar4)();
  if (iVar5 != 8) goto LAB_0000_59ff;
  *(uint *)0x3b6 = uVar7;
  uVar15 = uVar7 < 0x69c0;
  puVar12 = puVar8;
  if ((bool)uVar15) goto LAB_0000_59b3;
  pcVar4 = (code *)swi(0x21);
  (*pcVar4)();
  *(undefined2 *)((int)puVar8 + -2) = 0x588c;
  FUN_0000_60a2();
  *(undefined2 *)((int)puVar8 + -2) = 0x5895;
  dos_exec();
  if ((bool)uVar15) {
    *(undefined2 *)((int)puVar8 + -2) = 0x589a;
    FUN_0000_5a25();
  }
  else {
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
  }
  *(undefined2 *)((int)puVar8 + -2) = 0x58a9;
  dos_exec();
  if ((bool)uVar15) {
    *(undefined2 *)((int)puVar8 + -2) = 0x58ae;
    FUN_0000_5a25();
  }
  else {
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
  }
  *(undefined2 *)((int)puVar8 + -2) = 0x58b7;
  FUN_0000_63ce();
  *(undefined2 *)((int)puVar8 + -2) = 0x58ba;
  FUN_0000_6147();
  pcVar4 = (code *)swi(0x21);
  uVar6 = (*pcVar4)();
  if ((bool)uVar15) goto LAB_0000_59ff;
  *(undefined2 *)0x354 = uVar6;
  pcVar4 = (code *)swi(0x21);
  uVar6 = (*pcVar4)();
  if ((bool)uVar15) goto LAB_0000_59ff;
  *(undefined2 *)0x356 = uVar6;
  pcVar4 = (code *)swi(0x21);
  uVar6 = (*pcVar4)();
  if ((bool)uVar15) goto LAB_0000_59ff;
  *(undefined2 *)0x35e = uVar6;
  *(undefined2 *)0x3ac = 2;
  *(undefined2 *)0x3ae = 2;
  *(undefined2 *)((int)puVar8 + -2) = 0x58f4;
  FUN_0000_5f20();
  uVar6 = unaff_CS;
  uVar13 = unaff_CS;
  while( true ) {
    *(undefined2 *)((int)puVar8 + -2) = uVar6;
    uVar6 = *(undefined2 *)0x356;
    puVar12 = (undefined2 *)&DAT_0000_1800;
    bVar14 = false;
    for (iVar5 = 0x400; iVar5 != 0; iVar5 = iVar5 + -1) {
      puVar1 = puVar12;
      puVar12 = puVar12 + 1;
      *puVar1 = 0;
    }
    uVar6 = *(undefined2 *)((int)puVar8 + -2);
    *(undefined2 *)0x358 = (code *)sgm_load;
    *(undefined2 *)((int)puVar8 + -2) = uVar13;
    uVar2 = ((undefined2 *)0x358)[1];
    pcVar3 = (code *)*(undefined2 *)0x358;
    *(undefined2 *)((int)puVar8 + -4) = unaff_CS;
    puVar9 = (undefined2 *)((int)puVar8 + -6);
    *(undefined2 *)((int)puVar8 + -6) = 0x5919;
    (*pcVar3)();
    uVar13 = *puVar9;
    puVar8 = puVar9 + 1;
    puVar12 = puVar9 + 1;
    puVar10 = puVar9 + 1;
    if (bVar14) break;
    *puVar9 = 0x591f;
    FUN_0000_5aff();
    if ((*(byte *)&DAT_0000_0405 & 0x80) == 0) break;
    *puVar9 = 0x5929;
    FUN_0000_5f58();
  }
  uVar15 = 0;
  if ((*(byte *)&DAT_0000_0405 & 3) != 0) {
    pcVar4 = (code *)swi(0x7f);
    (*pcVar4)();
    *(undefined2 *)0x358 = (code *)frame_hook_5ef5;
    puVar12 = puVar10;
    for (iVar5 = 0x15; uVar15 = 0, iVar5 != 0; iVar5 = iVar5 + -1) {
      uVar6 = ((undefined2 *)0x358)[1];
      pcVar3 = (code *)*(undefined2 *)0x358;
      *(undefined2 *)((int)puVar12 + -2) = unaff_CS;
      puVar11 = (undefined2 *)((int)puVar12 + -4);
      puVar12 = (undefined2 *)((int)puVar12 + -4);
      *puVar11 = 0x594d;
      (*pcVar3)();
    }
    pcVar4 = (code *)swi(0x7f);
    (*pcVar4)();
  }
  *(undefined2 *)((int)puVar12 + -2) = 0x5957;
  FUN_0000_5f75();
  pcVar4 = (code *)swi(0x21);
  (*pcVar4)();
  if (!(bool)uVar15) {
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
    if (!(bool)uVar15) {
      pcVar4 = (code *)swi(0x21);
      (*pcVar4)();
      if (!(bool)uVar15) goto LAB_0000_5972;
    }
  }
  pcVar4 = (code *)swi(0x21);
  (*pcVar4)();
  do {
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
    *(undefined2 *)0x358 = *(undefined2 *)0x38a;
    uVar6 = ((undefined2 *)0x358)[1];
    pcVar3 = (code *)*(undefined2 *)0x358;
    *(undefined2 *)((int)puVar12 + -2) = unaff_CS;
    puVar8 = (undefined2 *)((int)puVar12 + -4);
    *(undefined2 *)((int)puVar12 + -4) = 0x59fa;
    (*pcVar3)();
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
LAB_0000_59ff:
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
    *(undefined2 *)0x358 = *(undefined2 *)0x38a;
    uVar6 = ((undefined2 *)0x358)[1];
    pcVar3 = (code *)*(undefined2 *)0x358;
    *(undefined2 *)((int)puVar8 + -2) = unaff_CS;
    puVar12 = (undefined2 *)((int)puVar8 + -4);
    *(undefined2 *)((int)puVar8 + -4) = 0x5a22;
    (*pcVar3)();
LAB_0000_5972:
    *(undefined2 *)((int)puVar12 + -2) = 0x5975;
    FUN_0000_60a2();
    bVar14 = false;
    if ((*(byte *)&DAT_0000_0405 & 1) != 0) {
      *(undefined2 *)((int)puVar12 + -2) = 0x5985;
      dos_exec();
      if (bVar14) {
        *(undefined2 *)((int)puVar12 + -2) = 0x598a;
        FUN_0000_5a25();
      }
      else {
        pcVar4 = (code *)swi(0x21);
        (*pcVar4)();
      }
    }
    bVar14 = false;
    if ((*(byte *)&DAT_0000_0405 & 8) != 0) {
      *(undefined2 *)((int)puVar12 + -2) = 0x59a0;
      dos_exec();
      if (bVar14) {
        *(undefined2 *)((int)puVar12 + -2) = 0x59a5;
        FUN_0000_5a25();
      }
      else {
        pcVar4 = (code *)swi(0x21);
        (*pcVar4)();
      }
    }
    *(undefined2 *)((int)puVar12 + -2) = 0x59ae;
    FUN_0000_6147();
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
LAB_0000_59b3:
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
    *(undefined2 *)((int)puVar12 + -2) = 0x59bd;
    FUN_0000_5be1();
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
    *(undefined2 *)((int)puVar12 + -2) = 0x59c7;
    FUN_0000_5c20();
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
  } while( true );
}



/* 0000:5834  sgc_startup */

/* WARNING: Stack frame is not setup normally: Input value of stackpointer is not used */
/* WARNING: This function may have set the stack pointer */
/* WARNING: Removing unreachable block (ram,0x00005851) */

void sgc_startup(void)

{
  undefined2 *puVar1;
  undefined2 uVar2;
  code *pcVar3;
  code *pcVar4;
  int iVar5;
  undefined2 uVar6;
  uint uVar7;
  undefined2 *puVar8;
  undefined2 *puVar9;
  undefined2 *puVar10;
  undefined2 *puVar11;
  undefined2 *puVar12;
  undefined2 unaff_CS;
  undefined2 uVar13;
  bool bVar14;
  undefined1 uVar15;
  
  *(undefined2 *)0x348 = 0x5846;
  init_bp_pointers();
  puVar8 = (undefined2 *)0x34a;
  bVar14 = true;
  pcVar4 = (code *)swi(0x21);
  (*pcVar4)();
  if (bVar14) goto LAB_0000_59ff;
  pcVar4 = (code *)swi(0x21);
  (*pcVar4)();
  uVar7 = 0xa000;
  pcVar4 = (code *)swi(0x21);
  iVar5 = (*pcVar4)();
  if (iVar5 != 8) goto LAB_0000_59ff;
  *(uint *)0x3b6 = uVar7;
  uVar15 = uVar7 < 0x69c0;
  puVar12 = puVar8;
  if ((bool)uVar15) goto LAB_0000_59b3;
  pcVar4 = (code *)swi(0x21);
  (*pcVar4)();
  *(undefined2 *)((int)puVar8 + -2) = 0x588c;
  FUN_0000_60a2();
  *(undefined2 *)((int)puVar8 + -2) = 0x5895;
  dos_exec();
  if ((bool)uVar15) {
    *(undefined2 *)((int)puVar8 + -2) = 0x589a;
    FUN_0000_5a25();
  }
  else {
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
  }
  *(undefined2 *)((int)puVar8 + -2) = 0x58a9;
  dos_exec();
  if ((bool)uVar15) {
    *(undefined2 *)((int)puVar8 + -2) = 0x58ae;
    FUN_0000_5a25();
  }
  else {
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
  }
  *(undefined2 *)((int)puVar8 + -2) = 0x58b7;
  FUN_0000_63ce();
  *(undefined2 *)((int)puVar8 + -2) = 0x58ba;
  FUN_0000_6147();
  pcVar4 = (code *)swi(0x21);
  uVar6 = (*pcVar4)();
  if ((bool)uVar15) goto LAB_0000_59ff;
  *(undefined2 *)0x354 = uVar6;
  pcVar4 = (code *)swi(0x21);
  uVar6 = (*pcVar4)();
  if ((bool)uVar15) goto LAB_0000_59ff;
  *(undefined2 *)0x356 = uVar6;
  pcVar4 = (code *)swi(0x21);
  uVar6 = (*pcVar4)();
  if ((bool)uVar15) goto LAB_0000_59ff;
  *(undefined2 *)0x35e = uVar6;
  *(undefined2 *)0x3ac = 2;
  *(undefined2 *)0x3ae = 2;
  *(undefined2 *)((int)puVar8 + -2) = 0x58f4;
  FUN_0000_5f20();
  uVar6 = unaff_CS;
  uVar13 = unaff_CS;
  while( true ) {
    *(undefined2 *)((int)puVar8 + -2) = uVar6;
    uVar6 = *(undefined2 *)0x356;
    puVar12 = (undefined2 *)&DAT_0000_1800;
    bVar14 = false;
    for (iVar5 = 0x400; iVar5 != 0; iVar5 = iVar5 + -1) {
      puVar1 = puVar12;
      puVar12 = puVar12 + 1;
      *puVar1 = 0;
    }
    uVar6 = *(undefined2 *)((int)puVar8 + -2);
    *(undefined2 *)0x358 = (code *)sgm_load;
    *(undefined2 *)((int)puVar8 + -2) = uVar13;
    uVar2 = ((undefined2 *)0x358)[1];
    pcVar3 = (code *)*(undefined2 *)0x358;
    *(undefined2 *)((int)puVar8 + -4) = unaff_CS;
    puVar9 = (undefined2 *)((int)puVar8 + -6);
    *(undefined2 *)((int)puVar8 + -6) = 0x5919;
    (*pcVar3)();
    uVar13 = *puVar9;
    puVar8 = puVar9 + 1;
    puVar12 = puVar9 + 1;
    puVar10 = puVar9 + 1;
    if (bVar14) break;
    *puVar9 = 0x591f;
    FUN_0000_5aff();
    if ((*(byte *)&DAT_0000_0405 & 0x80) == 0) break;
    *puVar9 = 0x5929;
    FUN_0000_5f58();
  }
  uVar15 = 0;
  if ((*(byte *)&DAT_0000_0405 & 3) != 0) {
    pcVar4 = (code *)swi(0x7f);
    (*pcVar4)();
    *(undefined2 *)0x358 = (code *)frame_hook_5ef5;
    puVar12 = puVar10;
    for (iVar5 = 0x15; uVar15 = 0, iVar5 != 0; iVar5 = iVar5 + -1) {
      uVar6 = ((undefined2 *)0x358)[1];
      pcVar3 = (code *)*(undefined2 *)0x358;
      *(undefined2 *)((int)puVar12 + -2) = unaff_CS;
      puVar11 = (undefined2 *)((int)puVar12 + -4);
      puVar12 = (undefined2 *)((int)puVar12 + -4);
      *puVar11 = 0x594d;
      (*pcVar3)();
    }
    pcVar4 = (code *)swi(0x7f);
    (*pcVar4)();
  }
  *(undefined2 *)((int)puVar12 + -2) = 0x5957;
  FUN_0000_5f75();
  pcVar4 = (code *)swi(0x21);
  (*pcVar4)();
  if (!(bool)uVar15) {
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
    if (!(bool)uVar15) {
      pcVar4 = (code *)swi(0x21);
      (*pcVar4)();
      if (!(bool)uVar15) goto LAB_0000_5972;
    }
  }
  pcVar4 = (code *)swi(0x21);
  (*pcVar4)();
  do {
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
    *(undefined2 *)0x358 = *(undefined2 *)0x38a;
    uVar6 = ((undefined2 *)0x358)[1];
    pcVar3 = (code *)*(undefined2 *)0x358;
    *(undefined2 *)((int)puVar12 + -2) = unaff_CS;
    puVar8 = (undefined2 *)((int)puVar12 + -4);
    *(undefined2 *)((int)puVar12 + -4) = 0x59fa;
    (*pcVar3)();
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
LAB_0000_59ff:
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
    *(undefined2 *)0x358 = *(undefined2 *)0x38a;
    uVar6 = ((undefined2 *)0x358)[1];
    pcVar3 = (code *)*(undefined2 *)0x358;
    *(undefined2 *)((int)puVar8 + -2) = unaff_CS;
    puVar12 = (undefined2 *)((int)puVar8 + -4);
    *(undefined2 *)((int)puVar8 + -4) = 0x5a22;
    (*pcVar3)();
LAB_0000_5972:
    *(undefined2 *)((int)puVar12 + -2) = 0x5975;
    FUN_0000_60a2();
    bVar14 = false;
    if ((*(byte *)&DAT_0000_0405 & 1) != 0) {
      *(undefined2 *)((int)puVar12 + -2) = 0x5985;
      dos_exec();
      if (bVar14) {
        *(undefined2 *)((int)puVar12 + -2) = 0x598a;
        FUN_0000_5a25();
      }
      else {
        pcVar4 = (code *)swi(0x21);
        (*pcVar4)();
      }
    }
    bVar14 = false;
    if ((*(byte *)&DAT_0000_0405 & 8) != 0) {
      *(undefined2 *)((int)puVar12 + -2) = 0x59a0;
      dos_exec();
      if (bVar14) {
        *(undefined2 *)((int)puVar12 + -2) = 0x59a5;
        FUN_0000_5a25();
      }
      else {
        pcVar4 = (code *)swi(0x21);
        (*pcVar4)();
      }
    }
    *(undefined2 *)((int)puVar12 + -2) = 0x59ae;
    FUN_0000_6147();
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
LAB_0000_59b3:
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
    *(undefined2 *)((int)puVar12 + -2) = 0x59bd;
    FUN_0000_5be1();
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
    *(undefined2 *)((int)puVar12 + -2) = 0x59c7;
    FUN_0000_5c20();
    pcVar4 = (code *)swi(0x21);
    (*pcVar4)();
  } while( true );
}



/* 0000:5a25  FUN_0000_5a25 */

void __cdecl16near FUN_0000_5a25(undefined2 param_1)

{
  char *pcVar1;
  code *pcVar2;
  char *in_DX;
  int unaff_BP;
  undefined2 unaff_SS;
  
  pcVar2 = (code *)swi(0x21);
  (*pcVar2)();
  pcVar2 = (code *)swi(0x21);
  (*pcVar2)();
  while( true ) {
    pcVar1 = in_DX;
    in_DX = in_DX + 1;
    if (*pcVar1 == '\0') break;
    pcVar2 = (code *)swi(0x21);
    (*pcVar2)();
  }
  pcVar2 = (code *)swi(0x21);
  (*pcVar2)();
  pcVar2 = (code *)swi(0x21);
  (*pcVar2)();
  pcVar2 = (code *)swi(0x21);
  (*pcVar2)();
  pcVar2 = (code *)swi(0x21);
  (*pcVar2)();
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x3e);
  (*(code *)*(undefined2 *)(unaff_BP + 0xc))();
  return;
}



/* 0000:5a62  init_bp_pointers */

void __cdecl16near init_bp_pointers(void)

{
  int unaff_BP;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_BP + 6) = unaff_CS;
  *(undefined2 *)(unaff_BP + 0xe) = unaff_CS;
  *(undefined2 *)(unaff_BP + 0x14) = unaff_CS;
  *(undefined2 *)(unaff_BP + 0x28) = (code *)dos_read;
  *(undefined2 *)(unaff_BP + 0x2a) = (code *)dos_write;
  *(undefined2 *)(unaff_BP + 0x2c) = (code *)dos_delete;
  *(undefined2 *)(unaff_BP + 0x2e) = (code *)lz_decompress;
  *(undefined2 *)(unaff_BP + 0x30) = (code *)lz_to_gvram_planes;
  *(undefined2 *)(unaff_BP + 0x32) = (code *)sgm_load;
  *(undefined2 *)(unaff_BP + 0x34) = (code *)frame_hook_6c0b;
  *(undefined2 *)(unaff_BP + 0x36) = (code *)hook_6bd1;
  *(undefined2 *)(unaff_BP + 0x38) = (code *)hook_6d00;
  *(undefined2 *)(unaff_BP + 0x3a) = (code *)hook_6cab;
  *(undefined2 *)(unaff_BP + 0x3c) = (code *)frame_hook_5ef5;
  *(undefined2 *)(unaff_BP + 0x3e) = (code *)wait_key;
  *(undefined2 *)(unaff_BP + 0x40) = (code *)poll_input;
  *(undefined2 *)(unaff_BP + 0x42) = (code *)blit_expand;
  *(undefined2 *)(unaff_BP + 0x44) = (code *)fill_vram_rect;
  *(undefined2 *)(unaff_BP + 0x46) = (code *)hook_6dcd;
  *(undefined2 *)(unaff_BP + 0x48) = (code *)hook_6d40;
  *(undefined2 *)(unaff_BP + 0x4a) = (code *)hook_6d89;
  *(undefined2 *)(unaff_BP + 0x4c) = (code *)name_buf;
  *(undefined2 *)(unaff_BP + 0x4e) = (code *)name_buf_2;
  *(undefined2 *)(unaff_BP + 0x50) = 0x430;
  *(undefined2 *)(unaff_BP + 0x52) = 0x43a;
  *(undefined2 *)(unaff_BP + 0x54) = 0x563a;
  *(undefined2 *)(unaff_BP + 0x56) = 0x564a;
  *(undefined1 *)(unaff_BP + 0xb6) = 0;
  *(undefined1 *)(unaff_BP + 0xba) = 0;
  *(undefined1 *)(unaff_BP + 0xb7) = 0;
  *(undefined1 *)(unaff_BP + 0xbb) = 0xff;
  *(undefined2 *)0x432 = (char *)s_SGM1_DAT_0000_5745;
  return;
}



/* 0000:5aff  FUN_0000_5aff */

undefined4 __cdecl16near FUN_0000_5aff(void)

{
  undefined2 uVar1;
  undefined2 in_AX;
  undefined2 in_DX;
  int unaff_BP;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  *(undefined2 *)(unaff_BP + 0x10) = 0x100;
  uVar1 = *(undefined2 *)(unaff_BP + 0x12);
  *(undefined1 *)0x80 = 2;
  *(int *)0x81 = unaff_BP;
  *(undefined1 *)0x83 = 0xd;
  (*(code *)*(undefined2 *)(unaff_BP + 0x10))();
  return CONCAT22(in_DX,in_AX);
}



/* 0000:5b20  dos_exec */

undefined4 __cdecl16near dos_exec(undefined2 param_1)

{
  char *pcVar1;
  char cVar2;
  code *pcVar3;
  undefined2 in_AX;
  char *unaff_SI;
  char *pcVar4;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  *(undefined1 *)0x5b61 = 0;
  pcVar4 = "\r";
  while( true ) {
    pcVar1 = unaff_SI;
    unaff_SI = unaff_SI + 1;
    cVar2 = *pcVar1;
    pcVar1 = pcVar4;
    pcVar4 = pcVar4 + 1;
    *pcVar1 = cVar2;
    if (cVar2 == '\r') break;
    *"" = *"" + '\x01';
  }
  *(undefined2 *)0x5b57 = unaff_CS;
  *(undefined2 *)0x5b5b = unaff_CS;
  *(undefined2 *)0x5b5f = unaff_CS;
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  return CONCAT22(in_AX,param_1);
}



/* 0000:5b53  exec_param_block */

/* WARNING: Unable to track spacebase fully for stack */

void exec_param_block(char *param_1,int param_2)

{
  code *pcVar1;
  char in_AL;
  char cVar2;
  char cVar3;
  uint uVar4;
  int in_BX;
  int unaff_SI;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  char *in_stack_00000000;
  undefined2 in_stack_0000000c;
  char in_stack_0000000e;
  int in_stack_00000010;
  
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + in_AL;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  *param_1 = *param_1 + (char)((uint)in_stack_0000000c >> 8);
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  *in_stack_00000000 = *in_stack_00000000 + (char)in_stack_0000000c;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  param_1[in_stack_00000010] = param_1[in_stack_00000010] + in_stack_0000000e;
  cVar2 = '\0';
  for (uVar4 = 0x69ffU - *(int *)(param_2 + 0x6a) >> 6; 99 < uVar4; uVar4 = uVar4 - 100) {
    cVar2 = cVar2 + '\x01';
  }
  cVar3 = '\0';
  if (cVar2 != '\0') {
    pcVar1 = (code *)swi(0x21);
    cVar3 = (*pcVar1)();
  }
  for (; 9 < uVar4; uVar4 = uVar4 - 10) {
    cVar3 = cVar3 + '\x01';
  }
  if (cVar3 != '\0') {
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
  }
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return;
}



/* 0000:5be1  FUN_0000_5be1 */

void __cdecl16near FUN_0000_5be1(void)

{
  code *pcVar1;
  char cVar2;
  char cVar3;
  uint uVar4;
  int unaff_BP;
  undefined2 unaff_SS;
  
  cVar2 = '\0';
  for (uVar4 = 0x69ffU - *(int *)(unaff_BP + 0x6a) >> 6; 99 < uVar4; uVar4 = uVar4 - 100) {
    cVar2 = cVar2 + '\x01';
  }
  cVar3 = '\0';
  if (cVar2 != '\0') {
    pcVar1 = (code *)swi(0x21);
    cVar3 = (*pcVar1)();
  }
  for (; 9 < uVar4; uVar4 = uVar4 - 10) {
    cVar3 = cVar3 + '\x01';
  }
  if (cVar3 != '\0') {
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
  }
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return;
}



/* 0000:5c20  FUN_0000_5c20 */

void __cdecl16near FUN_0000_5c20(void)

{
  code *pcVar1;
  char cVar2;
  char cVar3;
  uint uVar4;
  int unaff_BP;
  undefined2 unaff_SS;
  
  cVar2 = '\0';
  for (uVar4 = 0x69c0 - (*(uint *)(unaff_BP + 0x6a) & 0xffc0); 0x1869 < uVar4;
      uVar4 = uVar4 + 0xe796) {
    cVar2 = cVar2 + '\x01';
  }
  cVar3 = '\0';
  if (cVar2 != '\0') {
    pcVar1 = (code *)swi(0x21);
    cVar3 = (*pcVar1)();
  }
  for (uVar4 = uVar4 << 4; 9999 < uVar4; uVar4 = uVar4 + 0xd8f0) {
    cVar3 = cVar3 + '\x01';
  }
  cVar2 = '\0';
  if (cVar3 != '\0') {
    pcVar1 = (code *)swi(0x21);
    cVar2 = (*pcVar1)();
  }
  for (; 999 < uVar4; uVar4 = uVar4 - 1000) {
    cVar2 = cVar2 + '\x01';
  }
  cVar3 = '\0';
  if (cVar2 != '\0') {
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    pcVar1 = (code *)swi(0x21);
    cVar3 = (*pcVar1)();
  }
  for (; 99 < uVar4; uVar4 = uVar4 - 100) {
    cVar3 = cVar3 + '\x01';
  }
  cVar2 = '\0';
  if (cVar3 != '\0') {
    pcVar1 = (code *)swi(0x21);
    cVar2 = (*pcVar1)();
  }
  for (; 9 < uVar4; uVar4 = uVar4 - 10) {
    cVar2 = cVar2 + '\x01';
  }
  if (cVar2 != '\0') {
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
  }
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return;
}



/* 0000:5cb9  wait_key */

void __cdecl16far wait_key(void)

{
  do {
  } while (((((((DAT_0000_052a != '\0' || DAT_0000_052c._1_1_ != '\0') ||
               DAT_0000_052e._1_1_ != '\0') || DAT_0000_0530 != '\0') || DAT_0000_0531 != '\0') ||
            DAT_0000_0532 != '\0') || DAT_0000_0533 != '\0') || DAT_0000_0536._1_1_ != '\0');
  do {
  } while (((((((DAT_0000_052a == '\0' && DAT_0000_052c._1_1_ == '\0') &&
               DAT_0000_052e._1_1_ == '\0') && DAT_0000_0530 == '\0') && DAT_0000_0531 == '\0') &&
            DAT_0000_0532 == '\0') && DAT_0000_0533 == '\0') && DAT_0000_0536._1_1_ == '\0');
  return;
}



/* 0000:5d26  sgm_load */

void __cdecl16far sgm_load(void)

{
  undefined2 uVar1;
  undefined2 *puVar2;
  undefined2 *puVar3;
  undefined2 in_AX;
  int iVar4;
  int iVar5;
  uint uVar6;
  int in_BX;
  int *piVar7;
  int iVar8;
  int unaff_BP;
  undefined2 *unaff_SI;
  undefined2 *puVar9;
  undefined2 *unaff_DI;
  undefined2 *puVar10;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  undefined2 uVar11;
  bool bVar12;
  
  DAT_0000_5ef0 = (byte)in_AX;
  DAT_0000_5ef1 = (byte)((uint)in_AX >> 8);
  DAT_0000_5ee8 = unaff_DS;
  *(undefined2 *)(unaff_BP + 0xe) = unaff_SS;
  DAT_0000_5ef2 = (byte)in_BX;
  DAT_0000_5ef3 = (byte)((uint)in_BX >> 8);
  piVar7 = (int *)(DAT_0000_5ef1 + 0x1800);
  uVar11 = *(undefined2 *)(unaff_BP + 10);
  DAT_0000_5eea = unaff_SI;
  DAT_0000_5eec = unaff_DI;
  if (((DAT_0000_5ef0 & 0x42) == 0) && ((DAT_0000_5ef0 & 1) != 0)) {
    if (in_BX == *piVar7) {
      return;
    }
    *piVar7 = in_BX;
  }
  else {
    *piVar7 = 0;
  }
  uVar11 = DAT_0000_5ee8;
  puVar9 = (undefined2 *)*(undefined2 *)(*(int *)(unaff_BP + 0x50) + (uint)DAT_0000_5ef3 * 2);
  puVar10 = (undefined2 *)*(undefined2 *)(unaff_BP + 0x4c);
  for (iVar4 = 0x28; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar3 = puVar10;
    puVar10 = puVar10 + 1;
    puVar2 = puVar9;
    puVar9 = puVar9 + 1;
    *puVar3 = *puVar2;
  }
  uVar11 = 0;
  iVar8 = 0;
  iVar4 = 0;
  iVar5 = -0x6000;
  if ((DAT_0000_5ef0 & 1) == 0) goto LAB_0000_5e30;
  if ((DAT_0000_5ef0 & 2) == 0) {
    if (DAT_0000_5ef3 != DAT_0000_5ef4) {
      DAT_0000_5ef4 = DAT_0000_5ef3;
      goto LAB_0000_5ddd;
    }
  }
  else {
    DAT_0000_5ef4 = 0;
LAB_0000_5ddd:
    *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x28);
    bVar12 = false;
    uVar11 = 0;
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
    if (bVar12) {
      return;
    }
  }
  piVar7 = (int *)((uint)DAT_0000_5ef2 * 4);
  uVar1 = *(undefined2 *)(unaff_BP + 10);
  iVar5 = piVar7[3] - piVar7[1];
  for (iVar4 = piVar7[2] - *piVar7; iVar4 != 0; iVar4 = iVar4 + -1) {
    iVar5 = iVar5 + 2;
  }
  iVar4 = piVar7[1];
  iVar8 = *piVar7;
LAB_0000_5e30:
  *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x28);
  bVar12 = false;
  DAT_0000_5eee =
       (undefined2 *)(*(code *)*(undefined2 *)(unaff_BP + 0xc))(0,uVar11,iVar4,iVar5,iVar8);
  if (bVar12) {
    return;
  }
  if ((DAT_0000_5ef0 & 4) == 0) {
    if ((DAT_0000_5ef0 & 2) == 0) {
      *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x2e);
    }
    else {
      *(undefined2 *)(unaff_BP + 0xc) = *(undefined2 *)(unaff_BP + 0x30);
    }
    (*(code *)*(undefined2 *)(unaff_BP + 0xc))(0);
  }
  else {
    iVar4 = *(int *)((uint)DAT_0000_5ef1 + unaff_BP);
    if ((DAT_0000_5ef0 & 0x80) != 0) {
      iVar4 = iVar4 + 0x10;
    }
    uVar11 = *(undefined2 *)(unaff_BP + 8);
    if (DAT_0000_5eea < DAT_0000_5eee) {
      uVar6 = (uint)((int)DAT_0000_5eee - (int)DAT_0000_5eea) >> 1;
      puVar9 = DAT_0000_5eea;
      puVar10 = DAT_0000_5eec;
      if (((int)DAT_0000_5eee - (int)DAT_0000_5eea & 1U) != 0) {
        puVar10 = (undefined2 *)((int)DAT_0000_5eec + 1);
        puVar9 = (undefined2 *)((int)DAT_0000_5eea + 1);
        *(undefined1 *)DAT_0000_5eec = *(undefined1 *)DAT_0000_5eea;
      }
      for (; uVar6 != 0; uVar6 = uVar6 - 1) {
        puVar3 = puVar10;
        puVar10 = puVar10 + 1;
        puVar2 = puVar9;
        puVar9 = puVar9 + 1;
        *puVar3 = *puVar2;
      }
    }
  }
  return;
}



/* 0000:5ef5  frame_hook_5ef5 */

void __cdecl16far frame_hook_5ef5(void)

{
  undefined1 in_AL;
  int unaff_BP;
  undefined2 unaff_SS;
  
  out(100,in_AL);
  *(undefined2 *)(unaff_BP + 0x66) = *(undefined2 *)(unaff_BP + 0x5e);
  do {
  } while (*(uint *)(unaff_BP + 0x5c) <
           (uint)(*(int *)(unaff_BP + 0x60) + *(int *)(unaff_BP + 0x5e)));
  *(undefined2 *)(unaff_BP + 0x5e) = *(undefined2 *)(unaff_BP + 0x5c);
  *(int *)(unaff_BP + 0x66) = *(int *)(unaff_BP + 0x5c) - *(int *)(unaff_BP + 0x66);
  return;
}



/* 0000:5f20  FUN_0000_5f20 */

void __cdecl16near FUN_0000_5f20(void)

{
  FUN_0000_5fbd();
  FUN_0000_5fee();
  FUN_0000_6215();
  FUN_0000_63ce();
  FUN_0000_62fb();
  FUN_0000_64a8();
  FUN_0000_62cb();
  FUN_0000_62b6();
  FUN_0000_626c();
  FUN_0000_6345();
  FUN_0000_633b();
  FUN_0000_6331();
  FUN_0000_634c();
  FUN_0000_6174();
  FUN_0000_61cd();
  FUN_0000_60a2();
  FUN_0000_601a();
  return;
}



/* 0000:5f58  FUN_0000_5f58 */

void __cdecl16near FUN_0000_5f58(void)

{
  FUN_0000_62fb();
  FUN_0000_64a8();
  FUN_0000_62cb();
  FUN_0000_626c();
  FUN_0000_6345();
  FUN_0000_633b();
  FUN_0000_6331();
  FUN_0000_634c();
  return;
}



/* 0000:5f75  FUN_0000_5f75 */

void __cdecl16near FUN_0000_5f75(void)

{
  FUN_0000_6057();
  FUN_0000_6147();
  FUN_0000_6206();
  FUN_0000_61b6();
  FUN_0000_636d();
  FUN_0000_6360();
  FUN_0000_62b6();
  FUN_0000_632c();
  FUN_0000_64a8();
  FUN_0000_626c();
  FUN_0000_6345();
  FUN_0000_5fff();
  FUN_0000_6331();
  FUN_0000_62cb();
  FUN_0000_62e4();
  FUN_0000_633b();
  return;
}



/* 0000:5fbd  FUN_0000_5fbd */

void __cdecl16near FUN_0000_5fbd(void)

{
  code *pcVar1;
  int iVar2;
  
  DAT_0000_5fae = 0;
  pcVar1 = (code *)swi(0x18);
  (*pcVar1)();
  if (DAT_0000_5fbc == '1') {
    DAT_0000_5fae = DAT_0000_5fae | 0x80;
    pcVar1 = (code *)swi(0x18);
    iVar2 = (*pcVar1)();
    if (iVar2 != 0) {
      DAT_0000_5fae = DAT_0000_5fae + 1;
    }
  }
  return;
}



/* 0000:5fee  FUN_0000_5fee */

void __cdecl16near FUN_0000_5fee(void)

{
  code *pcVar1;
  
  if ((DAT_0000_5fae & 0x80) != 0) {
    pcVar1 = (code *)swi(0x18);
    (*pcVar1)();
  }
  return;
}



/* 0000:5fff  FUN_0000_5fff */

void __cdecl16near FUN_0000_5fff(void)

{
  code *pcVar1;
  int iVar2;
  
  if (((DAT_0000_5fae & 0x80) != 0) && ((DAT_0000_5fae & 0x7f) != 0)) {
    pcVar1 = (code *)swi(0x18);
    iVar2 = (*pcVar1)();
    if (iVar2 != 0) {
      DAT_0000_5fae = DAT_0000_5fae - 1;
    }
  }
  return;
}



/* 0000:601a  FUN_0000_601a */

uint __cdecl16near FUN_0000_601a(undefined2 param_1,undefined2 param_2,undefined2 param_3)

{
  code *pcVar1;
  undefined1 uVar2;
  uint in_AX;
  undefined1 extraout_AH;
  uint uVar3;
  undefined2 in_BX;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  if (*(char *)0x60a0 != '\0') {
    return in_AX;
  }
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  *(undefined2 *)0x609c = in_BX;
  *(undefined2 *)0x609e = unaff_ES;
  *(undefined2 *)0x3a8 = 0;
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  DAT_0000_60a1 = in(2);
  uVar3 = CONCAT11(extraout_AH,DAT_0000_60a1) & 0xfffb;
  uVar2 = (undefined1)uVar3;
  out(2,uVar2);
  out(100,uVar2);
  *(undefined1 *)0x60a0 = 1;
  return uVar3;
}



/* 0000:6057  FUN_0000_6057 */

void __cdecl16near FUN_0000_6057(undefined2 param_1)

{
  code *pcVar1;
  undefined2 unaff_DS;
  
  if (*(char *)0x60a0 == '\0') {
    return;
  }
  out(2,DAT_0000_60a1);
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  *(undefined1 *)0x60a0 = 0;
  return;
}



/* 0000:60a2  FUN_0000_60a2 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16near FUN_0000_60a2(void)

{
  code *pcVar1;
  undefined2 in_BX;
  undefined2 unaff_ES;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  DAT_0000_6136 = in_BX;
  _DAT_0000_6138 = unaff_ES;
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  DAT_0000_613a = in_BX;
  _DAT_0000_613c = unaff_ES;
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  DAT_0000_613e = in_BX;
  _DAT_0000_6140 = unaff_ES;
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  DAT_0000_6142 = in_BX;
  _DAT_0000_6144 = unaff_ES;
  (*pcVar1)();
  return;
}



/* 0000:6147  FUN_0000_6147 */

void __cdecl16near FUN_0000_6147(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return;
}



/* 0000:6174  FUN_0000_6174 */

void __cdecl16near FUN_0000_6174(void)

{
  code *pcVar1;
  undefined2 in_BX;
  undefined2 unaff_ES;
  
  if ((DAT_0000_0405 & 3) == 0) {
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    pcVar1 = (code *)swi(0x21);
    DAT_0000_61b2._0_2_ = in_BX;
    DAT_0000_61b2._2_2_ = unaff_ES;
    (*pcVar1)();
  }
  pcVar1 = (code *)swi(0x7f);
  (*pcVar1)();
  return;
}



/* 0000:61b6  FUN_0000_61b6 */

void __cdecl16near FUN_0000_61b6(void)

{
  code *pcVar1;
  
  if ((DAT_0000_0405 & 3) == 0) {
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
  }
  return;
}



/* 0000:61cd  FUN_0000_61cd */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16near FUN_0000_61cd(void)

{
  code *pcVar1;
  undefined2 in_BX;
  undefined2 unaff_ES;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  pcVar1 = (code *)swi(0x21);
  DAT_0000_6202 = in_BX;
  _DAT_0000_6204 = unaff_ES;
  (*pcVar1)();
  return;
}



/* 0000:6206  FUN_0000_6206 */

void __cdecl16near FUN_0000_6206(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return;
}



/* 0000:6215  FUN_0000_6215 */

void __cdecl16near FUN_0000_6215(void)

{
  code *pcVar1;
  char cVar2;
  
  pcVar1 = (code *)swi(0x21);
  cVar2 = (*pcVar1)();
  DAT_0000_622c = cVar2 + 'A';
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return;
}



/* 0000:626c  FUN_0000_626c */

void __cdecl16near FUN_0000_626c(void)

{
  out(0xa4,0);
  out(0xa6,1);
  FUN_0000_6283();
  out(0xa6,0);
  out(0xa4,1);
  FUN_0000_6283();
  return;
}



/* 0000:6283  FUN_0000_6283 */

void __cdecl16near FUN_0000_6283(void)

{
  undefined2 *puVar1;
  int iVar2;
  undefined2 *puVar3;
  
  puVar3 = (undefined2 *)0x0;
  for (iVar2 = 16000; iVar2 != 0; iVar2 = iVar2 + -1) {
    puVar1 = puVar3;
    puVar3 = puVar3 + 1;
    *puVar1 = 0;
  }
  puVar3 = (undefined2 *)0x0;
  for (iVar2 = 16000; iVar2 != 0; iVar2 = iVar2 + -1) {
    puVar1 = puVar3;
    puVar3 = puVar3 + 1;
    *puVar1 = 0;
  }
  puVar3 = (undefined2 *)0x0;
  for (iVar2 = 16000; iVar2 != 0; iVar2 = iVar2 + -1) {
    puVar1 = puVar3;
    puVar3 = puVar3 + 1;
    *puVar1 = 0;
  }
  puVar3 = (undefined2 *)0x0;
  for (iVar2 = 16000; iVar2 != 0; iVar2 = iVar2 + -1) {
    puVar1 = puVar3;
    puVar3 = puVar3 + 1;
    *puVar1 = 0;
  }
  return;
}



/* 0000:62b6  FUN_0000_62b6 */

void __cdecl16near FUN_0000_62b6(void)

{
  byte bVar1;
  
  bVar1 = 0;
  do {
    out(0xa8,bVar1);
    out(0xac,0);
    out(0xaa,0);
    out(0xae,0);
    bVar1 = bVar1 + 1;
  } while (bVar1 < 0x10);
  return;
}



/* 0000:62cb  FUN_0000_62cb */

void __cdecl16near FUN_0000_62cb(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return;
}



/* 0000:62e4  FUN_0000_62e4 */

void __cdecl16near FUN_0000_62e4(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return;
}



/* 0000:62fb  FUN_0000_62fb */

undefined1 __cdecl16near FUN_0000_62fb(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(0x18);
  (*pcVar1)();
  out(0x68,2);
  out(0x68,4);
  out(0x68,7);
  out(0x68,8);
  out(0x6a,1);
  out(0x6a,0x41);
  out(0x7c,0xc0);
  out(0x7e,0xff);
  out(0x7e,0xff);
  out(0x7e,0xff);
  out(0x7e,0xff);
  out(0x7c,0);
  return 0;
}



/* 0000:632c  FUN_0000_632c */

undefined1 __cdecl16near FUN_0000_632c(void)

{
  out(0x6a,0x40);
  return 0x40;
}



/* 0000:6331  FUN_0000_6331 */

void __cdecl16near FUN_0000_6331(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(0x18);
  (*pcVar1)();
  return;
}



/* 0000:633b  FUN_0000_633b */

void __cdecl16near FUN_0000_633b(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(0x18);
  (*pcVar1)();
  return;
}



/* 0000:6345  FUN_0000_6345 */

undefined1 __cdecl16near FUN_0000_6345(void)

{
  out(0xa4,0);
  out(0xa6,0);
  return 0;
}



/* 0000:634c  FUN_0000_634c */

void __cdecl16near FUN_0000_634c(void)

{
  DAT_0000_635f = DAT_0000_0500;
  DAT_0000_0500 = DAT_0000_0500 | 0x20;
  return;
}



/* 0000:6360  FUN_0000_6360 */

void __cdecl16near FUN_0000_6360(void)

{
  DAT_0000_0500 = DAT_0000_635f;
  return;
}



/* 0000:636d  FUN_0000_636d */

void __cdecl16near FUN_0000_636d(void)

{
  code *pcVar1;
  int in_BX;
  byte bVar2;
  
  do {
    bVar2 = (byte)((uint)in_BX >> 8);
    pcVar1 = (code *)swi(0x18);
    (*pcVar1)();
    in_BX = (uint)bVar2 << 8;
  } while (bVar2 != 0);
  return;
}



/* 0000:63ce  FUN_0000_63ce */

void __cdecl16near FUN_0000_63ce(void)

{
  undefined2 unaff_DS;
  undefined1 uVar1;
  
  uVar1 = false;
  *(byte *)&DAT_0000_0405 = *(byte *)&DAT_0000_0405 & 8;
  FUN_0000_641a();
  if (!(bool)uVar1) {
    uVar1 = 0;
    *(byte *)&DAT_0000_0405 = *(byte *)&DAT_0000_0405 | 1;
  }
  FUN_0000_643c();
  if (!(bool)uVar1) {
    uVar1 = 0;
    *(byte *)&DAT_0000_0405 = *(byte *)&DAT_0000_0405 | 2;
  }
  FUN_0000_645e();
  if (!(bool)uVar1) {
    uVar1 = 0;
    *(byte *)&DAT_0000_0405 = *(byte *)&DAT_0000_0405 | 8;
  }
  FUN_0000_6413();
  if ((bool)uVar1) {
    *(byte *)&DAT_0000_0405 = *(byte *)&DAT_0000_0405 | 0x80;
  }
  *(byte *)&DAT_0000_0405 = *(byte *)&DAT_0000_0405 | (DAT_0000_054d & 0x20) << 1;
  FUN_0000_6495();
  return;
}



/* 0000:6413  FUN_0000_6413 */

void __cdecl16near FUN_0000_6413(void)

{
  code *pcVar1;
  
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  return;
}



/* 0000:641a  FUN_0000_641a */

undefined2 __cdecl16near FUN_0000_641a(void)

{
  undefined2 in_AX;
  
  return in_AX;
}



/* 0000:643c  FUN_0000_643c */

undefined2 __cdecl16near FUN_0000_643c(void)

{
  undefined2 in_AX;
  
  return in_AX;
}



/* 0000:645e  FUN_0000_645e */

undefined2 __cdecl16near FUN_0000_645e(undefined2 param_1)

{
  int *piVar1;
  int *piVar2;
  code *pcVar3;
  int iVar4;
  int *piVar5;
  int *piVar6;
  undefined2 unaff_ES;
  undefined2 unaff_CS;
  
  pcVar3 = (code *)swi(0x21);
  (*pcVar3)();
  piVar6 = (int *)0x106;
  iVar4 = 6;
  piVar5 = (int *)0x6489;
  do {
    if (iVar4 == 0) {
      return param_1;
    }
    iVar4 = iVar4 + -1;
    piVar2 = piVar6;
    piVar6 = piVar6 + 1;
    piVar1 = piVar5;
    piVar5 = piVar5 + 1;
  } while (*piVar1 == *piVar2);
  return param_1;
}



/* 0000:6495  FUN_0000_6495 */

void __cdecl16near FUN_0000_6495(void)

{
  code *pcVar1;
  undefined1 uVar2;
  undefined2 unaff_DS;
  
  pcVar1 = (code *)swi(0x21);
  uVar2 = (*pcVar1)();
  *(undefined1 *)0x64a6 = uVar2;
  pcVar1 = (code *)swi(0x21);
  uVar2 = (*pcVar1)();
  *(undefined1 *)0x64a7 = uVar2;
  return;
}



/* 0000:64a8  FUN_0000_64a8 */

char __cdecl16near FUN_0000_64a8(void)

{
  char cVar1;
  undefined2 uVar2;
  undefined2 unaff_DS;
  
  uVar2 = 0;
  FUN_0000_64d7(0x64ae);
  out(0xa2,0x70);
  FUN_0000_64d7();
  out(0xa0,(char)uVar2);
  FUN_0000_64d7();
  out(0xa0,(char)((uint)uVar2 >> 8));
  FUN_0000_64d7();
  out(0xa0,0);
  FUN_0000_64d7();
  cVar1 = (*(byte *)&DAT_0000_0405 & 0x40) + 0x19;
  out(0xa0,cVar1);
  return cVar1;
}



/* 0000:64d7  FUN_0000_64d7 */

void __cdecl16near FUN_0000_64d7(void)

{
  byte bVar1;
  
  do {
    bVar1 = in(0x60);
  } while ((bVar1 & 2) != 0);
  return;
}



/* 0000:64f0  dos_read */

undefined2 __cdecl16far dos_read(void)

{
  code *pcVar1;
  undefined2 in_AX;
  undefined2 uVar2;
  undefined2 in_CX;
  undefined2 in_DX;
  undefined2 in_BX;
  undefined2 unaff_DI;
  undefined2 unaff_ES;
  undefined1 in_CF;
  undefined1 uVar3;
  
  DAT_0000_6785 = (undefined1)((uint)in_AX >> 8);
  DAT_0000_6784 = 0;
  DAT_0000_0402 = 0;
  DAT_0000_6726 = in_AX;
  DAT_0000_6728 = in_BX;
  DAT_0000_672a = in_CX;
  DAT_0000_672c = in_DX;
  DAT_0000_672e = unaff_DI;
  DAT_0000_6730 = unaff_ES;
  do {
    while( true ) {
      while( true ) {
        uVar3 = in_CF;
        pcVar1 = (code *)swi(0x21);
        uVar2 = (*pcVar1)();
        if (!(bool)uVar3) break;
        FUN_0000_66ce();
        in_CF = 0;
        if ((bool)uVar3) {
          DAT_0000_6784 = 1;
          return DAT_0000_6726;
        }
      }
      pcVar1 = (code *)swi(0x21);
      DAT_0000_6732 = uVar2;
      (*pcVar1)();
      if (!(bool)uVar3) break;
      FUN_0000_66ce();
      in_CF = uVar3;
      if ((bool)uVar3) {
        DAT_0000_6784 = 1;
        uVar2 = DAT_0000_6726;
        goto LAB_0000_6593;
      }
    }
    pcVar1 = (code *)swi(0x21);
    uVar2 = (*pcVar1)();
    if (!(bool)uVar3) goto LAB_0000_6593;
    FUN_0000_66ce();
    in_CF = uVar3;
  } while (!(bool)uVar3);
  DAT_0000_6784 = 1;
  uVar2 = DAT_0000_6726;
LAB_0000_6593:
  DAT_0000_6726 = uVar2;
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if ((bool)uVar3) {
    DAT_0000_6784 = 1;
  }
  return DAT_0000_6726;
}



/* 0000:65c4  dos_write */

undefined2 __cdecl16far dos_write(void)

{
  code *pcVar1;
  undefined2 in_AX;
  undefined2 uVar2;
  undefined2 in_CX;
  undefined2 in_DX;
  undefined2 in_BX;
  undefined2 unaff_SI;
  undefined2 unaff_DI;
  undefined2 unaff_ES;
  undefined1 uVar3;
  
  DAT_0000_67e9 = (undefined1)((uint)in_AX >> 8);
  DAT_0000_6794 = 0;
  DAT_0000_6796 = 0;
  DAT_0000_67e8 = 0;
  DAT_0000_0402 = 0;
  DAT_0000_6786 = in_AX;
  DAT_0000_6788 = in_BX;
  DAT_0000_678a = in_CX;
  DAT_0000_678c = in_DX;
  DAT_0000_678e = unaff_SI;
  DAT_0000_6790 = unaff_DI;
  DAT_0000_6792 = unaff_ES;
  while( true ) {
    uVar3 = false;
    pcVar1 = (code *)swi(0x21);
    uVar2 = (*pcVar1)();
    if (!(bool)uVar3) break;
    FUN_0000_66e4();
    if ((bool)uVar3) {
      DAT_0000_67e8 = 1;
      DAT_0000_0402 = 0;
      return DAT_0000_6794;
    }
  }
  pcVar1 = (code *)swi(0x21);
  DAT_0000_6796 = uVar2;
  uVar2 = (*pcVar1)();
  if ((bool)uVar3) {
    FUN_0000_66e4();
    if (!(bool)uVar3) {
      pcVar1 = (code *)swi(0x21);
      uVar2 = (*pcVar1)();
      if (!(bool)uVar3) goto LAB_0000_6652;
    }
    DAT_0000_67e8 = 1;
    uVar2 = DAT_0000_6794;
  }
LAB_0000_6652:
  DAT_0000_6794 = uVar2;
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if ((bool)uVar3) {
    DAT_0000_0402 = 0;
    DAT_0000_67e8 = 1;
    return DAT_0000_6794;
  }
  DAT_0000_0402 = 0;
  return DAT_0000_6794;
}



/* 0000:6692  dos_delete */

void __cdecl16far dos_delete(void)

{
  code *pcVar1;
  undefined1 in_CF;
  
  DAT_0000_67e9 = 0;
  pcVar1 = (code *)swi(0x21);
  (*pcVar1)();
  if (!(bool)in_CF) {
    pcVar1 = (code *)swi(0x21);
    (*pcVar1)();
    if (!(bool)in_CF) {
      while( true ) {
        pcVar1 = (code *)swi(0x21);
        (*pcVar1)();
        if ((!(bool)in_CF) || (FUN_0000_66e4(), !(bool)in_CF)) break;
        DAT_0000_67e8 = 1;
      }
    }
  }
  return;
}



/* 0000:66ce  FUN_0000_66ce */

void FUN_0000_66ce(void)

{
  FUN_0000_66fa();
  return;
}



/* 0000:66e4  FUN_0000_66e4 */

void FUN_0000_66e4(void)

{
  FUN_0000_66fa();
  return;
}



/* 0000:66fa  FUN_0000_66fa */

undefined4 __cdecl16near FUN_0000_66fa(undefined2 param_1,undefined2 param_2,undefined2 param_3)

{
  undefined1 in_CF;
  
  disk_error_screen();
  if (!(bool)in_CF) {
    DAT_0000_0402 = 0;
    return CONCAT22(param_1,param_3);
  }
  DAT_0000_03b8 = DAT_0000_03b8 | 0x8000;
  DAT_0000_03ba = DAT_0000_03ba | 0x8000;
  DAT_0000_0402 = 0;
  return CONCAT22(param_1,param_3);
}



/* 0000:6734  name_buf */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 name_buf(void)

{
  undefined2 *puVar1;
  code *pcVar2;
  char cVar3;
  undefined2 in_AX;
  undefined2 in_CX;
  int iVar4;
  undefined2 in_DX;
  int in_BX;
  int unaff_SI;
  undefined2 *puVar5;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  undefined2 in_stack_0000000e;
  undefined2 in_stack_00000012;
  
  DAT_0000_6930 = (char)in_AX;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  DAT_0000_6931 = (char)((uint)in_AX >> 8);
  DAT_0000_6932 = '\0';
  pcVar2 = (code *)swi(0x18);
  DAT_0000_692a = in_CX;
  DAT_0000_692c = in_DX;
  DAT_0000_692e = unaff_ES;
  (*pcVar2)();
  out(0xa8,0);
  out(0xaa,0);
  out(0xac,0);
  out(0xae,0);
  puVar5 = (undefined2 *)0x0;
  for (iVar4 = 2000; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    *puVar1 = 0;
  }
  puVar5 = (undefined2 *)&DAT_0000_2000;
  for (iVar4 = 2000; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    *puVar1 = 0xe1;
  }
  pcVar2 = (code *)swi(0x18);
  (*pcVar2)();
  FUN_0000_6a23();
  if (DAT_0000_6931 != '\0') {
    FUN_0000_6a23();
  }
  pcVar2 = (code *)swi(0x21);
  cVar3 = (*pcVar2)();
  _DAT_a000_1684 = CONCAT11(cVar3 + 'A',3);
  _DAT_a000_3684 = 0xe1;
  _DAT_a000_1686 = _DAT_a000_1684 | 0x80;
  _DAT_a000_3686 = 0xe1;
  FUN_0000_69ec();
  FUN_0000_69ec();
  FUN_0000_69ec();
  FUN_0000_69ec();
  FUN_0000_6a23(0);
  if (DAT_0000_6930 == '\0') {
    FUN_0000_6a23();
  }
  else {
    FUN_0000_6a23();
  }
  do {
    poll_input();
    if ((DAT_0000_03ba & 0x30) != 0) goto LAB_0000_68f4;
  } while ((DAT_0000_6931 == '\0') || ((DAT_0000_03ba & 0x80) == 0));
  DAT_0000_6932 = DAT_0000_6932 + '\x01';
LAB_0000_68f4:
  pcVar2 = (code *)swi(0x18);
  (*pcVar2)();
  puVar5 = (undefined2 *)0x0;
  for (iVar4 = 2000; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    *puVar1 = 0;
  }
  puVar5 = (undefined2 *)&DAT_0000_2000;
  for (iVar4 = 2000; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    *puVar1 = 0xe1;
  }
  pcVar2 = (code *)swi(0x18);
  (*pcVar2)();
  DAT_0000_0404 = 1;
  return CONCAT22(in_stack_0000000e,in_stack_00000012);
}



/* 0000:6798  name_buf_2 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 name_buf_2(void)

{
  undefined2 *puVar1;
  code *pcVar2;
  char cVar3;
  undefined2 in_AX;
  undefined2 in_CX;
  int iVar4;
  undefined2 in_DX;
  int in_BX;
  int unaff_SI;
  undefined2 *puVar5;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  undefined2 in_stack_0000000e;
  undefined2 in_stack_00000012;
  
  DAT_0000_6930 = (char)in_AX;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  *(char *)(in_BX + unaff_SI) = *(char *)(in_BX + unaff_SI) + DAT_0000_6930;
  DAT_0000_6931 = (char)((uint)in_AX >> 8);
  DAT_0000_6932 = '\0';
  pcVar2 = (code *)swi(0x18);
  DAT_0000_692a = in_CX;
  DAT_0000_692c = in_DX;
  DAT_0000_692e = unaff_ES;
  (*pcVar2)();
  out(0xa8,0);
  out(0xaa,0);
  out(0xac,0);
  out(0xae,0);
  puVar5 = (undefined2 *)0x0;
  for (iVar4 = 2000; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    *puVar1 = 0;
  }
  puVar5 = (undefined2 *)&DAT_0000_2000;
  for (iVar4 = 2000; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    *puVar1 = 0xe1;
  }
  pcVar2 = (code *)swi(0x18);
  (*pcVar2)();
  FUN_0000_6a23();
  if (DAT_0000_6931 != '\0') {
    FUN_0000_6a23();
  }
  pcVar2 = (code *)swi(0x21);
  cVar3 = (*pcVar2)();
  _DAT_a000_1684 = CONCAT11(cVar3 + 'A',3);
  _DAT_a000_3684 = 0xe1;
  _DAT_a000_1686 = _DAT_a000_1684 | 0x80;
  _DAT_a000_3686 = 0xe1;
  FUN_0000_69ec();
  FUN_0000_69ec();
  FUN_0000_69ec();
  FUN_0000_69ec();
  FUN_0000_6a23(0);
  if (DAT_0000_6930 == '\0') {
    FUN_0000_6a23();
  }
  else {
    FUN_0000_6a23();
  }
  do {
    poll_input();
    if ((DAT_0000_03ba & 0x30) != 0) goto LAB_0000_68f4;
  } while ((DAT_0000_6931 == '\0') || ((DAT_0000_03ba & 0x80) == 0));
  DAT_0000_6932 = DAT_0000_6932 + '\x01';
LAB_0000_68f4:
  pcVar2 = (code *)swi(0x18);
  (*pcVar2)();
  puVar5 = (undefined2 *)0x0;
  for (iVar4 = 2000; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    *puVar1 = 0;
  }
  puVar5 = (undefined2 *)&DAT_0000_2000;
  for (iVar4 = 2000; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    *puVar1 = 0xe1;
  }
  pcVar2 = (code *)swi(0x18);
  (*pcVar2)();
  DAT_0000_0404 = 1;
  return CONCAT22(in_stack_0000000e,in_stack_00000012);
}



/* 0000:67ea  disk_error_screen */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __cdecl16near disk_error_screen(void)

{
  undefined2 *puVar1;
  code *pcVar2;
  char cVar3;
  undefined2 in_AX;
  undefined2 in_CX;
  int iVar4;
  undefined2 in_DX;
  undefined2 *puVar5;
  undefined2 unaff_ES;
  undefined2 in_stack_0000000e;
  undefined2 in_stack_00000012;
  
  DAT_0000_6931 = (char)((uint)in_AX >> 8);
  DAT_0000_6930 = (char)in_AX;
  DAT_0000_6932 = '\0';
  pcVar2 = (code *)swi(0x18);
  DAT_0000_692a = in_CX;
  DAT_0000_692c = in_DX;
  DAT_0000_692e = unaff_ES;
  (*pcVar2)();
  out(0xa8,0);
  out(0xaa,0);
  out(0xac,0);
  out(0xae,0);
  puVar5 = (undefined2 *)0x0;
  for (iVar4 = 2000; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    *puVar1 = 0;
  }
  puVar5 = (undefined2 *)&DAT_0000_2000;
  for (iVar4 = 2000; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    *puVar1 = 0xe1;
  }
  pcVar2 = (code *)swi(0x18);
  (*pcVar2)();
  FUN_0000_6a23();
  if (DAT_0000_6931 != '\0') {
    FUN_0000_6a23();
  }
  pcVar2 = (code *)swi(0x21);
  cVar3 = (*pcVar2)();
  _DAT_a000_1684 = CONCAT11(cVar3 + 'A',3);
  _DAT_a000_3684 = 0xe1;
  _DAT_a000_1686 = _DAT_a000_1684 | 0x80;
  _DAT_a000_3686 = 0xe1;
  FUN_0000_69ec();
  FUN_0000_69ec();
  FUN_0000_69ec();
  FUN_0000_69ec();
  FUN_0000_6a23(0);
  if (DAT_0000_6930 == '\0') {
    FUN_0000_6a23();
  }
  else {
    FUN_0000_6a23();
  }
  do {
    poll_input();
    if ((DAT_0000_03ba & 0x30) != 0) goto LAB_0000_68f4;
  } while ((DAT_0000_6931 == '\0') || ((DAT_0000_03ba & 0x80) == 0));
  DAT_0000_6932 = DAT_0000_6932 + '\x01';
LAB_0000_68f4:
  pcVar2 = (code *)swi(0x18);
  (*pcVar2)();
  puVar5 = (undefined2 *)0x0;
  for (iVar4 = 2000; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    *puVar1 = 0;
  }
  puVar5 = (undefined2 *)&DAT_0000_2000;
  for (iVar4 = 2000; iVar4 != 0; iVar4 = iVar4 + -1) {
    puVar1 = puVar5;
    puVar5 = puVar5 + 1;
    *puVar1 = 0xe1;
  }
  pcVar2 = (code *)swi(0x18);
  (*pcVar2)();
  DAT_0000_0404 = 1;
  return CONCAT22(in_stack_0000000e,in_stack_00000012);
}



/* 0000:69ec  FUN_0000_69ec */

undefined4 __cdecl16near FUN_0000_69ec(void)

{
  uint *puVar1;
  byte bVar2;
  uint uVar3;
  uint in_AX;
  int iVar4;
  undefined2 in_DX;
  uint uVar5;
  uint *unaff_DI;
  undefined2 unaff_ES;
  
  iVar4 = 4;
  uVar5 = in_AX;
  do {
    uVar3 = uVar5 >> 0xc;
    uVar5 = uVar5 << 4 | uVar3;
    bVar2 = ((undefined *)&DAT_0000_6a13)[uVar3];
    unaff_DI[0x1000] = 0xe1;
    puVar1 = unaff_DI;
    unaff_DI = unaff_DI + 1;
    *puVar1 = (uint)bVar2;
    iVar4 = iVar4 + -1;
  } while (iVar4 != 0);
  return CONCAT22(in_DX,in_AX);
}



/* 0000:6a23  FUN_0000_6a23 */

undefined4 __cdecl16near FUN_0000_6a23(void)

{
  uint *puVar1;
  byte bVar2;
  char cVar3;
  undefined2 in_AX;
  byte bVar6;
  byte bVar7;
  uint uVar4;
  undefined1 uVar8;
  int iVar5;
  undefined2 in_DX;
  uint *unaff_SI;
  uint *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  while( true ) {
    puVar1 = unaff_SI + 1;
    uVar4 = *unaff_SI;
    bVar2 = (byte)uVar4;
    if (bVar2 == 0) break;
    bVar6 = (byte)(uVar4 >> 8);
    if (bVar2 == 0xfa) {
      unaff_SI = (uint *)((int)unaff_SI + 3);
      iVar5 = CONCAT11((char)((uint)bVar6 * 2 >> 8),(char)*puVar1);
      unaff_DI = (uint *)((uint)bVar6 * 2 + iVar5 * 0x20 + iVar5 * 0x80);
    }
    else {
      if (bVar2 == 0xff) break;
      if ((bVar2 < 0x20) || ((0x7f < bVar2 && ((bVar2 < 0xa1 || (0xdf < bVar2)))))) {
        bVar7 = bVar6 - 0x1f;
        if (-1 < (char)bVar7) {
          bVar7 = bVar6 + 0xbf + (bVar7 < 0x61);
        }
        uVar4 = CONCAT11(bVar7 + 0xa1,bVar2 * '\x02' + '\x1f' + (0x5e < bVar7)) & 0x7f7f;
        cVar3 = (char)uVar4;
        uVar8 = (undefined1)(uVar4 >> 8);
        *unaff_DI = CONCAT11(uVar8,cVar3 + -0x20);
        unaff_DI[0x1000] = 0xe1;
        unaff_DI[1] = CONCAT11(uVar8,cVar3 + '`');
        unaff_DI[0x1001] = 0xe1;
        unaff_DI = unaff_DI + 2;
        unaff_SI = puVar1;
      }
      else {
        unaff_SI = (uint *)((int)unaff_SI + 1);
        *unaff_DI = uVar4 & 0xff;
        unaff_DI[0x1000] = 0xe1;
        unaff_DI = unaff_DI + 1;
      }
    }
  }
  return CONCAT22(in_DX,in_AX);
}



/* 0000:6aa0  lz_decompress */

void __cdecl16far lz_decompress(void)

{
  lz_core();
  return;
}



/* 0000:6aa4  lz_to_gvram_planes */

void __cdecl16far lz_to_gvram_planes(void)

{
  lz_core();
  lz_core();
  lz_core();
  lz_core();
  return;
}



/* 0000:6acd  lz_core */

void __cdecl16near lz_core(void)

{
  byte *pbVar1;
  byte bVar2;
  byte *pbVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  byte *unaff_SI;
  byte *pbVar7;
  byte *pbVar8;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  bool bVar9;
  
  uVar4 = 0;
LAB_0000_6acf:
  while( true ) {
    pbVar7 = unaff_SI + 1;
    bVar2 = *unaff_SI;
    if (bVar2 == 0) {
      return;
    }
    if ((char)bVar2 < '\0') break;
    for (iVar5 = CONCAT11((char)(uVar4 >> 8),bVar2); iVar5 != 0; iVar5 = iVar5 + -1) {
      pbVar3 = unaff_DI;
      unaff_DI = unaff_DI + 1;
      pbVar1 = pbVar7;
      pbVar7 = pbVar7 + 1;
      *pbVar3 = *pbVar1;
    }
    uVar4 = 0;
    unaff_SI = pbVar7;
  }
  uVar6 = (uint)(byte)((bVar2 & 0x7f) + 3);
  unaff_SI = unaff_SI + 2;
  pbVar8 = unaff_DI + -(int)(*pbVar7 + 1);
  if (unaff_DI < (byte *)(*pbVar7 + 1)) goto LAB_0000_6afd;
  goto LAB_0000_6af3;
LAB_0000_6afd:
  do {
    pbVar1 = unaff_DI;
    unaff_DI = unaff_DI + 1;
    *pbVar1 = 0;
    pbVar8 = pbVar8 + 1;
    bVar9 = pbVar8 == (byte *)0x0;
    uVar6 = uVar6 - 1;
  } while (uVar6 != 0 && !bVar9);
  pbVar8 = (byte *)0x0;
  uVar4 = uVar6;
  if (bVar9) {
LAB_0000_6af3:
    for (; uVar4 = 0, uVar6 != 0; uVar6 = uVar6 - 1) {
      pbVar3 = unaff_DI;
      unaff_DI = unaff_DI + 1;
      pbVar1 = pbVar8;
      pbVar8 = pbVar8 + 1;
      *pbVar3 = *pbVar1;
    }
  }
  goto LAB_0000_6acf;
}



/* 0000:6bd1  hook_6bd1 */

void __cdecl16far hook_6bd1(void)

{
  byte *pbVar1;
  byte bVar2;
  undefined2 *puVar3;
  undefined1 uVar4;
  int iVar5;
  int unaff_BP;
  byte *unaff_SI;
  undefined2 *puVar6;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  undefined2 unaff_DS;
  
  puVar6 = (undefined2 *)&DAT_0000_6b70;
  for (iVar5 = 0x30; iVar5 != 0; iVar5 = iVar5 + -1) {
    puVar3 = puVar6;
    puVar6 = puVar6 + 1;
    *puVar3 = 0;
  }
  while( true ) {
    bVar2 = *unaff_SI;
    if (0xf < bVar2) break;
    iVar5 = (uint)bVar2 * 6;
    uVar4 = (undefined1)((uint)bVar2 * 4 >> 8);
    *(uint *)((undefined *)&DAT_0000_6b10 + iVar5) = CONCAT11(uVar4,unaff_SI[1] << 4);
    pbVar1 = unaff_SI + 3;
    *(uint *)((undefined *)&DAT_0000_6b12 + iVar5) = CONCAT11(uVar4,unaff_SI[2] << 4);
    unaff_SI = unaff_SI + 4;
    *(uint *)((undefined *)&DAT_0000_6b14 + iVar5) = CONCAT11(uVar4,*pbVar1 << 4);
  }
  *(undefined1 *)(unaff_BP + 0xb8) = 1;
  return;
}



/* 0000:6c0b  frame_hook_6c0b */

void __cdecl16far frame_hook_6c0b(void)

{
  undefined2 *puVar1;
  char cVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  byte bVar5;
  uint uVar6;
  int unaff_BP;
  undefined2 *puVar7;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  
  if (*(char *)(unaff_BP + 0xb8) != '\0') {
    *(undefined1 *)(unaff_BP + 0xb8) = 0;
    puVar7 = (undefined2 *)&DAT_0000_6b10;
    uVar6 = 0;
    if (*(char *)0x6bd0 == '\0') {
      for (; uVar6 < 0x10; uVar6 = uVar6 + 1) {
        out(0xa8,(char)uVar6);
        uVar3 = *puVar7;
        if ((char)((uint)uVar3 >> 8) != '\0') {
          uVar3 = 0xf0;
        }
        out(0xac,(char)uVar3 >> 4);
        puVar1 = puVar7 + 2;
        uVar3 = puVar7[1];
        if ((char)((uint)uVar3 >> 8) != '\0') {
          uVar3 = 0xf0;
        }
        out(0xaa,(char)uVar3 >> 4);
        puVar7 = puVar7 + 3;
        uVar3 = *puVar1;
        if ((char)((uint)uVar3 >> 8) != '\0') {
          uVar3 = 0xf0;
        }
        out(0xae,(char)uVar3 >> 4);
      }
    }
    else {
      while (bVar5 = (byte)uVar6, bVar5 < 0x10) {
        out(0xa8,bVar5);
        uVar3 = *puVar7;
        if ((char)((uint)uVar3 >> 8) != '\0') {
          uVar3 = 0xf0;
        }
        puVar1 = puVar7 + 2;
        uVar4 = puVar7[1];
        if ((char)((uint)uVar4 >> 8) != '\0') {
          uVar4 = 0xf0;
        }
        cVar2 = (char)uVar4 >> 4;
        puVar7 = puVar7 + 3;
        uVar4 = *puVar1;
        if ((char)((uint)uVar4 >> 8) != '\0') {
          uVar4 = 0xf0;
        }
        cVar2 = ((char)uVar3 >> 4) * '\x04' + cVar2 + cVar2 * '\x02' + ((char)uVar4 >> 4);
        out(0xac,-((char)(cVar2 * '\x02') < '\0'));
        out(0xaa,-((char)(cVar2 * '\x04') < '\0'));
        out(0xae,-((char)(cVar2 * '\b') < '\0'));
        uVar6 = (uint)(byte)(bVar5 + 1);
      }
    }
  }
  return;
}



/* 0000:6cab  hook_6cab */

void __cdecl16far hook_6cab(void)

{
  byte *pbVar1;
  undefined2 *puVar2;
  int iVar3;
  byte *unaff_SI;
  undefined2 *puVar4;
  undefined2 unaff_CS;
  undefined2 unaff_DS;
  
  puVar4 = (undefined2 *)&DAT_0000_6b70;
  for (iVar3 = 0x30; iVar3 != 0; iVar3 = iVar3 + -1) {
    puVar2 = puVar4;
    puVar4 = puVar4 + 1;
    *puVar2 = 0;
  }
  while( true ) {
    if (0xf < *unaff_SI) break;
    iVar3 = (uint)*unaff_SI * 6;
    *(int *)((undefined *)&DAT_0000_6b70 + iVar3) =
         (int)((uint)unaff_SI[1] * 0x10 - *(int *)((undefined *)&DAT_0000_6b10 + iVar3)) >> 4;
    pbVar1 = unaff_SI + 3;
    *(int *)((undefined *)&DAT_0000_6b72 + iVar3) =
         (int)((uint)unaff_SI[2] * 0x10 - *(int *)((undefined *)&DAT_0000_6b12 + iVar3)) >> 4;
    unaff_SI = unaff_SI + 4;
    *(int *)((undefined *)&DAT_0000_6b74 + iVar3) =
         (int)((uint)*pbVar1 * 0x10 - *(int *)((undefined *)&DAT_0000_6b14 + iVar3)) >> 4;
  }
  return;
}



/* 0000:6d00  hook_6d00 */

void __cdecl16far hook_6d00(void)

{
  byte in_AL;
  byte bVar1;
  int unaff_BP;
  int *piVar2;
  int *piVar3;
  undefined2 unaff_CS;
  undefined2 unaff_SS;
  
  piVar2 = (int *)&DAT_0000_6b70;
  piVar3 = (int *)&DAT_0000_6b10;
  for (bVar1 = 0; bVar1 < 0x10; bVar1 = bVar1 + 1) {
    *piVar3 = *piVar3 + (*piVar2 << (in_AL & 0x1f));
    piVar3[1] = piVar3[1] + (piVar2[1] << (in_AL & 0x1f));
    piVar3[2] = piVar3[2] + (piVar2[2] << (in_AL & 0x1f));
    piVar2 = piVar2 + 3;
    piVar3 = piVar3 + 3;
  }
  *(undefined1 *)(unaff_BP + 0xb8) = 1;
  return;
}



/* 0000:6d40  hook_6d40 */

void __cdecl16far hook_6d40(void)

{
  char cVar1;
  char cVar2;
  char cVar3;
  int unaff_SI;
  int iVar4;
  int unaff_DI;
  undefined2 unaff_DS;
  
  cVar1 = *(char *)(unaff_SI + 1);
  cVar3 = *(char *)(unaff_SI + 2);
  iVar4 = unaff_SI + 3;
  cVar2 = cVar1;
  do {
    do {
      FUN_0000_725b(unaff_DS,iVar4,unaff_DI);
      iVar4 = iVar4 + 1;
      cVar2 = cVar2 + -1;
    } while (cVar2 != '\0');
    unaff_DI = unaff_DI + 0x500;
    cVar3 = cVar3 + -1;
    cVar2 = cVar1;
  } while (cVar3 != '\0');
  return;
}



/* 0000:6d89  hook_6d89 */

void __cdecl16far hook_6d89(void)

{
  char cVar1;
  char cVar2;
  char cVar3;
  int unaff_SI;
  int iVar4;
  int unaff_DI;
  undefined2 unaff_DS;
  
  cVar1 = *(char *)(unaff_SI + 1);
  cVar3 = *(char *)(unaff_SI + 2);
  iVar4 = unaff_SI + 3;
  cVar2 = cVar1;
  do {
    do {
      FUN_0000_7342(unaff_DS,iVar4,unaff_DI);
      iVar4 = iVar4 + 1;
      cVar2 = cVar2 + -1;
    } while (cVar2 != '\0');
    unaff_DI = unaff_DI + 0x500;
    cVar3 = cVar3 + -1;
    cVar2 = cVar1;
  } while (cVar3 != '\0');
  return;
}



/* 0000:6dcd  hook_6dcd */

void __cdecl16far hook_6dcd(void)

{
  char cVar1;
  char cVar2;
  char cVar3;
  int unaff_SI;
  int iVar4;
  int unaff_DI;
  undefined2 unaff_DS;
  
  cVar1 = *(char *)(unaff_SI + 1);
  cVar3 = *(char *)(unaff_SI + 2);
  iVar4 = unaff_SI + 3;
  cVar2 = cVar1;
  do {
    do {
      FUN_0000_6e1b(unaff_DS,iVar4,unaff_DI);
      iVar4 = iVar4 + 1;
      cVar2 = cVar2 + -1;
    } while (cVar2 != '\0');
    unaff_DI = unaff_DI + 0x500;
    cVar3 = cVar3 + -1;
    cVar2 = cVar1;
  } while (cVar3 != '\0');
  return;
}



/* 0000:6e1b  FUN_0000_6e1b */

undefined2 __cdecl16near FUN_0000_6e1b(void)

{
  undefined2 *puVar1;
  byte bVar3;
  uint uVar2;
  byte in_CL;
  char cVar4;
  undefined2 *unaff_SI;
  undefined2 *unaff_DI;
  undefined2 *puVar5;
  undefined2 *puVar6;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  if (in_CL == 0) {
    out(0x7c,0xc0);
    out(0x7e,0);
    out(0x7e,0);
    out(0x7e,0);
    out(0x7e,0);
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
    out(0x7c,199);
    *unaff_DI = unaff_SI[0x40];
    unaff_DI[0x28] = unaff_SI[0x41];
    unaff_DI[0x50] = unaff_SI[0x42];
    unaff_DI[0x78] = unaff_SI[0x43];
    unaff_DI[0xa0] = unaff_SI[0x44];
    unaff_DI[200] = unaff_SI[0x45];
    unaff_DI[0xf0] = unaff_SI[0x46];
    unaff_DI[0x118] = unaff_SI[0x47];
    unaff_DI[0x140] = unaff_SI[0x48];
    unaff_DI[0x168] = unaff_SI[0x49];
    unaff_DI[400] = unaff_SI[0x4a];
    unaff_DI[0x1b8] = unaff_SI[0x4b];
    unaff_DI[0x1e0] = unaff_SI[0x4c];
    unaff_DI[0x208] = unaff_SI[0x4d];
    unaff_DI[0x230] = unaff_SI[0x4e];
    unaff_DI[600] = unaff_SI[0x4f];
    out(0x7c,0);
    return 0;
  }
  out(0x7c,0xc0);
  out(0x7e,0);
  out(0x7e,0);
  out(0x7e,0);
  out(0x7e,0);
  cVar4 = '\x10';
  do {
    puVar5 = unaff_DI;
    puVar1 = unaff_SI;
    unaff_SI = unaff_SI + 1;
    bVar3 = (byte)((uint)*puVar1 >> 8);
    uVar2 = CONCAT11((char)*puVar1,bVar3) >> (in_CL & 0x1f);
    *puVar5 = CONCAT11((char)uVar2,(char)(uVar2 >> 8));
    *(char *)(puVar5 + 1) = (char)(((uint)bVar3 << 8) >> (in_CL & 0x1f));
    cVar4 = cVar4 + -1;
    unaff_DI = puVar5 + 0x28;
  } while (cVar4 != '\0');
  out(0x7e,0xff);
  out(0x7e,0xff);
  out(0x7e,0xff);
  out(0x7e,0xff);
  out(0x7c,0xce);
  cVar4 = '\x10';
  puVar5 = puVar5 + -600;
  do {
    puVar6 = puVar5;
    puVar1 = unaff_SI;
    unaff_SI = unaff_SI + 1;
    bVar3 = (byte)((uint)*puVar1 >> 8);
    uVar2 = CONCAT11((char)*puVar1,bVar3) >> (in_CL & 0x1f);
    *puVar6 = CONCAT11((char)uVar2,(char)(uVar2 >> 8));
    *(char *)(puVar6 + 1) = (char)(((uint)bVar3 << 8) >> (in_CL & 0x1f));
    cVar4 = cVar4 + -1;
    puVar5 = puVar6 + 0x28;
  } while (cVar4 != '\0');
  out(0x7c,0xcd);
  cVar4 = '\x10';
  puVar5 = puVar6 + -600;
  do {
    puVar6 = puVar5;
    puVar1 = unaff_SI;
    unaff_SI = unaff_SI + 1;
    bVar3 = (byte)((uint)*puVar1 >> 8);
    uVar2 = CONCAT11((char)*puVar1,bVar3) >> (in_CL & 0x1f);
    *puVar6 = CONCAT11((char)uVar2,(char)(uVar2 >> 8));
    *(char *)(puVar6 + 1) = (char)(((uint)bVar3 << 8) >> (in_CL & 0x1f));
    cVar4 = cVar4 + -1;
    puVar5 = puVar6 + 0x28;
  } while (cVar4 != '\0');
  out(0x7c,0xcb);
  cVar4 = '\x10';
  puVar5 = puVar6 + -600;
  do {
    puVar6 = puVar5;
    puVar1 = unaff_SI;
    unaff_SI = unaff_SI + 1;
    bVar3 = (byte)((uint)*puVar1 >> 8);
    uVar2 = CONCAT11((char)*puVar1,bVar3) >> (in_CL & 0x1f);
    *puVar6 = CONCAT11((char)uVar2,(char)(uVar2 >> 8));
    *(char *)(puVar6 + 1) = (char)(((uint)bVar3 << 8) >> (in_CL & 0x1f));
    cVar4 = cVar4 + -1;
    puVar5 = puVar6 + 0x28;
  } while (cVar4 != '\0');
  puVar6 = puVar6 + -600;
  out(0x7c,199);
  cVar4 = '\x10';
  do {
    puVar1 = unaff_SI;
    unaff_SI = unaff_SI + 1;
    bVar3 = (byte)((uint)*puVar1 >> 8);
    uVar2 = CONCAT11((char)*puVar1,bVar3) >> (in_CL & 0x1f);
    *puVar6 = CONCAT11((char)uVar2,(char)(uVar2 >> 8));
    *(char *)(puVar6 + 1) = (char)(((uint)bVar3 << 8) >> (in_CL & 0x1f));
    puVar6 = puVar6 + 0x28;
    cVar4 = cVar4 + -1;
  } while (cVar4 != '\0');
  out(0x7c,0);
  return 0;
}



/* 0000:725b  FUN_0000_725b */

void __cdecl16near FUN_0000_725b(void)

{
  int unaff_SI;
  undefined2 *unaff_DI;
  undefined2 unaff_DS;
  
  *unaff_DI = *(undefined2 *)(unaff_SI + 0x20);
  unaff_DI[0x28] = *(undefined2 *)(unaff_SI + 0x22);
  unaff_DI[0x50] = *(undefined2 *)(unaff_SI + 0x24);
  unaff_DI[0x78] = *(undefined2 *)(unaff_SI + 0x26);
  unaff_DI[0xa0] = *(undefined2 *)(unaff_SI + 0x28);
  unaff_DI[200] = *(undefined2 *)(unaff_SI + 0x2a);
  unaff_DI[0xf0] = *(undefined2 *)(unaff_SI + 0x2c);
  unaff_DI[0x118] = *(undefined2 *)(unaff_SI + 0x2e);
  unaff_DI[0x140] = *(undefined2 *)(unaff_SI + 0x30);
  unaff_DI[0x168] = *(undefined2 *)(unaff_SI + 0x32);
  unaff_DI[400] = *(undefined2 *)(unaff_SI + 0x34);
  unaff_DI[0x1b8] = *(undefined2 *)(unaff_SI + 0x36);
  unaff_DI[0x1e0] = *(undefined2 *)(unaff_SI + 0x38);
  unaff_DI[0x208] = *(undefined2 *)(unaff_SI + 0x3a);
  unaff_DI[0x230] = *(undefined2 *)(unaff_SI + 0x3c);
  unaff_DI[600] = *(undefined2 *)(unaff_SI + 0x3e);
  *unaff_DI = *(undefined2 *)(unaff_SI + 0x40);
  unaff_DI[0x28] = *(undefined2 *)(unaff_SI + 0x42);
  unaff_DI[0x50] = *(undefined2 *)(unaff_SI + 0x44);
  unaff_DI[0x78] = *(undefined2 *)(unaff_SI + 0x46);
  unaff_DI[0xa0] = *(undefined2 *)(unaff_SI + 0x48);
  unaff_DI[200] = *(undefined2 *)(unaff_SI + 0x4a);
  unaff_DI[0xf0] = *(undefined2 *)(unaff_SI + 0x4c);
  unaff_DI[0x118] = *(undefined2 *)(unaff_SI + 0x4e);
  unaff_DI[0x140] = *(undefined2 *)(unaff_SI + 0x50);
  unaff_DI[0x168] = *(undefined2 *)(unaff_SI + 0x52);
  unaff_DI[400] = *(undefined2 *)(unaff_SI + 0x54);
  unaff_DI[0x1b8] = *(undefined2 *)(unaff_SI + 0x56);
  unaff_DI[0x1e0] = *(undefined2 *)(unaff_SI + 0x58);
  unaff_DI[0x208] = *(undefined2 *)(unaff_SI + 0x5a);
  unaff_DI[0x230] = *(undefined2 *)(unaff_SI + 0x5c);
  unaff_DI[600] = *(undefined2 *)(unaff_SI + 0x5e);
  unaff_DI[-0x4000] = *(undefined2 *)(unaff_SI + 0x60);
  unaff_DI[-0x3fd8] = *(undefined2 *)(unaff_SI + 0x62);
  unaff_DI[-0x3fb0] = *(undefined2 *)(unaff_SI + 100);
  unaff_DI[-0x3f88] = *(undefined2 *)(unaff_SI + 0x66);
  unaff_DI[-0x3f60] = *(undefined2 *)(unaff_SI + 0x68);
  unaff_DI[-0x3f38] = *(undefined2 *)(unaff_SI + 0x6a);
  unaff_DI[-0x3f10] = *(undefined2 *)(unaff_SI + 0x6c);
  unaff_DI[-0x3ee8] = *(undefined2 *)(unaff_SI + 0x6e);
  unaff_DI[-0x3ec0] = *(undefined2 *)(unaff_SI + 0x70);
  unaff_DI[-0x3e98] = *(undefined2 *)(unaff_SI + 0x72);
  unaff_DI[-0x3e70] = *(undefined2 *)(unaff_SI + 0x74);
  unaff_DI[-0x3e48] = *(undefined2 *)(unaff_SI + 0x76);
  unaff_DI[-0x3e20] = *(undefined2 *)(unaff_SI + 0x78);
  unaff_DI[-0x3df8] = *(undefined2 *)(unaff_SI + 0x7a);
  unaff_DI[-0x3dd0] = *(undefined2 *)(unaff_SI + 0x7c);
  unaff_DI[-0x3da8] = *(undefined2 *)(unaff_SI + 0x7e);
  *unaff_DI = *(undefined2 *)(unaff_SI + 0x80);
  unaff_DI[0x28] = *(undefined2 *)(unaff_SI + 0x82);
  unaff_DI[0x50] = *(undefined2 *)(unaff_SI + 0x84);
  unaff_DI[0x78] = *(undefined2 *)(unaff_SI + 0x86);
  unaff_DI[0xa0] = *(undefined2 *)(unaff_SI + 0x88);
  unaff_DI[200] = *(undefined2 *)(unaff_SI + 0x8a);
  unaff_DI[0xf0] = *(undefined2 *)(unaff_SI + 0x8c);
  unaff_DI[0x118] = *(undefined2 *)(unaff_SI + 0x8e);
  unaff_DI[0x140] = *(undefined2 *)(unaff_SI + 0x90);
  unaff_DI[0x168] = *(undefined2 *)(unaff_SI + 0x92);
  unaff_DI[400] = *(undefined2 *)(unaff_SI + 0x94);
  unaff_DI[0x1b8] = *(undefined2 *)(unaff_SI + 0x96);
  unaff_DI[0x1e0] = *(undefined2 *)(unaff_SI + 0x98);
  unaff_DI[0x208] = *(undefined2 *)(unaff_SI + 0x9a);
  unaff_DI[0x230] = *(undefined2 *)(unaff_SI + 0x9c);
  unaff_DI[600] = *(undefined2 *)(unaff_SI + 0x9e);
  return;
}



/* 0000:7342  FUN_0000_7342 */

void __cdecl16near FUN_0000_7342(void)

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
  return;
}



/* 0000:7428  blit_expand */

void __cdecl16far blit_expand(void)

{
  char in_AL;
  byte in_BL;
  byte in_BH;
  
  if ((in_AL == '\0') && (((in_BH & 0x10) == 0 || ((in_BL & 0x10) == 0)))) {
    FUN_0000_7448();
  }
  else {
    FUN_0000_7541();
  }
  return;
}



/* 0000:7448  FUN_0000_7448 */

void __cdecl16near FUN_0000_7448(void)

{
  undefined2 *puVar1;
  undefined2 *puVar2;
  byte in_CL;
  uint uVar3;
  int in_DX;
  int iVar4;
  byte in_BL;
  byte in_BH;
  byte bVar5;
  undefined2 *unaff_SI;
  undefined2 *unaff_DI;
  undefined2 *puVar6;
  undefined2 unaff_DS;
  
  bVar5 = 0x50 - in_CL;
  if ((in_BH & 0x10) != 0) {
    unaff_SI = (undefined2 *)((int)unaff_SI + in_DX * (uint)in_CL);
  }
  DAT_0000_7426 = in_BL;
  DAT_0000_7427 = in_BH;
  if ((in_BH & 1) != 0) {
    iVar4 = in_DX;
    puVar6 = unaff_DI;
    if ((in_BL & 1) == 0) {
      unaff_SI = (undefined2 *)((int)unaff_SI + in_DX * (uint)in_CL);
    }
    else {
      do {
        uVar3 = (uint)(in_CL >> 1);
        if ((bool)(in_CL & 1)) {
          puVar2 = puVar6;
          puVar6 = (undefined2 *)((int)puVar6 + 1);
          puVar1 = unaff_SI;
          unaff_SI = (undefined2 *)((int)unaff_SI + 1);
          *(undefined1 *)puVar2 = *(undefined1 *)puVar1;
        }
        for (; uVar3 != 0; uVar3 = uVar3 - 1) {
          puVar2 = puVar6;
          puVar6 = puVar6 + 1;
          puVar1 = unaff_SI;
          unaff_SI = unaff_SI + 1;
          *puVar2 = *puVar1;
        }
        iVar4 = iVar4 + -1;
        puVar6 = (undefined2 *)((int)puVar6 + (uint)bVar5);
      } while (iVar4 != 0);
    }
  }
  if ((DAT_0000_7427 & 2) != 0) {
    iVar4 = in_DX;
    puVar6 = unaff_DI;
    if ((DAT_0000_7426 & 2) == 0) {
      unaff_SI = (undefined2 *)((int)unaff_SI + in_DX * (uint)in_CL);
    }
    else {
      do {
        uVar3 = (uint)(in_CL >> 1);
        if ((bool)(in_CL & 1)) {
          puVar2 = puVar6;
          puVar6 = (undefined2 *)((int)puVar6 + 1);
          puVar1 = unaff_SI;
          unaff_SI = (undefined2 *)((int)unaff_SI + 1);
          *(undefined1 *)puVar2 = *(undefined1 *)puVar1;
        }
        for (; uVar3 != 0; uVar3 = uVar3 - 1) {
          puVar2 = puVar6;
          puVar6 = puVar6 + 1;
          puVar1 = unaff_SI;
          unaff_SI = unaff_SI + 1;
          *puVar2 = *puVar1;
        }
        iVar4 = iVar4 + -1;
        puVar6 = (undefined2 *)((int)puVar6 + (uint)bVar5);
      } while (iVar4 != 0);
    }
  }
  if ((DAT_0000_7427 & 4) != 0) {
    iVar4 = in_DX;
    puVar6 = unaff_DI;
    if ((DAT_0000_7426 & 4) == 0) {
      unaff_SI = (undefined2 *)((int)unaff_SI + in_DX * (uint)in_CL);
    }
    else {
      do {
        uVar3 = (uint)(in_CL >> 1);
        if ((bool)(in_CL & 1)) {
          puVar2 = puVar6;
          puVar6 = (undefined2 *)((int)puVar6 + 1);
          puVar1 = unaff_SI;
          unaff_SI = (undefined2 *)((int)unaff_SI + 1);
          *(undefined1 *)puVar2 = *(undefined1 *)puVar1;
        }
        for (; uVar3 != 0; uVar3 = uVar3 - 1) {
          puVar2 = puVar6;
          puVar6 = puVar6 + 1;
          puVar1 = unaff_SI;
          unaff_SI = unaff_SI + 1;
          *puVar2 = *puVar1;
        }
        iVar4 = iVar4 + -1;
        puVar6 = (undefined2 *)((int)puVar6 + (uint)bVar5);
      } while (iVar4 != 0);
    }
  }
  if (((DAT_0000_7427 & 8) != 0) && ((DAT_0000_7426 & 8) != 0)) {
    do {
      uVar3 = (uint)(in_CL >> 1);
      if ((bool)(in_CL & 1)) {
        puVar2 = unaff_DI;
        unaff_DI = (undefined2 *)((int)unaff_DI + 1);
        puVar1 = unaff_SI;
        unaff_SI = (undefined2 *)((int)unaff_SI + 1);
        *(undefined1 *)puVar2 = *(undefined1 *)puVar1;
      }
      for (; uVar3 != 0; uVar3 = uVar3 - 1) {
        puVar2 = unaff_DI;
        unaff_DI = unaff_DI + 1;
        puVar1 = unaff_SI;
        unaff_SI = unaff_SI + 1;
        *puVar2 = *puVar1;
      }
      unaff_DI = (undefined2 *)((int)unaff_DI + (uint)bVar5);
      in_DX = in_DX + -1;
    } while (in_DX != 0);
  }
  return;
}



/* 0000:7541  FUN_0000_7541 */

undefined2 __cdecl16near FUN_0000_7541(void)

{
  byte *pbVar1;
  byte *pbVar2;
  byte in_AL;
  int iVar3;
  uint uVar4;
  byte in_CL;
  uint uVar5;
  byte bVar6;
  int in_DX;
  uint in_BX;
  int iVar7;
  byte *unaff_SI;
  byte *unaff_DI;
  byte *pbVar8;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  bVar6 = (byte)in_BX;
  DAT_0000_7427 = (byte)(in_BX >> 8);
  if (in_AL == 0) {
    bVar6 = 0x50 - in_CL;
    out(0x7c,0xc0);
    iVar3 = 0;
    out(0x7e,0);
    out(0x7e,0);
    out(0x7e,0);
    out(0x7e,0);
    iVar7 = in_DX;
    pbVar8 = unaff_DI;
    if ((in_BX & 0x10) == 0) {
      if ((in_BX & 0x1000) != 0) {
        iVar3 = in_DX * (uint)in_CL;
        unaff_SI = unaff_SI + iVar3;
      }
      do {
        uVar4 = (uint)(in_CL >> 1);
        if ((bool)(in_CL & 1)) {
          pbVar2 = pbVar8;
          pbVar8 = pbVar8 + 1;
          *pbVar2 = (byte)iVar3;
        }
        for (; uVar4 != 0; uVar4 = uVar4 - 1) {
          pbVar2 = pbVar8;
          pbVar8 = pbVar8 + 2;
          *(int *)pbVar2 = iVar3;
        }
        iVar7 = iVar7 + -1;
        pbVar8 = pbVar8 + bVar6;
      } while (iVar7 != 0);
    }
    else {
      do {
        uVar4 = (uint)(in_CL >> 1);
        if ((bool)(in_CL & 1)) {
          pbVar1 = pbVar8;
          pbVar8 = pbVar8 + 1;
          pbVar2 = unaff_SI;
          unaff_SI = unaff_SI + 1;
          *pbVar1 = *pbVar2;
        }
        for (; uVar4 != 0; uVar4 = uVar4 - 1) {
          pbVar1 = pbVar8;
          pbVar8 = pbVar8 + 2;
          pbVar2 = unaff_SI;
          unaff_SI = unaff_SI + 2;
          *(int *)pbVar1 = *(int *)pbVar2;
        }
        iVar7 = iVar7 + -1;
        pbVar8 = pbVar8 + bVar6;
      } while (iVar7 != 0);
    }
    uVar4 = (uint)in_CL;
    out(0x7e,0xff);
    out(0x7e,0xff);
    out(0x7e,0xff);
    out(0x7e,0xff);
    if ((in_BX & 1) == 0) {
      if ((DAT_0000_7427 & 1) != 0) {
        unaff_SI = unaff_SI + in_DX * uVar4;
      }
    }
    else {
      out(0x7c,0xce);
      iVar3 = in_DX;
      pbVar8 = unaff_DI;
      do {
        uVar5 = (uint)(in_CL >> 1);
        if ((bool)(in_CL & 1)) {
          pbVar1 = pbVar8;
          pbVar8 = pbVar8 + 1;
          pbVar2 = unaff_SI;
          unaff_SI = unaff_SI + 1;
          *pbVar1 = *pbVar2;
        }
        for (; uVar5 != 0; uVar5 = uVar5 - 1) {
          pbVar1 = pbVar8;
          pbVar8 = pbVar8 + 2;
          pbVar2 = unaff_SI;
          unaff_SI = unaff_SI + 2;
          *(int *)pbVar1 = *(int *)pbVar2;
        }
        pbVar8 = pbVar8 + bVar6;
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
    }
    if ((in_BX & 2) == 0) {
      if ((DAT_0000_7427 & 2) != 0) {
        unaff_SI = unaff_SI + in_DX * uVar4;
      }
    }
    else {
      out(0x7c,0xcd);
      iVar3 = in_DX;
      pbVar8 = unaff_DI;
      do {
        uVar5 = (uint)(in_CL >> 1);
        if ((bool)(in_CL & 1)) {
          pbVar1 = pbVar8;
          pbVar8 = pbVar8 + 1;
          pbVar2 = unaff_SI;
          unaff_SI = unaff_SI + 1;
          *pbVar1 = *pbVar2;
        }
        for (; uVar5 != 0; uVar5 = uVar5 - 1) {
          pbVar1 = pbVar8;
          pbVar8 = pbVar8 + 2;
          pbVar2 = unaff_SI;
          unaff_SI = unaff_SI + 2;
          *(int *)pbVar1 = *(int *)pbVar2;
        }
        pbVar8 = pbVar8 + bVar6;
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
    }
    if ((in_BX & 4) == 0) {
      if ((DAT_0000_7427 & 4) != 0) {
        unaff_SI = unaff_SI + in_DX * uVar4;
      }
    }
    else {
      out(0x7c,0xcb);
      iVar3 = in_DX;
      pbVar8 = unaff_DI;
      do {
        uVar4 = (uint)(in_CL >> 1);
        if ((bool)(in_CL & 1)) {
          pbVar1 = pbVar8;
          pbVar8 = pbVar8 + 1;
          pbVar2 = unaff_SI;
          unaff_SI = unaff_SI + 1;
          *pbVar1 = *pbVar2;
        }
        for (; uVar4 != 0; uVar4 = uVar4 - 1) {
          pbVar1 = pbVar8;
          pbVar8 = pbVar8 + 2;
          pbVar2 = unaff_SI;
          unaff_SI = unaff_SI + 2;
          *(int *)pbVar1 = *(int *)pbVar2;
        }
        pbVar8 = pbVar8 + bVar6;
        iVar3 = iVar3 + -1;
      } while (iVar3 != 0);
    }
    if ((in_BX & 8) != 0) {
      out(0x7c,199);
      do {
        uVar4 = (uint)(in_CL >> 1);
        if ((bool)(in_CL & 1)) {
          pbVar1 = unaff_DI;
          unaff_DI = unaff_DI + 1;
          pbVar2 = unaff_SI;
          unaff_SI = unaff_SI + 1;
          *pbVar1 = *pbVar2;
        }
        for (; uVar4 != 0; uVar4 = uVar4 - 1) {
          pbVar1 = unaff_DI;
          unaff_DI = unaff_DI + 2;
          pbVar2 = unaff_SI;
          unaff_SI = unaff_SI + 2;
          *(int *)pbVar1 = *(int *)pbVar2;
        }
        unaff_DI = unaff_DI + bVar6;
        in_DX = in_DX + -1;
      } while (in_DX != 0);
    }
  }
  else {
    out(0x7c,0xc0);
    out(0x7e,0);
    out(0x7e,0);
    out(0x7e,0);
    out(0x7e,0);
    DAT_0000_7426 = bVar6;
    if (((in_BX & 0x1000) == 0) || ((in_BX & 0x10) == 0)) {
      do {
        bVar6 = in_CL >> 1;
        if (!(bool)(in_CL & 1)) goto LAB_0000_76cb;
        pbVar2 = unaff_DI;
        unaff_DI = unaff_DI + 1;
        *pbVar2 = 0xff >> (in_AL & 0x1f);
        for (; bVar6 != 0; bVar6 = bVar6 - 1) {
LAB_0000_76cb:
          uVar4 = 0xffff >> (in_AL & 0x1f);
          pbVar2 = unaff_DI;
          unaff_DI = unaff_DI + 2;
          *(int *)pbVar2 =
               CONCAT11((char)uVar4,
                        (byte)(uVar4 >> 8) |
                        (byte)((0xffffU >> (in_AL & 0xf)) >> 8) |
                        (byte)((uint)(-1 << 0x10 - (in_AL & 0xf)) >> 8));
        }
        *unaff_DI = (byte)(0xff00 >> (in_AL & 0x1f));
        unaff_DI = unaff_DI + (byte)(0x4f - in_CL) + 1;
        in_DX = in_DX + -1;
      } while (in_DX != 0);
    }
    else {
      FUN_0000_7795();
    }
    out(0x7e,0xff);
    out(0x7e,0xff);
    out(0x7e,0xff);
    out(0x7e,0xff);
    if (((DAT_0000_7427 & 1) != 0) && ((DAT_0000_7426 & 1) != 0)) {
      out(0x7c,0xce);
      FUN_0000_7795();
    }
    if ((DAT_0000_7426 & 2) != 0) {
      out(0x7c,0xcd);
      FUN_0000_7795();
    }
    if ((DAT_0000_7426 & 4) != 0) {
      out(0x7c,0xcb);
      FUN_0000_7795();
    }
    if ((DAT_0000_7426 & 8) != 0) {
      out(0x7c,199);
      FUN_0000_7795();
    }
  }
  out(0x7c,0);
  return 0;
}



/* 0000:7795  FUN_0000_7795 */

void __cdecl16near FUN_0000_7795(void)

{
  byte *pbVar1;
  undefined1 uVar3;
  uint uVar2;
  byte in_CL;
  byte in_CH;
  byte bVar4;
  byte in_DL;
  byte in_DH;
  uint uVar5;
  byte bVar6;
  int unaff_BP;
  byte *unaff_SI;
  byte *pbVar7;
  byte *unaff_DI;
  undefined2 unaff_ES;
  undefined2 unaff_DS;
  
  do {
    uVar5 = 0;
    bVar4 = in_CH >> 1;
    pbVar7 = unaff_SI;
    if ((bool)(in_CH & 1)) {
      pbVar7 = unaff_SI + 1;
      pbVar1 = unaff_DI;
      unaff_DI = unaff_DI + 1;
      *pbVar1 = *unaff_SI >> (in_CL & 0x1f);
      if (bVar4 != 0) {
        uVar5 = (uint)*unaff_SI;
        goto LAB_0000_77a8;
      }
    }
    else {
LAB_0000_77a8:
      do {
        pbVar1 = pbVar7;
        pbVar7 = pbVar7 + 2;
        uVar3 = (undefined1)((uint)*(undefined2 *)pbVar1 >> 8);
        bVar6 = (byte)(((uVar5 & 0xff) >> (in_CL & 0xf)) >> 8) |
                (byte)(((uVar5 & 0xff) << 0x10 - (in_CL & 0xf)) >> 8);
        uVar5 = CONCAT11(bVar6,uVar3);
        uVar2 = CONCAT11((char)*(undefined2 *)pbVar1,uVar3) >> (in_CL & 0x1f);
        pbVar1 = unaff_DI;
        unaff_DI = unaff_DI + 2;
        *(uint *)pbVar1 = CONCAT11((char)uVar2,(byte)(uVar2 >> 8) | bVar6);
        bVar4 = bVar4 - 1;
      } while (bVar4 != 0);
    }
    *unaff_DI = (byte)(((uint)pbVar7[-1] << 8) >> (in_CL & 0x1f));
    unaff_DI = unaff_DI + in_DL + 1;
    unaff_BP = unaff_BP + -1;
    in_CH = in_DH;
    unaff_SI = pbVar7;
    if (unaff_BP == 0) {
      return;
    }
  } while( true );
}



/* 0000:77d4  fill_vram_rect */

void __cdecl16far fill_vram_rect(void)

{
  FUN_0000_77da();
  return;
}



/* 0000:77da  FUN_0000_77da */

undefined2 __cdecl16near FUN_0000_77da(void)

{
  undefined1 *puVar1;
  byte in_AL;
  byte bVar2;
  int in_CX;
  int iVar3;
  int in_DX;
  undefined1 *unaff_DI;
  
  bVar2 = ~in_AL;
  out(0x7c,0xc0);
  out(0x7e,(bVar2 & 1) - 1);
  out(0x7e,(bVar2 >> 1 & 1) - 1);
  out(0x7e,(bVar2 >> 2 & 1) - 1);
  out(0x7e,(bVar2 >> 3 & 1) - 1);
  iVar3 = in_CX;
  do {
    for (; iVar3 != 0; iVar3 = iVar3 + -1) {
      puVar1 = unaff_DI;
      unaff_DI = unaff_DI + 1;
      *puVar1 = 0xff;
    }
    unaff_DI = unaff_DI + (0x50 - in_CX);
    in_DX = in_DX + -1;
    iVar3 = in_CX;
  } while (in_DX != 0);
  out(0x7c,0);
  return 0;
}



/* 0000:7846  poll_input */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far poll_input(void)

{
  code *pcVar1;
  uint uVar2;
  int iVar3;
  byte bVar4;
  uint uVar5;
  undefined2 unaff_CS;
  
  *(uint *)0x7836 = _DAT_0000_052a;
  *(uint *)0x7838 = DAT_0000_052c;
  *(uint *)0x783a = DAT_0000_052e;
  *(uint *)0x783c = _DAT_0000_0530;
  *(uint *)0x783e = _DAT_0000_0532;
  *(uint *)0x7840 = DAT_0000_0534;
  *(uint *)0x7842 = DAT_0000_0536;
  *(uint *)0x7844 = DAT_0000_0538;
  iVar3 = 0x2c0;
  do {
    out(0x5f,(char)unaff_CS);
    iVar3 = iVar3 + -1;
  } while (iVar3 != 0);
  *(uint *)0x7836 = *(uint *)0x7836 | _DAT_0000_052a;
  *(uint *)0x7838 = *(uint *)0x7838 | DAT_0000_052c;
  *(uint *)0x783a = *(uint *)0x783a | DAT_0000_052e;
  *(uint *)0x783c = *(uint *)0x783c | _DAT_0000_0530;
  *(uint *)0x783e = *(uint *)0x783e | _DAT_0000_0532;
  *(uint *)0x7840 = *(uint *)0x7840 | DAT_0000_0534;
  *(uint *)0x7842 = *(uint *)0x7842 | DAT_0000_0536;
  *(uint *)0x7844 = *(uint *)0x7844 | DAT_0000_0538;
  *(undefined2 *)0x7830 = *(undefined2 *)&DAT_0000_03b8;
  *(undefined1 *)0x7835 = *(undefined1 *)0x7834;
  uVar5 = 0;
  bVar4 = *(byte *)0x7836;
  if ((bVar4 & 1) != 0) {
    uVar5 = 0x80;
  }
  if ((bVar4 & 2) != 0) {
    uVar5 = uVar5 | 0x2000;
  }
  if ((bVar4 & 4) != 0) {
    uVar5 = uVar5 | 0x1000;
  }
  if ((bVar4 & 8) != 0) {
    uVar5 = uVar5 | 0x800;
  }
  if ((bVar4 & 0x10) != 0) {
    uVar5 = uVar5 | 0x400;
  }
  if ((*(byte *)0x7839 & 0x10) != 0) {
    uVar5 = uVar5 | 0x20;
  }
  bVar4 = *(byte *)0x783b;
  if ((bVar4 & 2) != 0) {
    uVar5 = uVar5 | 0x10;
  }
  if ((bVar4 & 4) != 0) {
    uVar5 = uVar5 | 0x20;
  }
  if ((bVar4 & 0x40) != 0) {
    uVar5 = uVar5 | 0x20;
  }
  if ((bVar4 & 0x80) != 0) {
    uVar5 = uVar5 | 0x20;
  }
  if ((*(byte *)0x783c & 0x10) != 0) {
    uVar5 = uVar5 | 0x10;
  }
  if ((*(byte *)0x783f & 0x40) != 0) {
    uVar5 = uVar5 | 0x10;
  }
  if ((*(byte *)0x7843 & 8) != 0) {
    uVar5 = uVar5 | 0x8000;
  }
  if ((*(byte *)0x7844 & 1) != 0) {
    uVar5 = uVar5 | 0x40;
  }
  uVar2 = *(uint *)0x783d;
  if ((uVar2 & 0x804) != 0) {
    uVar5 = uVar5 | 1;
  }
  if ((uVar2 & 0x4008) != 0) {
    uVar5 = uVar5 | 2;
  }
  uVar2 = CONCAT11(*(undefined1 *)0x783f,(char)uVar2);
  if ((uVar2 & 0x110) != 0) {
    uVar5 = uVar5 | 4;
  }
  if ((uVar2 & 0x820) != 0) {
    uVar5 = uVar5 | 8;
  }
  if ((*(uint *)0x3be & 0x400) != 0) {
    pcVar1 = (code *)swi(0x7b);
    uVar2 = (*pcVar1)();
    if ((uVar2 & 1) != 0) {
      uVar5 = uVar5 | 1;
    }
    if ((uVar2 & 2) != 0) {
      uVar5 = uVar5 | 8;
    }
    if ((uVar2 & 4) != 0) {
      uVar5 = uVar5 | 2;
    }
    if ((uVar2 & 8) != 0) {
      uVar5 = uVar5 | 4;
    }
    if ((uVar2 & 0x10) != 0) {
      uVar5 = uVar5 | 0x10;
    }
    if ((uVar2 & 0x20) != 0) {
      uVar5 = uVar5 | 0x20;
    }
  }
  uVar2 = uVar5;
  if ((*(uint *)0x3be & 0x200) != 0) {
    uVar2 = uVar5 & 0xffcf;
    if ((uVar5 & 0x10) != 0) {
      uVar2 = uVar2 | 0x20;
    }
    if ((uVar5 & 0x20) != 0) {
      uVar2 = uVar2 | 0x10;
    }
  }
  *(uint *)&DAT_0000_03b8 = uVar2;
  uVar2 = (~*(uint *)0x7830 | 0x40) & uVar2;
  *(uint *)&DAT_0000_03ba = uVar2;
  *(uint *)0x3bc = uVar2;
  if ((uVar2 & 0xffbf) == 0) {
    iVar3 = *(int *)0x7832;
    uVar5 = iVar3 + 1U & 0x7fff;
    *(uint *)0x7832 = uVar5;
    if ((0xf < uVar5) && ((iVar3 + 1U & 1) == 0)) {
      *(undefined2 *)0x3bc = *(undefined2 *)&DAT_0000_03b8;
    }
  }
  else {
    *(undefined2 *)0x7832 = 0;
  }
  bVar4 = 0;
  if ((*(byte *)0x7838 & 4) != 0) {
    bVar4 = 2;
  }
  if ((*(byte *)0x7839 & 1) != 0) {
    bVar4 = bVar4 | 0x10;
  }
  if ((*(byte *)0x783a & 0x10) != 0) {
    bVar4 = bVar4 | 4;
  }
  if ((*(byte *)0x783b & 8) != 0) {
    bVar4 = bVar4 | 1;
  }
  if ((*(byte *)0x783b & 0x40) != 0) {
    bVar4 = bVar4 | 8;
  }
  *(byte *)0x7834 = bVar4;
  *(byte *)0x40e = ~*(byte *)0x7835 & bVar4;
  return;
}


