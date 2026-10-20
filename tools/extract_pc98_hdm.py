#!/usr/bin/env python3
"""Extract files from a PC-98 2HD (1024 B/sector) FAT12 .hdm / .hdi image."""
from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path


def fat12_get(fat: bytes, cluster: int) -> int:
    offset = cluster + (cluster // 2)
    if offset + 1 >= len(fat):
        return 0xFFF
    val = fat[offset] | (fat[offset + 1] << 8)
    if cluster & 1:
        return val >> 4
    return val & 0xFFF


def read_chain(img: bytes, data_start: int, spc: int, bps: int, fat: bytes,
               start: int) -> bytes:
    out = bytearray()
    c = start
    seen: set[int] = set()
    while 0x002 <= c <= 0xFEF:
        if c in seen:
            break
        seen.add(c)
        off = data_start + (c - 2) * spc * bps
        out += img[off : off + spc * bps]
        c = fat12_get(fat, c)
    return bytes(out)


def parse_dir(img: bytes, blob: bytes, data_start: int, spc: int, bps: int,
              fat: bytes, prefix: str = "") -> list[tuple]:
    entries: list[tuple] = []
    for i in range(0, len(blob), 32):
        e = blob[i : i + 32]
        if not e or e[0] == 0x00:
            break
        if e[0] == 0xE5 or e[11] == 0x0F:
            continue
        name = e[0:8].decode("cp437", "replace").rstrip()
        ext = e[8:11].decode("cp437", "replace").rstrip()
        if name.startswith("."):
            continue
        fname = f"{name}.{ext}" if ext else name
        attr = e[11]
        cluster = struct.unpack_from("<H", e, 26)[0]
        size = struct.unpack_from("<I", e, 28)[0]
        path = prefix + fname
        entries.append((path, attr, cluster, size))
        if attr & 0x10:
            sub = read_chain(img, data_start, spc, bps, fat, cluster)
            entries.extend(
                parse_dir(img, sub, data_start, spc, bps, fat, path + "/")
            )
    return entries


def extract(image: Path, out_dir: Path) -> int:
    img = image.read_bytes()
    bps = struct.unpack_from("<H", img, 0x0B)[0]
    if bps not in (256, 512, 1024):
        print(f"unexpected bytes/sector: {bps}", file=sys.stderr)
        return 1
    spc = img[0x0D]
    rsec = struct.unpack_from("<H", img, 0x0E)[0]
    nfats = img[0x10]
    nroot = struct.unpack_from("<H", img, 0x11)[0]
    spf = struct.unpack_from("<H", img, 0x16)[0]

    fat_start = rsec * bps
    fat_size = spf * bps
    root_start = fat_start + nfats * fat_size
    root_size = nroot * 32
    data_start = root_start + root_size
    fat = img[fat_start : fat_start + fat_size]
    root = img[root_start : root_start + root_size]

    out_dir.mkdir(parents=True, exist_ok=True)
    for path, attr, cluster, size in parse_dir(
        img, root, data_start, spc, bps, fat
    ):
        dest = out_dir / path
        if attr & 0x08:
            continue
        if attr & 0x10:
            dest.mkdir(parents=True, exist_ok=True)
            print(f"dir  {path}")
            continue
        data = read_chain(img, data_start, spc, bps, fat, cluster)[:size]
        dest.parent.mkdir(parents=True, exist_ok=True)
        dest.write_bytes(data)
        print(f"file {path} ({len(data)} bytes)")
    return 0


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("image", type=Path, help="PC-98 .hdm / .hdi path")
    ap.add_argument("out_dir", type=Path, help="output directory")
    args = ap.parse_args()
    return extract(args.image, args.out_dir)


if __name__ == "__main__":
    raise SystemExit(main())
