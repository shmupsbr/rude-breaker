# X68000 port feasibility (research)

This is a measurement of the PC-98 game, not a port. The PC build and its pull request are unchanged. The target machine is the original 10 MHz 68000 X68000 with a RAM expansion (4 MB or more). The XVI (16 MHz) and the X68030 (25 MHz) are not targets. Graphics, sliced sprite patterns, and converted audio are preloaded into that RAM. Nothing is redrawn; cells are cut from the original patterns by a tool.

Numbers below come from one headless run of the current bytecode VM: `vm_reset_play` on stage 1, fire held, ship kept alive, through the ending flag at frame 25925. That is the same stage march the VM checks already record (stage 2 at 5504, stage 3 at 11792, stage 4 at 16313, stage 5 at 22452, ending at 25925). It is the dense case: both option pods and continuous fire, so player shots and explosions are high. A human who taps fire will be under these peaks. The machine still has to survive this case or drop sprites on purpose.

## Verdict

Feasible on the original 10 MHz X68000 with a RAM expansion, if three limits are respected.

1. The 8086 islands inside the scripts (ship, shots, options, score, movers) are rewritten as 68000 C. Leaving them as an 8086 interpreter does not fit in a 30.667 Hz frame.
2. The director can stay a bytecode interpreter. A tight jump-table plus those rewritten natives fits the measured frames, including the heaviest one, with a margin that disappears if the interpreter is a naive C switch full of calls.
3. Sprites are hardware PCG with a hard cap of 128 on screen and 32 on a line. The measured peaks are 262 to 371 cells and 47 to 54 on one line. The overflow is mostly explosion debris (`0x428D`). Cull that debris. Do not software-blit it onto the graphics plane; a full debris blit costs more than the frame.

The scrolling map is the graphics plane with hardware scroll. The HUD is the text plane. The two BG tile planes are left unused, because they share the 256 PCG patterns with the sprites.

Biggest risks, in order: the sprite cap on debris and bullet-heavy frames, the 10 MHz margin if the interpreter is sloppy, and SSG channels that the YM2151 does not have. Display timing and RAM are not the risks once the machine is the original 10 MHz board with 4 MB.

## What was counted

Each drawn object is already stored as 16×16 cells (mask plane plus three color planes, 128 bytes). A cell counts as one hardware sprite when its mask hits the stage window, x 80..559 and y 0..399. Fully transparent cells and cells entirely off that window are skipped. The PC-98 shift of 0..7 pixels is a byte-alignment pad; the X68000 sprite is placed at that pixel, so a shifted cell is still one sprite, not two.

A hardware sprite occupies every scanline of its 16-line box that lies on the 400-line playfield, including transparent rows inside the box. The background map is not in this count.

| Layer | What it is in this game | Hardware |
| --- | --- | --- |
| Graphics plane | 30×25 tiles of 16×16 (the 480×400 map), scrolled | CRTC scroll of the 1024-tall page. Not sprites. |
| Text plane | HUD glyph row, and menus | Text VRAM. Not sprites. |
| Sprites | Ship, options, shots, enemies, bosses, debris, items | PCG. The tables below. |
| BG tile planes | Nothing in the original | Leave unused. They share the 256 patterns. |

## 1. Sprite load

Limits on every X68000, including the original: 128 sprites on screen, 32 on one scanline, each 16×16 and 16 colors.

| Stage | Frames | Sprites p50 | p95 | Peak | Peak frame in stage | Scanline p50 | p95 | Peak line |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | 5503 | 119 | 205 | 314 | 5193 | 15 | 25 | 51 |
| 2 | 6288 | 133 | 204 | 289 | 5956 | 15 | 28 | 49 |
| 3 | 4521 | 111 | 204 | 262 | 4193 | 15 | 29 | 54 |
| 4 | 6139 | 133 | 248 | 371 | 5711 | 17 | 32 | 50 |
| 5 | 3473 | 139 | 218 | 336 | 871 | 16 | 27 | 47 |

Median frames on stages 2, 4, and 5 are already over 128. Median lines are 15 to 17, under 32. The line limit breaks on the peaks (47 to 54), and stage 4's 95th percentile sits on 32.

What the peak frames are made of. Counts are 16×16 cells of one script origin. Origins under 4 cells are omitted.

| Stage | Debris `0x428D` | Option shots `0x6557` | Main shot `0x6331` | Ship `0x4F6E` | Boss / large | Other large groups |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | 120 | 39 | 6 | 6 | `0xA16B` 42 (stage 1 core) | bullets `0x4745` 16, `0x41D2` 14, `0x4355` 12, `0x47C4` 11 |
| 2 | 92 | 39 | 8 | 6 | `0x9CD8` 10, `0x9DB0` 18, `0x9E70` 18 | bullets `0x4355` 23, `0x4745` 18, `0x41D2` 16 |
| 3 | 124 | 44 | 12 | 6 | (spread across smaller origins) | `0x4745` 18, `0x47C4` 15, `0x41D2` 12 |
| 4 | 56 | 40 | 12 | 6 | `0x9B5B` 10 plus parts `0x9FFF` 16, `0xA096` 16, `0xA12D` 18, `0xA29A` 18, `0xA526` 26 | formation `0x1ED0` 36, `0x20DC` 16 |
| 5 | 160 | 38 | 18 | 6 | `0x895E` 14 | bullets `0x41D2` 48, `0x4355` 18 |

Debris alone exceeds 128 on stage 5 (160) and is the single largest group on stages 1, 3, and 5. Bosses are large but secondary: the stage 1 core is 42 cells, not hundreds. Player option shots are a steady ~40 cells because fire is held and both pods are live.

Renderer plan at 10 MHz:

- Ship, bosses, enemies, and bullets take the hardware sprites, in that priority.
- Past 32 on a line, or 128 on the frame, drop debris first, then surplus player shots.
- A raster interrupt can reuse sprite slots in two Y bands (rewriting 64 sprite records is a few thousand cycles). That raises the frame cap toward 256. It does not raise the 32-per-line cap. The measured lines of 47 to 54 still need a cull.
- Software-stamping the overflow into the graphics plane is the wrong fix. 160 masked 16×16 writes are on the order of a whole 10 MHz frame. The 2 second stage gap is the place to upload patterns, not the place to catch up a blit.

## 2. Pattern memory

PCG RAM is 32 KB. A 16×16 4-bit pattern is 128 bytes, so the RAM holds 256 patterns. The sprite controller can flip horizontally and vertically, and the counts below already treat a flip as the same pattern. The IPSJ museum table says "128 patterns"; 32 KB divided by 128 bytes is 256, which is also Sharp's sprite-RAM size and the figure used here.

| Stage | Unique cells in the whole stage | Unique cells on the busiest frame (p50 / p95 / max) |
| --- | --- | --- |
| 1 | 590 | 40 / 70 / 143 |
| 2 | 665 | 41 / 79 / 136 |
| 3 | 568 | 32 / 72 / 136 |
| 4 | 654 | 45 / 90 / 177 |
| 5 | 409 | 47 / 74 / 149 |

No stage fits in 256 patterns if every cell it will ever show is resident. Every single frame does fit: the worst frame is 177, under 256.

Preload all of those cells into main RAM at boot (a few hundred kilobytes even if the five stages share nothing: 590+665+568+654+409 = 2886 patterns, about 370 KB). PCG itself is a 256-pattern cache.

- During the stage-load pause, copy that stage's first working set in. Worst case 177×128 ≈ 23 KB.
- During play, upload a pattern only on a cache miss. A typical frame's whole set is ~40 patterns, about 5 KB. Replacing the entire worst-frame set every frame would be ~23 KB, on the order of 140,000 cycles (about half of the 10 MHz logic frame) and is not the plan.
- No per-scanline streaming. The 31.5 kHz line is about 320 cycles at 10 MHz, which is an interrupt entry and not a pattern copy.

Bandwidth that is acceptable: a handful of 128-byte uploads on the frames where a new enemy or explosion pattern appears, plus one burst during the existing 2 second gap between stages.

## 3. Palettes

The PC-98 picture is one 16-color analog palette, shared by the map and the sprites. The sprite attribute's low bit picks the bright half of that same 16 (index | 8). Attribute 2 is a mono cell in color 11. Fades rewrite those 16 entries. They do not allocate a second 16-color set.

The X68000 has 16 sprite/BG palette blocks of 16 colors, and a separate graphics palette. Block 0 is shared with the text palette.

| Use | Banks needed | Banks present |
| --- | --- | --- |
| Sprites | 1 | 16 |
| Graphics plane (the map) | 1, in the graphics palette, same 16 colors | 16 or 256 entries depending on mode |
| HUD text | the text palette, or sprite block 0 if the HUD stays in the text colors | 16 text entries |

Palette count is not a constraint. One sprite bank, animated by the same fade the script already runs.

## 4. Display and the 30.667 Hz tick

The stage window is 480×400 inside the PC-98 640×400 screen. The 80-pixel side columns are black during play. The HUD is text VRAM drawn over the playfield, not extra lines below it. Menus and the title use the full 640.

On the original machine the matching stock mode is CRTMOD 16: 768×512 shown, 1024×1024 page, 16 colors, one page, horizontal 31.5 kHz, vertical 55.46 Hz (31,500 / 568 lines). That page is tall enough to scroll the map in hardware and wide enough for the 480-pixel window with borders. CRTMOD 4 (512×512, 16 colors, four pages, same 55.46 Hz) also fits the 480×400 window but the page is only 512 lines tall, so the map ring wraps sooner.

CRTMOD 19 (640×480, 31.5 kHz, 59.94 Hz) was added on the Compact. It is not on the original 10 MHz machine. The stub in `src/hal/x68k/video_x68k.c` that aims at 640×480 should not be the bring-up mode.

A custom vertical total of 525 lines (480 visible, about 60.0 Hz) is possible on the original CRTC. It is an extra bring-up risk. Stock 55.46 Hz is the plan.

Pacing. Logic is 92/3 = 30.667 Hz. One logic frame at 10 MHz is 10,000,000 × 3 / 92 = 326,087 cycles.

| Refresh | Logic frames per vblank | How to pace |
| --- | --- | --- |
| 55.46 Hz (CRTMOD 16) | 0.553 | Run one logic tick on 5 of every 9 vblanks (30.81 Hz, 0.5% fast), or a fractional accumulator for exact 30.667. Show the last logic frame on the idle vblanks. |
| 60.0 Hz (custom 525 lines) | 0.511 | Same accumulator. Every other vblank is 30.0 Hz, 2% slow, so the accumulator is the one to use. |

Never run two logic ticks inside one vblank on this machine. The vblank is faster than the game, so the display repeats frames. It does not need the PC port's 4-frame catch-up, and catch-up would blow the 326k cycle budget.

Scroll of 4 pixels per logic frame is a CRTC register. A new 16-pixel map row is copied about every fourth tick: 480×16 at 4 bits is 3,840 bytes, fine next to the interpreter.

## 5. CPU budget at 10 MHz

The run counted, per logic frame, bytecode opcodes, sub-ops, 8086 instructions inside opcode `0x0C`, native calls, and collision candidate pairs (opcode `0x4F` / `0x50`).

Stage 4 is the heavy one. The other stages sit in the same band (opcode p50 from 376 to 492, 8086-instruction p50 from 1509 to 2188).

| Per logic frame, stage 4 | p50 | p95 | max |
| --- | --- | --- | --- |
| Bytecode opcodes | 442 | 724 | 958 |
| Sub-ops | 180 | 334 | 644 |
| Native calls (`0x0C`) | 107 | 176 | 255 |
| 8086 instructions inside those calls | 1828 | 2982 | 4356 |
| Collision pairs | 136 | 362 | 791 |

Cycle model, stated as a model, not a trace from a 68000. Jump-table interpreter, gcc -O2, no cache: 100 cycles for a typical opcode, 50 for a sub-op, 40 for a pair test, 400 for a native that has been rewritten in C. An 8086 instruction still executed by a C interpreter is taken at 100 cycles, which is kind to that interpreter.

| Work | p50 | p95 | max | Share of 326,087 at max |
| --- | --- | --- | --- | --- |
| Opcodes + sub-ops + pairs + rewritten natives | 101,000 | 174,000 | 262,000 | 80% |
| Same, but 8086 islands still interpreted | 284,000 | 472,000 | 697,000 | over the frame from the median up |

So:

- The bytecode VM is viable as the director at 10 MHz, with the natives rewritten. The heaviest measured frame lands near 80% of the budget on this model. A sloppy interpreter (extra calls per opcode, or 150+ cycles) eats the margin. Profile the real binary on XM6 before adding features.
- The VM is not viable as it runs today on the PC, because opcode `0x0C` is an 8086 interpreter. Those islands (ship `0x53AB`, shots, options `0x59CB` / `0x5F7B`, movers, score `0x71DC`) become ordinary C. That is the port, not a faster interpreter.
- Collision opcode `0x50` is the hot bytecode (about 648,000 executions in this run). Keep it in the interpreter only if the pair test stays a handful of compares. If a 10 MHz profile shows it hot, special-case it in C. Do not special-case the whole instruction set up front.
- Precompute offline, do not do it on the 68000: slice and dedupe patterns, build the PCG cache images, convert the map into a 1024-tall graphics page per stage, convert FM instruments to YM2151 register images, premix any drum or explosion sample to one MSM6258 buffer.

Raw clocks on the machines that are not targets, for scale only. A 16 MHz 68000 has 1.6× the cycles (about 522,000 per logic frame). A 25 MHz 68030 has more than 2.5× because of the cache. Both would hide a sloppy interpreter. The plan does not depend on that.

The audio sequencer is a separate interrupt near 59 Hz (the PC-98 driver uses YM2608 timer A at about 58.9 Hz). A few thousand cycles there is about one percent of 10 MHz. It has to be the C driver, not `FPLAY.COM`.

## 6. Sound

PC-98 chip: YM2608 (OPNA) at 7.9872 MHz. Six 4-operator FM channels, three SSG square channels, six ADPCM-A rhythm voices from the Yamaha ROM, one ADPCM-B channel. `FPLAY.COM` is a 16 KB 8086 driver with 16 logical channels and 48 tracks in `SONG.DAT` (14 KB). Stage BGM does not write the ADPCM-A ports. That was measured on this `SONG.DAT`: tracks 0–47, started as music or as effects, never touch low-port addresses `0x10`–`0x1F`.

X68000 chip: YM2151 (OPM), eight 4-operator FM channels, and one MSM6258 ADPCM channel. No SSG.

| OPNA piece | OPM / MSM6258 | How close |
| --- | --- | --- |
| 6× FM, 4-op, the same eight algorithms | 6 of the 8 OPM channels | Registers differ (f-number and block become KC/KF). Envelopes match closely. SSG-EG on the OPNA has no OPM equivalent. The LFO is a different circuit (OPNA multi-waveform LFO versus OPM AMS/PMS). Melody and bass can be faithful. A wide LFO or SSG-EG patch will not be. |
| 3× SSG square | The two spare OPM channels, algorithm chosen to sound narrow | A square will sound rounder. The third SSG voice is folded into one of those channels or dropped. This is the audible gap in the BGM. |
| ADPCM-A, 6 drums | One MSM6258 channel | Not six live voices. BGM does not use them. Any effect that does is premixed offline into one sample and played on the MSM6258. |
| ADPCM-B | Same MSM6258 channel | One shot at a time. Premix, do not stream from the 68000. |
| 16 logical FPLAY channels | 8 OPM voices, stolen the way the PC port already moves overlapping effects off the music slots | Music keeps 6. Effects borrow 2, and may steal a music voice for the extend jingle the way slots 7 and 8 are cleared today. |

`FPLAY.COM` itself does not run. The sequence data in `SONG.DAT` is interpreted by a new C driver that writes OPM registers. Instrument conversion is a build-time table, not a runtime translation of every register write.

Expect the songs to be recognizable. Do not expect a chip-identical dub until someone listens on hardware and retunes the SSG lines. There is no honest percentage without that listen.

## 7. Memory and media

Stock RAM is 1 MB on the original, 2 MB on the XVI. The plan assumes a 4 MB (or larger) expansion, which the original accepts up to 12 MB. Graphics VRAM (512 KB) and text VRAM (512 KB) and the 32 KB sprite RAM are separate and are not part of this budget.

| What | Size |
| --- | --- |
| `SGM1.DAT` compressed | 549 KB |
| `SONG.DAT` | 14 KB |
| `FPLAY.COM` | 16 KB of 8086. Not copied into the image as code. |
| Script chunks 1–6 decompressed | about 250 KB |
| All sprite patterns if no stage shares a cell | about 370 KB |
| 68000 code, stacks, entity table (256×0x52), rank, text | well under 1 MB |
| Preloaded total, rough | about 2–3 MB once the map pages and the OPM/ADPCM buffers are included |

| Machine RAM | Fit |
| --- | --- |
| 1 MB stock | No. The preload does not fit next to the program. |
| 2 MB | Only if stages stay compressed and stream off disk. That is not this plan. |
| 4 MB expansion | Yes. Preload graphics, sliced patterns, and converted audio. This is the target. |

Media. A 5.25-inch 2HD floppy is about 1.2 MB. The compressed game plus a 68000 executable can fit one disk only if the preconverted PCG and ADPCM buffers are built into extended RAM after load, not stored raw. Two floppies or a hard-disk image is the straightforward layout: disk 1 is Human68k and the executable, disk 2 (or the HD partition) is `SGM1` and the converted audio. Boot reads them into the 4 MB once. The 2 second stage gaps do not touch the disk.

## 8. Endianness

The 68000 is big-endian. The PC-98 data is little-endian. The C port already keeps scripts and entity records as byte arrays and reads words with explicit little-endian accessors, so that C compiles on a 68000 without a silent swap bug. A build-time converter is still worth it for anything the 68000 will load with `move.w`.

| Data | Conversion |
| --- | --- |
| SGM1 directory (`u16` bank, `u16` offset) | Swap, or keep the LE reader. |
| Bytecode immediates (every `u16` operand) | Keep the LE reader. Swapping the whole chunk breaks the 8086 islands if any remain, and breaks byte-aligned opcodes. |
| Entity records | Already a byte image. No swap. |
| Sprite and tile patterns | Bitplanes, byte oriented. Re-pack into PCG nibble order (four 8×8 quadrants, two pixels per byte). That is a layout change, not an endian swap. |
| Map words | Swap if the scroll page is stored as 68000 words. |
| `SONG.DAT` track pointers and event words | Swap in the converter that emits OPM register lists. |
| `FPLAY.COM` | Do not convert. Replace. |
| `RBRANK.DAT` | 10-byte header plus 10 score records. Swap the score dwords and the header words. Names are byte pairs. |
| Palettes | Bytes. No swap. Expand 4-bit PC-98 levels to the X68000 GRBI word. |

## 9. Toolchain and the HAL

Compiler. GCC targeting `m68k-elf` or `m68k-human`, then [elf2x68k](https://github.com/yosshin4004/elf2x68k) (or the Lydux xdev GCC 4.6 kit, which is the same idea) to produce a Human68k executable. Link against a small Human68k libc or libiocs. C99 as the port already uses is fine on a current m68k-elf GCC; the old xdev GCC is the one to pick if the Human68k syscall wrappers are required and a newer toolchain fights them. Develop on the PC with the existing SDL HAL, and cross-compile the same `src/core` with `PLATFORM=x68k`.

Emulator. XM6 TypeG for sprite limits, CRTC, and OPM, because that is the emulator people check against real boards. MAME's `x68000` driver for a Linux run and for timing traces. Confirm the 128/32 sprite behavior on XM6; MAME's source comment has disagreed with Sharp's 32-per-line figure before.

HAL, replacing SDL2. The files under `src/hal/x68k/` are the seam.

| SDL2 today | X68000 |
| --- | --- |
| Window, 640×400 texture, no vsync | CRTMOD 16. Graphics plane at `$C00000`, scroll via the CRTC. Text plane at `$E00000` for the HUD. |
| Software sprite blit | Sprite records at `$EB0000`, patterns at `$EB8000`. The core submits the capped cell list. |
| `ymfm` YM2608 | YM2151 at `$E90001`. MSM6258 through IOCS `_ADPCMMOD` / `_ADPCMOUT` for one premixed sample. |
| Keyboard / gamepad | IOCS `_JOYGET`. The script's edge bits stay as they are. |
| 1 ms waitable timer, 30.667 Hz accumulator | VDISP interrupt. Add 30.667/55.46 each vblank and run one `vm_frame` when the accumulator crosses 1. |
| `FPLAY.COM` loaded as 8086 | C sequencer fed by the converted `SONG.DAT`. |

## Phases

Each phase is shippable on its own. None of them is a redraw.

1. **Picture.** CRTMOD 16, one stage's map on the graphics plane, hardware scroll at 4 pixels per logic tick, HUD glyphs on the text plane, side borders black. Still driven by the PC if that is easier, then the same path on XM6. Proves the 480×400 window and the 55.46 Hz pace.
2. **Sprites.** Offline slicer (this measurement's rules: skip empty cells, dedupe flips, PCG nibble layout). Capped renderer, debris first to drop. Run the recorded peak frames (stage 4 frame 5711, stage 5 frame 871) and check 128 and 32 on XM6.
3. **CPU.** Cross-compile the VM. Replace the 8086 islands with C. Time `vm_frame` on XM6 at 10 MHz across stage 4's heavy frames. The pass mark is one logic tick inside one vblank, no second tick queued.
4. **Sound.** Build-time FM to OPM table, two-channel SSG approximation, one MSM6258 buffer. Play stage 1's track beside the PC mix and retune by ear. This is the phase that can slip without blocking the picture.
5. **Playable build.** All five stages from the preloaded image, fire and death and continue, on XM6 and then on a 10 MHz board with 4 MB. Floppy or HD image last.

The large work is phase 3 (the native islands) and phase 2 (the cap, which is a visible behavior change, not a tool bug). Phase 1 is the existing split between map and sprites, pointed at IOCS. Phase 4 is a converter plus a thin register writer. Phase 5 is integration.
