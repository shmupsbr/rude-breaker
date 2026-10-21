# Rude Breaker — clean-room C99 port

Two-phase port of the PC-98 title **Rude Breaker** (Compile / SgcPr, 1995)
to a modular C99 codebase.

| Phase | Target | HAL backend |
|-------|--------|-------------|
| **1** | Modern PC (SDL2) | `src/hal/sdl2/` |
| **2** | Sharp X68000 (Human68k) | `src/hal/x68k/` |

Core game logic (`src/core/`) is shared. Only the HAL files change between
phases. Structs are `__attribute__((packed, aligned(2)))` for future M68000
builds. Binary assets are read through Little-Endian helpers in `hal.h`.

## Source disk (not in repo)

PC-98 2HD floppy image (`Rude Breaker.hdm`, 1 261 568 bytes):

```
RUDEBR/
  SGC.COM      main executable (SgcPr com1.00, 1995-12-08)
  FPLAY.COM    FM/SSG music driver 2.0A
  READJS.COM   joystick helper
  SGM1.DAT     packed asset bank (37 chunks, bank:offset TOC)
  SONG.DAT     BGM data for FPLAY
  RBRANK.DAT   ranking / high-score table
  RUDEBR.BAT   launches SGC
```

Extract locally (image is copyrighted; do not commit dumps):

```bash
python3 tools/extract_pc98_hdm.py /path/to/Rude\ Breaker.hdm assets/raw
python3 tools/parse_sgm1.py assets/raw/RUDEBR/SGM1.DAT assets/sgm1
```

## Build — Phase 1 (SDL2)

```bash
sudo apt-get install -y libsdl2-dev   # once
make -C rudebreaker
./rudebreaker/build/rudebreaker
```

Optional asset path (defaults to `assets/raw/RUDEBR` if present):

```bash
RUDEBR_ASSETS=./assets/raw/RUDEBR ./build/rudebreaker
```

## YM2608 rhythm ROM (not in the repo)

The six internal percussion samples are Yamaha copyright. Do not commit
`ym2608_adpcm_rom.bin`, a zip of it, or any dump derived from it. The
SDL2 build looks for that file at runtime and checks that it is exactly
8192 bytes with CRC32 `23c9e0d8`. A match is fed to ymfm's ADPCM-A
external read. Anything else, including a missing file, leaves those
reads at zero, which is the same silent rhythm as before.

Search order:

1. `--ym2608-rom /path/to/ym2608_adpcm_rom.bin` (this replaces the search)
2. `RUDEBR_YM2608_ROM` (this also replaces the search)
3. `ym2608_adpcm_rom.bin` in the same directory as the executable
4. `ym2608_adpcm_rom.bin` in the asset directory

```bash
./build/rudebreaker --ym2608-rom "$HOME/ym2608_adpcm_rom.bin" assets/raw/RUDEBR
RUDEBR_YM2608_ROM="$HOME/ym2608_adpcm_rom.bin" ./build/rudebreaker
```

## Build — Phase 2 (X68000 / Human68k)

Requires the Human68k GCC cross toolchain. Replace the SDL2 objects with the
X68k HAL (core objects stay identical):

```bash
make -C rudebreaker PLATFORM=x68k
```

Native video mode target: 640×480 @ 31 kHz with a 640×400 frame buffer
centered (40 px vertical margins). Frame sync via `_iocs_vdisp_wait`.

## Layout

```
include/hal.h           HAL API + endian helpers + packed entity structs
src/core/               platform-agnostic game logic
src/hal/sdl2/           Phase 1 backends
src/hal/x68k/           Phase 2 backends
tools/                  disk / SGM1 extractors
docs/ARCHITECTURE.md    phase rules and RE notes
```

## Status

Phase 1 scaffold runs at 640×400 / 60 FPS with HAL-backed input, video, and
audio stubs. Full gameplay requires ongoing reverse engineering of `SGC.COM`
and the `SGM1.DAT` chunk formats (Ghidra dumps feed into `src/core/`).
