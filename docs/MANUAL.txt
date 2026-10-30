RUDE BREAKER 30th AE - PC PORT - USER MANUAL
============================================

Build pr9p tester build - 2026-10-29


1. WHAT THIS IS
---------------

This is a Windows PC port of Rude Breaker, the vertical shoot 'em up
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
  - New title screen with the 30th AE mark and a 2026 ShmupsBR credit.

The port does not remake the game. It runs the original game scripts
and logic from the original data files. Enemy patterns, scoring, boss
HP and difficulty all come from the original code. Music is the
original FPLAY driver playing on an emulated YM2203 (ymfm), the
PC-98 FM sound chip the game was written for.

The PC side handles the window, sound output, keyboard and gamepad,
frame pacing, and a set of options in the CONFIG menu.

IMPORTANT: the game data (SGM1.DAT, SONG.DAT, FPLAY.COM) is
copyrighted. It is NOT part of the public port and must not be
redistributed. This build includes it only
because it is a private test build. Please do not share it.


2. INSTALLING AND RUNNING
-------------------------

Unzip the folder anywhere and run rudebreaker.exe. Nothing needs to be
installed.

Files in the folder:

  rudebreaker.exe       The game.
  SDL2.dll              Window, sound and gamepad library. Required.
  SGM1.DAT              Original stages, sprites and scripts. Required.
  SONG.DAT              Original music data. Required for music.
  FPLAY.COM             Original music driver. Required for music.
  LICENSE-ymfm.txt      License of the ymfm sound chip emulator.
  LICENSE-SDL2.txt      License of the SDL2 library.
  MANUAL.txt / .md      This manual.

Files the game creates next to the exe:

  RBRANK.DAT       The original save file: the hi-score table and the
                   game settings (difficulty and the GAMEPLAY options).
                   It is created with the original table if missing.
                   Delete it to reset scores and settings.
  rudebreaker.cfg  PC options: palette, rank, training, key and pad
                   bindings.
  rudebreaker.ini  Frame pacing choice: pacing=smooth or pacing=exact.
  pace.txt         A small timing report (see section 7). Safe to
                   delete.


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

Each game starts at the MOVE SPEED chosen in GAMEPLAY. A change made
with the button lasts until the game ends; it does not change the
MOVE SPEED setting.

The top bar shows the current speed as "SPD" with 1, 2 or 3 arrows,
between the score and HI.

PAUSE: Escape (or Start) pauses only while you are playing a game.
On the title, in the attract demo and in the menus it does not pause.
In the menus, Escape (keyboard) or the B button (pad) goes back one
page.

The window can be resized. The picture keeps the original 640x400
aspect.


4. TITLE SCREEN AND CONFIG MENU
-------------------------------

The logo, title, ranking display and intro are the original screens.
The title screen shows the 30th AE mark under the logo and the
credit lines (c) COMPILE 1996 and (c) 2026 SHMUPSBR.
Press Fire at the title to start, or open CONFIG from the title menu.

CONFIG has these pages:

GAMEPLAY

  DIFFICULTY       VERY EASY, EASY, NORMAL, HARD, VERY HARD. These are
                   the original game's own difficulty levels. NORMAL
                   is the original default. Saved in RBRANK.DAT.

  SUICIDE BULLETS  ON: enemies fire bullets back when they are
                   destroyed, as in the original option. OFF: they don't.

  SHIP SELECT      The original game's option of the same name. It
                   controls how weapons work:
                   ON: at the start you pick one weapon (VULCAN, LASER
                   or MISSILE) on the "PLEASE SELECT YOUR OPTION"
                   screen. After that every weapon capsule is that
                   weapon, so you keep it for the whole game.
                   OFF (default): there is no pick. Capsules come as V,
                   L and M, and you change weapon during the game by
                   picking up a capsule of another letter.

  MOVE SPEED       LOW, MID (default), MAX. The ship's movement
                   speed, as in the original. Each game starts at it.

  RANK             ON (default): enemy bullets get faster or slower
                   with how you play (see section 6). OFF: bullet
                   speed stays fixed, as in the original.

  PALETTE          MODERN (default): Filipe's color rebalance of the
                   game. ORIGINAL: the PC-98 colors.

  TRAINING         ON: practice mode (see section 5). OFF: normal game.

  FRAME PACING     SMOOTH or EXACT (see section 7).

  BACK

CONTROLS

  JOYSTICK         Shows whether a gamepad is in use, and its name.
                   Change it with left/right.
  DEFINE KEYS      Remap UP, DOWN, LEFT, RIGHT, FIRE, MODE, SHIFT,
                   PAUSE and SPEED. Select an action, then press the new key or
                   pad button. RESTORE DEFAULTS puts back the keys
                   listed in section 3.
  BACK

SOUND TEST

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

RANK (GAMEPLAY option, ON by default) is an addition of this port,
modeled on the rank system of Zanac. A hidden rank value goes up
while you stay alive and when you score points, and is cut in half
each time you lose a life. A higher rank speeds up enemy bullets,
the same way a higher DIFFICULTY does; nothing else changes, and the
bonuses still follow the DIFFICULTY you chose. DIFFICULTY sets the
starting point and the floor: rank never makes bullets slower than
the difficulty you chose. On VERY EASY and EASY rank can only go half
way up; on HARD and VERY HARD it starts half way up. Rank starts
from zero with each new game. With RANK OFF the game plays exactly
like the original.

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
exe when the game starts and when you quit. It is useful if you report a smoothness problem.


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
  4. Your Windows version, graphics card, monitor refresh rate, and
     the controller you use (keyboard or which gamepad).
  5. The PACING setting (SMOOTH or EXACT) and the file pace.txt from
     the game folder (it is rewritten each time you quit, so copy it
     right after the problem).
  6. The build name: Rude Breaker 30th AE pr9p.

Send it to Filipe Gracioli / shmupsbr.


11. CREDITS
-----------

  Rude Breaker (c) Compile, 1996. Original game, graphics, music and
               program.
  ymfm         YM2203 sound emulation by Aaron Giles, BSD 3-Clause
               license (see LICENSE-ymfm.txt).
  SDL2         Simple DirectMedia Layer, zlib license.
  PC port      Filipe Gracioli / shmupsbr.
  30th AE      (c) 2026 ShmupsBR.
