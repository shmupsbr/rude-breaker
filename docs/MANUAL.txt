RUDE BREAKER 30th AE - PC PORT - USER MANUAL
============================================

Version 0.10.1 - 2026-11-04

The program shows the same number: rudebreaker --version, the window
title, and the top line of the CONFIG page (VER 0.10.1).

Also in: Portuguese (MANUAL.pt-BR.md / .txt), Japanese (MANUAL.ja.md / .txt).


1. WHAT THIS IS
---------------

This is a PC (Windows and Linux) port of Rude Breaker, the vertical shoot 'em up
Compile made for the NEC PC-98 (Disc Station, 1996).

Rude Breaker 30th AE (30th Anniversary Edition) marks 30 years since
the game came out on Disc Station Vol. 10 on April 5, 1996. It is the
same game with these additions:

  - SPEED button: change the ship's speed during the game, with an
    SPD readout in the top bar (section 3).
  - RANK: an optional Zanac-style rank system, ON by default
    (sections 4 and 6).
  - MODERN palette is now the default (ORIGINAL is still available).
  - Weapon change during the game (SHIP SELECT OFF) is now the
    default (section 4).
  - Escape pauses only during a game, never on the title or in menus.
  - The final boss's homing shot now turns its trail sprite to the
    direction it flies, as in the original.
  - New title screen with the 30th AE mark and a ShmupsBR 2026 credit.

The port does not remake the game. It runs the original game scripts
and logic from the original data files. Enemy patterns, scoring, boss
HP and difficulty all come from the original code. Music is the
original FPLAY driver playing on an emulated YM2203 (ymfm), the
PC-98 FM sound chip the game was written for.

The PC side handles the window, sound output, keyboard and gamepad,
frame pacing, and a set of options in the CONFIG menu.

IMPORTANT: the game data (SGM1.DAT, SONG.DAT, FPLAY.COM) is
copyrighted. It is NOT part of the public port and must not be
redistributed. You supply it from your own legal copy of the game
(section 2). A private test build that includes it is for its tester
only. Please do not share it.


2. INSTALLING, RUNNING AND THE GAME FILES
-----------------------------------------

Unzip the folder anywhere. Nothing needs to be installed.

  Windows: run rudebreaker.exe.
  Linux:   in a terminal, in the folder: chmod +x rudebreaker, then
           ./rudebreaker
  Raspberry Pi: the same as Linux; see RASPBERRY PI at the end of
           this section.

Files of the port:

  rudebreaker.exe       The game (Windows).
  SDL2.dll              Window, sound and gamepad library (Windows).
  rudebreaker           The game (Linux x86_64).
  libSDL2-2.0.so.0      The same library (Linux).
  LICENSE-ymfm.txt      License of the ymfm sound chip emulator.
  LICENSE-SDL2.txt      License of the SDL2 library.
  MANUAL.txt / .md      This manual.

GAME FILES

The port has no game data of its own. It reads it from your own copy
of Rude Breaker (Disc Station Vol. 10, PC-98):

  SGM1.DAT     required   Stages, sprites, scripts and the game program.
  FPLAY.COM    required   The original music driver.
  SONG.DAT     required   The music and sound effect data.
  RBRANK.DAT   optional   The original hi-score and settings file.

SGC.COM, READJS.COM and RUDEBR.BAT are also on the disk; they are not
read and are not needed. No BIOS, font ROM or sound ROM is needed:
the music uses YM2203 emulation, which needs no ROM.

There are two ways to give the files to the port:

  1. Loose files. Copy SGM1.DAT, FPLAY.COM and SONG.DAT (and
     RBRANK.DAT if you want your old scores) into the game folder,
     next to the program, or into a subfolder named RUDEBR:

       Rude Breaker PC/
         rudebreaker.exe
         SDL2.dll
         SGM1.DAT
         FPLAY.COM
         SONG.DAT

     On Linux the names must match exactly, in upper case
     (SGM1.DAT, not sgm1.dat).

  2. A disk image. Put a PC-98 disk image of the game in the game
     folder (or in its RUDEBR subfolder) as it is. Accepted:
     .hdm, .fdi, .d88, .hdi, .nhd, or a .zip that holds one of those.
     The port reads the files out of the image's FAT filesystem,
     also from a RUDEBR folder inside it. Nothing is extracted to disk.
     If there are several images, the first one that has the files
     is used, trying .hdm, .zip, .fdi, .d88, .hdi and .nhd in that
     order.

Where the port looks, in this order. A folder or image given in 1 or
2 is always used, even when the files are not there (you then get
the window described below). Places 3 to 5 are used only when they
have the files; if none has them, the program's folder is reported:

  1. A folder or disk image given as the program argument, for
     example: rudebreaker.exe D:\games\rudebr.hdm
  2. The folder or image in the RUDEBR_ASSETS environment variable.
  3. The program's own folder: loose files, then a disk image.
  4. The RUDEBR subfolder of the program's folder.
  5. assets/raw/RUDEBR under the current folder (for developers).

If a required file is missing, the game does not start. A window
titled "disk files needed" shows the folder or image it tried, and
for each file whether it was found or is MISSING. Add the missing
files and run the game again. If RBRANK.DAT is missing, the game
starts with the original hi-score table and default settings and
creates the file when it saves.

Files the game creates:

  RBRANK.DAT       The original save file: the hi-score table and the
                   game settings (difficulty and the GAMEPLAY options).
                   Kept in the folder the game files come from (next
                   to the disk image, when you use one). Delete it to
                   reset scores and settings.
  rudebreaker.cfg  PC options: palette, rank, training, key and pad
                   bindings. Next to the program.
  rudebreaker.ini  Frame pacing choice: pacing=smooth or pacing=exact.
  pace.txt         A small timing report (see section 7). Safe to
                   delete.


RASPBERRY PI

The Raspberry Pi package is for 64-bit Raspberry Pi OS:

  RudeBreaker-RaspberryPi-*.zip    64-bit Raspberry Pi OS (aarch64).
                                   Pi 3, Pi 4, Pi 5, Pi Zero 2 W.

To check your system, open a terminal and run:

  uname -m           aarch64 = 64-bit
  getconf LONG_BIT   64

Unzip the package, add SGM1.DAT, SONG.DAT and FPLAY.COM next to the
program (same names, same upper case), then, inside that folder:

  chmod +x rudebreaker
  ./rudebreaker

The package brings its own libSDL2-2.0.so.0, so nothing else needs
to be installed.

Desktop: on the current Raspberry Pi OS the desktop runs on Wayland,
and the game uses it by default. If the window does not open or
looks wrong, force X11:

  SDL_VIDEODRIVER=x11 ./rudebreaker

Raspberry Pi OS Lite (no desktop): run the game from the text
console, not over SSH, with:

  SDL_VIDEODRIVER=kmsdrm SDL_AUDIODRIVER=alsa ./rudebreaker

Your user must be in the video and input groups (the default user
already is). If it is not:

  sudo usermod -aG video,input $USER

then log out and back in.

Performance: a Pi 4 or Pi 5 is recommended. On a Pi 3, keep the
window small instead of full screen, and close other programs.

"cannot execute binary file: Exec format error" means you are using
a 32-bit system or the wrong package. Only 64-bit Raspberry Pi OS
is supported; 32-bit builds are no longer made.


3. CONTROLS
-----------

Keyboard (defaults):

  Move              Arrow keys, W A S D, or keypad 8 4 6 2
  Fire / confirm    Z, Space, or keypad 0
  Mode              X, N, M, Enter, or keypad Enter
                    (switches the side pods between attack and
                    defense)
  Special           Left or Right Shift
  Speed             C, Left Ctrl or Right Ctrl (see below)
  Pause / back      Escape
  Quit              F10
  Fullscreen        F11 (on/off at any time; saved like the
                    FULLSCREEN option)
  Timing overlay    F8 (shows the frame pacing info, see section 7)

Gamepad:

  Any standard PC gamepad (XInput or DirectInput) is detected
  automatically. You can plug it in or unplug it while the game is
  running.

  Move              Left stick or D-pad
  Fire / confirm    South button (A on Xbox pads)
  Mode              East button (B)
  Special           West button (X)
  Speed             North button (Y). On pads without a standard
                    layout: button 4 (the fourth button).
  Pause             Start
  Quit              Back / Select

SPEED button: each press changes the ship's speed during the game,
going up and down in turn:

  LOW  ->  MID  ->  MAX  ->  MID  ->  LOW  ->  MID  ...

  LOW  slow
  MID  normal (default)
  MAX  fast

The top bar shows the current speed as SPD LOW, SPD MID or SPD MAX.

Each game starts at MID. A change made with the button lasts until the
game ends. How fast each step really is depends on SHIP SPEED in
GAMEPLAY.

PAUSE: Escape or the pad's START button pauses only while you are
playing a game. START is detected automatically: on pads known to SDL
(Xbox, PlayStation, Switch and most others) it is the START/OPTIONS/+
button; on plain joysticks it is button 8 or 10, or the first other
button above 10 that you press during play. While paused, a menu shows
RESUME and MAIN MENU. RESUME (or Escape/START/B again) goes back to the game. MAIN MENU ends the game and goes back to the title sequence (Compile logo first).
The pause menu is drawn over the frozen game picture, which keeps the
COLOR FILTER and CRT effects on. The pause menu works on the pad too: d-pad or stick to move, A to confirm, B or START to resume.
On the title, in the attract demo and in the menus it does not pause.
In the menus, Escape (keyboard) or the B button (pad) goes back one
page.

The window can be resized. The picture keeps the original 640x400
aspect.


4. TITLE SCREEN AND CONFIG MENU
-------------------------------

The logo, title, ranking display and intro are the original screens.
The game starts on the Compile logo and its music, as in the original. With SIDE
PANELS on, the top score line of the playfield is inset a few pixels
on each side so it is not clipped by the panel frames.
The title screen shows "30TH ANNIVERSARY EDITION" in white, centred
under the logo, with a soft glow around the letters every 5 seconds
(the same glow plays under the logo in the left side panel), PUSH
TRIGGER changing smoothly between yellow and green every 10 seconds,
and the credit lines (c) COMPILE 1996 and (c) SHMUPSBR 2026.
Press Fire at the title to start, or open CONFIG from the title menu.
In the title menu, RETURN goes back to the start of the game (Compile
logo, demo and title, as in the original) and EXIT closes the game.
The menu rows sit over the big (c) COMPILE 1996 line of the title art;
that is how the original lays it out.

Attract mode (GAMEPLAY > ATTRACT MODE, ON by default): when nobody
touches the keyboard or pad, the original loop (Compile logo, intro
flight, title, ranking) plays once, and as soon as the ranking ends a
30-second demo of one stage follows, without music (only the game's
sound effects), with a
blinking INSERT COIN in the middle of the screen, arcade style (just
for fun: no coins needed); then the loop again and the next stage, from
stage 1 to stage 5 and around. Any key or button ends the demo and
goes back to the title sequence. A demo saves nothing: no hi-score,
no ranking entry, no settings. The demo ship's movement follows a
real player's path; it is our own input recording, not game data.

The two rankings are different things. In attract mode the ranking is
part of the original loop: it shows by itself for a few seconds and
moves on, and no key is needed (any key leaves the loop). RANKING in
CONFIG opens the same table to look at as long as you like; it closes
when you press a key.

CONFIG has these pages: GAMEPLAY, GRAPHICS, SOUND, ALTERNATE VERSIONS, CONTROLS, RANKING,
PASSWORD, ORIGINAL PRESET, RESTORE DEFAULTS and BACK. Settings keep their old names in rudebreaker.cfg,
so saves from earlier builds load unchanged.

GAMEPLAY

  Options, in this order: DIFFICULTY, RANK, SUICIDE BULLETS, SHIP
  SELECT, SHIP SPEED, TRAINING, ATTRACT MODE, I-FRAMES and BACK.

  DIFFICULTY       VERY EASY, EASY, NORMAL, HARD, VERY HARD. These are
                   the original game's own difficulty levels. NORMAL
                   is the original default. Saved in RBRANK.DAT.

  RANK             ADVANCED (default), CLASSIC or OFF. ADVANCED: a
                   hidden rank that also reacts to your power, pickups
                   and medals, and adds return bullets and a score bonus
                   (see section 6, RANK MODE ADVANCED). CLASSIC: enemy
                   bullets get faster or slower with how you play (see
                   section 6). OFF: bullet speed stays fixed, as in the
                   original. ADVANCED is the default only for a new
                   rudebreaker.cfg; an existing save keeps its choice.

  SUICIDE BULLETS  ON: enemies fire bullets back when they are
                   destroyed, as in the original option. OFF: they don't.
                   (Builds before pr9zg showed this label inverted.
                   The saved setting is unchanged, so the game plays
                   the same; only the label is now right.)

  SHIP SELECT      The original game's option of the same name. It
                   controls how weapons work:
                   ON: at the start you pick one weapon (VULCAN, LASER
                   or MISSILE) on the "PLEASE SELECT YOUR OPTION"
                   screen. After that every weapon capsule is that
                   weapon, so you keep it for the whole game.
                   OFF (default): there is no pick. Capsules come as V,
                   L and M, and you change weapon during the game by
                   picking up a capsule of another letter.

  SHIP SPEED       STANDARD or AGGRESSIVE (default). STANDARD: LOW,
                   MID and MAX move the ship as in the original.
                   AGGRESSIVE: LOW is 15% slower, MID is the same and
                   MAX is 15% faster. Every game starts at MID; the
                   in-game speed button still switches LOW / MID /
                   MAX. The attract demo always uses the original
                   movement. Saved in rudebreaker.cfg as ship_speed.

  TRAINING         ON: practice mode (see section 5). OFF: normal game.

  ATTRACT MODE     ON (default) or OFF. ON: after the ranking screen a
                   30-second demo plays (no music, sound effects only),
                   then the Compile logo again. OFF: no demo. Saved in
                   rudebreaker.cfg.

  I-FRAMES         CLASSIC, NEW - FIRE, NEW - LIME or
                   NEW - BLUE (default). CLASSIC: the ship's invulnerability
                   after a hit or a power-up looks as in the original
                   (the red outline). NEW: light in the chosen colour
                   (FIRE: warm reds and orange, LIME: greens, BLUE:
                   blues, all picked to sit with the game's PC-98
                   colours) also runs over the ship itself in waves,
                   from its centre outward, and stops at the edge of
                   the sprite; the options get it too while they are
                   docked. The waves start with the invulnerability
                   itself, and one frame in six the whole ship flashes
                   as a silhouette in that colour. It pulses and thins
                   out as the time left runs down, so you can see when
                   it is about to end. Drawn by the port over the
                   picture as plain pixels (no smoothing); the color
                   filters and CRT options apply to it too. Only the
                   look changes: the invulnerability time is the same.
                   Saved in rudebreaker.cfg (iframes=classic / fire /
                   lime / blue; the old value new means fire).

  BACK

GRAPHICS

  Options, in this order: FULLSCREEN, SCALING, ASPECT, ORIENTATION, SIDE
  PANELS, PALETTE, BULLET COLOR, HITBOX, the six CRT
  effects, FRAME PACING and BACK.

  FULLSCREEN       OFF (default) or ON. Fills the whole monitor,
                   keeping the game's proportions (SIDE PANELS fill the
                   sides on wide screens) and hiding the mouse pointer.
                   F11 switches it at any time. Saved in
                   rudebreaker.cfg.

  SCALING          How the picture is scaled to the window or monitor.
                   AUTO: INTEGER when it fills at least 80%
                   of the screen height, otherwise SHARP. That gives
                   SHARP at 720p and 1080p, INTEGER at 1440p, 4K and
                   800x480.
                   INTEGER: every game line becomes the same whole
                   number of screen lines, with black borders around.
                   With ASPECT 8:5 the pixels are square and all the
                   same size. With ASPECT 4:3 the height is a whole
                   multiple (2x = 800 lines, 3x = 1200) and the width
                   is set to close the 4:3 shape.
                   SHARP (default): fills as much of the screen as possible
                   keeping the proportions, without blur (some pixels
                   come out one screen pixel bigger). SMOOTH: fills
                   the same way with soft (bilinear) filtering. If the
                   window is too small for INTEGER, it acts as SHARP.
                   With SIDE PANELS shown the 16:9 picture (panels
                   included) fills the screen; with INTEGER (or AUTO
                   choosing it) the playfield inside it is drawn at a
                   whole multiple of its lines, centred in its area
                   with black around it. Saved in rudebreaker.cfg.

  ASPECT           Shape of the whole picture on screen.
                   4:3 (default): the 640x400 picture is shown with
                   the shape of a PC-98 monitor, each pixel 1.2 times
                   taller than wide (5:6), as the art was drawn to be
                   seen. On a 1080p screen that is 1440x1080 with
                   SHARP. 8:5: the 640x400 picture as it is (16:10,
                   square pixels); it looks a little flatter than on
                   the original. With SIDE PANELS the playfield follows
                   the same choice (8:5 leaves black bars above and
                   below it). With 8:5 the
                   ship's vertical movement is 1.2 times faster, so it
                   crosses the flatter playfield in the same time as
                   on 4:3 (the attract demo keeps the original moves).
                   Saved in rudebreaker.cfg.

  ORIENTATION      0 (default), 90, 180 or 270. Turns the whole picture
                   (game, title and menus) by that many degrees
                   clockwise, for a monitor that is turned or mounted
                   upside down. It is never guessed from the monitor:
                   you choose it. 0 and 180 use the normal wide layout.
                   90 and 270 use the vertical (TATE) layout: at the
                   top, the logo with the title's frame, then SCORE,
                   LOADOUT, and NO DEATH over SPEED; the whole playfield
                   in the middle, full width; at the bottom RANK, HIT
                   RATIO / HIT COUNT, LEVEL / TIME, EXTENDS / I-FRAMES,
                   and MEDALS over INPUT. With 90, turn the monitor so
                   its right edge is on top; with 270, its left edge.
                   Best with FULLSCREEN ON. SCALING and ASPECT apply as
                   usual. Saved in rudebreaker.cfg as orientation.

  SIDE PANELS      ON (default) or OFF. ON: in a 16:9 window or full screen, the
                   playfield sits in the middle with info panels on
                   both sides: rank graph, loadout, extends, i-frames,
                   speed, score, hit ratio, medals, level, time, NO
                   DEATH and an arcade stick that shows your input.
                   The logo and side art are taken from your own
                   title screen.
                   OFF, or a 4:3 / 16:10 window: the classic look.
                   LOADOUT shows two levels, one block per level, level
                   1 included (it fills the first block). SHOT is the
                   main shot: 4 levels, so 4 blocks; levels 2, 3 and 4
                   (MAX) come at 2, 10 and 18 power-up items and their
                   blocks fill item by item. Losing a life takes it
                   down one level. Each V / M / L sub-weapon has 3
                   levels, so 3 blocks; one more capsule of the weapon
                   you already have adds a level; at MAX
                   a capsule gives 1000 points. A capsule of another
                   colour changes the weapon and keeps its level. A
                   hit while you carry a sub-weapon takes it instead
                   of a life: the box then shows NO SUB and an empty
                   capsule slot. EXTENDS shows your ship and the lives
                   in stock.
                   I-FRAMES is your invulnerability time as a pie: a
                   full disc when given, a wedge opening clockwise
                   from 12 o'clock as it runs out, and only a faint
                   circle outline when empty. Yellow: power-up
                   item or capsule. Violet: after a hit or a respawn.
                   In the rank graph a skull marks each lost life.
                   The graph shows the last 60 seconds and scrolls:
                   old data drops off the left as new data comes in.
                   It is cleared at the start of each stage (the rank
                   itself is not).
                   With RANK OFF (or in Training) the graph box shows
                   RANK SYSTEM / OFFLINE in green instead, with a slow
                   CRT-style scanline rising through it every 15
                   seconds, and the rank numbers are hidden. Rank then
                   has no effect at all on the game.
                   Rank carries over from stage to stage; it starts
                   again only with a new game. The small "x1.05" next
                   to it is the current rank rise multiplier.
                   The playfield follows ASPECT: 4:3 (default) shows it
                   with the shape of a PC-98 monitor (pixels 1.2 times
                   taller than wide); 8:5 uses square pixels.
                   HIT COUNT shows the stage's
                   enemies you destroyed / the
                   stage's fixed number of enemies
                   (369, 315, 128, 205 and 46 in
                   stages 1 to 5: the enemies each
                   stage itself places; endless
                   streams, shot spawners and extras
                   that depend on rank or timing are
                   left out, as are bullets, items
                   and hidden spots). HIT RATIO is
                   the same as a percentage, with a
                   bar. After a lost life part of the
                   stage plays again; the count never
                   goes past the total.
                   MEDALS counts the hidden medals taken this stage
                   out of the 10 that every stage hides (stages 1 to
                   5). The count is the game's own medal counter: it
                   goes up only when you pick up a revealed medal and
                   starts again at 0 each stage.
                   NO DEATH shows YES until you lose a life, then NO
                   (no count is shown). It stays NO through all five
                   stages and a continue, and
                   starts again at YES only with a new game. In
                   Training the box header also shows TRAINING.
                   From loop 2 on, the RANK box title also shows the
                   loop: LOOP 2. It is hidden in loop 1.
                   The vertical (portrait) layout is no longer a SIDE
                   PANELS choice: it comes from ORIENTATION 90 or 270. A
                   save from an earlier build with SIDE PANELS TATE
                   loads as ON with ORIENTATION 90.

  PALETTE          MODERN (default): Filipe's color rebalance of the
                   game. ORIGINAL: the PC-98 colors.

  BULLET COLOR     PINK (default), LILAC, BLUE, GREEN or ORIGINAL.
                   Every enemy bullet that cannot be shot down is
                   recoloured, boss lasers included
                   at full length, Cave style,
                   whatever its original
                   colour (red, orange or gold); its dark rim, body
                   and bright core keep their shading. The game marks
                   these in the object type, not by colour: enemy
                   bullets have the bullet bit and no "can be shot"
                   bits. Bullets you can shoot, enemies, items and
                   your own shot keep their colours, and so do the
                   harmless fireballs left when you shoot a bullet
                   down. ORIGINAL leaves
                   every bullet as in the game.

  HITBOX           OFF (default), PINK, LILAC, BLUE, GREEN or RED
                   (an old WHITE setting becomes PINK). Draws the ship's real collision box (12 x 18
                   pixels) as a closed thin outline with a 40 %
                   see-through fill, so the ship stays visible and
                   you can see what can be hit. The box never uses the
                   same colour as the enemy bullets: that colour is
                   skipped, and if you change BULLET COLOR to the
                   hitbox colour, the hitbox moves on to the next one.
                   RED is skipped with BULLET COLOR ORIGINAL (the
                   original bullets are red). Saved in rudebreaker.cfg.

  COLOR FILTER     (ALTERNATE VERSIONS > CUSTOM) ORIGINAL (default), GAME BOY, VIRTUAL BOY, COMMODORE 64,
                   GAME GEAR, PC ENGINE, ZX SPECTRUM, MSX2, MEGA DRIVE. Recolours only the
                   playfield, and
                   only while a game (or the attract demo) is running:
                   title, menus, ranking, name entry and the side panels
                   always keep their real colours. The game's art and its
                   palette are untouched, and PALETTE still works
                   underneath. These filters are for fun: each one
                   exaggerates its machine's look. GAME BOY: DMG greens by
                   brightness, with the slow LCD (moving things leave a
                   ghost), the picture's own pixels (no blocks) under a fixed grid of square LCD dots at the real DMG pitch (160 across the playfield), its dark lines clearly visible, as on GAME GEAR. VIRTUAL BOY: shades of
                   red on black, with the short glow of the LEDs and the
                   the same clean square dot grid. COMMODORE 64: the 16 C64
                   colours (Pepto), 2x2 machine pixels, 3 colours plus black per 8x8 cell for the scenery; sprites, bullets and items keep their own colours, and PAL blending (each line mixes with
                   the one above). GAME GEAR: colour version of the GAME BOY
                   look: 4096-colour palette (4 bits per channel) with at
                   most 31 colours on screen (16 background + 15 sprite, sprite colour 0 is transparent),
                   chosen each frame, big crisp pixels (about 240x200
                   detail), no LCD smear, sharp edges, a clearly visible LCD dot grid and boosted contrast and saturation so sprites and bullets stand out from the scenery. PC ENGINE: strong,
                   saturated and bright colours (HuC6260 VCE: 3 bits per channel, 512 colours, levels v x 255 / 7), 16 colours per 8x8 background tile and 15 plus transparent per 16x16 sprite cell, no dithering, only a slight composite softness, on the same grid as MEGA DRIVE (240 columns over the playfield, all the game's lines; no resampling, so nothing shimmers while the stage scrolls).
                   ZX SPECTRUM: the real machine's limits, attribute clash included: the Spectrum's 192-line scale with big square pixels, and every 8x8 cell has just two colours, one INK and one PAPER out of the 8 base colours, sharing one BRIGHT bit; each pixel is one or the other. Black paper is preferred, as in most Spectrum games, and outlines and detail are kept. The scenery's cells are fixed to the scenery itself and scroll with it, so its colours never crawl or flicker. Scenery uses one solid dark (non-BRIGHT) Spectrum colour, with no ordered dither. Sprites are not dithered: each kind stays one flat colour plus black detail. Power-up capsules, your options and your shots take the weapon's colour (VULCAN red, LASER blue, MISSILE yellow). Your ship is bright white with black detail (light grey stays white, and the darker panel seams go black) and the wing tips stay visible in straight flight (during invulnerability only the I-FRAMES glow and flash take that mode's colour); the side options use that weapon colour with black detail too. Enemy bullets are never the same colour as the background behind them. In space (a black background with white stars) enemies are not white: they are yellow, magenta or green. The first large ship of stage 2 is dark yellow and the second is cyan, flat, even when that ship is drawn into the scrolling page. On the clouds stage the sky is solid cyan and the clouds are white with a black dither; a dark green city field stays dark green. Dark outlines, edges and inner detail lines of sprites and scenery are kept in black so shapes read, and the background stars are always white. Dark scenery keeps the dark (non-BRIGHT) colour of its own hue (the stage 1 asteroids stay dark magenta). It is not painted bright cyan. The on-screen text (score line at the top, stage captions) ignores the cells: it is drawn low-res, monochrome and opaque on top of the scenery so it never disappears. The same rule applies under every COLOR FILTER: the score line and stage captions are drawn low-res (2x2) and opaque on top, in white and black (on GAME BOY the lightest and darkest LCD greens, on VIRTUAL BOY the brightest red and black), so they stay readable. MSX2: SCREEN 5 style, 256x212 (scenery pixels fixed to the scenery, sprite and score-line pixels to the screen, so nothing shimmers), 16 colours at a time picked for the scene from the MSX2's 512 (3 bits per red, green and blue); the set changes slowly so it does not flicker; every pixel takes the nearest of the 16, no colour clash. MEGA DRIVE: interlace mode, 240 columns across the playfield on the game's own line count (a 320x224 grid made everything, sprites included, seem to wobble as the scenery scrolled), the Mega Drive's 512 colours (3 bits per channel, real DAC levels) in 4 lines of 15 colours plus the black backdrop: two lines for the scenery, two for sprites; each 8x16 tile uses one line, and the lines change slowly so they do not flicker.
                   Not saved: every launch, and the end of every game, goes back to ORIGINAL (old color_filter values in rudebreaker.cfg are ignored and removed).

  CRT effects: six separate ON/OFF switches, all OFF by default. Like
  COLOR FILTER they touch only the playfield during play (and the
  demo), never the title, menus, ranking or side panels. They can be
  combined with each other and with any COLOR FILTER, and work in
  every ORIENTATION. Saved in rudebreaker.cfg as crt_scanlines,
  crt_mask, crt_curvature, crt_bloom, crt_vignette and crt_badrf.
  Each one costs some speed; on a Raspberry Pi turn on only the ones
  you want.

  SCANLINES        Darker gaps between the lines, as on a low-resolution
                   CRT.

  CRT MASK         The red/green/blue slot mask of a TV tube.

  CURVATURE        Bends the playfield like the curved glass of a tube,
                   with black corners.

  BLOOM            Bright parts glow softly onto their surroundings.

  VIGNETTE         The edges and corners get gradually darker.

  BAD RF CABLE     A cheap aerial (RF) connection: wobbling lines,
                   colour bleed, a faint ghost image and some snow.

  FRAME PACING     SMOOTH or EXACT (see section 7).

  BACK

SOUND

  Options: SOUND CHANNELS, MASTER VOLUME, BALANCE, OUTPUT PROFILE,
  CUSTOM EQ, SOUND TEST and BACK.

  SOUND CHANNELS   EXTRA (default): effects play on a second emulated
                   chip, so the music is never cut. ORIGINAL: sound
                   effects share the FM chip with the music, as on
                   the PC-98, so an effect can cut a music channel
                   for a moment. Saved in rudebreaker.cfg.

  MUSIC CHIP       (ALTERNATE VERSIONS > CUSTOM) ORIGINAL (default), SID, GAME BOY, PSG, PC ENGINE, VIRTUAL BOY, ZX SPECTRUM, MSX2, MEGA DRIVE. For
                   fun: plays the music through a simple model of another
                   machine's sound chip instead of the PC-98 FM chip. The
                   notes the game sends to the FM chip are read (pitch, key
                   on/off, loudness) and the most important ones go to the
                   voices that chip has, so parts may drop out and the sound
                   is rough on purpose; it is not an exact conversion. SID
                   (Commodore 64): 3 voices with saw, pulse (with pulse-
                   width sweep), triangle and noise, through a resonant
                   filter. GAME BOY: 2 pulse channels, a 4-bit wave channel
                   for the bass and a noise channel, with a 4-bit DAC. PSG
                   (Game Gear SN76489): 3 square waves and noise, 2 dB
                   volume steps, and no notes below about 110 Hz. PC ENGINE:
                   6 channels of 32-step 5-bit wavetables. VIRTUAL BOY (VSU):
                   5 channels of 32-step 6-bit wavetables (the fifth with
                   sweep and modulation) and a noise channel, nothing below about 76 Hz. ZX SPECTRUM (AY-3-8912 of the 128K, 1.7734 MHz, mono): 3 square waves, one shared noise generator and the hardware envelope, 16 log volume steps. MSX2 (the PSG and a Konami SCC cartridge, no FM): 3 PSG square waves and noise, and 5 SCC channels of 32-step wavetables (the last two share a wave); the guitar is two SCC saws on the root and the fifth over a PSG square an octave down; bass and the other parts play on the SCC; drums are PSG squares and noise. MEGA DRIVE (Mega Drive 1: YM2612 FM, 6 channels of 4 operators, plus the SN76489 PSG): brass, bass and guitar FM patches (the guitar is a metal FM guitar played on an emulated YM2612 core: a high-feedback patch distorted on the chip itself, the note doubled an octave below about 11 cents flat and again two octaves below, short picked notes that slide into the pitch and fall away on release, no vibrato, no amp or cabinet) with the YM2612's coarse volume steps and 9-bit DAC sound; drums on the PSG squares and noise, no samples. The SOUND TEST
                   also plays through the chosen MUSIC CHIP. Independent of
                   COLOR FILTER. Not saved (back to ORIGINAL on every launch and after every game).
                   The distorted guitar gets a whole chip of its own (on VIRTUAL BOY and SID, each guitar part gets one),
                   using all its voices as one guitar (SID: sync/ring and
                   filter; GAME BOY: two pulses, a distorted wave and a
                   noise pick; PSG: three squares and a noise pick; PC ENGINE and VIRTUAL BOY: wavetables and a noise pick; ZX SPECTRUM: a square on the root (the envelope's buzzing saw only where its coarse period hits the note in tune), a power-chord fifth and octave and a noise pick; MSX2: two SCC saws on the root and the fifth, a PSG square an octave down and a PSG noise pick; MEGA DRIVE: an overdriven FM chain on the root and the fifth with a noise pick),
                   then amp and
                   cabinet colouring. Another chip plays the rest, with bass
                   and drums kept at the original's balance, and a second guitar part plays there too: two chips in all (VIRTUAL BOY and SID: up to three).

  SFX CHIP         Same choices for the sound effects, independent of MUSIC
                   CHIP. It applies to the second chip used by SOUND
                   CHANNELS: EXTRA; with ORIGINAL the effects play on the
                   music chip and follow MUSIC CHIP. Not saved.

  ALTERNATE VERSIONS  (its own CONFIG page, after SOUND) One
                   switch for "what if the game had come out on another
                   machine": ORIGINAL PC-98 (ORIGINAL/ORIGINAL/ORIGINAL),
                   COMMODORE 64 (filter COMMODORE 64, chips SID), GAME BOY
                   (GAME BOY, GAME BOY), GAME GEAR (GAME GEAR, PSG), PC
                   ENGINE (PC ENGINE, PC ENGINE), VIRTUAL BOY (VIRTUAL BOY,
                   VIRTUAL BOY), ZX SPECTRUM 128K (ZX SPECTRUM, ZX SPECTRUM),
                   MSX2 (MSX2, MSX2), MEGA DRIVE (MEGA DRIVE, MEGA DRIVE).
                   The active one shows ON. CUSTOM opens three more rows,
                   COLOR FILTER, MUSIC CHIP and SFX CHIP (described above), to pick
                   each one by hand (LEFT/RIGHT). None of this is saved:
                   every launch, and the end of every game (game over,
                   ending, or quitting to the title), goes back to ORIGINAL
                   PC-98.

  MASTER VOLUME    0 to 100 in steps of 10 (default 100). The volume of
                   the program's whole sound output, set by the port on
                   top of the game's own music and effect levels (which
                   do not change). 100 plays the sound exactly as
                   before. Saved in rudebreaker.cfg (master_volume=).

  BALANCE          CENTER (default), or LEFT / RIGHT 1 to 10: the
                   other side gets quieter, by a tenth per step, so 10
                   is one side only. Done by the port on the output.
                   Saved (balance=, -10 to 10).
  OUTPUT PROFILE   An EQ curve for what you listen on: FLAT (default,
                   no change), HEADPHONES (softer top end plus a light
                   crossfeed between the ears), TV (less deep bass,
                   voice range forward), MONITOR (desk speakers),
                   STEREO SPEAKERS, 2.1 (bass below about 120 Hz sent
                   to both sides alike, for a subwoofer, plus a bass
                   lift), LARGE SPEAKERS, HOME THEATER, LAPTOP and SMALL
                   SPEAKERS (deep bass cut so small drivers do not
                   distort), CUSTOM (the CUSTOM EQ curve). The level is
                   lowered by the largest boost so nothing clips. FLAT
                   with BALANCE CENTER leaves the sound exactly as
                   before. Saved (output_profile=).
  CUSTOM EQ        Page with a 10-band graphic EQ: 31, 62, 125, 250,
                   500 Hz, 1K, 2K, 4K, 8K, 16K, each -12 to +12 dB in
                   1 dB steps with left/right; Fire puts a band back to
                   0. Changing a band selects OUTPUT PROFILE CUSTOM.
                   Saved (custom_eq=, ten numbers).
  SOUND TEST       Opens the SOUND TEST page (below).

  BACK

CONTROLS

  JOYSTICK         ON or OFF: whether the game reads a gamepad. A pad
                   is switched ON by itself when it is plugged in, but
                   if you turn JOYSTICK OFF here it stays off, also
                   after a restart and when a pad is plugged in again,
                   until you turn it ON (saved as joystick=off; auto is
                   the default).
                   Below: DEVICE shows the one controller in use (its
                   name), or NONE. With two pads plugged in, the one in
                   use stays in use when the other is connected or
                   removed.
  DEFINE KEYS      Remap UP, DOWN, LEFT, RIGHT, FIRE, MODE, SHIFT,
                   PAUSE and SPEED. Select an action,
                   then press the new key or
                   pad button. RESTORE DEFAULTS puts back the keys
                   listed in section 3.
                   Pad buttons show readable names, by controller
                   family when it is known: A / B / X / Y, LB / RB, VIEW /
                   BACK and MENU / START on Xbox-style pads; CROSS,
                   CIRCLE, SQUARE, TRIANGLE, L1 / R1 on PlayStation
                   pads; Nintendo letters by position. Triggers, sticks,
                   the D-PAD, BACK PADDLE 1-4, TOUCHPAD CLICK and the
                   share / capture button have their own names.
  BACK

SOUND TEST (opened from SOUND)

  SOUND            Music and sound effects ON or OFF.
  BGM              Choose a music track (0-47) with left/right; Fire
                   plays it.
  SE               Choose a sound effect (0-47) with left/right; Fire
                   plays it.
  PLAY / PAUSE / STOP   Player controls.
  MUSIC CHIP / SFX CHIP   The chips of ALTERNATE VERSIONS, changed
                   here with left/right; the track playing switches at
                   once. Like there, not saved: back to ORIGINAL at the
                   next start and after a match.
  VU               LEVEL (default) or SPECTRUM: the style of the meter
                   at the bottom. Saved in rudebreaker.cfg (vu=).
  The top line shows the track and its state. The meter at the
  bottom shows what is playing: LEVEL is a VU in LED segments from
  green to red with a held peak; SPECTRUM is a hi-fi style analyser,
  frequency bars and a line with peak dots.
  (Some numbers are silent: they are empty in the original data.)

RANKING            Opens the original hi-score table.

PASSWORD           Opens the original password screen.

ORIGINAL PRESET    One step to the game as it was on the PC-98 disk,
                   e.g. to compare. Press Fire once: PRESS AGAIN
                   appears; press Fire again to confirm. It sets RANK
                   OFF, SHIP SELECT ON (the weapon pick, as on
                   the disk), SOUND CHANNELS ORIGINAL, the original
                   palette, no COLOR FILTER, no CRT, no side panels,
                   original bullet colours, HITBOX OFF, SHIP SPEED
                   STANDARD, TRAINING OFF, I-FRAMES CLASSIC, the
                   original music and effect chips, OUTPUT PROFILE FLAT
                   and BALANCE CENTER. Window, scaling,
                   aspect, orientation, keys, DIFFICULTY, SUICIDE
                   BULLETS, MASTER VOLUME, hi-scores, the ranking
                   (RBRANK.DAT) and passwords are not touched; nothing
                   is deleted. Every option can still be changed after.
                   The line preset=original in rudebreaker.cfg does
                   the same once at the next start.

RESTORE DEFAULTS   Puts every CONFIG option back as on a new install
                   (keys included). Press Fire once: PRESS AGAIN
                   appears; press Fire again to confirm (moving away
                   cancels). Hi-scores, the ranking and passwords are
                   kept; JOYSTICK stays as it is.

BACK               Returns to the title.


CONTROLLER PROFILES (Steam Deck and other pads)

  Any pad SDL knows works with no setup: stick or D-PAD to move, the
  bottom face button to fire, the right one for MODE, START to pause.
  Use DEFINE KEYS for anything else.
  Steam Deck (in desktop or game mode, through Steam Input with the
  "Gamepad" layout): left stick or D-PAD move, A FIRE, B MODE, X SHIFT,
  MENU (the button right of the screen) PAUSE. The back grips (L4, L5,
  R4, R5) are sent as BACK PADDLE 1-4, so you can give one to SPEED or
  SHIFT in DEFINE KEYS; if Steam maps them to other buttons, set them
  in Steam Input instead. Not tested on a real Deck yet.

LINUX: WAYLAND AND X11

  On Linux the program opens on X11 first (on a Wayland desktop, through
  XWayland), which works everywhere. To use Wayland natively, start it
  with SDL_VIDEODRIVER=wayland ./rudebreaker ; to go back, leave the
  variable out or set SDL_VIDEODRIVER=x11.

5. TRAINING MODE
----------------

TRAINING is for practice:

  - Your ship cannot be hit. You never lose the weapon or a life.
  - The hidden medal spots flash red in place all the time, the same
    red flash you normally only see while shooting them, so you can
    learn where they are. You still have to shoot a spot until the
    medal is revealed, as in the normal game, and scoring is unchanged.
  - RANK is not used: the difficulty stays at the DIFFICULTY setting.
  - Scores from a training run are not entered in the hi-score table,
    and nothing is saved to RBRANK.DAT from that run.

Turn TRAINING off to play normally.


6. SCORES, SAVING AND THE ORIGINAL SECRETS
------------------------------------------

Hi-scores and settings are saved in RBRANK.DAT, in the original
format. A new score is saved when you enter your name.

Difficulty and all gameplay rules work as in the original.

RANK (GAMEPLAY option, ADVANCED by default for a new cfg) is an addition of this port,
modeled on the rank system of Zanac. A hidden rank value goes up
while you stay alive and when you score points, and is reset when you
lose a life (see below). It rises at one third of the rate used by
earlier versions of this port, times a multiplier that grows through
the game:
+1% for each main-shot level gained, +5% for each sub-weapon level
gained (a capsule that only gives 1000 points or changes the colour
does not count), and +5% for each extra life earned by score. The
multiplier starts at x1.00 with each new game and does not change
the reset on a lost life or the limits set by DIFFICULTY.
Losing the sub-weapon (a hit that leaves you with only the main shot)
takes one third off the current rank. The amount taken is kept, and
picking up a sub-weapon again gives it back (never above the limit
for your DIFFICULTY). A new game clears it. A higher rank speeds up
enemy bullets,
the same way a higher DIFFICULTY does; nothing else changes, and the
bonuses still follow the DIFFICULTY you chose. DIFFICULTY sets the
starting point and the floor: rank never makes bullets slower than
the difficulty you chose. On VERY EASY and EASY rank can only go half
way up; on HARD and VERY HARD it starts half way up. Rank starts again
with each new game.

When rank reaches the top for the difficulty in play, the game steps
up one difficulty level and rank starts again at that level's starting
point, until it reaches the ceiling for the DIFFICULTY you chose:

  VERY EASY  -> up to NORMAL (through EASY)
  EASY       -> up to HARD (through NORMAL)
  NORMAL     -> up to HARD
  HARD       -> up to VERY HARD
  VERY HARD  -> stays VERY HARD

At the ceiling rank stays at the top. On each step-up the LEVEL box in
the side panel flashes for 2 seconds and the RANK graph gets a green
line with an up arrow at that moment (deaths show a skull). Each
step-up also pays the game's own 1000-point bonus, with its 1,000
popup above your ship.

If you chose NORMAL or VERY HARD, once you are at the ceiling with rank
at the top, enemy bullets get 1% faster for every 20 seconds of play,
up to +25%. The LEVEL box shows it (for example V.HARD +3% or HARD +3%). Taking
a hit (losing the sub-weapon) or losing a life puts it back to +0%.

Losing a life takes you back to the DIFFICULTY you chose, with its
starting rank. Stepping up only changes what rank
changes (enemy bullet speed and the rank limits): scoring, bonuses and
everything else keep following the DIFFICULTY you chose, and the saved
setting does not change. Not in Training. With RANK OFF the game plays
exactly like the original: no step-up, no bullet speed-up and no
+1000 bonus, and the side panel shows RANK SYSTEM / OFFLINE in the
rank graph.

RANK MODE ADVANCED (RANK set to ADVANCED; saved as rank_mode=advanced)
replaces the rules above with a hidden rank from 0 to 1000. No number
is shown: the RANK graph has no values and a thin bar under it shows
progress toward the next LEVEL band: empty at the start of the band,
full at the next one, when the LEVEL box flashes, an up arrow is
marked on the RANK graph (as in CLASSIC) and the bar starts again for
the new band. At the ceiling it stays full, drawn brighter
with an outline. It starts and is kept between limits set by
DIFFICULTY:

  VERY EASY  starts 0,   at most 400
  EASY       starts 80,  at most 550
  NORMAL     starts 150, at most 800
  HARD       starts 280, at most 900
  VERY HARD  starts 400, at most 1000

It never goes below the start. It goes up:
  - every second you stay alive, faster the longer since your last
    life lost (up to three times the rate after 3 minutes);
  - when the main shot gains a level, and a little every second while
    the main shot is at level 3 or 4;
  - when you pick up a sub-weapon or level it up (more for a stronger
    one);
  - when you take a medal (each medal of a stage is worth more than
    the one before);
  - a little every second for each life in stock (up to 5);
  - a little every second while you hold the shot button.
While NO DEATH is still YES, every rise is 10% larger.
It goes down when you lose a life (by 15% to 25% of its value, more
when it is high; the survival bonus starts again) and a little (4%)
when you lose the sub-weapon.

What it changes:
  - Enemy bullet speed follows rank smoothly, from the speed of the
    DIFFICULTY you chose (never slower) up to the VERY HARD speed at
    the top. Above 850 bullets get up to 15% faster again.
  - From 700 up, destroyed enemies fire return bullets (as with
    SUICIDE BULLETS), even if that option is off. Below 700 the option
    you chose applies again.
  - Score: every point you earn is raised by up to 50% at rank 1000
    (half of rank/1000).
  - The LEVEL box shows the band rank is in (V.EASY to V.HARD, never
    below your DIFFICULTY) and flashes when it moves up a band.
The rate and number of enemy shots are not changed: in the game data
each enemy's shot timing is its own counter inside its script, so it
cannot be changed for all enemies without changing their patterns.
Loop 2 starts rank 100 higher and allows it 50 higher (at most 1000),
and rank rises twice as fast (see TWO LOOPS below).
Not in Training. RANK OFF turns all of this off.

TWO LOOPS is how every run is played. There is no LOOP option.
Clearing stage 5 does not show the ending: stage 1 starts again as
loop 2, and you keep your score, lives and weapons. After stage 5 of
loop 2 the run ends, unless Kraken appears (below). In loop 2:

  - The base difficulty is one level above the DIFFICULTY you chose
    (VERY EASY -> EASY -> NORMAL -> HARD -> VERY HARD). At VERY HARD
    it stays VERY HARD. The new level, with the game's own settings
    for it, takes effect when stage 1 of loop 2 starts.
  - Rank starts again at the new base's starting point. A NORMAL game
    is HARD in loop 2, so it can step up to VERY HARD there.
  - Every value driven by rank is doubled: how fast rank rises, how
    much rank adds to enemy bullet speed, and the speed-up at the
    ceiling. Enemy bullet speed never goes past the game's own VERY
    HARD value. The speed-up at the ceiling never goes past +50%.
  - Not in Training, rank gain is doubled in both rank modes, and
    enemy bullets get faster with rank inside the loop: the rank
    range (start to ceiling) is split in 10 steps, and each step adds
    5% bullet speed.
  - SUICIDE BULLETS is always ON (the menu option is kept and applies
    again in loop 1 and in a new game).
  - Bosses have more HP: each stage boss +50%, and the final boss
    (the stage 5 fortress) +100%. HP is capped so it never overflows.
  - Losing a life takes you back to loop 2's base difficulty, with
    its starting rank.
  - The RANK box title in the side panel shows LOOP 2. It is hidden
    in loop 1.
  - Game over ends the run. A new game starts at loop 1 with the
    DIFFICULTY you chose.
  - In Training, stage 1 still starts again after stage 5, but the
    difficulty and the multiplier do not change.

KRAKEN is the true last boss. It appears only after stage 5 of loop 2,
and only when you took every hidden medal in both loops and never lost
a ship. The recurring mid-boss (the ship that enters and leaves in
every stage, not the stage 5 fortress) flies off, waits, and returns
as only its central hull, named Kraken, with four segmented arms.
Each arm ends in a glowing orb and bends toward your ship. Kraken
cannot be hit until all four arms are destroyed. Each arm has half of
Kraken's hit points. Kraken fires every pattern the stage 5 fortress
fires, at twice the rate, and switches pattern every 10 seconds. The
arms fire homing shots: one at a time while four arms remain, two at
a time from random arms when three remain, one from each arm when two
remain, and very fast when one remains. Every 10 seconds one random
arm (the same arm can be picked again) stretches out to hit you, and
that arm does not shoot while it is stretched. The other attacks
continue. Destroying Kraken plays the ending.

The original debug cheat is still in the code. During the game, pause
with Escape and type N E K O C O. Then:

  2   toggle invincibility
  3   toggle the debug display
  1   hold to run without the frame wait (fast forward)
  4   jump to the ending

This is the original's hidden feature and is left as it was.


7. FRAME PACING AND VSYNC
-------------------------

The original game logic runs at about 30.67 frames per second. That
speed is kept on every PC, so the game is never faster than the
original.

The port waits for your monitor's refresh (vsync) when the graphics
driver allows it:

  SMOOTH (default)  On a 60, 120 or 240 Hz monitor each game frame
                    is shown for exactly 2, 4 or 8 refreshes. Motion
                    is perfectly even. The game then runs at 30.00
                    frames per second, about 2% slower than the
                    original, which you won't notice.

  EXACT             The game runs at exactly 30.67 frames per second
                    and every refresh shows the newest frame. Motion
                    has a slight regular unevenness on 60 Hz screens.

On other refresh rates (75, 144, 165 Hz...) the game always uses EXACT.
If vsync isn't available, a timer paces the game at 30.67.

F8 shows a small overlay with the mode, the measured refresh rate and
timing stats. The same information is written to pace.txt next to the
exe when the game starts and when you quit. It is useful if you report
a smoothness problem.


8. ORIGINAL OPTIONS REMOVED ON PURPOSE
--------------------------------------

The original options menu had some items that this port does not
show:

  AUTO SHOT, WAIT, PRIORITY, FRAME SKIP, SWAP KEYS

They were there to work around the speed and limits of the PC-98
hardware, or are covered by the PC controls. A modern PC runs the
game at full speed with no slowdown or frame skipping, the speed is
fixed by the port's pacing (section 7), and DEFINE KEYS replaces key
swapping. The original's separate INVINCIBLE and NAME ENTRY items are
replaced by TRAINING.


9. KNOWN LIMITATIONS
--------------------

  - Vsync and SMOOTH pacing have not yet been tested on every kind of
    monitor and graphics card. If motion is not smooth, try EXACT and
    send pace.txt.
  - This is a test build. Please report anything that looks or
    sounds different from the original.


10. REPORTING BUGS
------------------

When something looks, sounds or feels wrong, please send:

  1. What happened and what you expected instead.
  2. The steps to make it happen again (stage, mode, menu, what you
     pressed). Say if it happens every time or only sometimes.
  3. A screenshot or a short video, if you can.
  4. Your Windows or Linux version, graphics card, monitor refresh rate, and
     the controller you use (keyboard or which gamepad).
  5. The PACING setting (SMOOTH or EXACT) and the file pace.txt from
     the game folder (it is rewritten each time you quit, so copy it
     right after the problem).
  6. The build name: Rude Breaker 30th AE pr9ze.

Send it to Filipe Gracioli / shmupsbr.


11. CREDITS
-----------

  Rude Breaker (c) Compile, 1996. Original game, graphics, music and
               program.
  ymfm         YM2203 sound emulation by Aaron Giles, BSD 3-Clause
               license (see LICENSE-ymfm.txt).
  SDL2         Simple DirectMedia Layer, zlib license.
  PC port      Filipe Gracioli / shmupsbr.
  30th AE      (c) ShmupsBR 2026.

Panel font: Robotica by Courtney Novits (free font, dafont.com),
built into the program.
