/* Video: VESA 256-colour modes, banked or linear, palette, drawing primitives, ROM font. */
#include <i86.h>
#include <conio.h>
#include "cc.h"

u8 *back;
int SW = 640, SH = 480;
int vid_lfb, vid_force_bank;
u8 pal[768];
int cx0, cy0, cx1, cy1;

static u8 *shadow;          /* what is on the screen now */
static u8 *lfb;             /* linear frame buffer, if any */
static int pitch;           /* bytes per scan line on the card */
static u32 gran, winsize;   /* bank granularity and window size, bytes */
static u8 *winptr;          /* window A in conventional memory */
static int curbank = -1;
static int all_dirty = 1;
static u8 font16[256 * 16], font8[256 * 8];
static int in_gfx;

#pragma pack(push, 1)
typedef struct { u32 edi, esi, ebp, rsv, ebx, edx, ecx, eax; u16 flags, es, ds, fs, gs, ip, cs, sp, ss; } RMI;
#pragma pack(pop)

static int rmint(int n, RMI *r)
{
    union REGS in; struct SREGS s;
    segread(&s);
    r->flags = 0; r->sp = 0; r->ss = 0;
    in.w.ax = 0x300; in.h.bl = (u8)n; in.h.bh = 0; in.w.cx = 0;
    s.es = FP_SEG(r); in.x.edi = FP_OFF(r);
    int386x(0x31, &in, &in, &s);
    return in.x.cflag;
}
static u16 dos_seg, dos_sel;
static u8 *dos_buf(void)   /* 1 KB of conventional memory for VBE calls */
{
    union REGS r;
    if (!dos_seg) {
        r.w.ax = 0x100; r.w.bx = 64; int386(0x31, &r, &r);
        if (r.x.cflag) return 0;
        dos_seg = r.w.ax; dos_sel = r.w.dx;
    }
    return (u8 *)((u32)dos_seg << 4);
}
static int mode_of(int w, int h) { return w == 640 ? 0x101 : w == 800 ? 0x103 : w == 1024 ? 0x105 : 0; }

/* Query one mode. Returns 0 if usable and fills the outputs. */
static int mode_info(int mode, int *ppitch, u32 *pgran, u32 *pwin, u32 *pseg, u32 *pphys, int *plfb)
{
    RMI r; u8 *b = dos_buf(); int ver;
    if (!b) return 5;
    memset(b, 0, 512); memcpy(b, "VBE2", 4);
    memset(&r, 0, sizeof r); r.eax = 0x4F00; r.es = dos_seg; r.edi = 0;
    rmint(0x10, &r);
    if ((r.eax & 0xFFFF) != 0x004F) return 1;
    ver = b[4] | (b[5] << 8);
    memset(b, 0, 256);
    memset(&r, 0, sizeof r); r.eax = 0x4F01; r.ecx = mode; r.es = dos_seg; r.edi = 0;
    rmint(0x10, &r);
    if ((r.eax & 0xFFFF) != 0x004F) return 2;
    if (!(b[0] & 1)) return 2;
    if (b[0x19] != 8) return 2;
    *ppitch = b[0x10] | (b[0x11] << 8);
    *pgran = (u32)(b[4] | (b[5] << 8)) * 1024;
    *pwin = (u32)(b[6] | (b[7] << 8)) * 1024;
    *pseg = b[8] | (b[9] << 8);
    *pphys = b[0x28] | (b[0x29] << 8) | ((u32)b[0x2A] << 16) | ((u32)b[0x2B] << 24);
    *plfb = (ver >= 0x200) && (b[0] & 0x80) && *pphys;
    if (!*pgran) *pgran = 65536;
    if (!*pwin) *pwin = 65536;
    if (!*ppitch) return 2;
    return 0;
}
int vid_mode_ok(int w, int h)
{
    int p, l; u32 g, ws, sg, ph;
    (void)h;
    return mode_of(w, h) && mode_info(mode_of(w, h), &p, &g, &ws, &sg, &ph, &l) == 0;
}
const char *vid_error(int code)
{
    switch (code) {
    case 1: return "No VESA BIOS found. This program needs an SVGA card with VESA support.\nIf your card came with a VESA driver (or UNIVBE), load it first.";
    case 2: return "The VESA BIOS does not offer this 256-colour mode.";
    case 3: return "The video mode could not be set.";
    case 4: return "Not enough memory for the screen buffers.";
    default: return "Could not get DOS memory for the VESA call.";
    }
}
static void load_fonts(void)
{
    RMI r; u8 *p;
    memset(&r, 0, sizeof r); r.eax = 0x1130; r.ebx = 0x0600; rmint(0x10, &r);
    p = (u8 *)(((u32)r.es << 4) + (r.ebp & 0xFFFF));
    memcpy(font16, p, 4096);
    memset(&r, 0, sizeof r); r.eax = 0x1130; r.ebx = 0x0300; rmint(0x10, &r);
    p = (u8 *)(((u32)r.es << 4) + (r.ebp & 0xFFFF));
    memcpy(font8, p, 1024);
    memset(&r, 0, sizeof r); r.eax = 0x1130; r.ebx = 0x0400; rmint(0x10, &r);
    p = (u8 *)(((u32)r.es << 4) + (r.ebp & 0xFFFF));
    memcpy(font8 + 1024, p, 1024);
}
int vid_init(int w, int h)
{
    int mode = mode_of(w, h), e, uselfb; u32 seg, phys; union REGS r;
    static int fonts;
    if (!mode) return 2;
    if (!fonts) { load_fonts(); fonts = 1; }
    e = mode_info(mode, &pitch, &gran, &winsize, &seg, &phys, &uselfb);
    if (e) return e;
    if (vid_force_bank) uselfb = 0;
    if (back) { free(back); free(shadow); back = shadow = 0; }
    back = (u8 *)malloc((u32)w * h); shadow = (u8 *)malloc((u32)w * h);
    if (!back || !shadow) return 4;
    lfb = 0;
    if (uselfb) {
        r.w.ax = 0x800; r.w.bx = (u16)(phys >> 16); r.w.cx = (u16)phys;
        r.w.si = (u16)(((u32)pitch * h) >> 16); r.w.di = (u16)((u32)pitch * h);
        int386(0x31, &r, &r);
        if (!r.x.cflag) lfb = (u8 *)(((u32)r.w.bx << 16) | r.w.cx);
    }
    r.w.ax = 0x4F02; r.w.bx = (u16)(lfb ? (mode | 0x4000) : mode);
    int386(0x10, &r, &r);
    if (r.w.ax != 0x004F && lfb) {   /* linear failed: fall back to banks */
        lfb = 0; r.w.ax = 0x4F02; r.w.bx = (u16)mode; int386(0x10, &r, &r);
    }
    if (r.w.ax != 0x004F) return 3;
    vid_lfb = lfb != 0;
    winptr = (u8 *)(seg << 4);
    SW = w; SH = h; curbank = -1; in_gfx = 1;
    memset(back, 0, (u32)w * h); memset(shadow, 0, (u32)w * h);
    all_dirty = 1; unclip();
    pal_build(); pal_apply(64, 7);
    return 0;
}
void vid_done(void)
{
    union REGS r;
    if (in_gfx) { r.w.ax = 3; int386(0x10, &r, &r); in_gfx = 0; }
}
void vid_invalidate(void) { all_dirty = 1; }
void vid_vsync(void)
{
    int n = 0;
    while ((inp(0x3DA) & 8) && ++n < 200000) ;
    n = 0;
    while (!(inp(0x3DA) & 8) && ++n < 200000) ;
}
static void setbank(int b)
{
    union REGS r;
    if (b == curbank) return;
    curbank = b;
    r.w.ax = 0x4F05; r.w.bx = 0; r.w.dx = (u16)b; int386(0x10, &r, &r);
}
static void put_span(u32 off, const u8 *src, int len)
{
    if (lfb) { memcpy(lfb + off, src, len); return; }
    while (len > 0) {
        u32 b = off / gran, wo = off - b * gran;
        int n = (int)(winsize - wo);
        if (n > len) n = len;
        setbank((int)b);
        memcpy(winptr + wo, src, n);
        off += n; src += n; len -= n;
    }
}
void vid_present(void)
{
    int y, a, b;
    for (y = 0; y < SH; y++) {
        u8 *p = back + (u32)y * SW, *q = shadow + (u32)y * SW;
        if (!all_dirty) {
            if (!memcmp(p, q, SW)) continue;
            a = 0; while (a < SW - 4 && *(u32 *)(p + a) == *(u32 *)(q + a)) a += 4;
            b = SW; while (b > a + 4 && *(u32 *)(p + b - 4) == *(u32 *)(q + b - 4)) b -= 4;
        } else { a = 0; b = SW; }
        put_span((u32)y * pitch + a, p + a, b - a);
        memcpy(q + a, p + a, b - a);
        if ((y & 31) == 0) sys_idle();
    }
    all_dirty = 0;
}

/* ---------- palette ---------- */
static const u8 cube6[6] = { 0, 13, 25, 38, 50, 63 };
static u8 lr[256], lg[256], lb[256];   /* cube level of each colour, for additive drawing */
void pal_build(void)
{
    int i, r, g, b;
    static const u8 extra[16][3] = {
        {22,20,16},{30,28,23},{38,36,30},{45,43,36},{51,49,42},{56,54,47},{61,59,53}, /* beige */
        {12,15,22},{26,31,42},{4,6,10},  /* panel, light edge, dark edge */
        {63,44,8},{10,60,18},{63,12,10}, /* amber, green LED, red LED */
        {4,6,14},{63,58,20},{63,63,63}   /* ink, selection, white */
    };
    for (i = 0; i < 216; i++) {
        r = i / 36; g = (i / 6) % 6; b = i % 6;
        pal[i * 3] = cube6[r]; pal[i * 3 + 1] = cube6[g]; pal[i * 3 + 2] = cube6[b];
        lr[i] = (u8)r; lg[i] = (u8)g; lb[i] = (u8)b;
    }
    for (i = 0; i < 24; i++) {
        int v = i * 63 / 23;
        pal[(216 + i) * 3] = pal[(216 + i) * 3 + 1] = pal[(216 + i) * 3 + 2] = (u8)v;
        lr[216 + i] = lg[216 + i] = lb[216 + i] = (u8)((v * 5 + 31) / 63);
    }
    for (i = 0; i < 16; i++) {
        pal[(240 + i) * 3] = extra[i][0]; pal[(240 + i) * 3 + 1] = extra[i][1]; pal[(240 + i) * 3 + 2] = extra[i][2];
        lr[240 + i] = (u8)((extra[i][0] * 5 + 31) / 63); lg[240 + i] = (u8)((extra[i][1] * 5 + 31) / 63); lb[240 + i] = (u8)((extra[i][2] * 5 + 31) / 63);
    }
}
void pal_apply(int level, int guns)
{
    int i;
    outp(0x3C8, 0);
    for (i = 0; i < 256; i++) {
        outp(0x3C9, (guns & 1) ? pal[i * 3] * level / 64 : 0);
        outp(0x3C9, (guns & 2) ? pal[i * 3 + 1] * level / 64 : 0);
        outp(0x3C9, (guns & 4) ? pal[i * 3 + 2] * level / 64 : 0);
    }
}
void pal_fade(int from, int to)
{
    int l, st = from < to ? 8 : -8;
    if (auto_mode) { pal_apply(to, 7); return; }
    for (l = from; l != to; l += st) { vid_vsync(); pal_apply(l, 7); sys_idle(); }
    pal_apply(to, 7);
}
void vid_shot(const char *name)
{
    FILE *f = fopen(name, "wb"); u8 h[54]; int i, y; u32 sz = 54 + 1024 + (u32)SW * SH;
    if (!f) return;
    memset(h, 0, 54); h[0] = 'B'; h[1] = 'M';
    h[2] = (u8)sz; h[3] = (u8)(sz >> 8); h[4] = (u8)(sz >> 16); h[10] = 54 + 1024 - 1024 + 0; h[10] = (u8)(54 + 1024); h[11] = (u8)((54 + 1024) >> 8);
    h[14] = 40; h[18] = (u8)SW; h[19] = (u8)(SW >> 8); h[22] = (u8)SH; h[23] = (u8)(SH >> 8); h[26] = 1; h[28] = 8;
    fwrite(h, 1, 54, f);
    for (i = 0; i < 256; i++) { u8 q[4]; q[0] = (u8)(pal[i * 3 + 2] * 255 / 63); q[1] = (u8)(pal[i * 3 + 1] * 255 / 63); q[2] = (u8)(pal[i * 3] * 255 / 63); q[3] = 0; fwrite(q, 1, 4, f); }
    for (y = SH - 1; y >= 0; y--) fwrite(back + (u32)y * SW, 1, SW, f);
    fclose(f);
}

/* ---------- primitives ---------- */
void clip(int x0, int y0, int x1, int y1)
{
    cx0 = x0 < 0 ? 0 : x0; cy0 = y0 < 0 ? 0 : y0;
    cx1 = x1 >= SW ? SW - 1 : x1; cy1 = y1 >= SH ? SH - 1 : y1;
}
void unclip(void) { cx0 = 0; cy0 = 0; cx1 = SW - 1; cy1 = SH - 1; }
void pset(int x, int y, int c) { if (x >= cx0 && x <= cx1 && y >= cy0 && y <= cy1) back[(u32)y * SW + x] = (u8)c; }
void hline(int x0, int x1, int y, int c)
{
    if (y < cy0 || y > cy1) return;
    if (x0 < cx0) x0 = cx0;
    if (x1 > cx1) x1 = cx1;
    if (x1 >= x0) memset(back + (u32)y * SW + x0, c, x1 - x0 + 1);
}
void vline(int x, int y0, int y1, int c)
{
    u8 *p;
    if (x < cx0 || x > cx1) return;
    if (y0 < cy0) y0 = cy0;
    if (y1 > cy1) y1 = cy1;
    for (p = back + (u32)y0 * SW + x; y0 <= y1; y0++, p += SW) *p = (u8)c;
}
void fill(int x, int y, int w, int h, int c) { int j; for (j = 0; j < h; j++) hline(x, x + w - 1, y + j, c); }
void box(int x, int y, int w, int h, int c) { hline(x, x + w - 1, y, c); hline(x, x + w - 1, y + h - 1, c); vline(x, y, y + h - 1, c); vline(x + w - 1, y, y + h - 1, c); }
void bevel(int x, int y, int w, int h, int hi, int lo) { hline(x, x + w - 1, y, hi); vline(x, y, y + h - 1, hi); hline(x, x + w - 1, y + h - 1, lo); vline(x + w - 1, y, y + h - 1, lo); }
void line(int x0, int y0, int x1, int y1, int c)
{
    int dx = abs(x1 - x0), dy = abs(y1 - y0), sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1, e = dx - dy, e2;
    for (;;) {
        pset(x0, y0, c);
        if (x0 == x1 && y0 == y1) break;
        e2 = 2 * e;
        if (e2 > -dy) { e -= dy; x0 += sx; }
        if (e2 < dx) { e += dx; y0 += sy; }
    }
}
void circle(int cx, int cy, int r, int c)
{
    int x = r, y = 0, e = 1 - r;
    while (x >= y) {
        pset(cx + x, cy + y, c); pset(cx - x, cy + y, c); pset(cx + x, cy - y, c); pset(cx - x, cy - y, c);
        pset(cx + y, cy + x, c); pset(cx - y, cy + x, c); pset(cx + y, cy - x, c); pset(cx - y, cy - x, c);
        y++;
        if (e < 0) e += 2 * y + 1; else { x--; e += 2 * (y - x) + 1; }
    }
}
void disc(int cx, int cy, int r, int c)
{
    int y, x;
    for (y = -r; y <= r; y++) { x = 0; while ((x + 1) * (x + 1) + y * y <= r * r) x++; hline(cx - x, cx + x, cy + y, c); }
}
int rgbc(int r, int g, int b) { return ((r * 5 + 127) / 255) * 36 + ((g * 5 + 127) / 255) * 6 + (b * 5 + 127) / 255; }
static const u8 bay[16] = { 0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5 };
int dith(int x, int y, int r, int g, int b)
{
    int t = bay[((y & 3) << 2) | (x & 3)] * 16 + 8;   /* 8..248 */
    int R = (r * 5 + t) / 255, G = (g * 5 + t) / 255, B = (b * 5 + t) / 255;
    if (R > 5) R = 5;
    if (G > 5) G = 5;
    if (B > 5) B = 5;
    return R * 36 + G * 6 + B;
}
void dfill(int x, int y, int w, int h, int r0, int g0, int b0, int r1, int g1, int b1)
{
    int i, j, d = h > 1 ? h - 1 : 1;
    for (j = 0; j < h; j++) {
        int r = r0 + (r1 - r0) * j / d, g = g0 + (g1 - g0) * j / d, b = b0 + (b1 - b0) * j / d;
        if (y + j < cy0 || y + j > cy1) continue;
        for (i = 0; i < w; i++) if (x + i >= cx0 && x + i <= cx1) back[(u32)(y + j) * SW + x + i] = (u8)dith(x + i, y + j, r, g, b);
    }
}
void addrgb(int x, int y, int r, int g, int b)
{
    u8 *p; int t, R, G, B;
    if (x < cx0 || x > cx1 || y < cy0 || y > cy1) return;
    p = back + (u32)y * SW + x;
    t = bay[((y & 3) << 2) | (x & 3)] * 16 + 8;
    R = lr[*p] + (r * 5 + t) / 255; G = lg[*p] + (g * 5 + t) / 255; B = lb[*p] + (b * 5 + t) / 255;
    if (R > 5) R = 5;
    if (G > 5) G = 5;
    if (B > 5) B = 5;
    *p = (u8)(R * 36 + G * 6 + B);
}
void addline(int x0, int y0, int x1, int y1, int r, int g, int b)
{
    int dx = abs(x1 - x0), dy = abs(y1 - y0), sx = x0 < x1 ? 1 : -1, sy = y0 < y1 ? 1 : -1, e = dx - dy, e2;
    for (;;) {
        if (x0 == x1 && y0 == y1) break;   /* the end point belongs to the next segment */
        addrgb(x0, y0, r, g, b);
        e2 = 2 * e;
        if (e2 > -dy) { e -= dy; x0 += sx; }
        if (e2 < dx) { e += dx; y0 += sy; }
    }
}
static void glyph(int x, int y, const u8 *g, int h, int c, int sc)
{
    int i, j;
    for (j = 0; j < h; j++) {
        u8 bits = g[j];
        for (i = 0; i < 8; i++) if (bits & (0x80 >> i)) {
            if (sc == 1) pset(x + i, y + j, c); else fill(x + i * sc, y + j * sc, sc, sc, c);
        }
    }
}
void text(int x, int y, const char *s, int c) { for (; *s; s++, x += 8) glyph(x, y, font16 + (u8)*s * 16, 16, c, 1); }
void text8(int x, int y, const char *s, int c) { for (; *s; s++, x += 8) glyph(x, y, font8 + (u8)*s * 8, 8, c, 1); }
void textbig(int x, int y, const char *s, int c, int sc) { for (; *s; s++, x += 8 * sc) glyph(x, y, font16 + (u8)*s * 16, 16, c, sc); }
void textsh(int x, int y, const char *s, int c) { text(x + 1, y + 1, s, C_BLACK); text(x, y, s, c); }
void textc(int xc, int y, const char *s, int c) { textsh(xc - (int)strlen(s) * 4, y, s, c); }
/* additive text, used for the misconverged logo */
void textadd(int x, int y, const char *s, int sc, int r, int g, int b)
{
    int i, j, a, c;
    for (; *s; s++, x += 8 * sc) {
        const u8 *gl = font16 + (u8)*s * 16;
        for (j = 0; j < 16; j++) for (i = 0; i < 8; i++) if (gl[j] & (0x80 >> i))
            for (a = 0; a < sc; a++) for (c = 0; c < sc; c++) addrgb(x + i * sc + c, y + j * sc + a, r, g, b);
    }
}
