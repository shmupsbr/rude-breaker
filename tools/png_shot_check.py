#!/usr/bin/env python3
"""Decode the test PNGs with zlib and check pixels and the _n filename."""
import glob
import os
import struct
import sys
import zlib


def read_chunk(data, at):
    ln = struct.unpack(">I", data[at : at + 4])[0]
    typ = data[at + 4 : at + 8]
    body = data[at + 8 : at + 8 + ln]
    crc = struct.unpack(">I", data[at + 8 + ln : at + 12 + ln])[0]
    calc = zlib.crc32(typ + body) & 0xFFFFFFFF
    if calc != crc:
        raise SystemExit("bad crc %s" % typ)
    return typ, body, at + 12 + ln


def decode(path):
    data = open(path, "rb").read()
    if data[:8] != b"\x89PNG\r\n\x1a\n":
        raise SystemExit("bad signature " + path)
    at = 8
    ihdr = None
    raw = b""
    while at < len(data):
        typ, body, at = read_chunk(data, at)
        if typ == b"IHDR":
            ihdr = body
        elif typ == b"IDAT":
            raw += body
        elif typ == b"IEND":
            break
    w, h, depth, color = struct.unpack(">IIBB", ihdr[:10])
    if depth != 8 or color != 2:
        raise SystemExit("not rgb8")
    img = zlib.decompress(raw)
    stride = 1 + w * 3
    if len(img) != h * stride:
        raise SystemExit("size %d != %d" % (len(img), h * stride))
    px = []
    for y in range(h):
        row = img[y * stride : (y + 1) * stride]
        if row[0] != 0:
            raise SystemExit("filter")
        for x in range(w):
            i = 1 + x * 3
            px.append((row[i], row[i + 1], row[i + 2]))
    return w, h, px


def main():
    small, big, folder = sys.argv[1:]
    w, h, px = decode(small)
    if (w, h) != (3, 2):
        raise SystemExit("small geom")
    expect = [
        (0x11, 0x22, 0x33),
        (0x44, 0x55, 0x66),
        (0x77, 0x88, 0x99),
        (0xAA, 0xBB, 0xCC),
        (0x01, 0x02, 0x03),
        (0xFE, 0xFD, 0xFC),
    ]
    if px != expect:
        raise SystemExit("small pixels %r" % px)
    w, h, px = decode(big)
    if (w, h) != (400, 200):
        raise SystemExit("big geom")
    for y in range(h):
        for x in range(w):
            r, g, b = px[y * w + x]
            if (r, g, b) != (x & 255, y & 255, (x + y) & 255):
                raise SystemExit("big pixel %d,%d" % (x, y))
    names = sorted(os.path.basename(p) for p in glob.glob(os.path.join(folder, "screenshots", "*.png")))
    if len(names) != 2:
        raise SystemExit("expected 2 shots, got %r" % names)
    if not names[0].startswith("RudeBreaker_") or not names[0].endswith(".png"):
        raise SystemExit(names[0])
    if "_2.png" not in names[1]:
        raise SystemExit("second shot should be _2: %r" % names)
    # Both share the timestamp stem.
    stem = names[0][: -len(".png")]
    if names[1] != stem + "_2.png":
        raise SystemExit("names %r" % names)
    print("png check ok", names[0], names[1])


if __name__ == "__main__":
    main()
