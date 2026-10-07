#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
light-image-tool 测试: 用纯标准库生成已知 BMP, 调用 bin/lit.exe 各子命令,
再独立解析输出 BMP 校验像素变换是否正确。
"""
import os, struct, subprocess, sys, re

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LIT = os.path.join(ROOT, "bin", "lit.exe")
OUT = os.path.join(ROOT, "test", "out")
os.makedirs(OUT, exist_ok=True)


# ---------------- BMP 读写(纯 stdlib) ----------------
def write_bmp_24(path, rows):
    H = len(rows); W = len(rows[0])
    stride = (W * 3 + 3) & ~3
    pixel_size = stride * H
    palette_size = 0
    data_offset = 14 + 40 + palette_size
    filesize = data_offset + pixel_size
    with open(path, "wb") as f:
        f.write(b"BM")
        f.write(struct.pack("<I", filesize))
        f.write(struct.pack("<I", 0))
        f.write(struct.pack("<I", data_offset))
        f.write(struct.pack("<I", 40))
        f.write(struct.pack("<i", W))
        f.write(struct.pack("<i", H))
        f.write(struct.pack("<H", 1))
        f.write(struct.pack("<H", 24))
        f.write(struct.pack("<I", 0))
        f.write(struct.pack("<I", pixel_size))
        f.write(struct.pack("<i", 2835))
        f.write(struct.pack("<i", 2835))
        f.write(struct.pack("<I", 0))
        f.write(struct.pack("<I", 0))
        for y in range(H - 1, -1, -1):  # 自底向上
            row = bytearray()
            for (r, g, b) in rows[y]:
                row += bytes((b, g, r))
            row += b"\x00" * (stride - W * 3)
            f.write(row)


def write_bmp_8(path, rows):
    H = len(rows); W = len(rows[0])
    stride = (W + 3) & ~3
    pixel_size = stride * H
    palette_size = 256 * 4
    data_offset = 14 + 40 + palette_size
    filesize = data_offset + pixel_size
    with open(path, "wb") as f:
        f.write(b"BM"); f.write(struct.pack("<I", filesize))
        f.write(struct.pack("<I", 0)); f.write(struct.pack("<I", data_offset))
        f.write(struct.pack("<I", 40))
        f.write(struct.pack("<i", W)); f.write(struct.pack("<i", H))
        f.write(struct.pack("<H", 1)); f.write(struct.pack("<H", 8))
        f.write(struct.pack("<I", 0)); f.write(struct.pack("<I", pixel_size))
        f.write(struct.pack("<i", 2835)); f.write(struct.pack("<i", 2835))
        f.write(struct.pack("<I", 256)); f.write(struct.pack("<I", 0))
        for i in range(256):
            f.write(bytes((i, i, i, 0)))  # 灰阶调色板
        for y in range(H - 1, -1, -1):
            row = bytearray(rows[y])
            row += b"\x00" * (stride - W)
            f.write(row)


def write_bmp_32(path, rows):
    H = len(rows); W = len(rows[0])
    stride = (W * 4 + 3) & ~3
    pixel_size = stride * H
    data_offset = 14 + 40
    filesize = data_offset + pixel_size
    with open(path, "wb") as f:
        f.write(b"BM"); f.write(struct.pack("<I", filesize))
        f.write(struct.pack("<I", 0)); f.write(struct.pack("<I", data_offset))
        f.write(struct.pack("<I", 40))
        f.write(struct.pack("<i", W)); f.write(struct.pack("<i", H))
        f.write(struct.pack("<H", 1)); f.write(struct.pack("<H", 32))
        f.write(struct.pack("<I", 0)); f.write(struct.pack("<I", pixel_size))
        f.write(struct.pack("<i", 2835)); f.write(struct.pack("<i", 2835))
        f.write(struct.pack("<I", 0)); f.write(struct.pack("<I", 0))
        for y in range(H - 1, -1, -1):
            row = bytearray()
            for (r, g, b, a) in rows[y]:
                row += bytes((b, g, r, a))
            row += b"\x00" * (stride - W * 4)
            f.write(row)


def read_bmp(path):
    with open(path, "rb") as f:
        data = f.read()
    assert data[:2] == b"BM", "not BMP"
    data_offset = struct.unpack_from("<I", data, 10)[0]
    info_size = struct.unpack_from("<I", data, 14)[0]
    W = struct.unpack_from("<i", data, 18)[0]
    H = struct.unpack_from("<i", data, 22)[0]
    bpp = struct.unpack_from("<H", data, 28)[0]
    pos = 14 + info_size
    if bpp == 8:
        pos += 256 * 4  # 跳过调色板
    rows = []
    if bpp == 24:
        stride = (W * 3 + 3) & ~3
        for y in range(H):
            line = data[pos + (H - 1 - y) * stride: pos + (H - 1 - y) * stride + W * 3]
            rows.append([(line[i*3+2], line[i*3+1], line[i*3]) for i in range(W)])
        return ("RGB", W, H, rows)
    if bpp == 8:
        stride = (W + 3) & ~3
        for y in range(H):
            line = data[pos + (H - 1 - y) * stride: pos + (H - 1 - y) * stride + W]
            rows.append([line[i] for i in range(W)])
        return ("GRAY", W, H, rows)
    if bpp == 32:
        stride = (W * 4 + 3) & ~3
        for y in range(H):
            line = data[pos + (H - 1 - y) * stride: pos + (H - 1 - y) * stride + W * 4]
            rows.append([(line[i*4+2], line[i*4+1], line[i*4], line[i*4+3]) for i in range(W)])
        return ("RGBA", W, H, rows)
    raise ValueError("unsupported bpp %d" % bpp)


# ---------------- 测试驱动 ----------------
fails = 0
def check(name, cond, extra=""):
    global fails
    if cond:
        print("  PASS  %s" % name)
    else:
        fails += 1
        print("  FAIL  %s   %s" % (name, extra))


def run(args):
    r = subprocess.run([LIT] + args, capture_output=True, text=True)
    return r.returncode, r.stdout.strip(), r.stderr.strip()


# 1) RGB 基础
W, H = 4, 3
src = [[((x * 37) % 256, (y * 53) % 256, ((x * 3 + y) * 17) % 256) for x in range(W)] for y in range(H)]
inp = os.path.join(OUT, "rgb_in.bmp")
write_bmp_24(inp, src)

rc, out, err = run(["info", inp])
m = re.search(r"(\d+)x(\d+), format=(\w+), stride=(\d+)", out)
check("info rc", rc == 0, err)
check("info dims", m and int(m.group(1)) == W and int(m.group(2)) == H, out)
check("info fmt", m and m.group(3) == "RGB", out)
check("info stride", m and int(m.group(4)) == W * 3, out)

# to-gray
gout = os.path.join(OUT, "rgb_gray.bmp")
run(["to-gray", inp, gout])
_, _, _, g = read_bmp(gout)
exp_g = [[int(0.299 * src[y][x][0] + 0.587 * src[y][x][1] + 0.114 * src[y][x][2])
          for x in range(W)] for y in range(H)]
check("to-gray values", g == exp_g, "%r vs %r" % (g, exp_g))

# flip-h
fh = os.path.join(OUT, "rgb_fliph.bmp")
run(["flip-h", inp, fh])
_, _, _, fh_img = read_bmp(fh)
exp_fh = [[src[y][W - 1 - x] for x in range(W)] for y in range(H)]
check("flip-h values", fh_img == exp_fh)

# flip-v
fv = os.path.join(OUT, "rgb_flipv.bmp")
run(["flip-v", inp, fv])
_, _, _, fv_img = read_bmp(fv)
exp_fv = [[src[H - 1 - y][x] for x in range(W)] for y in range(H)]
check("flip-v values", fv_img == exp_fv)

# rot90 (out: H'=W=4, W'=H=3); out[y][x] = src[H-1-x][y]
r90 = os.path.join(OUT, "rgb_rot90.bmp")
run(["rot90", inp, r90])
_, _, _, r_img = read_bmp(r90)
check("rot90 dims", (len(r_img), len(r_img[0])) == (W, H), "%dx%d" % (len(r_img), len(r_img[0])))
exp_r = [[src[H - 1 - x][y] for x in range(H)] for y in range(W)]
check("rot90 values", r_img == exp_r, "%r" % r_img)

# 2) GRAY 8 位
gw, gh = 5, 2
g8 = [[(x * 40 + y * 7) % 256 for x in range(gw)] for y in range(gh)]
gin = os.path.join(OUT, "gray_in.bmp")
write_bmp_8(gin, g8)
rc, out, err = run(["info", gin])
m = re.search(r"(\d+)x(\d+), format=(\w+)", out)
check("gray info fmt", m and m.group(3) == "GRAY", out)
gfh = os.path.join(OUT, "gray_fliph.bmp")
run(["flip-h", gin, gfh])
_, _, _, gfh_img = read_bmp(gfh)
exp_gfh = [[g8[y][gw - 1 - x] for x in range(gw)] for y in range(gh)]
check("gray flip-h values", gfh_img == exp_gfh, "%r vs %r" % (gfh_img, exp_gfh))

# 3) RGBA 32 位 往返
rw, rh = 3, 2
rgba = [[((x*30)%256, (y*60)%256, ((x+y)*20)%256, (x*10+y*5)%256) for x in range(rw)] for y in range(rh)]
rin = os.path.join(OUT, "rgba_in.bmp")
write_bmp_32(rin, rgba)
rc, out, err = run(["info", rin])
m = re.search(r"format=(\w+)", out)
check("rgba info fmt", m and m.group(1) == "RGBA", out)
# 用 flip-h 做一次变换后比较(等价于 RGBA 通道保留)
rfh = os.path.join(OUT, "rgba_fliph.bmp")
run(["flip-h", rin, rfh])
_, _, _, rfh_img = read_bmp(rfh)
exp_rfh = [[rgba[y][rw - 1 - x] for x in range(rw)] for y in range(rh)]
check("rgba flip-h values", rfh_img == exp_rfh, "%r vs %r" % (rfh_img, exp_rfh))

print("\n结果: %s" % ("全部通过" if fails == 0 else ("%d 项失败" % fails)))
sys.exit(1 if fails else 0)
