#!/usr/local/bin/python

import getopt
import math
import sys
from functools import reduce

from PIL import Image


def main(argv):
    palette = 6

    try:
        opts, args = getopt.getopt(argv[1:], "p:i:o:", ["pal=", "in=", "out="])
    except getopt.GetoptError:
        print('convert.py -p <0-5> -i <inputfile> -o <outputfile>')
        sys.exit(2)
    input_file = ''
    output_file = ''
    for opt, arg in opts:
        if opt == '-h':
            print('test.py -i <inputfile> -o <outputfile>')
            sys.exit()
        elif opt in ("-p", "--pal"):
            palette = int(arg)
        elif opt in ("-i", "--in"):
            input_file = arg
        elif opt in ("-o", "--out"):
            output_file = arg

    image = Image.open(input_file)  # Can be many different formats.
    width = image.size[0]
    height = image.size[1]

    if width > 255 or height > 255:
        raise ValueError('Image is too large')

    bpf = {
        0: lambda w, h: (w * h) >> 3,
        1: lambda w, h: (w * h) >> 1,
        2: lambda w, h: (w * h) >> 1,
        3: lambda w, h: (w * h) >> 1,
        4: lambda w, h: w * h,
        5: lambda w, h: w * h,
        6: lambda w, h: (w * h) + ((w * h) >> 1)
    }[palette](width, height)

    buffer = [0 for _ in range(bpf + 3 + 1)]
    buffer[0] = width
    buffer[1] = height
    buffer[2] = palette

    bits = {
        0: lambda w, h: (w * h) & 0x7,
        1: lambda w, h: ((w * h) & 0x1) * 4,
        2: lambda w, h: ((w * h) & 0x1) * 4,
        3: lambda w, h: ((w * h) & 0x1) * 4,
        4: lambda w, h: 0,
        5: lambda w, h: 0,
        6: lambda w, h: ((w * h) & 0x1) << 2
    }[palette](width, height)

    if bits != 0:
        bpf = bpf + 1  # optional padded byte at the front of the frame
    bpf = bpf + 1  # footer

    color_list = colors(palette)

    for f in range(1):  # TODO add support for multi-frame GIFs
        byte = f * bpf + 3
        bit = 7 if bits == 0 else bits - 1  # current bit in the current byte
        pixels = image.load()

        for y in range(height):
            for x in range(width):
                rgb = pixels[x, y]
                rgbx = nearest(rgb, color_list)
                c = rgbx[3]

                if palette == 0:  # 1-bit
                    buffer[byte] |= c << bit
                    bit = bit - 1
                    if bit < 0:
                        bit = 7
                        byte = byte + 1
                elif palette < 4:  # 4-bit
                    buffer[byte] |= c << 4 if bit == 7 else c & 0xf
                    bit = bit - 4
                    if bit < 0:
                        bit = 7
                        byte = byte + 1
                elif palette < 6:  # 8-bit
                    buffer[byte] = c
                    byte = byte + 1
                else:  # 12-bit
                    buffer[byte] |= (c << 4) & 0xf if bit == 7 else c & 0xf
                    bit = bit - 4
                    if bit < 0:
                        bit = 7
                        byte = byte + 1
                    buffer[byte] |= (c << 4) & 0xf if bit == 7 else c & 0xf
                    bit = bit - 4
                    if bit < 0:
                        bit = 7
                        byte = byte + 1
                    buffer[byte] |= (c << 4) & 0xf if bit == 7 else c & 0xf
                    bit = bit - 4
                    if bit < 0:
                        bit = 7
                        byte = byte + 1

        buffer[byte] = 0
        byte = byte + 1

    out = open(output_file, 'wb')
    out.write(bytes(buffer))


def distance(a, b):
    pr = .2126
    pg = .7152
    pb = .0722

    dr = a[0] - b[0]
    dg = a[1] - b[1]
    db = a[2] - b[2]

    return math.sqrt((pr * dr * dr) + (pg * dg * dg) + (pb * db * db))


def nearest(color, color_list):
    return reduce(lambda x, y: x if distance(x, color) < distance(y, color) else y, color_list)


def d(value):
    return value << 4 | value


def colors(palette):
    return list({
        0: lambda: map(lambda i: c0(i), range(2)),
        1: lambda: map(lambda i: c1(i), range(16)),
        2: lambda: map(lambda i: c2(i), range(16)),
        3: lambda: map(lambda i: c3(i), range(16)),
        4: lambda: map(lambda i: c4(i), range(256)),
        5: lambda: map(lambda i: c5(i), range(256)),
        6: lambda: map(lambda i: c6(i), range(4096))
    }[palette]())


def c0(i):
    return [0, 0, 0, i] if i == 0 else [255, 255, 255, i]


def c1(i):
    return [d(i), d(i), d(i), i]


def c2(i):
    r = ((i >> 2) & 0x3)
    r = (r << 2) + r
    g = ((i >> 0) & 0x3)
    g = (g << 2) + g
    return [d(r), d(g), 0, i]


def c3(i):
    if i == 6:
        return [0xaa, 0x55, 0x00, 6]
    else:
        n = ((i >> 3) & 0x1) * 0x5
        r = ((i >> 2) & 0x1) * 0x8
        g = ((i >> 1) & 0x1) * 0x8
        b = ((i >> 0) & 0x1) * 0x8
        if r > 0:
            r = r + 2
        if g > 0:
            g = g + 2
        if b > 0:
            b = b + 2
        return [d(r + n), d(g + n), d(b + n), i]


def c4(i):
    n = ((i >> 6) & 0x3) * 1
    r = ((i >> 4) & 0x3) * 4
    g = ((i >> 2) & 0x3) * 4
    b = ((i >> 0) & 0x3) * 4
    return [d(r + n), d(g + n), d(b + n), i]


def c5(i):
    r = ((i >> 5) & 0x7) * 2 + 1
    g = ((i >> 2) & 0x7) * 2 + 1
    b = ((i >> 0) & 0x3) * 4 + 3
    return [d(r), d(g), d(b), i]


def c6(i):
    r = (i >> 8) & 0xf
    g = (i >> 4) & 0xf
    b = (i >> 0) & 0xf
    return [d(r), d(g), d(b), i]


main(sys.argv)
