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
| `0x2FD6` | `vm_fetch` | Reads the opcode, increments `DI`, stores the IP past the opcode, then jumps to the handler. Opcode `0x04` (`0x304A`) only ends this frame; the next frame continues at the next byte. It is a one-frame yield, not a script terminator. Opcode `0x00` yields forever. Opcode `0x06` waits N frames before the next opcode (N=1 runs it on the following frame). | high |
| `0x35D8` | `vm_subop_table` | Even sub-opcode ids, used by opcodes `0x20`–`0x24` and `0x4F`/`0x50`. | high |
| `0x3604` | `vm_subop_skip` | Byte length to skip when the target entity is absent. Valid through sub-op `0x2A`. | high |
| `0x3BAE` | `byte_alu` | Byte operators. Index is the raw script byte. | high |
| `0x3C2A` | `word_alu` | Word operators. `0x00` mov, `0x06` add, `0x08` sub, `0x12` and, `0x14` or, `0x16` xor, `0x1C` random modulo, `0x1E` shl, `0x20` sar. | high |
| `0x3CB8` | `cmp_alu` | `0x00` eq, `0x02` ne, `0x04` unsigned `>`, `0x06` unsigned `<`, `0x08` signed `>`, `0x0A` signed `<`, then the `>=` / `<=` pairs, `0x14` test-zero, `0x16` test-nonzero. Carry clear means the condition is true (`JNC` at `0x39AF`). | high |
| `0x4144` | `scroll_step` | Opcode `0x4B`. `[bp+0x74] += [bp+0x82]`, wrap `0x190`. `[bp+0x76] = 0x190 - [bp+0x74]`. Also steps `[bp+0x7A]` by the speed and wraps on `[bp+0x7E] << 4`. | high |
| `0x41A7` | opcode `0x4C` | `compose_sprites` unless `[bp+0xC1] != 0`. | high |
| `0x41FD` | opcode `0x4E` | Draw every entity whose type bit `0x200` is clear, via `0x1132`. | high |
| `0x422A` | `hit_all` | Opcode `0x4F`. See Collision. | high |
| `0x42D1` | `hit_first` | Opcode `0x50`. Same boxes, stops on the first hit. | high |

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

The PC-98 ports stay out of the core. The SDL2 build blits a 640×400 buffer from a software GVRAM in `src/core/gfx.c`.

| What | Where | Confidence |
|------|--------|------------|
| Text | `A000`, attributes `+0x2000`, via `draw_text`. The SDL buffer paints this after the scrolled GVRAM, so the HUD does not scroll. | high |
| Graphics | `A800` and the other planes, GRCG ports `0x7C` / `0x7E` | high |
| CRTC scroll | ports `0xA0` / `0xA2`, `crtc_set_scroll` | high |
| Page flip | ports `0xA4` / `0xA6`, byte `[bp+0xBA]`. Opcode `0x31` calls loader `0x6AA4`, which expands one SGM1 record as four LZ streams into `A800` / `B000` / `B800` / `E000`. | high |
| Vsync | port `0x60` bit `0x20` | high |
| Playfield height | `0x190` = 400 | high |
| Horizontal cell limit | constants `0x18` / `0x19` inside the blit | medium |

## Sound

`FPLAY.COM` is executed at startup and owns INT 7Fh. Bytecode opcodes `0x42`–`0x46` call INT 7Fh with AH = 0, 1, 3, or 4. `[bp+0xB9]` bit `0x04` mutes. `SONG.DAT` is opened with AX=`0x0900`, INT 7Fh (loader `0x61A0`, filename at `0x61A9`). Core code calls `sys_play_sound` / `sys_play_music` only.

## Who calls the ship

Startup (`0x88B` / `0x8C7`) and the stage reload (`0x8E6`) both load SGM1 chunk 1. The script header words are `0x78B0` (boot), `0x78C0` (reload, jumps to `0x78FF`), and `0x78C3`. Stage chunks 2–6 start at `0x78AA`, which jumps to `0x78C0`.

The stage root spawns three back-allocated children and then kills itself (`0x4F00`): script `0x5CF4`, the hitbox proxy `0x5641`, and the per-frame ship script `0x4F6E`. That script calls the movement native every frame.

| Chunk | Native entry | Opcode `0x0C` call | Confidence |
|-------|--------------|--------------------|------------|
| 1 | `0x53B1` (`inc word [si+0x34]`) | `0x4F93` (`0C B1 53`) | high |
| 2–6 | `0x53AB` (same prologue) | `0x4F8D` (`0C AB 53`) | high |

`0x53B4` is `test word [bp+0x6E], 0x20`, three bytes into the routine, not the entry. Searching for the word `0x53B4` finds nothing.

The per-frame ship script (same shape in every stage chunk, chunk 2 shown): init globals, call ship init `0x5368` (chunk 1's copy is `0x536E`), spawn the shot controller `0x62E3` with type `0x8000`, set byte `[bp+0xDF] = 0x60`, then each frame: yield, native ship move, native `0x5578` (hiscore compare, not firing), and if word `[bp+0x72]` does not have bit `0x0100`, opcode `0x50` with mask `0x00C0` and `word [target+0x3C] -= 6`.

## Collision (opcodes `0x4F` and `0x50`)

Both scan 256 entities of stride `0x52`. A candidate must have `(type & mask) != 0` and `(type & 0x0100) == 0`. The box test is five signed compares, checked in the listing at `0x426E` and `0x4314`:

- miss if `self_left >= other_right`
- miss if `self_right < other_left`
- miss if `self_top >= other_bottom`
- miss if `other_bottom < 0x20`
- miss if `self_bottom < other_top`

Touching edges are hits. Ghidra's field indices for these handlers do not match this listing; the bytes above are the ones the port uses.

| Opcode | On a hit | Confidence |
|--------|----------|------------|
| `0x4F` `0x422A` | Run the sub-op list on the target, then `dec word [attacker+0x3C]`, keep scanning. | high |
| `0x50` `0x42D1` | `dec word [attacker+0x3C]` first, run the sub-op list on that one target, store the IP past the list, stop. | high |

Two live responses:

| Site | Mask | Sub-op | What matches | Confidence |
|------|------|--------|--------------|------------|
| Ship script `0x4F9A` | `0x00C0` | target hp `-= 6` | Not a type this port spawns. Stream `0x8020` and set-piece `0x8220` have bit `0x20`, which is outside `0xC0`. | high |
| Hitbox proxy `0x5716` | `0x00E0` | target hp `-= 4` | Stream, set-piece, and the popup (`0x8010`, because bit `0x10` is inside `0xE0`). The proxy box is ±6 on both axes and its counter starts at 1, so one overlapping frame spends a life. | high |

The movement routine's own box (X ±6 or ±10, Y `-8..+10`) is not the body test. The proxy is. Its script tail after the `0x50` is what waits and respawns it; the VM runs that tail. The separation latch in `collision.c` remains only on the scaffold path used when SGM1 is missing.

Score is native `0x71DC` in the stage chunk, not a kill bonus. If the saved hp at `+0x3E` differs from `+0x3C`, add 10 to the dword at `[bp+0x8E]` / `[bp+0x90]` and play sound `0x25`. An offscreen kill sets hp to 0 and skips this call, so it scores nothing. The port collects the +10 after collision and only for enemies still in the pool.

## Ship movement (SGM1 chunk 1)

Native island at chunk offset `0x53B1`, `retf` at `0x54B4`. Stage chunks place the same bytes at `0x53AB`.

| Chunk offset | Name | Role | Confidence |
|--------------|------|------|------------|
| `0x536E` | ship init script | Sub-op block: id `+0x04 = 0x0012`, X = `0x0140` (320), Y = `0x0158` (344), Y hitbox `-8 .. +10`, then opcode `0x30` copies X/Y out to `[bp+0x96]` / `[bp+0x98]`, then return. | medium |
| `0x53B1` | `ship_update` | First instruction increments `[si+0x34]`. At `0x53B4`, button edge bit `0x20` toggles mode byte `[bp+0xD5]` and calls chunk `0x7260`. Sets X hitbox to `-10..+10` if mode is 1, else `-6..+6`. | high |
| `0x5404` | velocity add | `dir = [bp+0x6C] & 0x0F`, index `dir * 4` into a dx/dy table. Flag `[bp+0x72]` bit `0x10` selects the slow table, bit `0x20` the fast table, otherwise the normal table. Adds dx to X and dy to Y. | high |
| `0x54B8` | slow table | 16 × int16 dx, dy. Cardinals 6 and 5. Stage chunks. Chunk 1's copy is 6 bytes later. | high |
| `0x54F8` | normal table | Cardinals 8 horizontal, 6 vertical. Diagonals 6 and 5. Opposite directions cancel. | high |
| `0x5538` | fast table | Cardinals 10 and 8. | high |
| `0x547C` | clamp | X to `[0x60, 0x220]` (96..544), Y to `[0x38, 0x168]` (56..360), inclusive. Stores both into the entity and into `[bp+0x96]` / `[bp+0x98]`. | high |

A bank-angle byte at `[bp+0xD4]` eases toward the horizontal delta, clamped to `-12..11`, and picks a sprite word. The VM's native interpreter runs that. It does not change the position.

No x86 instruction in the game image adds a speed to the ship. The direction nibble indexes the table.

## Stage director (chunks 2–6)

Chunk 2 is stage 1. The stage root calls init `0x7959`, spawns the director `0x79CB` (chunk 6 uses `0x79DD`) and the ship group, then yields 17 frames (one yield plus a 16-iteration loop) before spawning the timeline. Timeline IPs: chunk 2 `0x81CF`, 3 `0x81A8`, 4 `0x7C12`, 5 `0x836F`, 6 `0x7F4B`.

Init writes `[bp+0x7A] = 0x1E00`, cell `[bp+0x7C] = 0x1E0`, speed `[bp+0x82] = 15`, cruise target `[bp+0x86] = 4`. `[bp+0x84]` is written once, to 0, so `Y += [bp+0x84]` in the set-piece loop does not move them. The director's first frame is opcode `0x4D` (`0x41B7`): 25 steps of `scroll_y += 16` (subtract `0x190` when the sum is above `0x190`) and `cell -= 1`, then a yield. The per-frame opcode `0x4B` (`0x4144`) starts on the next frame. It subtracts the speed from the sub-position, wraps by `[bp+0x7E] << 4` (`0x1E00`), and adds the speed into scroll Y, wrapping at `0x190`. A new cell calls `0x108E`, which copies one map row into the software GVRAM.

The director's tail (chunk 2 `0x8184`, same bytes in the other stages) copies the speed, subtracts the target, and holds when the difference is 0. A positive difference subtracts `0x1000` from the accumulator and SBBs the speed; a negative difference adds `0x1000` and ADCs the speed. The compare and a dummy counter add both rotate the carry byte `[bp+0xBD]`. From speed 15 and target 4 the speed is 14 on frame 1 and reaches 4 at frame 162.

On a cell change the phase byte `[bp+0xC8]` selects a rule, then an equality chain (`26 7C 00`) spawns that cell's rows. A rewind stores `(sub & 0xF) + add` and the waves see the new cell. `tools/gen_stage_tables.py` emits `src/core/stage_data.c`.

| Stage | Chunk | Timeline | Spawns | Cell waves | Popup `0x40A3` | Notes |
|-------|-------|----------|--------|------------|----------------|-------|
| 1 | 2 | 293 events, stops at a jump below `0x7000` (step 505) | 288 | 59, first cell 358 | yes, when the cell changes and cell `< 0x70` or cell `> 0x130` | Phase 0 and `0x0A` rewind at cell 416. Phase `0x14` at that cell sets target speed 2 and phase `0x1E`. The spawn at `0x8B05` is formation `0x29E1` (five children, then `0x7102`). It runs once `[bp+0xAE]` drops to 0. The only stores to `[bp+0xCB]` in this chunk are the ship script at `0x511D` (2) and `0x530C` (1). |
| 2 | 3 | 174 events | 169 | 58 | yes | Switch starts at phase `0x0A`. |
| 3 | 4 | 129 events | 122 | 10 | no | The chain walker stopped early. Treat 10 rows as incomplete. |
| 4 | 5 | 144 events | 139 | 64 | no | Several compares are unsigned `>` (body runs when the cell is `<=` the immediate). |
| 5 | 6 | 17 events, 14 spawns, jump-out at step 46 | 14 | 40 | no | The rest of the stage is the cell chain. Phase 40 in the decoded switch does not advance, and the timeline only stores phase `0x1E`, so the port's stage-finished flag does not fire here. |

After the popup test, every loop frame runs `1B` on the director's `+0x34`: spawn stream child `0x4848` only when `(counter & 0x1FF) == 0`, then add 1 to that counter (chunk 2 at `0x816F`). The first loop frame spawns one, and the next comes 512 frames later. The child is identical in chunks 2–6: type `0x8020`, `X = (rng % 0x160) + 0x90`, `Y = 0`, `vx = 0xFF80` (`rng % 1` is 0), `vy = 0x0300`, `ay = -2`, hp 20, box ±16. The popup is type `0x8010`, `X = (rng % 0x1E0) + 0x50`, hp 1, `vy = ((rng % 3) << 7) + 0x80`, box ±4 (the opener writes no box; ±4 is the stand-in used for this one object). Set-piece child `0x4DE4` is type `0x8220`, hp 96, box ±20, mover `0x6F7C`. With velocity 0 and `Y = 0` the bottom edge is 20, which is below `0x20`, so they are not hittable until something else moves them.

| Mover | Native | Kill bounds | Confidence |
|-------|--------|-------------|------------|
| stream | `0x6F2E` | X `< -16` or `>= 0x290`; Y `< 0` or `>= 0x1A0`. Then add `+0x2A` / `+0x2C` into the velocity words. | high |
| set-piece | `0x6F7C` | X `< 0x10` or `>= 0x270`; Y `< -64` or `>= 0x1D0`. | high |
| polar | `0x7066` | Rebuild `+0x2A` / `+0x2C` from sincos, add `+0x26` / `+0x28`, integrate. X `< 0x10` or `>= 0x270`; Y `< -160` or `>= 0x1A0`. | high |
| polar-wide | `0x7102` | Same step. X `< -16` or `>= 0x290`; Y `< -160` or `>= 0x1F0`. Child `0x441B`. | high |

Polar X uses sub-op `0x22` (angle `+ 0x4000`); Y uses sub-op `0x1E`. The formation then adds the spin with sub-op `0x06`. Aim sub-op `0x2A` steps the heading with `0x0F88` and writes it through the word ALU. RNG is `0xE4E` / `0xE5D`. The ROM seeds it from INT 1Ch; the port uses `0x1234`.

Opcode `0x22` applies its sub-op list to the first entity whose type matches the mask, then stops. Opcode `0x21` applies the list to every match.

A timeline gate on `[bp+0xAE]` waits while that counter is above 0. Counted children add 1 at their opener and subtract 1 on the death script (`2C AE 08 01 00`). The bytecode VM follows those bytes, including jumps below `0x7000`.

## Ported into the core

Play runs the bytecode VM in `src/core/vm.c` when SGM1 loads. The scaffold director, the placeholder shot, and `stage_finished` stay only for a missing image. HAL button bits are remapped at the VM boundary (bit 0 up, 1 left, 2 right, 3 down, 4 fire).

| Original | Core | Confidence |
|----------|------|------------|
| Frame order at `0x933`: input, entities, draw, vsync, present | `main`. The CRTC ports stay in the HAL. Opcode `0x31` expands the page into the software GVRAM. | high |
| Opcodes `0x00`–`0x50`, sub-ops through `0x2A`, ALU, compare, RNG, sincos, atan, aim | `vm_frame` | high for the dispatch, medium where a native island uses an unhandled 80186 opcode |
| Ship script `0x4F6E`, shot controller `0x62E3` and children `0x6331` / `0x6498` | the VM. Fire is held bit `0x10`. | high |
| Native ship move `0x53AB` (chunk 1 `0x53B1`), tables `0x54B8` / `0x54F8` / `0x5538`, clamp | interpreted from the chunk | high |
| Option followers. Stage-chunk entry `0x59CB` does `OR word [si], 0x200` and `X = ship X - 16` at `0x59FF`. The twin at `0x5F7B` uses `X + 16`. Chunk 1's `OR` is the byte at `0x5A05`. | interpreted. Chunk 1 at `0x9507` arms `[bp+0xD5]` to 1 before a stage load; a direct stage entry keeps that. `gfx_draw_entities` draws type bit `0x200`. Opcode `0x4E` skips that bit. | high |
| Proxy `0x5641`, opcode `0x50` mask `0x00E0`, damage 4, and the script tail after that hit | the VM. `rb_hit_test` is the five compares. | high for the test. The tail runs when `[bp+0xD5]` is not 0. |
| Enemy shots whose type matches mask `0x00C0`, ship damage 6 | the ship script's opcode `0x50` | high |
| Score +10 on an hp change (`0x71DC`) | the native, inside the VM | high |
| Scroll `0x4B` / prelude `0x4D`, the bytecode speed regulator | the VM. Speed is 15 on the prelude frame, 14 on the next, and 4 sixteen frames at a time later. | high |
| Stage 3 cell chain and stage 5 past the old jump-out | the real scripts. A stage-1 run changes cell hundreds of times; stage 5 spawns well past the old 14-spawn cutoff. | high |
| End of stage | opcode `0x0D` at chunk offset `0x78B3` loads SGM1 record 1, header word 2 (chunk 1 IP `0x78C3`, `JMP 0x8E70`). `stage_finished` is not consulted. | high for the transfer. |
| Nested formation `0x29E1` | five `0x16` spawns (`0x353A`, `0x36E0`, `0x31B2`, `0x3376`, `0x2FA0`) then native `0x2DC8` / `0x7102`. Stage 5 runs the five children. On stage 1 the spawn is chunk 2 `0x8B05`, after the `[bp+0xAE]` wait at `0x8AF4`. Holding fire reaches it at frame 1260, and the five children the same frame, while the chunk is still 2. The clear load is frame 1489. An idle ship never gets there: the second death takes `0x5047`, stores `[bp+0xCB] = 2` at `0x511D`, then `1` at `0x530C`, and the root's `CB == 1` test loads record 1 header 2 at frame 904. `[bp+0x62]` stays 3. Top-left does the same spawn at frame 1392. | high |
| 256-slot dispatcher | the VM's forward scan | high |
| Opcode `0x32` binds an SGM1 record to a `[bp+slot]` bank and does not switch the script. Stage 1 binds sprite records 12, 13, 16, 17, 18, 19, tiles record 25 at `[bp+0x26]`, and the map record 31 at `[bp+0x16]`. | `gfx_bind` | high |
| Fill `0x3C` / `0x3D` (GRCG color at loader `0x77D4`) | `gfx_fill`. Stage 1's root fills 80×400 with color 0. | high |
| Palette `0x3F` (absolute, `0x6BD1`), `0x40` (deltas, `0x6CAB`), `0x41` (step, `0x6D00`) | `gfx_palette_*`. `0x41` is not a music call. Levels are stored `<< 4` and expanded with `v * 17`. | high |
| Scroll row `0x108E`, compose `0x4C` (`0x10D2`), entity draw `0x4E` (`0x1132`) | `gfx_copy_row`, `gfx_compose`, `gfx_draw_entities`. Tiles are 96 bytes, planes blue/red/green, intensity left 0. Sprites are 128 bytes: mask, blue, red, green. Bit 7 is the left pixel. The playfield is 30×16 pixels inset by 80. Present reads line `(y + [bp+0x76]) mod 400`. | high for the stage-1 path. |
| Text `0x47` (`0x40D0`), digits `0x48` / `0x49` / `0x4A` | `draw_text` at `0x12C5` and the digit writer at `0x1554`. A zero digit is two spaces of word `0x2020` until `BL` is set; `0x4A` sets `BL` only before the last of eight digits. Gaiji `0x76xx` come from record 11 (32 bytes each, registered like INT 18h AH=1Ah at `0x1592`). Opcode `0x33` is that registration. Direct stage entry installs glyphs 1..63 as codes `0x7621`..`0x765F`, the same range chunk 1's `33 24 01 3F 21 76` would leave behind. Stage 1 writes the HUD at `0x76A9`, the green title at `0x761D`, hiscore at text `0x010C`, score at `0x00D8`. The SDL `SCORE` / `STAGE` strings are not drawn once GVRAM is active. | high |
| Page flip `0x31` (`0x3D1A`) | `op_page`. AL=3 on the loader call, so `[bp+0x30]` (`0x6AA4`) runs four times through `0x6ACD` into the four planes. A plane ends at the next `00`. The software buffer keeps the first 32000 bytes and still consumes that terminator, so the following stream is not read as an empty plane. | high |
| Blits `0x34`–`0x3B` | Tile grid `0x6D40`, shifted tile `0x6DCD`, sprite grid `0x6D89`, and the `MOVSW` copy at `0x7428`. Stage 1's playfield is still `0x4C` / `0x4E`. The GRCG masked path at `0x7541` (AL nonzero, or both `0x10` bits in BX) is the copy. | medium for the masked path, high for the copy |
| Ranking `0x9128` / `0x9156` / `0x91AF` / `0x9213` / `0x923A` | PUSH/POP ES and DS, `LOOP`, `MOV` to a segment, and `REP MOVSW`. `[bp+0x56]` points at a private 256-byte block (`0x400`), not at the globals. Text VRAM is a side buffer. | high for the natives. The name-entry loop at `0x8F60` waits for an edge on bits `0x30` (fire). Holding fire does not leave it. |

Records after chunk 6 use the same LZ77, one stream per call to `0x6ACD`. A single stream stops at the first `00`. Record 10 is 12983 bytes and that first byte is `00`, so one stream returns 0 bytes. That is not a short file and not a 2-byte header. Opcode `0x31` runs four streams in a row. Record 10 is then an empty blue plane, an empty red plane, a 32000-byte green plane (9164 nonzero), and a 32000-byte intensity plane (4518 nonzero), and the compressed bytes are all consumed. Records 7 and 8 are four full 32000-byte planes. Record 9 is two full planes and then two empty ones. Record 11 is one stream of 8192 bytes, the 256×32 gaiji sheet. Records 12–23 are 32768 (256×128 sprites), 24–29 are 36864 (384×96 tiles), 31 is 28800 (480×30 map words). Opcode `0x32` still uses one stream, so binding record 10 that way stays empty. The stage-1 camera starts on the last rows, which are sparse stars, and the dense tiles sit further up the map.

## Next

1. The continue jump at chunk 1 `0x9AD1` (`JMP 0x78C6`) does run after the ranking natives, once fire is tapped through the name entry. That loader picks record 2 when `[bp+0xC5]` is 1, and records 3–6 when the byte is 2–5. Stage 1 stores 1 in both `[bp+0xC5]` and `[bp+0xC7]`, and `0x8DE4` copies `C7` back into `C5`, so a stage-1 clear reloads record 2. Chunk 2 has no other store to `[bp+0xCB]` than the ship script, so the clear screen in a normal run is that death path, not a finished stage.
2. INT 7Fh command map inside `FPLAY.COM`, then real `sys_play_music` track ids.
3. Loader `0x6C0B` and `0x5EF5`, the two function pointers the frame loop calls beside vsync and the page flip.
4. The GRCG masked blit at `0x7541`. Opcodes `0x3A` / `0x3B` take the `MOVSW` copy.
5. x86 parity (`JP` / `JNP` stay false), `F2` REPNZ (skipped), and `STD` (DF stays clear).
6. Physical keycap names. The HAL bits are remapped, not renamed.
