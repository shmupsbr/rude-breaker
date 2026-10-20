# Architecture

## Phase 1 — PC-98 → C99 + SDL2

Goal: reconstruct a clean, playable C99 version (640×400 native, 60 FPS)
strictly decoupled from x86 / PC-98 hardware.

Rules:

1. No x86 registers, segment arithmetic, BIOS interrupts, or VRAM addresses
   (`0xA8000` / `0xE0000`, etc.) in core or SDL2 code.
2. Entities live in packed, 2-byte-aligned structs (`RB_PACKED` in `hal.h`).
3. All raw asset reads use `read_u16_le` / `read_u32_le` / `SWAP16` / `SWAP32`.
4. Hardware goes through `hal.h` only (`sys_draw_sprite`, `sys_play_sound`,
   `sys_read_input`, …).
5. SDL2 lives exclusively in `video_sdl2.c`, `audio_sdl2.c`, `input_sdl2.c`.
6. Comments in source are English only.

## Phase 2 — SDL2 → Sharp X68000

Goal: same core, native Human68k backends.

Rules:

1. Do **not** modify `src/core/*`.
2. Replace only HAL backends with `video_x68k.c`, `audio_x68k.c`,
   `input_x68k.c`.
3. Big-endian host: still use the LE readers in `hal.h` for PC-98 assets.
4. Maintain 2-byte alignment; never touch 16/32-bit values at odd addresses.
5. Sync the main loop to 60 Hz VBlank (`_iocs_vdisp_wait` / CRTC VDisp).
6. Video: 640×480 @ 31 kHz, 640×400 playfield centered with 40 px margins.
7. Audio: YM2151 (OPM) + MSM6258 ADPCM via documented IOCS only.
8. Comments in source are English only.

## Disk / asset RE notes

### Floppy

`Rude Breaker.hdm` is a PC-98 2HD image (1024 B/sector, 8 spt, 2 heads,
77 tracks → 1 261 568 bytes). FAT12 BPB: OEM `NEC 2.00`.

### SGM1.DAT TOC

Bytes `0x00`–`0x97`: up to 38 entries of `{ uint16_t bank_le; uint16_t off_le }`
(`RB_PACKED`). Absolute file offset = `bank * 0x10000 + off`. Entry 37
(`bank=8, off=0x911E`) equals EOF and acts as sentinel → **37 data chunks**.

Many later chunks begin with a repeating header pattern
`01 00 fc 00 01 ff 9c 00` (compressed / planar graphic candidate; format TBD).

### Executables

| File | Role |
|------|------|
| `SGC.COM` | Game program (`SgcPr`, needs ~460 KiB conventional) |
| `FPLAY.COM` | Music driver (FM / SSG / soft envelope labels) |
| `READJS.COM` | Joystick reader (Tatsuki.K 1.05a) |
| `SONG.DAT` | Sequence data consumed by FPLAY |
| `RBRANK.DAT` | Ranking table (230 bytes) |

Feed Ghidra / IDA pseudo-C for `SGC.COM` into `src/core/` modules
(`game.c`, `player.c`, `enemy.c`, `collision.c`) without carrying over
register-level idioms.
