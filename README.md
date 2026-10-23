# Rude Breaker — clean-room C99 port

Playable SDL2 port of the PC-98 title **Rude Breaker** (Compile / SgcPr, 1995).
The original disk and the Yamaha YM2608 rhythm ROM are copyrighted and are
**not** in this repository or in the downloadable builds. You supply both
from your own copy.

| Phase | Target | HAL backend |
|-------|--------|-------------|
| **1** | Modern PC (SDL2) | `src/hal/sdl2/` |
| **2** | Sharp X68000 (Human68k) | `src/hal/x68k/` |

Core game logic (`src/core/`) is shared. Structs are
`__attribute__((packed, aligned(2)))`. Binary assets are read through
little-endian helpers in `include/hal.h`. Music is Compile's FPLAY 2.11a
driving Aaron Giles' ymfm YM2608 (BSD-3-Clause, `third_party/ymfm/LICENSE`).

## Play

Download a build (GitHub Actions artifacts on each push, or a GitHub
Release for tags `v*`). Unzip it. Copy files from **your** Rude Breaker
disk into the same folder as the program, then start it.

### Files that go next to the executable

| File | Required | What this port does with it |
|------|----------|------------------------------|
| `SGM1.DAT` | yes | Stage, sprite, and script bank. The game will not start without it. |
| `FPLAY.COM` | yes | FM/SSG driver. Music stays silent without it. |
| `SONG.DAT` | yes | Song data for that driver. |
| `RBRANK.DAT` | no | Ranking table and saved difficulty. Loaded at boot, written when a score is entered, and created with the original table if it is missing. |
| `ym2608_adpcm_rom.bin` | no | Yamaha rhythm ROM. 8192 bytes, CRC32 `23c9e0d8`. See below. |

These names on the disk are **not** read. Leave them out of the folder:

- `SGC.COM` — original packed launcher
- `READJS.COM` — joystick helper the loader used
- `RUDEBR.BAT` — DOS batch file that started `SGC.COM`

Search order for the data folder:

1. A directory argument: `./rudebreaker /path/to/your/RUDEBR`
2. `RUDEBR_ASSETS`
3. The executable's own directory, if `SGM1.DAT` is there
4. `<directory of the executable>/RUDEBR`, if `SGM1.DAT` is there
5. `assets/raw/RUDEBR` when you are running from a source checkout

`--help` prints the same file list.

### Controls

The movement keys match the PC-98 clusters. WASD is an extra set.

| Action | Keyboard | Gamepad |
|--------|----------|---------|
| Move | Arrow keys, keypad 8 / 4 / 6 / 2, or W A S D | Left stick or d-pad |
| Fire, and the title trigger | Z, Space, or keypad 0 | South (A) |
| Mode | X, N, M, Enter, or keypad Enter | East (B) |
| Special | Left Shift or Right Shift | West (X) |
| Escape | Escape | Start |
| Quit | F10 | Back |
| Original number keys | 1, 2, 3, 4 | — |

The original logo, title, and menus run from the disk script. Z or Space is the trigger those screens read. Escape is the PC-98 escape bit. F10 quits.

### Rhythm ROM

The six internal percussion samples are Yamaha copyright. Do not commit
`ym2608_adpcm_rom.bin` or a zip of it. This port checks that the file is
exactly 8192 bytes with CRC32 `23c9e0d8` and feeds a match to ymfm's
ADPCM-A read. A missing or rejected file leaves those reads at zero.

Search order:

1. `--ym2608-rom /path/to/ym2608_adpcm_rom.bin` (replaces the search)
2. `RUDEBR_YM2608_ROM` (also replaces the search)
3. `ym2608_adpcm_rom.bin` beside the executable
4. `ym2608_adpcm_rom.bin` in the data directory

This disk's `SONG.DAT` does not program the rhythm channels. The ROM is
still loaded when you supply it, for anything that keys ADPCM-A directly.

### Which build

| Archive | Machine | What you need installed |
|---------|---------|-------------------------|
| `rudebreaker-linux-x86_64.zip` | 64-bit Linux | glibc, plus `libX11.so.6` and `libasound.so.2`. SDL2 is compiled into the binary and loads those two at runtime. PulseAudio and Wayland are used only when those libraries are already installed. |
| `rudebreaker-windows-x86_64.zip` | 64-bit Windows | Nothing else. `SDL2.dll` is in the zip. |
| `rudebreaker-macos-*.zip` | macOS, from GitHub Actions | The SDL2 library shipped next to the program. |

The Linux binary loads `libX11.so.6` and `libasound.so.2` when it starts.
A normal desktop install already has them. The zip contains the program,
this README, and the ymfm license. It does not contain `SGM1.DAT`,
`FPLAY.COM`, `SONG.DAT`, `RBRANK.DAT`, or the Yamaha ROM.

## Source disk (not in the repo)

PC-98 2HD floppy image (`Rude Breaker.hdm`, 1 261 568 bytes):

```
RUDEBR/
  SGC.COM      main executable (SgcPr com1.00, 1995-12-08) — not read
  FPLAY.COM    FM/SSG music driver — required beside the port
  READJS.COM   joystick helper — not read
  SGM1.DAT     packed asset bank (37 chunks) — required
  SONG.DAT     BGM data for FPLAY — required
  RBRANK.DAT   ranking table and saved difficulty — loaded and written beside the program
  RUDEBR.BAT   launches SGC — not read
```

Extract locally if you are building from a checkout (do not commit dumps):

```bash
python3 tools/extract_pc98_hdm.py /path/to/Rude\ Breaker.hdm assets/raw
python3 tools/parse_sgm1.py assets/raw/RUDEBR/SGM1.DAT assets/sgm1
```

## Build from source

SDL2 development files once, then:

```bash
sudo apt-get install -y libsdl2-dev
make
./build/rudebreaker
```

`tools/package_dist.sh` builds the zip that players download. `linux`
compiles a static SDL2 (X11 and ALSA only). `windows` cross-compiles with
MinGW and copies `SDL2.dll`. `macos` is meant to run on a Mac with
Homebrew SDL2; this repository's GitHub Actions runner does that.

```bash
bash tools/package_dist.sh linux
bash tools/package_dist.sh windows
```

Phase 2 (X68000 / Human68k) needs that cross compiler:

```bash
make PLATFORM=x68k
```

Native video mode target: 640×480 @ 31 kHz with a 640×400 frame buffer
centered (40 px vertical margins).

## Continuous builds

`.github/workflows/release.yml` builds Linux x86_64, Windows x86_64, and
macOS on every pull request and on tags named `v*`. Each job uploads a
zip as an artifact. A tag also publishes those zips on a GitHub Release
(`contents: write`). The workflow refuses to upload a zip that contains
the disk files or the rhythm ROM.

## Layout

```
include/hal.h           HAL API + endian helpers + packed entity structs
src/core/               platform-agnostic game logic
src/hal/sdl2/           Phase 1 backends, including the ymfm wrapper
src/hal/x68k/           Phase 2 backends
third_party/ymfm/       BSD-3-Clause YM2608 core
tools/                  disk extractors and the release packager
docs/ARCHITECTURE.md    phase rules and RE notes
```

## Developer smoke switches

These environment variables are for the headless smoke test. They are not
controls, and the packaged README section above is the player list.

| Variable | Effect |
|----------|--------|
| `RUDEBR_FAST=1` | Skip the PC-98 wait (about 30 logic frames per second). |
| `RUDEBR_TIMING=1` | Log logic frames per second. Not a player control. |
| `RUDEBR_TRACE=1` | Log state, ship, chunk, and score. |
| `RUDEBR_HOLD=A,RIGHT` | Hold those buttons. Tokens: `A` `B` `C` `LEFT` `RIGHT` `UP` `DOWN` `START` `QUIT` `1`–`4`. |
| `RUDEBR_SUSTAIN=1` | Keep the ship above the death hit so a stage can finish. The VM frame function does not call this. |
| `RUDEBR_STOP=title\|stage1\|clear\|over` | Exit when that point is reached. Exit status 3 means it was not. |
| `RUDEBR_FRAMES=N` | Exit after N frames. |

```bash
SDL_VIDEODRIVER=dummy SDL_AUDIODRIVER=dummy \
  RUDEBR_FAST=1 RUDEBR_TRACE=1 RUDEBR_HOLD=A,RIGHT RUDEBR_STOP=stage1 \
  ./build/rudebreaker
```
