#!/usr/bin/env python3
"""Paint the title picture and digitise it for the game.

The scene (a television repair shop) is composed from shapes at 1280x800, gone over
with thousands of short brush strokes that follow the forms, then scaled to 320x200
and reduced to 256 colours with error-diffusion dithering, the way painted art was
scanned for VGA games. Run with --embed to write the result into index.html.
"""
import base64, io, math, random, sys, pathlib
import numpy as np
from PIL import Image, ImageDraw, ImageFilter

W, H = 1280, 800
random.seed(1982)
np.random.seed(1982)
root = pathlib.Path(__file__).resolve().parent.parent

def vgrad(draw, box, top, bot):
    x0, y0, x1, y1 = box
    for y in range(y0, y1):
        t = (y - y0) / max(1, y1 - y0 - 1)
        draw.line([(x0, y), (x1, y)], fill=tuple(int(top[i] + (bot[i] - top[i]) * t) for i in range(3)))

def glow(img, cx, cy, r, col, strength):
    g = Image.new('RGB', img.size, (0, 0, 0))
    d = ImageDraw.Draw(g)
    for k in range(24, 0, -1):
        rr = r * k / 24
        a = strength * (1 - k / 24) ** 1.5
        d.ellipse([cx - rr, cy - rr * .8, cx + rr, cy + rr * .8], fill=tuple(int(c * a) for c in col))
    g = g.filter(ImageFilter.GaussianBlur(r / 10))
    return Image.fromarray(np.clip(np.asarray(img).astype(int) + np.asarray(g), 0, 255).astype('uint8'))

def tv(d, x, y, w, h, wood, screen, knobs=True):
    d.rectangle([x, y, x + w, y + h], fill=wood)
    d.rectangle([x, y, x + w, y + 6], fill=tuple(min(255, c + 40) for c in wood))
    sw = int(w * (.7 if knobs else .86))
    d.rounded_rectangle([x + 10, y + 12, x + sw, y + h - 12], radius=int(h * .14), fill=(18, 18, 22))
    d.rounded_rectangle([x + 18, y + 20, x + sw - 8, y + h - 20], radius=int(h * .12), fill=screen)
    if knobs:
        for i in range(2):
            cy = y + h * (.3 + i * .3)
            d.ellipse([x + w * .8, cy - h * .09, x + w * .8 + h * .18, cy + h * .09], fill=(200, 180, 130))
        for i in range(5):
            d.line([(x + w * .76, y + h * .78 + i * 5), (x + w * .96, y + h * .78 + i * 5)], fill=(30, 20, 12), width=2)

def scene():
    img = Image.new('RGB', (W, H))
    d = ImageDraw.Draw(img)
    # wall: darker at the top, where the title goes
    vgrad(d, (0, 0, W, 560), (14, 20, 34), (52, 60, 66))
    for x in range(0, W, 160):                     # wall boards
        d.line([(x, 0), (x, 560)], fill=(20, 26, 36), width=3)
    # window with a night street and a neon glow
    d.rectangle([60, 110, 300, 330], fill=(40, 30, 24)); vgrad(d, (72, 122, 288, 318), (10, 14, 44), (40, 30, 80))
    d.line([(180, 122), (180, 318)], fill=(40, 30, 24), width=8); d.line([(72, 220), (288, 220)], fill=(40, 30, 24), width=8)
    for i in range(14): d.ellipse([90 + i * 14, 250 + (i * 37) % 40, 96 + i * 14, 256 + (i * 37) % 40], fill=(255, 220, 140))
    d.arc([200, 140, 270, 200], 0, 300, fill=(255, 60, 120), width=6)
    # shelves of sets waiting for repair
    for sy in (250, 400):
        d.rectangle([340, sy, W - 30, sy + 14], fill=(86, 56, 30)); d.rectangle([340, sy + 14, W - 30, sy + 22], fill=(40, 26, 14))
    tv(d, 370, 150, 150, 100, (96, 60, 32), (40, 80, 120))
    tv(d, 560, 164, 120, 86, (60, 60, 66), (20, 30, 40))
    tv(d, 960, 140, 170, 110, (110, 70, 36), (70, 120, 150))
    d.rectangle([1150, 190, 1240, 250], fill=(120, 40, 30)); d.ellipse([1162, 202, 1200, 240], fill=(230, 200, 120))   # radio
    tv(d, 1000, 300, 130, 100, (70, 46, 26), (26, 40, 46)); d.rectangle([1150, 330, 1235, 400], fill=(50, 70, 60))
    for i in range(6): d.rectangle([362 + i * 20, 350, 374 + i * 20, 400], fill=(150 - i * 12, 60 + i * 20, 40))            # service manuals
    # the bench
    d.polygon([(0, 560), (W, 560), (W, 610), (0, 610)], fill=(132, 88, 46))
    d.rectangle([0, 606, W, 630], fill=(66, 42, 22)); vgrad(d, (0, 630, W, H), (46, 32, 22), (14, 10, 10))
    for x in (120, 520, 920): d.rectangle([x, 660, x + 260, 760], fill=(60, 40, 24)); d.ellipse([x + 118, 700, x + 142, 724], fill=(170, 140, 80))
    # the patient: a big wood-cabinet colour set, its screen showing a crosshatch
    X, Y, TW, TH = 440, 270, 430, 300
    d.polygon([(X + TW, Y), (X + TW + 70, Y + 26), (X + TW + 70, Y + TH - 6), (X + TW, Y + TH)], fill=(70, 42, 20))       # side
    d.rectangle([X, Y, X + TW, Y + TH], fill=(128, 80, 38)); d.rectangle([X, Y, X + TW, Y + 12], fill=(170, 112, 56))
    for i in range(0, TH, 22): d.line([(X + 4, Y + i), (X + TW - 4, Y + i + 6)], fill=(112, 68, 30), width=2)            # grain
    d.rounded_rectangle([X + 22, Y + 28, X + 318, Y + TH - 30], radius=44, fill=(20, 20, 24))
    d.rounded_rectangle([X + 36, Y + 42, X + 304, Y + TH - 44], radius=36, fill=(14, 30, 46))
    for i in range(1, 9): d.line([(X + 36 + i * 30, Y + 46), (X + 36 + i * 30, Y + TH - 48)], fill=(230, 240, 255), width=3)
    for j in range(1, 7): d.line([(X + 40, Y + 42 + j * 31), (X + 300, Y + 42 + j * 31)], fill=(230, 240, 255), width=3)
    for i in (2, 5):                                                                                                        # misconverged fringes
        d.line([(X + 31 + i * 30, Y + 60), (X + 31 + i * 30, Y + TH - 60)], fill=(255, 60, 60), width=2)
        d.line([(X + 41 + i * 30, Y + 60), (X + 41 + i * 30, Y + TH - 60)], fill=(70, 110, 255), width=2)
    for i in range(3): d.ellipse([X + 345, Y + 50 + i * 62, X + 395, Y + 100 + i * 62], fill=(210, 190, 140)); d.ellipse([X + 362, Y + 67 + i * 62, X + 378, Y + 83 + i * 62], fill=(90, 70, 40))
    for i in range(7): d.line([(X + 338, Y + 236 + i * 6), (X + 404, Y + 236 + i * 6)], fill=(50, 30, 14), width=3)
    d.line([(X + 150, Y), (X + 60, Y - 150)], fill=(190, 190, 200), width=5); d.line([(X + 190, Y), (X + 300, Y - 140)], fill=(190, 190, 200), width=5)
    # the mirror the technician works by
    d.line([(980, 560), (1000, 420)], fill=(110, 110, 124), width=8)
    d.polygon([(930, 330), (1080, 300), (1100, 440), (950, 470)], fill=(150, 160, 176)); d.polygon([(942, 342), (1070, 316), (1088, 430), (960, 456)], fill=(60, 90, 120))
    for i in range(1, 5): d.line([(950 + i * 26, 340 - i * 5), (966 + i * 26, 452 - i * 5)], fill=(200, 220, 240), width=2)
    # oscilloscope
    d.rounded_rectangle([110, 380, 380, 560], radius=12, fill=(96, 104, 100)); d.rectangle([110, 380, 380, 394], fill=(140, 150, 144))
    d.ellipse([132, 404, 262, 534], fill=(14, 24, 16)); d.ellipse([142, 414, 252, 524], fill=(16, 60, 30))
    pts = [(146 + i, 469 + 34 * math.sin(i / 9.0)) for i in range(0, 104, 2)]
    d.line(pts, fill=(120, 255, 150), width=4)
    for i in range(3):
        for j in range(3): d.ellipse([284 + i * 30, 408 + j * 46, 304 + i * 30, 428 + j * 46], fill=(30, 30, 34) if (i + j) % 2 else (200, 60, 40))
    # multimeter, probes, soldering iron, valves, tools
    d.rounded_rectangle([1010, 470, 1110, 562], radius=8, fill=(220, 170, 40)); d.rectangle([1024, 482, 1096, 516], fill=(220, 230, 210)); d.line([(1060, 514), (1084, 488)], fill=(20, 20, 20), width=3); d.ellipse([1046, 524, 1076, 554], fill=(30, 30, 30))
    d.line([(1040, 562), (1000, 590), (940, 584)], fill=(220, 40, 40), width=5); d.line([(1080, 562), (1130, 592), (1190, 586)], fill=(20, 20, 20), width=5)
    d.line([(60, 548), (150, 596)], fill=(60, 60, 70), width=9); d.line([(34, 536), (64, 550)], fill=(220, 160, 60), width=7); d.ellipse([140, 584, 190, 604], fill=(50, 50, 56))
    for i, c in enumerate([(180, 200, 220), (200, 190, 160), (170, 200, 210)]):
        d.rounded_rectangle([1150 + i * 34, 500, 1176 + i * 34, 562], radius=11, fill=c); d.rectangle([1156 + i * 34, 520, 1170 + i * 34, 548], fill=(90, 90, 100)); d.ellipse([1158 + i * 34, 526, 1168 + i * 34, 536], fill=(255, 150, 60))
    d.line([(400, 592), (470, 584)], fill=(210, 60, 50), width=8); d.line([(470, 584), (520, 580)], fill=(200, 200, 210), width=4)
    d.line([(880, 598), (960, 590)], fill=(60, 110, 200), width=8); d.line([(830, 602), (880, 598)], fill=(200, 200, 210), width=4)
    d.ellipse([560, 586, 640, 604], outline=(180, 110, 50), width=7)                                                        # degaussing coil
    # desk lamp
    d.line([(1220, 560), (1180, 330), (1060, 250)], fill=(40, 60, 54), width=10); d.polygon([(1000, 230), (1090, 220), (1110, 290), (1010, 300)], fill=(60, 110, 96))
    img = glow(img, 1050, 300, 380, (255, 190, 90), .55)          # lamp light
    img = glow(img, X + 170, Y + 150, 330, (90, 150, 255), .5)     # the set's own light
    img = glow(img, 197, 469, 150, (60, 255, 120), .35)            # scope
    img = glow(img, 235, 170, 120, (255, 50, 110), .3)             # neon outside
    # light pooling on the bench
    a = np.asarray(img).astype(float); yy = np.arange(H)[:, None]; xx = np.arange(W)[None, :]
    shade = np.clip(1.15 - ((xx - 700) / 900.0) ** 2 - ((yy - 520) / 520.0) ** 2, .28, 1.1)
    return Image.fromarray(np.clip(a * shade[:, :, None], 0, 255).astype('uint8'))

def paint(base):
    """Brush-stroke pass: short strokes along the forms, colour picked up from the scene."""
    a = np.asarray(base.filter(ImageFilter.GaussianBlur(2))).astype(float)
    lum = a.mean(2); gy, gx = np.gradient(lum)
    out = base.filter(ImageFilter.GaussianBlur(5)); d = ImageDraw.Draw(out)
    for size, count in ((13, 9000), (7, 16000), (4, 14000)):
        for _ in range(count):
            x = random.randrange(W); y = random.randrange(H)
            ang = math.atan2(gy[y, x], gx[y, x]) + math.pi / 2 + random.uniform(-.5, .5)
            if abs(gx[y, x]) + abs(gy[y, x]) < 1.5: ang = random.uniform(-.35, .35) + (0 if y > 540 else .9)
            L = size * random.uniform(1.4, 3.2); j = random.uniform(-14, 14)
            c = tuple(int(max(0, min(255, a[y, x, k] + j + random.uniform(-6, 6)))) for k in range(3))
            dx, dy = math.cos(ang) * L / 2, math.sin(ang) * L / 2
            d.line([(x - dx, y - dy), (x + dx, y + dy)], fill=c, width=size)
    return Image.blend(out, base, .22)      # let the drawing show through a little

def digitise(img):
    small = img.resize((320, 200), Image.LANCZOS)
    return small.quantize(colors=256, method=Image.MEDIANCUT, dither=Image.FLOYDSTEINBERG)

if __name__ == '__main__':
    pic = digitise(paint(scene()))
    (root / 'art').mkdir(exist_ok=True)
    pic.save(root / 'art' / 'title.png', optimize=True)
    buf = io.BytesIO(); pic.save(buf, 'PNG', optimize=True)
    print('art/title.png', len(buf.getvalue()), 'bytes, colours', len(pic.getcolors()))
    if '--embed' in sys.argv:
        p = root / 'index.html'; s = p.read_text()
        a = s.index('/*TITLE-ART*/'); b = s.index('/*END-TITLE-ART*/')
        s = s[:a] + "/*TITLE-ART*/'data:image/png;base64," + base64.b64encode(buf.getvalue()).decode() + "'" + s[b:]
        p.write_text(s); print('embedded in index.html')
