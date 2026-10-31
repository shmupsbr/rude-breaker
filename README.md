# Rude Breaker 30th AE — PC port (engine only)

🇬🇧 [English](README.md) · 🇧🇷 [Português](README.pt-BR.md) · 🇯🇵 [日本語](README.ja.md)

---

## English

**Rude Breaker 30th AE** (30th Anniversary Edition) celebrates 30 years since the game's release on April 5, 1996. It is an unofficial PC (Windows and Linux) port of **Rude Breaker**, the vertical shooter by Compile released on *Disc Station Vol. 10* (PC-98, 1996). The original x86 game code runs inside an interpreter with a modern SDL2 host: vsync, gamepad support, remappable keys, a sound test with a VU meter, Training Mode, a saved hi-score table, an in-game SPEED button with a speed readout, an optional Zanac-style RANK system and more. The music is the original driver playing on an emulated YM2203, the PC-98 FM sound chip. Manuals: [English](docs/MANUAL.md) · [Português](docs/MANUAL.pt-BR.md) · [日本語](docs/MANUAL.ja.md) (section 2 explains how to set up the game files).

### A personal note
This program exists to build changes on top of a game I am a huge fan of, made by a company that taught me to love video games. I never intend to harm its current rights holders, whoever they are. If they want this taken down, they only need to reach me by email (shmupsbr@gmail.com) or on X/Twitter ([@shmupsbr](https://x.com/shmupsbr)), and I will take it down if that is the case. The rights holders are fully free to reuse the code in this repository, in part or in full, without giving me any credit (but if they do, I will be very happy!).

— Filipe Gracioli (ShmupsBR)

### New in 30th AE
- **SPEED button** (C / Ctrl, pad Y): changes the ship's speed during the game, cycling LOW / MID / MAX / MID / LOW. Each game starts at MID by default, or at the MOVE SPEED chosen in the menu. The top bar shows it as `SPD LOW`, `SPD MID` or `SPD MAX`.
- **RANK** (GAMEPLAY menu, ON by default): a Zanac-style rank. It rises while you survive and score, halves when you lose a life, and speeds up enemy bullets (never slower than the chosen difficulty). Bonuses still follow the chosen difficulty. On VERY EASY and EASY rank only goes half way up; on HARD and VERY HARD it starts half way up. OFF plays exactly like the original.
- **LOOP** (GAMEPLAY menu, OFF by default): with LOOP ON, clearing stage 5 starts stage 1 again as loop 2, 3 and so on, keeping score, lives and weapons. Each loop raises the base difficulty one level (up to VERY HARD), and every rank-driven value is multiplied by the loop number (capped at x8; enemy bullet speed never passes VERY HARD; the speed-up at the ceiling never passes +50%). Losing a life goes back to the loop's base difficulty. From loop 2 on, the RANK box title shows LOOP 2, LOOP 3... OFF keeps the original ending. Details in section 6 of the manual.
- The GAMEPLAY menu lists its options on consecutive lines, with no blank lines between them.
- **MODERN palette** is now the default.
- **Weapon change during the game** (SHIP SELECT OFF) is now the default: pick up V, L or M capsules to switch weapon.
- **Escape** pauses only during a game.
- The final boss's homing shot now points its trail in the direction it flies, as in the original.
- A short 30th AE card ("Rude Breaker - 30th Anniversary Edition by ShmupsBR 2026") before the Compile logo; trigger or Start skips it.
- New title screen with the 30th AE mark and the © ShmupsBR 2026 credit, and a 30th AE credit line in the ending. The ranking screen keeps its original look.

### Why the game files are not included
This repository contains **only the engine** (the code written for this port). It does **not** include any of the original game's files, and never will:

- **Rude Breaker is not free software or shareware.** It was a commercial release by Compile, and its rights are still active: D4 Enterprise re-released it on **Project EGG** in 2014.
- The game's graphics, music, sound, stage data and program (`SGM1.DAT`, `SONG.DAT`, `FPLAY.COM`) belong to their rights holders. Redistributing them, even for free, is not allowed.

You must supply these files from **your own legal copy** of the game.

### Building the play package
1. Get the game legally, for example from [Project EGG](https://www.amusement-center.com/), or from your own *Disc Station Vol. 10* disc.
2. Extract these files from the Rude Breaker disk: `SGM1.DAT`, `SONG.DAT` and `FPLAY.COM`.
3. Download `releases/RudeBreaker-PC-pr9p.zip` from this repository (or build `rudebreaker.exe` yourself), unzip it, and add the game files so the folder looks like this:
   ```
   Rude Breaker PC/
     rudebreaker.exe
     SDL2.dll
     SGM1.DAT
     SONG.DAT
     FPLAY.COM
   ```
4. Run `rudebreaker.exe`.

Instead of loose files you can also drop a PC-98 disk image of the game next to the program (`.hdm`, `.fdi`, `.d88`, `.hdi`, `.nhd`, or a `.zip` of one); the files are read straight from it. See section 2 of the manual.

These three files are all you need. No other file (BIOS, sound ROM or the like) is required: the music uses YM2203 emulation, which needs no ROM.

### Linux
A Linux x86_64 build is in `releases/RudeBreaker-Linux-pr9p.zip`. It ships its own `libSDL2-2.0.so.0` and runs on most distributions with glibc 2.17 or newer (X11 or Wayland; PulseAudio, PipeWire or ALSA).

1. Unzip it and add the same game files, so the folder looks like this:
   ```
   Rude Breaker Linux/
     rudebreaker
     libSDL2-2.0.so.0
     SGM1.DAT
     SONG.DAT
     FPLAY.COM
   ```
   The file names must match exactly, including upper case (`SGM1.DAT`, not `sgm1.dat`).
2. In a terminal, inside that folder:
   ```
   chmod +x rudebreaker
   ./rudebreaker
   ```

To build it yourself, install a C/C++ compiler, `make`, `pkg-config` and the SDL2 development package (Debian/Ubuntu: `sudo apt install build-essential pkg-config libsdl2-dev`; Fedora: `sudo dnf install gcc-c++ make SDL2-devel`; Arch: `sudo pacman -S base-devel sdl2`), then run:
```
make linux
```
The binary is `build/rudebreaker`. `make linux` looks for `libSDL2-2.0.so.0` next to the binary first and falls back to the system library, and links the C++ runtime statically.

### Distribution notice
Do **not** distribute the game files. You may only use them if you own the game legally: the original *Disc Station Vol. 10* disc, or a purchase or subscription through Project EGG. If you do not own the game, you may play it for no more than **24 hours, for testing purposes only**, and must then delete the game files.

### Legal
This is a non-commercial fan project with no affiliation to Compile or D4 Enterprise. Third-party licenses: SDL2 (zlib) and ymfm (BSD-3-Clause). Side-panel font: Robotica by Courtney Novits (free font).
