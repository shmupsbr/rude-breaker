#!/usr/bin/env python3
"""Curate a controller-mapping subset from an SDL gamecontrollerdb.

Reads the community database (zlib license) and writes a C include of
the Windows, macOS and Linux lines for the pads this game autodetects.
The output is an altered subset, not the original database.

  python3 tools/curate_gc_db.py gamecontrollerdb.txt src/hal/sdl2/gc_db.inc
"""
import sys

PLATFORMS = {"Windows", "Mac OS X", "Linux"}

# Name fragments, lower case. A line is kept when its controller name
# contains one of these. Pi GPIO boards and DragonRise sticks are not
# in the upstream list; those stay on the unmapped fallback.
KEYS = (
    "8bitdo",
    "8bit do",
    "dualshock",
    "dualsense",
    "dual sense",
    "ps3 controller",
    "ps4",
    "ps5",
    "playstation",
    "xbox",
    "xinput",
    "x-box",
    "wii",
    "mayflash",
    "magic ns",
    "magic-ns",
    "arcade",
    "fightstick",
    "fight stick",
    "fighting stick",
    "fighting commander",
    "hitbox",
    "qanba",
    "panthera",
    "victrix",
    "etokki",
    "brook",
    "hori real arcade",
    "hori fight",
    "horipad",
    "hori pad",
    "switch pro",
    "joy-con",
    "joycon",
    "dragonrise",
    "gpio",
    "raspberry",
    "picade",
    "retroflag",
    "xinmo",
    "xin-mo",
    "zero delay",
    "generic usb",
    "usb gamepad",
    "usb joystick",
    "usb controller",
    "pc controller",
    "sony dual",
    "sony playstation",
)


def name_of(line):
    parts = line.split(",")
    i = 1
    while i < len(parts) and ":" not in parts[i]:
        i += 1
    return ",".join(parts[1:i])


def platform_of(line):
    return line.rsplit("platform:", 1)[-1].strip().rstrip(",")


def wanted(name):
    n = name.lower()
    return any(k in n for k in KEYS)


def c_escape(s):
    return s.replace("\\", "\\\\").replace('"', '\\"')


NOTICE = r"""/* Curated subset of the SDL Game Controller DB.
 * Altered: only Windows, macOS and Linux lines whose names match the
 * pads Rude Breaker autodetects (PlayStation, Xbox, Wii and Wii U Pro
 * adapters, 8BitDo, arcade sticks, common USB pads). This is not the
 * original database.
 *
 * Source: https://github.com/mdqinc/SDL_GameControllerDB
 * License: zlib. The notice below is unchanged from that distribution.
 *
 * Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */
"""


def main():
    if len(sys.argv) != 3:
        sys.stderr.write("usage: curate_gc_db.py gamecontrollerdb.txt out.inc\n")
        return 2
    text = open(sys.argv[1], encoding="utf-8", errors="replace").read().splitlines()
    kept = []
    seen = set()
    for raw in text:
        line = raw.strip()
        if not line or line.startswith("#") or ",platform:" not in line:
            continue
        if len(line) < 32 or not all(c in "0123456789abcdefABCDEF" for c in line[:32]):
            continue
        plat = platform_of(line)
        if plat not in PLATFORMS:
            continue
        name = name_of(line)
        if not wanted(name):
            continue
        if line in seen:
            continue
        seen.add(line)
        kept.append((plat, name.lower(), line[:32], line))
    kept.sort()
    out = open(sys.argv[2], "w", encoding="utf-8", newline="\n")
    out.write(NOTICE)
    out.write("static const char *const rb_gc_map[] = {\n")
    for _, _, _, line in kept:
        out.write('"%s",\n' % c_escape(line))
    out.write("};\n")
    out.write("static const int rb_gc_map_count = (int)(sizeof rb_gc_map / sizeof rb_gc_map[0]);\n")
    out.close()
    sys.stderr.write("curated %d mappings\n" % len(kept))
    return 0


if __name__ == "__main__":
    sys.exit(main())
