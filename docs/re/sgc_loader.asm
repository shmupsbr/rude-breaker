; Ghidra 12.1.4 disassembly of sgc_loader.bin
; Language: x86:LE:16:Real Mode
; Image base: 0x100
0000:0100  e9 31 57           JMP 0x0000:5834
0000:5834  fc                 CLD
0000:5835  8c c8              MOV AX,CS
0000:5837  8e d8              MOV AX,DS
0000:5839  8e c0              MOV AX,ES
0000:583b  8e d0              MOV AX,SS
0000:583d  bc 4a 03           MOV SP,0x34a
0000:5840  bd 4c 03           MOV BP,0x34c
0000:5843  e8 1c 02           CALL 0x0000:5a62
0000:5846  b8 3f 7e           MOV AX,0x7e3f
0000:5849  c0 ec 02           SHR AH,0x2
0000:584c  80 fc 24           CMP AH,0x24
0000:584f  76 03              JBE 0x0000:5854
0000:5851  e9 7c 01           JMP 0x0000:59d0
0000:5854  bb 00 09           MOV BX,0x900
0000:5857  b4 4a              MOV AH,0x4a
0000:5859  cd 21              INT 0x21
0000:585b  73 03              JNC 0x0000:5860
0000:585d  e9 9f 01           JMP 0x0000:59ff
0000:5860  ba c1 57           MOV DX,0x57c1
0000:5863  b4 09              MOV AH,0x9
0000:5865  cd 21              INT 0x21
0000:5867  bb 00 a0           MOV BX,0xa000
0000:586a  b4 48              MOV AH,0x48
0000:586c  cd 21              INT 0x21
0000:586e  83 f8 08           CMP AX,0x8
0000:5871  74 03              JZ 0x0000:5876
0000:5873  e9 89 01           JMP 0x0000:59ff
0000:5876  89 5e 6a           MOV word ptr [BP + 0x6a],BX
0000:5879  81 fb c0 69        CMP BX,0x69c0
0000:587d  73 03              JNC 0x0000:5882
0000:587f  e9 31 01           JMP 0x0000:59b3
0000:5882  ba ed 57           MOV DX,0x57ed
0000:5885  b4 09              MOV AH,0x9
0000:5887  cd 21              INT 0x21
0000:5889  e8 16 08           CALL 0x0000:60a2
0000:588c  ba 26 57           MOV DX,0x5726
0000:588f  be 4e 57           MOV SI,0x574e
0000:5892  e8 8b 02           CALL 0x0000:5b20
0000:5895  73 05              JNC 0x0000:589c
0000:5897  e8 8b 01           CALL 0x0000:5a25
0000:589a  eb 04              JMP 0x0000:58a0
0000:589c  b4 4d              MOV AH,0x4d
0000:589e  cd 21              INT 0x21
0000:58a0  ba 31 57           MOV DX,0x5731
0000:58a3  be 4f 57           MOV SI,0x574f
0000:58a6  e8 77 02           CALL 0x0000:5b20
0000:58a9  73 05              JNC 0x0000:58b0
0000:58ab  e8 77 01           CALL 0x0000:5a25
0000:58ae  eb 04              JMP 0x0000:58b4
0000:58b0  b4 4d              MOV AH,0x4d
0000:58b2  cd 21              INT 0x21
0000:58b4  e8 17 0b           CALL 0x0000:63ce
0000:58b7  e8 8d 08           CALL 0x0000:6147
0000:58ba  bb 00 0a           MOV BX,0xa00
0000:58bd  b4 48              MOV AH,0x48
0000:58bf  cd 21              INT 0x21
0000:58c1  73 03              JNC 0x0000:58c6
0000:58c3  e9 39 01           JMP 0x0000:59ff
0000:58c6  89 46 08           MOV word ptr [BP + 0x8],AX
0000:58c9  bb 00 02           MOV BX,0x200
0000:58cc  b4 48              MOV AH,0x48
0000:58ce  cd 21              INT 0x21
0000:58d0  73 03              JNC 0x0000:58d5
0000:58d2  e9 2a 01           JMP 0x0000:59ff
0000:58d5  89 46 0a           MOV word ptr [BP + 0xa],AX
0000:58d8  bb 00 06           MOV BX,0x600
0000:58db  b4 48              MOV AH,0x48
0000:58dd  cd 21              INT 0x21
0000:58df  73 03              JNC 0x0000:58e4
0000:58e1  e9 1b 01           JMP 0x0000:59ff
0000:58e4  89 46 12           MOV word ptr [BP + 0x12],AX
0000:58e7  c7 46 60 02 00     MOV word ptr [BP + 0x60],0x2
0000:58ec  c7 46 62 02 00     MOV word ptr [BP + 0x62],0x2
0000:58f1  e8 2c 06           CALL 0x0000:5f20
0000:58f4  06                 PUSH ES
0000:58f5  8e 46 0a           MOV ES,word ptr [BP + 0xa]
0000:58f8  bf 00 18           MOV DI,0x1800
0000:58fb  33 c0              XOR AX,AX
0000:58fd  b9 00 04           MOV CX,0x400
0000:5900  f3 ab              STOSW.REP ES:DI
0000:5902  07                 POP ES
0000:5903  b0 c1              MOV AL,0xc1
0000:5905  b4 12              MOV AH,0x12
0000:5907  bb 00 01           MOV BX,0x100
0000:590a  be 00 00           MOV SI,0x0
0000:590d  bf 00 00           MOV DI,0x0
0000:5910  c7 46 0c 26 5d     MOV word ptr [BP + 0xc],0x5d26
0000:5915  1e                 PUSH DS
0000:5916  ff 5e 0c           CALLF [BP + 0xc]
0000:5919  1f                 POP DS
0000:591a  72 0f              JC 0x0000:592b
0000:591c  e8 e0 01           CALL 0x0000:5aff
0000:591f  f6 86 b9 00 80     TEST byte ptr [BP + 0xb9],0x80
0000:5924  74 05              JZ 0x0000:592b
0000:5926  e8 2f 06           CALL 0x0000:5f58
0000:5929  eb c9              JMP 0x0000:58f4
0000:592b  8c c8              MOV AX,CS
0000:592d  8e d8              MOV AX,DS
0000:592f  bd 4c 03           MOV BP,0x34c
0000:5932  f6 86 b9 00 03     TEST byte ptr [BP + 0xb9],0x3
0000:5937  74 1b              JZ 0x0000:5954
0000:5939  b8 40 04           MOV AX,0x440
0000:593c  cd 7f              INT 0x7f
0000:593e  c7 46 0c f5 5e     MOV word ptr [BP + 0xc],0x5ef5
0000:5943  b9 15 00           MOV CX,0x15
0000:5946  0b c9              OR CX,CX
0000:5948  74 06              JZ 0x0000:5950
0000:594a  ff 5e 0c           CALLF [BP + 0xc]
0000:594d  49                 DEC CX
0000:594e  eb f6              JMP 0x0000:5946
0000:5950  b4 03              MOV AH,0x3
0000:5952  cd 7f              INT 0x7f
0000:5954  e8 1e 06           CALL 0x0000:5f75
0000:5957  8e 46 12           MOV ES,word ptr [BP + 0x12]
0000:595a  b4 49              MOV AH,0x49
0000:595c  cd 21              INT 0x21
0000:595e  72 7d              JC 0x0000:59dd
0000:5960  8e 46 0a           MOV ES,word ptr [BP + 0xa]
0000:5963  b4 49              MOV AH,0x49
0000:5965  cd 21              INT 0x21
0000:5967  72 74              JC 0x0000:59dd
0000:5969  8e 46 08           MOV ES,word ptr [BP + 0x8]
0000:596c  b4 49              MOV AH,0x49
0000:596e  cd 21              INT 0x21
0000:5970  72 6b              JC 0x0000:59dd
0000:5972  e8 2d 07           CALL 0x0000:60a2
0000:5975  f6 86 b9 00 01     TEST byte ptr [BP + 0xb9],0x1
0000:597a  74 14              JZ 0x0000:5990
0000:597c  ba 31 57           MOV DX,0x5731
0000:597f  be 4f 57           MOV SI,0x574f
0000:5982  e8 9b 01           CALL 0x0000:5b20
0000:5985  73 05              JNC 0x0000:598c
0000:5987  e8 9b 00           CALL 0x0000:5a25
0000:598a  eb 04              JMP 0x0000:5990
0000:598c  b4 4d              MOV AH,0x4d
0000:598e  cd 21              INT 0x21
0000:5990  f6 86 b9 00 08     TEST byte ptr [BP + 0xb9],0x8
0000:5995  74 14              JZ 0x0000:59ab
0000:5997  ba 26 57           MOV DX,0x5726
0000:599a  be 4e 57           MOV SI,0x574e
0000:599d  e8 80 01           CALL 0x0000:5b20
0000:59a0  73 05              JNC 0x0000:59a7
0000:59a2  e8 80 00           CALL 0x0000:5a25
0000:59a5  eb 04              JMP 0x0000:59ab
0000:59a7  b4 4d              MOV AH,0x4d
0000:59a9  cd 21              INT 0x21
0000:59ab  e8 99 07           CALL 0x0000:6147
0000:59ae  b8 00 4c           MOV AX,0x4c00
0000:59b1  cd 21              INT 0x21
0000:59b3  ba f5 57           MOV DX,0x57f5
0000:59b6  b4 09              MOV AH,0x9
0000:59b8  cd 21              INT 0x21
0000:59ba  e8 24 02           CALL 0x0000:5be1
0000:59bd  ba 00 58           MOV DX,0x5800
0000:59c0  b4 09              MOV AH,0x9
0000:59c2  cd 21              INT 0x21
0000:59c4  e8 59 02           CALL 0x0000:5c20
0000:59c7  ba 04 58           MOV DX,0x5804
0000:59ca  b4 09              MOV AH,0x9
0000:59cc  cd 21              INT 0x21
0000:59ce  eb 1a              JMP 0x0000:59ea
0000:59d0  8c ca              MOV DX,CS
0000:59d2  8e da              MOV DX,DS
0000:59d4  ba 9a 57           MOV DX,0x579a
0000:59d7  b4 09              MOV AH,0x9
0000:59d9  cd 21              INT 0x21
0000:59db  eb 0d              JMP 0x0000:59ea
0000:59dd  8c ca              MOV DX,CS
0000:59df  8e da              MOV DX,DS
0000:59e1  ba 89 57           MOV DX,0x5789
0000:59e4  b4 09              MOV AH,0x9
0000:59e6  cd 21              INT 0x21
0000:59e8  eb 00              JMP 0x0000:59ea
0000:59ea  ba 18 58           MOV DX,0x5818
0000:59ed  b4 09              MOV AH,0x9
0000:59ef  cd 21              INT 0x21
0000:59f1  8b 46 3e           MOV AX,word ptr [BP + 0x3e]
0000:59f4  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:59f7  ff 5e 0c           CALLF [BP + 0xc]
0000:59fa  b8 01 4c           MOV AX,0x4c01
0000:59fd  cd 21              INT 0x21
0000:59ff  8c ca              MOV DX,CS
0000:5a01  8e da              MOV DX,DS
0000:5a03  ba 77 57           MOV DX,0x5777
0000:5a06  83 f8 08           CMP AX,0x8
0000:5a09  74 03              JZ 0x0000:5a0e
0000:5a0b  ba 89 57           MOV DX,0x5789
0000:5a0e  b4 09              MOV AH,0x9
0000:5a10  cd 21              INT 0x21
0000:5a12  ba 18 58           MOV DX,0x5818
0000:5a15  b4 09              MOV AH,0x9
0000:5a17  cd 21              INT 0x21
0000:5a19  8b 46 3e           MOV AX,word ptr [BP + 0x3e]
0000:5a1c  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:5a1f  ff 5e 0c           CALLF [BP + 0xc]
0000:5a22  e9 4d ff           JMP 0x0000:5972
0000:5a25  8b f2              MOV SI,DX
0000:5a27  1e                 PUSH DS
0000:5a28  8c c8              MOV AX,CS
0000:5a2a  8e d8              MOV AX,DS
0000:5a2c  ba 59 57           MOV DX,0x5759
0000:5a2f  b4 09              MOV AH,0x9
0000:5a31  cd 21              INT 0x21
0000:5a33  1f                 POP DS
0000:5a34  b4 02              MOV AH,0x2
0000:5a36  b2 28              MOV DL,0x28
0000:5a38  cd 21              INT 0x21
0000:5a3a  ac                 LODSB SI
0000:5a3b  0a c0              OR AL,AL
0000:5a3d  74 06              JZ 0x0000:5a45
0000:5a3f  8a d0              MOV DL,AL
0000:5a41  cd 21              INT 0x21
0000:5a43  eb f5              JMP 0x0000:5a3a
0000:5a45  b2 29              MOV DL,0x29
0000:5a47  cd 21              INT 0x21
0000:5a49  b2 0d              MOV DL,0xd
0000:5a4b  cd 21              INT 0x21
0000:5a4d  b2 0a              MOV DL,0xa
0000:5a4f  cd 21              INT 0x21
0000:5a51  ba 18 58           MOV DX,0x5818
0000:5a54  b4 09              MOV AH,0x9
0000:5a56  cd 21              INT 0x21
0000:5a58  8b 46 3e           MOV AX,word ptr [BP + 0x3e]
0000:5a5b  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:5a5e  ff 5e 0c           CALLF [BP + 0xc]
0000:5a61  c3                 RET
0000:5a62  8c c8              MOV AX,CS
0000:5a64  89 46 06           MOV word ptr [BP + 0x6],AX
0000:5a67  89 46 0e           MOV word ptr [BP + 0xe],AX
0000:5a6a  89 46 14           MOV word ptr [BP + 0x14],AX
0000:5a6d  c7 46 28 f0 64     MOV word ptr [BP + 0x28],0x64f0
0000:5a72  c7 46 2a c4 65     MOV word ptr [BP + 0x2a],0x65c4
0000:5a77  c7 46 2c 92 66     MOV word ptr [BP + 0x2c],0x6692
0000:5a7c  c7 46 2e a0 6a     MOV word ptr [BP + 0x2e],0x6aa0
0000:5a81  c7 46 30 a4 6a     MOV word ptr [BP + 0x30],0x6aa4
0000:5a86  c7 46 32 26 5d     MOV word ptr [BP + 0x32],0x5d26
0000:5a8b  c7 46 34 0b 6c     MOV word ptr [BP + 0x34],0x6c0b
0000:5a90  c7 46 36 d1 6b     MOV word ptr [BP + 0x36],0x6bd1
0000:5a95  c7 46 38 00 6d     MOV word ptr [BP + 0x38],0x6d00
0000:5a9a  c7 46 3a ab 6c     MOV word ptr [BP + 0x3a],0x6cab
0000:5a9f  c7 46 3c f5 5e     MOV word ptr [BP + 0x3c],0x5ef5
0000:5aa4  c7 46 3e b9 5c     MOV word ptr [BP + 0x3e],0x5cb9
0000:5aa9  c7 46 40 46 78     MOV word ptr [BP + 0x40],0x7846
0000:5aae  c7 46 42 28 74     MOV word ptr [BP + 0x42],0x7428
0000:5ab3  c7 46 44 d4 77     MOV word ptr [BP + 0x44],0x77d4
0000:5ab8  c7 46 46 cd 6d     MOV word ptr [BP + 0x46],0x6dcd
0000:5abd  c7 46 48 40 6d     MOV word ptr [BP + 0x48],0x6d40
0000:5ac2  c7 46 4a 89 6d     MOV word ptr [BP + 0x4a],0x6d89
0000:5ac7  c7 46 4c 34 67     MOV word ptr [BP + 0x4c],0x6734
0000:5acc  c7 46 4e 98 67     MOV word ptr [BP + 0x4e],0x6798
0000:5ad1  c7 46 50 30 04     MOV word ptr [BP + 0x50],0x430
0000:5ad6  c7 46 52 3a 04     MOV word ptr [BP + 0x52],0x43a
0000:5adb  c7 46 54 3a 56     MOV word ptr [BP + 0x54],0x563a
0000:5ae0  c7 46 56 4a 56     MOV word ptr [BP + 0x56],0x564a
0000:5ae5  33 c0              XOR AX,AX
0000:5ae7  88 86 b6 00        MOV byte ptr [BP + 0xb6],AL
0000:5aeb  88 86 ba 00        MOV byte ptr [BP + 0xba],AL
0000:5aef  88 86 b7 00        MOV byte ptr [BP + 0xb7],AL
0000:5af3  c6 86 bb 00 ff     MOV byte ptr [BP + 0xbb],0xff
0000:5af8  c7 06 32 04 45 57  MOV word ptr [0x432],0x5745
0000:5afe  c3                 RET
0000:5aff  60                 PUSHA
0000:5b00  1e                 PUSH DS
0000:5b01  06                 PUSH ES
0000:5b02  c7 46 10 00 01     MOV word ptr [BP + 0x10],0x100
0000:5b07  8e 46 12           MOV ES,word ptr [BP + 0x12]
0000:5b0a  bf 80 00           MOV DI,0x80
0000:5b0d  b0 02              MOV AL,0x2
0000:5b0f  aa                 STOSB ES:DI
0000:5b10  bf 81 00           MOV DI,0x81
0000:5b13  8b c5              MOV AX,BP
0000:5b15  ab                 STOSW ES:DI
0000:5b16  b0 0d              MOV AL,0xd
0000:5b18  aa                 STOSB ES:DI
0000:5b19  ff 5e 10           CALLF [BP + 0x10]
0000:5b1c  07                 POP ES
0000:5b1d  1f                 POP DS
0000:5b1e  61                 POPA
0000:5b1f  c3                 RET
0000:5b20  60                 PUSHA
0000:5b21  1e                 PUSH DS
0000:5b22  06                 PUSH ES
0000:5b23  fc                 CLD
0000:5b24  8c c8              MOV AX,CS
0000:5b26  8e c0              MOV AX,ES
0000:5b28  c6 06 61 5b 00     MOV byte ptr [0x5b61],0x0
0000:5b2d  bf 62 5b           MOV DI,0x5b62
0000:5b30  ac                 LODSB SI
0000:5b31  aa                 STOSB ES:DI
0000:5b32  3c 0d              CMP AL,0xd
0000:5b34  74 06              JZ 0x0000:5b3c
0000:5b36  fe 06 61 5b        INC byte ptr [0x5b61]
0000:5b3a  eb f4              JMP 0x0000:5b30
0000:5b3c  8c c8              MOV AX,CS
0000:5b3e  a3 57 5b           MOV [0x5b57],AX
0000:5b41  a3 5b 5b           MOV [0x5b5b],AX
0000:5b44  a3 5f 5b           MOV [0x5b5f],AX
0000:5b47  bb 53 5b           MOV BX,0x5b53
0000:5b4a  b8 00 4b           MOV AX,0x4b00
0000:5b4d  cd 21              INT 0x21
0000:5b4f  07                 POP ES
0000:5b50  1f                 POP DS
0000:5b51  61                 POPA
0000:5b52  c3                 RET
0000:5b53  00 00              ADD byte ptr [BX + SI],AL
0000:5b55  61                 POPA
0000:5b56  5b                 POP BX
0000:5b57  00 00              ADD byte ptr [BX + SI],AL
0000:5b59  5c                 POP SP
0000:5b5a  00 00              ADD byte ptr [BX + SI],AL
0000:5b5c  00 6c 00           ADD byte ptr [SI + 0x0],CH
0000:5b5f  00 00              ADD byte ptr [BX + SI],AL
0000:5b61  00 0d              ADD byte ptr [DI],CL
0000:5b63  00 00              ADD byte ptr [BX + SI],AL
0000:5b65  00 00              ADD byte ptr [BX + SI],AL
0000:5b67  00 00              ADD byte ptr [BX + SI],AL
0000:5b69  00 00              ADD byte ptr [BX + SI],AL
0000:5b6b  00 00              ADD byte ptr [BX + SI],AL
0000:5b6d  00 00              ADD byte ptr [BX + SI],AL
0000:5b6f  00 00              ADD byte ptr [BX + SI],AL
0000:5b71  00 00              ADD byte ptr [BX + SI],AL
0000:5b73  00 00              ADD byte ptr [BX + SI],AL
0000:5b75  00 00              ADD byte ptr [BX + SI],AL
0000:5b77  00 00              ADD byte ptr [BX + SI],AL
0000:5b79  00 00              ADD byte ptr [BX + SI],AL
0000:5b7b  00 00              ADD byte ptr [BX + SI],AL
0000:5b7d  00 00              ADD byte ptr [BX + SI],AL
0000:5b7f  00 00              ADD byte ptr [BX + SI],AL
0000:5b81  00 00              ADD byte ptr [BX + SI],AL
0000:5b83  00 00              ADD byte ptr [BX + SI],AL
0000:5be1  b9 ff 69           MOV CX,0x69ff
0000:5be4  2b 4e 6a           SUB CX,word ptr [BP + 0x6a]
0000:5be7  c1 e9 06           SHR CX,0x6
0000:5bea  b8 00 02           MOV AX,0x200
0000:5bed  b2 30              MOV DL,0x30
0000:5bef  83 f9 64           CMP CX,0x64
0000:5bf2  72 09              JC 0x0000:5bfd
0000:5bf4  83 e9 64           SUB CX,0x64
0000:5bf7  fe c2              INC DL
0000:5bf9  fe c0              INC AL
0000:5bfb  eb f2              JMP 0x0000:5bef
0000:5bfd  0a c0              OR AL,AL
0000:5bff  74 02              JZ 0x0000:5c03
0000:5c01  cd 21              INT 0x21
0000:5c03  b2 30              MOV DL,0x30
0000:5c05  83 f9 0a           CMP CX,0xa
0000:5c08  72 09              JC 0x0000:5c13
0000:5c0a  83 e9 0a           SUB CX,0xa
0000:5c0d  fe c2              INC DL
0000:5c0f  fe c0              INC AL
0000:5c11  eb f2              JMP 0x0000:5c05
0000:5c13  0a c0              OR AL,AL
0000:5c15  74 02              JZ 0x0000:5c19
0000:5c17  cd 21              INT 0x21
0000:5c19  b2 30              MOV DL,0x30
0000:5c1b  02 d1              ADD DL,CL
0000:5c1d  cd 21              INT 0x21
0000:5c1f  c3                 RET
0000:5c20  b9 a7 01           MOV CX,0x1a7
0000:5c23  c1 e1 06           SHL CX,0x6
0000:5c26  83 c1 3f           ADD CX,0x3f
0000:5c29  83 e1 c0           AND CX,0xffc0
0000:5c2c  8b 46 6a           MOV AX,word ptr [BP + 0x6a]
0000:5c2f  83 e0 c0           AND AX,0xffc0
0000:5c32  2b c8              SUB CX,AX
0000:5c34  b8 00 02           MOV AX,0x200
0000:5c37  b2 30              MOV DL,0x30
0000:5c39  81 f9 6a 18        CMP CX,0x186a
0000:5c3d  72 0a              JC 0x0000:5c49
0000:5c3f  81 e9 6a 18        SUB CX,0x186a
0000:5c43  fe c2              INC DL
0000:5c45  fe c0              INC AL
0000:5c47  eb f0              JMP 0x0000:5c39
0000:5c49  0a c0              OR AL,AL
0000:5c4b  74 02              JZ 0x0000:5c4f
0000:5c4d  cd 21              INT 0x21
0000:5c4f  c1 e1 04           SHL CX,0x4
0000:5c52  b2 30              MOV DL,0x30
0000:5c54  81 f9 10 27        CMP CX,0x2710
0000:5c58  72 0a              JC 0x0000:5c64
0000:5c5a  81 e9 10 27        SUB CX,0x2710
0000:5c5e  fe c2              INC DL
0000:5c60  fe c0              INC AL
0000:5c62  eb f0              JMP 0x0000:5c54
0000:5c64  0a c0              OR AL,AL
0000:5c66  74 02              JZ 0x0000:5c6a
0000:5c68  cd 21              INT 0x21
0000:5c6a  b2 30              MOV DL,0x30
0000:5c6c  81 f9 e8 03        CMP CX,0x3e8
0000:5c70  72 0a              JC 0x0000:5c7c
0000:5c72  81 e9 e8 03        SUB CX,0x3e8
0000:5c76  fe c2              INC DL
0000:5c78  fe c0              INC AL
0000:5c7a  eb f0              JMP 0x0000:5c6c
0000:5c7c  0a c0              OR AL,AL
0000:5c7e  74 06              JZ 0x0000:5c86
0000:5c80  cd 21              INT 0x21
0000:5c82  b2 2c              MOV DL,0x2c
0000:5c84  cd 21              INT 0x21
0000:5c86  b2 30              MOV DL,0x30
0000:5c88  83 f9 64           CMP CX,0x64
0000:5c8b  72 09              JC 0x0000:5c96
0000:5c8d  83 e9 64           SUB CX,0x64
0000:5c90  fe c2              INC DL
0000:5c92  fe c0              INC AL
0000:5c94  eb f2              JMP 0x0000:5c88
0000:5c96  0a c0              OR AL,AL
0000:5c98  74 02              JZ 0x0000:5c9c
0000:5c9a  cd 21              INT 0x21
0000:5c9c  b2 30              MOV DL,0x30
0000:5c9e  83 f9 0a           CMP CX,0xa
0000:5ca1  72 09              JC 0x0000:5cac
0000:5ca3  83 e9 0a           SUB CX,0xa
0000:5ca6  fe c2              INC DL
0000:5ca8  fe c0              INC AL
0000:5caa  eb f2              JMP 0x0000:5c9e
0000:5cac  0a c0              OR AL,AL
0000:5cae  74 02              JZ 0x0000:5cb2
0000:5cb0  cd 21              INT 0x21
0000:5cb2  b2 30              MOV DL,0x30
0000:5cb4  02 d1              ADD DL,CL
0000:5cb6  cd 21              INT 0x21
0000:5cb8  c3                 RET
0000:5cb9  06                 PUSH ES
0000:5cba  33 c0              XOR AX,AX
0000:5cbc  8e c0              MOV AX,ES
0000:5cbe  33 c0              XOR AX,AX
0000:5cc0  26 0a 06 2a 05     OR AL,byte ptr ES:[0x52a]
0000:5cc5  26 0a 06 2d 05     OR AL,byte ptr ES:[0x52d]
0000:5cca  26 0a 06 2f 05     OR AL,byte ptr ES:[0x52f]
0000:5ccf  26 0a 06 30 05     OR AL,byte ptr ES:[0x530]
0000:5cd4  26 0a 06 31 05     OR AL,byte ptr ES:[0x531]
0000:5cd9  26 0a 06 32 05     OR AL,byte ptr ES:[0x532]
0000:5cde  26 0a 06 33 05     OR AL,byte ptr ES:[0x533]
0000:5ce3  26 0a 06 37 05     OR AL,byte ptr ES:[0x537]
0000:5ce8  3c 00              CMP AL,0x0
0000:5cea  75 d2              JNZ 0x0000:5cbe
0000:5cec  33 c0              XOR AX,AX
0000:5cee  26 0a 06 2a 05     OR AL,byte ptr ES:[0x52a]
0000:5cf3  26 0a 06 2d 05     OR AL,byte ptr ES:[0x52d]
0000:5cf8  26 0a 06 2f 05     OR AL,byte ptr ES:[0x52f]
0000:5cfd  26 0a 06 30 05     OR AL,byte ptr ES:[0x530]
0000:5d02  26 0a 06 31 05     OR AL,byte ptr ES:[0x531]
0000:5d07  26 0a 06 32 05     OR AL,byte ptr ES:[0x532]
0000:5d0c  26 0a 06 33 05     OR AL,byte ptr ES:[0x533]
0000:5d11  26 0a 06 37 05     OR AL,byte ptr ES:[0x537]
0000:5d16  3c 00              CMP AL,0x0
0000:5d18  74 d2              JZ 0x0000:5cec
0000:5d1a  26 f6 06 37 05 08  TEST byte ptr ES:[0x537],0x8
0000:5d20  07                 POP ES
0000:5d21  f8                 CLC
0000:5d22  74 01              JZ 0x0000:5d25
0000:5d24  f9                 STC
0000:5d25  cb                 RETF
0000:5d26  55                 PUSH BP
0000:5d27  1e                 PUSH DS
0000:5d28  06                 PUSH ES
0000:5d29  2e 8c 1e e8 5e     MOV word ptr CS:[0x5ee8],DS
0000:5d2e  2e a2 f0 5e        MOV CS:[0x5ef0],AL
0000:5d32  2e 88 26 f1 5e     MOV byte ptr CS:[0x5ef1],AH
0000:5d37  8c c8              MOV AX,CS
0000:5d39  8e d8              MOV AX,DS
0000:5d3b  8c 56 0e           MOV word ptr [BP + 0xe],SS
0000:5d3e  2e 89 36 ea 5e     MOV word ptr CS:[0x5eea],SI
0000:5d43  2e 89 3e ec 5e     MOV word ptr CS:[0x5eec],DI
0000:5d48  2e 88 1e f2 5e     MOV byte ptr CS:[0x5ef2],BL
0000:5d4d  2e 88 3e f3 5e     MOV byte ptr CS:[0x5ef3],BH
0000:5d52  2e a0 f2 5e        MOV AL,CS:[0x5ef2]
0000:5d56  2e 8a 26 f3 5e     MOV AH,byte ptr CS:[0x5ef3]
0000:5d5b  33 db              XOR BX,BX
0000:5d5d  2e 8a 1e f1 5e     MOV BL,byte ptr CS:[0x5ef1]
0000:5d62  81 c3 00 18        ADD BX,0x1800
0000:5d66  8e 46 0a           MOV ES,word ptr [BP + 0xa]
0000:5d69  2e f6 06 f0 5e 42  TEST byte ptr CS:[0x5ef0],0x42
0000:5d6f  75 08              JNZ 0x0000:5d79
0000:5d71  2e f6 06 f0 5e 01  TEST byte ptr CS:[0x5ef0],0x1
0000:5d77  75 07              JNZ 0x0000:5d80
0000:5d79  26 c7 07 00 00     MOV word ptr ES:[BX],0x0
0000:5d7e  eb 0b              JMP 0x0000:5d8b
0000:5d80  26 3b 07           CMP AX,word ptr ES:[BX]
0000:5d83  75 03              JNZ 0x0000:5d88
0000:5d85  e9 55 01           JMP 0x0000:5edd
0000:5d88  26 89 07           MOV word ptr ES:[BX],AX
0000:5d8b  33 c0              XOR AX,AX
0000:5d8d  2e a0 f3 5e        MOV AL,CS:[0x5ef3]
0000:5d91  d1 e0              SHL AX,0x1
0000:5d93  8b 5e 50           MOV BX,word ptr [BP + 0x50]
0000:5d96  03 d8              ADD BX,AX
0000:5d98  36 8b 37           MOV SI,word ptr SS:[BX]
0000:5d9b  8b 7e 4c           MOV DI,word ptr [BP + 0x4c]
0000:5d9e  8c c8              MOV AX,CS
0000:5da0  8e c0              MOV AX,ES
0000:5da2  2e a1 e8 5e        MOV AX,CS:[0x5ee8]
0000:5da6  1e                 PUSH DS
0000:5da7  8e d8              MOV AX,DS
0000:5da9  b9 28 00           MOV CX,0x28
0000:5dac  f3 a5              MOVSW.REP ES:DI,SI
0000:5dae  1f                 POP DS
0000:5daf  33 db              XOR BX,BX
0000:5db1  33 d2              XOR DX,DX
0000:5db3  b9 00 a0           MOV CX,0xa000
0000:5db6  2e f6 06 f0 5e 01  TEST byte ptr CS:[0x5ef0],0x1
0000:5dbc  74 72              JZ 0x0000:5e30
0000:5dbe  2e f6 06 f0 5e 02  TEST byte ptr CS:[0x5ef0],0x2
0000:5dc4  74 08              JZ 0x0000:5dce
0000:5dc6  2e c6 06 f4 5e 00  MOV byte ptr CS:[0x5ef4],0x0
0000:5dcc  eb 0f              JMP 0x0000:5ddd
0000:5dce  2e a0 f3 5e        MOV AL,CS:[0x5ef3]
0000:5dd2  2e 3a 06 f4 5e     CMP AL,byte ptr CS:[0x5ef4]
0000:5dd7  74 25              JZ 0x0000:5dfe
0000:5dd9  2e a2 f4 5e        MOV CS:[0x5ef4],AL
0000:5ddd  8b 76 4c           MOV SI,word ptr [BP + 0x4c]
0000:5de0  b9 00 18           MOV CX,0x1800
0000:5de3  33 ff              XOR DI,DI
0000:5de5  33 db              XOR BX,BX
0000:5de7  33 d2              XOR DX,DX
0000:5de9  8e 46 0a           MOV ES,word ptr [BP + 0xa]
0000:5dec  8b 46 28           MOV AX,word ptr [BP + 0x28]
0000:5def  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:5df2  33 c0              XOR AX,AX
0000:5df4  1e                 PUSH DS
0000:5df5  ff 5e 0c           CALLF [BP + 0xc]
0000:5df8  1f                 POP DS
0000:5df9  73 03              JNC 0x0000:5dfe
0000:5dfb  e9 e4 00           JMP 0x0000:5ee2
0000:5dfe  33 db              XOR BX,BX
0000:5e00  2e 8a 1e f2 5e     MOV BL,byte ptr CS:[0x5ef2]
0000:5e05  c1 e3 02           SHL BX,0x2
0000:5e08  8e 46 0a           MOV ES,word ptr [BP + 0xa]
0000:5e0b  26 8b 57 06        MOV DX,word ptr ES:[BX + 0x6]
0000:5e0f  26 8b 4f 04        MOV CX,word ptr ES:[BX + 0x4]
0000:5e13  26 2b 57 02        SUB DX,word ptr ES:[BX + 0x2]
0000:5e17  26 2b 0f           SUB CX,word ptr ES:[BX]
0000:5e1a  0b c9              OR CX,CX
0000:5e1c  74 09              JZ 0x0000:5e27
0000:5e1e  b8 ff ff           MOV AX,0xffff
0000:5e21  2b d0              SUB DX,AX
0000:5e23  42                 INC DX
0000:5e24  49                 DEC CX
0000:5e25  eb f3              JMP 0x0000:5e1a
0000:5e27  8b ca              MOV CX,DX
0000:5e29  26 8b 57 02        MOV DX,word ptr ES:[BX + 0x2]
0000:5e2d  26 8b 1f           MOV BX,word ptr ES:[BX]
0000:5e30  8b 76 4c           MOV SI,word ptr [BP + 0x4c]
0000:5e33  33 ff              XOR DI,DI
0000:5e35  53                 PUSH BX
0000:5e36  51                 PUSH CX
0000:5e37  52                 PUSH DX
0000:5e38  8e 46 08           MOV ES,word ptr [BP + 0x8]
0000:5e3b  8b 46 28           MOV AX,word ptr [BP + 0x28]
0000:5e3e  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:5e41  33 c0              XOR AX,AX
0000:5e43  2e 8a 26 f0 5e     MOV AH,byte ptr CS:[0x5ef0]
0000:5e48  80 e4 08           AND AH,0x8
0000:5e4b  1e                 PUSH DS
0000:5e4c  ff 5e 0c           CALLF [BP + 0xc]
0000:5e4f  2e a3 ee 5e        MOV CS:[0x5eee],AX
0000:5e53  1f                 POP DS
0000:5e54  5a                 POP DX
0000:5e55  59                 POP CX
0000:5e56  5b                 POP BX
0000:5e57  73 03              JNC 0x0000:5e5c
0000:5e59  e9 86 00           JMP 0x0000:5ee2
0000:5e5c  2e f6 06 f0 5e 04  TEST byte ptr CS:[0x5ef0],0x4
0000:5e62  74 3a              JZ 0x0000:5e9e
0000:5e64  33 db              XOR BX,BX
0000:5e66  2e 8a 1e f1 5e     MOV BL,byte ptr CS:[0x5ef1]
0000:5e6b  03 dd              ADD BX,BP
0000:5e6d  36 8b 07           MOV AX,word ptr SS:[BX]
0000:5e70  2e f6 06 f0 5e 80  TEST byte ptr CS:[0x5ef0],0x80
0000:5e76  74 03              JZ 0x0000:5e7b
0000:5e78  83 c0 10           ADD AX,0x10
0000:5e7b  8e c0              MOV AX,ES
0000:5e7d  8e 5e 08           MOV DS,word ptr [BP + 0x8]
0000:5e80  2e 8b 36 ea 5e     MOV SI,word ptr CS:[0x5eea]
0000:5e85  2e 8b 3e ec 5e     MOV DI,word ptr CS:[0x5eec]
0000:5e8a  2e 8b 0e ee 5e     MOV CX,word ptr CS:[0x5eee]
0000:5e8f  3b f1              CMP SI,CX
0000:5e91  73 4a              JNC 0x0000:5edd
0000:5e93  2b ce              SUB CX,SI
0000:5e95  d1 e9              SHR CX,0x1
0000:5e97  73 01              JNC 0x0000:5e9a
0000:5e99  a4                 MOVSB ES:DI,SI
0000:5e9a  f3 a5              MOVSW.REP ES:DI,SI
0000:5e9c  eb 3f              JMP 0x0000:5edd
0000:5e9e  2e f6 06 f0 5e 02  TEST byte ptr CS:[0x5ef0],0x2
0000:5ea4  74 08              JZ 0x0000:5eae
0000:5ea6  8b 46 30           MOV AX,word ptr [BP + 0x30]
0000:5ea9  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:5eac  eb 06              JMP 0x0000:5eb4
0000:5eae  8b 46 2e           MOV AX,word ptr [BP + 0x2e]
0000:5eb1  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:5eb4  2e 8b 36 ea 5e     MOV SI,word ptr CS:[0x5eea]
0000:5eb9  2e 8b 3e ec 5e     MOV DI,word ptr CS:[0x5eec]
0000:5ebe  33 db              XOR BX,BX
0000:5ec0  2e 8a 1e f1 5e     MOV BL,byte ptr CS:[0x5ef1]
0000:5ec5  03 dd              ADD BX,BP
0000:5ec7  36 8b 07           MOV AX,word ptr SS:[BX]
0000:5eca  2e f6 06 f0 5e 80  TEST byte ptr CS:[0x5ef0],0x80
0000:5ed0  74 03              JZ 0x0000:5ed5
0000:5ed2  83 c0 10           ADD AX,0x10
0000:5ed5  8e c0              MOV AX,ES
0000:5ed7  8e 5e 08           MOV DS,word ptr [BP + 0x8]
0000:5eda  ff 5e 0c           CALLF [BP + 0xc]
0000:5edd  07                 POP ES
0000:5ede  1f                 POP DS
0000:5edf  5d                 POP BP
0000:5ee0  f8                 CLC
0000:5ee1  cb                 RETF
0000:5ee2  07                 POP ES
0000:5ee3  1f                 POP DS
0000:5ee4  5d                 POP BP
0000:5ee5  f9                 STC
0000:5ee6  cb                 RETF
0000:5ef5  e6 64              OUT 0x64,AL
0000:5ef7  8b 46 5e           MOV AX,word ptr [BP + 0x5e]
0000:5efa  89 46 66           MOV word ptr [BP + 0x66],AX
0000:5efd  8b 46 60           MOV AX,word ptr [BP + 0x60]
0000:5f00  03 46 5e           ADD AX,word ptr [BP + 0x5e]
0000:5f03  3b 46 5c           CMP AX,word ptr [BP + 0x5c]
0000:5f06  77 f5              JA 0x0000:5efd
0000:5f08  8b 46 5c           MOV AX,word ptr [BP + 0x5c]
0000:5f0b  89 46 5e           MOV word ptr [BP + 0x5e],AX
0000:5f0e  8b 46 5c           MOV AX,word ptr [BP + 0x5c]
0000:5f11  2b 46 66           SUB AX,word ptr [BP + 0x66]
0000:5f14  89 46 66           MOV word ptr [BP + 0x66],AX
0000:5f17  cb                 RETF
0000:5f20  55                 PUSH BP
0000:5f21  1e                 PUSH DS
0000:5f22  e8 98 00           CALL 0x0000:5fbd
0000:5f25  e8 c6 00           CALL 0x0000:5fee
0000:5f28  e8 ea 02           CALL 0x0000:6215
0000:5f2b  e8 a0 04           CALL 0x0000:63ce
0000:5f2e  e8 ca 03           CALL 0x0000:62fb
0000:5f31  e8 74 05           CALL 0x0000:64a8
0000:5f34  e8 94 03           CALL 0x0000:62cb
0000:5f37  e8 7c 03           CALL 0x0000:62b6
0000:5f3a  e8 2f 03           CALL 0x0000:626c
0000:5f3d  e8 05 04           CALL 0x0000:6345
0000:5f40  e8 f8 03           CALL 0x0000:633b
0000:5f43  e8 eb 03           CALL 0x0000:6331
0000:5f46  e8 03 04           CALL 0x0000:634c
0000:5f49  e8 28 02           CALL 0x0000:6174
0000:5f4c  e8 7e 02           CALL 0x0000:61cd
0000:5f4f  e8 50 01           CALL 0x0000:60a2
0000:5f52  e8 c5 00           CALL 0x0000:601a
0000:5f55  1f                 POP DS
0000:5f56  5d                 POP BP
0000:5f57  c3                 RET
0000:5f58  55                 PUSH BP
0000:5f59  1e                 PUSH DS
0000:5f5a  e8 9e 03           CALL 0x0000:62fb
0000:5f5d  e8 48 05           CALL 0x0000:64a8
0000:5f60  e8 68 03           CALL 0x0000:62cb
0000:5f63  e8 06 03           CALL 0x0000:626c
0000:5f66  e8 dc 03           CALL 0x0000:6345
0000:5f69  e8 cf 03           CALL 0x0000:633b
0000:5f6c  e8 c2 03           CALL 0x0000:6331
0000:5f6f  e8 da 03           CALL 0x0000:634c
0000:5f72  1f                 POP DS
0000:5f73  5d                 POP BP
0000:5f74  c3                 RET
0000:5f75  55                 PUSH BP
0000:5f76  1e                 PUSH DS
0000:5f77  8c c8              MOV AX,CS
0000:5f79  8e d8              MOV AX,DS
0000:5f7b  e8 d9 00           CALL 0x0000:6057
0000:5f7e  e8 c6 01           CALL 0x0000:6147
0000:5f81  e8 82 02           CALL 0x0000:6206
0000:5f84  e8 2f 02           CALL 0x0000:61b6
0000:5f87  e8 e3 03           CALL 0x0000:636d
0000:5f8a  e8 d3 03           CALL 0x0000:6360
0000:5f8d  e8 26 03           CALL 0x0000:62b6
0000:5f90  e8 99 03           CALL 0x0000:632c
0000:5f93  e8 12 05           CALL 0x0000:64a8
0000:5f96  e8 d3 02           CALL 0x0000:626c
0000:5f99  e8 a9 03           CALL 0x0000:6345
0000:5f9c  e8 60 00           CALL 0x0000:5fff
0000:5f9f  e8 8f 03           CALL 0x0000:6331
0000:5fa2  e8 26 03           CALL 0x0000:62cb
0000:5fa5  e8 3c 03           CALL 0x0000:62e4
0000:5fa8  e8 90 03           CALL 0x0000:633b
0000:5fab  1f                 POP DS
0000:5fac  5d                 POP BP
0000:5fad  c3                 RET
0000:5fbd  2e c6 06 ae 5f 00  MOV byte ptr CS:[0x5fae],0x0
0000:5fc3  bb a3 c0           MOV BX,0xc0a3
0000:5fc6  8c c8              MOV AX,CS
0000:5fc8  8e c0              MOV AX,ES
0000:5fca  bf af 5f           MOV DI,0x5faf
0000:5fcd  b4 0b              MOV AH,0xb
0000:5fcf  cd 18              INT 0x18
0000:5fd1  2e 80 3e bc 5f 31  CMP byte ptr CS:[0x5fbc],0x31
0000:5fd7  75 14              JNZ 0x0000:5fed
0000:5fd9  2e 80 0e ae 5f 80  OR byte ptr CS:[0x5fae],0x80
0000:5fdf  b8 01 ff           MOV AX,0xff01
0000:5fe2  cd 18              INT 0x18
0000:5fe4  0b c0              OR AX,AX
0000:5fe6  74 05              JZ 0x0000:5fed
0000:5fe8  2e fe 06 ae 5f     INC byte ptr CS:[0x5fae]
0000:5fed  c3                 RET
0000:5fee  2e f6 06 ae 5f 80  TEST byte ptr CS:[0x5fae],0x80
0000:5ff4  74 08              JZ 0x0000:5ffe
0000:5ff6  b8 00 0a           MOV AX,0xa00
0000:5ff9  bb a3 c0           MOV BX,0xc0a3
0000:5ffc  cd 18              INT 0x18
0000:5ffe  c3                 RET
0000:5fff  2e a0 ae 5f        MOV AL,CS:[0x5fae]
0000:6003  a8 80              TEST AL,0x80
0000:6005  74 12              JZ 0x0000:6019
0000:6007  24 7f              AND AL,0x7f
0000:6009  74 0e              JZ 0x0000:6019
0000:600b  b8 02 ff           MOV AX,0xff02
0000:600e  cd 18              INT 0x18
0000:6010  0b c0              OR AX,AX
0000:6012  74 05              JZ 0x0000:6019
0000:6014  2e fe 0e ae 5f     DEC byte ptr CS:[0x5fae]
0000:6019  c3                 RET
0000:601a  80 3e a0 60 00     CMP byte ptr [0x60a0],0x0
0000:601f  74 01              JZ 0x0000:6022
0000:6021  c3                 RET
0000:6022  b8 0a 35           MOV AX,0x350a
0000:6025  cd 21              INT 0x21
0000:6027  89 1e 9c 60        MOV word ptr [0x609c],BX
0000:602b  8c 06 9e 60        MOV word ptr [0x609e],ES
0000:602f  c7 06 a8 03 00 00  MOV word ptr [0x3a8],0x0
0000:6035  1e                 PUSH DS
0000:6036  8c c8              MOV AX,CS
0000:6038  8e d8              MOV AX,DS
0000:603a  ba 78 60           MOV DX,0x6078
0000:603d  b8 0a 25           MOV AX,0x250a
0000:6040  fa                 CLI
0000:6041  cd 21              INT 0x21
0000:6043  e4 02              IN AL,0x2
0000:6045  2e a2 a1 60        MOV CS:[0x60a1],AL
0000:6049  24 fb              AND AL,0xfb
0000:604b  e6 02              OUT 0x2,AL
0000:604d  fb                 STI
0000:604e  e6 64              OUT 0x64,AL
0000:6050  1f                 POP DS
0000:6051  c6 06 a0 60 01     MOV byte ptr [0x60a0],0x1
0000:6056  c3                 RET
0000:6057  80 3e a0 60 00     CMP byte ptr [0x60a0],0x0
0000:605c  75 01              JNZ 0x0000:605f
0000:605e  c3                 RET
0000:605f  fa                 CLI
0000:6060  2e a0 a1 60        MOV AL,CS:[0x60a1]
0000:6064  e6 02              OUT 0x2,AL
0000:6066  1e                 PUSH DS
0000:6067  c5 16 9c 60        LDS DX,[0x609c]
0000:606b  b8 0a 25           MOV AX,0x250a
0000:606e  cd 21              INT 0x21
0000:6070  1f                 POP DS
0000:6071  c6 06 a0 60 00     MOV byte ptr [0x60a0],0x0
0000:6076  fb                 STI
0000:6077  c3                 RET
0000:60a2  1e                 PUSH DS
0000:60a3  06                 PUSH ES
0000:60a4  fa                 CLI
0000:60a5  8c c8              MOV AX,CS
0000:60a7  8e d8              MOV AX,DS
0000:60a9  b8 05 35           MOV AX,0x3505
0000:60ac  cd 21              INT 0x21
0000:60ae  2e 8c 06 38 61     MOV word ptr CS:[0x6138],ES
0000:60b3  2e 89 1e 36 61     MOV word ptr CS:[0x6136],BX
0000:60b8  ba 09 61           MOV DX,0x6109
0000:60bb  b8 05 25           MOV AX,0x2505
0000:60be  cd 21              INT 0x21
0000:60c0  b8 06 35           MOV AX,0x3506
0000:60c3  cd 21              INT 0x21
0000:60c5  2e 8c 06 3c 61     MOV word ptr CS:[0x613c],ES
0000:60ca  2e 89 1e 3a 61     MOV word ptr CS:[0x613a],BX
0000:60cf  ba 09 61           MOV DX,0x6109
0000:60d2  b8 06 25           MOV AX,0x2506
0000:60d5  cd 21              INT 0x21
0000:60d7  b8 23 35           MOV AX,0x3523
0000:60da  cd 21              INT 0x21
0000:60dc  2e 8c 06 40 61     MOV word ptr CS:[0x6140],ES
0000:60e1  2e 89 1e 3e 61     MOV word ptr CS:[0x613e],BX
0000:60e6  ba 09 61           MOV DX,0x6109
0000:60e9  b8 23 25           MOV AX,0x2523
0000:60ec  cd 21              INT 0x21
0000:60ee  b8 24 35           MOV AX,0x3524
0000:60f1  cd 21              INT 0x21
0000:60f3  2e 8c 06 44 61     MOV word ptr CS:[0x6144],ES
0000:60f8  2e 89 1e 42 61     MOV word ptr CS:[0x6142],BX
0000:60fd  ba 0a 61           MOV DX,0x610a
0000:6100  b8 24 25           MOV AX,0x2524
0000:6103  cd 21              INT 0x21
0000:6105  fb                 STI
0000:6106  07                 POP ES
0000:6107  1f                 POP DS
0000:6108  c3                 RET
0000:6147  fa                 CLI
0000:6148  1e                 PUSH DS
0000:6149  2e c5 16 42 61     LDS DX,CS:[0x6142]
0000:614e  b8 24 25           MOV AX,0x2524
0000:6151  cd 21              INT 0x21
0000:6153  2e c5 16 3e 61     LDS DX,CS:[0x613e]
0000:6158  b8 23 25           MOV AX,0x2523
0000:615b  cd 21              INT 0x21
0000:615d  2e c5 16 3a 61     LDS DX,CS:[0x613a]
0000:6162  b8 06 25           MOV AX,0x2506
0000:6165  cd 21              INT 0x21
0000:6167  2e c5 16 36 61     LDS DX,CS:[0x6136]
0000:616c  b8 05 25           MOV AX,0x2505
0000:616f  cd 21              INT 0x21
0000:6171  fb                 STI
0000:6172  1f                 POP DS
0000:6173  c3                 RET
0000:6174  2e f6 06 05 04 03  TEST byte ptr CS:[0x405],0x3
0000:617a  75 21              JNZ 0x0000:619d
0000:617c  1e                 PUSH DS
0000:617d  06                 PUSH ES
0000:617e  fa                 CLI
0000:617f  8c c8              MOV AX,CS
0000:6181  8e d8              MOV AX,DS
0000:6183  b8 7f 35           MOV AX,0x357f
0000:6186  cd 21              INT 0x21
0000:6188  2e 8c 06 b4 61     MOV word ptr CS:[0x61b4],ES
0000:618d  2e 89 1e b2 61     MOV word ptr CS:[0x61b2],BX
0000:6192  ba a8 61           MOV DX,0x61a8
0000:6195  b8 7f 25           MOV AX,0x257f
0000:6198  cd 21              INT 0x21
0000:619a  fb                 STI
0000:619b  07                 POP ES
0000:619c  1f                 POP DS
0000:619d  ba a9 61           MOV DX,0x61a9
0000:61a0  b8 00 09           MOV AX,0x900
0000:61a3  fa                 CLI
0000:61a4  cd 7f              INT 0x7f
0000:61a6  fb                 STI
0000:61a7  c3                 RET
0000:61b6  2e f6 06 05 04 03  TEST byte ptr CS:[0x405],0x3
0000:61bc  75 0e              JNZ 0x0000:61cc
0000:61be  fa                 CLI
0000:61bf  1e                 PUSH DS
0000:61c0  2e c5 16 b2 61     LDS DX,CS:[0x61b2]
0000:61c5  b8 7f 25           MOV AX,0x257f
0000:61c8  cd 21              INT 0x21
0000:61ca  fb                 STI
0000:61cb  1f                 POP DS
0000:61cc  c3                 RET
0000:61cd  1e                 PUSH DS
0000:61ce  06                 PUSH ES
0000:61cf  fa                 CLI
0000:61d0  8c c8              MOV AX,CS
0000:61d2  8e d8              MOV AX,DS
0000:61d4  b8 00 35           MOV AX,0x3500
0000:61d7  cd 21              INT 0x21
0000:61d9  2e 8c 06 04 62     MOV word ptr CS:[0x6204],ES
0000:61de  2e 89 1e 02 62     MOV word ptr CS:[0x6202],BX
0000:61e3  ba ef 61           MOV DX,0x61ef
0000:61e6  b8 00 25           MOV AX,0x2500
0000:61e9  cd 21              INT 0x21
0000:61eb  fb                 STI
0000:61ec  07                 POP ES
0000:61ed  1f                 POP DS
0000:61ee  c3                 RET
0000:6206  1e                 PUSH DS
0000:6207  fa                 CLI
0000:6208  2e c5 16 02 62     LDS DX,CS:[0x6202]
0000:620d  b8 00 25           MOV AX,0x2500
0000:6210  cd 21              INT 0x21
0000:6212  fb                 STI
0000:6213  1f                 POP DS
0000:6214  c3                 RET
0000:6215  b4 19              MOV AH,0x19
0000:6217  cd 21              INT 0x21
0000:6219  04 41              ADD AL,0x41
0000:621b  be 2c 62           MOV SI,0x622c
0000:621e  2e 88 04           MOV byte ptr CS:[SI],AL
0000:6221  83 c6 03           ADD SI,0x3
0000:6224  b2 00              MOV DL,0x0
0000:6226  b4 47              MOV AH,0x47
0000:6228  cd 21              INT 0x21
0000:622a  c3                 RET
0000:626c  06                 PUSH ES
0000:626d  33 c0              XOR AX,AX
0000:626f  e6 a4              OUT 0xa4,AL
0000:6271  40                 INC AX
0000:6272  e6 a6              OUT 0xa6,AL
0000:6274  e8 0c 00           CALL 0x0000:6283
0000:6277  33 c0              XOR AX,AX
0000:6279  e6 a6              OUT 0xa6,AL
0000:627b  40                 INC AX
0000:627c  e6 a4              OUT 0xa4,AL
0000:627e  e8 02 00           CALL 0x0000:6283
0000:6281  07                 POP ES
0000:6282  c3                 RET
0000:6283  33 c0              XOR AX,AX
0000:6285  b9 00 a8           MOV CX,0xa800
0000:6288  8e c1              MOV CX,ES
0000:628a  33 ff              XOR DI,DI
0000:628c  b9 80 3e           MOV CX,0x3e80
0000:628f  f3 ab              STOSW.REP ES:DI
0000:6291  b9 00 b0           MOV CX,0xb000
0000:6294  8e c1              MOV CX,ES
0000:6296  33 ff              XOR DI,DI
0000:6298  b9 80 3e           MOV CX,0x3e80
0000:629b  f3 ab              STOSW.REP ES:DI
0000:629d  b9 00 b8           MOV CX,0xb800
0000:62a0  8e c1              MOV CX,ES
0000:62a2  33 ff              XOR DI,DI
0000:62a4  b9 80 3e           MOV CX,0x3e80
0000:62a7  f3 ab              STOSW.REP ES:DI
0000:62a9  b9 00 e0           MOV CX,0xe000
0000:62ac  8e c1              MOV CX,ES
0000:62ae  33 ff              XOR DI,DI
0000:62b0  b9 80 3e           MOV CX,0x3e80
0000:62b3  f3 ab              STOSW.REP ES:DI
0000:62b5  c3                 RET
0000:62b6  33 c0              XOR AX,AX
0000:62b8  e6 a8              OUT 0xa8,AL
0000:62ba  86 c4              XCHG AH,AL
0000:62bc  e6 ac              OUT 0xac,AL
0000:62be  e6 aa              OUT 0xaa,AL
0000:62c0  e6 ae              OUT 0xae,AL
0000:62c2  86 c4              XCHG AH,AL
0000:62c4  fe c0              INC AL
0000:62c6  3c 10              CMP AL,0x10
0000:62c8  72 ee              JC 0x0000:62b8
0000:62ca  c3                 RET
0000:62cb  8c c8              MOV AX,CS
0000:62cd  8e d8              MOV AX,DS
0000:62cf  ba d7 62           MOV DX,0x62d7
0000:62d2  b4 09              MOV AH,0x9
0000:62d4  cd 21              INT 0x21
0000:62d6  c3                 RET
0000:62e4  8c c8              MOV AX,CS
0000:62e6  8e d8              MOV AX,DS
0000:62e8  ba f0 62           MOV DX,0x62f0
0000:62eb  b4 09              MOV AH,0x9
0000:62ed  cd 21              INT 0x21
0000:62ef  c3                 RET
0000:62fb  b5 c0              MOV CH,0xc0
0000:62fd  b4 42              MOV AH,0x42
0000:62ff  cd 18              INT 0x18
0000:6301  b0 02              MOV AL,0x2
0000:6303  e6 68              OUT 0x68,AL
0000:6305  b0 04              MOV AL,0x4
0000:6307  e6 68              OUT 0x68,AL
0000:6309  b0 07              MOV AL,0x7
0000:630b  e6 68              OUT 0x68,AL
0000:630d  b0 08              MOV AL,0x8
0000:630f  e6 68              OUT 0x68,AL
0000:6311  b0 01              MOV AL,0x1
0000:6313  e6 6a              OUT 0x6a,AL
0000:6315  b0 41              MOV AL,0x41
0000:6317  e6 6a              OUT 0x6a,AL
0000:6319  b0 c0              MOV AL,0xc0
0000:631b  e6 7c              OUT 0x7c,AL
0000:631d  b0 ff              MOV AL,0xff
0000:631f  e6 7e              OUT 0x7e,AL
0000:6321  e6 7e              OUT 0x7e,AL
0000:6323  e6 7e              OUT 0x7e,AL
0000:6325  e6 7e              OUT 0x7e,AL
0000:6327  32 c0              XOR AL,AL
0000:6329  e6 7c              OUT 0x7c,AL
0000:632b  c3                 RET
0000:632c  b0 40              MOV AL,0x40
0000:632e  e6 6a              OUT 0x6a,AL
0000:6330  c3                 RET
0000:6331  b4 40              MOV AH,0x40
0000:6333  cd 18              INT 0x18
0000:6335  c3                 RET
0000:633b  b4 0c              MOV AH,0xc
0000:633d  cd 18              INT 0x18
0000:633f  c3                 RET
0000:6345  32 c0              XOR AL,AL
0000:6347  e6 a4              OUT 0xa4,AL
0000:6349  e6 a6              OUT 0xa6,AL
0000:634b  c3                 RET
0000:634c  33 c0              XOR AX,AX
0000:634e  8e c0              MOV AX,ES
0000:6350  26 a0 00 05        MOV AL,ES:[0x500]
0000:6354  26 80 0e 00 05 20  OR byte ptr ES:[0x500],0x20
0000:635a  2e a2 5f 63        MOV CS:[0x635f],AL
0000:635e  c3                 RET
0000:6360  33 c0              XOR AX,AX
0000:6362  8e c0              MOV AX,ES
0000:6364  2e a0 5f 63        MOV AL,CS:[0x635f]
0000:6368  26 a2 00 05        MOV ES:[0x500],AL
0000:636c  c3                 RET
0000:636d  53                 PUSH BX
0000:636e  b4 05              MOV AH,0x5
0000:6370  cd 18              INT 0x18
0000:6372  0a ff              OR BH,BH
0000:6374  75 f8              JNZ 0x0000:636e
0000:6376  5b                 POP BX
0000:6377  c3                 RET
0000:63ce  1e                 PUSH DS
0000:63cf  80 26 05 04 08     AND byte ptr [0x405],0x8
0000:63d4  e8 43 00           CALL 0x0000:641a
0000:63d7  72 05              JC 0x0000:63de
0000:63d9  80 0e 05 04 01     OR byte ptr [0x405],0x1
0000:63de  e8 5b 00           CALL 0x0000:643c
0000:63e1  72 05              JC 0x0000:63e8
0000:63e3  80 0e 05 04 02     OR byte ptr [0x405],0x2
0000:63e8  e8 73 00           CALL 0x0000:645e
0000:63eb  72 05              JC 0x0000:63f2
0000:63ed  80 0e 05 04 08     OR byte ptr [0x405],0x8
0000:63f2  e8 1e 00           CALL 0x0000:6413
0000:63f5  73 05              JNC 0x0000:63fc
0000:63f7  80 0e 05 04 80     OR byte ptr [0x405],0x80
0000:63fc  06                 PUSH ES
0000:63fd  33 c0              XOR AX,AX
0000:63ff  8e c0              MOV AX,ES
0000:6401  26 a0 4d 05        MOV AL,ES:[0x54d]
0000:6405  07                 POP ES
0000:6406  24 20              AND AL,0x20
0000:6408  d0 e0              SHL AL,0x1
0000:640a  08 06 05 04        OR byte ptr [0x405],AL
0000:640e  e8 84 00           CALL 0x0000:6495
0000:6411  1f                 POP DS
0000:6412  c3                 RET
0000:6413  b4 30              MOV AH,0x30
0000:6415  cd 21              INT 0x21
0000:6417  3c 03              CMP AL,0x3
0000:6419  c3                 RET
0000:641a  50                 PUSH AX
0000:641b  06                 PUSH ES
0000:641c  b8 00 00           MOV AX,0x0
0000:641f  8e c0              MOV AX,ES
0000:6421  26 81 3e fc 01 00 08  CMP word ptr ES:[0x1fc],0x800
0000:6428  74 03              JZ 0x0000:642d
0000:642a  f9                 STC
0000:642b  eb 0c              JMP 0x0000:6439
0000:642d  26 a1 fe 01        MOV AX,ES:[0x1fe]
0000:6431  26 3b 06 52 00     CMP AX,word ptr ES:[0x52]
0000:6436  74 01              JZ 0x0000:6439
0000:6438  f9                 STC
0000:6439  07                 POP ES
0000:643a  58                 POP AX
0000:643b  c3                 RET
0000:643c  50                 PUSH AX
0000:643d  06                 PUSH ES
0000:643e  b8 00 00           MOV AX,0x0
0000:6441  8e c0              MOV AX,ES
0000:6443  26 81 3e fc 01 00 08  CMP word ptr ES:[0x1fc],0x800
0000:644a  74 03              JZ 0x0000:644f
0000:644c  f9                 STC
0000:644d  eb 0c              JMP 0x0000:645b
0000:644f  26 a1 fe 01        MOV AX,ES:[0x1fe]
0000:6453  26 3b 06 22 00     CMP AX,word ptr ES:[0x22]
0000:6458  74 01              JZ 0x0000:645b
0000:645a  f9                 STC
0000:645b  07                 POP ES
0000:645c  58                 POP AX
0000:645d  c3                 RET
0000:645e  50                 PUSH AX
0000:645f  53                 PUSH BX
0000:6460  51                 PUSH CX
0000:6461  56                 PUSH SI
0000:6462  57                 PUSH DI
0000:6463  1e                 PUSH DS
0000:6464  06                 PUSH ES
0000:6465  8c c8              MOV AX,CS
0000:6467  8e d8              MOV AX,DS
0000:6469  b8 7b 35           MOV AX,0x357b
0000:646c  cd 21              INT 0x21
0000:646e  bf 06 01           MOV DI,0x106
0000:6471  b9 06 00           MOV CX,0x6
0000:6474  be 89 64           MOV SI,0x6489
0000:6477  f3 a7              CMPSW.REPE ES:DI,SI
0000:6479  0b c9              OR CX,CX
0000:647b  74 03              JZ 0x0000:6480
0000:647d  f9                 STC
0000:647e  eb 01              JMP 0x0000:6481
0000:6480  f8                 CLC
0000:6481  07                 POP ES
0000:6482  1f                 POP DS
0000:6483  5f                 POP DI
0000:6484  5e                 POP SI
0000:6485  59                 POP CX
0000:6486  5b                 POP BX
0000:6487  58                 POP AX
0000:6488  c3                 RET
0000:6495  b4 19              MOV AH,0x19
0000:6497  cd 21              INT 0x21
0000:6499  a2 a6 64           MOV [0x64a6],AL
0000:649c  8a d0              MOV DL,AL
0000:649e  b4 0e              MOV AH,0xe
0000:64a0  cd 21              INT 0x21
0000:64a2  a2 a7 64           MOV [0x64a7],AL
0000:64a5  c3                 RET
0000:64a8  bb 00 00           MOV BX,0x0
0000:64ab  e8 00 00           CALL 0x0000:64ae
0000:64ae  e8 26 00           CALL 0x0000:64d7
0000:64b1  b0 70              MOV AL,0x70
0000:64b3  e6 a2              OUT 0xa2,AL
0000:64b5  e8 1f 00           CALL 0x0000:64d7
0000:64b8  8a c3              MOV AL,BL
0000:64ba  e6 a0              OUT 0xa0,AL
0000:64bc  e8 18 00           CALL 0x0000:64d7
0000:64bf  8a c7              MOV AL,BH
0000:64c1  e6 a0              OUT 0xa0,AL
0000:64c3  e8 11 00           CALL 0x0000:64d7
0000:64c6  32 c0              XOR AL,AL
0000:64c8  e6 a0              OUT 0xa0,AL
0000:64ca  e8 0a 00           CALL 0x0000:64d7
0000:64cd  a0 05 04           MOV AL,[0x405]
0000:64d0  24 40              AND AL,0x40
0000:64d2  04 19              ADD AL,0x19
0000:64d4  e6 a0              OUT 0xa0,AL
0000:64d6  c3                 RET
0000:64d7  e4 60              IN AL,0x60
0000:64d9  a8 02              TEST AL,0x2
0000:64db  75 fa              JNZ 0x0000:64d7
0000:64dd  eb 00              JMP 0x0000:64df
0000:64df  eb 00              JMP 0x0000:64e1
0000:64e1  c3                 RET
0000:64f0  2e a3 26 67        MOV CS:[0x6726],AX
0000:64f4  2e 89 1e 28 67     MOV word ptr CS:[0x6728],BX
0000:64f9  2e 89 0e 2a 67     MOV word ptr CS:[0x672a],CX
0000:64fe  2e 89 16 2c 67     MOV word ptr CS:[0x672c],DX
0000:6503  2e 89 3e 2e 67     MOV word ptr CS:[0x672e],DI
0000:6508  2e 8c 06 30 67     MOV word ptr CS:[0x6730],ES
0000:650d  2e 88 26 85 67     MOV byte ptr CS:[0x6785],AH
0000:6512  2e c6 06 84 67 00  MOV byte ptr CS:[0x6784],0x0
0000:6518  2e c6 06 02 04 00  MOV byte ptr CS:[0x402],0x0
0000:651e  fc                 CLD
0000:651f  8b d6              MOV DX,SI
0000:6521  b8 00 3d           MOV AX,0x3d00
0000:6524  cd 21              INT 0x21
0000:6526  73 0f              JNC 0x0000:6537
0000:6528  b4 3d              MOV AH,0x3d
0000:652a  e8 a1 01           CALL 0x0000:66ce
0000:652d  73 f0              JNC 0x0000:651f
0000:652f  2e c6 06 84 67 01  MOV byte ptr CS:[0x6784],0x1
0000:6535  eb 6d              JMP 0x0000:65a4
0000:6537  2e a3 32 67        MOV CS:[0x6732],AX
0000:653b  2e 8b 1e 32 67     MOV BX,word ptr CS:[0x6732]
0000:6540  2e 8b 16 2c 67     MOV DX,word ptr CS:[0x672c]
0000:6545  2e 8b 0e 28 67     MOV CX,word ptr CS:[0x6728]
0000:654a  2e a1 26 67        MOV AX,CS:[0x6726]
0000:654e  b4 42              MOV AH,0x42
0000:6550  cd 21              INT 0x21
0000:6552  73 0f              JNC 0x0000:6563
0000:6554  b4 42              MOV AH,0x42
0000:6556  e8 75 01           CALL 0x0000:66ce
0000:6559  73 c4              JNC 0x0000:651f
0000:655b  2e c6 06 84 67 01  MOV byte ptr CS:[0x6784],0x1
0000:6561  eb 30              JMP 0x0000:6593
0000:6563  2e 8b 1e 32 67     MOV BX,word ptr CS:[0x6732]
0000:6568  2e 8b 0e 2a 67     MOV CX,word ptr CS:[0x672a]
0000:656d  2e 8b 16 2e 67     MOV DX,word ptr CS:[0x672e]
0000:6572  2e a1 30 67        MOV AX,CS:[0x6730]
0000:6576  1e                 PUSH DS
0000:6577  8e d8              MOV AX,DS
0000:6579  b4 3f              MOV AH,0x3f
0000:657b  cd 21              INT 0x21
0000:657d  1f                 POP DS
0000:657e  73 0f              JNC 0x0000:658f
0000:6580  b4 3f              MOV AH,0x3f
0000:6582  e8 49 01           CALL 0x0000:66ce
0000:6585  73 98              JNC 0x0000:651f
0000:6587  2e c6 06 84 67 01  MOV byte ptr CS:[0x6784],0x1
0000:658d  eb 04              JMP 0x0000:6593
0000:658f  2e a3 26 67        MOV CS:[0x6726],AX
0000:6593  2e 8b 1e 32 67     MOV BX,word ptr CS:[0x6732]
0000:6598  b4 3e              MOV AH,0x3e
0000:659a  cd 21              INT 0x21
0000:659c  73 06              JNC 0x0000:65a4
0000:659e  2e c6 06 84 67 01  MOV byte ptr CS:[0x6784],0x1
0000:65a4  fc                 CLD
0000:65a5  2e a0 84 67        MOV AL,CS:[0x6784]
0000:65a9  d0 e8              SHR AL,0x1
0000:65ab  2e 8b 3e 2e 67     MOV DI,word ptr CS:[0x672e]
0000:65b0  2e 8b 16 2c 67     MOV DX,word ptr CS:[0x672c]
0000:65b5  2e 8b 0e 2a 67     MOV CX,word ptr CS:[0x672a]
0000:65ba  2e 8b 1e 28 67     MOV BX,word ptr CS:[0x6728]
0000:65bf  2e a1 26 67        MOV AX,CS:[0x6726]
0000:65c3  cb                 RETF
0000:65c4  2e a3 86 67        MOV CS:[0x6786],AX
0000:65c8  2e 89 1e 88 67     MOV word ptr CS:[0x6788],BX
0000:65cd  2e 89 0e 8a 67     MOV word ptr CS:[0x678a],CX
0000:65d2  2e 89 16 8c 67     MOV word ptr CS:[0x678c],DX
0000:65d7  2e 89 36 8e 67     MOV word ptr CS:[0x678e],SI
0000:65dc  2e 89 3e 90 67     MOV word ptr CS:[0x6790],DI
0000:65e1  2e 8c 06 92 67     MOV word ptr CS:[0x6792],ES
0000:65e6  2e 88 26 e9 67     MOV byte ptr CS:[0x67e9],AH
0000:65eb  33 c0              XOR AX,AX
0000:65ed  2e a3 94 67        MOV CS:[0x6794],AX
0000:65f1  2e a3 96 67        MOV CS:[0x6796],AX
0000:65f5  2e a2 e8 67        MOV CS:[0x67e8],AL
0000:65f9  2e a2 02 04        MOV CS:[0x402],AL
0000:65fd  fc                 CLD
0000:65fe  be 98 67           MOV SI,0x6798
0000:6601  8b d6              MOV DX,SI
0000:6603  33 c9              XOR CX,CX
0000:6605  b4 3c              MOV AH,0x3c
0000:6607  cd 21              INT 0x21
0000:6609  73 09              JNC 0x0000:6614
0000:660b  b4 3c              MOV AH,0x3c
0000:660d  e8 d4 00           CALL 0x0000:66e4
0000:6610  73 ef              JNC 0x0000:6601
0000:6612  eb 49              JMP 0x0000:665d
0000:6614  2e a3 96 67        MOV CS:[0x6796],AX
0000:6618  2e 8b 1e 96 67     MOV BX,word ptr CS:[0x6796]
0000:661d  2e 8b 0e 8a 67     MOV CX,word ptr CS:[0x678a]
0000:6622  2e 8b 16 90 67     MOV DX,word ptr CS:[0x6790]
0000:6627  1e                 PUSH DS
0000:6628  2e 8e 1e 92 67     MOV DS,word ptr CS:[0x6792]
0000:662d  b4 40              MOV AH,0x40
0000:662f  cd 21              INT 0x21
0000:6631  1f                 POP DS
0000:6632  73 1a              JNC 0x0000:664e
0000:6634  b4 40              MOV AH,0x40
0000:6636  e8 ab 00           CALL 0x0000:66e4
0000:6639  72 0b              JC 0x0000:6646
0000:663b  2e 8b 1e 96 67     MOV BX,word ptr CS:[0x6796]
0000:6640  b4 3e              MOV AH,0x3e
0000:6642  cd 21              INT 0x21
0000:6644  73 08              JNC 0x0000:664e
0000:6646  2e c6 06 e8 67 01  MOV byte ptr CS:[0x67e8],0x1
0000:664c  eb 04              JMP 0x0000:6652
0000:664e  2e a3 94 67        MOV CS:[0x6794],AX
0000:6652  2e 8b 1e 96 67     MOV BX,word ptr CS:[0x6796]
0000:6657  b4 3e              MOV AH,0x3e
0000:6659  cd 21              INT 0x21
0000:665b  73 06              JNC 0x0000:6663
0000:665d  2e c6 06 e8 67 01  MOV byte ptr CS:[0x67e8],0x1
0000:6663  2e c6 06 02 04 00  MOV byte ptr CS:[0x402],0x0
0000:6669  2e a0 e8 67        MOV AL,CS:[0x67e8]
0000:666d  d0 e8              SHR AL,0x1
0000:666f  2e 8e 06 92 67     MOV ES,word ptr CS:[0x6792]
0000:6674  2e 8b 3e 90 67     MOV DI,word ptr CS:[0x6790]
0000:6679  2e 8b 36 8e 67     MOV SI,word ptr CS:[0x678e]
0000:667e  2e 8b 16 8c 67     MOV DX,word ptr CS:[0x678c]
0000:6683  2e 8b 0e 8a 67     MOV CX,word ptr CS:[0x678a]
0000:6688  2e 8b 1e 88 67     MOV BX,word ptr CS:[0x6788]
0000:668d  2e a1 94 67        MOV AX,CS:[0x6794]
0000:6691  cb                 RETF
0000:6692  2e c6 06 e9 67 00  MOV byte ptr CS:[0x67e9],0x0
0000:6698  52                 PUSH DX
0000:6699  1e                 PUSH DS
0000:669a  8c c8              MOV AX,CS
0000:669c  8e d8              MOV AX,DS
0000:669e  ba 98 67           MOV DX,0x6798
0000:66a1  b8 00 3d           MOV AX,0x3d00
0000:66a4  cd 21              INT 0x21
0000:66a6  73 03              JNC 0x0000:66ab
0000:66a8  f8                 CLC
0000:66a9  eb 20              JMP 0x0000:66cb
0000:66ab  8b d8              MOV BX,AX
0000:66ad  b4 3e              MOV AH,0x3e
0000:66af  cd 21              INT 0x21
0000:66b1  72 18              JC 0x0000:66cb
0000:66b3  ba 98 67           MOV DX,0x6798
0000:66b6  b4 41              MOV AH,0x41
0000:66b8  cd 21              INT 0x21
0000:66ba  73 0f              JNC 0x0000:66cb
0000:66bc  b4 41              MOV AH,0x41
0000:66be  e8 23 00           CALL 0x0000:66e4
0000:66c1  73 08              JNC 0x0000:66cb
0000:66c3  2e c6 06 e8 67 01  MOV byte ptr CS:[0x67e8],0x1
0000:66c9  eb e8              JMP 0x0000:66b3
0000:66cb  1f                 POP DS
0000:66cc  5a                 POP DX
0000:66cd  cb                 RETF
0000:66ce  50                 PUSH AX
0000:66cf  51                 PUSH CX
0000:66d0  52                 PUSH DX
0000:66d1  06                 PUSH ES
0000:66d2  8b c8              MOV CX,AX
0000:66d4  ba 34 67           MOV DX,0x6734
0000:66d7  8c c8              MOV AX,CS
0000:66d9  8e c0              MOV AX,ES
0000:66db  33 c0              XOR AX,AX
0000:66dd  2e 8a 26 85 67     MOV AH,byte ptr CS:[0x6785]
0000:66e2  eb 16              JMP 0x0000:66fa
0000:66e4  50                 PUSH AX
0000:66e5  51                 PUSH CX
0000:66e6  52                 PUSH DX
0000:66e7  06                 PUSH ES
0000:66e8  8b c8              MOV CX,AX
0000:66ea  ba 98 67           MOV DX,0x6798
0000:66ed  8c c8              MOV AX,CS
0000:66ef  8e c0              MOV AX,ES
0000:66f1  b0 01              MOV AL,0x1
0000:66f3  2e 8a 26 e9 67     MOV AH,byte ptr CS:[0x67e9]
0000:66f8  eb 00              JMP 0x0000:66fa
0000:66fa  e8 ed 00           CALL 0x0000:67ea
0000:66fd  72 0c              JC 0x0000:670b
0000:66ff  2e c6 06 02 04 00  MOV byte ptr CS:[0x402],0x0
0000:6705  07                 POP ES
0000:6706  5a                 POP DX
0000:6707  59                 POP CX
0000:6708  58                 POP AX
0000:6709  f8                 CLC
0000:670a  c3                 RET
0000:670b  2e 81 0e b8 03 00 80  OR word ptr CS:[0x3b8],0x8000
0000:6712  2e 81 0e ba 03 00 80  OR word ptr CS:[0x3ba],0x8000
0000:6719  2e c6 06 02 04 00  MOV byte ptr CS:[0x402],0x0
0000:671f  07                 POP ES
0000:6720  5a                 POP DX
0000:6721  59                 POP CX
0000:6722  58                 POP AX
0000:6723  f9                 STC
0000:6724  c3                 RET
0000:6734  00 00              ADD byte ptr [BX + SI],AL
0000:6736  00 00              ADD byte ptr [BX + SI],AL
0000:6738  00 00              ADD byte ptr [BX + SI],AL
0000:673a  00 00              ADD byte ptr [BX + SI],AL
0000:673c  00 00              ADD byte ptr [BX + SI],AL
0000:673e  00 00              ADD byte ptr [BX + SI],AL
0000:6740  00 00              ADD byte ptr [BX + SI],AL
0000:6742  00 00              ADD byte ptr [BX + SI],AL
0000:6744  00 00              ADD byte ptr [BX + SI],AL
0000:6746  00 00              ADD byte ptr [BX + SI],AL
0000:6748  00 00              ADD byte ptr [BX + SI],AL
0000:674a  00 00              ADD byte ptr [BX + SI],AL
0000:674c  00 00              ADD byte ptr [BX + SI],AL
0000:674e  00 00              ADD byte ptr [BX + SI],AL
0000:6750  00 00              ADD byte ptr [BX + SI],AL
0000:6752  00 00              ADD byte ptr [BX + SI],AL
0000:6754  00 00              ADD byte ptr [BX + SI],AL
0000:6798  00 00              ADD byte ptr [BX + SI],AL
0000:679a  00 00              ADD byte ptr [BX + SI],AL
0000:679c  00 00              ADD byte ptr [BX + SI],AL
0000:679e  00 00              ADD byte ptr [BX + SI],AL
0000:67a0  00 00              ADD byte ptr [BX + SI],AL
0000:67a2  00 00              ADD byte ptr [BX + SI],AL
0000:67a4  00 00              ADD byte ptr [BX + SI],AL
0000:67a6  00 00              ADD byte ptr [BX + SI],AL
0000:67a8  00 00              ADD byte ptr [BX + SI],AL
0000:67aa  00 00              ADD byte ptr [BX + SI],AL
0000:67ac  00 00              ADD byte ptr [BX + SI],AL
0000:67ae  00 00              ADD byte ptr [BX + SI],AL
0000:67b0  00 00              ADD byte ptr [BX + SI],AL
0000:67b2  00 00              ADD byte ptr [BX + SI],AL
0000:67b4  00 00              ADD byte ptr [BX + SI],AL
0000:67b6  00 00              ADD byte ptr [BX + SI],AL
0000:67b8  00 00              ADD byte ptr [BX + SI],AL
0000:67ea  60                 PUSHA
0000:67eb  1e                 PUSH DS
0000:67ec  06                 PUSH ES
0000:67ed  2e 89 0e 2a 69     MOV word ptr CS:[0x692a],CX
0000:67f2  2e 89 16 2c 69     MOV word ptr CS:[0x692c],DX
0000:67f7  2e 8c 06 2e 69     MOV word ptr CS:[0x692e],ES
0000:67fc  2e 88 26 31 69     MOV byte ptr CS:[0x6931],AH
0000:6801  2e a2 30 69        MOV CS:[0x6930],AL
0000:6805  2e c6 06 32 69 00  MOV byte ptr CS:[0x6932],0x0
0000:680b  b4 41              MOV AH,0x41
0000:680d  cd 18              INT 0x18
0000:680f  33 c0              XOR AX,AX
0000:6811  e6 a8              OUT 0xa8,AL
0000:6813  e6 aa              OUT 0xaa,AL
0000:6815  e6 ac              OUT 0xac,AL
0000:6817  e6 ae              OUT 0xae,AL
0000:6819  b8 00 a1           MOV AX,0xa100
0000:681c  8e c0              MOV AX,ES
0000:681e  bf 00 00           MOV DI,0x0
0000:6821  33 c0              XOR AX,AX
0000:6823  b9 d0 07           MOV CX,0x7d0
0000:6826  f3 ab              STOSW.REP ES:DI
0000:6828  bf 00 20           MOV DI,0x2000
0000:682b  b8 e1 00           MOV AX,0xe1
0000:682e  b9 d0 07           MOV CX,0x7d0
0000:6831  f3 ab              STOSW.REP ES:DI
0000:6833  ba 00 10           MOV DX,0x1000
0000:6836  b4 0e              MOV AH,0xe
0000:6838  cd 18              INT 0x18
0000:683a  1e                 PUSH DS
0000:683b  b8 00 a1           MOV AX,0xa100
0000:683e  8e c0              MOV AX,ES
0000:6840  8c c8              MOV AX,CS
0000:6842  8e d8              MOV AX,DS
0000:6844  be 33 69           MOV SI,0x6933
0000:6847  e8 d9 01           CALL 0x0000:6a23
0000:684a  2e 80 3e 31 69 00  CMP byte ptr CS:[0x6931],0x0
0000:6850  74 06              JZ 0x0000:6858
0000:6852  be b2 69           MOV SI,0x69b2
0000:6855  e8 cb 01           CALL 0x0000:6a23
0000:6858  bf 84 06           MOV DI,0x684
0000:685b  b4 19              MOV AH,0x19
0000:685d  cd 21              INT 0x21
0000:685f  8a e0              MOV AH,AL
0000:6861  80 c4 41           ADD AH,0x41
0000:6864  b0 03              MOV AL,0x3
0000:6866  26 89 05           MOV word ptr ES:[DI],AX
0000:6869  26 c7 85 00 20 e1 00  MOV word ptr ES:[DI + 0x2000],0xe1
0000:6870  0d 80 00           OR AX,0x80
0000:6873  26 89 45 02        MOV word ptr ES:[DI + 0x2],AX
0000:6877  26 c7 85 02 20 e1 00  MOV word ptr ES:[DI + 0x2002],0xe1
0000:687e  bf 96 09           MOV DI,0x996
0000:6881  2e a1 2a 69        MOV AX,CS:[0x692a]
0000:6885  e8 64 01           CALL 0x0000:69ec
0000:6888  bf aa 09           MOV DI,0x9aa
0000:688b  33 c0              XOR AX,AX
0000:688d  2e a0 02 04        MOV AL,CS:[0x402]
0000:6891  e8 58 01           CALL 0x0000:69ec
0000:6894  bf ba 09           MOV DI,0x9ba
0000:6897  2e a1 a4 03        MOV AX,CS:[0x3a4]
0000:689b  e8 4e 01           CALL 0x0000:69ec
0000:689e  bf ca 09           MOV DI,0x9ca
0000:68a1  2e a1 a6 03        MOV AX,CS:[0x3a6]
0000:68a5  e8 44 01           CALL 0x0000:69ec
0000:68a8  1e                 PUSH DS
0000:68a9  2e 8e 1e 2e 69     MOV DS,word ptr CS:[0x692e]
0000:68ae  2e 8b 36 2c 69     MOV SI,word ptr CS:[0x692c]
0000:68b3  bf 68 05           MOV DI,0x568
0000:68b6  e8 6a 01           CALL 0x0000:6a23
0000:68b9  1f                 POP DS
0000:68ba  2e 80 3e 30 69 00  CMP byte ptr CS:[0x6930],0x0
0000:68c0  75 08              JNZ 0x0000:68ca
0000:68c2  be ce 69           MOV SI,0x69ce
0000:68c5  e8 5b 01           CALL 0x0000:6a23
0000:68c8  eb 06              JMP 0x0000:68d0
0000:68ca  be dd 69           MOV SI,0x69dd
0000:68cd  e8 53 01           CALL 0x0000:6a23
0000:68d0  1f                 POP DS
0000:68d1  0e                 PUSH CS
0000:68d2  e8 71 0f           CALL 0x0000:7846
0000:68d5  2e f7 06 ba 03 30 00  TEST word ptr CS:[0x3ba],0x30
0000:68dc  75 16              JNZ 0x0000:68f4
0000:68de  2e 80 3e 31 69 00  CMP byte ptr CS:[0x6931],0x0
0000:68e4  74 eb              JZ 0x0000:68d1
0000:68e6  2e f7 06 ba 03 80 00  TEST word ptr CS:[0x3ba],0x80
0000:68ed  74 e2              JZ 0x0000:68d1
0000:68ef  2e fe 06 32 69     INC byte ptr CS:[0x6932]
0000:68f4  ba 00 00           MOV DX,0x0
0000:68f7  b4 0e              MOV AH,0xe
0000:68f9  cd 18              INT 0x18
0000:68fb  b8 00 a1           MOV AX,0xa100
0000:68fe  8e c0              MOV AX,ES
0000:6900  bf 00 00           MOV DI,0x0
0000:6903  33 c0              XOR AX,AX
0000:6905  b9 d0 07           MOV CX,0x7d0
0000:6908  f3 ab              STOSW.REP ES:DI
0000:690a  bf 00 20           MOV DI,0x2000
0000:690d  b8 e1 00           MOV AX,0xe1
0000:6910  b9 d0 07           MOV CX,0x7d0
0000:6913  f3 ab              STOSW.REP ES:DI
0000:6915  b4 40              MOV AH,0x40
0000:6917  cd 18              INT 0x18
0000:6919  07                 POP ES
0000:691a  1f                 POP DS
0000:691b  2e c6 06 04 04 01  MOV byte ptr CS:[0x404],0x1
0000:6921  2e a0 32 69        MOV AL,CS:[0x6932]
0000:6925  d0 e8              SHR AL,0x1
0000:6927  61                 POPA
0000:6928  c3                 RET
0000:69ec  50                 PUSH AX
0000:69ed  53                 PUSH BX
0000:69ee  51                 PUSH CX
0000:69ef  52                 PUSH DX
0000:69f0  8b d0              MOV DX,AX
0000:69f2  33 c0              XOR AX,AX
0000:69f4  b9 04 00           MOV CX,0x4
0000:69f7  c1 c2 04           ROL DX,0x4
0000:69fa  8b da              MOV BX,DX
0000:69fc  83 e3 0f           AND BX,0xf
0000:69ff  2e 8a 87 13 6a     MOV AL,byte ptr CS:[BX + 0x6a13]
0000:6a04  26 c7 85 00 20 e1 00  MOV word ptr ES:[DI + 0x2000],0xe1
0000:6a0b  ab                 STOSW ES:DI
0000:6a0c  e2 e9              LOOP 0x0000:69f7
0000:6a0e  5a                 POP DX
0000:6a0f  59                 POP CX
0000:6a10  5b                 POP BX
0000:6a11  58                 POP AX
0000:6a12  c3                 RET
0000:6a23  50                 PUSH AX
0000:6a24  52                 PUSH DX
0000:6a25  56                 PUSH SI
0000:6a26  ba e1 00           MOV DX,0xe1
0000:6a29  ad                 LODSW SI
0000:6a2a  3c 00              CMP AL,0x0
0000:6a2c  74 6e              JZ 0x0000:6a9c
0000:6a2e  3c fa              CMP AL,0xfa
0000:6a30  74 55              JZ 0x0000:6a87
0000:6a32  3c ff              CMP AL,0xff
0000:6a34  74 66              JZ 0x0000:6a9c
0000:6a36  3c 20              CMP AL,0x20
0000:6a38  72 1e              JC 0x0000:6a58
0000:6a3a  3c 80              CMP AL,0x80
0000:6a3c  72 0a              JC 0x0000:6a48
0000:6a3e  3c a1              CMP AL,0xa1
0000:6a40  72 16              JC 0x0000:6a58
0000:6a42  3c e0              CMP AL,0xe0
0000:6a44  72 02              JC 0x0000:6a48
0000:6a46  eb 10              JMP 0x0000:6a58
0000:6a48  4e                 DEC SI
0000:6a49  32 e4              XOR AH,AH
0000:6a4b  26 89 05           MOV word ptr ES:[DI],AX
0000:6a4e  26 89 95 00 20     MOV word ptr ES:[DI + 0x2000],DX
0000:6a53  83 c7 02           ADD DI,0x2
0000:6a56  eb d1              JMP 0x0000:6a29
0000:6a58  02 c0              ADD AL,AL
0000:6a5a  80 ec 1f           SUB AH,0x1f
0000:6a5d  78 06              JS 0x0000:6a65
0000:6a5f  80 fc 61           CMP AH,0x61
0000:6a62  80 d4 de           ADC AH,0xde
0000:6a65  80 c4 a1           ADD AH,0xa1
0000:6a68  14 1f              ADC AL,0x1f
0000:6a6a  25 7f 7f           AND AX,0x7f7f
0000:6a6d  2c 20              SUB AL,0x20
0000:6a6f  26 89 05           MOV word ptr ES:[DI],AX
0000:6a72  26 89 95 00 20     MOV word ptr ES:[DI + 0x2000],DX
0000:6a77  04 80              ADD AL,0x80
0000:6a79  26 89 45 02        MOV word ptr ES:[DI + 0x2],AX
0000:6a7d  26 89 95 02 20     MOV word ptr ES:[DI + 0x2002],DX
0000:6a82  83 c7 04           ADD DI,0x4
0000:6a85  eb a2              JMP 0x0000:6a29
0000:6a87  8a c4              MOV AL,AH
0000:6a89  32 e4              XOR AH,AH
0000:6a8b  03 c0              ADD AX,AX
0000:6a8d  8b f8              MOV DI,AX
0000:6a8f  ac                 LODSB SI
0000:6a90  c1 e0 05           SHL AX,0x5
0000:6a93  03 f8              ADD DI,AX
0000:6a95  c1 e0 02           SHL AX,0x2
0000:6a98  03 f8              ADD DI,AX
0000:6a9a  eb 8d              JMP 0x0000:6a29
0000:6a9c  5e                 POP SI
0000:6a9d  5a                 POP DX
0000:6a9e  58                 POP AX
0000:6a9f  c3                 RET
0000:6aa0  e8 2a 00           CALL 0x0000:6acd
0000:6aa3  cb                 RETF
0000:6aa4  b8 00 a8           MOV AX,0xa800
0000:6aa7  8e c0              MOV AX,ES
0000:6aa9  33 ff              XOR DI,DI
0000:6aab  e8 1f 00           CALL 0x0000:6acd
0000:6aae  b8 00 b0           MOV AX,0xb000
0000:6ab1  8e c0              MOV AX,ES
0000:6ab3  33 ff              XOR DI,DI
0000:6ab5  e8 15 00           CALL 0x0000:6acd
0000:6ab8  b8 00 b8           MOV AX,0xb800
0000:6abb  8e c0              MOV AX,ES
0000:6abd  33 ff              XOR DI,DI
0000:6abf  e8 0b 00           CALL 0x0000:6acd
0000:6ac2  b8 00 e0           MOV AX,0xe000
0000:6ac5  8e c0              MOV AX,ES
0000:6ac7  33 ff              XOR DI,DI
0000:6ac9  e8 01 00           CALL 0x0000:6acd
0000:6acc  cb                 RETF
0000:6acd  33 c9              XOR CX,CX
0000:6acf  ac                 LODSB SI
0000:6ad0  0a c0              OR AL,AL
0000:6ad2  74 31              JZ 0x0000:6b05
0000:6ad4  78 06              JS 0x0000:6adc
0000:6ad6  8a c8              MOV CL,AL
0000:6ad8  f3 a4              MOVSB.REP ES:DI,SI
0000:6ada  eb f3              JMP 0x0000:6acf
0000:6adc  83 e0 7f           AND AX,0x7f
0000:6adf  04 03              ADD AL,0x3
0000:6ae1  8b c8              MOV CX,AX
0000:6ae3  ac                 LODSB SI
0000:6ae4  40                 INC AX
0000:6ae5  8b de              MOV BX,SI
0000:6ae7  8c da              MOV DX,DS
0000:6ae9  8b f7              MOV SI,DI
0000:6aeb  2b f0              SUB SI,AX
0000:6aed  8c c0              MOV AX,ES
0000:6aef  8e d8              MOV AX,DS
0000:6af1  72 08              JC 0x0000:6afb
0000:6af3  f3 a4              MOVSB.REP ES:DI,SI
0000:6af5  8b f3              MOV SI,BX
0000:6af7  8e da              MOV DX,DS
0000:6af9  eb d4              JMP 0x0000:6acf
0000:6afb  32 c0              XOR AL,AL
0000:6afd  aa                 STOSB ES:DI
0000:6afe  46                 INC SI
0000:6aff  e0 fc              LOOPNZ 0x0000:6afd
0000:6b01  74 f0              JZ 0x0000:6af3
0000:6b03  eb f0              JMP 0x0000:6af5
0000:6b05  c3                 RET
0000:6bd1  06                 PUSH ES
0000:6bd2  8c c8              MOV AX,CS
0000:6bd4  8e c0              MOV AX,ES
0000:6bd6  bf 70 6b           MOV DI,0x6b70
0000:6bd9  33 c0              XOR AX,AX
0000:6bdb  b9 30 00           MOV CX,0x30
0000:6bde  f3 ab              STOSW.REP ES:DI
0000:6be0  33 c0              XOR AX,AX
0000:6be2  ac                 LODSB SI
0000:6be3  3c 0f              CMP AL,0xf
0000:6be5  77 1d              JA 0x0000:6c04
0000:6be7  d1 e0              SHL AX,0x1
0000:6be9  8b f8              MOV DI,AX
0000:6beb  d1 e0              SHL AX,0x1
0000:6bed  03 f8              ADD DI,AX
0000:6bef  81 c7 10 6b        ADD DI,0x6b10
0000:6bf3  ac                 LODSB SI
0000:6bf4  c0 e0 04           SHL AL,0x4
0000:6bf7  ab                 STOSW ES:DI
0000:6bf8  ac                 LODSB SI
0000:6bf9  c0 e0 04           SHL AL,0x4
0000:6bfc  ab                 STOSW ES:DI
0000:6bfd  ac                 LODSB SI
0000:6bfe  c0 e0 04           SHL AL,0x4
0000:6c01  ab                 STOSW ES:DI
0000:6c02  eb dc              JMP 0x0000:6be0
0000:6c04  c6 86 b8 00 01     MOV byte ptr [BP + 0xb8],0x1
0000:6c09  07                 POP ES
0000:6c0a  cb                 RETF
0000:6c0b  1e                 PUSH DS
0000:6c0c  8c c8              MOV AX,CS
0000:6c0e  8e d8              MOV AX,DS
0000:6c10  80 be b8 00 00     CMP byte ptr [BP + 0xb8],0x0
0000:6c15  74 41              JZ 0x0000:6c58
0000:6c17  c6 86 b8 00 00     MOV byte ptr [BP + 0xb8],0x0
0000:6c1c  be 10 6b           MOV SI,0x6b10
0000:6c1f  33 c9              XOR CX,CX
0000:6c21  80 3e d0 6b 00     CMP byte ptr [0x6bd0],0x0
0000:6c26  75 32              JNZ 0x0000:6c5a
0000:6c28  83 f9 10           CMP CX,0x10
0000:6c2b  73 2b              JNC 0x0000:6c58
0000:6c2d  8a c1              MOV AL,CL
0000:6c2f  e6 a8              OUT 0xa8,AL
0000:6c31  ad                 LODSW SI
0000:6c32  0a e4              OR AH,AH
0000:6c34  74 02              JZ 0x0000:6c38
0000:6c36  b0 f0              MOV AL,0xf0
0000:6c38  c0 f8 04           SAR AL,0x4
0000:6c3b  e6 ac              OUT 0xac,AL
0000:6c3d  ad                 LODSW SI
0000:6c3e  0a e4              OR AH,AH
0000:6c40  74 02              JZ 0x0000:6c44
0000:6c42  b0 f0              MOV AL,0xf0
0000:6c44  c0 f8 04           SAR AL,0x4
0000:6c47  e6 aa              OUT 0xaa,AL
0000:6c49  ad                 LODSW SI
0000:6c4a  0a e4              OR AH,AH
0000:6c4c  74 02              JZ 0x0000:6c50
0000:6c4e  b0 f0              MOV AL,0xf0
0000:6c50  c0 f8 04           SAR AL,0x4
0000:6c53  e6 ae              OUT 0xae,AL
0000:6c55  41                 INC CX
0000:6c56  eb d0              JMP 0x0000:6c28
0000:6c58  1f                 POP DS
0000:6c59  cb                 RETF
0000:6c5a  80 f9 10           CMP CL,0x10
0000:6c5d  73 f9              JNC 0x0000:6c58
0000:6c5f  8a c1              MOV AL,CL
0000:6c61  e6 a8              OUT 0xa8,AL
0000:6c63  ad                 LODSW SI
0000:6c64  0a e4              OR AH,AH
0000:6c66  74 02              JZ 0x0000:6c6a
0000:6c68  b0 f0              MOV AL,0xf0
0000:6c6a  c0 f8 04           SAR AL,0x4
0000:6c6d  c0 e0 02           SHL AL,0x2
0000:6c70  8a e8              MOV CH,AL
0000:6c72  ad                 LODSW SI
0000:6c73  0a e4              OR AH,AH
0000:6c75  74 02              JZ 0x0000:6c79
0000:6c77  b0 f0              MOV AL,0xf0
0000:6c79  c0 f8 04           SAR AL,0x4
0000:6c7c  02 e8              ADD CH,AL
0000:6c7e  d0 e0              SHL AL,0x1
0000:6c80  02 e8              ADD CH,AL
0000:6c82  ad                 LODSW SI
0000:6c83  0a e4              OR AH,AH
0000:6c85  74 02              JZ 0x0000:6c89
0000:6c87  b0 f0              MOV AL,0xf0
0000:6c89  c0 f8 04           SAR AL,0x4
0000:6c8c  02 e8              ADD CH,AL
0000:6c8e  32 c0              XOR AL,AL
0000:6c90  c0 e5 02           SHL CH,0x2
0000:6c93  1a c0              SBB AL,AL
0000:6c95  e6 ac              OUT 0xac,AL
0000:6c97  32 c0              XOR AL,AL
0000:6c99  d0 e5              SHL CH,0x1
0000:6c9b  1a c0              SBB AL,AL
0000:6c9d  e6 aa              OUT 0xaa,AL
0000:6c9f  32 c0              XOR AL,AL
0000:6ca1  d0 e5              SHL CH,0x1
0000:6ca3  1a c0              SBB AL,AL
0000:6ca5  e6 ae              OUT 0xae,AL
0000:6ca7  fe c1              INC CL
0000:6ca9  eb af              JMP 0x0000:6c5a
0000:6cab  06                 PUSH ES
0000:6cac  8c c8              MOV AX,CS
0000:6cae  8e c0              MOV AX,ES
0000:6cb0  bf 70 6b           MOV DI,0x6b70
0000:6cb3  33 c0              XOR AX,AX
0000:6cb5  b9 30 00           MOV CX,0x30
0000:6cb8  f3 ab              STOSW.REP ES:DI
0000:6cba  33 c0              XOR AX,AX
0000:6cbc  ac                 LODSB SI
0000:6cbd  3c 0f              CMP AL,0xf
0000:6cbf  77 3d              JA 0x0000:6cfe
0000:6cc1  8b f8              MOV DI,AX
0000:6cc3  03 c0              ADD AX,AX
0000:6cc5  03 f8              ADD DI,AX
0000:6cc7  d1 e7              SHL DI,0x1
0000:6cc9  8b df              MOV BX,DI
0000:6ccb  81 c7 70 6b        ADD DI,0x6b70
0000:6ccf  81 c3 10 6b        ADD BX,0x6b10
0000:6cd3  33 c0              XOR AX,AX
0000:6cd5  ac                 LODSB SI
0000:6cd6  c1 e0 04           SHL AX,0x4
0000:6cd9  26 2b 07           SUB AX,word ptr ES:[BX]
0000:6cdc  c1 f8 04           SAR AX,0x4
0000:6cdf  ab                 STOSW ES:DI
0000:6ce0  33 c0              XOR AX,AX
0000:6ce2  ac                 LODSB SI
0000:6ce3  c1 e0 04           SHL AX,0x4
0000:6ce6  26 2b 47 02        SUB AX,word ptr ES:[BX + 0x2]
0000:6cea  c1 f8 04           SAR AX,0x4
0000:6ced  ab                 STOSW ES:DI
0000:6cee  33 c0              XOR AX,AX
0000:6cf0  ac                 LODSB SI
0000:6cf1  c1 e0 04           SHL AX,0x4
0000:6cf4  26 2b 47 04        SUB AX,word ptr ES:[BX + 0x4]
0000:6cf8  c1 f8 04           SAR AX,0x4
0000:6cfb  ab                 STOSW ES:DI
0000:6cfc  eb bc              JMP 0x0000:6cba
0000:6cfe  07                 POP ES
0000:6cff  cb                 RETF
0000:6d00  8a c8              MOV CL,AL
0000:6d02  1e                 PUSH DS
0000:6d03  8c c8              MOV AX,CS
0000:6d05  8e d8              MOV AX,DS
0000:6d07  be 70 6b           MOV SI,0x6b70
0000:6d0a  bf 10 6b           MOV DI,0x6b10
0000:6d0d  32 ed              XOR CH,CH
0000:6d0f  80 fd 10           CMP CH,0x10
0000:6d12  73 20              JNC 0x0000:6d34
0000:6d14  8b 04              MOV AX,word ptr [SI]
0000:6d16  d3 e0              SHL AX,CL
0000:6d18  01 05              ADD word ptr [DI],AX
0000:6d1a  8b 44 02           MOV AX,word ptr [SI + 0x2]
0000:6d1d  d3 e0              SHL AX,CL
0000:6d1f  01 45 02           ADD word ptr [DI + 0x2],AX
0000:6d22  8b 44 04           MOV AX,word ptr [SI + 0x4]
0000:6d25  d3 e0              SHL AX,CL
0000:6d27  01 45 04           ADD word ptr [DI + 0x4],AX
0000:6d2a  83 c6 06           ADD SI,0x6
0000:6d2d  83 c7 06           ADD DI,0x6
0000:6d30  fe c5              INC CH
0000:6d32  eb db              JMP 0x0000:6d0f
0000:6d34  c6 86 b8 00 01     MOV byte ptr [BP + 0xb8],0x1
0000:6d39  1f                 POP DS
0000:6d3a  cb                 RETF
0000:6d40  55                 PUSH BP
0000:6d41  1e                 PUSH DS
0000:6d42  06                 PUSH ES
0000:6d43  8c d8              MOV AX,DS
0000:6d45  8e c0              MOV AX,ES
0000:6d47  8a 44 01           MOV AL,byte ptr [SI + 0x1]
0000:6d4a  8a 64 02           MOV AH,byte ptr [SI + 0x2]
0000:6d4d  33 db              XOR BX,BX
0000:6d4f  8a 1c              MOV BL,byte ptr [SI]
0000:6d51  03 dd              ADD BX,BP
0000:6d53  36 8e 1f           MOV DS,word ptr SS:[BX]
0000:6d56  8b d8              MOV BX,AX
0000:6d58  83 c6 03           ADD SI,0x3
0000:6d5b  53                 PUSH BX
0000:6d5c  57                 PUSH DI
0000:6d5d  56                 PUSH SI
0000:6d5e  06                 PUSH ES
0000:6d5f  33 c0              XOR AX,AX
0000:6d61  26 8a 04           MOV AL,byte ptr ES:[SI]
0000:6d64  8b f0              MOV SI,AX
0000:6d66  c1 e0 02           SHL AX,0x2
0000:6d69  03 f0              ADD SI,AX
0000:6d6b  c1 e6 05           SHL SI,0x5
0000:6d6e  e8 ea 04           CALL 0x0000:725b
0000:6d71  07                 POP ES
0000:6d72  5e                 POP SI
0000:6d73  46                 INC SI
0000:6d74  83 c7 02           ADD DI,0x2
0000:6d77  fe cb              DEC BL
0000:6d79  75 e2              JNZ 0x0000:6d5d
0000:6d7b  5f                 POP DI
0000:6d7c  5b                 POP BX
0000:6d7d  81 c7 00 05        ADD DI,0x500
0000:6d81  fe cf              DEC BH
0000:6d83  75 d6              JNZ 0x0000:6d5b
0000:6d85  07                 POP ES
0000:6d86  1f                 POP DS
0000:6d87  5d                 POP BP
0000:6d88  cb                 RETF
0000:6d89  55                 PUSH BP
0000:6d8a  1e                 PUSH DS
0000:6d8b  06                 PUSH ES
0000:6d8c  8c d8              MOV AX,DS
0000:6d8e  8e c0              MOV AX,ES
0000:6d90  8a 44 01           MOV AL,byte ptr [SI + 0x1]
0000:6d93  8a 64 02           MOV AH,byte ptr [SI + 0x2]
0000:6d96  33 db              XOR BX,BX
0000:6d98  8a 1c              MOV BL,byte ptr [SI]
0000:6d9a  03 dd              ADD BX,BP
0000:6d9c  36 8e 1f           MOV DS,word ptr SS:[BX]
0000:6d9f  8b d8              MOV BX,AX
0000:6da1  83 c6 03           ADD SI,0x3
0000:6da4  53                 PUSH BX
0000:6da5  57                 PUSH DI
0000:6da6  56                 PUSH SI
0000:6da7  06                 PUSH ES
0000:6da8  33 c0              XOR AX,AX
0000:6daa  26 8a 04           MOV AL,byte ptr ES:[SI]
0000:6dad  c1 e0 07           SHL AX,0x7
0000:6db0  8b f0              MOV SI,AX
0000:6db2  e8 8d 05           CALL 0x0000:7342
0000:6db5  07                 POP ES
0000:6db6  5e                 POP SI
0000:6db7  46                 INC SI
0000:6db8  83 c7 02           ADD DI,0x2
0000:6dbb  fe cb              DEC BL
0000:6dbd  75 e7              JNZ 0x0000:6da6
0000:6dbf  5f                 POP DI
0000:6dc0  5b                 POP BX
0000:6dc1  81 c7 00 05        ADD DI,0x500
0000:6dc5  fe cf              DEC BH
0000:6dc7  75 db              JNZ 0x0000:6da4
0000:6dc9  07                 POP ES
0000:6dca  1f                 POP DS
0000:6dcb  5d                 POP BP
0000:6dcc  cb                 RETF
0000:6dcd  55                 PUSH BP
0000:6dce  1e                 PUSH DS
0000:6dcf  06                 PUSH ES
0000:6dd0  8c d8              MOV AX,DS
0000:6dd2  8e c0              MOV AX,ES
0000:6dd4  8a 44 01           MOV AL,byte ptr [SI + 0x1]
0000:6dd7  8a 64 02           MOV AH,byte ptr [SI + 0x2]
0000:6dda  33 db              XOR BX,BX
0000:6ddc  8a 1c              MOV BL,byte ptr [SI]
0000:6dde  03 dd              ADD BX,BP
0000:6de0  36 8e 1f           MOV DS,word ptr SS:[BX]
0000:6de3  8b d8              MOV BX,AX
0000:6de5  83 c6 03           ADD SI,0x3
0000:6de8  53                 PUSH BX
0000:6de9  57                 PUSH DI
0000:6dea  56                 PUSH SI
0000:6deb  06                 PUSH ES
0000:6dec  33 c0              XOR AX,AX
0000:6dee  26 8a 04           MOV AL,byte ptr ES:[SI]
0000:6df1  8b f0              MOV SI,AX
0000:6df3  c1 e0 02           SHL AX,0x2
0000:6df6  03 f0              ADD SI,AX
0000:6df8  c1 e6 05           SHL SI,0x5
0000:6dfb  b8 00 a8           MOV AX,0xa800
0000:6dfe  8e c0              MOV AX,ES
0000:6e00  e8 18 00           CALL 0x0000:6e1b
0000:6e03  07                 POP ES
0000:6e04  5e                 POP SI
0000:6e05  46                 INC SI
0000:6e06  83 c7 02           ADD DI,0x2
0000:6e09  fe cb              DEC BL
0000:6e0b  75 dd              JNZ 0x0000:6dea
0000:6e0d  5f                 POP DI
0000:6e0e  5b                 POP BX
0000:6e0f  81 c7 00 05        ADD DI,0x500
0000:6e13  fe cf              DEC BH
0000:6e15  75 d1              JNZ 0x0000:6de8
0000:6e17  07                 POP ES
0000:6e18  1f                 POP DS
0000:6e19  5d                 POP BP
0000:6e1a  cb                 RETF
0000:6e1b  0a c9              OR CL,CL
0000:6e1d  75 03              JNZ 0x0000:6e22
0000:6e1f  e9 c3 00           JMP 0x0000:6ee5
0000:6e22  fa                 CLI
0000:6e23  bd 4d 00           MOV BP,0x4d
0000:6e26  b0 c0              MOV AL,0xc0
0000:6e28  e6 7c              OUT 0x7c,AL
0000:6e2a  33 c0              XOR AX,AX
0000:6e2c  e6 7e              OUT 0x7e,AL
0000:6e2e  e6 7e              OUT 0x7e,AL
0000:6e30  e6 7e              OUT 0x7e,AL
0000:6e32  e6 7e              OUT 0x7e,AL
0000:6e34  b5 10              MOV CH,0x10
0000:6e36  ad                 LODSW SI
0000:6e37  8a f4              MOV DH,AH
0000:6e39  32 d2              XOR DL,DL
0000:6e3b  86 c4              XCHG AH,AL
0000:6e3d  d3 e8              SHR AX,CL
0000:6e3f  86 c4              XCHG AH,AL
0000:6e41  ab                 STOSW ES:DI
0000:6e42  d3 ea              SHR DX,CL
0000:6e44  8a c2              MOV AL,DL
0000:6e46  aa                 STOSB ES:DI
0000:6e47  03 fd              ADD DI,BP
0000:6e49  fe cd              DEC CH
0000:6e4b  75 e9              JNZ 0x0000:6e36
0000:6e4d  81 ef 00 05        SUB DI,0x500
0000:6e51  b0 ff              MOV AL,0xff
0000:6e53  e6 7e              OUT 0x7e,AL
0000:6e55  e6 7e              OUT 0x7e,AL
0000:6e57  e6 7e              OUT 0x7e,AL
0000:6e59  e6 7e              OUT 0x7e,AL
0000:6e5b  b0 ce              MOV AL,0xce
0000:6e5d  e6 7c              OUT 0x7c,AL
0000:6e5f  b5 10              MOV CH,0x10
0000:6e61  ad                 LODSW SI
0000:6e62  8a f4              MOV DH,AH
0000:6e64  32 d2              XOR DL,DL
0000:6e66  86 c4              XCHG AH,AL
0000:6e68  d3 e8              SHR AX,CL
0000:6e6a  86 c4              XCHG AH,AL
0000:6e6c  ab                 STOSW ES:DI
0000:6e6d  d3 ea              SHR DX,CL
0000:6e6f  8a c2              MOV AL,DL
0000:6e71  aa                 STOSB ES:DI
0000:6e72  03 fd              ADD DI,BP
0000:6e74  fe cd              DEC CH
0000:6e76  75 e9              JNZ 0x0000:6e61
0000:6e78  81 ef 00 05        SUB DI,0x500
0000:6e7c  b0 cd              MOV AL,0xcd
0000:6e7e  e6 7c              OUT 0x7c,AL
0000:6e80  b5 10              MOV CH,0x10
0000:6e82  ad                 LODSW SI
0000:6e83  8a f4              MOV DH,AH
0000:6e85  32 d2              XOR DL,DL
0000:6e87  86 c4              XCHG AH,AL
0000:6e89  d3 e8              SHR AX,CL
0000:6e8b  86 c4              XCHG AH,AL
0000:6e8d  ab                 STOSW ES:DI
0000:6e8e  d3 ea              SHR DX,CL
0000:6e90  8a c2              MOV AL,DL
0000:6e92  aa                 STOSB ES:DI
0000:6e93  03 fd              ADD DI,BP
0000:6e95  fe cd              DEC CH
0000:6e97  75 e9              JNZ 0x0000:6e82
0000:6e99  81 ef 00 05        SUB DI,0x500
0000:6e9d  b0 cb              MOV AL,0xcb
0000:6e9f  e6 7c              OUT 0x7c,AL
0000:6ea1  b5 10              MOV CH,0x10
0000:6ea3  ad                 LODSW SI
0000:6ea4  8a f4              MOV DH,AH
0000:6ea6  32 d2              XOR DL,DL
0000:6ea8  86 c4              XCHG AH,AL
0000:6eaa  d3 e8              SHR AX,CL
0000:6eac  86 c4              XCHG AH,AL
0000:6eae  ab                 STOSW ES:DI
0000:6eaf  d3 ea              SHR DX,CL
0000:6eb1  8a c2              MOV AL,DL
0000:6eb3  aa                 STOSB ES:DI
0000:6eb4  03 fd              ADD DI,BP
0000:6eb6  fe cd              DEC CH
0000:6eb8  75 e9              JNZ 0x0000:6ea3
0000:6eba  81 ef 00 05        SUB DI,0x500
0000:6ebe  b0 c7              MOV AL,0xc7
0000:6ec0  e6 7c              OUT 0x7c,AL
0000:6ec2  b5 10              MOV CH,0x10
0000:6ec4  ad                 LODSW SI
0000:6ec5  8a f4              MOV DH,AH
0000:6ec7  32 d2              XOR DL,DL
0000:6ec9  86 c4              XCHG AH,AL
0000:6ecb  d3 e8              SHR AX,CL
0000:6ecd  86 c4              XCHG AH,AL
0000:6ecf  ab                 STOSW ES:DI
0000:6ed0  d3 ea              SHR DX,CL
0000:6ed2  8a c2              MOV AL,DL
0000:6ed4  aa                 STOSB ES:DI
0000:6ed5  03 fd              ADD DI,BP
0000:6ed7  fe cd              DEC CH
0000:6ed9  75 e9              JNZ 0x0000:6ec4
0000:6edb  81 ef 00 05        SUB DI,0x500
0000:6edf  33 c0              XOR AX,AX
0000:6ee1  e6 7c              OUT 0x7c,AL
0000:6ee3  fb                 STI
0000:6ee4  c3                 RET
0000:6ee5  fa                 CLI
0000:6ee6  b0 c0              MOV AL,0xc0
0000:6ee8  e6 7c              OUT 0x7c,AL
0000:6eea  33 c0              XOR AX,AX
0000:6eec  e6 7e              OUT 0x7e,AL
0000:6eee  e6 7e              OUT 0x7e,AL
0000:6ef0  e6 7e              OUT 0x7e,AL
0000:6ef2  e6 7e              OUT 0x7e,AL
0000:6ef4  b8 4e 00           MOV AX,0x4e
0000:6ef7  a5                 MOVSW ES:DI,SI
0000:6ef8  03 f8              ADD DI,AX
0000:6efa  a5                 MOVSW ES:DI,SI
0000:6efb  03 f8              ADD DI,AX
0000:6efd  a5                 MOVSW ES:DI,SI
0000:6efe  03 f8              ADD DI,AX
0000:6f00  a5                 MOVSW ES:DI,SI
0000:6f01  03 f8              ADD DI,AX
0000:6f03  a5                 MOVSW ES:DI,SI
0000:6f04  03 f8              ADD DI,AX
0000:6f06  a5                 MOVSW ES:DI,SI
0000:6f07  03 f8              ADD DI,AX
0000:6f09  a5                 MOVSW ES:DI,SI
0000:6f0a  03 f8              ADD DI,AX
0000:6f0c  a5                 MOVSW ES:DI,SI
0000:6f0d  03 f8              ADD DI,AX
0000:6f0f  a5                 MOVSW ES:DI,SI
0000:6f10  03 f8              ADD DI,AX
0000:6f12  a5                 MOVSW ES:DI,SI
0000:6f13  03 f8              ADD DI,AX
0000:6f15  a5                 MOVSW ES:DI,SI
0000:6f16  03 f8              ADD DI,AX
0000:6f18  a5                 MOVSW ES:DI,SI
0000:6f19  03 f8              ADD DI,AX
0000:6f1b  a5                 MOVSW ES:DI,SI
0000:6f1c  03 f8              ADD DI,AX
0000:6f1e  a5                 MOVSW ES:DI,SI
0000:6f1f  03 f8              ADD DI,AX
0000:6f21  a5                 MOVSW ES:DI,SI
0000:6f22  03 f8              ADD DI,AX
0000:6f24  a5                 MOVSW ES:DI,SI
0000:6f25  81 ef b2 04        SUB DI,0x4b2
0000:6f29  b0 ff              MOV AL,0xff
0000:6f2b  e6 7e              OUT 0x7e,AL
0000:6f2d  e6 7e              OUT 0x7e,AL
0000:6f2f  e6 7e              OUT 0x7e,AL
0000:6f31  e6 7e              OUT 0x7e,AL
0000:6f33  b0 ce              MOV AL,0xce
0000:6f35  e6 7c              OUT 0x7c,AL
0000:6f37  b8 4e 00           MOV AX,0x4e
0000:6f3a  a5                 MOVSW ES:DI,SI
0000:6f3b  03 f8              ADD DI,AX
0000:6f3d  a5                 MOVSW ES:DI,SI
0000:6f3e  03 f8              ADD DI,AX
0000:6f40  a5                 MOVSW ES:DI,SI
0000:6f41  03 f8              ADD DI,AX
0000:6f43  a5                 MOVSW ES:DI,SI
0000:6f44  03 f8              ADD DI,AX
0000:6f46  a5                 MOVSW ES:DI,SI
0000:6f47  03 f8              ADD DI,AX
0000:6f49  a5                 MOVSW ES:DI,SI
0000:6f4a  03 f8              ADD DI,AX
0000:6f4c  a5                 MOVSW ES:DI,SI
0000:6f4d  03 f8              ADD DI,AX
0000:6f4f  a5                 MOVSW ES:DI,SI
0000:6f50  03 f8              ADD DI,AX
0000:6f52  a5                 MOVSW ES:DI,SI
0000:6f53  03 f8              ADD DI,AX
0000:6f55  a5                 MOVSW ES:DI,SI
0000:6f56  03 f8              ADD DI,AX
0000:6f58  a5                 MOVSW ES:DI,SI
0000:6f59  03 f8              ADD DI,AX
0000:6f5b  a5                 MOVSW ES:DI,SI
0000:6f5c  03 f8              ADD DI,AX
0000:6f5e  a5                 MOVSW ES:DI,SI
0000:6f5f  03 f8              ADD DI,AX
0000:6f61  a5                 MOVSW ES:DI,SI
0000:6f62  03 f8              ADD DI,AX
0000:6f64  a5                 MOVSW ES:DI,SI
0000:6f65  03 f8              ADD DI,AX
0000:6f67  a5                 MOVSW ES:DI,SI
0000:6f68  81 ef b2 04        SUB DI,0x4b2
0000:6f6c  b0 cd              MOV AL,0xcd
0000:6f6e  e6 7c              OUT 0x7c,AL
0000:6f70  b8 4e 00           MOV AX,0x4e
0000:6f73  a5                 MOVSW ES:DI,SI
0000:6f74  03 f8              ADD DI,AX
0000:6f76  a5                 MOVSW ES:DI,SI
0000:6f77  03 f8              ADD DI,AX
0000:6f79  a5                 MOVSW ES:DI,SI
0000:6f7a  03 f8              ADD DI,AX
0000:6f7c  a5                 MOVSW ES:DI,SI
0000:6f7d  03 f8              ADD DI,AX
0000:6f7f  a5                 MOVSW ES:DI,SI
0000:6f80  03 f8              ADD DI,AX
0000:6f82  a5                 MOVSW ES:DI,SI
0000:6f83  03 f8              ADD DI,AX
0000:6f85  a5                 MOVSW ES:DI,SI
0000:6f86  03 f8              ADD DI,AX
0000:6f88  a5                 MOVSW ES:DI,SI
0000:6f89  03 f8              ADD DI,AX
0000:6f8b  a5                 MOVSW ES:DI,SI
0000:6f8c  03 f8              ADD DI,AX
0000:6f8e  a5                 MOVSW ES:DI,SI
0000:6f8f  03 f8              ADD DI,AX
0000:6f91  a5                 MOVSW ES:DI,SI
0000:6f92  03 f8              ADD DI,AX
0000:6f94  a5                 MOVSW ES:DI,SI
0000:6f95  03 f8              ADD DI,AX
0000:6f97  a5                 MOVSW ES:DI,SI
0000:6f98  03 f8              ADD DI,AX
0000:6f9a  a5                 MOVSW ES:DI,SI
0000:6f9b  03 f8              ADD DI,AX
0000:6f9d  a5                 MOVSW ES:DI,SI
0000:6f9e  03 f8              ADD DI,AX
0000:6fa0  a5                 MOVSW ES:DI,SI
0000:6fa1  81 ef b2 04        SUB DI,0x4b2
0000:6fa5  b0 cb              MOV AL,0xcb
0000:6fa7  e6 7c              OUT 0x7c,AL
0000:6fa9  b8 4e 00           MOV AX,0x4e
0000:6fac  a5                 MOVSW ES:DI,SI
0000:6fad  03 f8              ADD DI,AX
0000:6faf  a5                 MOVSW ES:DI,SI
0000:6fb0  03 f8              ADD DI,AX
0000:6fb2  a5                 MOVSW ES:DI,SI
0000:6fb3  03 f8              ADD DI,AX
0000:6fb5  a5                 MOVSW ES:DI,SI
0000:6fb6  03 f8              ADD DI,AX
0000:6fb8  a5                 MOVSW ES:DI,SI
0000:6fb9  03 f8              ADD DI,AX
0000:6fbb  a5                 MOVSW ES:DI,SI
0000:6fbc  03 f8              ADD DI,AX
0000:6fbe  a5                 MOVSW ES:DI,SI
0000:6fbf  03 f8              ADD DI,AX
0000:6fc1  a5                 MOVSW ES:DI,SI
0000:6fc2  03 f8              ADD DI,AX
0000:6fc4  a5                 MOVSW ES:DI,SI
0000:6fc5  03 f8              ADD DI,AX
0000:6fc7  a5                 MOVSW ES:DI,SI
0000:6fc8  03 f8              ADD DI,AX
0000:6fca  a5                 MOVSW ES:DI,SI
0000:6fcb  03 f8              ADD DI,AX
0000:6fcd  a5                 MOVSW ES:DI,SI
0000:6fce  03 f8              ADD DI,AX
0000:6fd0  a5                 MOVSW ES:DI,SI
0000:6fd1  03 f8              ADD DI,AX
0000:6fd3  a5                 MOVSW ES:DI,SI
0000:6fd4  03 f8              ADD DI,AX
0000:6fd6  a5                 MOVSW ES:DI,SI
0000:6fd7  03 f8              ADD DI,AX
0000:6fd9  a5                 MOVSW ES:DI,SI
0000:6fda  81 ef b2 04        SUB DI,0x4b2
0000:6fde  b0 c7              MOV AL,0xc7
0000:6fe0  e6 7c              OUT 0x7c,AL
0000:6fe2  b8 4e 00           MOV AX,0x4e
0000:6fe5  a5                 MOVSW ES:DI,SI
0000:6fe6  03 f8              ADD DI,AX
0000:6fe8  a5                 MOVSW ES:DI,SI
0000:6fe9  03 f8              ADD DI,AX
0000:6feb  a5                 MOVSW ES:DI,SI
0000:6fec  03 f8              ADD DI,AX
0000:6fee  a5                 MOVSW ES:DI,SI
0000:6fef  03 f8              ADD DI,AX
0000:6ff1  a5                 MOVSW ES:DI,SI
0000:6ff2  03 f8              ADD DI,AX
0000:6ff4  a5                 MOVSW ES:DI,SI
0000:6ff5  03 f8              ADD DI,AX
0000:6ff7  a5                 MOVSW ES:DI,SI
0000:6ff8  03 f8              ADD DI,AX
0000:6ffa  a5                 MOVSW ES:DI,SI
0000:6ffb  03 f8              ADD DI,AX
0000:6ffd  a5                 MOVSW ES:DI,SI
0000:6ffe  03 f8              ADD DI,AX
0000:7000  a5                 MOVSW ES:DI,SI
0000:7001  03 f8              ADD DI,AX
0000:7003  a5                 MOVSW ES:DI,SI
0000:7004  03 f8              ADD DI,AX
0000:7006  a5                 MOVSW ES:DI,SI
0000:7007  03 f8              ADD DI,AX
0000:7009  a5                 MOVSW ES:DI,SI
0000:700a  03 f8              ADD DI,AX
0000:700c  a5                 MOVSW ES:DI,SI
0000:700d  03 f8              ADD DI,AX
0000:700f  a5                 MOVSW ES:DI,SI
0000:7010  03 f8              ADD DI,AX
0000:7012  a5                 MOVSW ES:DI,SI
0000:7013  81 ef b2 04        SUB DI,0x4b2
0000:7017  33 c0              XOR AX,AX
0000:7019  e6 7c              OUT 0x7c,AL
0000:701b  fb                 STI
0000:701c  c3                 RET
0000:725b  83 c6 20           ADD SI,0x20
0000:725e  b8 00 a8           MOV AX,0xa800
0000:7261  8e c0              MOV AX,ES
0000:7263  b8 4e 00           MOV AX,0x4e
0000:7266  a5                 MOVSW ES:DI,SI
0000:7267  03 f8              ADD DI,AX
0000:7269  a5                 MOVSW ES:DI,SI
0000:726a  03 f8              ADD DI,AX
0000:726c  a5                 MOVSW ES:DI,SI
0000:726d  03 f8              ADD DI,AX
0000:726f  a5                 MOVSW ES:DI,SI
0000:7270  03 f8              ADD DI,AX
0000:7272  a5                 MOVSW ES:DI,SI
0000:7273  03 f8              ADD DI,AX
0000:7275  a5                 MOVSW ES:DI,SI
0000:7276  03 f8              ADD DI,AX
0000:7278  a5                 MOVSW ES:DI,SI
0000:7279  03 f8              ADD DI,AX
0000:727b  a5                 MOVSW ES:DI,SI
0000:727c  03 f8              ADD DI,AX
0000:727e  a5                 MOVSW ES:DI,SI
0000:727f  03 f8              ADD DI,AX
0000:7281  a5                 MOVSW ES:DI,SI
0000:7282  03 f8              ADD DI,AX
0000:7284  a5                 MOVSW ES:DI,SI
0000:7285  03 f8              ADD DI,AX
0000:7287  a5                 MOVSW ES:DI,SI
0000:7288  03 f8              ADD DI,AX
0000:728a  a5                 MOVSW ES:DI,SI
0000:728b  03 f8              ADD DI,AX
0000:728d  a5                 MOVSW ES:DI,SI
0000:728e  03 f8              ADD DI,AX
0000:7290  a5                 MOVSW ES:DI,SI
0000:7291  03 f8              ADD DI,AX
0000:7293  a5                 MOVSW ES:DI,SI
0000:7294  81 ef b2 04        SUB DI,0x4b2
0000:7298  b8 00 b0           MOV AX,0xb000
0000:729b  8e c0              MOV AX,ES
0000:729d  b8 4e 00           MOV AX,0x4e
0000:72a0  a5                 MOVSW ES:DI,SI
0000:72a1  03 f8              ADD DI,AX
0000:72a3  a5                 MOVSW ES:DI,SI
0000:72a4  03 f8              ADD DI,AX
0000:72a6  a5                 MOVSW ES:DI,SI
0000:72a7  03 f8              ADD DI,AX
0000:72a9  a5                 MOVSW ES:DI,SI
0000:72aa  03 f8              ADD DI,AX
0000:72ac  a5                 MOVSW ES:DI,SI
0000:72ad  03 f8              ADD DI,AX
0000:72af  a5                 MOVSW ES:DI,SI
0000:72b0  03 f8              ADD DI,AX
0000:72b2  a5                 MOVSW ES:DI,SI
0000:72b3  03 f8              ADD DI,AX
0000:72b5  a5                 MOVSW ES:DI,SI
0000:72b6  03 f8              ADD DI,AX
0000:72b8  a5                 MOVSW ES:DI,SI
0000:72b9  03 f8              ADD DI,AX
0000:72bb  a5                 MOVSW ES:DI,SI
0000:72bc  03 f8              ADD DI,AX
0000:72be  a5                 MOVSW ES:DI,SI
0000:72bf  03 f8              ADD DI,AX
0000:72c1  a5                 MOVSW ES:DI,SI
0000:72c2  03 f8              ADD DI,AX
0000:72c4  a5                 MOVSW ES:DI,SI
0000:72c5  03 f8              ADD DI,AX
0000:72c7  a5                 MOVSW ES:DI,SI
0000:72c8  03 f8              ADD DI,AX
0000:72ca  a5                 MOVSW ES:DI,SI
0000:72cb  03 f8              ADD DI,AX
0000:72cd  a5                 MOVSW ES:DI,SI
0000:72ce  81 c7 4e 7b        ADD DI,0x7b4e
0000:72d2  b8 4e 00           MOV AX,0x4e
0000:72d5  a5                 MOVSW ES:DI,SI
0000:72d6  03 f8              ADD DI,AX
0000:72d8  a5                 MOVSW ES:DI,SI
0000:72d9  03 f8              ADD DI,AX
0000:72db  a5                 MOVSW ES:DI,SI
0000:72dc  03 f8              ADD DI,AX
0000:72de  a5                 MOVSW ES:DI,SI
0000:72df  03 f8              ADD DI,AX
0000:72e1  a5                 MOVSW ES:DI,SI
0000:72e2  03 f8              ADD DI,AX
0000:72e4  a5                 MOVSW ES:DI,SI
0000:72e5  03 f8              ADD DI,AX
0000:72e7  a5                 MOVSW ES:DI,SI
0000:72e8  03 f8              ADD DI,AX
0000:72ea  a5                 MOVSW ES:DI,SI
0000:72eb  03 f8              ADD DI,AX
0000:72ed  a5                 MOVSW ES:DI,SI
0000:72ee  03 f8              ADD DI,AX
0000:72f0  a5                 MOVSW ES:DI,SI
0000:72f1  03 f8              ADD DI,AX
0000:72f3  a5                 MOVSW ES:DI,SI
0000:72f4  03 f8              ADD DI,AX
0000:72f6  a5                 MOVSW ES:DI,SI
0000:72f7  03 f8              ADD DI,AX
0000:72f9  a5                 MOVSW ES:DI,SI
0000:72fa  03 f8              ADD DI,AX
0000:72fc  a5                 MOVSW ES:DI,SI
0000:72fd  03 f8              ADD DI,AX
0000:72ff  a5                 MOVSW ES:DI,SI
0000:7300  03 f8              ADD DI,AX
0000:7302  a5                 MOVSW ES:DI,SI
0000:7303  81 ef b2 84        SUB DI,0x84b2
0000:7307  b8 00 e0           MOV AX,0xe000
0000:730a  8e c0              MOV AX,ES
0000:730c  b8 4e 00           MOV AX,0x4e
0000:730f  a5                 MOVSW ES:DI,SI
0000:7310  03 f8              ADD DI,AX
0000:7312  a5                 MOVSW ES:DI,SI
0000:7313  03 f8              ADD DI,AX
0000:7315  a5                 MOVSW ES:DI,SI
0000:7316  03 f8              ADD DI,AX
0000:7318  a5                 MOVSW ES:DI,SI
0000:7319  03 f8              ADD DI,AX
0000:731b  a5                 MOVSW ES:DI,SI
0000:731c  03 f8              ADD DI,AX
0000:731e  a5                 MOVSW ES:DI,SI
0000:731f  03 f8              ADD DI,AX
0000:7321  a5                 MOVSW ES:DI,SI
0000:7322  03 f8              ADD DI,AX
0000:7324  a5                 MOVSW ES:DI,SI
0000:7325  03 f8              ADD DI,AX
0000:7327  a5                 MOVSW ES:DI,SI
0000:7328  03 f8              ADD DI,AX
0000:732a  a5                 MOVSW ES:DI,SI
0000:732b  03 f8              ADD DI,AX
0000:732d  a5                 MOVSW ES:DI,SI
0000:732e  03 f8              ADD DI,AX
0000:7330  a5                 MOVSW ES:DI,SI
0000:7331  03 f8              ADD DI,AX
0000:7333  a5                 MOVSW ES:DI,SI
0000:7334  03 f8              ADD DI,AX
0000:7336  a5                 MOVSW ES:DI,SI
0000:7337  03 f8              ADD DI,AX
0000:7339  a5                 MOVSW ES:DI,SI
0000:733a  03 f8              ADD DI,AX
0000:733c  a5                 MOVSW ES:DI,SI
0000:733d  81 ef b2 04        SUB DI,0x4b2
0000:7341  c3                 RET
0000:7342  b8 00 a8           MOV AX,0xa800
0000:7345  8e c0              MOV AX,ES
0000:7347  b8 4e 00           MOV AX,0x4e
0000:734a  a5                 MOVSW ES:DI,SI
0000:734b  03 f8              ADD DI,AX
0000:734d  a5                 MOVSW ES:DI,SI
0000:734e  03 f8              ADD DI,AX
0000:7350  a5                 MOVSW ES:DI,SI
0000:7351  03 f8              ADD DI,AX
0000:7353  a5                 MOVSW ES:DI,SI
0000:7354  03 f8              ADD DI,AX
0000:7356  a5                 MOVSW ES:DI,SI
0000:7357  03 f8              ADD DI,AX
0000:7359  a5                 MOVSW ES:DI,SI
0000:735a  03 f8              ADD DI,AX
0000:735c  a5                 MOVSW ES:DI,SI
0000:735d  03 f8              ADD DI,AX
0000:735f  a5                 MOVSW ES:DI,SI
0000:7360  03 f8              ADD DI,AX
0000:7362  a5                 MOVSW ES:DI,SI
0000:7363  03 f8              ADD DI,AX
0000:7365  a5                 MOVSW ES:DI,SI
0000:7366  03 f8              ADD DI,AX
0000:7368  a5                 MOVSW ES:DI,SI
0000:7369  03 f8              ADD DI,AX
0000:736b  a5                 MOVSW ES:DI,SI
0000:736c  03 f8              ADD DI,AX
0000:736e  a5                 MOVSW ES:DI,SI
0000:736f  03 f8              ADD DI,AX
0000:7371  a5                 MOVSW ES:DI,SI
0000:7372  03 f8              ADD DI,AX
0000:7374  a5                 MOVSW ES:DI,SI
0000:7375  03 f8              ADD DI,AX
0000:7377  a5                 MOVSW ES:DI,SI
0000:7378  81 ef b2 04        SUB DI,0x4b2
0000:737c  b8 00 b0           MOV AX,0xb000
0000:737f  8e c0              MOV AX,ES
0000:7381  b8 4e 00           MOV AX,0x4e
0000:7384  a5                 MOVSW ES:DI,SI
0000:7385  03 f8              ADD DI,AX
0000:7387  a5                 MOVSW ES:DI,SI
0000:7388  03 f8              ADD DI,AX
0000:738a  a5                 MOVSW ES:DI,SI
0000:738b  03 f8              ADD DI,AX
0000:738d  a5                 MOVSW ES:DI,SI
0000:738e  03 f8              ADD DI,AX
0000:7390  a5                 MOVSW ES:DI,SI
0000:7391  03 f8              ADD DI,AX
0000:7393  a5                 MOVSW ES:DI,SI
0000:7394  03 f8              ADD DI,AX
0000:7396  a5                 MOVSW ES:DI,SI
0000:7397  03 f8              ADD DI,AX
0000:7399  a5                 MOVSW ES:DI,SI
0000:739a  03 f8              ADD DI,AX
0000:739c  a5                 MOVSW ES:DI,SI
0000:739d  03 f8              ADD DI,AX
0000:739f  a5                 MOVSW ES:DI,SI
0000:73a0  03 f8              ADD DI,AX
0000:73a2  a5                 MOVSW ES:DI,SI
0000:73a3  03 f8              ADD DI,AX
0000:73a5  a5                 MOVSW ES:DI,SI
0000:73a6  03 f8              ADD DI,AX
0000:73a8  a5                 MOVSW ES:DI,SI
0000:73a9  03 f8              ADD DI,AX
0000:73ab  a5                 MOVSW ES:DI,SI
0000:73ac  03 f8              ADD DI,AX
0000:73ae  a5                 MOVSW ES:DI,SI
0000:73af  03 f8              ADD DI,AX
0000:73b1  a5                 MOVSW ES:DI,SI
0000:73b2  81 c7 4e 7b        ADD DI,0x7b4e
0000:73b6  b8 4e 00           MOV AX,0x4e
0000:73b9  a5                 MOVSW ES:DI,SI
0000:73ba  03 f8              ADD DI,AX
0000:73bc  a5                 MOVSW ES:DI,SI
0000:73bd  03 f8              ADD DI,AX
0000:73bf  a5                 MOVSW ES:DI,SI
0000:73c0  03 f8              ADD DI,AX
0000:73c2  a5                 MOVSW ES:DI,SI
0000:73c3  03 f8              ADD DI,AX
0000:73c5  a5                 MOVSW ES:DI,SI
0000:73c6  03 f8              ADD DI,AX
0000:73c8  a5                 MOVSW ES:DI,SI
0000:73c9  03 f8              ADD DI,AX
0000:73cb  a5                 MOVSW ES:DI,SI
0000:73cc  03 f8              ADD DI,AX
0000:73ce  a5                 MOVSW ES:DI,SI
0000:73cf  03 f8              ADD DI,AX
0000:73d1  a5                 MOVSW ES:DI,SI
0000:73d2  03 f8              ADD DI,AX
0000:73d4  a5                 MOVSW ES:DI,SI
0000:73d5  03 f8              ADD DI,AX
0000:73d7  a5                 MOVSW ES:DI,SI
0000:73d8  03 f8              ADD DI,AX
0000:73da  a5                 MOVSW ES:DI,SI
0000:73db  03 f8              ADD DI,AX
0000:73dd  a5                 MOVSW ES:DI,SI
0000:73de  03 f8              ADD DI,AX
0000:73e0  a5                 MOVSW ES:DI,SI
0000:73e1  03 f8              ADD DI,AX
0000:73e3  a5                 MOVSW ES:DI,SI
0000:73e4  03 f8              ADD DI,AX
0000:73e6  a5                 MOVSW ES:DI,SI
0000:73e7  81 ef b2 84        SUB DI,0x84b2
0000:73eb  b8 00 e0           MOV AX,0xe000
0000:73ee  8e c0              MOV AX,ES
0000:73f0  b8 4e 00           MOV AX,0x4e
0000:73f3  a5                 MOVSW ES:DI,SI
0000:73f4  03 f8              ADD DI,AX
0000:73f6  a5                 MOVSW ES:DI,SI
0000:73f7  03 f8              ADD DI,AX
0000:73f9  a5                 MOVSW ES:DI,SI
0000:73fa  03 f8              ADD DI,AX
0000:73fc  a5                 MOVSW ES:DI,SI
0000:73fd  03 f8              ADD DI,AX
0000:73ff  a5                 MOVSW ES:DI,SI
0000:7400  03 f8              ADD DI,AX
0000:7402  a5                 MOVSW ES:DI,SI
0000:7403  03 f8              ADD DI,AX
0000:7405  a5                 MOVSW ES:DI,SI
0000:7406  03 f8              ADD DI,AX
0000:7408  a5                 MOVSW ES:DI,SI
0000:7409  03 f8              ADD DI,AX
0000:740b  a5                 MOVSW ES:DI,SI
0000:740c  03 f8              ADD DI,AX
0000:740e  a5                 MOVSW ES:DI,SI
0000:740f  03 f8              ADD DI,AX
0000:7411  a5                 MOVSW ES:DI,SI
0000:7412  03 f8              ADD DI,AX
0000:7414  a5                 MOVSW ES:DI,SI
0000:7415  03 f8              ADD DI,AX
0000:7417  a5                 MOVSW ES:DI,SI
0000:7418  03 f8              ADD DI,AX
0000:741a  a5                 MOVSW ES:DI,SI
0000:741b  03 f8              ADD DI,AX
0000:741d  a5                 MOVSW ES:DI,SI
0000:741e  03 f8              ADD DI,AX
0000:7420  a5                 MOVSW ES:DI,SI
0000:7421  81 ef b2 04        SUB DI,0x4b2
0000:7425  c3                 RET
0000:7428  55                 PUSH BP
0000:7429  06                 PUSH ES
0000:742a  0a c0              OR AL,AL
0000:742c  75 0a              JNZ 0x0000:7438
0000:742e  f6 c7 10           TEST BH,0x10
0000:7431  74 0f              JZ 0x0000:7442
0000:7433  f6 c3 10           TEST BL,0x10
0000:7436  74 0a              JZ 0x0000:7442
0000:7438  bd 00 a8           MOV BP,0xa800
0000:743b  8e c5              MOV BP,ES
0000:743d  e8 01 01           CALL 0x0000:7541
0000:7440  eb 03              JMP 0x0000:7445
0000:7442  e8 03 00           CALL 0x0000:7448
0000:7445  07                 POP ES
0000:7446  5d                 POP BP
0000:7447  cb                 RETF
0000:7448  2e 88 1e 26 74     MOV byte ptr CS:[0x7426],BL
0000:744d  2e 88 3e 27 74     MOV byte ptr CS:[0x7427],BH
0000:7452  8a d9              MOV BL,CL
0000:7454  b7 50              MOV BH,0x50
0000:7456  2a f9              SUB BH,CL
0000:7458  32 ed              XOR CH,CH
0000:745a  8b ea              MOV BP,DX
0000:745c  2e f6 06 27 74 10  TEST byte ptr CS:[0x7427],0x10
0000:7462  74 08              JZ 0x0000:746c
0000:7464  8b c5              MOV AX,BP
0000:7466  f7 e1              MUL CX
0000:7468  03 f0              ADD SI,AX
0000:746a  8b d5              MOV DX,BP
0000:746c  2e f6 06 27 74 01  TEST byte ptr CS:[0x7427],0x1
0000:7472  74 2d              JZ 0x0000:74a1
0000:7474  2e f6 06 26 74 01  TEST byte ptr CS:[0x7426],0x1
0000:747a  75 0a              JNZ 0x0000:7486
0000:747c  8b c5              MOV AX,BP
0000:747e  f7 e1              MUL CX
0000:7480  03 f0              ADD SI,AX
0000:7482  8b d5              MOV DX,BP
0000:7484  eb 1b              JMP 0x0000:74a1
0000:7486  b8 00 a8           MOV AX,0xa800
0000:7489  8e c0              MOV AX,ES
0000:748b  8b c7              MOV AX,DI
0000:748d  d0 e9              SHR CL,0x1
0000:748f  73 01              JNC 0x0000:7492
0000:7491  a4                 MOVSB ES:DI,SI
0000:7492  f3 a5              MOVSW.REP ES:DI,SI
0000:7494  8a cf              MOV CL,BH
0000:7496  03 f9              ADD DI,CX
0000:7498  8a cb              MOV CL,BL
0000:749a  4a                 DEC DX
0000:749b  75 f0              JNZ 0x0000:748d
0000:749d  8b d5              MOV DX,BP
0000:749f  8b f8              MOV DI,AX
0000:74a1  2e f6 06 27 74 02  TEST byte ptr CS:[0x7427],0x2
0000:74a7  74 2d              JZ 0x0000:74d6
0000:74a9  2e f6 06 26 74 02  TEST byte ptr CS:[0x7426],0x2
0000:74af  75 0a              JNZ 0x0000:74bb
0000:74b1  8b c5              MOV AX,BP
0000:74b3  f7 e1              MUL CX
0000:74b5  03 f0              ADD SI,AX
0000:74b7  8b d5              MOV DX,BP
0000:74b9  eb 1b              JMP 0x0000:74d6
0000:74bb  b8 00 b0           MOV AX,0xb000
0000:74be  8e c0              MOV AX,ES
0000:74c0  8b c7              MOV AX,DI
0000:74c2  d0 e9              SHR CL,0x1
0000:74c4  73 01              JNC 0x0000:74c7
0000:74c6  a4                 MOVSB ES:DI,SI
0000:74c7  f3 a5              MOVSW.REP ES:DI,SI
0000:74c9  8a cf              MOV CL,BH
0000:74cb  03 f9              ADD DI,CX
0000:74cd  8a cb              MOV CL,BL
0000:74cf  4a                 DEC DX
0000:74d0  75 f0              JNZ 0x0000:74c2
0000:74d2  8b d5              MOV DX,BP
0000:74d4  8b f8              MOV DI,AX
0000:74d6  2e f6 06 27 74 04  TEST byte ptr CS:[0x7427],0x4
0000:74dc  74 2d              JZ 0x0000:750b
0000:74de  2e f6 06 26 74 04  TEST byte ptr CS:[0x7426],0x4
0000:74e4  75 0a              JNZ 0x0000:74f0
0000:74e6  8b c5              MOV AX,BP
0000:74e8  f7 e1              MUL CX
0000:74ea  03 f0              ADD SI,AX
0000:74ec  8b d5              MOV DX,BP
0000:74ee  eb 1b              JMP 0x0000:750b
0000:74f0  b8 00 b8           MOV AX,0xb800
0000:74f3  8e c0              MOV AX,ES
0000:74f5  8b c7              MOV AX,DI
0000:74f7  d0 e9              SHR CL,0x1
0000:74f9  73 01              JNC 0x0000:74fc
0000:74fb  a4                 MOVSB ES:DI,SI
0000:74fc  f3 a5              MOVSW.REP ES:DI,SI
0000:74fe  8a cf              MOV CL,BH
0000:7500  03 f9              ADD DI,CX
0000:7502  8a cb              MOV CL,BL
0000:7504  4a                 DEC DX
0000:7505  75 f0              JNZ 0x0000:74f7
0000:7507  8b d5              MOV DX,BP
0000:7509  8b f8              MOV DI,AX
0000:750b  2e f6 06 27 74 08  TEST byte ptr CS:[0x7427],0x8
0000:7511  74 2d              JZ 0x0000:7540
0000:7513  2e f6 06 26 74 08  TEST byte ptr CS:[0x7426],0x8
0000:7519  75 0a              JNZ 0x0000:7525
0000:751b  8b c5              MOV AX,BP
0000:751d  f7 e1              MUL CX
0000:751f  03 f0              ADD SI,AX
0000:7521  8b d5              MOV DX,BP
0000:7523  eb 1b              JMP 0x0000:7540
0000:7525  b8 00 e0           MOV AX,0xe000
0000:7528  8e c0              MOV AX,ES
0000:752a  8b c7              MOV AX,DI
0000:752c  d0 e9              SHR CL,0x1
0000:752e  73 01              JNC 0x0000:7531
0000:7530  a4                 MOVSB ES:DI,SI
0000:7531  f3 a5              MOVSW.REP ES:DI,SI
0000:7533  8a cf              MOV CL,BH
0000:7535  03 f9              ADD DI,CX
0000:7537  8a cb              MOV CL,BL
0000:7539  4a                 DEC DX
0000:753a  75 f0              JNZ 0x0000:752c
0000:753c  8b d5              MOV DX,BP
0000:753e  8b f8              MOV DI,AX
0000:7540  c3                 RET
0000:7541  0a c0              OR AL,AL
0000:7543  74 03              JZ 0x0000:7548
0000:7545  e9 31 01           JMP 0x0000:7679
0000:7548  2e 88 3e 27 74     MOV byte ptr CS:[0x7427],BH
0000:754d  b7 50              MOV BH,0x50
0000:754f  2a f9              SUB BH,CL
0000:7551  32 ed              XOR CH,CH
0000:7553  8b ea              MOV BP,DX
0000:7555  b0 c0              MOV AL,0xc0
0000:7557  e6 7c              OUT 0x7c,AL
0000:7559  33 c0              XOR AX,AX
0000:755b  e6 7e              OUT 0x7e,AL
0000:755d  e6 7e              OUT 0x7e,AL
0000:755f  e6 7e              OUT 0x7e,AL
0000:7561  e6 7e              OUT 0x7e,AL
0000:7563  f6 c3 10           TEST BL,0x10
0000:7566  75 28              JNZ 0x0000:7590
0000:7568  2e f6 06 27 74 10  TEST byte ptr CS:[0x7427],0x10
0000:756e  74 06              JZ 0x0000:7576
0000:7570  8b c5              MOV AX,BP
0000:7572  f7 e1              MUL CX
0000:7574  03 f0              ADD SI,AX
0000:7576  57                 PUSH DI
0000:7577  55                 PUSH BP
0000:7578  8a d1              MOV DL,CL
0000:757a  d0 e9              SHR CL,0x1
0000:757c  73 01              JNC 0x0000:757f
0000:757e  aa                 STOSB ES:DI
0000:757f  f3 ab              STOSW.REP ES:DI
0000:7581  8a cf              MOV CL,BH
0000:7583  03 f9              ADD DI,CX
0000:7585  8a ca              MOV CL,DL
0000:7587  4d                 DEC BP
0000:7588  75 f0              JNZ 0x0000:757a
0000:758a  5d                 POP BP
0000:758b  8b d5              MOV DX,BP
0000:758d  5f                 POP DI
0000:758e  eb 16              JMP 0x0000:75a6
0000:7590  8a c1              MOV AL,CL
0000:7592  57                 PUSH DI
0000:7593  d0 e9              SHR CL,0x1
0000:7595  73 01              JNC 0x0000:7598
0000:7597  a4                 MOVSB ES:DI,SI
0000:7598  f3 a5              MOVSW.REP ES:DI,SI
0000:759a  8a cf              MOV CL,BH
0000:759c  03 f9              ADD DI,CX
0000:759e  8a c8              MOV CL,AL
0000:75a0  4a                 DEC DX
0000:75a1  75 f0              JNZ 0x0000:7593
0000:75a3  8b d5              MOV DX,BP
0000:75a5  5f                 POP DI
0000:75a6  b0 ff              MOV AL,0xff
0000:75a8  e6 7e              OUT 0x7e,AL
0000:75aa  e6 7e              OUT 0x7e,AL
0000:75ac  e6 7e              OUT 0x7e,AL
0000:75ae  e6 7e              OUT 0x7e,AL
0000:75b0  f6 c3 01           TEST BL,0x1
0000:75b3  75 12              JNZ 0x0000:75c7
0000:75b5  2e f6 06 27 74 01  TEST byte ptr CS:[0x7427],0x1
0000:75bb  74 24              JZ 0x0000:75e1
0000:75bd  8b c5              MOV AX,BP
0000:75bf  f7 e1              MUL CX
0000:75c1  03 f0              ADD SI,AX
0000:75c3  8b d5              MOV DX,BP
0000:75c5  eb 1a              JMP 0x0000:75e1
0000:75c7  b0 ce              MOV AL,0xce
0000:75c9  e6 7c              OUT 0x7c,AL
0000:75cb  8a c1              MOV AL,CL
0000:75cd  57                 PUSH DI
0000:75ce  d0 e9              SHR CL,0x1
0000:75d0  73 01              JNC 0x0000:75d3
0000:75d2  a4                 MOVSB ES:DI,SI
0000:75d3  f3 a5              MOVSW.REP ES:DI,SI
0000:75d5  8a cf              MOV CL,BH
0000:75d7  03 f9              ADD DI,CX
0000:75d9  8a c8              MOV CL,AL
0000:75db  4a                 DEC DX
0000:75dc  75 f0              JNZ 0x0000:75ce
0000:75de  8b d5              MOV DX,BP
0000:75e0  5f                 POP DI
0000:75e1  f6 c3 02           TEST BL,0x2
0000:75e4  75 12              JNZ 0x0000:75f8
0000:75e6  2e f6 06 27 74 02  TEST byte ptr CS:[0x7427],0x2
0000:75ec  74 24              JZ 0x0000:7612
0000:75ee  8b c5              MOV AX,BP
0000:75f0  f7 e1              MUL CX
0000:75f2  03 f0              ADD SI,AX
0000:75f4  8b d5              MOV DX,BP
0000:75f6  eb 1a              JMP 0x0000:7612
0000:75f8  b0 cd              MOV AL,0xcd
0000:75fa  e6 7c              OUT 0x7c,AL
0000:75fc  8a c1              MOV AL,CL
0000:75fe  57                 PUSH DI
0000:75ff  d0 e9              SHR CL,0x1
0000:7601  73 01              JNC 0x0000:7604
0000:7603  a4                 MOVSB ES:DI,SI
0000:7604  f3 a5              MOVSW.REP ES:DI,SI
0000:7606  8a cf              MOV CL,BH
0000:7608  03 f9              ADD DI,CX
0000:760a  8a c8              MOV CL,AL
0000:760c  4a                 DEC DX
0000:760d  75 f0              JNZ 0x0000:75ff
0000:760f  8b d5              MOV DX,BP
0000:7611  5f                 POP DI
0000:7612  f6 c3 04           TEST BL,0x4
0000:7615  75 12              JNZ 0x0000:7629
0000:7617  2e f6 06 27 74 04  TEST byte ptr CS:[0x7427],0x4
0000:761d  74 24              JZ 0x0000:7643
0000:761f  8b c5              MOV AX,BP
0000:7621  f7 e1              MUL CX
0000:7623  03 f0              ADD SI,AX
0000:7625  8b d5              MOV DX,BP
0000:7627  eb 1a              JMP 0x0000:7643
0000:7629  b0 cb              MOV AL,0xcb
0000:762b  e6 7c              OUT 0x7c,AL
0000:762d  8a c1              MOV AL,CL
0000:762f  57                 PUSH DI
0000:7630  d0 e9              SHR CL,0x1
0000:7632  73 01              JNC 0x0000:7635
0000:7634  a4                 MOVSB ES:DI,SI
0000:7635  f3 a5              MOVSW.REP ES:DI,SI
0000:7637  8a cf              MOV CL,BH
0000:7639  03 f9              ADD DI,CX
0000:763b  8a c8              MOV CL,AL
0000:763d  4a                 DEC DX
0000:763e  75 f0              JNZ 0x0000:7630
0000:7640  8b d5              MOV DX,BP
0000:7642  5f                 POP DI
0000:7643  f6 c3 08           TEST BL,0x8
0000:7646  75 12              JNZ 0x0000:765a
0000:7648  2e f6 06 27 74 08  TEST byte ptr CS:[0x7427],0x8
0000:764e  74 24              JZ 0x0000:7674
0000:7650  8b c5              MOV AX,BP
0000:7652  f7 e1              MUL CX
0000:7654  03 f0              ADD SI,AX
0000:7656  8b d5              MOV DX,BP
0000:7658  eb 1a              JMP 0x0000:7674
0000:765a  b0 c7              MOV AL,0xc7
0000:765c  e6 7c              OUT 0x7c,AL
0000:765e  8a c1              MOV AL,CL
0000:7660  57                 PUSH DI
0000:7661  d0 e9              SHR CL,0x1
0000:7663  73 01              JNC 0x0000:7666
0000:7665  a4                 MOVSB ES:DI,SI
0000:7666  f3 a5              MOVSW.REP ES:DI,SI
0000:7668  8a cf              MOV CL,BH
0000:766a  03 f9              ADD DI,CX
0000:766c  8a c8              MOV CL,AL
0000:766e  4a                 DEC DX
0000:766f  75 f0              JNZ 0x0000:7661
0000:7671  8b d5              MOV DX,BP
0000:7673  5f                 POP DI
0000:7674  33 c0              XOR AX,AX
0000:7676  e6 7c              OUT 0x7c,AL
0000:7678  c3                 RET
0000:7679  2e 88 1e 26 74     MOV byte ptr CS:[0x7426],BL
0000:767e  2e 88 3e 27 74     MOV byte ptr CS:[0x7427],BH
0000:7683  8b ea              MOV BP,DX
0000:7685  b2 4f              MOV DL,0x4f
0000:7687  2a d1              SUB DL,CL
0000:7689  8a f1              MOV DH,CL
0000:768b  8a e9              MOV CH,CL
0000:768d  8a c8              MOV CL,AL
0000:768f  b0 c0              MOV AL,0xc0
0000:7691  e6 7c              OUT 0x7c,AL
0000:7693  33 c0              XOR AX,AX
0000:7695  e6 7e              OUT 0x7e,AL
0000:7697  e6 7e              OUT 0x7e,AL
0000:7699  e6 7e              OUT 0x7e,AL
0000:769b  e6 7e              OUT 0x7e,AL
0000:769d  2e f6 06 27 74 10  TEST byte ptr CS:[0x7427],0x10
0000:76a3  74 12              JZ 0x0000:76b7
0000:76a5  2e f6 06 26 74 10  TEST byte ptr CS:[0x7426],0x10
0000:76ab  75 4c              JNZ 0x0000:76f9
0000:76ad  8b da              MOV BX,DX
0000:76af  8b c5              MOV AX,BP
0000:76b1  f7 e1              MUL CX
0000:76b3  03 f0              ADD SI,AX
0000:76b5  8b d3              MOV DX,BX
0000:76b7  57                 PUSH DI
0000:76b8  55                 PUSH BP
0000:76b9  bb ff ff           MOV BX,0xffff
0000:76bc  d0 ed              SHR CH,0x1
0000:76be  73 0b              JNC 0x0000:76cb
0000:76c0  b0 ff              MOV AL,0xff
0000:76c2  d2 e8              SHR AL,CL
0000:76c4  aa                 STOSB ES:DI
0000:76c5  0a ed              OR CH,CH
0000:76c7  74 18              JZ 0x0000:76e1
0000:76c9  b3 ff              MOV BL,0xff
0000:76cb  b8 ff ff           MOV AX,0xffff
0000:76ce  86 c4              XCHG AH,AL
0000:76d0  b7 ff              MOV BH,0xff
0000:76d2  d3 cb              ROR BX,CL
0000:76d4  8a d8              MOV BL,AL
0000:76d6  d3 e8              SHR AX,CL
0000:76d8  86 c4              XCHG AH,AL
0000:76da  0a c7              OR AL,BH
0000:76dc  ab                 STOSW ES:DI
0000:76dd  fe cd              DEC CH
0000:76df  75 ea              JNZ 0x0000:76cb
0000:76e1  33 db              XOR BX,BX
0000:76e3  b7 ff              MOV BH,0xff
0000:76e5  d3 eb              SHR BX,CL
0000:76e7  8a c3              MOV AL,BL
0000:76e9  aa                 STOSB ES:DI
0000:76ea  33 c0              XOR AX,AX
0000:76ec  8a c2              MOV AL,DL
0000:76ee  03 f8              ADD DI,AX
0000:76f0  8a ee              MOV CH,DH
0000:76f2  4d                 DEC BP
0000:76f3  75 c4              JNZ 0x0000:76b9
0000:76f5  5d                 POP BP
0000:76f6  5f                 POP DI
0000:76f7  eb 03              JMP 0x0000:76fc
0000:76f9  e8 99 00           CALL 0x0000:7795
0000:76fc  b0 ff              MOV AL,0xff
0000:76fe  e6 7e              OUT 0x7e,AL
0000:7700  e6 7e              OUT 0x7e,AL
0000:7702  e6 7e              OUT 0x7e,AL
0000:7704  e6 7e              OUT 0x7e,AL
0000:7706  2e f6 06 27 74 01  TEST byte ptr CS:[0x7427],0x1
0000:770c  74 1b              JZ 0x0000:7729
0000:770e  2e f6 06 26 74 01  TEST byte ptr CS:[0x7426],0x1
0000:7714  75 0c              JNZ 0x0000:7722
0000:7716  8b da              MOV BX,DX
0000:7718  8b c5              MOV AX,BP
0000:771a  f7 e1              MUL CX
0000:771c  03 f0              ADD SI,AX
0000:771e  8b d3              MOV DX,BX
0000:7720  eb 07              JMP 0x0000:7729
0000:7722  b0 ce              MOV AL,0xce
0000:7724  e6 7c              OUT 0x7c,AL
0000:7726  e8 6c 00           CALL 0x0000:7795
0000:7729  2e f6 06 26 74 02  TEST byte ptr CS:[0x7426],0x2
0000:772f  75 14              JNZ 0x0000:7745
0000:7731  2e f6 06 27 74 02  TEST byte ptr CS:[0x7427],0x2
0000:7737  74 13              JZ 0x0000:774c
0000:7739  8b da              MOV BX,DX
0000:773b  8b c5              MOV AX,BP
0000:773d  f7 e1              MUL CX
0000:773f  03 f0              ADD SI,AX
0000:7741  8b d3              MOV DX,BX
0000:7743  eb 07              JMP 0x0000:774c
0000:7745  b0 cd              MOV AL,0xcd
0000:7747  e6 7c              OUT 0x7c,AL
0000:7749  e8 49 00           CALL 0x0000:7795
0000:774c  2e f6 06 26 74 04  TEST byte ptr CS:[0x7426],0x4
0000:7752  75 14              JNZ 0x0000:7768
0000:7754  2e f6 06 27 74 04  TEST byte ptr CS:[0x7427],0x4
0000:775a  74 13              JZ 0x0000:776f
0000:775c  8b da              MOV BX,DX
0000:775e  8b c5              MOV AX,BP
0000:7760  f7 e1              MUL CX
0000:7762  03 f0              ADD SI,AX
0000:7764  8b d3              MOV DX,BX
0000:7766  eb 07              JMP 0x0000:776f
0000:7768  b0 cb              MOV AL,0xcb
0000:776a  e6 7c              OUT 0x7c,AL
0000:776c  e8 26 00           CALL 0x0000:7795
0000:776f  2e f6 06 26 74 08  TEST byte ptr CS:[0x7426],0x8
0000:7775  75 14              JNZ 0x0000:778b
0000:7777  2e f6 06 27 74 08  TEST byte ptr CS:[0x7427],0x8
0000:777d  74 13              JZ 0x0000:7792
0000:777f  8b da              MOV BX,DX
0000:7781  8b c5              MOV AX,BP
0000:7783  f7 e1              MUL CX
0000:7785  03 f0              ADD SI,AX
0000:7787  8b d3              MOV DX,BX
0000:7789  eb 07              JMP 0x0000:7792
0000:778b  b0 c7              MOV AL,0xc7
0000:778d  e6 7c              OUT 0x7c,AL
0000:778f  e8 03 00           CALL 0x0000:7795
0000:7792  e9 df fe           JMP 0x0000:7674
0000:7795  57                 PUSH DI
0000:7796  55                 PUSH BP
0000:7797  33 db              XOR BX,BX
0000:7799  d0 ed              SHR CH,0x1
0000:779b  73 0b              JNC 0x0000:77a8
0000:779d  ac                 LODSB SI
0000:779e  d2 e8              SHR AL,CL
0000:77a0  aa                 STOSB ES:DI
0000:77a1  0a ed              OR CH,CH
0000:77a3  74 17              JZ 0x0000:77bc
0000:77a5  8a 5c ff           MOV BL,byte ptr [SI + -0x1]
0000:77a8  ad                 LODSW SI
0000:77a9  86 c4              XCHG AH,AL
0000:77ab  32 ff              XOR BH,BH
0000:77ad  d3 cb              ROR BX,CL
0000:77af  8a d8              MOV BL,AL
0000:77b1  d3 e8              SHR AX,CL
0000:77b3  86 c4              XCHG AH,AL
0000:77b5  0a c7              OR AL,BH
0000:77b7  ab                 STOSW ES:DI
0000:77b8  fe cd              DEC CH
0000:77ba  75 ec              JNZ 0x0000:77a8
0000:77bc  33 db              XOR BX,BX
0000:77be  8a 7c ff           MOV BH,byte ptr [SI + -0x1]
0000:77c1  d3 eb              SHR BX,CL
0000:77c3  8a c3              MOV AL,BL
0000:77c5  aa                 STOSB ES:DI
0000:77c6  33 c0              XOR AX,AX
0000:77c8  8a c2              MOV AL,DL
0000:77ca  03 f8              ADD DI,AX
0000:77cc  8a ee              MOV CH,DH
0000:77ce  4d                 DEC BP
0000:77cf  75 c6              JNZ 0x0000:7797
0000:77d1  5d                 POP BP
0000:77d2  5f                 POP DI
0000:77d3  c3                 RET
0000:77d4  06                 PUSH ES
0000:77d5  e8 02 00           CALL 0x0000:77da
0000:77d8  07                 POP ES
0000:77d9  cb                 RETF
0000:77da  55                 PUSH BP
0000:77db  bd 50 00           MOV BP,0x50
0000:77de  2b e9              SUB BP,CX
0000:77e0  8b d9              MOV BX,CX
0000:77e2  fa                 CLI
0000:77e3  8a e0              MOV AH,AL
0000:77e5  f6 d4              NOT AH
0000:77e7  b0 c0              MOV AL,0xc0
0000:77e9  e6 7c              OUT 0x7c,AL
0000:77eb  32 c0              XOR AL,AL
0000:77ed  d0 ec              SHR AH,0x1
0000:77ef  14 00              ADC AL,0x0
0000:77f1  fe c8              DEC AL
0000:77f3  e6 7e              OUT 0x7e,AL
0000:77f5  32 c0              XOR AL,AL
0000:77f7  d0 ec              SHR AH,0x1
0000:77f9  14 00              ADC AL,0x0
0000:77fb  fe c8              DEC AL
0000:77fd  e6 7e              OUT 0x7e,AL
0000:77ff  32 c0              XOR AL,AL
0000:7801  d0 ec              SHR AH,0x1
0000:7803  14 00              ADC AL,0x0
0000:7805  fe c8              DEC AL
0000:7807  e6 7e              OUT 0x7e,AL
0000:7809  32 c0              XOR AL,AL
0000:780b  d0 ec              SHR AH,0x1
0000:780d  14 00              ADC AL,0x0
0000:780f  fe c8              DEC AL
0000:7811  e6 7e              OUT 0x7e,AL
0000:7813  b8 00 a8           MOV AX,0xa800
0000:7816  8e c0              MOV AX,ES
0000:7818  b8 ff ff           MOV AX,0xffff
0000:781b  57                 PUSH DI
0000:781c  f3 aa              STOSB.REP ES:DI
0000:781e  8b cb              MOV CX,BX
0000:7820  03 fd              ADD DI,BP
0000:7822  83 ea 01           SUB DX,0x1
0000:7825  75 f5              JNZ 0x0000:781c
0000:7827  5f                 POP DI
0000:7828  33 c0              XOR AX,AX
0000:782a  e6 7c              OUT 0x7c,AL
0000:782c  fb                 STI
0000:782d  5d                 POP BP
0000:782e  c3                 RET
0000:7846  1e                 PUSH DS
0000:7847  fc                 CLD
0000:7848  33 c0              XOR AX,AX
0000:784a  8e d8              MOV AX,DS
0000:784c  8c c8              MOV AX,CS
0000:784e  8e c0              MOV AX,ES
0000:7850  be 2a 05           MOV SI,0x52a
0000:7853  bf 36 78           MOV DI,0x7836
0000:7856  a5                 MOVSW ES:DI,SI
0000:7857  a5                 MOVSW ES:DI,SI
0000:7858  a5                 MOVSW ES:DI,SI
0000:7859  a5                 MOVSW ES:DI,SI
0000:785a  a5                 MOVSW ES:DI,SI
0000:785b  a5                 MOVSW ES:DI,SI
0000:785c  a5                 MOVSW ES:DI,SI
0000:785d  a5                 MOVSW ES:DI,SI
0000:785e  b9 c0 02           MOV CX,0x2c0
0000:7861  e6 5f              OUT 0x5f,AL
0000:7863  e2 fc              LOOP 0x0000:7861
0000:7865  be 2a 05           MOV SI,0x52a
0000:7868  bf 36 78           MOV DI,0x7836
0000:786b  ad                 LODSW SI
0000:786c  26 09 05           OR word ptr ES:[DI],AX
0000:786f  83 c7 02           ADD DI,0x2
0000:7872  ad                 LODSW SI
0000:7873  26 09 05           OR word ptr ES:[DI],AX
0000:7876  83 c7 02           ADD DI,0x2
0000:7879  ad                 LODSW SI
0000:787a  26 09 05           OR word ptr ES:[DI],AX
0000:787d  83 c7 02           ADD DI,0x2
0000:7880  ad                 LODSW SI
0000:7881  26 09 05           OR word ptr ES:[DI],AX
0000:7884  83 c7 02           ADD DI,0x2
0000:7887  ad                 LODSW SI
0000:7888  26 09 05           OR word ptr ES:[DI],AX
0000:788b  83 c7 02           ADD DI,0x2
0000:788e  ad                 LODSW SI
0000:788f  26 09 05           OR word ptr ES:[DI],AX
0000:7892  83 c7 02           ADD DI,0x2
0000:7895  ad                 LODSW SI
0000:7896  26 09 05           OR word ptr ES:[DI],AX
0000:7899  83 c7 02           ADD DI,0x2
0000:789c  ad                 LODSW SI
0000:789d  26 09 05           OR word ptr ES:[DI],AX
0000:78a0  83 c7 02           ADD DI,0x2
0000:78a3  8c c8              MOV AX,CS
0000:78a5  8e d8              MOV AX,DS
0000:78a7  a1 b8 03           MOV AX,[0x3b8]
0000:78aa  a3 30 78           MOV [0x7830],AX
0000:78ad  a0 34 78           MOV AL,[0x7834]
0000:78b0  a2 35 78           MOV [0x7835],AL
0000:78b3  33 db              XOR BX,BX
0000:78b5  a0 36 78           MOV AL,[0x7836]
0000:78b8  a8 01              TEST AL,0x1
0000:78ba  74 04              JZ 0x0000:78c0
0000:78bc  81 cb 80 00        OR BX,0x80
0000:78c0  a8 02              TEST AL,0x2
0000:78c2  74 04              JZ 0x0000:78c8
0000:78c4  81 cb 00 20        OR BX,0x2000
0000:78c8  a8 04              TEST AL,0x4
0000:78ca  74 04              JZ 0x0000:78d0
0000:78cc  81 cb 00 10        OR BX,0x1000
0000:78d0  a8 08              TEST AL,0x8
0000:78d2  74 04              JZ 0x0000:78d8
0000:78d4  81 cb 00 08        OR BX,0x800
0000:78d8  a8 10              TEST AL,0x10
0000:78da  74 04              JZ 0x0000:78e0
0000:78dc  81 cb 00 04        OR BX,0x400
0000:78e0  f6 06 39 78 10     TEST byte ptr [0x7839],0x10
0000:78e5  74 03              JZ 0x0000:78ea
0000:78e7  83 cb 20           OR BX,0x20
0000:78ea  a0 3b 78           MOV AL,[0x783b]
0000:78ed  a8 02              TEST AL,0x2
0000:78ef  74 03              JZ 0x0000:78f4
0000:78f1  83 cb 10           OR BX,0x10
0000:78f4  a8 04              TEST AL,0x4
0000:78f6  74 03              JZ 0x0000:78fb
0000:78f8  83 cb 20           OR BX,0x20
0000:78fb  a8 40              TEST AL,0x40
0000:78fd  74 03              JZ 0x0000:7902
0000:78ff  83 cb 20           OR BX,0x20
0000:7902  a8 80              TEST AL,0x80
0000:7904  74 03              JZ 0x0000:7909
0000:7906  83 cb 20           OR BX,0x20
0000:7909  f6 06 3c 78 10     TEST byte ptr [0x783c],0x10
0000:790e  74 03              JZ 0x0000:7913
0000:7910  83 cb 10           OR BX,0x10
0000:7913  f6 06 3f 78 40     TEST byte ptr [0x783f],0x40
0000:7918  74 03              JZ 0x0000:791d
0000:791a  83 cb 10           OR BX,0x10
0000:791d  f6 06 43 78 08     TEST byte ptr [0x7843],0x8
0000:7922  74 04              JZ 0x0000:7928
0000:7924  81 cb 00 80        OR BX,0x8000
0000:7928  a0 44 78           MOV AL,[0x7844]
0000:792b  a8 01              TEST AL,0x1
0000:792d  74 03              JZ 0x0000:7932
0000:792f  83 cb 40           OR BX,0x40
0000:7932  a1 3d 78           MOV AX,[0x783d]
0000:7935  a9 04 08           TEST AX,0x804
0000:7938  74 05              JZ 0x0000:793f
0000:793a  83 cb 01           OR BX,0x1
0000:793d  eb 00              JMP 0x0000:793f
0000:793f  a9 08 40           TEST AX,0x4008
0000:7942  74 05              JZ 0x0000:7949
0000:7944  83 cb 02           OR BX,0x2
0000:7947  eb 00              JMP 0x0000:7949
0000:7949  8a 26 3f 78        MOV AH,byte ptr [0x783f]
0000:794d  a9 10 01           TEST AX,0x110
0000:7950  74 05              JZ 0x0000:7957
0000:7952  83 cb 04           OR BX,0x4
0000:7955  eb 00              JMP 0x0000:7957
0000:7957  a9 20 08           TEST AX,0x820
0000:795a  74 05              JZ 0x0000:7961
0000:795c  83 cb 08           OR BX,0x8
0000:795f  eb 00              JMP 0x0000:7961
0000:7961  f7 06 be 03 00 04  TEST word ptr [0x3be],0x400
0000:7967  74 2e              JZ 0x0000:7997
0000:7969  33 c0              XOR AX,AX
0000:796b  cd 7b              INT 0x7b
0000:796d  a8 01              TEST AL,0x1
0000:796f  74 03              JZ 0x0000:7974
0000:7971  83 cb 01           OR BX,0x1
0000:7974  a8 02              TEST AL,0x2
0000:7976  74 03              JZ 0x0000:797b
0000:7978  83 cb 08           OR BX,0x8
0000:797b  a8 04              TEST AL,0x4
0000:797d  74 03              JZ 0x0000:7982
0000:797f  83 cb 02           OR BX,0x2
0000:7982  a8 08              TEST AL,0x8
0000:7984  74 03              JZ 0x0000:7989
0000:7986  83 cb 04           OR BX,0x4
0000:7989  a8 10              TEST AL,0x10
0000:798b  74 03              JZ 0x0000:7990
0000:798d  83 cb 10           OR BX,0x10
0000:7990  a8 20              TEST AL,0x20
0000:7992  74 03              JZ 0x0000:7997
0000:7994  83 cb 20           OR BX,0x20
0000:7997  f7 06 be 03 00 02  TEST word ptr [0x3be],0x200
0000:799d  74 18              JZ 0x0000:79b7
0000:799f  8b c3              MOV AX,BX
0000:79a1  83 e3 cf           AND BX,0xffcf
0000:79a4  83 e0 30           AND AX,0x30
0000:79a7  a9 10 00           TEST AX,0x10
0000:79aa  74 03              JZ 0x0000:79af
0000:79ac  83 cb 20           OR BX,0x20
0000:79af  a9 20 00           TEST AX,0x20
0000:79b2  74 03              JZ 0x0000:79b7
0000:79b4  83 cb 10           OR BX,0x10
0000:79b7  89 1e b8 03        MOV word ptr [0x3b8],BX
0000:79bb  a1 30 78           MOV AX,[0x7830]
0000:79be  f7 d0              NOT AX
0000:79c0  83 c8 40           OR AX,0x40
0000:79c3  23 c3              AND AX,BX
0000:79c5  a3 ba 03           MOV [0x3ba],AX
0000:79c8  a3 bc 03           MOV [0x3bc],AX
0000:79cb  83 e0 bf           AND AX,0xffbf
0000:79ce  0b c0              OR AX,AX
0000:79d0  74 08              JZ 0x0000:79da
0000:79d2  c7 06 32 78 00 00  MOV word ptr [0x7832],0x0
0000:79d8  eb 1a              JMP 0x0000:79f4
0000:79da  a1 32 78           MOV AX,[0x7832]
0000:79dd  40                 INC AX
0000:79de  25 ff 7f           AND AX,0x7fff
0000:79e1  a3 32 78           MOV [0x7832],AX
0000:79e4  83 f8 10           CMP AX,0x10
0000:79e7  72 0b              JC 0x0000:79f4
0000:79e9  a9 01 00           TEST AX,0x1
0000:79ec  75 06              JNZ 0x0000:79f4
0000:79ee  a1 b8 03           MOV AX,[0x3b8]
0000:79f1  a3 bc 03           MOV [0x3bc],AX
0000:79f4  33 db              XOR BX,BX
0000:79f6  f6 06 38 78 04     TEST byte ptr [0x7838],0x4
0000:79fb  74 03              JZ 0x0000:7a00
0000:79fd  80 cb 02           OR BL,0x2
0000:7a00  f6 06 39 78 01     TEST byte ptr [0x7839],0x1
0000:7a05  74 03              JZ 0x0000:7a0a
0000:7a07  80 cb 10           OR BL,0x10
0000:7a0a  f6 06 3a 78 10     TEST byte ptr [0x783a],0x10
0000:7a0f  74 03              JZ 0x0000:7a14
0000:7a11  80 cb 04           OR BL,0x4
0000:7a14  a0 3b 78           MOV AL,[0x783b]
0000:7a17  a8 08              TEST AL,0x8
0000:7a19  74 03              JZ 0x0000:7a1e
0000:7a1b  80 cb 01           OR BL,0x1
0000:7a1e  a8 40              TEST AL,0x40
0000:7a20  74 03              JZ 0x0000:7a25
0000:7a22  80 cb 08           OR BL,0x8
0000:7a25  88 1e 34 78        MOV byte ptr [0x7834],BL
0000:7a29  a0 35 78           MOV AL,[0x7835]
0000:7a2c  f6 d0              NOT AL
0000:7a2e  22 c3              AND AL,BL
0000:7a30  a2 0e 04           MOV [0x40e],AL
0000:7a33  1f                 POP DS
0000:7a34  cb                 RETF
