; Ghidra 12.1.4 disassembly of SGC.COM
; Language: x86:LE:16:Real Mode
; Image base: 0x100
0000:0100  fc                 CLD
0000:0101  60                 PUSHA
0000:0102  8c c8              MOV AX,CS
0000:0104  8e d8              MOV AX,DS
0000:0106  80 c4 10           ADD AH,0x10
0000:0109  8e c0              MOV AX,ES
0000:010b  be 00 01           MOV SI,0x100
0000:010e  8b fe              MOV DI,SI
0000:0110  b9 80 7f           MOV CX,0x7f80
0000:0113  f3 a5              MOVSW.REP ES:DI,SI
0000:0115  0e                 PUSH CS
0000:0116  06                 PUSH ES
0000:0117  68 1b 01           PUSH 0x11b
0000:011a  cb                 RETF
0000:011b  8c d8              MOV AX,DS
0000:011d  83 c0 10           ADD AX,0x10
0000:0120  8e c0              MOV AX,ES
0000:0122  8c c8              MOV AX,CS
0000:0124  8e d8              MOV AX,DS
0000:0126  be 6e 01           MOV SI,0x16e
0000:0129  33 c9              XOR CX,CX
0000:012b  33 ff              XOR DI,DI
0000:012d  ac                 LODSB SI
0000:012e  0a c0              OR AL,AL
0000:0130  74 31              JZ 0x0000:0163
0000:0132  78 06              JS 0x0000:013a
0000:0134  8a c8              MOV CL,AL
0000:0136  f3 a4              MOVSB.REP ES:DI,SI
0000:0138  eb f3              JMP 0x0000:012d
0000:013a  83 e0 7f           AND AX,0x7f
0000:013d  04 03              ADD AL,0x3
0000:013f  8b c8              MOV CX,AX
0000:0141  ac                 LODSB SI
0000:0142  40                 INC AX
0000:0143  8b de              MOV BX,SI
0000:0145  8c da              MOV DX,DS
0000:0147  8b f7              MOV SI,DI
0000:0149  2b f0              SUB SI,AX
0000:014b  8c c0              MOV AX,ES
0000:014d  8e d8              MOV AX,DS
0000:014f  72 08              JC 0x0000:0159
0000:0151  f3 a4              MOVSB.REP ES:DI,SI
0000:0153  8b f3              MOV SI,BX
0000:0155  8e da              MOV DX,DS
0000:0157  eb d4              JMP 0x0000:012d
0000:0159  32 c0              XOR AL,AL
0000:015b  aa                 STOSB ES:DI
0000:015c  46                 INC SI
0000:015d  e0 fc              LOOPNZ 0x0000:015b
0000:015f  74 f0              JZ 0x0000:0151
0000:0161  eb f0              JMP 0x0000:0153
0000:0163  58                 POP AX
0000:0164  8e c0              MOV AX,ES
0000:0166  8e d8              MOV AX,DS
0000:0168  61                 POPA
0000:0169  1e                 PUSH DS
0000:016a  68 00 01           PUSH 0x100
0000:016d  cb                 RETF
