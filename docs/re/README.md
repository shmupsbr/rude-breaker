# Reverse-engineering notes

Text only. Disk images, `SGC.COM`, and `SGM1.DAT` stay under `assets/raw/`, which is gitignored.

Ghidra 12.1.4 headless, language `x86:LE:16:Real Mode`, image base `0x100`.

| File | What it is |
|------|------------|
| `SGC.COM.c` / `SGC.COM.asm` | On-disk `SGC.COM` (6,140 bytes). A stub. The bytes from `0x16E` are an LZ77 payload, not code. |
| `sgc_loader.c` / `sgc_loader.asm` | Stub payload after decompression (31,041 bytes). DOS loader: memory check, `SGM1.DAT` directory, LZ77, input poll. Real entry is the jump at `0x100` to `0x5834`. |
| `sgc_game.c` / `sgc_game.asm` | `SGM1.DAT` chunk 0 after decompression (19,233 bytes). Native game image, entered at `0x764`. |
| `MAP.md` | Addresses, names, confidence. |

The 16-bit decompiler drops segment context (`unaff_BP` is the loader's state pointer). Prefer `MAP.md` when the pseudo-C and the listing disagree. Player movement is not in these three images; it is native code inside `SGM1` chunk 1 and is described in `MAP.md`.
