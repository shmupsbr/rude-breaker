# Rude Breaker map

Addresses are load addresses (file offset + `0x100`) unless a row says "chunk offset". Chunk offsets are into decompressed `SGM1.DAT` chunk 1, which is bytecode plus a few native islands and is not loaded at `0x100`.

Confidence: **high** = instruction flow checked, **medium** = structure is clear but a name or a caller is inferred, **low** = guess.

## How the disk boots

| Item | Where | Confidence |
|------|--------|------------|
| On-disk `SGC.COM` | 6,140 bytes. Not the game. | high |
| Stub | `SGC.COM` `0x100`–`0x16D` | high |
| LZ77 payload | `SGC.COM` `0x16E` to EOF. Control `0x00` ends; `0x01`–`0x7F` is a literal run; `0x80\|n` is a match of length `(n & 0x7F) + 3` with the next byte as distance-1 (1..256). | high |
| Decompressed loader | 31,041 bytes. First instruction `E9 31 57` jumps to `0x5834`. Banner `SgcPr com1.00 [001] 1995.12.8`. | high |
| Game image | `SGM1` chunk 0, 19,233 bytes. First instruction jumps to `0x764`. | high |
| Stage / actor scripts | `SGM1` chunks 1–6. Chunk 1 decompresses to 44,206 bytes. First word `0x78B0` is the script entry. | high |

`SGM1.DAT` directory is the first `0x1800` bytes: overlapping `{u16 bank, u16 off}` pairs, absolute = `bank * 0x10000 + off`. Entry 37 is the EOF sentinel. 37 data chunks. Chunks that cross a 64K boundary are read two bytes long; decompression stops at the `0x00` terminator.

## Packed stub (`SGC.COM`)

| Address | Name | What it does | Confidence |
|---------|------|----------------|------------|
| `0x100` | `com_stub` | `pusha`, copy `0x7F80` words from `CS:0x100` to a segment `0x1000` paragraphs up, far-return to that segment at `0x11B`. | high |
| `0x11B` | `com_stub_relocated` | Point `DS:SI` at `0x16E` and LZ77-decompress over the image, then `popa` and far-return to `CS:0x100` of the new image. | high |

Ghidra's 16-bit listing sometimes swaps `MOV Sreg, r/m` with the reverse move. The byte sequence above is the one that matches the file.

## Loader (decompressed `SGC.COM`)

State pointer `BP` is `0x34C`. `SS=DS=ES=CS` for the loader. The game later receives this same `BP`.

| Address | Name | Role | Confidence |
|---------|------|------|------------|
| `0x100` | `com_entry` | `jmp 0x5834` | high |
| `0x5834` | `sgc_startup` | Memory check (needs about 460 KiB; string `460k(471,040)byte`), resize via INT 21h AH=4Ah, allocate the game arena, hook INT 05/06/23/24/0Ah/7Bh/7Fh, load `READJS.COM` and `FPLAY.COM`, then load chunk 0 and far-call it. | high |
| `0x5A62` | `init_bp_pointers` | Fills the `BP` function-pointer table below. | high |
| `0x5B20` | `dos_exec` | INT 21h AX=4B00. Parameter block built at `0x5B53`. | high |
| `0x5CB9` | `wait_key` | Polls the BIOS key matrix at `0000:052A`. | high |
| `0x5D26` | `sgm_load` | Far. `AL` flags, `AH` index of a destination segment in `BP`, `BH` filename-table index, `BL` TOC record. Reads the `0x1800` directory, then decompresses or copies one record. | high |
| `0x64F0` | `dos_read` | INT 21h open / lseek / read. | high |
| `0x65C4` | `dos_write` | INT 21h write. | high |
| `0x6692` | `dos_delete` | INT 21h delete. | high |
| `0x67EA` | `disk_error_screen` | Disk error text. | high |
| `0x6AA0` | `lz_decompress` | One-shot LZ77 into a buffer. | high |
| `0x6AA4` | `lz_to_gvram_planes` | Same LZ77, once per plane, into `A800` / `B000` / `B800` / `E000`. | high |
| `0x6ACD` | `lz_core` | The matcher. | high |
| `0x6C0B` | `frame_hook` | Called once per frame from the game loop (`[bp+0x34]`). Body not fully named. | medium |
| `0x7428` | `blit_expand` | Planar expand / blit helper. | medium |
| `0x77D4` | `fill_vram_rect` | Rectangle fill. | medium |
| `0x7846` | `poll_input` | Builds the held-input word. See Input. | high |

`[bp+offset] = loader address` set by `0x5A62` (high):

| BP | Address | Name |
|----|---------|------|
| `+0x28` | `0x64F0` | read |
| `+0x2A` | `0x65C4` | write |
| `+0x2C` | `0x6692` | delete |
| `+0x2E` | `0x6AA0` | lz |
| `+0x30` | `0x6AA4` | lz to VRAM planes |
| `+0x32` | `0x5D26` | load record |
| `+0x34` | `0x6C0B` | per-frame hook |
| `+0x3C` | `0x5EF5` | called beside the vsync wait |
| `+0x3E` | `0x5CB9` | wait key |
| `+0x40` | `0x7846` | input |
| `+0x42` | `0x7428` | blit |
| `+0x44` | `0x77D4` | fill |
| `+0x50` | `0x430` | filename pointer table |
| `+0x52` | `0x43A` | entity array (256 × `0x52`) |
| `+0x54` | `0x563A` | byte variables |
| `+0x56` | `0x564A` | `0x6E`-word state block |

## Game image (chunk 0)

| Address | Name | Role | Confidence |
|---------|------|------|------------|
| `0x100` | `com_entry` | `jmp 0x764` | high |
| `0x764` | `game_entry` | `DS=CS`. `BP` is the word the loader stored at `CS:0x81`. Checks the `SgcPr` signature at `CS:0x73E`. Installs filenames `SGM1.DAT`, `RBRANK.DAT`, `RANKDAT_TXT`. Allocates DOS blocks. Calls `0xE20` then `0x88B`. | high |
| `0x88B` | `run_game` | Stage reload wrapper. The frame loop returns here. | high |
| `0x8C7` | spawn first entity | One slot, type `0x4000`, update `0x2FD0`, script segment from the loader allocation. | high |
| `0x933` | `frame_loop` | See below. | high |
| `0xA36` | `quit_path` | Clears the quit edge, may call INT 18h AH=41, fills GVRAM `A800`, jumps to stage reload `0x8E6`. | high |
| `0xBD9` | `pause_if_flag` | If `[bp+0x72]` bit `0x8000`, save text cells and spin. | medium |
| `0xCE6` | `apply_debug_keys` | Folds extra key edges into `[bp+0x72]`. | high |
| `0xDEC` | `init_play_state` | Zeros scroll (`+0x74`, `+0x76`, `+0x7A`), sets width `+0x7E = 0x1E0`, scroll speed `+0x82 = 2`. | high |
| `0xE20` | `rng_seed` | INT 1Ch tick into `CS:0xE7E`. | high |
| `0xE4E` | `rng_mix` | 16-bit mixer over `[bp+0x5C]`, `[bp+0x64]`, `CS:0xE7E`. | high |
| `0xE80` | `sincos` | Quadrant lookup at `CS:0x1848`. | medium |
| `0xFB9` | `crtc_set_scroll` | GDC ports `0xA2` (register) / `0xA0` (data), regs `0x70`–`0x77`. Vertical constant `0x190` (400). | high |
| `0x108E` | `scroll_row_update` | Called when a scroll step crosses a cell row. | medium |
| `0x10D2` | `compose_sprites` | Sprite compose. Skipped when `[bp+0xC1] != 0`. | medium |
| `0x1132` | `draw_entity` | Blit one entity. GVRAM `A800`, GRCG, cell = coordinate >> 4. | high |
| `0x12C5` | `draw_text` | Text VRAM `A000`. Control bytes `F0`–`FF`. | high |
| `0x15B6` | `ranking_exit` | Called on the quit path before the wipe. | medium |
| `0x1648` | `load_rank_state` | Loads `0x6E` words into `[bp+0x56]`. Sets `[bp+0xB2] = 2` and `[bp+0x62] = 2` (lives-like, not confirmed). | medium |
| `0x1717` | `debug_hud` | Prints the labels below when `[bp+0x72]` bit `0x2000` is set. | high |
| `0x1A30` | `entity_dispatch` | 256 slots, stride `0x52`, base `[bp+0x52]`. If word `[si] != 0`, call word `[si+2]`. | high |
| `0x1A41` | `entity_alloc` | First free slot. `[si] = AX` type, `[si+2] = DX` function. | high |
| `0x1A53` | `entity_alloc_back` | Same from the end of the array. | high |
| `0x1AAD` | `entity_clear_one` | Zeros `0x29` words. | high |
| `0x2DD0` | `vm_opcode_table` | 256 words. Opcodes `0x00`–`0x50` are real. `0x51`–`0xFF` point at `0x2FF4`. | high |
| `0x2FD0` | `vm_execute` | Installs a return thunk and falls into `vm_fetch`. | high |
| `0x2FD6` | `vm_fetch` | Byte at `ES:[DI]`, `DI` from `[si+8]`, segment from `[si+0x0A]`. `jmp cs:[bx*2+0x2DD0]`. | high |
| `0x35D8` | `vm_subop_table` | Even sub-opcode ids, used by opcodes `0x20`–`0x24` and `0x4F`/`0x50`. | high |
| `0x3604` | `vm_subop_skip` | Byte length to skip when the target entity is absent. Valid through sub-op `0x2A`. | high |
| `0x3BAE` | `byte_alu` | Byte operators. Index is the raw script byte. | high |
| `0x3C2A` | `word_alu` | Word operators. `0x00` mov, `0x06` add, `0x08` sub, `0x12` and, `0x14` or, `0x16` xor, `0x1C` random modulo, `0x1E` shl, `0x20` sar. | high |
| `0x3CB8` | `cmp_alu` | `0x00` eq … `0x12` jle, `0x14` test-zero, `0x16` test-nonzero. Carry means "branch taken" for the conditional opcodes. | high |
| `0x4144` | `scroll_step` | Opcode `0x4B`. `[bp+0x74] += [bp+0x82]`, wrap `0x190`. `[bp+0x76] = 0x190 - [bp+0x74]`. Also steps `[bp+0x7A]` by the speed and wraps on `[bp+0x7E] << 4`. | high |
| `0x41A7` | opcode `0x4C` | `compose_sprites` unless `[bp+0xC1] != 0`. | high |
| `0x41FD` | opcode `0x4E` | Draw every entity whose type bit `0x200` is clear, via `0x1132`. | high |
| `0x422A` | `hit_test_a` | Opcode `0x4F`. AABB of `(x+[0x42], x+[0x44])` by `(y+[0x46], y+[0x48])`. | high |
| `0x42D1` | `hit_test_b` | Opcode `0x50`. Same test, second pass. | high |

### Frame loop (`0x933`)

Each iteration, high confidence from the listing and the decompiler:

1. If `[bp+0x6E]` bit `0x8000` (quit edge), `quit_path`.
2. Far-call `[bp+0x40]` → `poll_input`.
3. `apply_debug_keys`, `pause_if_flag`, `debug_hud`.
4. `entity_dispatch`.
5. Unless `[bp+0x72]` bit `0x4000`, wait on GDC vsync (port `0x60` bit `0x20`), with PIC port `0x0A` bit `0x10` gated by flag bit `0x1000`. Far-call `[bp+0x3C]` beside that wait.
6. `crtc_set_scroll`.
7. Page flip: OUT `0xA6` / `0xA4` toggling `[bp+0xBA]`.
8. Far-call `[bp+0x34]` (loader `0x6C0B`).
9. Optional OUT `0x6C`, AL=`0x10` when debug HUD flag `0x2000` is set.
10. Loop.

There is no separate "player update" x86 function in this image. The ship is an entity whose update pointer is the bytecode VM.

### Entity record (stride `0x52`)

| Offset | Meaning | Confidence |
|--------|---------|------------|
| `+0x00` | Type. 0 = free. Masks `0x0100`, `0x0200`, `0x1000`, `0x8000`, `0xA000` used as classes. | high |
| `+0x02` | Update function. | high |
| `+0x04` | Id compared by opcodes `0x23` / `0x24`. | high |
| `+0x06` | Timer. | medium |
| `+0x08` | Script IP. | high |
| `+0x0A` | Script segment. | high |
| `+0x0C` | Call-stack pointer. | high |
| `+0x22` | X. | high |
| `+0x24` | Y. | high |
| `+0x38` | Sprite word. | medium |
| `+0x3C` | Hit counter. Decremented by the hit opcodes. | high |
| `+0x42` / `+0x44` | Left / right hitbox offsets added to X. | high |
| `+0x46` / `+0x48` | Top / bottom hitbox offsets added to Y. | high |
| `+0x4C` / `+0x4D` | Draw width / height in cells. | medium |
| `+0x4E` / `+0x4F` | Draw masks. | low |

### Debug HUD labels (`0x1717`)

Printed only with flag `0x2000`. The two characters are a little-endian immediate.

| BP | Label | Note |
|----|-------|------|
| `+0x5C` | `cI` | RNG term |
| `+0x66` | `dI` | |
| `+0x68` | `DZ` | |
| `+0x72` | `FC` | debug / speed flags |
| `+0x74` | `DG` | scroll Y |
| `+0x7C` | `pm` | |
| `+0x82` | `ps` | scroll speed |
| `+0x96` | `xM` | published ship X. x86 only reads it; scripts and the ship routine write it. |
| `+0x98` | `yM` | published ship Y |
| `+0xAE` | `cE` | |
| `+0xB0` | `wE` | |
| `+0xB9` | `it` / `ST` | bit 2 mutes music |
| `+0xC2` | `KH` | second key group |
| `+0xC3` | `SH` | |
| `+0xDF` | `CU` | |
| `+0xE0` | `LF` | |

## Input (`poll_input`, loader `0x7846`)

Copies 16 bytes from BIOS `0000:052A` and ORs a second sample. Held bits land in `[bp+0x6C]`. Edge is `(~previous | 0x40) & held`, stored at `[bp+0x6E]` and `[bp+0x70]`. Bit 6 is forced into the edge every frame it is held. After 16 frames with no other edge, every even frame copies the held word to `[bp+0x70]`.

Joystick (INT 7Bh, if `[0x3BE]` bit `0x400`): AL bit0 → BX bit0, bit1 → bit3, bit2 → bit1, bit3 → bit2, bit4 → `0x10`, bit5 → `0x20`. If `[0x3BE]` bit `0x200`, button bits `0x10` and `0x20` are swapped.

Direction of the **game** nibble, confirmed by the ship velocity table (medium for the physical PC-98 key, high for the bit):

| Bit | Meaning |
|-----|---------|
| `0x0001` | up |
| `0x0002` | left |
| `0x0004` | right |
| `0x0008` | down |
| `0x0010` / `0x0020` | buttons (swappable) |
| `0x0040` | special, forced into the edge |
| `0x8000` | quit. The frame loop tests this on the edge word. |

Which matrix bit is which key cap is **low** confidence. The port maps the HAL d-pad onto this nibble and does not invent PC-98 key names.

## Video

PC-98 only. None of these belong in `src/core`.

| What | Where | Confidence |
|------|--------|------------|
| Text | `A000`, attributes `+0x2000`, via `draw_text` | high |
| Graphics | `A800` and the other planes, GRCG ports `0x7C` / `0x7E` | high |
| CRTC scroll | ports `0xA0` / `0xA2`, `crtc_set_scroll` | high |
| Page flip | ports `0xA4` / `0xA6`, byte `[bp+0xBA]` | high |
| Vsync | port `0x60` bit `0x20` | high |
| Playfield height | `0x190` = 400 | high |
| Horizontal cell limit | constants `0x18` / `0x19` inside the blit | medium |

## Sound

`FPLAY.COM` is executed at startup and owns INT 7Fh. Bytecode opcodes `0x42`–`0x46` call INT 7Fh with AH = 0, 1, 3, or 4. `[bp+0xB9]` bit `0x04` mutes. `SONG.DAT` is opened with AX=`0x0900`, INT 7Fh (loader `0x61A0`, filename at `0x61A9`). Core code calls `sys_play_sound` / `sys_play_music` only.

## Ship movement (SGM1 chunk 1)

Not a function in the game image. Native island at chunk offset `0x53B4`, `retf` at `0x54B4`.

| Chunk offset | Name | Role | Confidence |
|--------------|------|------|------------|
| `0x536E` | ship init script | Sub-op block: id `+0x04 = 0x0012`, X = `0x0140` (320), Y = `0x0158` (344), Y hitbox `-8 .. +10`, then opcode `0x30` copies X/Y out to `[bp+0x96]` / `[bp+0x98]`, then return. | medium |
| `0x53B4` | `ship_update` | On button edge bit `0x20`, toggles mode byte `[bp+0xD5]` and calls a helper at chunk `0x7260`. Sets X hitbox to `-10..+10` if mode is 1, else `-6..+6`. | high |
| `0x5404` | velocity add | `dir = [bp+0x6C] & 0x0F`, index `dir * 4` into a dx/dy table. Flag `[bp+0x72]` bit `0x10` selects the slow table, bit `0x20` the fast table, otherwise the normal table. Adds dx to X and dy to Y. | high |
| `0x54BE` | slow table | 16 × int16 dx, dy. Cardinals 6 and 5. | high |
| `0x54FE` | normal table | Cardinals 8 horizontal, 6 vertical. Diagonals 6 and 5. Opposite directions cancel. | high |
| `0x553E` | fast table | Cardinals 10 and 8. | high |
| `0x547C` | clamp | X to `[0x60, 0x220]` (96..544), Y to `[0x38, 0x168]` (56..360), inclusive. Stores both into the entity and into `[bp+0x96]` / `[bp+0x98]`. | high |

A bank-angle byte at `[bp+0xD4]` eases toward the horizontal delta, clamped to `-12..11`, and picks a sprite word. Not ported; it does not change the position.

No x86 instruction in the game image adds a speed to the ship. Searching chunk 1 for `x += held_input` finds nothing; the nibble indexes the table instead.

## Ported into the core

| Original | Core | Confidence |
|----------|------|------------|
| Frame order at `0x933`: input, entities, draw, vsync, present | `main` already had this order; the comment now cites `0x933`. CRTC and the page flip stay in the HAL. | high |
| Scroll step `[bp+0x74] += 2`, wrap 400 | `RbGame.scroll_y` / `scroll_speed` during play | high |
| Normal velocity table, direction nibble, origin clamp, spawn, default hitbox | `player_reset` / `player_update` | high for the table and the clamp, medium for treating chunk `0x536E` as the spawn used at stage start |

Still a stand-in: enemy waves, player shots, score, and the 64-slot pool. The original dispatcher is 256 slots of `0x52` bytes driven by the bytecode VM.

## Next

1. Walk chunk 1 from script IP `0x78B0` with the opcode sizes in this file, and name the block that far-calls `ship_update`. The offset `0x53B4` is not stored as an immediate in chunk 1 or in the game image.
2. Opcode `0x4F` / `0x50` hit responses, then replace the placeholder bullets.
3. Decode one stage chunk (2–6 share chunk 1's shape) far enough to spawn real enemies.
4. Graphic chunks whose header is `01 00 fc 00 01 ff 9c 00`, then `sys_draw_sprite` pixels.
5. INT 7Fh command map inside `FPLAY.COM`, then real `sys_play_music` track ids.
6. Loader `0x6C0B` and `0x5EF5`, the two function pointers the frame loop calls beside vsync and the page flip.
