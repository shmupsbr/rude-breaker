; Ghidra 12.1.4 disassembly of sgc_game.bin
; Language: x86:LE:16:Real Mode
; Image base: 0x100
0000:0100  e9 61 06           JMP 0x0000:0764
0000:0764  8c c8              MOV AX,CS
0000:0766  8e d8              MOV AX,DS
0000:0768  2e 8b 2e 81 00     MOV BP,word ptr CS:[0x81]
0000:076d  2e 89 2e 04 01     MOV word ptr CS:[0x104],BP
0000:0772  8c d0              MOV AX,SS
0000:0774  8e c0              MOV AX,ES
0000:0776  be 3e 07           MOV SI,0x73e
0000:0779  8b fd              MOV DI,BP
0000:077b  83 c7 00           ADD DI,0x0
0000:077e  b9 03 00           MOV CX,0x3
0000:0781  f3 a7              CMPSW.REPE ES:DI,SI
0000:0783  0b c9              OR CX,CX
0000:0785  74 0b              JZ 0x0000:0792
0000:0787  ba 44 07           MOV DX,0x744
0000:078a  b4 09              MOV AH,0x9
0000:078c  cd 21              INT 0x21
0000:078e  b8 01 00           MOV AX,0x1
0000:0791  cb                 RETF
0000:0792  8c c8              MOV AX,CS
0000:0794  8e c0              MOV AX,ES
0000:0796  8c 56 0e           MOV word ptr [BP + 0xe],SS
0000:0799  8b 5e 50           MOV BX,word ptr [BP + 0x50]
0000:079c  36 c7 47 04 1e 07  MOV word ptr SS:[BX + 0x4],0x71e
0000:07a2  36 c7 47 06 27 07  MOV word ptr SS:[BX + 0x6],0x727
0000:07a8  36 c7 47 08 32 07  MOV word ptr SS:[BX + 0x8],0x732
0000:07ae  e8 3b 06           CALL 0x0000:0dec
0000:07b1  bb 00 0b           MOV BX,0xb00
0000:07b4  b4 48              MOV AH,0x48
0000:07b6  cd 21              INT 0x21
0000:07b8  89 46 18           MOV word ptr [BP + 0x18],AX
0000:07bb  73 03              JNC 0x0000:07c0
0000:07bd  e9 c2 00           JMP 0x0000:0882
0000:07c0  bb 00 0a           MOV BX,0xa00
0000:07c3  b4 48              MOV AH,0x48
0000:07c5  cd 21              INT 0x21
0000:07c7  89 46 16           MOV word ptr [BP + 0x16],AX
0000:07ca  73 03              JNC 0x0000:07cf
0000:07cc  e9 b3 00           JMP 0x0000:0882
0000:07cf  bb 00 08           MOV BX,0x800
0000:07d2  b4 48              MOV AH,0x48
0000:07d4  cd 21              INT 0x21
0000:07d6  89 46 1a           MOV word ptr [BP + 0x1a],AX
0000:07d9  73 03              JNC 0x0000:07de
0000:07db  e9 a4 00           JMP 0x0000:0882
0000:07de  bb 00 08           MOV BX,0x800
0000:07e1  b4 48              MOV AH,0x48
0000:07e3  cd 21              INT 0x21
0000:07e5  89 46 1c           MOV word ptr [BP + 0x1c],AX
0000:07e8  73 03              JNC 0x0000:07ed
0000:07ea  e9 95 00           JMP 0x0000:0882
0000:07ed  bb 00 08           MOV BX,0x800
0000:07f0  b4 48              MOV AH,0x48
0000:07f2  cd 21              INT 0x21
0000:07f4  89 46 1e           MOV word ptr [BP + 0x1e],AX
0000:07f7  73 03              JNC 0x0000:07fc
0000:07f9  e9 86 00           JMP 0x0000:0882
0000:07fc  bb 00 08           MOV BX,0x800
0000:07ff  b4 48              MOV AH,0x48
0000:0801  cd 21              INT 0x21
0000:0803  89 46 20           MOV word ptr [BP + 0x20],AX
0000:0806  72 7a              JC 0x0000:0882
0000:0808  bb 00 08           MOV BX,0x800
0000:080b  b4 48              MOV AH,0x48
0000:080d  cd 21              INT 0x21
0000:080f  89 46 22           MOV word ptr [BP + 0x22],AX
0000:0812  72 6e              JC 0x0000:0882
0000:0814  bb 00 08           MOV BX,0x800
0000:0817  b4 48              MOV AH,0x48
0000:0819  cd 21              INT 0x21
0000:081b  89 46 24           MOV word ptr [BP + 0x24],AX
0000:081e  72 62              JC 0x0000:0882
0000:0820  bb 00 0a           MOV BX,0xa00
0000:0823  b4 48              MOV AH,0x48
0000:0825  cd 21              INT 0x21
0000:0827  89 46 26           MOV word ptr [BP + 0x26],AX
0000:082a  72 56              JC 0x0000:0882
0000:082c  8a 86 ba 00        MOV AL,byte ptr [BP + 0xba]
0000:0830  e6 a6              OUT 0xa6,AL
0000:0832  34 01              XOR AL,0x1
0000:0834  88 86 ba 00        MOV byte ptr [BP + 0xba],AL
0000:0838  e6 a4              OUT 0xa4,AL
0000:083a  e8 e3 05           CALL 0x0000:0e20
0000:083d  e8 4b 00           CALL 0x0000:088b
0000:0840  8e 46 26           MOV ES,word ptr [BP + 0x26]
0000:0843  b4 49              MOV AH,0x49
0000:0845  cd 21              INT 0x21
0000:0847  8e 46 24           MOV ES,word ptr [BP + 0x24]
0000:084a  b4 49              MOV AH,0x49
0000:084c  cd 21              INT 0x21
0000:084e  8e 46 22           MOV ES,word ptr [BP + 0x22]
0000:0851  b4 49              MOV AH,0x49
0000:0853  cd 21              INT 0x21
0000:0855  8e 46 20           MOV ES,word ptr [BP + 0x20]
0000:0858  b4 49              MOV AH,0x49
0000:085a  cd 21              INT 0x21
0000:085c  8e 46 1e           MOV ES,word ptr [BP + 0x1e]
0000:085f  b4 49              MOV AH,0x49
0000:0861  cd 21              INT 0x21
0000:0863  8e 46 1c           MOV ES,word ptr [BP + 0x1c]
0000:0866  b4 49              MOV AH,0x49
0000:0868  cd 21              INT 0x21
0000:086a  8e 46 1a           MOV ES,word ptr [BP + 0x1a]
0000:086d  b4 49              MOV AH,0x49
0000:086f  cd 21              INT 0x21
0000:0871  8e 46 16           MOV ES,word ptr [BP + 0x16]
0000:0874  b4 49              MOV AH,0x49
0000:0876  cd 21              INT 0x21
0000:0878  8e 46 18           MOV ES,word ptr [BP + 0x18]
0000:087b  b4 49              MOV AH,0x49
0000:087d  cd 21              INT 0x21
0000:087f  33 c0              XOR AX,AX
0000:0881  cb                 RETF
0000:0882  ba 55 07           MOV DX,0x755
0000:0885  b4 09              MOV AH,0x9
0000:0887  cd 15              INT 0x15
0000:0889  eb b5              JMP 0x0000:0840
0000:088b  e8 ba 0d           CALL 0x0000:1648
0000:088e  8b 46 40           MOV AX,word ptr [BP + 0x40]
0000:0891  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:0894  ff 5e 0c           CALLF [BP + 0xc]
0000:0897  e8 52 05           CALL 0x0000:0dec
0000:089a  8b 46 32           MOV AX,word ptr [BP + 0x32]
0000:089d  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:08a0  b0 01              MOV AL,0x1
0000:08a2  b4 18              MOV AH,0x18
0000:08a4  bb 01 02           MOV BX,0x201
0000:08a7  be 00 00           MOV SI,0x0
0000:08aa  bf 00 00           MOV DI,0x0
0000:08ad  ff 5e 0c           CALLF [BP + 0xc]
0000:08b0  1e                 PUSH DS
0000:08b1  06                 PUSH ES
0000:08b2  8b 46 14           MOV AX,word ptr [BP + 0x14]
0000:08b5  8e d8              MOV AX,DS
0000:08b7  8e c0              MOV AX,ES
0000:08b9  8b 76 52           MOV SI,word ptr [BP + 0x52]
0000:08bc  8b fe              MOV DI,SI
0000:08be  b9 00 29           MOV CX,0x2900
0000:08c1  e8 1a 12           CALL 0x0000:1ade
0000:08c4  ba d0 2f           MOV DX,0x2fd0
0000:08c7  bb 52 00           MOV BX,0x52
0000:08ca  b9 00 01           MOV CX,0x100
0000:08cd  b8 00 40           MOV AX,0x4000
0000:08d0  e8 6e 11           CALL 0x0000:1a41
0000:08d3  8b 46 18           MOV AX,word ptr [BP + 0x18]
0000:08d6  89 44 0a           MOV word ptr [SI + 0xa],AX
0000:08d9  8e c0              MOV AX,ES
0000:08db  26 a1 00 00        MOV AX,ES:[0x0]
0000:08df  89 44 08           MOV word ptr [SI + 0x8],AX
0000:08e2  07                 POP ES
0000:08e3  1f                 POP DS
0000:08e4  eb 4d              JMP 0x0000:0933
0000:08e6  e8 03 05           CALL 0x0000:0dec
0000:08e9  8b 46 32           MOV AX,word ptr [BP + 0x32]
0000:08ec  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:08ef  b0 01              MOV AL,0x1
0000:08f1  b4 18              MOV AH,0x18
0000:08f3  bb 01 02           MOV BX,0x201
0000:08f6  be 00 00           MOV SI,0x0
0000:08f9  bf 00 00           MOV DI,0x0
0000:08fc  ff 5e 0c           CALLF [BP + 0xc]
0000:08ff  1e                 PUSH DS
0000:0900  06                 PUSH ES
0000:0901  8b 46 14           MOV AX,word ptr [BP + 0x14]
0000:0904  8e d8              MOV AX,DS
0000:0906  8e c0              MOV AX,ES
0000:0908  8b 76 52           MOV SI,word ptr [BP + 0x52]
0000:090b  8b fe              MOV DI,SI
0000:090d  b9 00 29           MOV CX,0x2900
0000:0910  e8 cb 11           CALL 0x0000:1ade
0000:0913  ba d0 2f           MOV DX,0x2fd0
0000:0916  bb 52 00           MOV BX,0x52
0000:0919  b9 00 01           MOV CX,0x100
0000:091c  b8 00 40           MOV AX,0x4000
0000:091f  e8 1f 11           CALL 0x0000:1a41
0000:0922  8b 46 18           MOV AX,word ptr [BP + 0x18]
0000:0925  89 44 0a           MOV word ptr [SI + 0xa],AX
0000:0928  8e c0              MOV AX,ES
0000:092a  26 a1 02 00        MOV AX,ES:[0x2]
0000:092e  89 44 08           MOV word ptr [SI + 0x8],AX
0000:0931  07                 POP ES
0000:0932  1f                 POP DS
0000:0933  f7 46 6e 00 80     TEST word ptr [BP + 0x6e],0x8000
0000:0938  74 03              JZ 0x0000:093d
0000:093a  e9 f9 00           JMP 0x0000:0a36
0000:093d  8b 46 40           MOV AX,word ptr [BP + 0x40]
0000:0940  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:0943  ff 5e 0c           CALLF [BP + 0xc]
0000:0946  e8 9d 03           CALL 0x0000:0ce6
0000:0949  e8 8d 02           CALL 0x0000:0bd9
0000:094c  e8 c8 0d           CALL 0x0000:1717
0000:094f  1e                 PUSH DS
0000:0950  bb 52 00           MOV BX,0x52
0000:0953  b9 00 01           MOV CX,0x100
0000:0956  8b 76 52           MOV SI,word ptr [BP + 0x52]
0000:0959  8b 46 14           MOV AX,word ptr [BP + 0x14]
0000:095c  8e d8              MOV AX,DS
0000:095e  e8 cf 10           CALL 0x0000:1a30
0000:0961  1f                 POP DS
0000:0962  32 c0              XOR AL,AL
0000:0964  e6 6c              OUT 0x6c,AL
0000:0966  80 be c0 00 00     CMP byte ptr [BP + 0xc0],0x0
0000:096b  74 61              JZ 0x0000:09ce
0000:096d  80 be c1 00 00     CMP byte ptr [BP + 0xc1],0x0
0000:0972  75 0a              JNZ 0x0000:097e
0000:0974  8a 86 c0 00        MOV AL,byte ptr [BP + 0xc0]
0000:0978  88 86 c1 00        MOV byte ptr [BP + 0xc1],AL
0000:097c  eb 50              JMP 0x0000:09ce
0000:097e  f7 46 72 00 40     TEST word ptr [BP + 0x72],0x4000
0000:0983  75 2c              JNZ 0x0000:09b1
0000:0985  8b 46 3c           MOV AX,word ptr [BP + 0x3c]
0000:0988  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:098b  ff 5e 0c           CALLF [BP + 0xc]
0000:098e  e4 60              IN AL,0x60
0000:0990  a8 20              TEST AL,0x20
0000:0992  74 08              JZ 0x0000:099c
0000:0994  eb 00              JMP 0x0000:0996
0000:0996  e4 60              IN AL,0x60
0000:0998  a8 20              TEST AL,0x20
0000:099a  75 f8              JNZ 0x0000:0994
0000:099c  f7 46 72 00 10     TEST word ptr [BP + 0x72],0x1000
0000:09a1  75 06              JNZ 0x0000:09a9
0000:09a3  e4 0a              IN AL,0xa
0000:09a5  0c 10              OR AL,0x10
0000:09a7  e6 0a              OUT 0xa,AL
0000:09a9  eb 00              JMP 0x0000:09ab
0000:09ab  e4 60              IN AL,0x60
0000:09ad  a8 20              TEST AL,0x20
0000:09af  74 f8              JZ 0x0000:09a9
0000:09b1  f7 46 72 00 10     TEST word ptr [BP + 0x72],0x1000
0000:09b6  75 06              JNZ 0x0000:09be
0000:09b8  e4 0a              IN AL,0xa
0000:09ba  24 ef              AND AL,0xef
0000:09bc  e6 0a              OUT 0xa,AL
0000:09be  8b 46 34           MOV AX,word ptr [BP + 0x34]
0000:09c1  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:09c4  ff 5e 0c           CALLF [BP + 0xc]
0000:09c7  fe 8e c1 00        DEC byte ptr [BP + 0xc1]
0000:09cb  e9 65 ff           JMP 0x0000:0933
0000:09ce  f7 46 72 00 40     TEST word ptr [BP + 0x72],0x4000
0000:09d3  75 2c              JNZ 0x0000:0a01
0000:09d5  8b 46 3c           MOV AX,word ptr [BP + 0x3c]
0000:09d8  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:09db  ff 5e 0c           CALLF [BP + 0xc]
0000:09de  e4 60              IN AL,0x60
0000:09e0  a8 20              TEST AL,0x20
0000:09e2  74 08              JZ 0x0000:09ec
0000:09e4  eb 00              JMP 0x0000:09e6
0000:09e6  e4 60              IN AL,0x60
0000:09e8  a8 20              TEST AL,0x20
0000:09ea  75 f8              JNZ 0x0000:09e4
0000:09ec  f7 46 72 00 10     TEST word ptr [BP + 0x72],0x1000
0000:09f1  75 06              JNZ 0x0000:09f9
0000:09f3  e4 0a              IN AL,0xa
0000:09f5  0c 10              OR AL,0x10
0000:09f7  e6 0a              OUT 0xa,AL
0000:09f9  eb 00              JMP 0x0000:09fb
0000:09fb  e4 60              IN AL,0x60
0000:09fd  a8 20              TEST AL,0x20
0000:09ff  74 f8              JZ 0x0000:09f9
0000:0a01  e8 b5 05           CALL 0x0000:0fb9
0000:0a04  8a 86 ba 00        MOV AL,byte ptr [BP + 0xba]
0000:0a08  e6 a6              OUT 0xa6,AL
0000:0a0a  34 01              XOR AL,0x1
0000:0a0c  88 86 ba 00        MOV byte ptr [BP + 0xba],AL
0000:0a10  e6 a4              OUT 0xa4,AL
0000:0a12  f7 46 72 00 10     TEST word ptr [BP + 0x72],0x1000
0000:0a17  75 06              JNZ 0x0000:0a1f
0000:0a19  e4 0a              IN AL,0xa
0000:0a1b  24 ef              AND AL,0xef
0000:0a1d  e6 0a              OUT 0xa,AL
0000:0a1f  8b 46 34           MOV AX,word ptr [BP + 0x34]
0000:0a22  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:0a25  ff 5e 0c           CALLF [BP + 0xc]
0000:0a28  f7 46 72 00 20     TEST word ptr [BP + 0x72],0x2000
0000:0a2d  74 04              JZ 0x0000:0a33
0000:0a2f  b0 10              MOV AL,0x10
0000:0a31  e6 6c              OUT 0x6c,AL
0000:0a33  e9 fd fe           JMP 0x0000:0933
0000:0a36  1e                 PUSH DS
0000:0a37  8c c8              MOV AX,CS
0000:0a39  8e d8              MOV AX,DS
0000:0a3b  be eb 0a           MOV SI,0xaeb
0000:0a3e  e8 84 08           CALL 0x0000:12c5
0000:0a41  1f                 POP DS
0000:0a42  b0 10              MOV AL,0x10
0000:0a44  e6 74              OUT 0x74,AL
0000:0a46  81 66 72 ff 7f     AND word ptr [BP + 0x72],0x7fff
0000:0a4b  06                 PUSH ES
0000:0a4c  8b 7e 52           MOV DI,word ptr [BP + 0x52]
0000:0a4f  b9 00 29           MOV CX,0x2900
0000:0a52  8c d0              MOV AX,SS
0000:0a54  8e c0              MOV AX,ES
0000:0a56  e8 85 10           CALL 0x0000:1ade
0000:0a59  07                 POP ES
0000:0a5a  80 be bc 00 00     CMP byte ptr [BP + 0xbc],0x0
0000:0a5f  75 03              JNZ 0x0000:0a64
0000:0a61  e9 82 fe           JMP 0x0000:08e6
0000:0a64  32 c0              XOR AL,AL
0000:0a66  e6 6c              OUT 0x6c,AL
0000:0a68  f7 46 72 00 10     TEST word ptr [BP + 0x72],0x1000
0000:0a6d  75 06              JNZ 0x0000:0a75
0000:0a6f  e4 0a              IN AL,0xa
0000:0a71  24 ef              AND AL,0xef
0000:0a73  e6 0a              OUT 0xa,AL
0000:0a75  e8 3e 0b           CALL 0x0000:15b6
0000:0a78  f6 86 b9 00 80     TEST byte ptr [BP + 0xb9],0x80
0000:0a7d  75 01              JNZ 0x0000:0a80
0000:0a7f  c3                 RET
0000:0a80  b4 41              MOV AH,0x41
0000:0a82  cd 18              INT 0x18
0000:0a84  33 c0              XOR AX,AX
0000:0a86  e6 a8              OUT 0xa8,AL
0000:0a88  e6 aa              OUT 0xaa,AL
0000:0a8a  e6 ac              OUT 0xac,AL
0000:0a8c  e6 ae              OUT 0xae,AL
0000:0a8e  fa                 CLI
0000:0a8f  06                 PUSH ES
0000:0a90  b8 00 a8           MOV AX,0xa800
0000:0a93  8e c0              MOV AX,ES
0000:0a95  8a e0              MOV AH,AL
0000:0a97  f6 d4              NOT AH
0000:0a99  b0 c0              MOV AL,0xc0
0000:0a9b  e6 7c              OUT 0x7c,AL
0000:0a9d  33 c0              XOR AX,AX
0000:0a9f  e6 7e              OUT 0x7e,AL
0000:0aa1  e6 7e              OUT 0x7e,AL
0000:0aa3  e6 7e              OUT 0x7e,AL
0000:0aa5  e6 7e              OUT 0x7e,AL
0000:0aa7  b0 01              MOV AL,0x1
0000:0aa9  e6 a4              OUT 0xa4,AL
0000:0aab  e6 a6              OUT 0xa6,AL
0000:0aad  b8 ff ff           MOV AX,0xffff
0000:0ab0  b9 80 3e           MOV CX,0x3e80
0000:0ab3  f3 ab              STOSW.REP ES:DI
0000:0ab5  32 c0              XOR AL,AL
0000:0ab7  e6 a4              OUT 0xa4,AL
0000:0ab9  e6 a6              OUT 0xa6,AL
0000:0abb  b8 ff ff           MOV AX,0xffff
0000:0abe  b9 80 3e           MOV CX,0x3e80
0000:0ac1  f3 ab              STOSW.REP ES:DI
0000:0ac3  33 c0              XOR AX,AX
0000:0ac5  e6 7c              OUT 0x7c,AL
0000:0ac7  07                 POP ES
0000:0ac8  fb                 STI
0000:0ac9  1e                 PUSH DS
0000:0aca  8c c8              MOV AX,CS
0000:0acc  8e d8              MOV AX,DS
0000:0ace  be eb 0a           MOV SI,0xaeb
0000:0ad1  e8 f1 07           CALL 0x0000:12c5
0000:0ad4  be 6d 0b           MOV SI,0xb6d
0000:0ad7  e8 eb 07           CALL 0x0000:12c5
0000:0ada  1f                 POP DS
0000:0adb  8b 46 3e           MOV AX,word ptr [BP + 0x3e]
0000:0ade  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:0ae1  ff 5e 0c           CALLF [BP + 0xc]
0000:0ae4  b4 40              MOV AH,0x40
0000:0ae6  cd 18              INT 0x18
0000:0ae8  e9 fb fd           JMP 0x0000:08e6
0000:0bd9  f7 46 72 00 80     TEST word ptr [BP + 0x72],0x8000
0000:0bde  75 01              JNZ 0x0000:0be1
0000:0be0  c3                 RET
0000:0be1  1e                 PUSH DS
0000:0be2  06                 PUSH ES
0000:0be3  b8 00 a0           MOV AX,0xa000
0000:0be6  8e d8              MOV AX,DS
0000:0be8  8c c8              MOV AX,CS
0000:0bea  8e c0              MOV AX,ES
0000:0bec  be 74 0e           MOV SI,0xe74
0000:0bef  bf a8 0c           MOV DI,0xca8
0000:0bf2  b9 0a 00           MOV CX,0xa
0000:0bf5  f3 a5              MOVSW.REP ES:DI,SI
0000:0bf7  be 74 2e           MOV SI,0x2e74
0000:0bfa  bf bc 0c           MOV DI,0xcbc
0000:0bfd  b9 0a 00           MOV CX,0xa
0000:0c00  f3 a5              MOVSW.REP ES:DI,SI
0000:0c02  07                 POP ES
0000:0c03  1f                 POP DS
0000:0c04  8b 46 3c           MOV AX,word ptr [BP + 0x3c]
0000:0c07  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:0c0a  ff 5e 0c           CALLF [BP + 0xc]
0000:0c0d  f7 46 6e 00 80     TEST word ptr [BP + 0x6e],0x8000
0000:0c12  75 40              JNZ 0x0000:0c54
0000:0c14  8b 46 40           MOV AX,word ptr [BP + 0x40]
0000:0c17  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:0c1a  ff 5e 0c           CALLF [BP + 0xc]
0000:0c1d  e8 c6 00           CALL 0x0000:0ce6
0000:0c20  e8 f4 0a           CALL 0x0000:1717
0000:0c23  f7 46 72 00 80     TEST word ptr [BP + 0x72],0x8000
0000:0c28  74 2a              JZ 0x0000:0c54
0000:0c2a  2e ff 06 a6 0c     INC word ptr CS:[0xca6]
0000:0c2f  1e                 PUSH DS
0000:0c30  8c c8              MOV AX,CS
0000:0c32  8e d8              MOV AX,DS
0000:0c34  2e f7 06 a6 0c 01 00  TEST word ptr CS:[0xca6],0x1
0000:0c3b  75 08              JNZ 0x0000:0c45
0000:0c3d  be d3 0c           MOV SI,0xcd3
0000:0c40  e8 82 06           CALL 0x0000:12c5
0000:0c43  eb 06              JMP 0x0000:0c4b
0000:0c45  be d0 0c           MOV SI,0xcd0
0000:0c48  e8 7a 06           CALL 0x0000:12c5
0000:0c4b  be d6 0c           MOV SI,0xcd6
0000:0c4e  e8 74 06           CALL 0x0000:12c5
0000:0c51  1f                 POP DS
0000:0c52  eb b0              JMP 0x0000:0c04
0000:0c54  1e                 PUSH DS
0000:0c55  06                 PUSH ES
0000:0c56  8c c8              MOV AX,CS
0000:0c58  8e d8              MOV AX,DS
0000:0c5a  b8 00 a0           MOV AX,0xa000
0000:0c5d  8e c0              MOV AX,ES
0000:0c5f  be a8 0c           MOV SI,0xca8
0000:0c62  bf 74 0e           MOV DI,0xe74
0000:0c65  b9 0a 00           MOV CX,0xa
0000:0c68  f3 a5              MOVSW.REP ES:DI,SI
0000:0c6a  be bc 0c           MOV SI,0xcbc
0000:0c6d  bf 74 2e           MOV DI,0x2e74
0000:0c70  b9 0a 00           MOV CX,0xa
0000:0c73  f3 a5              MOVSW.REP ES:DI,SI
0000:0c75  07                 POP ES
0000:0c76  1f                 POP DS
0000:0c77  f7 46 6e 00 80     TEST word ptr [BP + 0x6e],0x8000
0000:0c7c  75 26              JNZ 0x0000:0ca4
0000:0c7e  8b 46 3c           MOV AX,word ptr [BP + 0x3c]
0000:0c81  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:0c84  ff 5e 0c           CALLF [BP + 0xc]
0000:0c87  f7 46 6e 00 80     TEST word ptr [BP + 0x6e],0x8000
0000:0c8c  75 16              JNZ 0x0000:0ca4
0000:0c8e  8b 46 40           MOV AX,word ptr [BP + 0x40]
0000:0c91  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:0c94  ff 5e 0c           CALLF [BP + 0xc]
0000:0c97  e8 4c 00           CALL 0x0000:0ce6
0000:0c9a  e8 7a 0a           CALL 0x0000:1717
0000:0c9d  f7 46 6c 80 00     TEST word ptr [BP + 0x6c],0x80
0000:0ca2  75 da              JNZ 0x0000:0c7e
0000:0ca4  c3                 RET
0000:0ce6  f7 46 72 00 80     TEST word ptr [BP + 0x72],0x8000
0000:0ceb  74 03              JZ 0x0000:0cf0
0000:0ced  e8 72 00           CALL 0x0000:0d62
0000:0cf0  8b 5e 72           MOV BX,word ptr [BP + 0x72]
0000:0cf3  8b 46 6e           MOV AX,word ptr [BP + 0x6e]
0000:0cf6  a9 80 00           TEST AX,0x80
0000:0cf9  74 04              JZ 0x0000:0cff
0000:0cfb  81 f3 00 80        XOR BX,0x8000
0000:0cff  f7 c3 00 08        TEST BX,0x800
0000:0d03  74 30              JZ 0x0000:0d35
0000:0d05  a9 00 10           TEST AX,0x1000
0000:0d08  74 09              JZ 0x0000:0d13
0000:0d0a  81 f3 00 01        XOR BX,0x100
0000:0d0e  c6 86 c4 00 01     MOV byte ptr [BP + 0xc4],0x1
0000:0d13  a9 00 08           TEST AX,0x800
0000:0d16  74 04              JZ 0x0000:0d1c
0000:0d18  81 f3 00 20        XOR BX,0x2000
0000:0d1c  a9 00 01           TEST AX,0x100
0000:0d1f  74 04              JZ 0x0000:0d25
0000:0d21  81 f3 00 10        XOR BX,0x1000
0000:0d25  8b 46 6c           MOV AX,word ptr [BP + 0x6c]
0000:0d28  81 e3 ff bf        AND BX,0xbfff
0000:0d2c  a9 00 20           TEST AX,0x2000
0000:0d2f  74 04              JZ 0x0000:0d35
0000:0d31  81 cb 00 40        OR BX,0x4000
0000:0d35  89 5e 72           MOV word ptr [BP + 0x72],BX
0000:0d38  f7 c3 00 08        TEST BX,0x800
0000:0d3c  74 23              JZ 0x0000:0d61
0000:0d3e  8b 46 6e           MOV AX,word ptr [BP + 0x6e]
0000:0d41  a9 00 04           TEST AX,0x400
0000:0d44  74 05              JZ 0x0000:0d4b
0000:0d46  c6 86 cc 00 01     MOV byte ptr [BP + 0xcc],0x1
0000:0d4b  a9 00 02           TEST AX,0x200
0000:0d4e  74 11              JZ 0x0000:0d61
0000:0d50  8a 9e c0 00        MOV BL,byte ptr [BP + 0xc0]
0000:0d54  fe c3              INC BL
0000:0d56  80 e3 03           AND BL,0x3
0000:0d59  88 9e c0 00        MOV byte ptr [BP + 0xc0],BL
0000:0d5d  88 9e c1 00        MOV byte ptr [BP + 0xc1],BL
0000:0d61  c3                 RET
0000:0d62  f7 46 72 00 08     TEST word ptr [BP + 0x72],0x800
0000:0d67  75 6e              JNZ 0x0000:0dd7
0000:0d69  8a a6 c3 00        MOV AH,byte ptr [BP + 0xc3]
0000:0d6d  8a 86 c2 00        MOV AL,byte ptr [BP + 0xc2]
0000:0d71  f6 c4 10           TEST AH,0x10
0000:0d74  75 48              JNZ 0x0000:0dbe
0000:0d76  f6 c4 08           TEST AH,0x8
0000:0d79  75 39              JNZ 0x0000:0db4
0000:0d7b  f6 c4 04           TEST AH,0x4
0000:0d7e  75 2a              JNZ 0x0000:0daa
0000:0d80  f6 c4 02           TEST AH,0x2
0000:0d83  75 1b              JNZ 0x0000:0da0
0000:0d85  f6 c4 01           TEST AH,0x1
0000:0d88  75 0c              JNZ 0x0000:0d96
0000:0d8a  eb 00              JMP 0x0000:0d8c
0000:0d8c  a8 08              TEST AL,0x8
0000:0d8e  74 3d              JZ 0x0000:0dcd
0000:0d90  80 8e c3 00 01     OR byte ptr [BP + 0xc3],0x1
0000:0d95  c3                 RET
0000:0d96  a8 02              TEST AL,0x2
0000:0d98  74 33              JZ 0x0000:0dcd
0000:0d9a  80 8e c3 00 02     OR byte ptr [BP + 0xc3],0x2
0000:0d9f  c3                 RET
0000:0da0  a8 04              TEST AL,0x4
0000:0da2  74 29              JZ 0x0000:0dcd
0000:0da4  80 8e c3 00 04     OR byte ptr [BP + 0xc3],0x4
0000:0da9  c3                 RET
0000:0daa  a8 10              TEST AL,0x10
0000:0dac  74 1f              JZ 0x0000:0dcd
0000:0dae  80 8e c3 00 08     OR byte ptr [BP + 0xc3],0x8
0000:0db3  c3                 RET
0000:0db4  a8 01              TEST AL,0x1
0000:0db6  74 15              JZ 0x0000:0dcd
0000:0db8  80 8e c3 00 10     OR byte ptr [BP + 0xc3],0x10
0000:0dbd  c3                 RET
0000:0dbe  a8 10              TEST AL,0x10
0000:0dc0  74 0b              JZ 0x0000:0dcd
0000:0dc2  c6 86 c3 00 00     MOV byte ptr [BP + 0xc3],0x0
0000:0dc7  81 4e 72 00 08     OR word ptr [BP + 0x72],0x800
0000:0dcc  c3                 RET
0000:0dcd  0a c0              OR AL,AL
0000:0dcf  74 05              JZ 0x0000:0dd6
0000:0dd1  c6 86 c3 00 00     MOV byte ptr [BP + 0xc3],0x0
0000:0dd6  c3                 RET
0000:0dd7  f7 46 6e 00 3f     TEST word ptr [BP + 0x6e],0x3f00
0000:0ddc  75 08              JNZ 0x0000:0de6
0000:0dde  80 be c2 00 00     CMP byte ptr [BP + 0xc2],0x0
0000:0de3  75 01              JNZ 0x0000:0de6
0000:0de5  c3                 RET
0000:0de6  81 66 72 ff f7     AND word ptr [BP + 0x72],0xf7ff
0000:0deb  c3                 RET
0000:0dec  c7 46 6e 00 00     MOV word ptr [BP + 0x6e],0x0
0000:0df1  c7 46 74 00 00     MOV word ptr [BP + 0x74],0x0
0000:0df6  c7 46 76 00 00     MOV word ptr [BP + 0x76],0x0
0000:0dfb  c7 46 7a 00 00     MOV word ptr [BP + 0x7a],0x0
0000:0e00  c7 46 7c 00 00     MOV word ptr [BP + 0x7c],0x0
0000:0e05  c7 46 7e e0 01     MOV word ptr [BP + 0x7e],0x1e0
0000:0e0a  c7 86 82 00 02 00  MOV word ptr [BP + 0x82],0x2
0000:0e10  c6 86 b7 00 00     MOV byte ptr [BP + 0xb7],0x0
0000:0e15  c6 86 bb 00 ff     MOV byte ptr [BP + 0xbb],0xff
0000:0e1a  c6 86 bc 00 01     MOV byte ptr [BP + 0xbc],0x1
0000:0e1f  c3                 RET
0000:0e20  06                 PUSH ES
0000:0e21  8c c8              MOV AX,CS
0000:0e23  8e c0              MOV AX,ES
0000:0e25  bb 48 0e           MOV BX,0xe48
0000:0e28  33 c0              XOR AX,AX
0000:0e2a  cd 1c              INT 0x1c
0000:0e2c  07                 POP ES
0000:0e2d  2e a1 48 0e        MOV AX,CS:[0xe48]
0000:0e31  2e 03 06 4a 0e     ADD AX,word ptr CS:[0xe4a]
0000:0e36  2e 03 06 4c 0e     ADD AX,word ptr CS:[0xe4c]
0000:0e3b  2e a3 7e 0e        MOV CS:[0xe7e],AX
0000:0e3f  e8 0c 00           CALL 0x0000:0e4e
0000:0e42  2e a3 7e 0e        MOV CS:[0xe7e],AX
0000:0e46  c3                 RET
0000:0e4e  53                 PUSH BX
0000:0e4f  e8 0b 00           CALL 0x0000:0e5d
0000:0e52  8b d8              MOV BX,AX
0000:0e54  53                 PUSH BX
0000:0e55  e8 05 00           CALL 0x0000:0e5d
0000:0e58  5b                 POP BX
0000:0e59  8a e3              MOV AH,BL
0000:0e5b  5b                 POP BX
0000:0e5c  c3                 RET
0000:0e5d  8b 46 5c           MOV AX,word ptr [BP + 0x5c]
0000:0e60  2e 8b 1e 7e 0e     MOV BX,word ptr CS:[0xe7e]
0000:0e65  03 5e 64           ADD BX,word ptr [BP + 0x64]
0000:0e68  03 d8              ADD BX,AX
0000:0e6a  86 c7              XCHG BH,AL
0000:0e6c  d1 db              RCR BX,0x1
0000:0e6e  d1 c0              ROL AX,0x1
0000:0e70  1b d8              SBB BX,AX
0000:0e72  2b d8              SUB BX,AX
0000:0e74  03 c3              ADD AX,BX
0000:0e76  2e 01 06 7e 0e     ADD word ptr CS:[0xe7e],AX
0000:0e7b  02 c4              ADD AL,AH
0000:0e7d  c3                 RET
0000:0e80  32 ff              XOR BH,BH
0000:0e82  f6 c3 c0           TEST BL,0xc0
0000:0e85  74 0c              JZ 0x0000:0e93
0000:0e87  f6 c3 80           TEST BL,0x80
0000:0e8a  74 0f              JZ 0x0000:0e9b
0000:0e8c  f6 c3 40           TEST BL,0x40
0000:0e8f  74 17              JZ 0x0000:0ea8
0000:0e91  eb 23              JMP 0x0000:0eb6
0000:0e93  03 db              ADD BX,BX
0000:0e95  2e 8b 87 48 18     MOV AX,word ptr CS:[BX + 0x1848]
0000:0e9a  c3                 RET
0000:0e9b  f7 db              NEG BX
0000:0e9d  83 c3 7f           ADD BX,0x7f
0000:0ea0  03 db              ADD BX,BX
0000:0ea2  2e 8b 87 48 18     MOV AX,word ptr CS:[BX + 0x1848]
0000:0ea7  c3                 RET
0000:0ea8  81 eb 80 00        SUB BX,0x80
0000:0eac  03 db              ADD BX,BX
0000:0eae  2e 8b 87 48 18     MOV AX,word ptr CS:[BX + 0x1848]
0000:0eb3  f7 d8              NEG AX
0000:0eb5  c3                 RET
0000:0eb6  f7 db              NEG BX
0000:0eb8  81 c3 ff 00        ADD BX,0xff
0000:0ebc  03 db              ADD BX,BX
0000:0ebe  2e 8b 87 48 18     MOV AX,word ptr CS:[BX + 0x1848]
0000:0ec3  f7 d8              NEG AX
0000:0ec5  c3                 RET
0000:0ec6  8b 5c 24           MOV BX,word ptr [SI + 0x24]
0000:0ec9  8b 44 22           MOV AX,word ptr [SI + 0x22]
0000:0ecc  3b da              CMP BX,DX
0000:0ece  7d 5c              JGE 0x0000:0f2c
0000:0ed0  3b c1              CMP AX,CX
0000:0ed2  7d 2d              JGE 0x0000:0f01
0000:0ed4  2b c8              SUB CX,AX
0000:0ed6  2b d3              SUB DX,BX
0000:0ed8  8b c1              MOV AX,CX
0000:0eda  8b ca              MOV CX,DX
0000:0edc  33 d2              XOR DX,DX
0000:0ede  8a f2              MOV DH,DL
0000:0ee0  8a d4              MOV DL,AH
0000:0ee2  8a e0              MOV AH,AL
0000:0ee4  32 c0              XOR AL,AL
0000:0ee6  f7 f1              DIV CX
0000:0ee8  8b d0              MOV DX,AX
0000:0eea  b8 00 40           MOV AX,0x4000
0000:0eed  bb c8 18           MOV BX,0x18c8
0000:0ef0  b9 40 00           MOV CX,0x40
0000:0ef3  2e 3b 17           CMP DX,word ptr CS:[BX]
0000:0ef6  72 08              JC 0x0000:0f00
0000:0ef8  83 c3 02           ADD BX,0x2
0000:0efb  2d 00 01           SUB AX,0x100
0000:0efe  e2 f3              LOOP 0x0000:0ef3
0000:0f00  c3                 RET
0000:0f01  2b c1              SUB AX,CX
0000:0f03  2b d3              SUB DX,BX
0000:0f05  8b ca              MOV CX,DX
0000:0f07  33 d2              XOR DX,DX
0000:0f09  8a f2              MOV DH,DL
0000:0f0b  8a d4              MOV DL,AH
0000:0f0d  8a e0              MOV AH,AL
0000:0f0f  32 c0              XOR AL,AL
0000:0f11  f7 f1              DIV CX
0000:0f13  8b d0              MOV DX,AX
0000:0f15  b8 00 3f           MOV AX,0x3f00
0000:0f18  bb c8 18           MOV BX,0x18c8
0000:0f1b  b9 40 00           MOV CX,0x40
0000:0f1e  2e 3b 17           CMP DX,word ptr CS:[BX]
0000:0f21  72 08              JC 0x0000:0f2b
0000:0f23  83 c3 02           ADD BX,0x2
0000:0f26  05 00 01           ADD AX,0x100
0000:0f29  e2 f3              LOOP 0x0000:0f1e
0000:0f2b  c3                 RET
0000:0f2c  3b c1              CMP AX,CX
0000:0f2e  7e 2b              JLE 0x0000:0f5b
0000:0f30  2b c1              SUB AX,CX
0000:0f32  2b da              SUB BX,DX
0000:0f34  8b cb              MOV CX,BX
0000:0f36  33 d2              XOR DX,DX
0000:0f38  8a f2              MOV DH,DL
0000:0f3a  8a d4              MOV DL,AH
0000:0f3c  8a e0              MOV AH,AL
0000:0f3e  32 c0              XOR AL,AL
0000:0f40  f7 f1              DIV CX
0000:0f42  8b d0              MOV DX,AX
0000:0f44  b8 00 c0           MOV AX,0xc000
0000:0f47  bb c8 18           MOV BX,0x18c8
0000:0f4a  b9 40 00           MOV CX,0x40
0000:0f4d  2e 3b 17           CMP DX,word ptr CS:[BX]
0000:0f50  72 08              JC 0x0000:0f5a
0000:0f52  83 c3 02           ADD BX,0x2
0000:0f55  2d 00 01           SUB AX,0x100
0000:0f58  e2 f3              LOOP 0x0000:0f4d
0000:0f5a  c3                 RET
0000:0f5b  2b c8              SUB CX,AX
0000:0f5d  2b da              SUB BX,DX
0000:0f5f  8b c1              MOV AX,CX
0000:0f61  8b cb              MOV CX,BX
0000:0f63  33 d2              XOR DX,DX
0000:0f65  8a f2              MOV DH,DL
0000:0f67  8a d4              MOV DL,AH
0000:0f69  8a e0              MOV AH,AL
0000:0f6b  32 c0              XOR AL,AL
0000:0f6d  f7 f1              DIV CX
0000:0f6f  8b d0              MOV DX,AX
0000:0f71  b8 00 bf           MOV AX,0xbf00
0000:0f74  bb c8 18           MOV BX,0x18c8
0000:0f77  b9 40 00           MOV CX,0x40
0000:0f7a  2e 3b 17           CMP DX,word ptr CS:[BX]
0000:0f7d  72 08              JC 0x0000:0f87
0000:0f7f  83 c3 02           ADD BX,0x2
0000:0f82  05 00 01           ADD AX,0x100
0000:0f85  e2 f3              LOOP 0x0000:0f7a
0000:0f87  c3                 RET
0000:0f88  e8 3b ff           CALL 0x0000:0ec6
0000:0f8b  8b 5c 2e           MOV BX,word ptr [SI + 0x2e]
0000:0f8e  33 c9              XOR CX,CX
0000:0f90  33 d2              XOR DX,DX
0000:0f92  8a cc              MOV CL,AH
0000:0f94  8a d7              MOV DL,BH
0000:0f96  81 c1 00 01        ADD CX,0x100
0000:0f9a  2b ca              SUB CX,DX
0000:0f9c  81 e1 ff 00        AND CX,0xff
0000:0fa0  81 f9 80 00        CMP CX,0x80
0000:0fa4  72 05              JC 0x0000:0fab
0000:0fa6  77 0a              JA 0x0000:0fb2
0000:0fa8  33 c0              XOR AX,AX
0000:0faa  c3                 RET
0000:0fab  8b 44 2e           MOV AX,word ptr [SI + 0x2e]
0000:0fae  03 44 30           ADD AX,word ptr [SI + 0x30]
0000:0fb1  c3                 RET
0000:0fb2  8b 44 2e           MOV AX,word ptr [SI + 0x2e]
0000:0fb5  2b 44 30           SUB AX,word ptr [SI + 0x30]
0000:0fb8  c3                 RET
0000:0fb9  2e c7 06 88 10 00 00  MOV word ptr CS:[0x1088],0x0
0000:0fc0  2e c7 06 8c 10 90 01  MOV word ptr CS:[0x108c],0x190
0000:0fc7  8b 46 74           MOV AX,word ptr [BP + 0x74]
0000:0fca  2b 86 84 00        SUB AX,word ptr [BP + 0x84]
0000:0fce  40                 INC AX
0000:0fcf  0b c0              OR AX,AX
0000:0fd1  7d 03              JGE 0x0000:0fd6
0000:0fd3  05 90 01           ADD AX,0x190
0000:0fd6  3d 90 01           CMP AX,0x190
0000:0fd9  7c 03              JL 0x0000:0fde
0000:0fdb  2d 90 01           SUB AX,0x190
0000:0fde  2e a3 8a 10        MOV CS:[0x108a],AX
0000:0fe2  8b 46 76           MOV AX,word ptr [BP + 0x76]
0000:0fe5  03 86 84 00        ADD AX,word ptr [BP + 0x84]
0000:0fe9  48                 DEC AX
0000:0fea  0b c0              OR AX,AX
0000:0fec  7d 03              JGE 0x0000:0ff1
0000:0fee  05 90 01           ADD AX,0x190
0000:0ff1  3d 90 01           CMP AX,0x190
0000:0ff4  7c 03              JL 0x0000:0ff9
0000:0ff6  2d 90 01           SUB AX,0x190
0000:0ff9  8b d0              MOV DX,AX
0000:0ffb  c1 e0 02           SHL AX,0x2
0000:0ffe  03 c2              ADD AX,DX
0000:1000  c1 e0 03           SHL AX,0x3
0000:1003  2e a3 86 10        MOV CS:[0x1086],AX
0000:1007  8b 86 82 00        MOV AX,word ptr [BP + 0x82]
0000:100b  89 86 84 00        MOV word ptr [BP + 0x84],AX
0000:100f  b0 70              MOV AL,0x70
0000:1011  e6 a2              OUT 0xa2,AL
0000:1013  2e a0 86 10        MOV AL,CS:[0x1086]
0000:1017  e6 a0              OUT 0xa0,AL
0000:1019  b0 71              MOV AL,0x71
0000:101b  e6 a2              OUT 0xa2,AL
0000:101d  2e a0 87 10        MOV AL,CS:[0x1087]
0000:1021  e6 a0              OUT 0xa0,AL
0000:1023  b0 72              MOV AL,0x72
0000:1025  e6 a2              OUT 0xa2,AL
0000:1027  2e a1 8a 10        MOV AX,CS:[0x108a]
0000:102b  c1 e0 04           SHL AX,0x4
0000:102e  e6 a0              OUT 0xa0,AL
0000:1030  b0 73              MOV AL,0x73
0000:1032  e6 a2              OUT 0xa2,AL
0000:1034  2e a1 8a 10        MOV AX,CS:[0x108a]
0000:1038  c1 e0 04           SHL AX,0x4
0000:103b  8a c4              MOV AL,AH
0000:103d  8a a6 b9 00        MOV AH,byte ptr [BP + 0xb9]
0000:1041  80 e4 40           AND AH,0x40
0000:1044  0a c4              OR AL,AH
0000:1046  e6 a0              OUT 0xa0,AL
0000:1048  b0 74              MOV AL,0x74
0000:104a  e6 a2              OUT 0xa2,AL
0000:104c  2e a0 88 10        MOV AL,CS:[0x1088]
0000:1050  e6 a0              OUT 0xa0,AL
0000:1052  b0 75              MOV AL,0x75
0000:1054  e6 a2              OUT 0xa2,AL
0000:1056  2e a0 89 10        MOV AL,CS:[0x1089]
0000:105a  e6 a0              OUT 0xa0,AL
0000:105c  b0 76              MOV AL,0x76
0000:105e  e6 a2              OUT 0xa2,AL
0000:1060  2e a1 8c 10        MOV AX,CS:[0x108c]
0000:1064  c1 e0 04           SHL AX,0x4
0000:1067  e6 a0              OUT 0xa0,AL
0000:1069  b0 77              MOV AL,0x77
0000:106b  e6 a2              OUT 0xa2,AL
0000:106d  2e a1 8c 10        MOV AX,CS:[0x108c]
0000:1071  c1 e0 04           SHL AX,0x4
0000:1074  8a c4              MOV AL,AH
0000:1076  8a a6 b9 00        MOV AH,byte ptr [BP + 0xb9]
0000:107a  80 e4 40           AND AH,0x40
0000:107d  0a c4              OR AL,AH
0000:107f  e6 a0              OUT 0xa0,AL
0000:1081  83 c7 04           ADD DI,0x4
0000:1084  c3                 RET
0000:108e  56                 PUSH SI
0000:108f  57                 PUSH DI
0000:1090  1e                 PUSH DS
0000:1091  8b 46 16           MOV AX,word ptr [BP + 0x16]
0000:1094  8e d8              MOV AX,DS
0000:1096  8b 46 76           MOV AX,word ptr [BP + 0x76]
0000:1099  3d 90 01           CMP AX,0x190
0000:109c  72 03              JC 0x0000:10a1
0000:109e  2d 90 01           SUB AX,0x190
0000:10a1  c1 e8 04           SHR AX,0x4
0000:10a4  ba 3c 00           MOV DX,0x3c
0000:10a7  f7 e2              MUL DX
0000:10a9  05 06 01           ADD AX,0x106
0000:10ac  8b f8              MOV DI,AX
0000:10ae  8b 46 7a           MOV AX,word ptr [BP + 0x7a]
0000:10b1  c1 e8 04           SHR AX,0x4
0000:10b4  ba 3c 00           MOV DX,0x3c
0000:10b7  f7 e2              MUL DX
0000:10b9  8b f0              MOV SI,AX
0000:10bb  b9 1e 00           MOV CX,0x1e
0000:10be  8b 04              MOV AX,word ptr [SI]
0000:10c0  83 c6 02           ADD SI,0x2
0000:10c3  0d 00 80           OR AX,0x8000
0000:10c6  2e 89 05           MOV word ptr CS:[DI],AX
0000:10c9  83 c7 02           ADD DI,0x2
0000:10cc  e2 f0              LOOP 0x0000:10be
0000:10ce  1f                 POP DS
0000:10cf  5f                 POP DI
0000:10d0  5e                 POP SI
0000:10d1  c3                 RET
0000:10d2  56                 PUSH SI
0000:10d3  57                 PUSH DI
0000:10d4  1e                 PUSH DS
0000:10d5  06                 PUSH ES
0000:10d6  33 db              XOR BX,BX
0000:10d8  b3 26              MOV BL,0x26
0000:10da  03 dd              ADD BX,BP
0000:10dc  36 8e 1f           MOV DS,word ptr SS:[BX]
0000:10df  be 06 01           MOV SI,0x106
0000:10e2  bf 0a 00           MOV DI,0xa
0000:10e5  ba 19 00           MOV DX,0x19
0000:10e8  52                 PUSH DX
0000:10e9  b9 1e 00           MOV CX,0x1e
0000:10ec  2e f7 04 00 c0     TEST word ptr CS:[SI],0xc000
0000:10f1  74 29              JZ 0x0000:111c
0000:10f3  8b de              MOV BX,SI
0000:10f5  2e 8b 04           MOV AX,word ptr CS:[SI]
0000:10f8  25 ff 01           AND AX,0x1ff
0000:10fb  c1 e0 05           SHL AX,0x5
0000:10fe  8b f0              MOV SI,AX
0000:1100  d1 e0              SHL AX,0x1
0000:1102  03 f0              ADD SI,AX
0000:1104  e8 1a 3a           CALL 0x0000:4b21
0000:1107  8b f3              MOV SI,BX
0000:1109  2e 8b 04           MOV AX,word ptr CS:[SI]
0000:110c  8b d8              MOV BX,AX
0000:110e  81 e3 ff 3f        AND BX,0x3fff
0000:1112  25 00 80           AND AX,0x8000
0000:1115  d1 e8              SHR AX,0x1
0000:1117  0b c3              OR AX,BX
0000:1119  2e 89 04           MOV word ptr CS:[SI],AX
0000:111c  83 c6 02           ADD SI,0x2
0000:111f  83 c7 02           ADD DI,0x2
0000:1122  49                 DEC CX
0000:1123  75 c7              JNZ 0x0000:10ec
0000:1125  81 c7 c4 04        ADD DI,0x4c4
0000:1129  5a                 POP DX
0000:112a  4a                 DEC DX
0000:112b  75 bb              JNZ 0x0000:10e8
0000:112d  07                 POP ES
0000:112e  1f                 POP DS
0000:112f  5f                 POP DI
0000:1130  5e                 POP SI
0000:1131  c3                 RET
0000:1132  57                 PUSH DI
0000:1133  55                 PUSH BP
0000:1134  1e                 PUSH DS
0000:1135  06                 PUSH ES
0000:1136  8b 4c 22           MOV CX,word ptr [SI + 0x22]
0000:1139  33 c0              XOR AX,AX
0000:113b  8a 44 4c           MOV AL,byte ptr [SI + 0x4c]
0000:113e  c1 e0 03           SHL AX,0x3
0000:1141  2b c8              SUB CX,AX
0000:1143  8b 54 24           MOV DX,word ptr [SI + 0x24]
0000:1146  33 c0              XOR AX,AX
0000:1148  8a 44 4d           MOV AL,byte ptr [SI + 0x4d]
0000:114b  c1 e0 03           SHL AX,0x3
0000:114e  2b d0              SUB DX,AX
0000:1150  8b c2              MOV AX,DX
0000:1152  03 46 76           ADD AX,word ptr [BP + 0x76]
0000:1155  3d 90 01           CMP AX,0x190
0000:1158  7c 03              JL 0x0000:115d
0000:115a  2d 90 01           SUB AX,0x190
0000:115d  c1 fa 04           SAR DX,0x4
0000:1160  8a f2              MOV DH,DL
0000:1162  8b f9              MOV DI,CX
0000:1164  8a d1              MOV DL,CL
0000:1166  c1 f9 04           SAR CX,0x4
0000:1169  8a e9              MOV CH,CL
0000:116b  22 54 4e           AND DL,byte ptr [SI + 0x4e]
0000:116e  2e 88 16 c3 12     MOV byte ptr CS:[0x12c3],DL
0000:1173  8a 54 4f           MOV DL,byte ptr [SI + 0x4f]
0000:1176  2e 88 16 c4 12     MOV byte ptr CS:[0x12c4],DL
0000:117b  8b d8              MOV BX,AX
0000:117d  c1 fb 04           SAR BX,0x4
0000:1180  8a cb              MOV CL,BL
0000:1182  c1 ff 03           SAR DI,0x3
0000:1185  c1 e0 04           SHL AX,0x4
0000:1188  03 f8              ADD DI,AX
0000:118a  c1 e0 02           SHL AX,0x2
0000:118d  03 f8              ADD DI,AX
0000:118f  8b 5c 38           MOV BX,word ptr [SI + 0x38]
0000:1192  8a 54 4d           MOV DL,byte ptr [SI + 0x4d]
0000:1195  0a d2              OR DL,DL
0000:1197  75 03              JNZ 0x0000:119c
0000:1199  e9 22 01           JMP 0x0000:12be
0000:119c  0a c9              OR CL,CL
0000:119e  7c 0a              JL 0x0000:11aa
0000:11a0  80 f9 19           CMP CL,0x19
0000:11a3  7c 08              JL 0x0000:11ad
0000:11a5  80 e9 19           SUB CL,0x19
0000:11a8  eb 03              JMP 0x0000:11ad
0000:11aa  80 c1 19           ADD CL,0x19
0000:11ad  0a f6              OR DH,DH
0000:11af  7d 03              JGE 0x0000:11b4
0000:11b1  e9 01 01           JMP 0x0000:12b5
0000:11b4  80 fe 18           CMP DH,0x18
0000:11b7  7c 03              JL 0x0000:11bc
0000:11b9  e9 ec 00           JMP 0x0000:12a8
0000:11bc  51                 PUSH CX
0000:11bd  52                 PUSH DX
0000:11be  57                 PUSH DI
0000:11bf  8a 54 4c           MOV DL,byte ptr [SI + 0x4c]
0000:11c2  0a d2              OR DL,DL
0000:11c4  75 03              JNZ 0x0000:11c9
0000:11c6  e9 dc 00           JMP 0x0000:12a5
0000:11c9  80 fd 04           CMP CH,0x4
0000:11cc  7d 03              JGE 0x0000:11d1
0000:11ce  e9 c9 00           JMP 0x0000:129a
0000:11d1  80 fd 23           CMP CH,0x23
0000:11d4  72 03              JC 0x0000:11d9
0000:11d6  e9 c1 00           JMP 0x0000:129a
0000:11d9  81 ff 00 7d        CMP DI,0x7d00
0000:11dd  72 04              JC 0x0000:11e3
0000:11df  81 ef 00 7d        SUB DI,0x7d00
0000:11e3  53                 PUSH BX
0000:11e4  56                 PUSH SI
0000:11e5  1e                 PUSH DS
0000:11e6  06                 PUSH ES
0000:11e7  8b 44 3a           MOV AX,word ptr [SI + 0x3a]
0000:11ea  8e d8              MOV AX,DS
0000:11ec  33 c0              XOR AX,AX
0000:11ee  26 8a 07           MOV AL,byte ptr ES:[BX]
0000:11f1  c1 e0 07           SHL AX,0x7
0000:11f4  8b f0              MOV SI,AX
0000:11f6  b8 00 a8           MOV AX,0xa800
0000:11f9  8e c0              MOV AX,ES
0000:11fb  8b da              MOV BX,DX
0000:11fd  51                 PUSH CX
0000:11fe  2e 8a 2e c4 12     MOV CH,byte ptr CS:[0x12c4]
0000:1203  2e 8a 0e c3 12     MOV CL,byte ptr CS:[0x12c3]
0000:1208  e8 10 36           CALL 0x0000:481b
0000:120b  59                 POP CX
0000:120c  8b d3              MOV DX,BX
0000:120e  33 c0              XOR AX,AX
0000:1210  8a c1              MOV AL,CL
0000:1212  b3 3c              MOV BL,0x3c
0000:1214  f6 e3              MUL BL
0000:1216  05 fc 00           ADD AX,0xfc
0000:1219  8b d8              MOV BX,AX
0000:121b  33 c0              XOR AX,AX
0000:121d  8a c5              MOV AL,CH
0000:121f  d1 e0              SHL AX,0x1
0000:1221  03 d8              ADD BX,AX
0000:1223  80 fd 05           CMP CH,0x5
0000:1226  7c 3d              JL 0x0000:1265
0000:1228  80 fa 01           CMP DL,0x1
0000:122b  74 16              JZ 0x0000:1243
0000:122d  2e 81 0f 00 80     OR word ptr CS:[BX],0x8000
0000:1232  80 f9 18           CMP CL,0x18
0000:1235  7c 04              JL 0x0000:123b
0000:1237  81 eb dc 05        SUB BX,0x5dc
0000:123b  2e 81 4f 3c 00 80  OR word ptr CS:[BX + 0x3c],0x8000
0000:1241  eb 53              JMP 0x0000:1296
0000:1243  2e 81 0f 00 80     OR word ptr CS:[BX],0x8000
0000:1248  2e 81 4f 02 00 80  OR word ptr CS:[BX + 0x2],0x8000
0000:124e  80 f9 18           CMP CL,0x18
0000:1251  7c 04              JL 0x0000:1257
0000:1253  81 eb dc 05        SUB BX,0x5dc
0000:1257  2e 81 4f 3c 00 80  OR word ptr CS:[BX + 0x3c],0x8000
0000:125d  2e 81 4f 3e 00 80  OR word ptr CS:[BX + 0x3e],0x8000
0000:1263  eb 31              JMP 0x0000:1296
0000:1265  80 fa 01           CMP DL,0x1
0000:1268  74 17              JZ 0x0000:1281
0000:126a  2e 81 4f 3e 00 80  OR word ptr CS:[BX + 0x3e],0x8000
0000:1270  80 f9 18           CMP CL,0x18
0000:1273  7c 04              JL 0x0000:1279
0000:1275  81 eb dc 05        SUB BX,0x5dc
0000:1279  2e 81 4f 7a 00 80  OR word ptr CS:[BX + 0x7a],0x8000
0000:127f  eb 15              JMP 0x0000:1296
0000:1281  2e 81 4f 02 00 80  OR word ptr CS:[BX + 0x2],0x8000
0000:1287  80 f9 18           CMP CL,0x18
0000:128a  7c 04              JL 0x0000:1290
0000:128c  81 eb dc 05        SUB BX,0x5dc
0000:1290  2e 81 4f 3e 00 80  OR word ptr CS:[BX + 0x3e],0x8000
0000:1296  07                 POP ES
0000:1297  1f                 POP DS
0000:1298  5e                 POP SI
0000:1299  5b                 POP BX
0000:129a  83 c7 02           ADD DI,0x2
0000:129d  43                 INC BX
0000:129e  fe c5              INC CH
0000:12a0  fe ca              DEC DL
0000:12a2  e9 1d ff           JMP 0x0000:11c2
0000:12a5  5f                 POP DI
0000:12a6  5a                 POP DX
0000:12a7  59                 POP CX
0000:12a8  fe c1              INC CL
0000:12aa  fe c6              INC DH
0000:12ac  fe ca              DEC DL
0000:12ae  81 c7 00 05        ADD DI,0x500
0000:12b2  e9 e0 fe           JMP 0x0000:1195
0000:12b5  33 c0              XOR AX,AX
0000:12b7  8a 44 4c           MOV AL,byte ptr [SI + 0x4c]
0000:12ba  03 d8              ADD BX,AX
0000:12bc  eb ea              JMP 0x0000:12a8
0000:12be  07                 POP ES
0000:12bf  1f                 POP DS
0000:12c0  5d                 POP BP
0000:12c1  5f                 POP DI
0000:12c2  c3                 RET
0000:12c5  56                 PUSH SI
0000:12c6  57                 PUSH DI
0000:12c7  06                 PUSH ES
0000:12c8  b8 00 a0           MOV AX,0xa000
0000:12cb  8e c0              MOV AX,ES
0000:12cd  2e 8b 3e 0e 14     MOV DI,word ptr CS:[0x140e]
0000:12d2  2e 8b 16 10 14     MOV DX,word ptr CS:[0x1410]
0000:12d7  33 c9              XOR CX,CX
0000:12d9  ac                 LODSB SI
0000:12da  3c f0              CMP AL,0xf0
0000:12dc  73 21              JNC 0x0000:12ff
0000:12de  8a c8              MOV CL,AL
0000:12e0  ad                 LODSW SI
0000:12e1  33 db              XOR BX,BX
0000:12e3  8a dc              MOV BL,AH
0000:12e5  32 e4              XOR AH,AH
0000:12e7  26 89 05           MOV word ptr ES:[DI],AX
0000:12ea  26 89 95 00 20     MOV word ptr ES:[DI + 0x2000],DX
0000:12ef  26 89 5d 02        MOV word ptr ES:[DI + 0x2],BX
0000:12f3  26 89 95 02 20     MOV word ptr ES:[DI + 0x2002],DX
0000:12f8  83 c7 04           ADD DI,0x4
0000:12fb  e2 ea              LOOP 0x0000:12e7
0000:12fd  eb da              JMP 0x0000:12d9
0000:12ff  3c f8              CMP AL,0xf8
0000:1301  74 2d              JZ 0x0000:1330
0000:1303  3c f9              CMP AL,0xf9
0000:1305  74 44              JZ 0x0000:134b
0000:1307  3c fa              CMP AL,0xfa
0000:1309  74 78              JZ 0x0000:1383
0000:130b  3c fb              CMP AL,0xfb
0000:130d  75 03              JNZ 0x0000:1312
0000:130f  e9 8d 00           JMP 0x0000:139f
0000:1312  3c fc              CMP AL,0xfc
0000:1314  75 03              JNZ 0x0000:1319
0000:1316  e9 b6 00           JMP 0x0000:13cf
0000:1319  3c fd              CMP AL,0xfd
0000:131b  75 03              JNZ 0x0000:1320
0000:131d  e9 ba 00           JMP 0x0000:13da
0000:1320  3c fe              CMP AL,0xfe
0000:1322  75 03              JNZ 0x0000:1327
0000:1324  e9 ca 00           JMP 0x0000:13f1
0000:1327  3c ff              CMP AL,0xff
0000:1329  75 03              JNZ 0x0000:132e
0000:132b  e9 d4 00           JMP 0x0000:1402
0000:132e  eb a9              JMP 0x0000:12d9
0000:1330  ad                 LODSW SI
0000:1331  2c 20              SUB AL,0x20
0000:1333  26 89 05           MOV word ptr ES:[DI],AX
0000:1336  26 89 95 00 20     MOV word ptr ES:[DI + 0x2000],DX
0000:133b  04 80              ADD AL,0x80
0000:133d  26 89 45 02        MOV word ptr ES:[DI + 0x2],AX
0000:1341  26 89 95 02 20     MOV word ptr ES:[DI + 0x2002],DX
0000:1346  83 c7 04           ADD DI,0x4
0000:1349  eb 8e              JMP 0x0000:12d9
0000:134b  ad                 LODSW SI
0000:134c  3c ff              CMP AL,0xff
0000:134e  74 2f              JZ 0x0000:137f
0000:1350  02 c0              ADD AL,AL
0000:1352  80 ec 1f           SUB AH,0x1f
0000:1355  78 06              JS 0x0000:135d
0000:1357  80 fc 61           CMP AH,0x61
0000:135a  80 d4 de           ADC AH,0xde
0000:135d  80 c4 a1           ADD AH,0xa1
0000:1360  14 1f              ADC AL,0x1f
0000:1362  25 7f 7f           AND AX,0x7f7f
0000:1365  2c 20              SUB AL,0x20
0000:1367  26 89 05           MOV word ptr ES:[DI],AX
0000:136a  26 89 95 00 20     MOV word ptr ES:[DI + 0x2000],DX
0000:136f  04 80              ADD AL,0x80
0000:1371  26 89 45 02        MOV word ptr ES:[DI + 0x2],AX
0000:1375  26 89 95 02 20     MOV word ptr ES:[DI + 0x2002],DX
0000:137a  83 c7 04           ADD DI,0x4
0000:137d  eb cc              JMP 0x0000:134b
0000:137f  4e                 DEC SI
0000:1380  e9 56 ff           JMP 0x0000:12d9
0000:1383  33 c0              XOR AX,AX
0000:1385  ac                 LODSB SI
0000:1386  8b f8              MOV DI,AX
0000:1388  03 f8              ADD DI,AX
0000:138a  33 c0              XOR AX,AX
0000:138c  ac                 LODSB SI
0000:138d  c1 e0 05           SHL AX,0x5
0000:1390  03 f8              ADD DI,AX
0000:1392  c1 e0 02           SHL AX,0x2
0000:1395  03 f8              ADD DI,AX
0000:1397  2e 89 3e 0c 14     MOV word ptr CS:[0x140c],DI
0000:139c  e9 3a ff           JMP 0x0000:12d9
0000:139f  33 db              XOR BX,BX
0000:13a1  8a 1c              MOV BL,byte ptr [SI]
0000:13a3  03 dd              ADD BX,BP
0000:13a5  33 c0              XOR AX,AX
0000:13a7  36 8a 07           MOV AL,byte ptr SS:[BX]
0000:13aa  8b f8              MOV DI,AX
0000:13ac  03 f8              ADD DI,AX
0000:13ae  33 db              XOR BX,BX
0000:13b0  8a 5c 01           MOV BL,byte ptr [SI + 0x1]
0000:13b3  03 dd              ADD BX,BP
0000:13b5  33 c0              XOR AX,AX
0000:13b7  36 8a 07           MOV AL,byte ptr SS:[BX]
0000:13ba  c1 e0 05           SHL AX,0x5
0000:13bd  03 f8              ADD DI,AX
0000:13bf  c1 e0 02           SHL AX,0x2
0000:13c2  03 f8              ADD DI,AX
0000:13c4  2e 89 3e 0c 14     MOV word ptr CS:[0x140c],DI
0000:13c9  83 c6 02           ADD SI,0x2
0000:13cc  e9 0a ff           JMP 0x0000:12d9
0000:13cf  ac                 LODSB SI
0000:13d0  8a d0              MOV DL,AL
0000:13d2  2e 89 16 10 14     MOV word ptr CS:[0x1410],DX
0000:13d7  e9 ff fe           JMP 0x0000:12d9
0000:13da  33 c0              XOR AX,AX
0000:13dc  ac                 LODSB SI
0000:13dd  8b c8              MOV CX,AX
0000:13df  26 89 95 00 20     MOV word ptr ES:[DI + 0x2000],DX
0000:13e4  26 89 95 02 20     MOV word ptr ES:[DI + 0x2002],DX
0000:13e9  83 c7 04           ADD DI,0x4
0000:13ec  e2 f1              LOOP 0x0000:13df
0000:13ee  e9 e8 fe           JMP 0x0000:12d9
0000:13f1  2e 8b 3e 0c 14     MOV DI,word ptr CS:[0x140c]
0000:13f6  81 c7 a0 00        ADD DI,0xa0
0000:13fa  2e 89 3e 0c 14     MOV word ptr CS:[0x140c],DI
0000:13ff  e9 d7 fe           JMP 0x0000:12d9
0000:1402  2e 89 3e 0e 14     MOV word ptr CS:[0x140e],DI
0000:1407  07                 POP ES
0000:1408  5f                 POP DI
0000:1409  5e                 POP SI
0000:140a  c3                 RET
0000:1412  56                 PUSH SI
0000:1413  57                 PUSH DI
0000:1414  06                 PUSH ES
0000:1415  8b fb              MOV DI,BX
0000:1417  8b f1              MOV SI,CX
0000:1419  33 db              XOR BX,BX
0000:141b  ba 00 a0           MOV DX,0xa000
0000:141e  8e c2              MOV DX,ES
0000:1420  3c 64              CMP AL,0x64
0000:1422  72 02              JC 0x0000:1426
0000:1424  b0 63              MOV AL,0x63
0000:1426  32 e4              XOR AH,AH
0000:1428  b1 0a              MOV CL,0xa
0000:142a  f6 f1              DIV CL
0000:142c  8b d0              MOV DX,AX
0000:142e  e8 23 01           CALL 0x0000:1554
0000:1431  83 c7 04           ADD DI,0x4
0000:1434  8a c6              MOV AL,DH
0000:1436  fe c3              INC BL
0000:1438  e8 19 01           CALL 0x0000:1554
0000:143b  07                 POP ES
0000:143c  5f                 POP DI
0000:143d  5e                 POP SI
0000:143e  c3                 RET
0000:143f  56                 PUSH SI
0000:1440  57                 PUSH DI
0000:1441  06                 PUSH ES
0000:1442  8b fb              MOV DI,BX
0000:1444  8b f1              MOV SI,CX
0000:1446  33 db              XOR BX,BX
0000:1448  ba 00 a0           MOV DX,0xa000
0000:144b  8e c2              MOV DX,ES
0000:144d  33 d2              XOR DX,DX
0000:144f  b9 10 27           MOV CX,0x2710
0000:1452  f7 f1              DIV CX
0000:1454  e8 fd 00           CALL 0x0000:1554
0000:1457  83 c7 04           ADD DI,0x4
0000:145a  8b c2              MOV AX,DX
0000:145c  33 d2              XOR DX,DX
0000:145e  b9 e8 03           MOV CX,0x3e8
0000:1461  f7 f1              DIV CX
0000:1463  e8 ee 00           CALL 0x0000:1554
0000:1466  83 c7 04           ADD DI,0x4
0000:1469  8b c2              MOV AX,DX
0000:146b  33 d2              XOR DX,DX
0000:146d  b9 64 00           MOV CX,0x64
0000:1470  f7 f1              DIV CX
0000:1472  e8 df 00           CALL 0x0000:1554
0000:1475  83 c7 04           ADD DI,0x4
0000:1478  8b c2              MOV AX,DX
0000:147a  33 d2              XOR DX,DX
0000:147c  b9 0a 00           MOV CX,0xa
0000:147f  f7 f1              DIV CX
0000:1481  e8 d0 00           CALL 0x0000:1554
0000:1484  83 c7 04           ADD DI,0x4
0000:1487  8b c2              MOV AX,DX
0000:1489  33 d2              XOR DX,DX
0000:148b  fe c3              INC BL
0000:148d  e8 c4 00           CALL 0x0000:1554
0000:1490  07                 POP ES
0000:1491  5f                 POP DI
0000:1492  5e                 POP SI
0000:1493  c3                 RET
0000:1494  56                 PUSH SI
0000:1495  57                 PUSH DI
0000:1496  06                 PUSH ES
0000:1497  81 fa f5 05        CMP DX,0x5f5
0000:149b  72 0d              JC 0x0000:14aa
0000:149d  77 05              JA 0x0000:14a4
0000:149f  3d f6 e0           CMP AX,0xe0f6
0000:14a2  76 06              JBE 0x0000:14aa
0000:14a4  ba f5 05           MOV DX,0x5f5
0000:14a7  b8 f6 e0           MOV AX,0xe0f6
0000:14aa  2e a3 4c 15        MOV CS:[0x154c],AX
0000:14ae  2e 89 16 4e 15     MOV word ptr CS:[0x154e],DX
0000:14b3  8b fb              MOV DI,BX
0000:14b5  8b f1              MOV SI,CX
0000:14b7  33 db              XOR BX,BX
0000:14b9  b9 10 27           MOV CX,0x2710
0000:14bc  8b c2              MOV AX,DX
0000:14be  33 d2              XOR DX,DX
0000:14c0  f7 f1              DIV CX
0000:14c2  2e a1 4c 15        MOV AX,CS:[0x154c]
0000:14c6  f7 f1              DIV CX
0000:14c8  2e a3 50 15        MOV CS:[0x1550],AX
0000:14cc  2e 89 16 52 15     MOV word ptr CS:[0x1552],DX
0000:14d1  b8 00 a0           MOV AX,0xa000
0000:14d4  8e c0              MOV AX,ES
0000:14d6  2e a1 50 15        MOV AX,CS:[0x1550]
0000:14da  33 d2              XOR DX,DX
0000:14dc  b9 e8 03           MOV CX,0x3e8
0000:14df  f7 f1              DIV CX
0000:14e1  e8 70 00           CALL 0x0000:1554
0000:14e4  83 c7 04           ADD DI,0x4
0000:14e7  8b c2              MOV AX,DX
0000:14e9  33 d2              XOR DX,DX
0000:14eb  b9 64 00           MOV CX,0x64
0000:14ee  f7 f1              DIV CX
0000:14f0  e8 61 00           CALL 0x0000:1554
0000:14f3  83 c7 04           ADD DI,0x4
0000:14f6  8b c2              MOV AX,DX
0000:14f8  33 d2              XOR DX,DX
0000:14fa  b9 0a 00           MOV CX,0xa
0000:14fd  f7 f1              DIV CX
0000:14ff  e8 52 00           CALL 0x0000:1554
0000:1502  83 c7 04           ADD DI,0x4
0000:1505  8b c2              MOV AX,DX
0000:1507  33 d2              XOR DX,DX
0000:1509  e8 48 00           CALL 0x0000:1554
0000:150c  83 c7 04           ADD DI,0x4
0000:150f  2e a1 52 15        MOV AX,CS:[0x1552]
0000:1513  33 d2              XOR DX,DX
0000:1515  b9 e8 03           MOV CX,0x3e8
0000:1518  f7 f1              DIV CX
0000:151a  e8 37 00           CALL 0x0000:1554
0000:151d  83 c7 04           ADD DI,0x4
0000:1520  8b c2              MOV AX,DX
0000:1522  33 d2              XOR DX,DX
0000:1524  b9 64 00           MOV CX,0x64
0000:1527  f7 f1              DIV CX
0000:1529  e8 28 00           CALL 0x0000:1554
0000:152c  83 c7 04           ADD DI,0x4
0000:152f  8b c2              MOV AX,DX
0000:1531  33 d2              XOR DX,DX
0000:1533  b9 0a 00           MOV CX,0xa
0000:1536  f7 f1              DIV CX
0000:1538  e8 19 00           CALL 0x0000:1554
0000:153b  83 c7 04           ADD DI,0x4
0000:153e  8b c2              MOV AX,DX
0000:1540  33 d2              XOR DX,DX
0000:1542  fe c3              INC BL
0000:1544  e8 0d 00           CALL 0x0000:1554
0000:1547  07                 POP ES
0000:1548  5f                 POP DI
0000:1549  5e                 POP SI
0000:154a  c3                 RET
0000:1554  50                 PUSH AX
0000:1555  0a db              OR BL,BL
0000:1557  75 04              JNZ 0x0000:155d
0000:1559  0a c0              OR AL,AL
0000:155b  74 1f              JZ 0x0000:157c
0000:155d  8a e0              MOV AH,AL
0000:155f  80 c4 21           ADD AH,0x21
0000:1562  b0 56              MOV AL,0x56
0000:1564  26 89 05           MOV word ptr ES:[DI],AX
0000:1567  0d 80 00           OR AX,0x80
0000:156a  26 89 45 02        MOV word ptr ES:[DI + 0x2],AX
0000:156e  26 89 b5 00 20     MOV word ptr ES:[DI + 0x2000],SI
0000:1573  26 89 b5 02 20     MOV word ptr ES:[DI + 0x2002],SI
0000:1578  fe c3              INC BL
0000:157a  eb 14              JMP 0x0000:1590
0000:157c  b8 20 20           MOV AX,0x2020
0000:157f  26 89 05           MOV word ptr ES:[DI],AX
0000:1582  26 89 45 02        MOV word ptr ES:[DI + 0x2],AX
0000:1586  26 89 b5 00 20     MOV word ptr ES:[DI + 0x2000],SI
0000:158b  26 89 b5 02 20     MOV word ptr ES:[DI + 0x2002],SI
0000:1590  58                 POP AX
0000:1591  c3                 RET
0000:1592  56                 PUSH SI
0000:1593  8b f1              MOV SI,CX
0000:1595  8b cb              MOV CX,BX
0000:1597  c1 e1 05           SHL CX,0x5
0000:159a  83 e9 02           SUB CX,0x2
0000:159d  8b d8              MOV BX,AX
0000:159f  fa                 CLI
0000:15a0  b0 0b              MOV AL,0xb
0000:15a2  e6 68              OUT 0x68,AL
0000:15a4  b4 1a              MOV AH,0x1a
0000:15a6  cd 18              INT 0x18
0000:15a8  83 c1 20           ADD CX,0x20
0000:15ab  42                 INC DX
0000:15ac  4e                 DEC SI
0000:15ad  75 f5              JNZ 0x0000:15a4
0000:15af  b0 0a              MOV AL,0xa
0000:15b1  e6 68              OUT 0x68,AL
0000:15b3  fb                 STI
0000:15b4  5e                 POP SI
0000:15b5  c3                 RET
0000:15b6  56                 PUSH SI
0000:15b7  57                 PUSH DI
0000:15b8  1e                 PUSH DS
0000:15b9  06                 PUSH ES
0000:15ba  8c d0              MOV AX,SS
0000:15bc  8e d8              MOV AX,DS
0000:15be  8e 46 24           MOV ES,word ptr [BP + 0x24]
0000:15c1  8b 76 56           MOV SI,word ptr [BP + 0x56]
0000:15c4  bf 0a 00           MOV DI,0xa
0000:15c7  b9 6e 00           MOV CX,0x6e
0000:15ca  f3 a5              MOVSW.REP ES:DI,SI
0000:15cc  33 db              XOR BX,BX
0000:15ce  8b 46 72           MOV AX,word ptr [BP + 0x72]
0000:15d1  26 89 07           MOV word ptr ES:[BX],AX
0000:15d4  8b 86 b2 00        MOV AX,word ptr [BP + 0xb2]
0000:15d8  26 89 47 02        MOV word ptr ES:[BX + 0x2],AX
0000:15dc  8b 46 62           MOV AX,word ptr [BP + 0x62]
0000:15df  26 89 47 04        MOV word ptr ES:[BX + 0x4],AX
0000:15e3  8a 86 c7 00        MOV AL,byte ptr [BP + 0xc7]
0000:15e7  26 88 47 06        MOV byte ptr ES:[BX + 0x6],AL
0000:15eb  8a 86 c9 00        MOV AL,byte ptr [BP + 0xc9]
0000:15ef  26 88 47 07        MOV byte ptr ES:[BX + 0x7],AL
0000:15f3  8a 86 b9 00        MOV AL,byte ptr [BP + 0xb9]
0000:15f7  24 04              AND AL,0x4
0000:15f9  26 88 47 08        MOV byte ptr ES:[BX + 0x8],AL
0000:15fd  8a 86 c0 00        MOV AL,byte ptr [BP + 0xc0]
0000:1601  26 88 47 09        MOV byte ptr ES:[BX + 0x9],AL
0000:1605  8c c8              MOV AX,CS
0000:1607  8e d8              MOV AX,DS
0000:1609  8c d0              MOV AX,SS
0000:160b  8e c0              MOV AX,ES
0000:160d  8b 5e 50           MOV BX,word ptr [BP + 0x50]
0000:1610  36 8b 77 06        MOV SI,word ptr SS:[BX + 0x6]
0000:1614  8b 7e 4e           MOV DI,word ptr [BP + 0x4e]
0000:1617  b9 28 00           MOV CX,0x28
0000:161a  f3 a5              MOVSW.REP ES:DI,SI
0000:161c  8b 46 2a           MOV AX,word ptr [BP + 0x2a]
0000:161f  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:1622  8c d0              MOV AX,SS
0000:1624  8e d8              MOV AX,DS
0000:1626  8e 46 24           MOV ES,word ptr [BP + 0x24]
0000:1629  33 db              XOR BX,BX
0000:162b  33 d2              XOR DX,DX
0000:162d  33 ff              XOR DI,DI
0000:162f  b8 00 01           MOV AX,0x100
0000:1632  b9 e6 00           MOV CX,0xe6
0000:1635  ff 5e 0c           CALLF [BP + 0xc]
0000:1638  8e 46 0a           MOV ES,word ptr [BP + 0xa]
0000:163b  bb 24 18           MOV BX,0x1824
0000:163e  26 c7 07 00 00     MOV word ptr ES:[BX],0x0
0000:1643  07                 POP ES
0000:1644  1f                 POP DS
0000:1645  5f                 POP DI
0000:1646  5e                 POP SI
0000:1647  c3                 RET
0000:1648  56                 PUSH SI
0000:1649  57                 PUSH DI
0000:164a  1e                 PUSH DS
0000:164b  06                 PUSH ES
0000:164c  8b 46 32           MOV AX,word ptr [BP + 0x32]
0000:164f  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:1652  b0 4c              MOV AL,0x4c
0000:1654  b4 24              MOV AH,0x24
0000:1656  bb 00 03           MOV BX,0x300
0000:1659  be 00 00           MOV SI,0x0
0000:165c  bf 00 00           MOV DI,0x0
0000:165f  ff 5e 0c           CALLF [BP + 0xc]
0000:1662  73 50              JNC 0x0000:16b4
0000:1664  8c c8              MOV AX,CS
0000:1666  8e d8              MOV AX,DS
0000:1668  8c d0              MOV AX,SS
0000:166a  8e c0              MOV AX,ES
0000:166c  be 48 19           MOV SI,0x1948
0000:166f  8b 7e 56           MOV DI,word ptr [BP + 0x56]
0000:1672  b9 6e 00           MOV CX,0x6e
0000:1675  f3 a5              MOVSW.REP ES:DI,SI
0000:1677  c7 46 72 00 00     MOV word ptr [BP + 0x72],0x0
0000:167c  c7 86 b2 00 02 00  MOV word ptr [BP + 0xb2],0x2
0000:1682  c7 46 62 02 00     MOV word ptr [BP + 0x62],0x2
0000:1687  c6 86 c7 00 01     MOV byte ptr [BP + 0xc7],0x1
0000:168c  c6 86 c9 00 07     MOV byte ptr [BP + 0xc9],0x7
0000:1691  80 a6 b9 00 fb     AND byte ptr [BP + 0xb9],0xfb
0000:1696  c6 86 c0 00 00     MOV byte ptr [BP + 0xc0],0x0
0000:169b  c6 86 c1 00 00     MOV byte ptr [BP + 0xc1],0x0
0000:16a0  8b 5e 56           MOV BX,word ptr [BP + 0x56]
0000:16a3  36 8b 07           MOV AX,word ptr SS:[BX]
0000:16a6  89 86 8a 00        MOV word ptr [BP + 0x8a],AX
0000:16aa  36 8b 47 02        MOV AX,word ptr SS:[BX + 0x2]
0000:16ae  89 86 8c 00        MOV word ptr [BP + 0x8c],AX
0000:16b2  eb 5e              JMP 0x0000:1712
0000:16b4  8e 5e 24           MOV DS,word ptr [BP + 0x24]
0000:16b7  8c d0              MOV AX,SS
0000:16b9  8e c0              MOV AX,ES
0000:16bb  be 0a 00           MOV SI,0xa
0000:16be  8b 7e 56           MOV DI,word ptr [BP + 0x56]
0000:16c1  b9 6e 00           MOV CX,0x6e
0000:16c4  f3 a5              MOVSW.REP ES:DI,SI
0000:16c6  33 db              XOR BX,BX
0000:16c8  8b 07              MOV AX,word ptr [BX]
0000:16ca  25 ff 7f           AND AX,0x7fff
0000:16cd  89 46 72           MOV word ptr [BP + 0x72],AX
0000:16d0  8b 47 02           MOV AX,word ptr [BX + 0x2]
0000:16d3  89 86 b2 00        MOV word ptr [BP + 0xb2],AX
0000:16d7  8b 47 04           MOV AX,word ptr [BX + 0x4]
0000:16da  89 46 62           MOV word ptr [BP + 0x62],AX
0000:16dd  8a 47 06           MOV AL,byte ptr [BX + 0x6]
0000:16e0  88 86 c7 00        MOV byte ptr [BP + 0xc7],AL
0000:16e4  8a 47 07           MOV AL,byte ptr [BX + 0x7]
0000:16e7  88 86 c9 00        MOV byte ptr [BP + 0xc9],AL
0000:16eb  8a 47 08           MOV AL,byte ptr [BX + 0x8]
0000:16ee  08 86 b9 00        OR byte ptr [BP + 0xb9],AL
0000:16f2  8a 47 09           MOV AL,byte ptr [BX + 0x9]
0000:16f5  88 86 c0 00        MOV byte ptr [BP + 0xc0],AL
0000:16f9  c6 86 c1 00 00     MOV byte ptr [BP + 0xc1],0x0
0000:16fe  8b 5e 56           MOV BX,word ptr [BP + 0x56]
0000:1701  36 8b 07           MOV AX,word ptr SS:[BX]
0000:1704  89 86 8a 00        MOV word ptr [BP + 0x8a],AX
0000:1708  36 8b 47 02        MOV AX,word ptr SS:[BX + 0x2]
0000:170c  89 86 8c 00        MOV word ptr [BP + 0x8c],AX
0000:1710  eb 00              JMP 0x0000:1712
0000:1712  07                 POP ES
0000:1713  1f                 POP DS
0000:1714  5f                 POP DI
0000:1715  5e                 POP SI
0000:1716  c3                 RET
0000:1717  f7 46 72 00 20     TEST word ptr [BP + 0x72],0x2000
0000:171c  75 03              JNZ 0x0000:1721
0000:171e  e9 04 01           JMP 0x0000:1825
0000:1721  50                 PUSH AX
0000:1722  53                 PUSH BX
0000:1723  52                 PUSH DX
0000:1724  ba 63 49           MOV DX,0x4963
0000:1727  bb 90 00           MOV BX,0x90
0000:172a  8b 46 5c           MOV AX,word ptr [BP + 0x5c]
0000:172d  e8 90 2c           CALL 0x0000:43c0
0000:1730  ba 64 49           MOV DX,0x4964
0000:1733  bb 30 01           MOV BX,0x130
0000:1736  8b 46 66           MOV AX,word ptr [BP + 0x66]
0000:1739  e8 84 2c           CALL 0x0000:43c0
0000:173c  ba 44 47           MOV DX,0x4744
0000:173f  bb d0 01           MOV BX,0x1d0
0000:1742  8b 46 74           MOV AX,word ptr [BP + 0x74]
0000:1745  e8 78 2c           CALL 0x0000:43c0
0000:1748  ba 70 73           MOV DX,0x7370
0000:174b  bb 70 02           MOV BX,0x270
0000:174e  8b 86 82 00        MOV AX,word ptr [BP + 0x82]
0000:1752  e8 6b 2c           CALL 0x0000:43c0
0000:1755  ba 70 6d           MOV DX,0x6d70
0000:1758  bb 10 03           MOV BX,0x310
0000:175b  8b 46 7c           MOV AX,word ptr [BP + 0x7c]
0000:175e  e8 5f 2c           CALL 0x0000:43c0
0000:1761  ba 44 5a           MOV DX,0x5a44
0000:1764  bb b0 03           MOV BX,0x3b0
0000:1767  8b 46 68           MOV AX,word ptr [BP + 0x68]
0000:176a  e8 53 2c           CALL 0x0000:43c0
0000:176d  ba 69 74           MOV DX,0x7469
0000:1770  bb 50 04           MOV BX,0x450
0000:1773  33 c0              XOR AX,AX
0000:1775  8a 86 b9 00        MOV AL,byte ptr [BP + 0xb9]
0000:1779  e8 04 2c           CALL 0x0000:4380
0000:177c  ba 78 4d           MOV DX,0x4d78
0000:177f  bb 90 05           MOV BX,0x590
0000:1782  8b 86 96 00        MOV AX,word ptr [BP + 0x96]
0000:1786  83 e8 48           SUB AX,0x48
0000:1789  83 e0 f0           AND AX,0xfff0
0000:178c  e8 f1 2b           CALL 0x0000:4380
0000:178f  ba 79 4d           MOV DX,0x4d79
0000:1792  bb 30 06           MOV BX,0x630
0000:1795  8b 86 98 00        MOV AX,word ptr [BP + 0x98]
0000:1799  83 e8 10           SUB AX,0x10
0000:179c  03 46 7a           ADD AX,word ptr [BP + 0x7a]
0000:179f  c1 e8 04           SHR AX,0x4
0000:17a2  e8 db 2b           CALL 0x0000:4380
0000:17a5  ba 4c 46           MOV DX,0x464c
0000:17a8  bb d0 06           MOV BX,0x6d0
0000:17ab  33 c0              XOR AX,AX
0000:17ad  8a 86 e0 00        MOV AL,byte ptr [BP + 0xe0]
0000:17b1  e8 0c 2c           CALL 0x0000:43c0
0000:17b4  e8 6f 00           CALL 0x0000:1826
0000:17b7  ba 63 41           MOV DX,0x4163
0000:17ba  bb b0 08           MOV BX,0x8b0
0000:17bd  e8 00 2c           CALL 0x0000:43c0
0000:17c0  ba 63 45           MOV DX,0x4563
0000:17c3  bb 50 09           MOV BX,0x950
0000:17c6  8b 86 ae 00        MOV AX,word ptr [BP + 0xae]
0000:17ca  e8 f3 2b           CALL 0x0000:43c0
0000:17cd  ba 77 45           MOV DX,0x4577
0000:17d0  bb f0 09           MOV BX,0x9f0
0000:17d3  8b 86 b0 00        MOV AX,word ptr [BP + 0xb0]
0000:17d7  e8 e6 2b           CALL 0x0000:43c0
0000:17da  ba 46 43           MOV DX,0x4346
0000:17dd  bb 30 0b           MOV BX,0xb30
0000:17e0  8b 46 72           MOV AX,word ptr [BP + 0x72]
0000:17e3  e8 9a 2b           CALL 0x0000:4380
0000:17e6  ba 53 54           MOV DX,0x5453
0000:17e9  bb d0 0b           MOV BX,0xbd0
0000:17ec  33 c0              XOR AX,AX
0000:17ee  8a 86 b9 00        MOV AL,byte ptr [BP + 0xb9]
0000:17f2  e8 8b 2b           CALL 0x0000:4380
0000:17f5  ba 43 55           MOV DX,0x5543
0000:17f8  bb 70 0c           MOV BX,0xc70
0000:17fb  33 c0              XOR AX,AX
0000:17fd  8a 86 df 00        MOV AL,byte ptr [BP + 0xdf]
0000:1801  e8 bc 2b           CALL 0x0000:43c0
0000:1804  ba 4b 48           MOV DX,0x484b
0000:1807  bb 10 0d           MOV BX,0xd10
0000:180a  33 c0              XOR AX,AX
0000:180c  8a 86 c2 00        MOV AL,byte ptr [BP + 0xc2]
0000:1810  e8 6d 2b           CALL 0x0000:4380
0000:1813  ba 53 48           MOV DX,0x4853
0000:1816  bb b0 0d           MOV BX,0xdb0
0000:1819  33 c0              XOR AX,AX
0000:181b  8a 86 c3 00        MOV AL,byte ptr [BP + 0xc3]
0000:181f  e8 5e 2b           CALL 0x0000:4380
0000:1822  5a                 POP DX
0000:1823  5b                 POP BX
0000:1824  58                 POP AX
0000:1825  c3                 RET
0000:1826  56                 PUSH SI
0000:1827  1e                 PUSH DS
0000:1828  8b 46 14           MOV AX,word ptr [BP + 0x14]
0000:182b  8e d8              MOV AX,DS
0000:182d  8b 76 52           MOV SI,word ptr [BP + 0x52]
0000:1830  bb 52 00           MOV BX,0x52
0000:1833  b9 00 01           MOV CX,0x100
0000:1836  ba 00 00           MOV DX,0x0
0000:1839  83 3c 00           CMP word ptr [SI],0x0
0000:183c  74 01              JZ 0x0000:183f
0000:183e  42                 INC DX
0000:183f  03 f3              ADD SI,BX
0000:1841  e2 f6              LOOP 0x0000:1839
0000:1843  8b c2              MOV AX,DX
0000:1845  1f                 POP DS
0000:1846  5e                 POP SI
0000:1847  c3                 RET
0000:1a30  83 3c 00           CMP word ptr [SI],0x0
0000:1a33  74 07              JZ 0x0000:1a3c
0000:1a35  53                 PUSH BX
0000:1a36  51                 PUSH CX
0000:1a37  ff 54 02           CALL word ptr [SI + 0x2]
0000:1a3a  59                 POP CX
0000:1a3b  5b                 POP BX
0000:1a3c  03 f3              ADD SI,BX
0000:1a3e  e2 f0              LOOP 0x0000:1a30
0000:1a40  c3                 RET
0000:1a41  83 3c 00           CMP word ptr [SI],0x0
0000:1a44  74 06              JZ 0x0000:1a4c
0000:1a46  03 f3              ADD SI,BX
0000:1a48  e2 f7              LOOP 0x0000:1a41
0000:1a4a  f9                 STC
0000:1a4b  c3                 RET
0000:1a4c  89 04              MOV word ptr [SI],AX
0000:1a4e  89 54 02           MOV word ptr [SI + 0x2],DX
0000:1a51  f8                 CLC
0000:1a52  c3                 RET
0000:1a53  50                 PUSH AX
0000:1a54  52                 PUSH DX
0000:1a55  8b c3              MOV AX,BX
0000:1a57  49                 DEC CX
0000:1a58  f7 e1              MUL CX
0000:1a5a  03 f0              ADD SI,AX
0000:1a5c  5a                 POP DX
0000:1a5d  58                 POP AX
0000:1a5e  83 3c 00           CMP word ptr [SI],0x0
0000:1a61  74 06              JZ 0x0000:1a69
0000:1a63  2b f3              SUB SI,BX
0000:1a65  e2 f7              LOOP 0x0000:1a5e
0000:1a67  f9                 STC
0000:1a68  c3                 RET
0000:1a69  89 04              MOV word ptr [SI],AX
0000:1a6b  89 54 02           MOV word ptr [SI + 0x2],DX
0000:1a6e  f8                 CLC
0000:1a6f  c3                 RET
0000:1a70  89 44 06           MOV word ptr [SI + 0x6],AX
0000:1a73  8f 44 02           POP word ptr [SI + 0x2]
0000:1a76  ff 64 02           JMP word ptr [SI + 0x2]
0000:1a79  58                 POP AX
0000:1a7a  83 7c 06 00        CMP word ptr [SI + 0x6],0x0
0000:1a7e  74 06              JZ 0x0000:1a86
0000:1a80  ff 4c 06           DEC word ptr [SI + 0x6]
0000:1a83  74 01              JZ 0x0000:1a86
0000:1a85  c3                 RET
0000:1a86  89 44 02           MOV word ptr [SI + 0x2],AX
0000:1a89  c3                 RET
0000:1a8a  53                 PUSH BX
0000:1a8b  57                 PUSH DI
0000:1a8c  56                 PUSH SI
0000:1a8d  06                 PUSH ES
0000:1a8e  8c d8              MOV AX,DS
0000:1a90  8e c0              MOV AX,ES
0000:1a92  bb 00 01           MOV BX,0x100
0000:1a95  85 14              TEST word ptr [SI],DX
0000:1a97  74 09              JZ 0x0000:1aa2
0000:1a99  8b fe              MOV DI,SI
0000:1a9b  33 c0              XOR AX,AX
0000:1a9d  b9 29 00           MOV CX,0x29
0000:1aa0  f3 ab              STOSW.REP ES:DI
0000:1aa2  83 c6 52           ADD SI,0x52
0000:1aa5  4b                 DEC BX
0000:1aa6  75 ed              JNZ 0x0000:1a95
0000:1aa8  07                 POP ES
0000:1aa9  5e                 POP SI
0000:1aaa  5f                 POP DI
0000:1aab  5b                 POP BX
0000:1aac  c3                 RET
0000:1aad  57                 PUSH DI
0000:1aae  06                 PUSH ES
0000:1aaf  8c d8              MOV AX,DS
0000:1ab1  8e c0              MOV AX,ES
0000:1ab3  8b fe              MOV DI,SI
0000:1ab5  33 c0              XOR AX,AX
0000:1ab7  b9 29 00           MOV CX,0x29
0000:1aba  f3 ab              STOSW.REP ES:DI
0000:1abc  07                 POP ES
0000:1abd  5f                 POP DI
0000:1abe  c3                 RET
0000:1abf  06                 PUSH ES
0000:1ac0  8c d8              MOV AX,DS
0000:1ac2  8e c0              MOV AX,ES
0000:1ac4  d1 eb              SHR BX,0x1
0000:1ac6  33 c0              XOR AX,AX
0000:1ac8  3b f7              CMP SI,DI
0000:1aca  75 06              JNZ 0x0000:1ad2
0000:1acc  03 fb              ADD DI,BX
0000:1ace  03 fb              ADD DI,BX
0000:1ad0  eb 08              JMP 0x0000:1ada
0000:1ad2  8b d1              MOV DX,CX
0000:1ad4  8b cb              MOV CX,BX
0000:1ad6  f3 ab              STOSW.REP ES:DI
0000:1ad8  8b ca              MOV CX,DX
0000:1ada  e2 ec              LOOP 0x0000:1ac8
0000:1adc  07                 POP ES
0000:1add  c3                 RET
0000:1ade  33 c0              XOR AX,AX
0000:1ae0  f3 ab              STOSW.REP ES:DI
0000:1ae2  c3                 RET
0000:1af8  b8 01 00           MOV AX,0x1
0000:1afb  e8 72 ff           CALL 0x0000:1a70
0000:1afe  8e 44 24           MOV ES,word ptr [SI + 0x24]
0000:1b01  8b 6c 22           MOV BP,word ptr [SI + 0x22]
0000:1b04  8b 7c 26           MOV DI,word ptr [SI + 0x26]
0000:1b07  26 8a 66 00        MOV AH,byte ptr ES:[BP + 0x0]
0000:1b0b  45                 INC BP
0000:1b0c  80 fc 61           CMP AH,0x61
0000:1b0f  72 17              JC 0x0000:1b28
0000:1b11  80 fc 7a           CMP AH,0x7a
0000:1b14  77 12              JA 0x0000:1b28
0000:1b16  33 db              XOR BX,BX
0000:1b18  8a dc              MOV BL,AH
0000:1b1a  80 eb 61           SUB BL,0x61
0000:1b1d  d1 e3              SHL BX,0x1
0000:1b1f  81 c3 66 1c        ADD BX,0x1c66
0000:1b23  2e ff 17           CALL word ptr CS:[BX]
0000:1b26  eb df              JMP 0x0000:1b07
0000:1b28  83 7c 2e 00        CMP word ptr [SI + 0x2e],0x0
0000:1b2c  74 5a              JZ 0x0000:1b88
0000:1b2e  83 7c 30 00        CMP word ptr [SI + 0x30],0x0
0000:1b32  74 33              JZ 0x0000:1b67
0000:1b34  ff 4c 30           DEC word ptr [SI + 0x30]
0000:1b37  74 2e              JZ 0x0000:1b67
0000:1b39  2e 8b 1e 04 01     MOV BX,word ptr CS:[0x104]
0000:1b3e  80 7c 46 00        CMP byte ptr [SI + 0x46],0x0
0000:1b42  75 44              JNZ 0x0000:1b88
0000:1b44  36 f7 47 6c 00 00  TEST word ptr SS:[BX + 0x6c],0x0
0000:1b4a  75 08              JNZ 0x0000:1b54
0000:1b4c  36 f7 47 6e 3f 00  TEST word ptr SS:[BX + 0x6e],0x3f
0000:1b52  74 06              JZ 0x0000:1b5a
0000:1b54  c6 44 46 01        MOV byte ptr [SI + 0x46],0x1
0000:1b58  eb 0d              JMP 0x0000:1b67
0000:1b5a  4d                 DEC BP
0000:1b5b  89 6c 22           MOV word ptr [SI + 0x22],BP
0000:1b5e  89 7c 26           MOV word ptr [SI + 0x26],DI
0000:1b61  2e 8b 2e 04 01     MOV BP,word ptr CS:[0x104]
0000:1b66  c3                 RET
0000:1b67  8b 5c 2e           MOV BX,word ptr [SI + 0x2e]
0000:1b6a  89 5c 30           MOV word ptr [SI + 0x30],BX
0000:1b6d  26 8a 46 00        MOV AL,byte ptr ES:[BP + 0x0]
0000:1b71  3d 40 81           CMP AX,0x8140
0000:1b74  74 12              JZ 0x0000:1b88
0000:1b76  80 fc a0           CMP AH,0xa0
0000:1b79  74 0d              JZ 0x0000:1b88
0000:1b7b  50                 PUSH AX
0000:1b7c  8a 44 47           MOV AL,byte ptr [SI + 0x47]
0000:1b7f  3c ff              CMP AL,0xff
0000:1b81  74 04              JZ 0x0000:1b87
0000:1b83  b4 01              MOV AH,0x1
0000:1b85  cd 7f              INT 0x7f
0000:1b87  58                 POP AX
0000:1b88  80 fc a0           CMP AH,0xa0
0000:1b8b  72 16              JC 0x0000:1ba3
0000:1b8d  80 fc df           CMP AH,0xdf
0000:1b90  77 11              JA 0x0000:1ba3
0000:1b92  33 db              XOR BX,BX
0000:1b94  8a dc              MOV BL,AH
0000:1b96  d1 e3              SHL BX,0x1
0000:1b98  81 c3 a6 1a        ADD BX,0x1aa6
0000:1b9c  2e 8b 07           MOV AX,word ptr CS:[BX]
0000:1b9f  86 c4              XCHG AH,AL
0000:1ba1  eb 05              JMP 0x0000:1ba8
0000:1ba3  26 8a 46 00        MOV AL,byte ptr ES:[BP + 0x0]
0000:1ba7  45                 INC BP
0000:1ba8  1e                 PUSH DS
0000:1ba9  06                 PUSH ES
0000:1baa  50                 PUSH AX
0000:1bab  8a 4c 44           MOV CL,byte ptr [SI + 0x44]
0000:1bae  8a 6c 45           MOV CH,byte ptr [SI + 0x45]
0000:1bb1  8c c8              MOV AX,CS
0000:1bb3  8e d8              MOV AX,DS
0000:1bb5  2e 8b 1e 04 01     MOV BX,word ptr CS:[0x104]
0000:1bba  36 8a 87 ba 00     MOV AL,byte ptr SS:[BX + 0xba]
0000:1bbf  e6 a6              OUT 0xa6,AL
0000:1bc1  58                 POP AX
0000:1bc2  50                 PUSH AX
0000:1bc3  51                 PUSH CX
0000:1bc4  56                 PUSH SI
0000:1bc5  57                 PUSH DI
0000:1bc6  e8 fe 08           CALL 0x0000:24c7
0000:1bc9  5f                 POP DI
0000:1bca  5e                 POP SI
0000:1bcb  59                 POP CX
0000:1bcc  2e 8b 1e 04 01     MOV BX,word ptr CS:[0x104]
0000:1bd1  36 8a 87 ba 00     MOV AL,byte ptr SS:[BX + 0xba]
0000:1bd6  34 01              XOR AL,0x1
0000:1bd8  e6 a6              OUT 0xa6,AL
0000:1bda  58                 POP AX
0000:1bdb  56                 PUSH SI
0000:1bdc  e8 e8 08           CALL 0x0000:24c7
0000:1bdf  5e                 POP SI
0000:1be0  07                 POP ES
0000:1be1  1f                 POP DS
0000:1be2  e9 22 ff           JMP 0x0000:1b07
0000:2020  2e 8b 2e 04 01     MOV BP,word ptr CS:[0x104]
0000:2025  c3                 RET
0000:20a2  7c 28              JL 0x0000:20cc
0000:20a4  89 7c 2a           MOV word ptr [SI + 0x2a],DI
0000:20a7  89 7c 26           MOV word ptr [SI + 0x26],DI
0000:20aa  c7 44 30 00 00     MOV word ptr [SI + 0x30],0x0
0000:20af  ff 44 22           INC word ptr [SI + 0x22]
0000:20b2  e9 43 fa           JMP 0x0000:1af8
0000:20cc  8e d8              MOV AX,DS
0000:20ce  ff 5e 0c           CALLF [BP + 0xc]
0000:20d1  1f                 POP DS
0000:20d2  5d                 POP BP
0000:20d3  5f                 POP DI
0000:20d4  5e                 POP SI
0000:20d5  c3                 RET
0000:24c7  80 e5 7f           AND CH,0x7f
0000:24ca  3d 40 85           CMP AX,0x8540
0000:24cd  72 08              JC 0x0000:24d7
0000:24cf  3d 9f 86           CMP AX,0x869f
0000:24d2  77 03              JA 0x0000:24d7
0000:24d4  80 cd 80           OR CH,0x80
0000:24d7  02 e4              ADD AH,AH
0000:24d9  2c 1f              SUB AL,0x1f
0000:24db  78 04              JS 0x0000:24e1
0000:24dd  3c 61              CMP AL,0x61
0000:24df  14 de              ADC AL,0xde
0000:24e1  05 a1 1f           ADD AX,0x1fa1
0000:24e4  25 7f 7f           AND AX,0x7f7f
0000:24e7  8b d0              MOV DX,AX
0000:24e9  51                 PUSH CX
0000:24ea  e8 83 08           CALL 0x0000:2d70
0000:24ed  f6 c5 80           TEST CH,0x80
0000:24f0  74 0f              JZ 0x0000:2501
0000:24f2  b9 10 00           MOV CX,0x10
0000:24f5  bb a5 2d           MOV BX,0x2da5
0000:24f8  32 c0              XOR AL,AL
0000:24fa  88 07              MOV byte ptr [BX],AL
0000:24fc  83 c3 02           ADD BX,0x2
0000:24ff  e2 f9              LOOP 0x0000:24fa
0000:2501  59                 POP CX
0000:2502  f6 c5 30           TEST CH,0x30
0000:2505  74 0d              JZ 0x0000:2514
0000:2507  f6 c5 20           TEST CH,0x20
0000:250a  74 05              JZ 0x0000:2511
0000:250c  e8 b2 00           CALL 0x0000:25c1
0000:250f  eb 03              JMP 0x0000:2514
0000:2511  e8 88 00           CALL 0x0000:259c
0000:2514  f6 c5 03           TEST CH,0x3
0000:2517  75 13              JNZ 0x0000:252c
0000:2519  f6 c5 0c           TEST CH,0xc
0000:251c  75 03              JNZ 0x0000:2521
0000:251e  e9 bb 00           JMP 0x0000:25dc
0000:2521  f6 c5 04           TEST CH,0x4
0000:2524  75 03              JNZ 0x0000:2529
0000:2526  e9 dd 00           JMP 0x0000:2606
0000:2529  e9 17 01           JMP 0x0000:2643
0000:252c  f6 c5 02           TEST CH,0x2
0000:252f  74 18              JZ 0x0000:2549
0000:2531  f6 c5 01           TEST CH,0x1
0000:2534  74 26              JZ 0x0000:255c
0000:2536  f6 c5 0c           TEST CH,0xc
0000:2539  75 03              JNZ 0x0000:253e
0000:253b  e9 13 05           JMP 0x0000:2a51
0000:253e  f6 c5 04           TEST CH,0x4
0000:2541  75 03              JNZ 0x0000:2546
0000:2543  e9 8a 05           JMP 0x0000:2ad0
0000:2546  e9 86 06           JMP 0x0000:2bcf
0000:2549  f6 c5 0c           TEST CH,0xc
0000:254c  75 03              JNZ 0x0000:2551
0000:254e  e9 39 01           JMP 0x0000:268a
0000:2551  f6 c5 04           TEST CH,0x4
0000:2554  75 03              JNZ 0x0000:2559
0000:2556  e9 60 01           JMP 0x0000:26b9
0000:2559  e9 a7 01           JMP 0x0000:2703
0000:255c  f6 c5 0c           TEST CH,0xc
0000:255f  75 03              JNZ 0x0000:2564
0000:2561  e9 fd 01           JMP 0x0000:2761
0000:2564  f6 c5 04           TEST CH,0x4
0000:2567  75 03              JNZ 0x0000:256c
0000:2569  e9 6f 02           JMP 0x0000:27db
0000:256c  e9 58 03           JMP 0x0000:28c7
0000:256f  32 c0              XOR AL,AL
0000:2571  fa                 CLI
0000:2572  e6 7c              OUT 0x7c,AL
0000:2574  fb                 STI
0000:2575  c3                 RET
0000:2576  b0 c0              MOV AL,0xc0
0000:2578  fa                 CLI
0000:2579  e6 7c              OUT 0x7c,AL
0000:257b  33 c0              XOR AX,AX
0000:257d  d0 ea              SHR DL,0x1
0000:257f  1a c0              SBB AL,AL
0000:2581  e6 7e              OUT 0x7e,AL
0000:2583  8a c4              MOV AL,AH
0000:2585  d0 ea              SHR DL,0x1
0000:2587  1a c0              SBB AL,AL
0000:2589  e6 7e              OUT 0x7e,AL
0000:258b  8a c4              MOV AL,AH
0000:258d  d0 ea              SHR DL,0x1
0000:258f  1a c0              SBB AL,AL
0000:2591  e6 7e              OUT 0x7e,AL
0000:2593  8a c4              MOV AL,AH
0000:2595  d0 ea              SHR DL,0x1
0000:2597  1a c0              SBB AL,AL
0000:2599  e6 7e              OUT 0x7e,AL
0000:259b  c3                 RET
0000:259c  51                 PUSH CX
0000:259d  be a4 2d           MOV SI,0x2da4
0000:25a0  33 c0              XOR AX,AX
0000:25a2  b9 10 00           MOV CX,0x10
0000:25a5  ad                 LODSW SI
0000:25a6  8b d0              MOV DX,AX
0000:25a8  8b d8              MOV BX,AX
0000:25aa  d0 e6              SHL DH,0x1
0000:25ac  d0 d2              RCL DL,0x1
0000:25ae  d0 eb              SHR BL,0x1
0000:25b0  d0 df              RCR BH,0x1
0000:25b2  f7 d3              NOT BX
0000:25b4  0b d8              OR BX,AX
0000:25b6  23 d3              AND DX,BX
0000:25b8  0b c2              OR AX,DX
0000:25ba  89 44 fe           MOV word ptr [SI + -0x2],AX
0000:25bd  e2 e6              LOOP 0x0000:25a5
0000:25bf  59                 POP CX
0000:25c0  c3                 RET
0000:25c1  8b d9              MOV BX,CX
0000:25c3  be a4 2d           MOV SI,0x2da4
0000:25c6  33 c0              XOR AX,AX
0000:25c8  b9 10 00           MOV CX,0x10
0000:25cb  ad                 LODSW SI
0000:25cc  8b d0              MOV DX,AX
0000:25ce  d0 e4              SHL AH,0x1
0000:25d0  d0 d0              RCL AL,0x1
0000:25d2  0b c2              OR AX,DX
0000:25d4  89 44 fe           MOV word ptr [SI + -0x2],AX
0000:25d7  e2 f2              LOOP 0x0000:25cb
0000:25d9  8b cb              MOV CX,BX
0000:25db  c3                 RET
0000:25dc  8b d9              MOV BX,CX
0000:25de  8a d1              MOV DL,CL
0000:25e0  e8 93 ff           CALL 0x0000:2576
0000:25e3  b8 00 a8           MOV AX,0xa800
0000:25e6  8e c0              MOV AX,ES
0000:25e8  be a4 2d           MOV SI,0x2da4
0000:25eb  b9 10 00           MOV CX,0x10
0000:25ee  a5                 MOVSW ES:DI,SI
0000:25ef  83 c7 4e           ADD DI,0x4e
0000:25f2  e2 fa              LOOP 0x0000:25ee
0000:25f4  81 ef fe 04        SUB DI,0x4fe
0000:25f8  8b cb              MOV CX,BX
0000:25fa  f6 c5 80           TEST CH,0x80
0000:25fd  75 03              JNZ 0x0000:2602
0000:25ff  e9 6d ff           JMP 0x0000:256f
0000:2602  4f                 DEC DI
0000:2603  e9 69 ff           JMP 0x0000:256f
0000:2606  8b d9              MOV BX,CX
0000:2608  8a d1              MOV DL,CL
0000:260a  c0 ea 04           SHR DL,0x4
0000:260d  e8 66 ff           CALL 0x0000:2576
0000:2610  b8 00 a8           MOV AX,0xa800
0000:2613  8e c0              MOV AX,ES
0000:2615  be a4 2d           MOV SI,0x2da4
0000:2618  b9 10 00           MOV CX,0x10
0000:261b  33 d2              XOR DX,DX
0000:261d  ad                 LODSW SI
0000:261e  d0 e8              SHR AL,0x1
0000:2620  d0 dc              RCR AH,0x1
0000:2622  d0 da              RCR DL,0x1
0000:2624  d0 e8              SHR AL,0x1
0000:2626  d0 dc              RCR AH,0x1
0000:2628  d0 da              RCR DL,0x1
0000:262a  26 88 75 4f        MOV byte ptr ES:[DI + 0x4f],DH
0000:262e  26 89 45 50        MOV word ptr ES:[DI + 0x50],AX
0000:2632  26 88 55 52        MOV byte ptr ES:[DI + 0x52],DL
0000:2636  83 c7 50           ADD DI,0x50
0000:2639  e2 e0              LOOP 0x0000:261b
0000:263b  81 ef 00 05        SUB DI,0x500
0000:263f  8b cb              MOV CX,BX
0000:2641  eb 99              JMP 0x0000:25dc
0000:2643  51                 PUSH CX
0000:2644  8a d1              MOV DL,CL
0000:2646  c0 ea 04           SHR DL,0x4
0000:2649  e8 2a ff           CALL 0x0000:2576
0000:264c  b8 00 a8           MOV AX,0xa800
0000:264f  8e c0              MOV AX,ES
0000:2651  be a4 2d           MOV SI,0x2da4
0000:2654  b9 10 00           MOV CX,0x10
0000:2657  33 d2              XOR DX,DX
0000:2659  ad                 LODSW SI
0000:265a  8b d8              MOV BX,AX
0000:265c  26 89 45 b0        MOV word ptr ES:[DI + -0x50],AX
0000:2660  26 89 45 50        MOV word ptr ES:[DI + 0x50],AX
0000:2664  d0 e8              SHR AL,0x1
0000:2666  d0 dc              RCR AH,0x1
0000:2668  d0 da              RCR DL,0x1
0000:266a  d0 e7              SHL BH,0x1
0000:266c  d0 d3              RCL BL,0x1
0000:266e  d0 d6              RCL DH,0x1
0000:2670  0b c3              OR AX,BX
0000:2672  26 88 75 ff        MOV byte ptr ES:[DI + -0x1],DH
0000:2676  26 89 05           MOV word ptr ES:[DI],AX
0000:2679  26 88 55 02        MOV byte ptr ES:[DI + 0x2],DL
0000:267d  83 c7 50           ADD DI,0x50
0000:2680  e2 d5              LOOP 0x0000:2657
0000:2682  81 ef 00 05        SUB DI,0x500
0000:2686  59                 POP CX
0000:2687  e9 52 ff           JMP 0x0000:25dc
0000:268a  8b d9              MOV BX,CX
0000:268c  8a d1              MOV DL,CL
0000:268e  e8 e5 fe           CALL 0x0000:2576
0000:2691  b8 00 a8           MOV AX,0xa800
0000:2694  8e c0              MOV AX,ES
0000:2696  be a4 2d           MOV SI,0x2da4
0000:2699  b9 10 00           MOV CX,0x10
0000:269c  ad                 LODSW SI
0000:269d  ab                 STOSW ES:DI
0000:269e  83 c7 4e           ADD DI,0x4e
0000:26a1  ab                 STOSW ES:DI
0000:26a2  83 c7 4e           ADD DI,0x4e
0000:26a5  e2 f5              LOOP 0x0000:269c
0000:26a7  81 ef fe 09        SUB DI,0x9fe
0000:26ab  8b cb              MOV CX,BX
0000:26ad  f6 c5 80           TEST CH,0x80
0000:26b0  75 03              JNZ 0x0000:26b5
0000:26b2  e9 ba fe           JMP 0x0000:256f
0000:26b5  4f                 DEC DI
0000:26b6  e9 b6 fe           JMP 0x0000:256f
0000:26b9  8b d9              MOV BX,CX
0000:26bb  8a d1              MOV DL,CL
0000:26bd  c0 ea 04           SHR DL,0x4
0000:26c0  e8 b3 fe           CALL 0x0000:2576
0000:26c3  b8 00 a8           MOV AX,0xa800
0000:26c6  8e c0              MOV AX,ES
0000:26c8  be a4 2d           MOV SI,0x2da4
0000:26cb  b9 10 00           MOV CX,0x10
0000:26ce  33 d2              XOR DX,DX
0000:26d0  ad                 LODSW SI
0000:26d1  d0 e8              SHR AL,0x1
0000:26d3  d0 dc              RCR AH,0x1
0000:26d5  d0 da              RCR DL,0x1
0000:26d7  26 88 b5 9f 00     MOV byte ptr ES:[DI + 0x9f],DH
0000:26dc  26 88 b5 f9 00     MOV byte ptr ES:[DI + 0xf9],DH
0000:26e1  26 89 85 a0 00     MOV word ptr ES:[DI + 0xa0],AX
0000:26e6  26 89 85 f0 00     MOV word ptr ES:[DI + 0xf0],AX
0000:26eb  26 88 95 a2 00     MOV byte ptr ES:[DI + 0xa2],DL
0000:26f0  26 88 95 f2 00     MOV byte ptr ES:[DI + 0xf2],DL
0000:26f5  81 c7 a0 00        ADD DI,0xa0
0000:26f9  e2 d3              LOOP 0x0000:26ce
0000:26fb  81 ef 00 0a        SUB DI,0xa00
0000:26ff  8b cb              MOV CX,BX
0000:2701  eb 87              JMP 0x0000:268a
0000:2703  51                 PUSH CX
0000:2704  8a d1              MOV DL,CL
0000:2706  c0 ea 04           SHR DL,0x4
0000:2709  e8 6a fe           CALL 0x0000:2576
0000:270c  b8 00 a8           MOV AX,0xa800
0000:270f  8e c0              MOV AX,ES
0000:2711  be a4 2d           MOV SI,0x2da4
0000:2714  b9 10 00           MOV CX,0x10
0000:2717  33 d2              XOR DX,DX
0000:2719  ad                 LODSW SI
0000:271a  8b d8              MOV BX,AX
0000:271c  26 89 85 60 ff     MOV word ptr ES:[DI + 0xff60],AX
0000:2721  26 89 45 b0        MOV word ptr ES:[DI + -0x50],AX
0000:2725  26 89 45 50        MOV word ptr ES:[DI + 0x50],AX
0000:2729  26 89 85 a0 00     MOV word ptr ES:[DI + 0xa0],AX
0000:272e  d0 e8              SHR AL,0x1
0000:2730  d0 dc              RCR AH,0x1
0000:2732  d0 da              RCR DL,0x1
0000:2734  d0 e7              SHL BH,0x1
0000:2736  d0 d3              RCL BL,0x1
0000:2738  d0 d6              RCL DH,0x1
0000:273a  0b c3              OR AX,BX
0000:273c  26 88 75 ff        MOV byte ptr ES:[DI + -0x1],DH
0000:2740  26 88 75 4f        MOV byte ptr ES:[DI + 0x4f],DH
0000:2744  26 89 05           MOV word ptr ES:[DI],AX
0000:2747  26 89 45 50        MOV word ptr ES:[DI + 0x50],AX
0000:274b  26 88 55 02        MOV byte ptr ES:[DI + 0x2],DL
0000:274f  26 88 55 52        MOV byte ptr ES:[DI + 0x52],DL
0000:2753  81 c7 a0 00        ADD DI,0xa0
0000:2757  e2 be              LOOP 0x0000:2717
0000:2759  81 ef 00 0a        SUB DI,0xa00
0000:275d  59                 POP CX
0000:275e  e9 29 ff           JMP 0x0000:268a
0000:2761  51                 PUSH CX
0000:2762  8a d1              MOV DL,CL
0000:2764  e8 0f fe           CALL 0x0000:2576
0000:2767  b8 00 a8           MOV AX,0xa800
0000:276a  8e c0              MOV AX,ES
0000:276c  be a4 2d           MOV SI,0x2da4
0000:276f  b9 10 00           MOV CX,0x10
0000:2772  8b d9              MOV BX,CX
0000:2774  b9 02 00           MOV CX,0x2
0000:2777  ac                 LODSB SI
0000:2778  8a f0              MOV DH,AL
0000:277a  8a d0              MOV DL,AL
0000:277c  d0 e2              SHL DL,0x1
0000:277e  d1 d0              RCL AX,0x1
0000:2780  d0 e6              SHL DH,0x1
0000:2782  d1 d0              RCL AX,0x1
0000:2784  d0 e2              SHL DL,0x1
0000:2786  d1 d0              RCL AX,0x1
0000:2788  d0 e6              SHL DH,0x1
0000:278a  d1 d0              RCL AX,0x1
0000:278c  d0 e2              SHL DL,0x1
0000:278e  d1 d0              RCL AX,0x1
0000:2790  d0 e6              SHL DH,0x1
0000:2792  d1 d0              RCL AX,0x1
0000:2794  d0 e2              SHL DL,0x1
0000:2796  d1 d0              RCL AX,0x1
0000:2798  d0 e6              SHL DH,0x1
0000:279a  d1 d0              RCL AX,0x1
0000:279c  d0 e2              SHL DL,0x1
0000:279e  d1 d0              RCL AX,0x1
0000:27a0  d0 e6              SHL DH,0x1
0000:27a2  d1 d0              RCL AX,0x1
0000:27a4  d0 e2              SHL DL,0x1
0000:27a6  d1 d0              RCL AX,0x1
0000:27a8  d0 e6              SHL DH,0x1
0000:27aa  d1 d0              RCL AX,0x1
0000:27ac  d0 e2              SHL DL,0x1
0000:27ae  d1 d0              RCL AX,0x1
0000:27b0  d0 e6              SHL DH,0x1
0000:27b2  d1 d0              RCL AX,0x1
0000:27b4  d0 e2              SHL DL,0x1
0000:27b6  d1 d0              RCL AX,0x1
0000:27b8  d0 e6              SHL DH,0x1
0000:27ba  d1 d0              RCL AX,0x1
0000:27bc  86 c4              XCHG AH,AL
0000:27be  ab                 STOSW ES:DI
0000:27bf  e2 b6              LOOP 0x0000:2777
0000:27c1  83 c7 4c           ADD DI,0x4c
0000:27c4  8b cb              MOV CX,BX
0000:27c6  e2 aa              LOOP 0x0000:2772
0000:27c8  81 ef fc 04        SUB DI,0x4fc
0000:27cc  59                 POP CX
0000:27cd  f6 c5 80           TEST CH,0x80
0000:27d0  75 03              JNZ 0x0000:27d5
0000:27d2  e9 9a fd           JMP 0x0000:256f
0000:27d5  83 ef 02           SUB DI,0x2
0000:27d8  e9 94 fd           JMP 0x0000:256f
0000:27db  51                 PUSH CX
0000:27dc  8a d1              MOV DL,CL
0000:27de  c0 ea 04           SHR DL,0x4
0000:27e1  e8 92 fd           CALL 0x0000:2576
0000:27e4  b8 00 a8           MOV AX,0xa800
0000:27e7  8e c0              MOV AX,ES
0000:27e9  be a4 2d           MOV SI,0x2da4
0000:27ec  b9 10 00           MOV CX,0x10
0000:27ef  32 db              XOR BL,BL
0000:27f1  ad                 LODSW SI
0000:27f2  d0 e8              SHR AL,0x1
0000:27f4  d0 dc              RCR AH,0x1
0000:27f6  d0 db              RCR BL,0x1
0000:27f8  8a fc              MOV BH,AH
0000:27fa  8a f0              MOV DH,AL
0000:27fc  8a d0              MOV DL,AL
0000:27fe  d0 e2              SHL DL,0x1
0000:2800  d1 d0              RCL AX,0x1
0000:2802  d0 e6              SHL DH,0x1
0000:2804  d1 d0              RCL AX,0x1
0000:2806  d0 e2              SHL DL,0x1
0000:2808  d1 d0              RCL AX,0x1
0000:280a  d0 e6              SHL DH,0x1
0000:280c  d1 d0              RCL AX,0x1
0000:280e  d0 e2              SHL DL,0x1
0000:2810  d1 d0              RCL AX,0x1
0000:2812  d0 e6              SHL DH,0x1
0000:2814  d1 d0              RCL AX,0x1
0000:2816  d0 e2              SHL DL,0x1
0000:2818  d1 d0              RCL AX,0x1
0000:281a  d0 e6              SHL DH,0x1
0000:281c  d1 d0              RCL AX,0x1
0000:281e  d0 e2              SHL DL,0x1
0000:2820  d1 d0              RCL AX,0x1
0000:2822  d0 e6              SHL DH,0x1
0000:2824  d1 d0              RCL AX,0x1
0000:2826  d0 e2              SHL DL,0x1
0000:2828  d1 d0              RCL AX,0x1
0000:282a  d0 e6              SHL DH,0x1
0000:282c  d1 d0              RCL AX,0x1
0000:282e  d0 e2              SHL DL,0x1
0000:2830  d1 d0              RCL AX,0x1
0000:2832  d0 e6              SHL DH,0x1
0000:2834  d1 d0              RCL AX,0x1
0000:2836  d0 e2              SHL DL,0x1
0000:2838  d1 d0              RCL AX,0x1
0000:283a  d0 e6              SHL DH,0x1
0000:283c  d1 d0              RCL AX,0x1
0000:283e  86 c4              XCHG AH,AL
0000:2840  26 89 45 50        MOV word ptr ES:[DI + 0x50],AX
0000:2844  8a f7              MOV DH,BH
0000:2846  8a d7              MOV DL,BH
0000:2848  d0 e2              SHL DL,0x1
0000:284a  d1 d0              RCL AX,0x1
0000:284c  d0 e6              SHL DH,0x1
0000:284e  d1 d0              RCL AX,0x1
0000:2850  d0 e2              SHL DL,0x1
0000:2852  d1 d0              RCL AX,0x1
0000:2854  d0 e6              SHL DH,0x1
0000:2856  d1 d0              RCL AX,0x1
0000:2858  d0 e2              SHL DL,0x1
0000:285a  d1 d0              RCL AX,0x1
0000:285c  d0 e6              SHL DH,0x1
0000:285e  d1 d0              RCL AX,0x1
0000:2860  d0 e2              SHL DL,0x1
0000:2862  d1 d0              RCL AX,0x1
0000:2864  d0 e6              SHL DH,0x1
0000:2866  d1 d0              RCL AX,0x1
0000:2868  d0 e2              SHL DL,0x1
0000:286a  d1 d0              RCL AX,0x1
0000:286c  d0 e6              SHL DH,0x1
0000:286e  d1 d0              RCL AX,0x1
0000:2870  d0 e2              SHL DL,0x1
0000:2872  d1 d0              RCL AX,0x1
0000:2874  d0 e6              SHL DH,0x1
0000:2876  d1 d0              RCL AX,0x1
0000:2878  d0 e2              SHL DL,0x1
0000:287a  d1 d0              RCL AX,0x1
0000:287c  d0 e6              SHL DH,0x1
0000:287e  d1 d0              RCL AX,0x1
0000:2880  d0 e2              SHL DL,0x1
0000:2882  d1 d0              RCL AX,0x1
0000:2884  d0 e6              SHL DH,0x1
0000:2886  d1 d0              RCL AX,0x1
0000:2888  86 c4              XCHG AH,AL
0000:288a  26 89 45 52        MOV word ptr ES:[DI + 0x52],AX
0000:288e  8a fb              MOV BH,BL
0000:2890  d0 e7              SHL BH,0x1
0000:2892  d0 d0              RCL AL,0x1
0000:2894  d0 e3              SHL BL,0x1
0000:2896  d0 d0              RCL AL,0x1
0000:2898  d0 e7              SHL BH,0x1
0000:289a  d0 d0              RCL AL,0x1
0000:289c  d0 e3              SHL BL,0x1
0000:289e  d0 d0              RCL AL,0x1
0000:28a0  d0 e7              SHL BH,0x1
0000:28a2  d0 d0              RCL AL,0x1
0000:28a4  d0 e3              SHL BL,0x1
0000:28a6  d0 d0              RCL AL,0x1
0000:28a8  d0 e7              SHL BH,0x1
0000:28aa  d0 d0              RCL AL,0x1
0000:28ac  d0 e3              SHL BL,0x1
0000:28ae  d0 d0              RCL AL,0x1
0000:28b0  26 88 45 54        MOV byte ptr ES:[DI + 0x54],AL
0000:28b4  83 c7 50           ADD DI,0x50
0000:28b7  83 e9 01           SUB CX,0x1
0000:28ba  74 03              JZ 0x0000:28bf
0000:28bc  e9 30 ff           JMP 0x0000:27ef
0000:28bf  81 ef 00 05        SUB DI,0x500
0000:28c3  59                 POP CX
0000:28c4  e9 9a fe           JMP 0x0000:2761
0000:28c7  51                 PUSH CX
0000:28c8  55                 PUSH BP
0000:28c9  8a d1              MOV DL,CL
0000:28cb  c0 ea 04           SHR DL,0x4
0000:28ce  e8 a5 fc           CALL 0x0000:2576
0000:28d1  b8 00 a8           MOV AX,0xa800
0000:28d4  8e c0              MOV AX,ES
0000:28d6  be a4 2d           MOV SI,0x2da4
0000:28d9  bd 10 00           MOV BP,0x10
0000:28dc  33 db              XOR BX,BX
0000:28de  ad                 LODSW SI
0000:28df  86 c4              XCHG AH,AL
0000:28e1  8b c8              MOV CX,AX
0000:28e3  d1 e1              SHL CX,0x1
0000:28e5  d0 d7              RCL BH,0x1
0000:28e7  0b c1              OR AX,CX
0000:28e9  c1 e9 02           SHR CX,0x2
0000:28ec  d0 db              RCR BL,0x1
0000:28ee  0b c8              OR CX,AX
0000:28f0  8a e7              MOV AH,BH
0000:28f2  d0 ec              SHR AH,0x1
0000:28f4  d0 d8              RCR AL,0x1
0000:28f6  d0 ef              SHR BH,0x1
0000:28f8  d0 d8              RCR AL,0x1
0000:28fa  d0 ec              SHR AH,0x1
0000:28fc  d0 d8              RCR AL,0x1
0000:28fe  d0 ef              SHR BH,0x1
0000:2900  d0 d8              RCR AL,0x1
0000:2902  d0 ec              SHR AH,0x1
0000:2904  d0 d8              RCR AL,0x1
0000:2906  d0 ef              SHR BH,0x1
0000:2908  d0 d8              RCR AL,0x1
0000:290a  d0 ec              SHR AH,0x1
0000:290c  d0 d8              RCR AL,0x1
0000:290e  d0 ef              SHR BH,0x1
0000:2910  d0 d8              RCR AL,0x1
0000:2912  26 88 45 ff        MOV byte ptr ES:[DI + -0x1],AL
0000:2916  8a f5              MOV DH,CH
0000:2918  8a d5              MOV DL,CH
0000:291a  d0 e2              SHL DL,0x1
0000:291c  d1 d0              RCL AX,0x1
0000:291e  d0 e6              SHL DH,0x1
0000:2920  d1 d0              RCL AX,0x1
0000:2922  d0 e2              SHL DL,0x1
0000:2924  d1 d0              RCL AX,0x1
0000:2926  d0 e6              SHL DH,0x1
0000:2928  d1 d0              RCL AX,0x1
0000:292a  d0 e2              SHL DL,0x1
0000:292c  d1 d0              RCL AX,0x1
0000:292e  d0 e6              SHL DH,0x1
0000:2930  d1 d0              RCL AX,0x1
0000:2932  d0 e2              SHL DL,0x1
0000:2934  d1 d0              RCL AX,0x1
0000:2936  d0 e6              SHL DH,0x1
0000:2938  d1 d0              RCL AX,0x1
0000:293a  d0 e2              SHL DL,0x1
0000:293c  d1 d0              RCL AX,0x1
0000:293e  d0 e6              SHL DH,0x1
0000:2940  d1 d0              RCL AX,0x1
0000:2942  d0 e2              SHL DL,0x1
0000:2944  d1 d0              RCL AX,0x1
0000:2946  d0 e6              SHL DH,0x1
0000:2948  d1 d0              RCL AX,0x1
0000:294a  d0 e2              SHL DL,0x1
0000:294c  d1 d0              RCL AX,0x1
0000:294e  d0 e6              SHL DH,0x1
0000:2950  d1 d0              RCL AX,0x1
0000:2952  d0 e2              SHL DL,0x1
0000:2954  d1 d0              RCL AX,0x1
0000:2956  d0 e6              SHL DH,0x1
0000:2958  d1 d0              RCL AX,0x1
0000:295a  86 c4              XCHG AH,AL
0000:295c  26 89 05           MOV word ptr ES:[DI],AX
0000:295f  8a f1              MOV DH,CL
0000:2961  8a d1              MOV DL,CL
0000:2963  d0 e2              SHL DL,0x1
0000:2965  d1 d0              RCL AX,0x1
0000:2967  d0 e6              SHL DH,0x1
0000:2969  d1 d0              RCL AX,0x1
0000:296b  d0 e2              SHL DL,0x1
0000:296d  d1 d0              RCL AX,0x1
0000:296f  d0 e6              SHL DH,0x1
0000:2971  d1 d0              RCL AX,0x1
0000:2973  d0 e2              SHL DL,0x1
0000:2975  d1 d0              RCL AX,0x1
0000:2977  d0 e6              SHL DH,0x1
0000:2979  d1 d0              RCL AX,0x1
0000:297b  d0 e2              SHL DL,0x1
0000:297d  d1 d0              RCL AX,0x1
0000:297f  d0 e6              SHL DH,0x1
0000:2981  d1 d0              RCL AX,0x1
0000:2983  d0 e2              SHL DL,0x1
0000:2985  d1 d0              RCL AX,0x1
0000:2987  d0 e6              SHL DH,0x1
0000:2989  d1 d0              RCL AX,0x1
0000:298b  d0 e2              SHL DL,0x1
0000:298d  d1 d0              RCL AX,0x1
0000:298f  d0 e6              SHL DH,0x1
0000:2991  d1 d0              RCL AX,0x1
0000:2993  d0 e2              SHL DL,0x1
0000:2995  d1 d0              RCL AX,0x1
0000:2997  d0 e6              SHL DH,0x1
0000:2999  d1 d0              RCL AX,0x1
0000:299b  d0 e2              SHL DL,0x1
0000:299d  d1 d0              RCL AX,0x1
0000:299f  d0 e6              SHL DH,0x1
0000:29a1  d1 d0              RCL AX,0x1
0000:29a3  86 c4              XCHG AH,AL
0000:29a5  26 89 45 02        MOV word ptr ES:[DI + 0x2],AX
0000:29a9  8a e3              MOV AH,BL
0000:29ab  d0 e4              SHL AH,0x1
0000:29ad  d0 d0              RCL AL,0x1
0000:29af  d0 e3              SHL BL,0x1
0000:29b1  d0 d0              RCL AL,0x1
0000:29b3  d0 e4              SHL AH,0x1
0000:29b5  d0 d0              RCL AL,0x1
0000:29b7  d0 e3              SHL BL,0x1
0000:29b9  d0 d0              RCL AL,0x1
0000:29bb  d0 e4              SHL AH,0x1
0000:29bd  d0 d0              RCL AL,0x1
0000:29bf  d0 e3              SHL BL,0x1
0000:29c1  d0 d0              RCL AL,0x1
0000:29c3  d0 e4              SHL AH,0x1
0000:29c5  d0 d0              RCL AL,0x1
0000:29c7  d0 e3              SHL BL,0x1
0000:29c9  d0 d0              RCL AL,0x1
0000:29cb  26 88 45 04        MOV byte ptr ES:[DI + 0x4],AL
0000:29cf  83 c7 50           ADD DI,0x50
0000:29d2  83 ed 01           SUB BP,0x1
0000:29d5  74 03              JZ 0x0000:29da
0000:29d7  e9 02 ff           JMP 0x0000:28dc
0000:29da  81 ef 00 05        SUB DI,0x500
0000:29de  5d                 POP BP
0000:29df  83 ef 50           SUB DI,0x50
0000:29e2  e8 0f 00           CALL 0x0000:29f4
0000:29e5  81 ef 60 04        SUB DI,0x460
0000:29e9  e8 08 00           CALL 0x0000:29f4
0000:29ec  81 ef 50 05        SUB DI,0x550
0000:29f0  59                 POP CX
0000:29f1  e9 6d fd           JMP 0x0000:2761
0000:29f4  be a4 2d           MOV SI,0x2da4
0000:29f7  b9 10 00           MOV CX,0x10
0000:29fa  8b d9              MOV BX,CX
0000:29fc  b9 02 00           MOV CX,0x2
0000:29ff  ac                 LODSB SI
0000:2a00  8a f0              MOV DH,AL
0000:2a02  8a d0              MOV DL,AL
0000:2a04  d0 e2              SHL DL,0x1
0000:2a06  d1 d0              RCL AX,0x1
0000:2a08  d0 e6              SHL DH,0x1
0000:2a0a  d1 d0              RCL AX,0x1
0000:2a0c  d0 e2              SHL DL,0x1
0000:2a0e  d1 d0              RCL AX,0x1
0000:2a10  d0 e6              SHL DH,0x1
0000:2a12  d1 d0              RCL AX,0x1
0000:2a14  d0 e2              SHL DL,0x1
0000:2a16  d1 d0              RCL AX,0x1
0000:2a18  d0 e6              SHL DH,0x1
0000:2a1a  d1 d0              RCL AX,0x1
0000:2a1c  d0 e2              SHL DL,0x1
0000:2a1e  d1 d0              RCL AX,0x1
0000:2a20  d0 e6              SHL DH,0x1
0000:2a22  d1 d0              RCL AX,0x1
0000:2a24  d0 e2              SHL DL,0x1
0000:2a26  d1 d0              RCL AX,0x1
0000:2a28  d0 e6              SHL DH,0x1
0000:2a2a  d1 d0              RCL AX,0x1
0000:2a2c  d0 e2              SHL DL,0x1
0000:2a2e  d1 d0              RCL AX,0x1
0000:2a30  d0 e6              SHL DH,0x1
0000:2a32  d1 d0              RCL AX,0x1
0000:2a34  d0 e2              SHL DL,0x1
0000:2a36  d1 d0              RCL AX,0x1
0000:2a38  d0 e6              SHL DH,0x1
0000:2a3a  d1 d0              RCL AX,0x1
0000:2a3c  d0 e2              SHL DL,0x1
0000:2a3e  d1 d0              RCL AX,0x1
0000:2a40  d0 e6              SHL DH,0x1
0000:2a42  d1 d0              RCL AX,0x1
0000:2a44  86 c4              XCHG AH,AL
0000:2a46  ab                 STOSW ES:DI
0000:2a47  e2 b6              LOOP 0x0000:29ff
0000:2a49  83 c7 4c           ADD DI,0x4c
0000:2a4c  8b cb              MOV CX,BX
0000:2a4e  e2 aa              LOOP 0x0000:29fa
0000:2a50  c3                 RET
0000:2a51  51                 PUSH CX
0000:2a52  8a d1              MOV DL,CL
0000:2a54  e8 1f fb           CALL 0x0000:2576
0000:2a57  b8 00 a8           MOV AX,0xa800
0000:2a5a  8e c0              MOV AX,ES
0000:2a5c  be a4 2d           MOV SI,0x2da4
0000:2a5f  b9 10 00           MOV CX,0x10
0000:2a62  8b d9              MOV BX,CX
0000:2a64  b9 02 00           MOV CX,0x2
0000:2a67  ac                 LODSB SI
0000:2a68  8a f0              MOV DH,AL
0000:2a6a  8a d0              MOV DL,AL
0000:2a6c  d0 e2              SHL DL,0x1
0000:2a6e  d1 d0              RCL AX,0x1
0000:2a70  d0 e6              SHL DH,0x1
0000:2a72  d1 d0              RCL AX,0x1
0000:2a74  d0 e2              SHL DL,0x1
0000:2a76  d1 d0              RCL AX,0x1
0000:2a78  d0 e6              SHL DH,0x1
0000:2a7a  d1 d0              RCL AX,0x1
0000:2a7c  d0 e2              SHL DL,0x1
0000:2a7e  d1 d0              RCL AX,0x1
0000:2a80  d0 e6              SHL DH,0x1
0000:2a82  d1 d0              RCL AX,0x1
0000:2a84  d0 e2              SHL DL,0x1
0000:2a86  d1 d0              RCL AX,0x1
0000:2a88  d0 e6              SHL DH,0x1
0000:2a8a  d1 d0              RCL AX,0x1
0000:2a8c  d0 e2              SHL DL,0x1
0000:2a8e  d1 d0              RCL AX,0x1
0000:2a90  d0 e6              SHL DH,0x1
0000:2a92  d1 d0              RCL AX,0x1
0000:2a94  d0 e2              SHL DL,0x1
0000:2a96  d1 d0              RCL AX,0x1
0000:2a98  d0 e6              SHL DH,0x1
0000:2a9a  d1 d0              RCL AX,0x1
0000:2a9c  d0 e2              SHL DL,0x1
0000:2a9e  d1 d0              RCL AX,0x1
0000:2aa0  d0 e6              SHL DH,0x1
0000:2aa2  d1 d0              RCL AX,0x1
0000:2aa4  d0 e2              SHL DL,0x1
0000:2aa6  d1 d0              RCL AX,0x1
0000:2aa8  d0 e6              SHL DH,0x1
0000:2aaa  d1 d0              RCL AX,0x1
0000:2aac  86 c4              XCHG AH,AL
0000:2aae  ab                 STOSW ES:DI
0000:2aaf  26 89 45 4e        MOV word ptr ES:[DI + 0x4e],AX
0000:2ab3  e2 b2              LOOP 0x0000:2a67
0000:2ab5  81 c7 9c 00        ADD DI,0x9c
0000:2ab9  8b cb              MOV CX,BX
0000:2abb  e2 a5              LOOP 0x0000:2a62
0000:2abd  81 ef fc 09        SUB DI,0x9fc
0000:2ac1  59                 POP CX
0000:2ac2  f6 c5 80           TEST CH,0x80
0000:2ac5  75 03              JNZ 0x0000:2aca
0000:2ac7  e9 a5 fa           JMP 0x0000:256f
0000:2aca  83 ef 02           SUB DI,0x2
0000:2acd  e9 9f fa           JMP 0x0000:256f
0000:2ad0  51                 PUSH CX
0000:2ad1  8a d1              MOV DL,CL
0000:2ad3  c0 ea 04           SHR DL,0x4
0000:2ad6  e8 9d fa           CALL 0x0000:2576
0000:2ad9  b8 00 a8           MOV AX,0xa800
0000:2adc  8e c0              MOV AX,ES
0000:2ade  be a4 2d           MOV SI,0x2da4
0000:2ae1  b9 10 00           MOV CX,0x10
0000:2ae4  32 db              XOR BL,BL
0000:2ae6  ad                 LODSW SI
0000:2ae7  d0 e8              SHR AL,0x1
0000:2ae9  d0 dc              RCR AH,0x1
0000:2aeb  d0 db              RCR BL,0x1
0000:2aed  8a fc              MOV BH,AH
0000:2aef  8a f0              MOV DH,AL
0000:2af1  8a d0              MOV DL,AL
0000:2af3  d0 e2              SHL DL,0x1
0000:2af5  d1 d0              RCL AX,0x1
0000:2af7  d0 e6              SHL DH,0x1
0000:2af9  d1 d0              RCL AX,0x1
0000:2afb  d0 e2              SHL DL,0x1
0000:2afd  d1 d0              RCL AX,0x1
0000:2aff  d0 e6              SHL DH,0x1
0000:2b01  d1 d0              RCL AX,0x1
0000:2b03  d0 e2              SHL DL,0x1
0000:2b05  d1 d0              RCL AX,0x1
0000:2b07  d0 e6              SHL DH,0x1
0000:2b09  d1 d0              RCL AX,0x1
0000:2b0b  d0 e2              SHL DL,0x1
0000:2b0d  d1 d0              RCL AX,0x1
0000:2b0f  d0 e6              SHL DH,0x1
0000:2b11  d1 d0              RCL AX,0x1
0000:2b13  d0 e2              SHL DL,0x1
0000:2b15  d1 d0              RCL AX,0x1
0000:2b17  d0 e6              SHL DH,0x1
0000:2b19  d1 d0              RCL AX,0x1
0000:2b1b  d0 e2              SHL DL,0x1
0000:2b1d  d1 d0              RCL AX,0x1
0000:2b1f  d0 e6              SHL DH,0x1
0000:2b21  d1 d0              RCL AX,0x1
0000:2b23  d0 e2              SHL DL,0x1
0000:2b25  d1 d0              RCL AX,0x1
0000:2b27  d0 e6              SHL DH,0x1
0000:2b29  d1 d0              RCL AX,0x1
0000:2b2b  d0 e2              SHL DL,0x1
0000:2b2d  d1 d0              RCL AX,0x1
0000:2b2f  d0 e6              SHL DH,0x1
0000:2b31  d1 d0              RCL AX,0x1
0000:2b33  86 c4              XCHG AH,AL
0000:2b35  26 89 85 a0 00     MOV word ptr ES:[DI + 0xa0],AX
0000:2b3a  26 89 85 f0 00     MOV word ptr ES:[DI + 0xf0],AX
0000:2b3f  8a f7              MOV DH,BH
0000:2b41  8a d7              MOV DL,BH
0000:2b43  d0 e2              SHL DL,0x1
0000:2b45  d1 d0              RCL AX,0x1
0000:2b47  d0 e6              SHL DH,0x1
0000:2b49  d1 d0              RCL AX,0x1
0000:2b4b  d0 e2              SHL DL,0x1
0000:2b4d  d1 d0              RCL AX,0x1
0000:2b4f  d0 e6              SHL DH,0x1
0000:2b51  d1 d0              RCL AX,0x1
0000:2b53  d0 e2              SHL DL,0x1
0000:2b55  d1 d0              RCL AX,0x1
0000:2b57  d0 e6              SHL DH,0x1
0000:2b59  d1 d0              RCL AX,0x1
0000:2b5b  d0 e2              SHL DL,0x1
0000:2b5d  d1 d0              RCL AX,0x1
0000:2b5f  d0 e6              SHL DH,0x1
0000:2b61  d1 d0              RCL AX,0x1
0000:2b63  d0 e2              SHL DL,0x1
0000:2b65  d1 d0              RCL AX,0x1
0000:2b67  d0 e6              SHL DH,0x1
0000:2b69  d1 d0              RCL AX,0x1
0000:2b6b  d0 e2              SHL DL,0x1
0000:2b6d  d1 d0              RCL AX,0x1
0000:2b6f  d0 e6              SHL DH,0x1
0000:2b71  d1 d0              RCL AX,0x1
0000:2b73  d0 e2              SHL DL,0x1
0000:2b75  d1 d0              RCL AX,0x1
0000:2b77  d0 e6              SHL DH,0x1
0000:2b79  d1 d0              RCL AX,0x1
0000:2b7b  d0 e2              SHL DL,0x1
0000:2b7d  d1 d0              RCL AX,0x1
0000:2b7f  d0 e6              SHL DH,0x1
0000:2b81  d1 d0              RCL AX,0x1
0000:2b83  86 c4              XCHG AH,AL
0000:2b85  26 89 85 a2 00     MOV word ptr ES:[DI + 0xa2],AX
0000:2b8a  26 89 85 f2 00     MOV word ptr ES:[DI + 0xf2],AX
0000:2b8f  8a fb              MOV BH,BL
0000:2b91  d0 e7              SHL BH,0x1
0000:2b93  d0 d0              RCL AL,0x1
0000:2b95  d0 e3              SHL BL,0x1
0000:2b97  d0 d0              RCL AL,0x1
0000:2b99  d0 e7              SHL BH,0x1
0000:2b9b  d0 d0              RCL AL,0x1
0000:2b9d  d0 e3              SHL BL,0x1
0000:2b9f  d0 d0              RCL AL,0x1
0000:2ba1  d0 e7              SHL BH,0x1
0000:2ba3  d0 d0              RCL AL,0x1
0000:2ba5  d0 e3              SHL BL,0x1
0000:2ba7  d0 d0              RCL AL,0x1
0000:2ba9  d0 e7              SHL BH,0x1
0000:2bab  d0 d0              RCL AL,0x1
0000:2bad  d0 e3              SHL BL,0x1
0000:2baf  d0 d0              RCL AL,0x1
0000:2bb1  26 88 85 a4 00     MOV byte ptr ES:[DI + 0xa4],AL
0000:2bb6  26 88 85 f4 00     MOV byte ptr ES:[DI + 0xf4],AL
0000:2bbb  81 c7 a0 00        ADD DI,0xa0
0000:2bbf  83 e9 01           SUB CX,0x1
0000:2bc2  74 03              JZ 0x0000:2bc7
0000:2bc4  e9 1d ff           JMP 0x0000:2ae4
0000:2bc7  81 ef 00 0a        SUB DI,0xa00
0000:2bcb  59                 POP CX
0000:2bcc  e9 82 fe           JMP 0x0000:2a51
0000:2bcf  51                 PUSH CX
0000:2bd0  55                 PUSH BP
0000:2bd1  8a d1              MOV DL,CL
0000:2bd3  c0 ea 04           SHR DL,0x4
0000:2bd6  e8 9d f9           CALL 0x0000:2576
0000:2bd9  b8 00 a8           MOV AX,0xa800
0000:2bdc  8e c0              MOV AX,ES
0000:2bde  be a4 2d           MOV SI,0x2da4
0000:2be1  bd 10 00           MOV BP,0x10
0000:2be4  33 db              XOR BX,BX
0000:2be6  ad                 LODSW SI
0000:2be7  86 c4              XCHG AH,AL
0000:2be9  8b c8              MOV CX,AX
0000:2beb  d1 e1              SHL CX,0x1
0000:2bed  d0 d7              RCL BH,0x1
0000:2bef  0b c1              OR AX,CX
0000:2bf1  c1 e9 02           SHR CX,0x2
0000:2bf4  d0 db              RCR BL,0x1
0000:2bf6  0b c8              OR CX,AX
0000:2bf8  8a e7              MOV AH,BH
0000:2bfa  d0 ec              SHR AH,0x1
0000:2bfc  d0 d8              RCR AL,0x1
0000:2bfe  d0 ef              SHR BH,0x1
0000:2c00  d0 d8              RCR AL,0x1
0000:2c02  d0 ec              SHR AH,0x1
0000:2c04  d0 d8              RCR AL,0x1
0000:2c06  d0 ef              SHR BH,0x1
0000:2c08  d0 d8              RCR AL,0x1
0000:2c0a  d0 ec              SHR AH,0x1
0000:2c0c  d0 d8              RCR AL,0x1
0000:2c0e  d0 ef              SHR BH,0x1
0000:2c10  d0 d8              RCR AL,0x1
0000:2c12  d0 ec              SHR AH,0x1
0000:2c14  d0 d8              RCR AL,0x1
0000:2c16  d0 ef              SHR BH,0x1
0000:2c18  d0 d8              RCR AL,0x1
0000:2c1a  26 88 45 ff        MOV byte ptr ES:[DI + -0x1],AL
0000:2c1e  26 88 45 4f        MOV byte ptr ES:[DI + 0x4f],AL
0000:2c22  8a f5              MOV DH,CH
0000:2c24  8a d5              MOV DL,CH
0000:2c26  d0 e2              SHL DL,0x1
0000:2c28  d1 d0              RCL AX,0x1
0000:2c2a  d0 e6              SHL DH,0x1
0000:2c2c  d1 d0              RCL AX,0x1
0000:2c2e  d0 e2              SHL DL,0x1
0000:2c30  d1 d0              RCL AX,0x1
0000:2c32  d0 e6              SHL DH,0x1
0000:2c34  d1 d0              RCL AX,0x1
0000:2c36  d0 e2              SHL DL,0x1
0000:2c38  d1 d0              RCL AX,0x1
0000:2c3a  d0 e6              SHL DH,0x1
0000:2c3c  d1 d0              RCL AX,0x1
0000:2c3e  d0 e2              SHL DL,0x1
0000:2c40  d1 d0              RCL AX,0x1
0000:2c42  d0 e6              SHL DH,0x1
0000:2c44  d1 d0              RCL AX,0x1
0000:2c46  d0 e2              SHL DL,0x1
0000:2c48  d1 d0              RCL AX,0x1
0000:2c4a  d0 e6              SHL DH,0x1
0000:2c4c  d1 d0              RCL AX,0x1
0000:2c4e  d0 e2              SHL DL,0x1
0000:2c50  d1 d0              RCL AX,0x1
0000:2c52  d0 e6              SHL DH,0x1
0000:2c54  d1 d0              RCL AX,0x1
0000:2c56  d0 e2              SHL DL,0x1
0000:2c58  d1 d0              RCL AX,0x1
0000:2c5a  d0 e6              SHL DH,0x1
0000:2c5c  d1 d0              RCL AX,0x1
0000:2c5e  d0 e2              SHL DL,0x1
0000:2c60  d1 d0              RCL AX,0x1
0000:2c62  d0 e6              SHL DH,0x1
0000:2c64  d1 d0              RCL AX,0x1
0000:2c66  86 c4              XCHG AH,AL
0000:2c68  26 89 05           MOV word ptr ES:[DI],AX
0000:2c6b  26 89 45 50        MOV word ptr ES:[DI + 0x50],AX
0000:2c6f  8a f1              MOV DH,CL
0000:2c71  8a d1              MOV DL,CL
0000:2c73  d0 e2              SHL DL,0x1
0000:2c75  d1 d0              RCL AX,0x1
0000:2c77  d0 e6              SHL DH,0x1
0000:2c79  d1 d0              RCL AX,0x1
0000:2c7b  d0 e2              SHL DL,0x1
0000:2c7d  d1 d0              RCL AX,0x1
0000:2c7f  d0 e6              SHL DH,0x1
0000:2c81  d1 d0              RCL AX,0x1
0000:2c83  d0 e2              SHL DL,0x1
0000:2c85  d1 d0              RCL AX,0x1
0000:2c87  d0 e6              SHL DH,0x1
0000:2c89  d1 d0              RCL AX,0x1
0000:2c8b  d0 e2              SHL DL,0x1
0000:2c8d  d1 d0              RCL AX,0x1
0000:2c8f  d0 e6              SHL DH,0x1
0000:2c91  d1 d0              RCL AX,0x1
0000:2c93  d0 e2              SHL DL,0x1
0000:2c95  d1 d0              RCL AX,0x1
0000:2c97  d0 e6              SHL DH,0x1
0000:2c99  d1 d0              RCL AX,0x1
0000:2c9b  d0 e2              SHL DL,0x1
0000:2c9d  d1 d0              RCL AX,0x1
0000:2c9f  d0 e6              SHL DH,0x1
0000:2ca1  d1 d0              RCL AX,0x1
0000:2ca3  d0 e2              SHL DL,0x1
0000:2ca5  d1 d0              RCL AX,0x1
0000:2ca7  d0 e6              SHL DH,0x1
0000:2ca9  d1 d0              RCL AX,0x1
0000:2cab  d0 e2              SHL DL,0x1
0000:2cad  d1 d0              RCL AX,0x1
0000:2caf  d0 e6              SHL DH,0x1
0000:2cb1  d1 d0              RCL AX,0x1
0000:2cb3  86 c4              XCHG AH,AL
0000:2cb5  26 89 45 02        MOV word ptr ES:[DI + 0x2],AX
0000:2cb9  26 89 45 52        MOV word ptr ES:[DI + 0x52],AX
0000:2cbd  8a e3              MOV AH,BL
0000:2cbf  d0 e4              SHL AH,0x1
0000:2cc1  d0 d0              RCL AL,0x1
0000:2cc3  d0 e3              SHL BL,0x1
0000:2cc5  d0 d0              RCL AL,0x1
0000:2cc7  d0 e4              SHL AH,0x1
0000:2cc9  d0 d0              RCL AL,0x1
0000:2ccb  d0 e3              SHL BL,0x1
0000:2ccd  d0 d0              RCL AL,0x1
0000:2ccf  d0 e4              SHL AH,0x1
0000:2cd1  d0 d0              RCL AL,0x1
0000:2cd3  d0 e3              SHL BL,0x1
0000:2cd5  d0 d0              RCL AL,0x1
0000:2cd7  d0 e4              SHL AH,0x1
0000:2cd9  d0 d0              RCL AL,0x1
0000:2cdb  d0 e3              SHL BL,0x1
0000:2cdd  d0 d0              RCL AL,0x1
0000:2cdf  26 88 45 04        MOV byte ptr ES:[DI + 0x4],AL
0000:2ce3  26 88 45 54        MOV byte ptr ES:[DI + 0x54],AL
0000:2ce7  81 c7 a0 00        ADD DI,0xa0
0000:2ceb  83 ed 01           SUB BP,0x1
0000:2cee  74 03              JZ 0x0000:2cf3
0000:2cf0  e9 f1 fe           JMP 0x0000:2be4
0000:2cf3  81 ef 00 0a        SUB DI,0xa00
0000:2cf7  5d                 POP BP
0000:2cf8  81 ef a0 00        SUB DI,0xa0
0000:2cfc  e8 0f 00           CALL 0x0000:2d0e
0000:2cff  81 ef c0 08        SUB DI,0x8c0
0000:2d03  e8 08 00           CALL 0x0000:2d0e
0000:2d06  81 ef a0 0a        SUB DI,0xaa0
0000:2d0a  59                 POP CX
0000:2d0b  e9 43 fd           JMP 0x0000:2a51
0000:2d0e  be a4 2d           MOV SI,0x2da4
0000:2d11  b9 10 00           MOV CX,0x10
0000:2d14  8b d9              MOV BX,CX
0000:2d16  b9 02 00           MOV CX,0x2
0000:2d19  ac                 LODSB SI
0000:2d1a  8a f0              MOV DH,AL
0000:2d1c  8a d0              MOV DL,AL
0000:2d1e  d0 e2              SHL DL,0x1
0000:2d20  d1 d0              RCL AX,0x1
0000:2d22  d0 e6              SHL DH,0x1
0000:2d24  d1 d0              RCL AX,0x1
0000:2d26  d0 e2              SHL DL,0x1
0000:2d28  d1 d0              RCL AX,0x1
0000:2d2a  d0 e6              SHL DH,0x1
0000:2d2c  d1 d0              RCL AX,0x1
0000:2d2e  d0 e2              SHL DL,0x1
0000:2d30  d1 d0              RCL AX,0x1
0000:2d32  d0 e6              SHL DH,0x1
0000:2d34  d1 d0              RCL AX,0x1
0000:2d36  d0 e2              SHL DL,0x1
0000:2d38  d1 d0              RCL AX,0x1
0000:2d3a  d0 e6              SHL DH,0x1
0000:2d3c  d1 d0              RCL AX,0x1
0000:2d3e  d0 e2              SHL DL,0x1
0000:2d40  d1 d0              RCL AX,0x1
0000:2d42  d0 e6              SHL DH,0x1
0000:2d44  d1 d0              RCL AX,0x1
0000:2d46  d0 e2              SHL DL,0x1
0000:2d48  d1 d0              RCL AX,0x1
0000:2d4a  d0 e6              SHL DH,0x1
0000:2d4c  d1 d0              RCL AX,0x1
0000:2d4e  d0 e2              SHL DL,0x1
0000:2d50  d1 d0              RCL AX,0x1
0000:2d52  d0 e6              SHL DH,0x1
0000:2d54  d1 d0              RCL AX,0x1
0000:2d56  d0 e2              SHL DL,0x1
0000:2d58  d1 d0              RCL AX,0x1
0000:2d5a  d0 e6              SHL DH,0x1
0000:2d5c  d1 d0              RCL AX,0x1
0000:2d5e  86 c4              XCHG AH,AL
0000:2d60  ab                 STOSW ES:DI
0000:2d61  26 89 45 4e        MOV word ptr ES:[DI + 0x4e],AX
0000:2d65  e2 b2              LOOP 0x0000:2d19
0000:2d67  81 c7 9c 00        ADD DI,0x9c
0000:2d6b  8b cb              MOV CX,BX
0000:2d6d  e2 a5              LOOP 0x0000:2d14
0000:2d6f  c3                 RET
0000:2d70  b0 0b              MOV AL,0xb
0000:2d72  e6 68              OUT 0x68,AL
0000:2d74  8a c6              MOV AL,DH
0000:2d76  2c 20              SUB AL,0x20
0000:2d78  e6 a3              OUT 0xa3,AL
0000:2d7a  8a c2              MOV AL,DL
0000:2d7c  e6 a1              OUT 0xa1,AL
0000:2d7e  bb a4 2d           MOV BX,0x2da4
0000:2d81  b9 10 00           MOV CX,0x10
0000:2d84  32 e4              XOR AH,AH
0000:2d86  8a c4              MOV AL,AH
0000:2d88  0c 20              OR AL,0x20
0000:2d8a  e6 a5              OUT 0xa5,AL
0000:2d8c  e4 a9              IN AL,0xa9
0000:2d8e  88 07              MOV byte ptr [BX],AL
0000:2d90  43                 INC BX
0000:2d91  8a c4              MOV AL,AH
0000:2d93  e6 a5              OUT 0xa5,AL
0000:2d95  e4 a9              IN AL,0xa9
0000:2d97  88 07              MOV byte ptr [BX],AL
0000:2d99  43                 INC BX
0000:2d9a  fe c4              INC AH
0000:2d9c  e2 e8              LOOP 0x0000:2d86
0000:2d9e  b0 0a              MOV AL,0xa
0000:2da0  e6 68              OUT 0x68,AL
0000:2da2  c3                 RET
0000:2fd0  b8 02 00           MOV AX,0x2
0000:2fd3  e8 9a ea           CALL 0x0000:1a70
0000:2fd6  80 be b7 00 00     CMP byte ptr [BP + 0xb7],0x0
0000:2fdb  74 01              JZ 0x0000:2fde
0000:2fdd  c3                 RET
0000:2fde  8e 44 0a           MOV ES,word ptr [SI + 0xa]
0000:2fe1  8b 7c 08           MOV DI,word ptr [SI + 0x8]
0000:2fe4  33 db              XOR BX,BX
0000:2fe6  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:2fe9  47                 INC DI
0000:2fea  89 7c 08           MOV word ptr [SI + 0x8],DI
0000:2fed  d1 e3              SHL BX,0x1
0000:2fef  2e ff a7 d0 2d     JMP word ptr CS:[BX + 0x2dd0]
0000:2ff4  56                 PUSH SI
0000:2ff5  8b 76 52           MOV SI,word ptr [BP + 0x52]
0000:2ff8  ba 13 30           MOV DX,0x3013
0000:2ffb  bb 52 00           MOV BX,0x52
0000:2ffe  b9 00 01           MOV CX,0x100
0000:3001  b8 00 80           MOV AX,0x8000
0000:3004  e8 3a ea           CALL 0x0000:1a41
0000:3007  5e                 POP SI
0000:3008  b8 02 00           MOV AX,0x2
0000:300b  e8 62 ea           CALL 0x0000:1a70
0000:300e  e8 68 ea           CALL 0x0000:1a79
0000:3011  eb f5              JMP 0x0000:3008
0000:3020  b8 02 00           MOV AX,0x2
0000:3023  e8 4a ea           CALL 0x0000:1a70
0000:3026  e8 50 ea           CALL 0x0000:1a79
0000:3029  eb f5              JMP 0x0000:3020
0000:302b  e8 7f ea           CALL 0x0000:1aad
0000:302e  c3                 RET
0000:302f  57                 PUSH DI
0000:3030  bb 52 00           MOV BX,0x52
0000:3033  b9 00 01           MOV CX,0x100
0000:3036  8b 7e 52           MOV DI,word ptr [BP + 0x52]
0000:3039  e8 83 ea           CALL 0x0000:1abf
0000:303c  5f                 POP DI
0000:303d  eb 97              JMP 0x0000:2fd6
0000:303f  81 4e 6c 00 80     OR word ptr [BP + 0x6c],0x8000
0000:3044  81 4e 6e 00 80     OR word ptr [BP + 0x6e],0x8000
0000:3049  c3                 RET
0000:304a  c3                 RET
0000:304b  26 8b 05           MOV AX,word ptr ES:[DI]
0000:304e  89 44 08           MOV word ptr [SI + 0x8],AX
0000:3051  eb 83              JMP 0x0000:2fd6
0000:3053  26 8b 05           MOV AX,word ptr ES:[DI]
0000:3056  83 c7 02           ADD DI,0x2
0000:3059  89 7c 08           MOV word ptr [SI + 0x8],DI
0000:305c  e8 11 ea           CALL 0x0000:1a70
0000:305f  e8 17 ea           CALL 0x0000:1a79
0000:3062  e9 71 ff           JMP 0x0000:2fd6
0000:3065  b8 01 00           MOV AX,0x1
0000:3068  e8 05 ea           CALL 0x0000:1a70
0000:306b  f7 46 6e 3f 00     TEST word ptr [BP + 0x6e],0x3f
0000:3070  75 03              JNZ 0x0000:3075
0000:3072  ff 44 06           INC word ptr [SI + 0x6]
0000:3075  e8 01 ea           CALL 0x0000:1a79
0000:3078  e9 5b ff           JMP 0x0000:2fd6
0000:307b  26 8b 05           MOV AX,word ptr ES:[DI]
0000:307e  83 c7 02           ADD DI,0x2
0000:3081  8b 5c 0c           MOV BX,word ptr [SI + 0xc]
0000:3084  83 44 0c 02        ADD word ptr [SI + 0xc],0x2
0000:3088  89 78 0e           MOV word ptr [BX + SI + 0xe],DI
0000:308b  89 44 08           MOV word ptr [SI + 0x8],AX
0000:308e  e9 45 ff           JMP 0x0000:2fd6
0000:3091  83 6c 0c 02        SUB word ptr [SI + 0xc],0x2
0000:3095  8b 5c 0c           MOV BX,word ptr [SI + 0xc]
0000:3098  8b 40 0e           MOV AX,word ptr [BX + SI + 0xe]
0000:309b  89 44 08           MOV word ptr [SI + 0x8],AX
0000:309e  e9 35 ff           JMP 0x0000:2fd6
0000:30a1  26 8b 05           MOV AX,word ptr ES:[DI]
0000:30a4  83 c7 02           ADD DI,0x2
0000:30a7  89 7c 08           MOV word ptr [SI + 0x8],DI
0000:30aa  8b 5c 0c           MOV BX,word ptr [SI + 0xc]
0000:30ad  89 78 0e           MOV word ptr [BX + SI + 0xe],DI
0000:30b0  83 c3 02           ADD BX,0x2
0000:30b3  89 40 0e           MOV word ptr [BX + SI + 0xe],AX
0000:30b6  83 44 0c 04        ADD word ptr [SI + 0xc],0x4
0000:30ba  e9 19 ff           JMP 0x0000:2fd6
0000:30bd  8b 5c 0c           MOV BX,word ptr [SI + 0xc]
0000:30c0  83 eb 02           SUB BX,0x2
0000:30c3  ff 48 0e           DEC word ptr [BX + SI + 0xe]
0000:30c6  74 0c              JZ 0x0000:30d4
0000:30c8  83 eb 02           SUB BX,0x2
0000:30cb  8b 40 0e           MOV AX,word ptr [BX + SI + 0xe]
0000:30ce  89 44 08           MOV word ptr [SI + 0x8],AX
0000:30d1  e9 02 ff           JMP 0x0000:2fd6
0000:30d4  83 6c 0c 04        SUB word ptr [SI + 0xc],0x4
0000:30d8  89 7c 08           MOV word ptr [SI + 0x8],DI
0000:30db  e9 f8 fe           JMP 0x0000:2fd6
0000:30de  56                 PUSH SI
0000:30df  1e                 PUSH DS
0000:30e0  8b 46 32           MOV AX,word ptr [BP + 0x32]
0000:30e3  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:30e6  8c c8              MOV AX,CS
0000:30e8  8e d8              MOV AX,DS
0000:30ea  33 db              XOR BX,BX
0000:30ec  26 8a 5d 03        MOV BL,byte ptr ES:[DI + 0x3]
0000:30f0  d1 e3              SHL BX,0x1
0000:30f2  53                 PUSH BX
0000:30f3  33 db              XOR BX,BX
0000:30f5  26 8a 25           MOV AH,byte ptr ES:[DI]
0000:30f8  8a dc              MOV BL,AH
0000:30fa  03 dd              ADD BX,BP
0000:30fc  36 8b 1f           MOV BX,word ptr SS:[BX]
0000:30ff  53                 PUSH BX
0000:3100  26 8b 5d 01        MOV BX,word ptr ES:[DI + 0x1]
0000:3104  b0 01              MOV AL,0x1
0000:3106  33 f6              XOR SI,SI
0000:3108  33 ff              XOR DI,DI
0000:310a  ff 5e 0c           CALLF [BP + 0xc]
0000:310d  07                 POP ES
0000:310e  5b                 POP BX
0000:310f  1f                 POP DS
0000:3110  5e                 POP SI
0000:3111  8c 44 0a           MOV word ptr [SI + 0xa],ES
0000:3114  26 8b 3f           MOV DI,word ptr ES:[BX]
0000:3117  89 7c 08           MOV word ptr [SI + 0x8],DI
0000:311a  e9 b9 fe           JMP 0x0000:2fd6
0000:311d  26 8b 05           MOV AX,word ptr ES:[DI]
0000:3120  89 46 60           MOV word ptr [BP + 0x60],AX
0000:3123  83 44 08 02        ADD word ptr [SI + 0x8],0x2
0000:3127  e9 ac fe           JMP 0x0000:2fd6
0000:312a  8b 46 62           MOV AX,word ptr [BP + 0x62]
0000:312d  89 46 60           MOV word ptr [BP + 0x60],AX
0000:3130  e9 a3 fe           JMP 0x0000:2fd6
0000:3133  26 8b 05           MOV AX,word ptr ES:[DI]
0000:3136  89 46 60           MOV word ptr [BP + 0x60],AX
0000:3139  89 46 62           MOV word ptr [BP + 0x62],AX
0000:313c  83 44 08 02        ADD word ptr [SI + 0x8],0x2
0000:3140  e9 93 fe           JMP 0x0000:2fd6
0000:3143  33 db              XOR BX,BX
0000:3145  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3148  03 5e 54           ADD BX,word ptr [BP + 0x54]
0000:314b  53                 PUSH BX
0000:314c  36 8a 07           MOV AL,byte ptr SS:[BX]
0000:314f  26 8a 65 02        MOV AH,byte ptr ES:[DI + 0x2]
0000:3153  33 db              XOR BX,BX
0000:3155  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3159  81 c3 ae 3b        ADD BX,0x3bae
0000:315d  2e ff 17           CALL word ptr CS:[BX]
0000:3160  5b                 POP BX
0000:3161  36 88 07           MOV byte ptr SS:[BX],AL
0000:3164  83 44 08 03        ADD word ptr [SI + 0x8],0x3
0000:3168  e9 6b fe           JMP 0x0000:2fd6
0000:316b  33 db              XOR BX,BX
0000:316d  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3170  03 5e 54           ADD BX,word ptr [BP + 0x54]
0000:3173  53                 PUSH BX
0000:3174  36 8a 07           MOV AL,byte ptr SS:[BX]
0000:3177  33 db              XOR BX,BX
0000:3179  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:317d  03 5e 54           ADD BX,word ptr [BP + 0x54]
0000:3180  36 8a 27           MOV AH,byte ptr SS:[BX]
0000:3183  33 db              XOR BX,BX
0000:3185  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3189  81 c3 ae 3b        ADD BX,0x3bae
0000:318d  2e ff 17           CALL word ptr CS:[BX]
0000:3190  5b                 POP BX
0000:3191  36 88 07           MOV byte ptr SS:[BX],AL
0000:3194  83 44 08 03        ADD word ptr [SI + 0x8],0x3
0000:3198  e9 3b fe           JMP 0x0000:2fd6
0000:319b  26 8a 0d           MOV CL,byte ptr ES:[DI]
0000:319e  26 ff 75 01        PUSH word ptr ES:[DI + 0x1]
0000:31a2  83 c7 03           ADD DI,0x3
0000:31a5  0a c9              OR CL,CL
0000:31a7  74 36              JZ 0x0000:31df
0000:31a9  33 db              XOR BX,BX
0000:31ab  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:31ae  03 5e 54           ADD BX,word ptr [BP + 0x54]
0000:31b1  33 c0              XOR AX,AX
0000:31b3  33 d2              XOR DX,DX
0000:31b5  36 8a 07           MOV AL,byte ptr SS:[BX]
0000:31b8  26 8a 55 02        MOV DL,byte ptr ES:[DI + 0x2]
0000:31bc  33 db              XOR BX,BX
0000:31be  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:31c2  81 c3 b8 3c        ADD BX,0x3cb8
0000:31c6  83 c7 03           ADD DI,0x3
0000:31c9  fe c9              DEC CL
0000:31cb  2e ff 17           CALL word ptr CS:[BX]
0000:31ce  73 d5              JNC 0x0000:31a5
0000:31d0  32 ed              XOR CH,CH
0000:31d2  03 f9              ADD DI,CX
0000:31d4  03 f9              ADD DI,CX
0000:31d6  03 f9              ADD DI,CX
0000:31d8  89 7c 08           MOV word ptr [SI + 0x8],DI
0000:31db  58                 POP AX
0000:31dc  e9 f7 fd           JMP 0x0000:2fd6
0000:31df  5f                 POP DI
0000:31e0  89 7c 08           MOV word ptr [SI + 0x8],DI
0000:31e3  e9 f0 fd           JMP 0x0000:2fd6
0000:31e6  26 8a 0d           MOV CL,byte ptr ES:[DI]
0000:31e9  26 ff 75 01        PUSH word ptr ES:[DI + 0x1]
0000:31ed  83 c7 03           ADD DI,0x3
0000:31f0  0a c9              OR CL,CL
0000:31f2  74 36              JZ 0x0000:322a
0000:31f4  33 db              XOR BX,BX
0000:31f6  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:31f9  03 5e 54           ADD BX,word ptr [BP + 0x54]
0000:31fc  33 c0              XOR AX,AX
0000:31fe  33 d2              XOR DX,DX
0000:3200  36 8a 07           MOV AL,byte ptr SS:[BX]
0000:3203  26 8a 55 02        MOV DL,byte ptr ES:[DI + 0x2]
0000:3207  33 db              XOR BX,BX
0000:3209  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:320d  81 c3 b8 3c        ADD BX,0x3cb8
0000:3211  83 c7 03           ADD DI,0x3
0000:3214  fe c9              DEC CL
0000:3216  2e ff 17           CALL word ptr CS:[BX]
0000:3219  73 d5              JNC 0x0000:31f0
0000:321b  32 ed              XOR CH,CH
0000:321d  03 f9              ADD DI,CX
0000:321f  03 f9              ADD DI,CX
0000:3221  03 f9              ADD DI,CX
0000:3223  89 7c 08           MOV word ptr [SI + 0x8],DI
0000:3226  58                 POP AX
0000:3227  e9 ac fd           JMP 0x0000:2fd6
0000:322a  58                 POP AX
0000:322b  8b 5c 0c           MOV BX,word ptr [SI + 0xc]
0000:322e  83 44 0c 02        ADD word ptr [SI + 0xc],0x2
0000:3232  89 78 0e           MOV word ptr [BX + SI + 0xe],DI
0000:3235  89 44 08           MOV word ptr [SI + 0x8],AX
0000:3238  e9 9b fd           JMP 0x0000:2fd6
0000:323b  b8 00 80           MOV AX,0x8000
0000:323e  ba d6 2f           MOV DX,0x2fd6
0000:3241  56                 PUSH SI
0000:3242  bb 52 00           MOV BX,0x52
0000:3245  b9 00 01           MOV CX,0x100
0000:3248  8b 76 52           MOV SI,word ptr [BP + 0x52]
0000:324b  e8 f3 e7           CALL 0x0000:1a41
0000:324e  72 0f              JC 0x0000:325f
0000:3250  8c 44 0a           MOV word ptr [SI + 0xa],ES
0000:3253  26 8b 05           MOV AX,word ptr ES:[DI]
0000:3256  89 44 08           MOV word ptr [SI + 0x8],AX
0000:3259  26 8b 45 02        MOV AX,word ptr ES:[DI + 0x2]
0000:325d  09 04              OR word ptr [SI],AX
0000:325f  5e                 POP SI
0000:3260  83 44 08 04        ADD word ptr [SI + 0x8],0x4
0000:3264  e9 6f fd           JMP 0x0000:2fd6
0000:3267  b8 00 80           MOV AX,0x8000
0000:326a  ba d6 2f           MOV DX,0x2fd6
0000:326d  56                 PUSH SI
0000:326e  bb 52 00           MOV BX,0x52
0000:3271  b9 00 01           MOV CX,0x100
0000:3274  8b 76 52           MOV SI,word ptr [BP + 0x52]
0000:3277  e8 d9 e7           CALL 0x0000:1a53
0000:327a  72 0f              JC 0x0000:328b
0000:327c  8c 44 0a           MOV word ptr [SI + 0xa],ES
0000:327f  26 8b 05           MOV AX,word ptr ES:[DI]
0000:3282  89 44 08           MOV word ptr [SI + 0x8],AX
0000:3285  26 8b 45 02        MOV AX,word ptr ES:[DI + 0x2]
0000:3289  09 04              OR word ptr [SI],AX
0000:328b  5e                 POP SI
0000:328c  83 44 08 04        ADD word ptr [SI + 0x8],0x4
0000:3290  e9 43 fd           JMP 0x0000:2fd6
0000:3293  b8 00 52           MOV AX,0x5200
0000:3296  8b de              MOV BX,SI
0000:3298  2b 5e 52           SUB BX,word ptr [BP + 0x52]
0000:329b  2b c3              SUB AX,BX
0000:329d  bb 52 00           MOV BX,0x52
0000:32a0  33 d2              XOR DX,DX
0000:32a2  8b cb              MOV CX,BX
0000:32a4  f7 f1              DIV CX
0000:32a6  8b c8              MOV CX,AX
0000:32a8  b8 00 80           MOV AX,0x8000
0000:32ab  ba d6 2f           MOV DX,0x2fd6
0000:32ae  56                 PUSH SI
0000:32af  e8 8f e7           CALL 0x0000:1a41
0000:32b2  72 0f              JC 0x0000:32c3
0000:32b4  8c 44 0a           MOV word ptr [SI + 0xa],ES
0000:32b7  26 8b 05           MOV AX,word ptr ES:[DI]
0000:32ba  89 44 08           MOV word ptr [SI + 0x8],AX
0000:32bd  26 8b 45 02        MOV AX,word ptr ES:[DI + 0x2]
0000:32c1  09 04              OR word ptr [SI],AX
0000:32c3  5e                 POP SI
0000:32c4  83 44 08 04        ADD word ptr [SI + 0x8],0x4
0000:32c8  e9 0b fd           JMP 0x0000:2fd6
0000:32cb  56                 PUSH SI
0000:32cc  bb 52 00           MOV BX,0x52
0000:32cf  b9 00 01           MOV CX,0x100
0000:32d2  26 8b 45 04        MOV AX,word ptr ES:[DI + 0x4]
0000:32d6  f7 e3              MUL BX
0000:32d8  8b d6              MOV DX,SI
0000:32da  2b 56 52           SUB DX,word ptr [BP + 0x52]
0000:32dd  3b c2              CMP AX,DX
0000:32df  72 0b              JC 0x0000:32ec
0000:32e1  26 2b 4d 04        SUB CX,word ptr ES:[DI + 0x4]
0000:32e5  03 46 52           ADD AX,word ptr [BP + 0x52]
0000:32e8  8b f0              MOV SI,AX
0000:32ea  eb 08              JMP 0x0000:32f4
0000:32ec  8b c2              MOV AX,DX
0000:32ee  33 d2              XOR DX,DX
0000:32f0  f7 f3              DIV BX
0000:32f2  2b c8              SUB CX,AX
0000:32f4  b8 00 80           MOV AX,0x8000
0000:32f7  ba d6 2f           MOV DX,0x2fd6
0000:32fa  e8 44 e7           CALL 0x0000:1a41
0000:32fd  72 0f              JC 0x0000:330e
0000:32ff  8c 44 0a           MOV word ptr [SI + 0xa],ES
0000:3302  26 8b 05           MOV AX,word ptr ES:[DI]
0000:3305  89 44 08           MOV word ptr [SI + 0x8],AX
0000:3308  26 8b 45 02        MOV AX,word ptr ES:[DI + 0x2]
0000:330c  09 04              OR word ptr [SI],AX
0000:330e  5e                 POP SI
0000:330f  83 44 08 06        ADD word ptr [SI + 0x8],0x6
0000:3313  e9 c0 fc           JMP 0x0000:2fd6
0000:3316  56                 PUSH SI
0000:3317  bb 52 00           MOV BX,0x52
0000:331a  b9 00 01           MOV CX,0x100
0000:331d  26 8b 45 04        MOV AX,word ptr ES:[DI + 0x4]
0000:3321  2b c8              SUB CX,AX
0000:3323  f7 e3              MUL BX
0000:3325  03 46 52           ADD AX,word ptr [BP + 0x52]
0000:3328  8b f0              MOV SI,AX
0000:332a  b8 00 80           MOV AX,0x8000
0000:332d  ba d6 2f           MOV DX,0x2fd6
0000:3330  e8 0e e7           CALL 0x0000:1a41
0000:3333  72 0f              JC 0x0000:3344
0000:3335  8c 44 0a           MOV word ptr [SI + 0xa],ES
0000:3338  26 8b 05           MOV AX,word ptr ES:[DI]
0000:333b  89 44 08           MOV word ptr [SI + 0x8],AX
0000:333e  26 8b 45 02        MOV AX,word ptr ES:[DI + 0x2]
0000:3342  09 04              OR word ptr [SI],AX
0000:3344  5e                 POP SI
0000:3345  83 44 08 06        ADD word ptr [SI + 0x8],0x6
0000:3349  e9 8a fc           JMP 0x0000:2fd6
0000:334c  33 db              XOR BX,BX
0000:334e  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3351  03 de              ADD BX,SI
0000:3353  33 c0              XOR AX,AX
0000:3355  8a 07              MOV AL,byte ptr [BX]
0000:3357  33 d2              XOR DX,DX
0000:3359  26 8a 55 02        MOV DL,byte ptr ES:[DI + 0x2]
0000:335d  33 db              XOR BX,BX
0000:335f  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3363  81 c3 b8 3c        ADD BX,0x3cb8
0000:3367  2e ff 17           CALL word ptr CS:[BX]
0000:336a  73 07              JNC 0x0000:3373
0000:336c  83 44 08 05        ADD word ptr [SI + 0x8],0x5
0000:3370  e9 63 fc           JMP 0x0000:2fd6
0000:3373  26 8b 45 03        MOV AX,word ptr ES:[DI + 0x3]
0000:3377  89 44 08           MOV word ptr [SI + 0x8],AX
0000:337a  e9 59 fc           JMP 0x0000:2fd6
0000:337d  33 db              XOR BX,BX
0000:337f  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3382  03 de              ADD BX,SI
0000:3384  8b 07              MOV AX,word ptr [BX]
0000:3386  26 8b 55 02        MOV DX,word ptr ES:[DI + 0x2]
0000:338a  33 db              XOR BX,BX
0000:338c  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3390  81 c3 b8 3c        ADD BX,0x3cb8
0000:3394  2e ff 17           CALL word ptr CS:[BX]
0000:3397  73 07              JNC 0x0000:33a0
0000:3399  83 44 08 06        ADD word ptr [SI + 0x8],0x6
0000:339d  e9 36 fc           JMP 0x0000:2fd6
0000:33a0  26 8b 45 04        MOV AX,word ptr ES:[DI + 0x4]
0000:33a4  89 44 08           MOV word ptr [SI + 0x8],AX
0000:33a7  e9 2c fc           JMP 0x0000:2fd6
0000:33aa  33 db              XOR BX,BX
0000:33ac  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:33af  03 de              ADD BX,SI
0000:33b1  33 c0              XOR AX,AX
0000:33b3  8a 07              MOV AL,byte ptr [BX]
0000:33b5  33 db              XOR BX,BX
0000:33b7  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:33bb  03 de              ADD BX,SI
0000:33bd  33 d2              XOR DX,DX
0000:33bf  8a 17              MOV DL,byte ptr [BX]
0000:33c1  33 db              XOR BX,BX
0000:33c3  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:33c7  81 c3 b8 3c        ADD BX,0x3cb8
0000:33cb  2e ff 17           CALL word ptr CS:[BX]
0000:33ce  73 07              JNC 0x0000:33d7
0000:33d0  83 44 08 05        ADD word ptr [SI + 0x8],0x5
0000:33d4  e9 ff fb           JMP 0x0000:2fd6
0000:33d7  26 8b 45 03        MOV AX,word ptr ES:[DI + 0x3]
0000:33db  89 44 08           MOV word ptr [SI + 0x8],AX
0000:33de  e9 f5 fb           JMP 0x0000:2fd6
0000:33e1  33 db              XOR BX,BX
0000:33e3  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:33e6  03 de              ADD BX,SI
0000:33e8  8b 07              MOV AX,word ptr [BX]
0000:33ea  33 db              XOR BX,BX
0000:33ec  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:33f0  03 de              ADD BX,SI
0000:33f2  8b 17              MOV DX,word ptr [BX]
0000:33f4  33 db              XOR BX,BX
0000:33f6  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:33fa  81 c3 b8 3c        ADD BX,0x3cb8
0000:33fe  2e ff 17           CALL word ptr CS:[BX]
0000:3401  73 07              JNC 0x0000:340a
0000:3403  83 44 08 05        ADD word ptr [SI + 0x8],0x5
0000:3407  e9 cc fb           JMP 0x0000:2fd6
0000:340a  26 8b 45 03        MOV AX,word ptr ES:[DI + 0x3]
0000:340e  89 44 08           MOV word ptr [SI + 0x8],AX
0000:3411  e9 c2 fb           JMP 0x0000:2fd6
0000:3414  33 db              XOR BX,BX
0000:3416  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3419  03 de              ADD BX,SI
0000:341b  33 c0              XOR AX,AX
0000:341d  8a 07              MOV AL,byte ptr [BX]
0000:341f  33 d2              XOR DX,DX
0000:3421  26 8a 55 02        MOV DL,byte ptr ES:[DI + 0x2]
0000:3425  33 db              XOR BX,BX
0000:3427  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:342b  81 c3 b8 3c        ADD BX,0x3cb8
0000:342f  2e ff 17           CALL word ptr CS:[BX]
0000:3432  73 07              JNC 0x0000:343b
0000:3434  83 44 08 05        ADD word ptr [SI + 0x8],0x5
0000:3438  e9 9b fb           JMP 0x0000:2fd6
0000:343b  26 8b 45 03        MOV AX,word ptr ES:[DI + 0x3]
0000:343f  8b 5c 0c           MOV BX,word ptr [SI + 0xc]
0000:3442  83 44 0c 02        ADD word ptr [SI + 0xc],0x2
0000:3446  83 c7 05           ADD DI,0x5
0000:3449  89 78 0e           MOV word ptr [BX + SI + 0xe],DI
0000:344c  89 44 08           MOV word ptr [SI + 0x8],AX
0000:344f  e9 84 fb           JMP 0x0000:2fd6
0000:3452  33 db              XOR BX,BX
0000:3454  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3457  03 de              ADD BX,SI
0000:3459  8b 07              MOV AX,word ptr [BX]
0000:345b  26 8b 55 02        MOV DX,word ptr ES:[DI + 0x2]
0000:345f  33 db              XOR BX,BX
0000:3461  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3465  81 c3 b8 3c        ADD BX,0x3cb8
0000:3469  2e ff 17           CALL word ptr CS:[BX]
0000:346c  73 07              JNC 0x0000:3475
0000:346e  83 44 08 06        ADD word ptr [SI + 0x8],0x6
0000:3472  e9 61 fb           JMP 0x0000:2fd6
0000:3475  26 8b 45 04        MOV AX,word ptr ES:[DI + 0x4]
0000:3479  8b 5c 0c           MOV BX,word ptr [SI + 0xc]
0000:347c  83 44 0c 02        ADD word ptr [SI + 0xc],0x2
0000:3480  83 c7 06           ADD DI,0x6
0000:3483  89 78 0e           MOV word ptr [BX + SI + 0xe],DI
0000:3486  89 44 08           MOV word ptr [SI + 0x8],AX
0000:3489  e9 4a fb           JMP 0x0000:2fd6
0000:348c  33 db              XOR BX,BX
0000:348e  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3491  47                 INC DI
0000:3492  80 fb ff           CMP BL,0xff
0000:3495  74 09              JZ 0x0000:34a0
0000:3497  81 c3 d8 35        ADD BX,0x35d8
0000:349b  2e ff 17           CALL word ptr CS:[BX]
0000:349e  eb ec              JMP 0x0000:348c
0000:34a0  89 7c 08           MOV word ptr [SI + 0x8],DI
0000:34a3  e9 30 fb           JMP 0x0000:2fd6
0000:34a6  26 8b 05           MOV AX,word ptr ES:[DI]
0000:34a9  83 c7 02           ADD DI,0x2
0000:34ac  56                 PUSH SI
0000:34ad  8b 76 52           MOV SI,word ptr [BP + 0x52]
0000:34b0  b9 00 01           MOV CX,0x100
0000:34b3  85 04              TEST word ptr [SI],AX
0000:34b5  74 1a              JZ 0x0000:34d1
0000:34b7  50                 PUSH AX
0000:34b8  51                 PUSH CX
0000:34b9  57                 PUSH DI
0000:34ba  33 db              XOR BX,BX
0000:34bc  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:34bf  47                 INC DI
0000:34c0  80 fb ff           CMP BL,0xff
0000:34c3  74 09              JZ 0x0000:34ce
0000:34c5  81 c3 d8 35        ADD BX,0x35d8
0000:34c9  2e ff 17           CALL word ptr CS:[BX]
0000:34cc  eb ec              JMP 0x0000:34ba
0000:34ce  5f                 POP DI
0000:34cf  59                 POP CX
0000:34d0  58                 POP AX
0000:34d1  83 c6 52           ADD SI,0x52
0000:34d4  e2 dd              LOOP 0x0000:34b3
0000:34d6  33 db              XOR BX,BX
0000:34d8  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:34db  47                 INC DI
0000:34dc  80 fb ff           CMP BL,0xff
0000:34df  74 09              JZ 0x0000:34ea
0000:34e1  81 c3 04 36        ADD BX,0x3604
0000:34e5  2e 03 3f           ADD DI,word ptr CS:[BX]
0000:34e8  eb ec              JMP 0x0000:34d6
0000:34ea  5e                 POP SI
0000:34eb  89 7c 08           MOV word ptr [SI + 0x8],DI
0000:34ee  e9 e5 fa           JMP 0x0000:2fd6
0000:34f1  26 8b 05           MOV AX,word ptr ES:[DI]
0000:34f4  83 c7 02           ADD DI,0x2
0000:34f7  8b 5e 52           MOV BX,word ptr [BP + 0x52]
0000:34fa  b9 00 01           MOV CX,0x100
0000:34fd  85 07              TEST word ptr [BX],AX
0000:34ff  75 1f              JNZ 0x0000:3520
0000:3501  83 c3 52           ADD BX,0x52
0000:3504  e2 f7              LOOP 0x0000:34fd
0000:3506  33 db              XOR BX,BX
0000:3508  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:350b  47                 INC DI
0000:350c  80 fb ff           CMP BL,0xff
0000:350f  74 09              JZ 0x0000:351a
0000:3511  81 c3 04 36        ADD BX,0x3604
0000:3515  2e 03 3f           ADD DI,word ptr CS:[BX]
0000:3518  eb ec              JMP 0x0000:3506
0000:351a  89 7c 08           MOV word ptr [SI + 0x8],DI
0000:351d  e9 b6 fa           JMP 0x0000:2fd6
0000:3520  56                 PUSH SI
0000:3521  8b f3              MOV SI,BX
0000:3523  33 db              XOR BX,BX
0000:3525  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3528  47                 INC DI
0000:3529  80 fb ff           CMP BL,0xff
0000:352c  74 09              JZ 0x0000:3537
0000:352e  81 c3 d8 35        ADD BX,0x35d8
0000:3532  2e ff 17           CALL word ptr CS:[BX]
0000:3535  eb ec              JMP 0x0000:3523
0000:3537  5e                 POP SI
0000:3538  89 7c 08           MOV word ptr [SI + 0x8],DI
0000:353b  e9 98 fa           JMP 0x0000:2fd6
0000:353e  26 8b 05           MOV AX,word ptr ES:[DI]
0000:3541  83 c7 02           ADD DI,0x2
0000:3544  56                 PUSH SI
0000:3545  8b 76 52           MOV SI,word ptr [BP + 0x52]
0000:3548  b9 00 01           MOV CX,0x100
0000:354b  3b 44 04           CMP AX,word ptr [SI + 0x4]
0000:354e  75 1a              JNZ 0x0000:356a
0000:3550  50                 PUSH AX
0000:3551  51                 PUSH CX
0000:3552  57                 PUSH DI
0000:3553  33 db              XOR BX,BX
0000:3555  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3558  47                 INC DI
0000:3559  80 fb ff           CMP BL,0xff
0000:355c  74 09              JZ 0x0000:3567
0000:355e  81 c3 d8 35        ADD BX,0x35d8
0000:3562  2e ff 17           CALL word ptr CS:[BX]
0000:3565  eb ec              JMP 0x0000:3553
0000:3567  5f                 POP DI
0000:3568  59                 POP CX
0000:3569  58                 POP AX
0000:356a  83 c6 52           ADD SI,0x52
0000:356d  e2 dc              LOOP 0x0000:354b
0000:356f  33 db              XOR BX,BX
0000:3571  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3574  47                 INC DI
0000:3575  80 fb ff           CMP BL,0xff
0000:3578  74 09              JZ 0x0000:3583
0000:357a  81 c3 04 36        ADD BX,0x3604
0000:357e  2e 03 3f           ADD DI,word ptr CS:[BX]
0000:3581  eb ec              JMP 0x0000:356f
0000:3583  5e                 POP SI
0000:3584  89 7c 08           MOV word ptr [SI + 0x8],DI
0000:3587  e9 4c fa           JMP 0x0000:2fd6
0000:358a  26 8b 05           MOV AX,word ptr ES:[DI]
0000:358d  83 c7 02           ADD DI,0x2
0000:3590  8b 5e 52           MOV BX,word ptr [BP + 0x52]
0000:3593  b9 00 01           MOV CX,0x100
0000:3596  3b 47 04           CMP AX,word ptr [BX + 0x4]
0000:3599  74 1f              JZ 0x0000:35ba
0000:359b  83 c3 52           ADD BX,0x52
0000:359e  e2 f6              LOOP 0x0000:3596
0000:35a0  33 db              XOR BX,BX
0000:35a2  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:35a5  47                 INC DI
0000:35a6  80 fb ff           CMP BL,0xff
0000:35a9  74 09              JZ 0x0000:35b4
0000:35ab  81 c3 04 36        ADD BX,0x3604
0000:35af  2e 03 3f           ADD DI,word ptr CS:[BX]
0000:35b2  eb ec              JMP 0x0000:35a0
0000:35b4  89 7c 08           MOV word ptr [SI + 0x8],DI
0000:35b7  e9 1c fa           JMP 0x0000:2fd6
0000:35ba  56                 PUSH SI
0000:35bb  8b f3              MOV SI,BX
0000:35bd  33 db              XOR BX,BX
0000:35bf  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:35c2  47                 INC DI
0000:35c3  80 fb ff           CMP BL,0xff
0000:35c6  74 09              JZ 0x0000:35d1
0000:35c8  81 c3 d8 35        ADD BX,0x35d8
0000:35cc  2e ff 17           CALL word ptr CS:[BX]
0000:35cf  eb ec              JMP 0x0000:35bd
0000:35d1  5e                 POP SI
0000:35d2  89 7c 08           MOV word ptr [SI + 0x8],DI
0000:35d5  e9 fe f9           JMP 0x0000:2fd6
0000:3630  33 db              XOR BX,BX
0000:3632  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3635  03 de              ADD BX,SI
0000:3637  53                 PUSH BX
0000:3638  8a 07              MOV AL,byte ptr [BX]
0000:363a  26 8a 65 02        MOV AH,byte ptr ES:[DI + 0x2]
0000:363e  33 db              XOR BX,BX
0000:3640  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3644  81 c3 ae 3b        ADD BX,0x3bae
0000:3648  2e ff 17           CALL word ptr CS:[BX]
0000:364b  5b                 POP BX
0000:364c  88 07              MOV byte ptr [BX],AL
0000:364e  83 c7 03           ADD DI,0x3
0000:3651  c3                 RET
0000:3652  33 db              XOR BX,BX
0000:3654  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3657  03 de              ADD BX,SI
0000:3659  53                 PUSH BX
0000:365a  8b 07              MOV AX,word ptr [BX]
0000:365c  26 8b 55 02        MOV DX,word ptr ES:[DI + 0x2]
0000:3660  33 db              XOR BX,BX
0000:3662  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3666  81 c3 2a 3c        ADD BX,0x3c2a
0000:366a  2e ff 17           CALL word ptr CS:[BX]
0000:366d  5b                 POP BX
0000:366e  89 07              MOV word ptr [BX],AX
0000:3670  83 c7 04           ADD DI,0x4
0000:3673  c3                 RET
0000:3674  33 db              XOR BX,BX
0000:3676  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3679  03 de              ADD BX,SI
0000:367b  53                 PUSH BX
0000:367c  8a 07              MOV AL,byte ptr [BX]
0000:367e  33 db              XOR BX,BX
0000:3680  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:3684  03 de              ADD BX,SI
0000:3686  8a 27              MOV AH,byte ptr [BX]
0000:3688  33 db              XOR BX,BX
0000:368a  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:368e  81 c3 ae 3b        ADD BX,0x3bae
0000:3692  2e ff 17           CALL word ptr CS:[BX]
0000:3695  5b                 POP BX
0000:3696  88 07              MOV byte ptr [BX],AL
0000:3698  83 c7 03           ADD DI,0x3
0000:369b  c3                 RET
0000:369c  33 db              XOR BX,BX
0000:369e  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:36a1  03 de              ADD BX,SI
0000:36a3  53                 PUSH BX
0000:36a4  8b 07              MOV AX,word ptr [BX]
0000:36a6  33 db              XOR BX,BX
0000:36a8  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:36ac  03 de              ADD BX,SI
0000:36ae  8b 17              MOV DX,word ptr [BX]
0000:36b0  33 db              XOR BX,BX
0000:36b2  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:36b6  81 c3 2a 3c        ADD BX,0x3c2a
0000:36ba  2e ff 17           CALL word ptr CS:[BX]
0000:36bd  5b                 POP BX
0000:36be  89 07              MOV word ptr [BX],AX
0000:36c0  83 c7 03           ADD DI,0x3
0000:36c3  c3                 RET
0000:36c4  33 db              XOR BX,BX
0000:36c6  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:36c9  03 de              ADD BX,SI
0000:36cb  53                 PUSH BX
0000:36cc  8a 07              MOV AL,byte ptr [BX]
0000:36ce  33 db              XOR BX,BX
0000:36d0  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:36d4  03 dd              ADD BX,BP
0000:36d6  36 8a 27           MOV AH,byte ptr SS:[BX]
0000:36d9  33 db              XOR BX,BX
0000:36db  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:36df  81 c3 ae 3b        ADD BX,0x3bae
0000:36e3  2e ff 17           CALL word ptr CS:[BX]
0000:36e6  5b                 POP BX
0000:36e7  88 07              MOV byte ptr [BX],AL
0000:36e9  83 c7 03           ADD DI,0x3
0000:36ec  c3                 RET
0000:36ed  33 db              XOR BX,BX
0000:36ef  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:36f2  03 de              ADD BX,SI
0000:36f4  53                 PUSH BX
0000:36f5  8b 07              MOV AX,word ptr [BX]
0000:36f7  33 db              XOR BX,BX
0000:36f9  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:36fd  03 dd              ADD BX,BP
0000:36ff  36 8b 17           MOV DX,word ptr SS:[BX]
0000:3702  33 db              XOR BX,BX
0000:3704  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3708  81 c3 2a 3c        ADD BX,0x3c2a
0000:370c  2e ff 17           CALL word ptr CS:[BX]
0000:370f  5b                 POP BX
0000:3710  89 07              MOV word ptr [BX],AX
0000:3712  83 c7 03           ADD DI,0x3
0000:3715  c3                 RET
0000:3716  33 db              XOR BX,BX
0000:3718  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:371b  03 dd              ADD BX,BP
0000:371d  53                 PUSH BX
0000:371e  36 8a 07           MOV AL,byte ptr SS:[BX]
0000:3721  26 8a 65 02        MOV AH,byte ptr ES:[DI + 0x2]
0000:3725  33 db              XOR BX,BX
0000:3727  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:372b  81 c3 ae 3b        ADD BX,0x3bae
0000:372f  2e ff 17           CALL word ptr CS:[BX]
0000:3732  5b                 POP BX
0000:3733  36 88 07           MOV byte ptr SS:[BX],AL
0000:3736  83 c7 03           ADD DI,0x3
0000:3739  c3                 RET
0000:373a  33 db              XOR BX,BX
0000:373c  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:373f  03 dd              ADD BX,BP
0000:3741  53                 PUSH BX
0000:3742  36 8b 07           MOV AX,word ptr SS:[BX]
0000:3745  26 8b 55 02        MOV DX,word ptr ES:[DI + 0x2]
0000:3749  33 db              XOR BX,BX
0000:374b  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:374f  81 c3 2a 3c        ADD BX,0x3c2a
0000:3753  2e ff 17           CALL word ptr CS:[BX]
0000:3756  5b                 POP BX
0000:3757  36 89 07           MOV word ptr SS:[BX],AX
0000:375a  83 c7 04           ADD DI,0x4
0000:375d  c3                 RET
0000:375e  33 db              XOR BX,BX
0000:3760  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3763  03 dd              ADD BX,BP
0000:3765  53                 PUSH BX
0000:3766  36 8a 07           MOV AL,byte ptr SS:[BX]
0000:3769  33 db              XOR BX,BX
0000:376b  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:376f  03 de              ADD BX,SI
0000:3771  8a 27              MOV AH,byte ptr [BX]
0000:3773  33 db              XOR BX,BX
0000:3775  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3779  81 c3 ae 3b        ADD BX,0x3bae
0000:377d  2e ff 17           CALL word ptr CS:[BX]
0000:3780  5b                 POP BX
0000:3781  36 88 07           MOV byte ptr SS:[BX],AL
0000:3784  83 c7 03           ADD DI,0x3
0000:3787  c3                 RET
0000:3788  33 db              XOR BX,BX
0000:378a  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:378d  03 dd              ADD BX,BP
0000:378f  53                 PUSH BX
0000:3790  36 8b 07           MOV AX,word ptr SS:[BX]
0000:3793  33 db              XOR BX,BX
0000:3795  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:3799  03 de              ADD BX,SI
0000:379b  8b 17              MOV DX,word ptr [BX]
0000:379d  33 db              XOR BX,BX
0000:379f  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:37a3  81 c3 2a 3c        ADD BX,0x3c2a
0000:37a7  2e ff 17           CALL word ptr CS:[BX]
0000:37aa  5b                 POP BX
0000:37ab  36 89 07           MOV word ptr SS:[BX],AX
0000:37ae  83 c7 03           ADD DI,0x3
0000:37b1  c3                 RET
0000:37b2  33 db              XOR BX,BX
0000:37b4  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:37b7  03 de              ADD BX,SI
0000:37b9  53                 PUSH BX
0000:37ba  8a 07              MOV AL,byte ptr [BX]
0000:37bc  33 db              XOR BX,BX
0000:37be  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:37c2  03 dd              ADD BX,BP
0000:37c4  36 8a 27           MOV AH,byte ptr SS:[BX]
0000:37c7  33 db              XOR BX,BX
0000:37c9  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:37cd  81 c3 ae 3b        ADD BX,0x3bae
0000:37d1  2e ff 17           CALL word ptr CS:[BX]
0000:37d4  5b                 POP BX
0000:37d5  88 07              MOV byte ptr [BX],AL
0000:37d7  83 c7 03           ADD DI,0x3
0000:37da  c3                 RET
0000:37db  33 db              XOR BX,BX
0000:37dd  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:37e0  03 de              ADD BX,SI
0000:37e2  53                 PUSH BX
0000:37e3  8b 07              MOV AX,word ptr [BX]
0000:37e5  33 db              XOR BX,BX
0000:37e7  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:37eb  03 dd              ADD BX,BP
0000:37ed  36 8b 17           MOV DX,word ptr SS:[BX]
0000:37f0  33 db              XOR BX,BX
0000:37f2  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:37f6  81 c3 2a 3c        ADD BX,0x3c2a
0000:37fa  2e ff 17           CALL word ptr CS:[BX]
0000:37fd  5b                 POP BX
0000:37fe  89 07              MOV word ptr [BX],AX
0000:3800  83 c7 03           ADD DI,0x3
0000:3803  c3                 RET
0000:3804  33 db              XOR BX,BX
0000:3806  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3809  03 de              ADD BX,SI
0000:380b  53                 PUSH BX
0000:380c  8a 07              MOV AL,byte ptr [BX]
0000:380e  33 db              XOR BX,BX
0000:3810  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:3814  03 de              ADD BX,SI
0000:3816  8a 1f              MOV BL,byte ptr [BX]
0000:3818  32 ff              XOR BH,BH
0000:381a  03 dd              ADD BX,BP
0000:381c  36 8a 27           MOV AH,byte ptr SS:[BX]
0000:381f  33 db              XOR BX,BX
0000:3821  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3825  81 c3 ae 3b        ADD BX,0x3bae
0000:3829  2e ff 17           CALL word ptr CS:[BX]
0000:382c  5b                 POP BX
0000:382d  88 07              MOV byte ptr [BX],AL
0000:382f  83 c7 03           ADD DI,0x3
0000:3832  c3                 RET
0000:3833  33 db              XOR BX,BX
0000:3835  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3838  03 de              ADD BX,SI
0000:383a  53                 PUSH BX
0000:383b  8b 07              MOV AX,word ptr [BX]
0000:383d  33 db              XOR BX,BX
0000:383f  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:3843  03 de              ADD BX,SI
0000:3845  8a 1f              MOV BL,byte ptr [BX]
0000:3847  32 ff              XOR BH,BH
0000:3849  03 dd              ADD BX,BP
0000:384b  36 8b 17           MOV DX,word ptr SS:[BX]
0000:384e  33 db              XOR BX,BX
0000:3850  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3854  81 c3 2a 3c        ADD BX,0x3c2a
0000:3858  2e ff 17           CALL word ptr CS:[BX]
0000:385b  5b                 POP BX
0000:385c  89 07              MOV word ptr [BX],AX
0000:385e  83 c7 03           ADD DI,0x3
0000:3861  c3                 RET
0000:3862  83 c7 04           ADD DI,0x4
0000:3865  c3                 RET
0000:3866  33 db              XOR BX,BX
0000:3868  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:386c  03 de              ADD BX,SI
0000:386e  8b 1f              MOV BX,word ptr [BX]
0000:3870  8a df              MOV BL,BH
0000:3872  e8 0b d6           CALL 0x0000:0e80
0000:3875  8a c4              MOV AL,AH
0000:3877  33 db              XOR BX,BX
0000:3879  26 8a 5d 03        MOV BL,byte ptr ES:[DI + 0x3]
0000:387d  03 de              ADD BX,SI
0000:387f  8b 17              MOV DX,word ptr [BX]
0000:3881  f6 ee              IMUL DH
0000:3883  8b d0              MOV DX,AX
0000:3885  33 db              XOR BX,BX
0000:3887  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:388a  03 de              ADD BX,SI
0000:388c  53                 PUSH BX
0000:388d  8b 07              MOV AX,word ptr [BX]
0000:388f  33 db              XOR BX,BX
0000:3891  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3895  81 c3 2a 3c        ADD BX,0x3c2a
0000:3899  2e ff 17           CALL word ptr CS:[BX]
0000:389c  5b                 POP BX
0000:389d  89 07              MOV word ptr [BX],AX
0000:389f  83 c7 04           ADD DI,0x4
0000:38a2  c3                 RET
0000:38a3  83 c7 04           ADD DI,0x4
0000:38a6  c3                 RET
0000:38a7  33 db              XOR BX,BX
0000:38a9  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:38ad  03 de              ADD BX,SI
0000:38af  8b 1f              MOV BX,word ptr [BX]
0000:38b1  81 c3 00 40        ADD BX,0x4000
0000:38b5  8a df              MOV BL,BH
0000:38b7  e8 c6 d5           CALL 0x0000:0e80
0000:38ba  8a c4              MOV AL,AH
0000:38bc  33 db              XOR BX,BX
0000:38be  26 8a 5d 03        MOV BL,byte ptr ES:[DI + 0x3]
0000:38c2  03 de              ADD BX,SI
0000:38c4  8b 17              MOV DX,word ptr [BX]
0000:38c6  f6 ee              IMUL DH
0000:38c8  8b d0              MOV DX,AX
0000:38ca  33 db              XOR BX,BX
0000:38cc  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:38cf  03 de              ADD BX,SI
0000:38d1  53                 PUSH BX
0000:38d2  8b 07              MOV AX,word ptr [BX]
0000:38d4  33 db              XOR BX,BX
0000:38d6  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:38da  81 c3 2a 3c        ADD BX,0x3c2a
0000:38de  2e ff 17           CALL word ptr CS:[BX]
0000:38e1  5b                 POP BX
0000:38e2  89 07              MOV word ptr [BX],AX
0000:38e4  83 c7 04           ADD DI,0x4
0000:38e7  c3                 RET
0000:38e8  83 c7 04           ADD DI,0x4
0000:38eb  c3                 RET
0000:38ec  33 db              XOR BX,BX
0000:38ee  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:38f2  03 dd              ADD BX,BP
0000:38f4  36 8b 0f           MOV CX,word ptr SS:[BX]
0000:38f7  33 db              XOR BX,BX
0000:38f9  26 8a 5d 03        MOV BL,byte ptr ES:[DI + 0x3]
0000:38fd  03 dd              ADD BX,BP
0000:38ff  36 8b 17           MOV DX,word ptr SS:[BX]
0000:3902  e8 c1 d5           CALL 0x0000:0ec6
0000:3905  8b d0              MOV DX,AX
0000:3907  33 db              XOR BX,BX
0000:3909  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:390c  03 de              ADD BX,SI
0000:390e  53                 PUSH BX
0000:390f  8b 07              MOV AX,word ptr [BX]
0000:3911  33 db              XOR BX,BX
0000:3913  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3917  81 c3 2a 3c        ADD BX,0x3c2a
0000:391b  2e ff 17           CALL word ptr CS:[BX]
0000:391e  5b                 POP BX
0000:391f  89 07              MOV word ptr [BX],AX
0000:3921  83 c7 04           ADD DI,0x4
0000:3924  c3                 RET
0000:3925  83 c7 04           ADD DI,0x4
0000:3928  c3                 RET
0000:3929  33 db              XOR BX,BX
0000:392b  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:392f  03 dd              ADD BX,BP
0000:3931  36 8b 0f           MOV CX,word ptr SS:[BX]
0000:3934  33 db              XOR BX,BX
0000:3936  26 8a 5d 03        MOV BL,byte ptr ES:[DI + 0x3]
0000:393a  03 dd              ADD BX,BP
0000:393c  36 8b 17           MOV DX,word ptr SS:[BX]
0000:393f  e8 46 d6           CALL 0x0000:0f88
0000:3942  8b d0              MOV DX,AX
0000:3944  33 db              XOR BX,BX
0000:3946  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3949  03 de              ADD BX,SI
0000:394b  53                 PUSH BX
0000:394c  8b 07              MOV AX,word ptr [BX]
0000:394e  33 db              XOR BX,BX
0000:3950  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3954  81 c3 2a 3c        ADD BX,0x3c2a
0000:3958  2e ff 17           CALL word ptr CS:[BX]
0000:395b  5b                 POP BX
0000:395c  89 07              MOV word ptr [BX],AX
0000:395e  83 c7 04           ADD DI,0x4
0000:3961  c3                 RET
0000:3962  33 db              XOR BX,BX
0000:3964  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3967  03 dd              ADD BX,BP
0000:3969  33 c0              XOR AX,AX
0000:396b  36 8a 07           MOV AL,byte ptr SS:[BX]
0000:396e  33 d2              XOR DX,DX
0000:3970  26 8a 55 02        MOV DL,byte ptr ES:[DI + 0x2]
0000:3974  33 db              XOR BX,BX
0000:3976  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:397a  81 c3 b8 3c        ADD BX,0x3cb8
0000:397e  2e ff 17           CALL word ptr CS:[BX]
0000:3981  73 07              JNC 0x0000:398a
0000:3983  83 44 08 05        ADD word ptr [SI + 0x8],0x5
0000:3987  e9 4c f6           JMP 0x0000:2fd6
0000:398a  26 8b 45 03        MOV AX,word ptr ES:[DI + 0x3]
0000:398e  89 44 08           MOV word ptr [SI + 0x8],AX
0000:3991  e9 42 f6           JMP 0x0000:2fd6
0000:3994  33 db              XOR BX,BX
0000:3996  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3999  03 dd              ADD BX,BP
0000:399b  36 8b 07           MOV AX,word ptr SS:[BX]
0000:399e  26 8b 55 02        MOV DX,word ptr ES:[DI + 0x2]
0000:39a2  33 db              XOR BX,BX
0000:39a4  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:39a8  81 c3 b8 3c        ADD BX,0x3cb8
0000:39ac  2e ff 17           CALL word ptr CS:[BX]
0000:39af  73 07              JNC 0x0000:39b8
0000:39b1  83 44 08 06        ADD word ptr [SI + 0x8],0x6
0000:39b5  e9 1e f6           JMP 0x0000:2fd6
0000:39b8  26 8b 45 04        MOV AX,word ptr ES:[DI + 0x4]
0000:39bc  89 44 08           MOV word ptr [SI + 0x8],AX
0000:39bf  e9 14 f6           JMP 0x0000:2fd6
0000:39c2  33 db              XOR BX,BX
0000:39c4  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:39c7  03 dd              ADD BX,BP
0000:39c9  33 c0              XOR AX,AX
0000:39cb  36 8a 07           MOV AL,byte ptr SS:[BX]
0000:39ce  33 db              XOR BX,BX
0000:39d0  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:39d4  03 dd              ADD BX,BP
0000:39d6  33 d2              XOR DX,DX
0000:39d8  36 8a 17           MOV DL,byte ptr SS:[BX]
0000:39db  33 db              XOR BX,BX
0000:39dd  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:39e1  81 c3 b8 3c        ADD BX,0x3cb8
0000:39e5  2e ff 17           CALL word ptr CS:[BX]
0000:39e8  73 07              JNC 0x0000:39f1
0000:39ea  83 44 08 05        ADD word ptr [SI + 0x8],0x5
0000:39ee  e9 e5 f5           JMP 0x0000:2fd6
0000:39f1  26 8b 45 03        MOV AX,word ptr ES:[DI + 0x3]
0000:39f5  89 44 08           MOV word ptr [SI + 0x8],AX
0000:39f8  e9 db f5           JMP 0x0000:2fd6
0000:39fb  33 db              XOR BX,BX
0000:39fd  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3a00  03 dd              ADD BX,BP
0000:3a02  36 8b 07           MOV AX,word ptr SS:[BX]
0000:3a05  33 db              XOR BX,BX
0000:3a07  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:3a0b  03 dd              ADD BX,BP
0000:3a0d  36 8b 17           MOV DX,word ptr SS:[BX]
0000:3a10  33 db              XOR BX,BX
0000:3a12  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3a16  81 c3 b8 3c        ADD BX,0x3cb8
0000:3a1a  2e ff 17           CALL word ptr CS:[BX]
0000:3a1d  73 07              JNC 0x0000:3a26
0000:3a1f  83 44 08 05        ADD word ptr [SI + 0x8],0x5
0000:3a23  e9 b0 f5           JMP 0x0000:2fd6
0000:3a26  26 8b 45 03        MOV AX,word ptr ES:[DI + 0x3]
0000:3a2a  89 44 08           MOV word ptr [SI + 0x8],AX
0000:3a2d  e9 a6 f5           JMP 0x0000:2fd6
0000:3a30  33 db              XOR BX,BX
0000:3a32  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3a35  03 dd              ADD BX,BP
0000:3a37  33 c0              XOR AX,AX
0000:3a39  36 8a 07           MOV AL,byte ptr SS:[BX]
0000:3a3c  33 d2              XOR DX,DX
0000:3a3e  26 8a 55 02        MOV DL,byte ptr ES:[DI + 0x2]
0000:3a42  33 db              XOR BX,BX
0000:3a44  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3a48  81 c3 b8 3c        ADD BX,0x3cb8
0000:3a4c  2e ff 17           CALL word ptr CS:[BX]
0000:3a4f  73 07              JNC 0x0000:3a58
0000:3a51  83 44 08 05        ADD word ptr [SI + 0x8],0x5
0000:3a55  e9 7e f5           JMP 0x0000:2fd6
0000:3a58  26 8b 45 03        MOV AX,word ptr ES:[DI + 0x3]
0000:3a5c  8b 5c 0c           MOV BX,word ptr [SI + 0xc]
0000:3a5f  83 44 0c 02        ADD word ptr [SI + 0xc],0x2
0000:3a63  83 c7 05           ADD DI,0x5
0000:3a66  89 78 0e           MOV word ptr [BX + SI + 0xe],DI
0000:3a69  89 44 08           MOV word ptr [SI + 0x8],AX
0000:3a6c  e9 67 f5           JMP 0x0000:2fd6
0000:3a6f  33 db              XOR BX,BX
0000:3a71  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3a74  03 dd              ADD BX,BP
0000:3a76  36 8b 07           MOV AX,word ptr SS:[BX]
0000:3a79  26 8b 55 02        MOV DX,word ptr ES:[DI + 0x2]
0000:3a7d  33 db              XOR BX,BX
0000:3a7f  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3a83  81 c3 b8 3c        ADD BX,0x3cb8
0000:3a87  2e ff 17           CALL word ptr CS:[BX]
0000:3a8a  73 07              JNC 0x0000:3a93
0000:3a8c  83 44 08 06        ADD word ptr [SI + 0x8],0x6
0000:3a90  e9 43 f5           JMP 0x0000:2fd6
0000:3a93  26 8b 45 04        MOV AX,word ptr ES:[DI + 0x4]
0000:3a97  8b 5c 0c           MOV BX,word ptr [SI + 0xc]
0000:3a9a  83 44 0c 02        ADD word ptr [SI + 0xc],0x2
0000:3a9e  83 c7 06           ADD DI,0x6
0000:3aa1  89 78 0e           MOV word ptr [BX + SI + 0xe],DI
0000:3aa4  89 44 08           MOV word ptr [SI + 0x8],AX
0000:3aa7  e9 2c f5           JMP 0x0000:2fd6
0000:3aaa  33 db              XOR BX,BX
0000:3aac  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3aaf  03 dd              ADD BX,BP
0000:3ab1  53                 PUSH BX
0000:3ab2  36 8a 07           MOV AL,byte ptr SS:[BX]
0000:3ab5  26 8a 65 02        MOV AH,byte ptr ES:[DI + 0x2]
0000:3ab9  33 db              XOR BX,BX
0000:3abb  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3abf  81 c3 ae 3b        ADD BX,0x3bae
0000:3ac3  2e ff 17           CALL word ptr CS:[BX]
0000:3ac6  5b                 POP BX
0000:3ac7  36 88 07           MOV byte ptr SS:[BX],AL
0000:3aca  83 44 08 03        ADD word ptr [SI + 0x8],0x3
0000:3ace  e9 05 f5           JMP 0x0000:2fd6
0000:3ad1  33 db              XOR BX,BX
0000:3ad3  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3ad6  03 dd              ADD BX,BP
0000:3ad8  53                 PUSH BX
0000:3ad9  36 8b 07           MOV AX,word ptr SS:[BX]
0000:3adc  26 8b 55 02        MOV DX,word ptr ES:[DI + 0x2]
0000:3ae0  33 db              XOR BX,BX
0000:3ae2  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3ae6  81 c3 2a 3c        ADD BX,0x3c2a
0000:3aea  2e ff 17           CALL word ptr CS:[BX]
0000:3aed  5b                 POP BX
0000:3aee  36 89 07           MOV word ptr SS:[BX],AX
0000:3af1  83 44 08 04        ADD word ptr [SI + 0x8],0x4
0000:3af5  e9 de f4           JMP 0x0000:2fd6
0000:3af8  33 db              XOR BX,BX
0000:3afa  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3afd  03 dd              ADD BX,BP
0000:3aff  53                 PUSH BX
0000:3b00  36 8a 07           MOV AL,byte ptr SS:[BX]
0000:3b03  33 db              XOR BX,BX
0000:3b05  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:3b09  03 dd              ADD BX,BP
0000:3b0b  36 8a 27           MOV AH,byte ptr SS:[BX]
0000:3b0e  33 db              XOR BX,BX
0000:3b10  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3b14  81 c3 ae 3b        ADD BX,0x3bae
0000:3b18  2e ff 17           CALL word ptr CS:[BX]
0000:3b1b  5b                 POP BX
0000:3b1c  36 88 07           MOV byte ptr SS:[BX],AL
0000:3b1f  83 44 08 03        ADD word ptr [SI + 0x8],0x3
0000:3b23  e9 b0 f4           JMP 0x0000:2fd6
0000:3b26  33 db              XOR BX,BX
0000:3b28  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3b2b  03 dd              ADD BX,BP
0000:3b2d  53                 PUSH BX
0000:3b2e  36 8b 07           MOV AX,word ptr SS:[BX]
0000:3b31  33 db              XOR BX,BX
0000:3b33  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:3b37  03 dd              ADD BX,BP
0000:3b39  36 8b 17           MOV DX,word ptr SS:[BX]
0000:3b3c  33 db              XOR BX,BX
0000:3b3e  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3b42  81 c3 2a 3c        ADD BX,0x3c2a
0000:3b46  2e ff 17           CALL word ptr CS:[BX]
0000:3b49  5b                 POP BX
0000:3b4a  36 89 07           MOV word ptr SS:[BX],AX
0000:3b4d  83 44 08 03        ADD word ptr [SI + 0x8],0x3
0000:3b51  e9 82 f4           JMP 0x0000:2fd6
0000:3b54  33 db              XOR BX,BX
0000:3b56  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3b59  03 dd              ADD BX,BP
0000:3b5b  53                 PUSH BX
0000:3b5c  36 8a 07           MOV AL,byte ptr SS:[BX]
0000:3b5f  33 db              XOR BX,BX
0000:3b61  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:3b65  03 de              ADD BX,SI
0000:3b67  8a 27              MOV AH,byte ptr [BX]
0000:3b69  33 db              XOR BX,BX
0000:3b6b  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3b6f  81 c3 ae 3b        ADD BX,0x3bae
0000:3b73  2e ff 17           CALL word ptr CS:[BX]
0000:3b76  5b                 POP BX
0000:3b77  36 88 07           MOV byte ptr SS:[BX],AL
0000:3b7a  83 44 08 03        ADD word ptr [SI + 0x8],0x3
0000:3b7e  e9 55 f4           JMP 0x0000:2fd6
0000:3b81  33 db              XOR BX,BX
0000:3b83  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3b86  03 dd              ADD BX,BP
0000:3b88  53                 PUSH BX
0000:3b89  36 8b 07           MOV AX,word ptr SS:[BX]
0000:3b8c  33 db              XOR BX,BX
0000:3b8e  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:3b92  03 de              ADD BX,SI
0000:3b94  8b 17              MOV DX,word ptr [BX]
0000:3b96  33 db              XOR BX,BX
0000:3b98  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3b9c  81 c3 2a 3c        ADD BX,0x3c2a
0000:3ba0  2e ff 17           CALL word ptr CS:[BX]
0000:3ba3  5b                 POP BX
0000:3ba4  36 89 07           MOV word ptr SS:[BX],AX
0000:3ba7  83 44 08 03        ADD word ptr [SI + 0x8],0x3
0000:3bab  e9 28 f4           JMP 0x0000:2fd6
0000:3bd0  8a c4              MOV AL,AH
0000:3bd2  c3                 RET
0000:3bd3  8a c4              MOV AL,AH
0000:3bd5  f6 d8              NEG AL
0000:3bd7  c3                 RET
0000:3bd8  02 c4              ADD AL,AH
0000:3bda  d0 96 bd 00        RCL byte ptr [BP + 0xbd],0x1
0000:3bde  c3                 RET
0000:3bdf  2a c4              SUB AL,AH
0000:3be1  d0 96 bd 00        RCL byte ptr [BP + 0xbd],0x1
0000:3be5  c3                 RET
0000:3be6  d0 9e bd 00        RCR byte ptr [BP + 0xbd],0x1
0000:3bea  12 c4              ADC AL,AH
0000:3bec  d0 96 bd 00        RCL byte ptr [BP + 0xbd],0x1
0000:3bf0  c3                 RET
0000:3bf1  d0 9e bd 00        RCR byte ptr [BP + 0xbd],0x1
0000:3bf5  1a c4              SBB AL,AH
0000:3bf7  d0 96 bd 00        RCL byte ptr [BP + 0xbd],0x1
0000:3bfb  c3                 RET
0000:3bfc  22 c4              AND AL,AH
0000:3bfe  c3                 RET
0000:3bff  0a c4              OR AL,AH
0000:3c01  c3                 RET
0000:3c02  32 c4              XOR AL,AH
0000:3c04  c3                 RET
0000:3c05  f6 e4              MUL AH
0000:3c07  c3                 RET
0000:3c08  8a d4              MOV DL,AH
0000:3c0a  32 e4              XOR AH,AH
0000:3c0c  f6 f2              DIV DL
0000:3c0e  c3                 RET
0000:3c0f  8a d4              MOV DL,AH
0000:3c11  e8 3a d2           CALL 0x0000:0e4e
0000:3c14  32 e4              XOR AH,AH
0000:3c16  f6 f2              DIV DL
0000:3c18  8a c4              MOV AL,AH
0000:3c1a  c3                 RET
0000:3c1b  86 cc              XCHG AH,CL
0000:3c1d  d2 e0              SHL AL,CL
0000:3c1f  86 cc              XCHG AH,CL
0000:3c21  c3                 RET
0000:3c22  86 cc              XCHG AH,CL
0000:3c24  d2 f8              SAR AL,CL
0000:3c26  86 cc              XCHG AH,CL
0000:3c28  c3                 RET
0000:3c4c  8b c2              MOV AX,DX
0000:3c4e  c3                 RET
0000:3c4f  8b c2              MOV AX,DX
0000:3c51  f7 d8              NEG AX
0000:3c53  c3                 RET
0000:3c54  8b c2              MOV AX,DX
0000:3c56  98                 CBW
0000:3c57  c3                 RET
0000:3c58  03 c2              ADD AX,DX
0000:3c5a  d0 96 bd 00        RCL byte ptr [BP + 0xbd],0x1
0000:3c5e  c3                 RET
0000:3c5f  2b c2              SUB AX,DX
0000:3c61  d0 96 bd 00        RCL byte ptr [BP + 0xbd],0x1
0000:3c65  c3                 RET
0000:3c66  d0 9e bd 00        RCR byte ptr [BP + 0xbd],0x1
0000:3c6a  13 c2              ADC AX,DX
0000:3c6c  d0 96 bd 00        RCL byte ptr [BP + 0xbd],0x1
0000:3c70  c3                 RET
0000:3c71  d0 9e bd 00        RCR byte ptr [BP + 0xbd],0x1
0000:3c75  1b c2              SBB AX,DX
0000:3c77  d0 96 bd 00        RCL byte ptr [BP + 0xbd],0x1
0000:3c7b  c3                 RET
0000:3c7c  92                 XCHG AX,DX
0000:3c7d  98                 CBW
0000:3c7e  03 c2              ADD AX,DX
0000:3c80  d0 96 bd 00        RCL byte ptr [BP + 0xbd],0x1
0000:3c84  c3                 RET
0000:3c85  92                 XCHG AX,DX
0000:3c86  98                 CBW
0000:3c87  92                 XCHG AX,DX
0000:3c88  2b c2              SUB AX,DX
0000:3c8a  d0 96 bd 00        RCL byte ptr [BP + 0xbd],0x1
0000:3c8e  c3                 RET
0000:3c8f  23 c2              AND AX,DX
0000:3c91  c3                 RET
0000:3c92  0b c2              OR AX,DX
0000:3c94  c3                 RET
0000:3c95  33 c2              XOR AX,DX
0000:3c97  c3                 RET
0000:3c98  f7 e2              MUL DX
0000:3c9a  c3                 RET
0000:3c9b  8b ca              MOV CX,DX
0000:3c9d  33 d2              XOR DX,DX
0000:3c9f  f7 f1              DIV CX
0000:3ca1  c3                 RET
0000:3ca2  e8 a9 d1           CALL 0x0000:0e4e
0000:3ca5  8b ca              MOV CX,DX
0000:3ca7  33 d2              XOR DX,DX
0000:3ca9  f7 f1              DIV CX
0000:3cab  8b c2              MOV AX,DX
0000:3cad  c3                 RET
0000:3cae  8a ca              MOV CL,DL
0000:3cb0  d3 e0              SHL AX,CL
0000:3cb2  c3                 RET
0000:3cb3  8a ca              MOV CL,DL
0000:3cb5  d3 f8              SAR AX,CL
0000:3cb7  c3                 RET
0000:3cd0  f8                 CLC
0000:3cd1  c3                 RET
0000:3cd2  3b c2              CMP AX,DX
0000:3cd4  74 fa              JZ 0x0000:3cd0
0000:3cd6  f9                 STC
0000:3cd7  c3                 RET
0000:3cd8  3b c2              CMP AX,DX
0000:3cda  75 f4              JNZ 0x0000:3cd0
0000:3cdc  f9                 STC
0000:3cdd  c3                 RET
0000:3cde  3b c2              CMP AX,DX
0000:3ce0  77 ee              JA 0x0000:3cd0
0000:3ce2  f9                 STC
0000:3ce3  c3                 RET
0000:3ce4  3b c2              CMP AX,DX
0000:3ce6  72 e8              JC 0x0000:3cd0
0000:3ce8  f9                 STC
0000:3ce9  c3                 RET
0000:3cea  3b c2              CMP AX,DX
0000:3cec  7f e2              JG 0x0000:3cd0
0000:3cee  f9                 STC
0000:3cef  c3                 RET
0000:3cf0  3b c2              CMP AX,DX
0000:3cf2  7c dc              JL 0x0000:3cd0
0000:3cf4  f9                 STC
0000:3cf5  c3                 RET
0000:3cf6  3b c2              CMP AX,DX
0000:3cf8  73 d6              JNC 0x0000:3cd0
0000:3cfa  f9                 STC
0000:3cfb  c3                 RET
0000:3cfc  3b c2              CMP AX,DX
0000:3cfe  76 d0              JBE 0x0000:3cd0
0000:3d00  f9                 STC
0000:3d01  c3                 RET
0000:3d02  3b c2              CMP AX,DX
0000:3d04  7d ca              JGE 0x0000:3cd0
0000:3d06  f9                 STC
0000:3d07  c3                 RET
0000:3d08  3b c2              CMP AX,DX
0000:3d0a  7e c4              JLE 0x0000:3cd0
0000:3d0c  f9                 STC
0000:3d0d  c3                 RET
0000:3d0e  85 c2              TEST DX,AX
0000:3d10  74 be              JZ 0x0000:3cd0
0000:3d12  f9                 STC
0000:3d13  c3                 RET
0000:3d14  85 c2              TEST DX,AX
0000:3d16  75 b8              JNZ 0x0000:3cd0
0000:3d18  f9                 STC
0000:3d19  c3                 RET
0000:3d1a  56                 PUSH SI
0000:3d1b  57                 PUSH DI
0000:3d1c  1e                 PUSH DS
0000:3d1d  8a 86 ba 00        MOV AL,byte ptr [BP + 0xba]
0000:3d21  e6 a6              OUT 0xa6,AL
0000:3d23  8b 46 32           MOV AX,word ptr [BP + 0x32]
0000:3d26  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:3d29  8c c8              MOV AX,CS
0000:3d2b  8e d8              MOV AX,DS
0000:3d2d  26 8b 1d           MOV BX,word ptr ES:[DI]
0000:3d30  b0 03              MOV AL,0x3
0000:3d32  32 e4              XOR AH,AH
0000:3d34  33 f6              XOR SI,SI
0000:3d36  33 ff              XOR DI,DI
0000:3d38  ff 5e 0c           CALLF [BP + 0xc]
0000:3d3b  8a 86 ba 00        MOV AL,byte ptr [BP + 0xba]
0000:3d3f  34 01              XOR AL,0x1
0000:3d41  e6 a6              OUT 0xa6,AL
0000:3d43  8e 5e 08           MOV DS,word ptr [BP + 0x8]
0000:3d46  33 f6              XOR SI,SI
0000:3d48  8b 46 30           MOV AX,word ptr [BP + 0x30]
0000:3d4b  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:3d4e  ff 5e 0c           CALLF [BP + 0xc]
0000:3d51  1f                 POP DS
0000:3d52  5f                 POP DI
0000:3d53  5e                 POP SI
0000:3d54  83 44 08 02        ADD word ptr [SI + 0x8],0x2
0000:3d58  e9 7b f2           JMP 0x0000:2fd6
0000:3d5b  56                 PUSH SI
0000:3d5c  57                 PUSH DI
0000:3d5d  1e                 PUSH DS
0000:3d5e  8b 46 32           MOV AX,word ptr [BP + 0x32]
0000:3d61  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:3d64  8c c8              MOV AX,CS
0000:3d66  8e d8              MOV AX,DS
0000:3d68  26 8a 25           MOV AH,byte ptr ES:[DI]
0000:3d6b  26 8b 5d 01        MOV BX,word ptr ES:[DI + 0x1]
0000:3d6f  26 8b 7d 03        MOV DI,word ptr ES:[DI + 0x3]
0000:3d73  b0 01              MOV AL,0x1
0000:3d75  33 f6              XOR SI,SI
0000:3d77  ff 5e 0c           CALLF [BP + 0xc]
0000:3d7a  1f                 POP DS
0000:3d7b  5f                 POP DI
0000:3d7c  5e                 POP SI
0000:3d7d  83 44 08 05        ADD word ptr [SI + 0x8],0x5
0000:3d81  e9 52 f2           JMP 0x0000:2fd6
0000:3d84  33 db              XOR BX,BX
0000:3d86  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3d89  03 dd              ADD BX,BP
0000:3d8b  36 8b 07           MOV AX,word ptr SS:[BX]
0000:3d8e  33 db              XOR BX,BX
0000:3d90  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3d94  33 c9              XOR CX,CX
0000:3d96  26 8a 4d 02        MOV CL,byte ptr ES:[DI + 0x2]
0000:3d9a  26 8b 55 03        MOV DX,word ptr ES:[DI + 0x3]
0000:3d9e  e8 f1 d7           CALL 0x0000:1592
0000:3da1  83 44 08 05        ADD word ptr [SI + 0x8],0x5
0000:3da5  e9 2e f2           JMP 0x0000:2fd6
0000:3da8  56                 PUSH SI
0000:3da9  1e                 PUSH DS
0000:3daa  8b 46 46           MOV AX,word ptr [BP + 0x46]
0000:3dad  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:3db0  8c c0              MOV AX,ES
0000:3db2  8e d8              MOV AX,DS
0000:3db4  26 8a 4d 04        MOV CL,byte ptr ES:[DI + 0x4]
0000:3db8  26 8b 75 02        MOV SI,word ptr ES:[DI + 0x2]
0000:3dbc  26 8b 3d           MOV DI,word ptr ES:[DI]
0000:3dbf  ff 5e 0c           CALLF [BP + 0xc]
0000:3dc2  1f                 POP DS
0000:3dc3  5e                 POP SI
0000:3dc4  83 44 08 05        ADD word ptr [SI + 0x8],0x5
0000:3dc8  e9 0b f2           JMP 0x0000:2fd6
0000:3dcb  56                 PUSH SI
0000:3dcc  1e                 PUSH DS
0000:3dcd  8b 46 46           MOV AX,word ptr [BP + 0x46]
0000:3dd0  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:3dd3  33 db              XOR BX,BX
0000:3dd5  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3dd9  03 de              ADD BX,SI
0000:3ddb  8b 07              MOV AX,word ptr [BX]
0000:3ddd  8b d8              MOV BX,AX
0000:3ddf  c1 e8 02           SHR AX,0x2
0000:3de2  03 c3              ADD AX,BX
0000:3de4  33 db              XOR BX,BX
0000:3de6  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3de9  03 de              ADD BX,SI
0000:3deb  8b 1f              MOV BX,word ptr [BX]
0000:3ded  c1 eb 06           SHR BX,0x6
0000:3df0  8a cb              MOV CL,BL
0000:3df2  80 e1 07           AND CL,0x7
0000:3df5  c1 eb 03           SHR BX,0x3
0000:3df8  03 c3              ADD AX,BX
0000:3dfa  26 8b 75 02        MOV SI,word ptr ES:[DI + 0x2]
0000:3dfe  8b f8              MOV DI,AX
0000:3e00  8c c0              MOV AX,ES
0000:3e02  8e d8              MOV AX,DS
0000:3e04  ff 5e 0c           CALLF [BP + 0xc]
0000:3e07  1f                 POP DS
0000:3e08  5e                 POP SI
0000:3e09  83 44 08 04        ADD word ptr [SI + 0x8],0x4
0000:3e0d  e9 c6 f1           JMP 0x0000:2fd6
0000:3e10  56                 PUSH SI
0000:3e11  1e                 PUSH DS
0000:3e12  8b 46 48           MOV AX,word ptr [BP + 0x48]
0000:3e15  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:3e18  8c c0              MOV AX,ES
0000:3e1a  8e d8              MOV AX,DS
0000:3e1c  26 8b 75 02        MOV SI,word ptr ES:[DI + 0x2]
0000:3e20  26 8b 3d           MOV DI,word ptr ES:[DI]
0000:3e23  ff 5e 0c           CALLF [BP + 0xc]
0000:3e26  1f                 POP DS
0000:3e27  5e                 POP SI
0000:3e28  83 44 08 04        ADD word ptr [SI + 0x8],0x4
0000:3e2c  e9 a7 f1           JMP 0x0000:2fd6
0000:3e2f  56                 PUSH SI
0000:3e30  1e                 PUSH DS
0000:3e31  8b 46 48           MOV AX,word ptr [BP + 0x48]
0000:3e34  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:3e37  33 db              XOR BX,BX
0000:3e39  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3e3d  03 de              ADD BX,SI
0000:3e3f  8b 07              MOV AX,word ptr [BX]
0000:3e41  8b d8              MOV BX,AX
0000:3e43  c1 e0 02           SHL AX,0x2
0000:3e46  03 c3              ADD AX,BX
0000:3e48  c1 e0 04           SHL AX,0x4
0000:3e4b  33 db              XOR BX,BX
0000:3e4d  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3e50  03 de              ADD BX,SI
0000:3e52  8b 1f              MOV BX,word ptr [BX]
0000:3e54  c1 eb 03           SHR BX,0x3
0000:3e57  03 c3              ADD AX,BX
0000:3e59  26 8b 75 02        MOV SI,word ptr ES:[DI + 0x2]
0000:3e5d  8b f8              MOV DI,AX
0000:3e5f  8c c0              MOV AX,ES
0000:3e61  8e d8              MOV AX,DS
0000:3e63  ff 5e 0c           CALLF [BP + 0xc]
0000:3e66  1f                 POP DS
0000:3e67  5e                 POP SI
0000:3e68  83 44 08 04        ADD word ptr [SI + 0x8],0x4
0000:3e6c  e9 67 f1           JMP 0x0000:2fd6
0000:3e6f  56                 PUSH SI
0000:3e70  1e                 PUSH DS
0000:3e71  8b 46 4a           MOV AX,word ptr [BP + 0x4a]
0000:3e74  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:3e77  8c c0              MOV AX,ES
0000:3e79  8e d8              MOV AX,DS
0000:3e7b  26 8b 75 02        MOV SI,word ptr ES:[DI + 0x2]
0000:3e7f  26 8b 3d           MOV DI,word ptr ES:[DI]
0000:3e82  ff 5e 0c           CALLF [BP + 0xc]
0000:3e85  1f                 POP DS
0000:3e86  5e                 POP SI
0000:3e87  83 44 08 04        ADD word ptr [SI + 0x8],0x4
0000:3e8b  e9 48 f1           JMP 0x0000:2fd6
0000:3e8e  56                 PUSH SI
0000:3e8f  1e                 PUSH DS
0000:3e90  8b 46 4a           MOV AX,word ptr [BP + 0x4a]
0000:3e93  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:3e96  33 db              XOR BX,BX
0000:3e98  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3e9c  03 de              ADD BX,SI
0000:3e9e  8b 07              MOV AX,word ptr [BX]
0000:3ea0  8b d8              MOV BX,AX
0000:3ea2  c1 e0 02           SHL AX,0x2
0000:3ea5  03 c3              ADD AX,BX
0000:3ea7  c1 e0 04           SHL AX,0x4
0000:3eaa  33 db              XOR BX,BX
0000:3eac  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3eaf  03 de              ADD BX,SI
0000:3eb1  8b 1f              MOV BX,word ptr [BX]
0000:3eb3  c1 eb 03           SHR BX,0x3
0000:3eb6  03 c3              ADD AX,BX
0000:3eb8  26 8b 75 02        MOV SI,word ptr ES:[DI + 0x2]
0000:3ebc  8b f8              MOV DI,AX
0000:3ebe  8c c0              MOV AX,ES
0000:3ec0  8e d8              MOV AX,DS
0000:3ec2  ff 5e 0c           CALLF [BP + 0xc]
0000:3ec5  1f                 POP DS
0000:3ec6  5e                 POP SI
0000:3ec7  83 44 08 04        ADD word ptr [SI + 0x8],0x4
0000:3ecb  e9 08 f1           JMP 0x0000:2fd6
0000:3ece  57                 PUSH DI
0000:3ecf  56                 PUSH SI
0000:3ed0  1e                 PUSH DS
0000:3ed1  06                 PUSH ES
0000:3ed2  8b 5e 42           MOV BX,word ptr [BP + 0x42]
0000:3ed5  89 5e 0c           MOV word ptr [BP + 0xc],BX
0000:3ed8  33 db              XOR BX,BX
0000:3eda  26 8a 5d 07        MOV BL,byte ptr ES:[DI + 0x7]
0000:3ede  03 dd              ADD BX,BP
0000:3ee0  36 8e 1f           MOV DS,word ptr SS:[BX]
0000:3ee3  26 8b 05           MOV AX,word ptr ES:[DI]
0000:3ee6  50                 PUSH AX
0000:3ee7  26 8a 45 02        MOV AL,byte ptr ES:[DI + 0x2]
0000:3eeb  26 8b 4d 03        MOV CX,word ptr ES:[DI + 0x3]
0000:3eef  26 8b 55 05        MOV DX,word ptr ES:[DI + 0x5]
0000:3ef3  26 8b 75 08        MOV SI,word ptr ES:[DI + 0x8]
0000:3ef7  26 8b 5d 0a        MOV BX,word ptr ES:[DI + 0xa]
0000:3efb  5f                 POP DI
0000:3efc  ff 5e 0c           CALLF [BP + 0xc]
0000:3eff  07                 POP ES
0000:3f00  1f                 POP DS
0000:3f01  5e                 POP SI
0000:3f02  5f                 POP DI
0000:3f03  83 44 08 0c        ADD word ptr [SI + 0x8],0xc
0000:3f07  e9 cc f0           JMP 0x0000:2fd6
0000:3f0a  57                 PUSH DI
0000:3f0b  56                 PUSH SI
0000:3f0c  1e                 PUSH DS
0000:3f0d  06                 PUSH ES
0000:3f0e  8b 5e 42           MOV BX,word ptr [BP + 0x42]
0000:3f11  89 5e 0c           MOV word ptr [BP + 0xc],BX
0000:3f14  33 db              XOR BX,BX
0000:3f16  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3f1a  03 de              ADD BX,SI
0000:3f1c  8b 17              MOV DX,word ptr [BX]
0000:3f1e  8b da              MOV BX,DX
0000:3f20  c1 e2 02           SHL DX,0x2
0000:3f23  03 d3              ADD DX,BX
0000:3f25  c1 e2 04           SHL DX,0x4
0000:3f28  33 db              XOR BX,BX
0000:3f2a  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3f2d  03 de              ADD BX,SI
0000:3f2f  8b 1f              MOV BX,word ptr [BX]
0000:3f31  c1 eb 03           SHR BX,0x3
0000:3f34  03 d3              ADD DX,BX
0000:3f36  52                 PUSH DX
0000:3f37  33 db              XOR BX,BX
0000:3f39  26 8a 5d 06        MOV BL,byte ptr ES:[DI + 0x6]
0000:3f3d  03 dd              ADD BX,BP
0000:3f3f  36 8e 1f           MOV DS,word ptr SS:[BX]
0000:3f42  24 07              AND AL,0x7
0000:3f44  26 8b 4d 02        MOV CX,word ptr ES:[DI + 0x2]
0000:3f48  26 8b 55 04        MOV DX,word ptr ES:[DI + 0x4]
0000:3f4c  26 8b 75 07        MOV SI,word ptr ES:[DI + 0x7]
0000:3f50  26 8b 5d 09        MOV BX,word ptr ES:[DI + 0x9]
0000:3f54  5f                 POP DI
0000:3f55  ff 5e 0c           CALLF [BP + 0xc]
0000:3f58  07                 POP ES
0000:3f59  1f                 POP DS
0000:3f5a  5e                 POP SI
0000:3f5b  5f                 POP DI
0000:3f5c  83 44 08 0b        ADD word ptr [SI + 0x8],0xb
0000:3f60  e9 73 f0           JMP 0x0000:2fd6
0000:3f63  8b 46 44           MOV AX,word ptr [BP + 0x44]
0000:3f66  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:3f69  33 c9              XOR CX,CX
0000:3f6b  26 8a 4d 02        MOV CL,byte ptr ES:[DI + 0x2]
0000:3f6f  26 8b 55 03        MOV DX,word ptr ES:[DI + 0x3]
0000:3f73  26 8a 45 05        MOV AL,byte ptr ES:[DI + 0x5]
0000:3f77  26 8b 3d           MOV DI,word ptr ES:[DI]
0000:3f7a  ff 5e 0c           CALLF [BP + 0xc]
0000:3f7d  83 44 08 06        ADD word ptr [SI + 0x8],0x6
0000:3f81  e9 52 f0           JMP 0x0000:2fd6
0000:3f84  8b 46 44           MOV AX,word ptr [BP + 0x44]
0000:3f87  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:3f8a  33 db              XOR BX,BX
0000:3f8c  26 8a 5d 01        MOV BL,byte ptr ES:[DI + 0x1]
0000:3f90  03 de              ADD BX,SI
0000:3f92  8b 17              MOV DX,word ptr [BX]
0000:3f94  8b da              MOV BX,DX
0000:3f96  c1 e2 02           SHL DX,0x2
0000:3f99  03 d3              ADD DX,BX
0000:3f9b  c1 e2 04           SHL DX,0x4
0000:3f9e  33 db              XOR BX,BX
0000:3fa0  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:3fa3  03 de              ADD BX,SI
0000:3fa5  8b 1f              MOV BX,word ptr [BX]
0000:3fa7  c1 eb 03           SHR BX,0x3
0000:3faa  03 da              ADD BX,DX
0000:3fac  33 c9              XOR CX,CX
0000:3fae  26 8a 4d 02        MOV CL,byte ptr ES:[DI + 0x2]
0000:3fb2  26 8b 55 03        MOV DX,word ptr ES:[DI + 0x3]
0000:3fb6  26 8a 45 05        MOV AL,byte ptr ES:[DI + 0x5]
0000:3fba  8b fb              MOV DI,BX
0000:3fbc  ff 5e 0c           CALLF [BP + 0xc]
0000:3fbf  83 44 08 06        ADD word ptr [SI + 0x8],0x6
0000:3fc3  e9 10 f0           JMP 0x0000:2fd6
0000:3fc6  56                 PUSH SI
0000:3fc7  8b 76 52           MOV SI,word ptr [BP + 0x52]
0000:3fca  ba f0 1a           MOV DX,0x1af0
0000:3fcd  bb 52 00           MOV BX,0x52
0000:3fd0  b9 00 01           MOV CX,0x100
0000:3fd3  b8 00 a0           MOV AX,0xa000
0000:3fd6  e8 68 da           CALL 0x0000:1a41
0000:3fd9  72 09              JC 0x0000:3fe4
0000:3fdb  26 8b 05           MOV AX,word ptr ES:[DI]
0000:3fde  89 44 22           MOV word ptr [SI + 0x22],AX
0000:3fe1  8c 44 24           MOV word ptr [SI + 0x24],ES
0000:3fe4  5e                 POP SI
0000:3fe5  83 44 08 02        ADD word ptr [SI + 0x8],0x2
0000:3fe9  e9 ea ef           JMP 0x0000:2fd6
0000:3fec  56                 PUSH SI
0000:3fed  1e                 PUSH DS
0000:3fee  06                 PUSH ES
0000:3fef  26 8b 05           MOV AX,word ptr ES:[DI]
0000:3ff2  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:3ff5  8c 46 0e           MOV word ptr [BP + 0xe],ES
0000:3ff8  ff 5e 0c           CALLF [BP + 0xc]
0000:3ffb  8c 56 0e           MOV word ptr [BP + 0xe],SS
0000:3ffe  07                 POP ES
0000:3fff  1f                 POP DS
0000:4000  5e                 POP SI
0000:4001  83 44 08 02        ADD word ptr [SI + 0x8],0x2
0000:4005  e9 ce ef           JMP 0x0000:2fd6
0000:4008  56                 PUSH SI
0000:4009  1e                 PUSH DS
0000:400a  26 8b 35           MOV SI,word ptr ES:[DI]
0000:400d  8c c0              MOV AX,ES
0000:400f  8e d8              MOV AX,DS
0000:4011  8b 46 36           MOV AX,word ptr [BP + 0x36]
0000:4014  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:4017  ff 5e 0c           CALLF [BP + 0xc]
0000:401a  1f                 POP DS
0000:401b  5e                 POP SI
0000:401c  83 44 08 02        ADD word ptr [SI + 0x8],0x2
0000:4020  e9 b3 ef           JMP 0x0000:2fd6
0000:4023  56                 PUSH SI
0000:4024  1e                 PUSH DS
0000:4025  26 8b 35           MOV SI,word ptr ES:[DI]
0000:4028  8c c0              MOV AX,ES
0000:402a  8e d8              MOV AX,DS
0000:402c  8b 46 3a           MOV AX,word ptr [BP + 0x3a]
0000:402f  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:4032  ff 5e 0c           CALLF [BP + 0xc]
0000:4035  1f                 POP DS
0000:4036  5e                 POP SI
0000:4037  83 44 08 02        ADD word ptr [SI + 0x8],0x2
0000:403b  e9 98 ef           JMP 0x0000:2fd6
0000:403e  56                 PUSH SI
0000:403f  06                 PUSH ES
0000:4040  8b 46 38           MOV AX,word ptr [BP + 0x38]
0000:4043  89 46 0c           MOV word ptr [BP + 0xc],AX
0000:4046  26 8a 05           MOV AL,byte ptr ES:[DI]
0000:4049  ff 5e 0c           CALLF [BP + 0xc]
0000:404c  07                 POP ES
0000:404d  5e                 POP SI
0000:404e  ff 44 08           INC word ptr [SI + 0x8]
0000:4051  e9 82 ef           JMP 0x0000:2fd6
0000:4054  ff 44 08           INC word ptr [SI + 0x8]
0000:4057  f6 86 b9 00 04     TEST byte ptr [BP + 0xb9],0x4
0000:405c  74 08              JZ 0x0000:4066
0000:405e  c6 86 bb 00 ff     MOV byte ptr [BP + 0xbb],0xff
0000:4063  e9 70 ef           JMP 0x0000:2fd6
0000:4066  26 8a 05           MOV AL,byte ptr ES:[DI]
0000:4069  3a 86 bb 00        CMP AL,byte ptr [BP + 0xbb]
0000:406d  75 03              JNZ 0x0000:4072
0000:406f  e9 64 ef           JMP 0x0000:2fd6
0000:4072  88 86 bb 00        MOV byte ptr [BP + 0xbb],AL
0000:4076  b4 00              MOV AH,0x0
0000:4078  cd 7f              INT 0x7f
0000:407a  e9 59 ef           JMP 0x0000:2fd6
0000:407d  ff 44 08           INC word ptr [SI + 0x8]
0000:4080  f6 86 b9 00 04     TEST byte ptr [BP + 0xb9],0x4
0000:4085  74 03              JZ 0x0000:408a
0000:4087  e9 4c ef           JMP 0x0000:2fd6
0000:408a  26 8a 05           MOV AL,byte ptr ES:[DI]
0000:408d  b4 01              MOV AH,0x1
0000:408f  cd 7f              INT 0x7f
0000:4091  e9 42 ef           JMP 0x0000:2fd6
0000:4094  ff 44 08           INC word ptr [SI + 0x8]
0000:4097  f6 86 b9 00 04     TEST byte ptr [BP + 0xb9],0x4
0000:409c  74 03              JZ 0x0000:40a1
0000:409e  e9 35 ef           JMP 0x0000:2fd6
0000:40a1  33 db              XOR BX,BX
0000:40a3  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:40a6  03 dd              ADD BX,BP
0000:40a8  36 8a 07           MOV AL,byte ptr SS:[BX]
0000:40ab  b4 01              MOV AH,0x1
0000:40ad  cd 7f              INT 0x7f
0000:40af  e9 24 ef           JMP 0x0000:2fd6
0000:40b2  c6 86 bb 00 ff     MOV byte ptr [BP + 0xbb],0xff
0000:40b7  b4 03              MOV AH,0x3
0000:40b9  cd 7f              INT 0x7f
0000:40bb  e9 18 ef           JMP 0x0000:2fd6
0000:40be  c6 86 bb 00 ff     MOV byte ptr [BP + 0xbb],0xff
0000:40c3  26 8a 05           MOV AL,byte ptr ES:[DI]
0000:40c6  b4 04              MOV AH,0x4
0000:40c8  cd 7f              INT 0x7f
0000:40ca  ff 44 08           INC word ptr [SI + 0x8]
0000:40cd  e9 06 ef           JMP 0x0000:2fd6
0000:40d0  56                 PUSH SI
0000:40d1  1e                 PUSH DS
0000:40d2  8c c0              MOV AX,ES
0000:40d4  8e d8              MOV AX,DS
0000:40d6  26 8b 35           MOV SI,word ptr ES:[DI]
0000:40d9  e8 e9 d1           CALL 0x0000:12c5
0000:40dc  1f                 POP DS
0000:40dd  5e                 POP SI
0000:40de  83 44 08 02        ADD word ptr [SI + 0x8],0x2
0000:40e2  e9 f1 ee           JMP 0x0000:2fd6
0000:40e5  33 db              XOR BX,BX
0000:40e7  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:40eb  03 dd              ADD BX,BP
0000:40ed  36 8a 07           MOV AL,byte ptr SS:[BX]
0000:40f0  26 8b 4d 03        MOV CX,word ptr ES:[DI + 0x3]
0000:40f4  26 8b 1d           MOV BX,word ptr ES:[DI]
0000:40f7  e8 18 d3           CALL 0x0000:1412
0000:40fa  83 44 08 05        ADD word ptr [SI + 0x8],0x5
0000:40fe  e9 d5 ee           JMP 0x0000:2fd6
0000:4101  33 db              XOR BX,BX
0000:4103  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:4107  03 dd              ADD BX,BP
0000:4109  36 8b 07           MOV AX,word ptr SS:[BX]
0000:410c  26 8b 4d 03        MOV CX,word ptr ES:[DI + 0x3]
0000:4110  26 8b 1d           MOV BX,word ptr ES:[DI]
0000:4113  e8 29 d3           CALL 0x0000:143f
0000:4116  83 44 08 05        ADD word ptr [SI + 0x8],0x5
0000:411a  e9 b9 ee           JMP 0x0000:2fd6
0000:411d  33 db              XOR BX,BX
0000:411f  26 8a 5d 02        MOV BL,byte ptr ES:[DI + 0x2]
0000:4123  03 dd              ADD BX,BP
0000:4125  36 8b 17           MOV DX,word ptr SS:[BX]
0000:4128  33 db              XOR BX,BX
0000:412a  26 8a 5d 03        MOV BL,byte ptr ES:[DI + 0x3]
0000:412e  03 dd              ADD BX,BP
0000:4130  36 8b 07           MOV AX,word ptr SS:[BX]
0000:4133  26 8b 4d 04        MOV CX,word ptr ES:[DI + 0x4]
0000:4137  26 8b 1d           MOV BX,word ptr ES:[DI]
0000:413a  e8 57 d3           CALL 0x0000:1494
0000:413d  83 44 08 06        ADD word ptr [SI + 0x8],0x6
0000:4141  e9 92 ee           JMP 0x0000:2fd6
0000:4144  8b 46 7a           MOV AX,word ptr [BP + 0x7a]
0000:4147  8b 56 7e           MOV DX,word ptr [BP + 0x7e]
0000:414a  c1 e2 04           SHL DX,0x4
0000:414d  2b 86 82 00        SUB AX,word ptr [BP + 0x82]
0000:4151  83 f8 00           CMP AX,0x0
0000:4154  7d 02              JGE 0x0000:4158
0000:4156  03 c2              ADD AX,DX
0000:4158  3b c2              CMP AX,DX
0000:415a  7c 02              JL 0x0000:415e
0000:415c  2b c2              SUB AX,DX
0000:415e  89 46 7a           MOV word ptr [BP + 0x7a],AX
0000:4161  8b 86 82 00        MOV AX,word ptr [BP + 0x82]
0000:4165  03 46 74           ADD AX,word ptr [BP + 0x74]
0000:4168  83 f8 00           CMP AX,0x0
0000:416b  7f 05              JG 0x0000:4172
0000:416d  05 90 01           ADD AX,0x190
0000:4170  eb 00              JMP 0x0000:4172
0000:4172  3d 90 01           CMP AX,0x190
0000:4175  7e 05              JLE 0x0000:417c
0000:4177  2d 90 01           SUB AX,0x190
0000:417a  eb f6              JMP 0x0000:4172
0000:417c  89 46 74           MOV word ptr [BP + 0x74],AX
0000:417f  ba 90 01           MOV DX,0x190
0000:4182  2b d0              SUB DX,AX
0000:4184  89 56 76           MOV word ptr [BP + 0x76],DX
0000:4187  c6 86 be 00 00     MOV byte ptr [BP + 0xbe],0x0
0000:418c  8b 46 7a           MOV AX,word ptr [BP + 0x7a]
0000:418f  8b 56 7c           MOV DX,word ptr [BP + 0x7c]
0000:4192  c1 e8 04           SHR AX,0x4
0000:4195  89 46 7c           MOV word ptr [BP + 0x7c],AX
0000:4198  3b c2              CMP AX,DX
0000:419a  74 08              JZ 0x0000:41a4
0000:419c  c6 86 be 00 01     MOV byte ptr [BP + 0xbe],0x1
0000:41a1  e8 ea ce           CALL 0x0000:108e
0000:41a4  e9 2f ee           JMP 0x0000:2fd6
0000:41a7  80 be c1 00 00     CMP byte ptr [BP + 0xc1],0x0
0000:41ac  74 03              JZ 0x0000:41b1
0000:41ae  e9 25 ee           JMP 0x0000:2fd6
0000:41b1  e8 1e cf           CALL 0x0000:10d2
0000:41b4  e9 1f ee           JMP 0x0000:2fd6
0000:41b7  c6 86 c1 00 00     MOV byte ptr [BP + 0xc1],0x0
0000:41bc  b9 19 00           MOV CX,0x19
0000:41bf  51                 PUSH CX
0000:41c0  bb 90 01           MOV BX,0x190
0000:41c3  8b 46 74           MOV AX,word ptr [BP + 0x74]
0000:41c6  83 c0 10           ADD AX,0x10
0000:41c9  3b c3              CMP AX,BX
0000:41cb  7e 02              JLE 0x0000:41cf
0000:41cd  2b c3              SUB AX,BX
0000:41cf  89 46 74           MOV word ptr [BP + 0x74],AX
0000:41d2  2b d8              SUB BX,AX
0000:41d4  89 5e 76           MOV word ptr [BP + 0x76],BX
0000:41d7  8b 46 7c           MOV AX,word ptr [BP + 0x7c]
0000:41da  8b 5e 7e           MOV BX,word ptr [BP + 0x7e]
0000:41dd  48                 DEC AX
0000:41de  3b c3              CMP AX,BX
0000:41e0  7c 02              JL 0x0000:41e4
0000:41e2  2b c3              SUB AX,BX
0000:41e4  83 f8 00           CMP AX,0x0
0000:41e7  7d 02              JGE 0x0000:41eb
0000:41e9  03 c3              ADD AX,BX
0000:41eb  89 46 7c           MOV word ptr [BP + 0x7c],AX
0000:41ee  c1 e0 04           SHL AX,0x4
0000:41f1  89 46 7a           MOV word ptr [BP + 0x7a],AX
0000:41f4  e8 97 ce           CALL 0x0000:108e
0000:41f7  59                 POP CX
0000:41f8  e2 c5              LOOP 0x0000:41bf
0000:41fa  e9 d9 ed           JMP 0x0000:2fd6
0000:41fd  80 be c1 00 00     CMP byte ptr [BP + 0xc1],0x0
0000:4202  74 03              JZ 0x0000:4207
0000:4204  e9 cf ed           JMP 0x0000:2fd6
0000:4207  56                 PUSH SI
0000:4208  8b 76 52           MOV SI,word ptr [BP + 0x52]
0000:420b  b9 00 01           MOV CX,0x100
0000:420e  f7 04 ff 00        TEST word ptr [SI],0xff
0000:4212  74 0d              JZ 0x0000:4221
0000:4214  f7 04 00 02        TEST word ptr [SI],0x200
0000:4218  75 07              JNZ 0x0000:4221
0000:421a  51                 PUSH CX
0000:421b  56                 PUSH SI
0000:421c  e8 13 cf           CALL 0x0000:1132
0000:421f  5e                 POP SI
0000:4220  59                 POP CX
0000:4221  83 c6 52           ADD SI,0x52
0000:4224  e2 e8              LOOP 0x0000:420e
0000:4226  5e                 POP SI
0000:4227  e9 ac ed           JMP 0x0000:2fd6
0000:422a  26 8b 05           MOV AX,word ptr ES:[DI]
0000:422d  83 c7 02           ADD DI,0x2
0000:4230  8b 5e 52           MOV BX,word ptr [BP + 0x52]
0000:4233  b9 00 01           MOV CX,0x100
0000:4236  85 07              TEST word ptr [BX],AX
0000:4238  74 06              JZ 0x0000:4240
0000:423a  f7 07 00 01        TEST word ptr [BX],0x100
0000:423e  74 21              JZ 0x0000:4261
0000:4240  83 c3 52           ADD BX,0x52
0000:4243  e2 f1              LOOP 0x0000:4236
0000:4245  eb 00              JMP 0x0000:4247
0000:4247  33 db              XOR BX,BX
0000:4249  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:424c  47                 INC DI
0000:424d  80 fb ff           CMP BL,0xff
0000:4250  74 09              JZ 0x0000:425b
0000:4252  81 c3 04 36        ADD BX,0x3604
0000:4256  2e 03 3f           ADD DI,word ptr CS:[BX]
0000:4259  eb ec              JMP 0x0000:4247
0000:425b  89 7c 08           MOV word ptr [SI + 0x8],DI
0000:425e  e9 75 ed           JMP 0x0000:2fd6
0000:4261  50                 PUSH AX
0000:4262  8b 57 44           MOV DX,word ptr [BX + 0x44]
0000:4265  03 57 22           ADD DX,word ptr [BX + 0x22]
0000:4268  8b 44 42           MOV AX,word ptr [SI + 0x42]
0000:426b  03 44 22           ADD AX,word ptr [SI + 0x22]
0000:426e  3b c2              CMP AX,DX
0000:4270  7d 37              JGE 0x0000:42a9
0000:4272  8b 57 42           MOV DX,word ptr [BX + 0x42]
0000:4275  03 57 22           ADD DX,word ptr [BX + 0x22]
0000:4278  8b 44 44           MOV AX,word ptr [SI + 0x44]
0000:427b  03 44 22           ADD AX,word ptr [SI + 0x22]
0000:427e  3b c2              CMP AX,DX
0000:4280  7c 27              JL 0x0000:42a9
0000:4282  8b 57 48           MOV DX,word ptr [BX + 0x48]
0000:4285  03 57 24           ADD DX,word ptr [BX + 0x24]
0000:4288  8b 44 46           MOV AX,word ptr [SI + 0x46]
0000:428b  03 44 24           ADD AX,word ptr [SI + 0x24]
0000:428e  3b c2              CMP AX,DX
0000:4290  7d 17              JGE 0x0000:42a9
0000:4292  83 fa 20           CMP DX,0x20
0000:4295  7c 12              JL 0x0000:42a9
0000:4297  8b 57 46           MOV DX,word ptr [BX + 0x46]
0000:429a  03 57 24           ADD DX,word ptr [BX + 0x24]
0000:429d  8b 44 48           MOV AX,word ptr [SI + 0x48]
0000:42a0  03 44 24           ADD AX,word ptr [SI + 0x24]
0000:42a3  3b c2              CMP AX,DX
0000:42a5  7c 02              JL 0x0000:42a9
0000:42a7  eb 03              JMP 0x0000:42ac
0000:42a9  58                 POP AX
0000:42aa  eb 94              JMP 0x0000:4240
0000:42ac  53                 PUSH BX
0000:42ad  51                 PUSH CX
0000:42ae  57                 PUSH DI
0000:42af  56                 PUSH SI
0000:42b0  8b f3              MOV SI,BX
0000:42b2  33 db              XOR BX,BX
0000:42b4  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:42b7  47                 INC DI
0000:42b8  80 fb ff           CMP BL,0xff
0000:42bb  74 09              JZ 0x0000:42c6
0000:42bd  81 c3 d8 35        ADD BX,0x35d8
0000:42c1  2e ff 17           CALL word ptr CS:[BX]
0000:42c4  eb ec              JMP 0x0000:42b2
0000:42c6  5e                 POP SI
0000:42c7  5f                 POP DI
0000:42c8  59                 POP CX
0000:42c9  5b                 POP BX
0000:42ca  58                 POP AX
0000:42cb  ff 4c 3c           DEC word ptr [SI + 0x3c]
0000:42ce  e9 6f ff           JMP 0x0000:4240
0000:42d1  26 8b 15           MOV DX,word ptr ES:[DI]
0000:42d4  83 c7 02           ADD DI,0x2
0000:42d7  8b 5e 52           MOV BX,word ptr [BP + 0x52]
0000:42da  b9 00 01           MOV CX,0x100
0000:42dd  8b 07              MOV AX,word ptr [BX]
0000:42df  a9 00 01           TEST AX,0x100
0000:42e2  75 04              JNZ 0x0000:42e8
0000:42e4  85 c2              TEST DX,AX
0000:42e6  75 1f              JNZ 0x0000:4307
0000:42e8  83 c3 52           ADD BX,0x52
0000:42eb  e2 f0              LOOP 0x0000:42dd
0000:42ed  33 db              XOR BX,BX
0000:42ef  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:42f2  47                 INC DI
0000:42f3  80 fb ff           CMP BL,0xff
0000:42f6  74 09              JZ 0x0000:4301
0000:42f8  81 c3 04 36        ADD BX,0x3604
0000:42fc  2e 03 3f           ADD DI,word ptr CS:[BX]
0000:42ff  eb ec              JMP 0x0000:42ed
0000:4301  89 7c 08           MOV word ptr [SI + 0x8],DI
0000:4304  e9 cf ec           JMP 0x0000:2fd6
0000:4307  52                 PUSH DX
0000:4308  8b 57 44           MOV DX,word ptr [BX + 0x44]
0000:430b  03 57 22           ADD DX,word ptr [BX + 0x22]
0000:430e  8b 44 42           MOV AX,word ptr [SI + 0x42]
0000:4311  03 44 22           ADD AX,word ptr [SI + 0x22]
0000:4314  3b c2              CMP AX,DX
0000:4316  7d 38              JGE 0x0000:4350
0000:4318  8b 57 42           MOV DX,word ptr [BX + 0x42]
0000:431b  03 57 22           ADD DX,word ptr [BX + 0x22]
0000:431e  8b 44 44           MOV AX,word ptr [SI + 0x44]
0000:4321  03 44 22           ADD AX,word ptr [SI + 0x22]
0000:4324  3b c2              CMP AX,DX
0000:4326  7c 28              JL 0x0000:4350
0000:4328  8b 57 48           MOV DX,word ptr [BX + 0x48]
0000:432b  03 57 24           ADD DX,word ptr [BX + 0x24]
0000:432e  8b 44 46           MOV AX,word ptr [SI + 0x46]
0000:4331  03 44 24           ADD AX,word ptr [SI + 0x24]
0000:4334  3b c2              CMP AX,DX
0000:4336  7d 18              JGE 0x0000:4350
0000:4338  83 fa 20           CMP DX,0x20
0000:433b  7c 13              JL 0x0000:4350
0000:433d  8b 57 46           MOV DX,word ptr [BX + 0x46]
0000:4340  03 57 24           ADD DX,word ptr [BX + 0x24]
0000:4343  8b 44 48           MOV AX,word ptr [SI + 0x48]
0000:4346  03 44 24           ADD AX,word ptr [SI + 0x24]
0000:4349  3b c2              CMP AX,DX
0000:434b  7c 03              JL 0x0000:4350
0000:434d  5a                 POP DX
0000:434e  eb 03              JMP 0x0000:4353
0000:4350  5a                 POP DX
0000:4351  eb 95              JMP 0x0000:42e8
0000:4353  ff 4c 3c           DEC word ptr [SI + 0x3c]
0000:4356  56                 PUSH SI
0000:4357  8b f3              MOV SI,BX
0000:4359  33 db              XOR BX,BX
0000:435b  26 8a 1d           MOV BL,byte ptr ES:[DI]
0000:435e  47                 INC DI
0000:435f  80 fb ff           CMP BL,0xff
0000:4362  74 09              JZ 0x0000:436d
0000:4364  81 c3 d8 35        ADD BX,0x35d8
0000:4368  2e ff 17           CALL word ptr CS:[BX]
0000:436b  eb ec              JMP 0x0000:4359
0000:436d  5e                 POP SI
0000:436e  89 7c 08           MOV word ptr [SI + 0x8],DI
0000:4371  e9 62 ec           JMP 0x0000:2fd6
0000:4380  06                 PUSH ES
0000:4381  50                 PUSH AX
0000:4382  b8 00 a0           MOV AX,0xa000
0000:4385  8e c0              MOV AX,ES
0000:4387  58                 POP AX
0000:4388  26 88 37           MOV byte ptr ES:[BX],DH
0000:438b  26 88 57 02        MOV byte ptr ES:[BX + 0x2],DL
0000:438f  26 c6 47 04 3a     MOV byte ptr ES:[BX + 0x4],0x3a
0000:4394  26 c6 47 06 20     MOV byte ptr ES:[BX + 0x6],0x20
0000:4399  26 c7 87 00 20 e1 00  MOV word ptr ES:[BX + 0x2000],0xe1
0000:43a0  26 c7 87 02 20 e1 00  MOV word ptr ES:[BX + 0x2002],0xe1
0000:43a7  26 c7 87 04 20 e1 00  MOV word ptr ES:[BX + 0x2004],0xe1
0000:43ae  26 c7 87 06 20 e1 00  MOV word ptr ES:[BX + 0x2006],0xe1
0000:43b5  83 c3 08           ADD BX,0x8
0000:43b8  e8 6b 00           CALL 0x0000:4426
0000:43bb  83 eb 08           SUB BX,0x8
0000:43be  07                 POP ES
0000:43bf  c3                 RET
0000:43c0  06                 PUSH ES
0000:43c1  50                 PUSH AX
0000:43c2  b8 00 a0           MOV AX,0xa000
0000:43c5  8e c0              MOV AX,ES
0000:43c7  58                 POP AX
0000:43c8  26 88 37           MOV byte ptr ES:[BX],DH
0000:43cb  26 88 57 02        MOV byte ptr ES:[BX + 0x2],DL
0000:43cf  26 c6 47 04 3a     MOV byte ptr ES:[BX + 0x4],0x3a
0000:43d4  26 c7 87 00 20 e1 00  MOV word ptr ES:[BX + 0x2000],0xe1
0000:43db  26 c7 87 02 20 e1 00  MOV word ptr ES:[BX + 0x2002],0xe1
0000:43e2  26 c7 87 04 20 e1 00  MOV word ptr ES:[BX + 0x2004],0xe1
0000:43e9  83 c3 06           ADD BX,0x6
0000:43ec  e8 c0 00           CALL 0x0000:44af
0000:43ef  83 eb 06           SUB BX,0x6
0000:43f2  07                 POP ES
0000:43f3  c3                 RET
0000:4426  50                 PUSH AX
0000:4427  52                 PUSH DX
0000:4428  06                 PUSH ES
0000:4429  ba 00 a0           MOV DX,0xa000
0000:442c  8e c2              MOV DX,ES
0000:442e  8b d0              MOV DX,AX
0000:4430  33 c0              XOR AX,AX
0000:4432  81 fa 00 10        CMP DX,0x1000
0000:4436  72 08              JC 0x0000:4440
0000:4438  81 ea 00 10        SUB DX,0x1000
0000:443c  fe c0              INC AL
0000:443e  eb f2              JMP 0x0000:4432
0000:4440  3c 0a              CMP AL,0xa
0000:4442  72 02              JC 0x0000:4446
0000:4444  04 07              ADD AL,0x7
0000:4446  04 30              ADD AL,0x30
0000:4448  26 88 07           MOV byte ptr ES:[BX],AL
0000:444b  26 c7 87 00 20 e1 00  MOV word ptr ES:[BX + 0x2000],0xe1
0000:4452  33 c0              XOR AX,AX
0000:4454  81 fa 00 01        CMP DX,0x100
0000:4458  72 08              JC 0x0000:4462
0000:445a  81 ea 00 01        SUB DX,0x100
0000:445e  fe c0              INC AL
0000:4460  eb f2              JMP 0x0000:4454
0000:4462  3c 0a              CMP AL,0xa
0000:4464  72 02              JC 0x0000:4468
0000:4466  04 07              ADD AL,0x7
0000:4468  04 30              ADD AL,0x30
0000:446a  26 88 47 02        MOV byte ptr ES:[BX + 0x2],AL
0000:446e  26 c7 87 02 20 e1 00  MOV word ptr ES:[BX + 0x2002],0xe1
0000:4475  33 c0              XOR AX,AX
0000:4477  83 fa 10           CMP DX,0x10
0000:447a  72 07              JC 0x0000:4483
0000:447c  83 ea 10           SUB DX,0x10
0000:447f  fe c0              INC AL
0000:4481  eb f4              JMP 0x0000:4477
0000:4483  3c 0a              CMP AL,0xa
0000:4485  72 02              JC 0x0000:4489
0000:4487  04 07              ADD AL,0x7
0000:4489  04 30              ADD AL,0x30
0000:448b  26 88 47 04        MOV byte ptr ES:[BX + 0x4],AL
0000:448f  26 c7 87 04 20 e1 00  MOV word ptr ES:[BX + 0x2004],0xe1
0000:4496  8b c2              MOV AX,DX
0000:4498  3c 0a              CMP AL,0xa
0000:449a  72 02              JC 0x0000:449e
0000:449c  04 07              ADD AL,0x7
0000:449e  04 30              ADD AL,0x30
0000:44a0  26 88 47 06        MOV byte ptr ES:[BX + 0x6],AL
0000:44a4  26 c7 87 06 20 e1 00  MOV word ptr ES:[BX + 0x2006],0xe1
0000:44ab  07                 POP ES
0000:44ac  5a                 POP DX
0000:44ad  58                 POP AX
0000:44ae  c3                 RET
0000:44af  50                 PUSH AX
0000:44b0  52                 PUSH DX
0000:44b1  06                 PUSH ES
0000:44b2  ba 00 a0           MOV DX,0xa000
0000:44b5  8e c2              MOV DX,ES
0000:44b7  8b d0              MOV DX,AX
0000:44b9  b0 30              MOV AL,0x30
0000:44bb  81 fa 10 27        CMP DX,0x2710
0000:44bf  72 08              JC 0x0000:44c9
0000:44c1  81 ea 10 27        SUB DX,0x2710
0000:44c5  fe c0              INC AL
0000:44c7  eb f2              JMP 0x0000:44bb
0000:44c9  26 88 07           MOV byte ptr ES:[BX],AL
0000:44cc  26 c7 87 00 20 e1 00  MOV word ptr ES:[BX + 0x2000],0xe1
0000:44d3  b0 30              MOV AL,0x30
0000:44d5  81 fa e8 03        CMP DX,0x3e8
0000:44d9  72 08              JC 0x0000:44e3
0000:44db  81 ea e8 03        SUB DX,0x3e8
0000:44df  fe c0              INC AL
0000:44e1  eb f2              JMP 0x0000:44d5
0000:44e3  26 88 47 02        MOV byte ptr ES:[BX + 0x2],AL
0000:44e7  26 c7 87 02 20 e1 00  MOV word ptr ES:[BX + 0x2002],0xe1
0000:44ee  b0 30              MOV AL,0x30
0000:44f0  83 fa 64           CMP DX,0x64
0000:44f3  72 07              JC 0x0000:44fc
0000:44f5  83 ea 64           SUB DX,0x64
0000:44f8  fe c0              INC AL
0000:44fa  eb f4              JMP 0x0000:44f0
0000:44fc  26 88 47 04        MOV byte ptr ES:[BX + 0x4],AL
0000:4500  26 c7 87 04 20 e1 00  MOV word ptr ES:[BX + 0x2004],0xe1
0000:4507  b0 30              MOV AL,0x30
0000:4509  83 fa 0a           CMP DX,0xa
0000:450c  72 07              JC 0x0000:4515
0000:450e  83 ea 0a           SUB DX,0xa
0000:4511  fe c0              INC AL
0000:4513  eb f4              JMP 0x0000:4509
0000:4515  26 88 47 06        MOV byte ptr ES:[BX + 0x6],AL
0000:4519  26 c7 87 06 20 e1 00  MOV word ptr ES:[BX + 0x2006],0xe1
0000:4520  80 c2 30           ADD DL,0x30
0000:4523  26 88 57 08        MOV byte ptr ES:[BX + 0x8],DL
0000:4527  26 c7 87 08 20 e1 00  MOV word ptr ES:[BX + 0x2008],0xe1
0000:452e  07                 POP ES
0000:452f  5a                 POP DX
0000:4530  58                 POP AX
0000:4531  c3                 RET
0000:481b  80 fd 02           CMP CH,0x2
0000:481e  75 03              JNZ 0x0000:4823
0000:4820  e9 57 02           JMP 0x0000:4a7a
0000:4823  bd 4d 00           MOV BP,0x4d
0000:4826  b0 c0              MOV AL,0xc0
0000:4828  e6 7c              OUT 0x7c,AL
0000:482a  33 c0              XOR AX,AX
0000:482c  e6 7e              OUT 0x7e,AL
0000:482e  e6 7e              OUT 0x7e,AL
0000:4830  e6 7e              OUT 0x7e,AL
0000:4832  f6 c5 01           TEST CH,0x1
0000:4835  75 02              JNZ 0x0000:4839
0000:4837  f6 d0              NOT AL
0000:4839  e6 7e              OUT 0x7e,AL
0000:483b  81 ff 00 78        CMP DI,0x7800
0000:483f  7e 03              JLE 0x0000:4844
0000:4841  e9 81 01           JMP 0x0000:49c5
0000:4844  0a c9              OR CL,CL
0000:4846  74 03              JZ 0x0000:484b
0000:4848  e9 ed 00           JMP 0x0000:4938
0000:484b  b8 4e 00           MOV AX,0x4e
0000:484e  a5                 MOVSW ES:DI,SI
0000:484f  03 f8              ADD DI,AX
0000:4851  a5                 MOVSW ES:DI,SI
0000:4852  03 f8              ADD DI,AX
0000:4854  a5                 MOVSW ES:DI,SI
0000:4855  03 f8              ADD DI,AX
0000:4857  a5                 MOVSW ES:DI,SI
0000:4858  03 f8              ADD DI,AX
0000:485a  a5                 MOVSW ES:DI,SI
0000:485b  03 f8              ADD DI,AX
0000:485d  a5                 MOVSW ES:DI,SI
0000:485e  03 f8              ADD DI,AX
0000:4860  a5                 MOVSW ES:DI,SI
0000:4861  03 f8              ADD DI,AX
0000:4863  a5                 MOVSW ES:DI,SI
0000:4864  03 f8              ADD DI,AX
0000:4866  a5                 MOVSW ES:DI,SI
0000:4867  03 f8              ADD DI,AX
0000:4869  a5                 MOVSW ES:DI,SI
0000:486a  03 f8              ADD DI,AX
0000:486c  a5                 MOVSW ES:DI,SI
0000:486d  03 f8              ADD DI,AX
0000:486f  a5                 MOVSW ES:DI,SI
0000:4870  03 f8              ADD DI,AX
0000:4872  a5                 MOVSW ES:DI,SI
0000:4873  03 f8              ADD DI,AX
0000:4875  a5                 MOVSW ES:DI,SI
0000:4876  03 f8              ADD DI,AX
0000:4878  a5                 MOVSW ES:DI,SI
0000:4879  03 f8              ADD DI,AX
0000:487b  a5                 MOVSW ES:DI,SI
0000:487c  81 ef b2 04        SUB DI,0x4b2
0000:4880  b0 ff              MOV AL,0xff
0000:4882  e6 7e              OUT 0x7e,AL
0000:4884  e6 7e              OUT 0x7e,AL
0000:4886  e6 7e              OUT 0x7e,AL
0000:4888  b0 ce              MOV AL,0xce
0000:488a  e6 7c              OUT 0x7c,AL
0000:488c  b8 4e 00           MOV AX,0x4e
0000:488f  a5                 MOVSW ES:DI,SI
0000:4890  03 f8              ADD DI,AX
0000:4892  a5                 MOVSW ES:DI,SI
0000:4893  03 f8              ADD DI,AX
0000:4895  a5                 MOVSW ES:DI,SI
0000:4896  03 f8              ADD DI,AX
0000:4898  a5                 MOVSW ES:DI,SI
0000:4899  03 f8              ADD DI,AX
0000:489b  a5                 MOVSW ES:DI,SI
0000:489c  03 f8              ADD DI,AX
0000:489e  a5                 MOVSW ES:DI,SI
0000:489f  03 f8              ADD DI,AX
0000:48a1  a5                 MOVSW ES:DI,SI
0000:48a2  03 f8              ADD DI,AX
0000:48a4  a5                 MOVSW ES:DI,SI
0000:48a5  03 f8              ADD DI,AX
0000:48a7  a5                 MOVSW ES:DI,SI
0000:48a8  03 f8              ADD DI,AX
0000:48aa  a5                 MOVSW ES:DI,SI
0000:48ab  03 f8              ADD DI,AX
0000:48ad  a5                 MOVSW ES:DI,SI
0000:48ae  03 f8              ADD DI,AX
0000:48b0  a5                 MOVSW ES:DI,SI
0000:48b1  03 f8              ADD DI,AX
0000:48b3  a5                 MOVSW ES:DI,SI
0000:48b4  03 f8              ADD DI,AX
0000:48b6  a5                 MOVSW ES:DI,SI
0000:48b7  03 f8              ADD DI,AX
0000:48b9  a5                 MOVSW ES:DI,SI
0000:48ba  03 f8              ADD DI,AX
0000:48bc  a5                 MOVSW ES:DI,SI
0000:48bd  81 ef b2 04        SUB DI,0x4b2
0000:48c1  b0 cd              MOV AL,0xcd
0000:48c3  e6 7c              OUT 0x7c,AL
0000:48c5  b8 4e 00           MOV AX,0x4e
0000:48c8  a5                 MOVSW ES:DI,SI
0000:48c9  03 f8              ADD DI,AX
0000:48cb  a5                 MOVSW ES:DI,SI
0000:48cc  03 f8              ADD DI,AX
0000:48ce  a5                 MOVSW ES:DI,SI
0000:48cf  03 f8              ADD DI,AX
0000:48d1  a5                 MOVSW ES:DI,SI
0000:48d2  03 f8              ADD DI,AX
0000:48d4  a5                 MOVSW ES:DI,SI
0000:48d5  03 f8              ADD DI,AX
0000:48d7  a5                 MOVSW ES:DI,SI
0000:48d8  03 f8              ADD DI,AX
0000:48da  a5                 MOVSW ES:DI,SI
0000:48db  03 f8              ADD DI,AX
0000:48dd  a5                 MOVSW ES:DI,SI
0000:48de  03 f8              ADD DI,AX
0000:48e0  a5                 MOVSW ES:DI,SI
0000:48e1  03 f8              ADD DI,AX
0000:48e3  a5                 MOVSW ES:DI,SI
0000:48e4  03 f8              ADD DI,AX
0000:48e6  a5                 MOVSW ES:DI,SI
0000:48e7  03 f8              ADD DI,AX
0000:48e9  a5                 MOVSW ES:DI,SI
0000:48ea  03 f8              ADD DI,AX
0000:48ec  a5                 MOVSW ES:DI,SI
0000:48ed  03 f8              ADD DI,AX
0000:48ef  a5                 MOVSW ES:DI,SI
0000:48f0  03 f8              ADD DI,AX
0000:48f2  a5                 MOVSW ES:DI,SI
0000:48f3  03 f8              ADD DI,AX
0000:48f5  a5                 MOVSW ES:DI,SI
0000:48f6  81 ef b2 04        SUB DI,0x4b2
0000:48fa  b0 cb              MOV AL,0xcb
0000:48fc  e6 7c              OUT 0x7c,AL
0000:48fe  b8 4e 00           MOV AX,0x4e
0000:4901  a5                 MOVSW ES:DI,SI
0000:4902  03 f8              ADD DI,AX
0000:4904  a5                 MOVSW ES:DI,SI
0000:4905  03 f8              ADD DI,AX
0000:4907  a5                 MOVSW ES:DI,SI
0000:4908  03 f8              ADD DI,AX
0000:490a  a5                 MOVSW ES:DI,SI
0000:490b  03 f8              ADD DI,AX
0000:490d  a5                 MOVSW ES:DI,SI
0000:490e  03 f8              ADD DI,AX
0000:4910  a5                 MOVSW ES:DI,SI
0000:4911  03 f8              ADD DI,AX
0000:4913  a5                 MOVSW ES:DI,SI
0000:4914  03 f8              ADD DI,AX
0000:4916  a5                 MOVSW ES:DI,SI
0000:4917  03 f8              ADD DI,AX
0000:4919  a5                 MOVSW ES:DI,SI
0000:491a  03 f8              ADD DI,AX
0000:491c  a5                 MOVSW ES:DI,SI
0000:491d  03 f8              ADD DI,AX
0000:491f  a5                 MOVSW ES:DI,SI
0000:4920  03 f8              ADD DI,AX
0000:4922  a5                 MOVSW ES:DI,SI
0000:4923  03 f8              ADD DI,AX
0000:4925  a5                 MOVSW ES:DI,SI
0000:4926  03 f8              ADD DI,AX
0000:4928  a5                 MOVSW ES:DI,SI
0000:4929  03 f8              ADD DI,AX
0000:492b  a5                 MOVSW ES:DI,SI
0000:492c  03 f8              ADD DI,AX
0000:492e  a5                 MOVSW ES:DI,SI
0000:492f  81 ef b2 04        SUB DI,0x4b2
0000:4933  33 c0              XOR AX,AX
0000:4935  e6 7c              OUT 0x7c,AL
0000:4937  c3                 RET
0000:4938  b5 10              MOV CH,0x10
0000:493a  ad                 LODSW SI
0000:493b  8a f4              MOV DH,AH
0000:493d  32 d2              XOR DL,DL
0000:493f  86 c4              XCHG AH,AL
0000:4941  d3 e8              SHR AX,CL
0000:4943  86 c4              XCHG AH,AL
0000:4945  ab                 STOSW ES:DI
0000:4946  d3 ea              SHR DX,CL
0000:4948  8a c2              MOV AL,DL
0000:494a  aa                 STOSB ES:DI
0000:494b  03 fd              ADD DI,BP
0000:494d  fe cd              DEC CH
0000:494f  75 e9              JNZ 0x0000:493a
0000:4951  81 ef 00 05        SUB DI,0x500
0000:4955  b0 ff              MOV AL,0xff
0000:4957  e6 7e              OUT 0x7e,AL
0000:4959  e6 7e              OUT 0x7e,AL
0000:495b  e6 7e              OUT 0x7e,AL
0000:495d  b0 ce              MOV AL,0xce
0000:495f  e6 7c              OUT 0x7c,AL
0000:4961  b5 10              MOV CH,0x10
0000:4963  ad                 LODSW SI
0000:4964  8a f4              MOV DH,AH
0000:4966  32 d2              XOR DL,DL
0000:4968  86 c4              XCHG AH,AL
0000:496a  d3 e8              SHR AX,CL
0000:496c  86 c4              XCHG AH,AL
0000:496e  ab                 STOSW ES:DI
0000:496f  d3 ea              SHR DX,CL
0000:4971  8a c2              MOV AL,DL
0000:4973  aa                 STOSB ES:DI
0000:4974  03 fd              ADD DI,BP
0000:4976  fe cd              DEC CH
0000:4978  75 e9              JNZ 0x0000:4963
0000:497a  81 ef 00 05        SUB DI,0x500
0000:497e  b0 cd              MOV AL,0xcd
0000:4980  e6 7c              OUT 0x7c,AL
0000:4982  b5 10              MOV CH,0x10
0000:4984  ad                 LODSW SI
0000:4985  8a f4              MOV DH,AH
0000:4987  32 d2              XOR DL,DL
0000:4989  86 c4              XCHG AH,AL
0000:498b  d3 e8              SHR AX,CL
0000:498d  86 c4              XCHG AH,AL
0000:498f  ab                 STOSW ES:DI
0000:4990  d3 ea              SHR DX,CL
0000:4992  8a c2              MOV AL,DL
0000:4994  aa                 STOSB ES:DI
0000:4995  03 fd              ADD DI,BP
0000:4997  fe cd              DEC CH
0000:4999  75 e9              JNZ 0x0000:4984
0000:499b  81 ef 00 05        SUB DI,0x500
0000:499f  b0 cb              MOV AL,0xcb
0000:49a1  e6 7c              OUT 0x7c,AL
0000:49a3  b5 10              MOV CH,0x10
0000:49a5  ad                 LODSW SI
0000:49a6  8a f4              MOV DH,AH
0000:49a8  32 d2              XOR DL,DL
0000:49aa  86 c4              XCHG AH,AL
0000:49ac  d3 e8              SHR AX,CL
0000:49ae  86 c4              XCHG AH,AL
0000:49b0  ab                 STOSW ES:DI
0000:49b1  d3 ea              SHR DX,CL
0000:49b3  8a c2              MOV AL,DL
0000:49b5  aa                 STOSB ES:DI
0000:49b6  03 fd              ADD DI,BP
0000:49b8  fe cd              DEC CH
0000:49ba  75 e9              JNZ 0x0000:49a5
0000:49bc  81 ef 00 05        SUB DI,0x500
0000:49c0  33 c0              XOR AX,AX
0000:49c2  e6 7c              OUT 0x7c,AL
0000:49c4  c3                 RET
0000:49c5  b5 10              MOV CH,0x10
0000:49c7  81 ff 00 7d        CMP DI,0x7d00
0000:49cb  72 04              JC 0x0000:49d1
0000:49cd  81 ef 00 7d        SUB DI,0x7d00
0000:49d1  ad                 LODSW SI
0000:49d2  8a f4              MOV DH,AH
0000:49d4  32 d2              XOR DL,DL
0000:49d6  86 c4              XCHG AH,AL
0000:49d8  d3 e8              SHR AX,CL
0000:49da  86 c4              XCHG AH,AL
0000:49dc  ab                 STOSW ES:DI
0000:49dd  d3 ea              SHR DX,CL
0000:49df  8a c2              MOV AL,DL
0000:49e1  aa                 STOSB ES:DI
0000:49e2  03 fd              ADD DI,BP
0000:49e4  fe cd              DEC CH
0000:49e6  75 df              JNZ 0x0000:49c7
0000:49e8  81 c7 00 78        ADD DI,0x7800
0000:49ec  b0 ff              MOV AL,0xff
0000:49ee  e6 7e              OUT 0x7e,AL
0000:49f0  e6 7e              OUT 0x7e,AL
0000:49f2  e6 7e              OUT 0x7e,AL
0000:49f4  b0 ce              MOV AL,0xce
0000:49f6  e6 7c              OUT 0x7c,AL
0000:49f8  b5 10              MOV CH,0x10
0000:49fa  81 ff 00 7d        CMP DI,0x7d00
0000:49fe  72 04              JC 0x0000:4a04
0000:4a00  81 ef 00 7d        SUB DI,0x7d00
0000:4a04  ad                 LODSW SI
0000:4a05  8a f4              MOV DH,AH
0000:4a07  32 d2              XOR DL,DL
0000:4a09  86 c4              XCHG AH,AL
0000:4a0b  d3 e8              SHR AX,CL
0000:4a0d  86 c4              XCHG AH,AL
0000:4a0f  ab                 STOSW ES:DI
0000:4a10  d3 ea              SHR DX,CL
0000:4a12  8a c2              MOV AL,DL
0000:4a14  aa                 STOSB ES:DI
0000:4a15  03 fd              ADD DI,BP
0000:4a17  fe cd              DEC CH
0000:4a19  75 df              JNZ 0x0000:49fa
0000:4a1b  81 c7 00 78        ADD DI,0x7800
0000:4a1f  b0 cd              MOV AL,0xcd
0000:4a21  e6 7c              OUT 0x7c,AL
0000:4a23  b5 10              MOV CH,0x10
0000:4a25  81 ff 00 7d        CMP DI,0x7d00
0000:4a29  72 04              JC 0x0000:4a2f
0000:4a2b  81 ef 00 7d        SUB DI,0x7d00
0000:4a2f  ad                 LODSW SI
0000:4a30  8a f4              MOV DH,AH
0000:4a32  32 d2              XOR DL,DL
0000:4a34  86 c4              XCHG AH,AL
0000:4a36  d3 e8              SHR AX,CL
0000:4a38  86 c4              XCHG AH,AL
0000:4a3a  ab                 STOSW ES:DI
0000:4a3b  d3 ea              SHR DX,CL
0000:4a3d  8a c2              MOV AL,DL
0000:4a3f  aa                 STOSB ES:DI
0000:4a40  03 fd              ADD DI,BP
0000:4a42  fe cd              DEC CH
0000:4a44  75 df              JNZ 0x0000:4a25
0000:4a46  81 c7 00 78        ADD DI,0x7800
0000:4a4a  b0 cb              MOV AL,0xcb
0000:4a4c  e6 7c              OUT 0x7c,AL
0000:4a4e  b5 10              MOV CH,0x10
0000:4a50  81 ff 00 7d        CMP DI,0x7d00
0000:4a54  72 04              JC 0x0000:4a5a
0000:4a56  81 ef 00 7d        SUB DI,0x7d00
0000:4a5a  ad                 LODSW SI
0000:4a5b  8a f4              MOV DH,AH
0000:4a5d  32 d2              XOR DL,DL
0000:4a5f  86 c4              XCHG AH,AL
0000:4a61  d3 e8              SHR AX,CL
0000:4a63  86 c4              XCHG AH,AL
0000:4a65  ab                 STOSW ES:DI
0000:4a66  d3 ea              SHR DX,CL
0000:4a68  8a c2              MOV AL,DL
0000:4a6a  aa                 STOSB ES:DI
0000:4a6b  03 fd              ADD DI,BP
0000:4a6d  fe cd              DEC CH
0000:4a6f  75 df              JNZ 0x0000:4a50
0000:4a71  81 c7 00 78        ADD DI,0x7800
0000:4a75  33 c0              XOR AX,AX
0000:4a77  e6 7c              OUT 0x7c,AL
0000:4a79  c3                 RET
0000:4a7a  bd 4d 00           MOV BP,0x4d
0000:4a7d  b0 c0              MOV AL,0xc0
0000:4a7f  e6 7c              OUT 0x7c,AL
0000:4a81  b0 ff              MOV AL,0xff
0000:4a83  e6 7e              OUT 0x7e,AL
0000:4a85  e6 7e              OUT 0x7e,AL
0000:4a87  f6 d0              NOT AL
0000:4a89  e6 7e              OUT 0x7e,AL
0000:4a8b  f6 d0              NOT AL
0000:4a8d  e6 7e              OUT 0x7e,AL
0000:4a8f  81 ff 00 78        CMP DI,0x7800
0000:4a93  7f 60              JG 0x0000:4af5
0000:4a95  0a c9              OR CL,CL
0000:4a97  75 3a              JNZ 0x0000:4ad3
0000:4a99  b8 4e 00           MOV AX,0x4e
0000:4a9c  a5                 MOVSW ES:DI,SI
0000:4a9d  03 f8              ADD DI,AX
0000:4a9f  a5                 MOVSW ES:DI,SI
0000:4aa0  03 f8              ADD DI,AX
0000:4aa2  a5                 MOVSW ES:DI,SI
0000:4aa3  03 f8              ADD DI,AX
0000:4aa5  a5                 MOVSW ES:DI,SI
0000:4aa6  03 f8              ADD DI,AX
0000:4aa8  a5                 MOVSW ES:DI,SI
0000:4aa9  03 f8              ADD DI,AX
0000:4aab  a5                 MOVSW ES:DI,SI
0000:4aac  03 f8              ADD DI,AX
0000:4aae  a5                 MOVSW ES:DI,SI
0000:4aaf  03 f8              ADD DI,AX
0000:4ab1  a5                 MOVSW ES:DI,SI
0000:4ab2  03 f8              ADD DI,AX
0000:4ab4  a5                 MOVSW ES:DI,SI
0000:4ab5  03 f8              ADD DI,AX
0000:4ab7  a5                 MOVSW ES:DI,SI
0000:4ab8  03 f8              ADD DI,AX
0000:4aba  a5                 MOVSW ES:DI,SI
0000:4abb  03 f8              ADD DI,AX
0000:4abd  a5                 MOVSW ES:DI,SI
0000:4abe  03 f8              ADD DI,AX
0000:4ac0  a5                 MOVSW ES:DI,SI
0000:4ac1  03 f8              ADD DI,AX
0000:4ac3  a5                 MOVSW ES:DI,SI
0000:4ac4  03 f8              ADD DI,AX
0000:4ac6  a5                 MOVSW ES:DI,SI
0000:4ac7  03 f8              ADD DI,AX
0000:4ac9  a5                 MOVSW ES:DI,SI
0000:4aca  81 ef b2 04        SUB DI,0x4b2
0000:4ace  33 c0              XOR AX,AX
0000:4ad0  e6 7c              OUT 0x7c,AL
0000:4ad2  c3                 RET
0000:4ad3  b5 10              MOV CH,0x10
0000:4ad5  ad                 LODSW SI
0000:4ad6  8a f4              MOV DH,AH
0000:4ad8  32 d2              XOR DL,DL
0000:4ada  86 c4              XCHG AH,AL
0000:4adc  d3 e8              SHR AX,CL
0000:4ade  86 c4              XCHG AH,AL
0000:4ae0  ab                 STOSW ES:DI
0000:4ae1  d3 ea              SHR DX,CL
0000:4ae3  8a c2              MOV AL,DL
0000:4ae5  aa                 STOSB ES:DI
0000:4ae6  03 fd              ADD DI,BP
0000:4ae8  fe cd              DEC CH
0000:4aea  75 e9              JNZ 0x0000:4ad5
0000:4aec  81 ef 00 05        SUB DI,0x500
0000:4af0  33 c0              XOR AX,AX
0000:4af2  e6 7c              OUT 0x7c,AL
0000:4af4  c3                 RET
0000:4af5  b5 10              MOV CH,0x10
0000:4af7  81 ff 00 7d        CMP DI,0x7d00
0000:4afb  72 04              JC 0x0000:4b01
0000:4afd  81 ef 00 7d        SUB DI,0x7d00
0000:4b01  ad                 LODSW SI
0000:4b02  8a f4              MOV DH,AH
0000:4b04  32 d2              XOR DL,DL
0000:4b06  86 c4              XCHG AH,AL
0000:4b08  d3 e8              SHR AX,CL
0000:4b0a  86 c4              XCHG AH,AL
0000:4b0c  ab                 STOSW ES:DI
0000:4b0d  d3 ea              SHR DX,CL
0000:4b0f  8a c2              MOV AL,DL
0000:4b11  aa                 STOSB ES:DI
0000:4b12  03 fd              ADD DI,BP
0000:4b14  fe cd              DEC CH
0000:4b16  75 df              JNZ 0x0000:4af7
0000:4b18  81 c7 00 78        ADD DI,0x7800
0000:4b1c  33 c0              XOR AX,AX
0000:4b1e  e6 7c              OUT 0x7c,AL
0000:4b20  c3                 RET
0000:4b21  ba 4e 00           MOV DX,0x4e
0000:4b24  b8 00 a8           MOV AX,0xa800
0000:4b27  8e c0              MOV AX,ES
0000:4b29  a5                 MOVSW ES:DI,SI
0000:4b2a  03 fa              ADD DI,DX
0000:4b2c  a5                 MOVSW ES:DI,SI
0000:4b2d  03 fa              ADD DI,DX
0000:4b2f  a5                 MOVSW ES:DI,SI
0000:4b30  03 fa              ADD DI,DX
0000:4b32  a5                 MOVSW ES:DI,SI
0000:4b33  03 fa              ADD DI,DX
0000:4b35  a5                 MOVSW ES:DI,SI
0000:4b36  03 fa              ADD DI,DX
0000:4b38  a5                 MOVSW ES:DI,SI
0000:4b39  03 fa              ADD DI,DX
0000:4b3b  a5                 MOVSW ES:DI,SI
0000:4b3c  03 fa              ADD DI,DX
0000:4b3e  a5                 MOVSW ES:DI,SI
0000:4b3f  03 fa              ADD DI,DX
0000:4b41  a5                 MOVSW ES:DI,SI
0000:4b42  03 fa              ADD DI,DX
0000:4b44  a5                 MOVSW ES:DI,SI
0000:4b45  03 fa              ADD DI,DX
0000:4b47  a5                 MOVSW ES:DI,SI
0000:4b48  03 fa              ADD DI,DX
0000:4b4a  a5                 MOVSW ES:DI,SI
0000:4b4b  03 fa              ADD DI,DX
0000:4b4d  a5                 MOVSW ES:DI,SI
0000:4b4e  03 fa              ADD DI,DX
0000:4b50  a5                 MOVSW ES:DI,SI
0000:4b51  03 fa              ADD DI,DX
0000:4b53  a5                 MOVSW ES:DI,SI
0000:4b54  03 fa              ADD DI,DX
0000:4b56  a5                 MOVSW ES:DI,SI
0000:4b57  81 ef b2 04        SUB DI,0x4b2
0000:4b5b  b8 00 b0           MOV AX,0xb000
0000:4b5e  8e c0              MOV AX,ES
0000:4b60  a5                 MOVSW ES:DI,SI
0000:4b61  03 fa              ADD DI,DX
0000:4b63  a5                 MOVSW ES:DI,SI
0000:4b64  03 fa              ADD DI,DX
0000:4b66  a5                 MOVSW ES:DI,SI
0000:4b67  03 fa              ADD DI,DX
0000:4b69  a5                 MOVSW ES:DI,SI
0000:4b6a  03 fa              ADD DI,DX
0000:4b6c  a5                 MOVSW ES:DI,SI
0000:4b6d  03 fa              ADD DI,DX
0000:4b6f  a5                 MOVSW ES:DI,SI
0000:4b70  03 fa              ADD DI,DX
0000:4b72  a5                 MOVSW ES:DI,SI
0000:4b73  03 fa              ADD DI,DX
0000:4b75  a5                 MOVSW ES:DI,SI
0000:4b76  03 fa              ADD DI,DX
0000:4b78  a5                 MOVSW ES:DI,SI
0000:4b79  03 fa              ADD DI,DX
0000:4b7b  a5                 MOVSW ES:DI,SI
0000:4b7c  03 fa              ADD DI,DX
0000:4b7e  a5                 MOVSW ES:DI,SI
0000:4b7f  03 fa              ADD DI,DX
0000:4b81  a5                 MOVSW ES:DI,SI
0000:4b82  03 fa              ADD DI,DX
0000:4b84  a5                 MOVSW ES:DI,SI
0000:4b85  03 fa              ADD DI,DX
0000:4b87  a5                 MOVSW ES:DI,SI
0000:4b88  03 fa              ADD DI,DX
0000:4b8a  a5                 MOVSW ES:DI,SI
0000:4b8b  03 fa              ADD DI,DX
0000:4b8d  a5                 MOVSW ES:DI,SI
0000:4b8e  81 ef b2 84        SUB DI,0x84b2
0000:4b92  a5                 MOVSW ES:DI,SI
0000:4b93  03 fa              ADD DI,DX
0000:4b95  a5                 MOVSW ES:DI,SI
0000:4b96  03 fa              ADD DI,DX
0000:4b98  a5                 MOVSW ES:DI,SI
0000:4b99  03 fa              ADD DI,DX
0000:4b9b  a5                 MOVSW ES:DI,SI
0000:4b9c  03 fa              ADD DI,DX
0000:4b9e  a5                 MOVSW ES:DI,SI
0000:4b9f  03 fa              ADD DI,DX
0000:4ba1  a5                 MOVSW ES:DI,SI
0000:4ba2  03 fa              ADD DI,DX
0000:4ba4  a5                 MOVSW ES:DI,SI
0000:4ba5  03 fa              ADD DI,DX
0000:4ba7  a5                 MOVSW ES:DI,SI
0000:4ba8  03 fa              ADD DI,DX
0000:4baa  a5                 MOVSW ES:DI,SI
0000:4bab  03 fa              ADD DI,DX
0000:4bad  a5                 MOVSW ES:DI,SI
0000:4bae  03 fa              ADD DI,DX
0000:4bb0  a5                 MOVSW ES:DI,SI
0000:4bb1  03 fa              ADD DI,DX
0000:4bb3  a5                 MOVSW ES:DI,SI
0000:4bb4  03 fa              ADD DI,DX
0000:4bb6  a5                 MOVSW ES:DI,SI
0000:4bb7  03 fa              ADD DI,DX
0000:4bb9  a5                 MOVSW ES:DI,SI
0000:4bba  03 fa              ADD DI,DX
0000:4bbc  a5                 MOVSW ES:DI,SI
0000:4bbd  03 fa              ADD DI,DX
0000:4bbf  a5                 MOVSW ES:DI,SI
0000:4bc0  81 ef b2 84        SUB DI,0x84b2
0000:4bc4  b8 00 e0           MOV AX,0xe000
0000:4bc7  8e c0              MOV AX,ES
0000:4bc9  ba 50 00           MOV DX,0x50
0000:4bcc  33 c0              XOR AX,AX
0000:4bce  26 89 05           MOV word ptr ES:[DI],AX
0000:4bd1  03 fa              ADD DI,DX
0000:4bd3  26 89 05           MOV word ptr ES:[DI],AX
0000:4bd6  03 fa              ADD DI,DX
0000:4bd8  26 89 05           MOV word ptr ES:[DI],AX
0000:4bdb  03 fa              ADD DI,DX
0000:4bdd  26 89 05           MOV word ptr ES:[DI],AX
0000:4be0  03 fa              ADD DI,DX
0000:4be2  26 89 05           MOV word ptr ES:[DI],AX
0000:4be5  03 fa              ADD DI,DX
0000:4be7  26 89 05           MOV word ptr ES:[DI],AX
0000:4bea  03 fa              ADD DI,DX
0000:4bec  26 89 05           MOV word ptr ES:[DI],AX
0000:4bef  03 fa              ADD DI,DX
0000:4bf1  26 89 05           MOV word ptr ES:[DI],AX
0000:4bf4  03 fa              ADD DI,DX
0000:4bf6  26 89 05           MOV word ptr ES:[DI],AX
0000:4bf9  03 fa              ADD DI,DX
0000:4bfb  26 89 05           MOV word ptr ES:[DI],AX
0000:4bfe  03 fa              ADD DI,DX
0000:4c00  26 89 05           MOV word ptr ES:[DI],AX
0000:4c03  03 fa              ADD DI,DX
0000:4c05  26 89 05           MOV word ptr ES:[DI],AX
0000:4c08  03 fa              ADD DI,DX
0000:4c0a  26 89 05           MOV word ptr ES:[DI],AX
0000:4c0d  03 fa              ADD DI,DX
0000:4c0f  26 89 05           MOV word ptr ES:[DI],AX
0000:4c12  03 fa              ADD DI,DX
0000:4c14  26 89 05           MOV word ptr ES:[DI],AX
0000:4c17  03 fa              ADD DI,DX
0000:4c19  26 89 05           MOV word ptr ES:[DI],AX
0000:4c1c  81 ef b0 04        SUB DI,0x4b0
0000:4c20  c3                 RET
