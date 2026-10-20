/*
 * Ghidra 12.1.4 headless decompile.
 * Program: SGC.COM
 * Language: x86:LE:16:Real Mode
 * Image base: 0x100 (.COM load address)
 * Pseudo-C is a guide. Trust docs/re/MAP.md where they disagree.
 */

/* 0000:0100  com_stub */

void __cdecl16far com_stub(void)

{
  code *pcVar1;
  code *pcVar2;
  int iVar3;
  code *pcVar4;
  code *pcVar5;
  undefined2 unaff_CS;
  
  pcVar4 = (code *)com_stub;
  pcVar5 = (code *)com_stub;
  for (iVar3 = 0x7f80; iVar3 != 0; iVar3 = iVar3 + -1) {
    pcVar2 = pcVar5;
    pcVar5 = pcVar5 + 2;
    pcVar1 = pcVar4;
    pcVar4 = pcVar4 + 2;
    *(undefined2 *)pcVar2 = *(undefined2 *)pcVar1;
  }
  return;
}



/* 0000:011b  com_stub_relocated */

undefined2 __cdecl16far com_stub_relocated(void)

{
  byte *pbVar1;
  byte bVar2;
  byte *pbVar3;
  int iVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  byte *pbVar8;
  byte *pbVar9;
  byte *pbVar10;
  byte *pbVar11;
  undefined2 unaff_CS;
  int unaff_DS;
  bool bVar12;
  undefined2 in_stack_00000010;
  
  iVar4 = unaff_DS + 0x10;
  pbVar11 = (byte *)0x0;
  uVar5 = 0;
  pbVar8 = (byte *)0x16e;
LAB_0000_012d:
  while( true ) {
    pbVar9 = pbVar8 + 1;
    bVar2 = *pbVar8;
    if (bVar2 == 0) {
      return in_stack_00000010;
    }
    if ((char)bVar2 < '\0') break;
    for (iVar6 = CONCAT11((char)(uVar5 >> 8),bVar2); iVar6 != 0; iVar6 = iVar6 + -1) {
      pbVar3 = pbVar11;
      pbVar11 = pbVar11 + 1;
      pbVar1 = pbVar9;
      pbVar9 = pbVar9 + 1;
      *pbVar3 = *pbVar1;
    }
    uVar5 = 0;
    pbVar8 = pbVar9;
  }
  uVar7 = (uint)(byte)((bVar2 & 0x7f) + 3);
  pbVar8 = pbVar8 + 2;
  pbVar10 = pbVar11 + -(int)(*pbVar9 + 1);
  if (pbVar11 < (byte *)(*pbVar9 + 1)) goto LAB_0000_015b;
  goto LAB_0000_0151;
LAB_0000_015b:
  do {
    pbVar1 = pbVar11;
    pbVar11 = pbVar11 + 1;
    *pbVar1 = 0;
    pbVar10 = pbVar10 + 1;
    bVar12 = pbVar10 == (byte *)0x0;
    uVar7 = uVar7 - 1;
  } while (uVar7 != 0 && !bVar12);
  pbVar10 = (byte *)0x0;
  uVar5 = uVar7;
  if (bVar12) {
LAB_0000_0151:
    for (; uVar5 = 0, uVar7 != 0; uVar7 = uVar7 - 1) {
      pbVar3 = pbVar11;
      pbVar11 = pbVar11 + 1;
      pbVar1 = pbVar10;
      pbVar10 = pbVar10 + 1;
      *pbVar3 = *pbVar1;
    }
  }
  goto LAB_0000_012d;
}


