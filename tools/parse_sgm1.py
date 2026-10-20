#!/usr/bin/env python3
"""Parse Rude Breaker SGM1.DAT TOC and dump chunks."""
from __future__ import annotations

import argparse
import struct
import sys
from pathlib import Path


def parse(path: Path, out_dir: Path | None) -> int:
    data = path.read_bytes()
    entries: list[tuple[int, int, int]] = []
    for i in range(38):
        bank, off = struct.unpack_from("<HH", data, i * 4)
        abs_off = bank * 0x10000 + off
        if i > 0 and abs_off >= len(data):
            break
        if bank == 0 and off == 0 and i > 0:
            break
        entries.append((i, bank, abs_off))

    print(f"SGM1 size={len(data)} chunks={len(entries)}")
    for idx, (i, bank, abs_off) in enumerate(entries):
        end = entries[idx + 1][2] if idx + 1 < len(entries) else len(data)
        size = max(0, end - abs_off)
        head = data[abs_off : abs_off + 8].hex() if abs_off < len(data) else ""
        print(f"  [{i:02d}] bank={bank} @{abs_off:06x} size={size:6d} head={head}")
        if out_dir is not None and size > 0:
            out_dir.mkdir(parents=True, exist_ok=True)
            (out_dir / f"chunk_{i:02d}.bin").write_bytes(data[abs_off:end])
    return 0


def main() -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("sgm1", type=Path)
    ap.add_argument("out_dir", type=Path, nargs="?", default=None)
    args = ap.parse_args()
    if not args.sgm1.is_file():
        print(f"missing {args.sgm1}", file=sys.stderr)
        return 1
    return parse(args.sgm1, args.out_dir)


if __name__ == "__main__":
    raise SystemExit(main())
