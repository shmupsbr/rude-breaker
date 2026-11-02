RUDE BREAKER 30th AE - PC PORT - USER MANUAL
============================================

Build pr9ze - 2026-10-31

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
RESUME, CONFIG and EXIT. RESUME (or Escape/START/B again) goes back to
the game. CONFIG opens GAMEPLAY, GRAPHICS, SOUND and CONTROLS; changes
take effect at once and BACK returns to the pause menu. EXIT ends the
game and goes back to the title sequence (Compile logo first).
The pause menu is drawn over the frozen game picture, which keeps the
COLOR FILTER and CRT effects on. The pause menu and all CONFIG pages
work on the pad too: d-pad or stick to move, A to confirm, B to go
back one page, START to resume.
On the title, in the attract demo and in the menus it does not pause.
In the menus, Escape (keyboard) or the B button (pad) goes back one
page.

The window can be resized. The picture keeps the original 640x400
aspect.


4. TITLE SCREEN AND CONFIG MENU
-------------------------------

The logo, title, ranking display and intro are the original screens.
Before them, a silent card plays: particles fly in and form the Rude
Breaker logo, it holds, then "30TH ANNIVERSARY EDITION" (in white,
spread across the width of the logo, with one soft glow once it has
landed) and the credit line slide up from below; after about 2.5
seconds in all the
card fades out and the original Compile logo
and its music start. Any key or gamepad button skips the card and
goes straight to the Compile logo. With SIDE
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

CONFIG has these pages: GAMEPLAY, GRAPHICS, SOUND, CONTROLS, RANKING,
PASSWORD and BACK. Settings keep their old names in rudebreaker.cfg,
so saves from earlier builds load unchanged.

GAMEPLAY

  Options, in this order: DIFFICULTY, RANK, SUICIDE BULLETS, SHIP
  SELECT, SHIP SPEED, TRAINING, LOOP, ATTRACT MODE and BACK.

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

  SHIP SPEED       STANDARD (default) or AGGRESSIVE. STANDARD: LOW,
                   MID and MAX move the ship as in the original.
                   AGGRESSIVE: LOW is 15% slower, MID is the same and
                   MAX is 15% faster. Every game starts at MID; the
                   in-game speed button still switches LOW / MID /
                   MAX. The attract demo always uses the original
                   movement. Saved in rudebreaker.cfg as ship_speed.

  TRAINING         ON: practice mode (see section 5). OFF: normal game.

  LOOP             ON (default) or OFF. OFF: the game ends after
                   stage 5 with the ending, as in the original. ON: clearing stage 5
                   starts stage 1 again as loop 2, then loop 3 and so
                   on, keeping your score, lives and weapons. Each loop
                   is harder (see section 6). Saved in rudebreaker.cfg.

  ATTRACT MODE     ON (default) or OFF. ON: after the ranking screen a
                   30-second demo plays (no music, sound effects only),
                   then the Compile logo again. OFF: no demo. Saved in
                   rudebreaker.cfg.

  BACK

GRAPHICS

  Options, in this order: FULLSCREEN, SCALING, ASPECT, ORIENTATION, SIDE
  PANELS, PALETTE, BULLET COLOR, HITBOX, COLOR FILTER, the six CRT
  effects, FRAME PACING and BACK.

  FULLSCREEN       OFF (default) or ON. Fills the whole monitor,
                   keeping the game's proportions (SIDE PANELS fill the
                   sides on wide screens) and hiding the mouse pointer.
                   F11 switches it at any time. Saved in
                   rudebreaker.cfg.

  SCALING          How the picture is scaled to the window or monitor.
                   AUTO (default): INTEGER when it fills at least 80%
                   of the screen height, otherwise SHARP. That gives
                   SHARP at 720p and 1080p, INTEGER at 1440p, 4K and
                   800x480.
                   INTEGER: every game line becomes the same whole
                   number of screen lines, with black borders around.
                   With ASPECT 1:1 the pixels are square and all the
                   same size. With ASPECT 4:3 the height is a whole
                   multiple (2x = 800 lines, 3x = 1200) and the width
                   is set to close the 4:3 shape.
                   SHARP: fills as much of the screen as possible
                   keeping the proportions, without blur (some pixels
                   come out one screen pixel bigger). SMOOTH: fills
                   the same way with soft (bilinear) filtering. If the
                   window is too small for INTEGER, it acts as SHARP.
                   With SIDE PANELS shown the 16:9 picture always
                   fills the screen. Saved in rudebreaker.cfg.

  ASPECT           4:3 (default): the 640x400 picture is shown with
                   the shape of a PC-98 monitor, each pixel 1.2 times
                   taller than wide (5:6), as the art was drawn to be
                   seen. On a 1080p screen that is 1440x1080 with
                   SHARP. 1:1: square pixels; the picture looks a
                   little flatter than on the original. With SIDE
                   PANELS the playfield follows the same choice (1:1
                   leaves black bars above and below it). With 1:1 the
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
                   taller than wide); 1:1 uses square pixels.
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
                   From loop 2 on (LOOP ON) the RANK box title also
                   shows the loop: LOOP 2, LOOP 3 ... It is hidden in
                   loop 1 and with LOOP OFF.
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

  HITBOX           OFF (default), PINK, LILAC, BLUE, GREEN, RED or
                   WHITE. Draws a dot on the ship with the exact size
                   of its real collision box (12 x 18 pixels), so you
                   can see what can be hit. The dot never uses the
                   same colour as the enemy bullets: that colour is
                   skipped, and if you change BULLET COLOR to the
                   hitbox colour, the hitbox moves on to the next one.
                   RED is skipped with BULLET COLOR ORIGINAL (the
                   original bullets are red). Saved in rudebreaker.cfg.

  COLOR FILTER     ORIGINAL (default), GAME BOY, VIRTUAL BOY, COMMODORE 64,
                   GAME GEAR, PC ENGINE. Recolours only the playfield, and
                   only while a game (or the attract demo) is running:
                   title, menus, ranking, name entry and the side panels
                   always keep their real colours. The game's art and its
                   palette are untouched, and PALETTE still works
                   underneath. These filters are for fun: each one
                   exaggerates its machine's look. GAME BOY: DMG greens by
                   brightness, with the slow LCD (moving things leave a
                   ghost) and the visible dot grid. VIRTUAL BOY: shades of
                   red on black, with the short glow of the LEDs and the
                   dark gaps between LED columns. COMMODORE 64: the 16 C64
                   colours (Pepto), wide pixels, 3 colours plus black per
                   cell for the scenery; sprites, bullets and items keep
                   their own colours, and PAL blending (each line mixes with
                   the one above). GAME GEAR: colour version of the GAME BOY
                   look: 4096-colour palette (4 bits per channel) with at
                   most 32 colours on screen (16 background + 16 sprite),
                   chosen each frame, big crisp pixels (about 160x144
                   detail), slow LCD with ghosting, a faint pixel grid and
                   slightly washed backlit colours. PC ENGINE: strong,
                   saturated and bright colours (9-bit, 512), 16 colours per
                   tile, no dithering, only a slight composite softness.
                   Saved in rudebreaker.cfg as color_filter (old values of
                   filters that were removed, such as spectrum, msx1, msx2,
                   nes, mastersystem or megadrive, now load as ORIGINAL).

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

  Options: SOUND CHANNELS, SOUND TEST and BACK.

  SOUND CHANNELS   EXTRA (default): effects play on a second emulated
                   chip, so the music is never cut. ORIGINAL: sound
                   effects share the FM chip with the music, as on
                   the PC-98, so an effect can cut a music channel
                   for a moment. Saved in rudebreaker.cfg.

  SOUND TEST       Opens the SOUND TEST page (below).

  BACK

CONTROLS

  JOYSTICK         Shows whether a gamepad is in use, and its name.
                   Change it with left/right.
  DEFINE KEYS      Remap UP, DOWN, LEFT, RIGHT, FIRE, MODE, SHIFT,
                   PAUSE and SPEED. Select an action,
                   then press the new key or
                   pad button. RESTORE DEFAULTS puts back the keys
                   listed in section 3.
  BACK

SOUND TEST (opened from SOUND)

  SOUND            Music and sound effects ON or OFF.
  BGM              Choose a music track (0-47) with left/right; Fire
                   plays it.
  SE               Choose a sound effect (0-47) with left/right; Fire
                   plays it.
  PLAY / PAUSE / STOP   Player controls.
  The bars at the bottom are a VU meter of what is playing.
  (Some numbers are silent: they are empty in the original data.)

RANKING            Opens the original hi-score table.

PASSWORD           Opens the original password screen.

BACK               Returns to the title.


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
With LOOP, each new loop starts rank 100 higher and allows it 50
higher (at most 1000), and rank rises the loop number times faster
(see LOOP below).
Not in Training. RANK OFF turns all of this off.

LOOP (GAMEPLAY option, ON by default) is an addition of this port.
With LOOP ON, clearing stage 5 does not show the ending: stage 1
starts again as loop 2, then loop 3 and so on, and you keep your
score, lives and weapons. In each new loop:

  - The base difficulty is one level above the loop before, starting
    from the DIFFICULTY you chose (VERY EASY -> EASY -> NORMAL -> HARD
    -> VERY HARD). At VERY HARD it stays VERY HARD and only the loop
    multiplier below keeps growing. The new level, with the game's
    own settings for it (enemy bullets and enemy strength), takes
    effect when stage 1 of the loop starts.
  - Rank starts again at the new base's starting point. The step-up
    ceiling, the rank limits and the speed-up at the ceiling follow
    the base the same way they follow your DIFFICULTY in loop 1 (for
    example a NORMAL game is HARD in loop 2, so it can step up to VERY
    HARD there).
  - Every value driven by rank is multiplied by the loop number: x1 in
    loop 1, x2 in loop 2, x3 in loop 3 and so on. That is how fast rank
    rises, how much rank adds to enemy bullet speed, and the +1% steps
    of the speed-up at the ceiling. Limits: the multiplier stops at x8
    (loop 8 and later); enemy bullet speed never goes past the game's
    own VERY HARD value; the speed-up at the ceiling goes up to +25%
    times the multiplier, but never past +50%.
  - From loop 2 on (not in Training), rank gain is multiplied by the
    loop number in both rank modes, and enemy bullets get faster with
    rank inside the loop: the rank range of the loop (start to
    ceiling) is split in 10 steps, and each step adds 5% bullet speed
    times (loop - 1): +5% per step in loop 2, +10% in loop 3 and so
    on, up to +100% at most.
  - From loop 2 on, SUICIDE BULLETS is always ON (the menu option is
    kept and applies again in loop 1 / a new game).
  - From loop 2 on, bosses have more HP: each stage boss +50% per
    extra loop (x1.5 in loop 2, x2 in loop 3 ...), and the final boss
    (the stage 5 fortress) +100% per extra loop (x2 in loop 2, x3 in
    loop 3 ...). The loop multiplier stops at x8 and HP is capped so
    it never overflows.
  - Losing a life takes you back to the loop's base difficulty, with
    its starting rank.
  - From loop 2 on, the RANK box title in the side panel shows the
    loop (LOOP 2, LOOP 3 ...); it is hidden in loop 1. The count stops
    at 99.
  - Game over ends the run; a new game starts at loop 1 with the
    DIFFICULTY you chose.
  - In Training, LOOP ON still starts stage 1 again after stage 5, but
    the difficulty and the multiplier do not change.

With LOOP OFF the game ends after stage 5 exactly as in the original.

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
