/* The simulated monitor: how each maladjustment changes the picture.
   u, v are raster coordinates, -256..256 (256 = edge). Positions are in 1/16 pixel. */
#include "g.h"

int val[NCTL], ideal[NCTL];
int magnetized, blot_u, blot_v;
int pattern;
int jitter_x, jitter_y, jitter_k;

#define HW 187          /* nominal half width of the raster, pixels */
#define HH 140
#define CXF (FX + FW / 2)
#define CYF (FY + FH / 2)

static int e(int i) { return val[i] - ideal[i]; }

void vec_res(int a, int *x, int *y)
{
    int av = val[a] * 32 / 3, ai = ideal[a] * 32 / 3;
    *x = val[a + 1] * COS(av) - ideal[a + 1] * COS(ai);
    *y = val[a + 1] * SIN(av) - ideal[a + 1] * SIN(ai);
}
static int isvec(int i) { return i >= CT_PURA && i <= CT_P6S; }
int ctl_err(int i)
{
    if (isvec(i)) {
        int a = CT_PURA + ((i - CT_PURA) & ~1), x, y;
        vec_res(a, &x, &y);
        x = abs(x); y = abs(y);
        return (x > y ? x + y / 2 : y + x / 2) / 127;     /* about |r| in spread units */
    }
    return abs(e(i));
}
int ctl_ok(int i)
{
    if (isvec(i)) {
        int a = CT_PURA + ((i - CT_PURA) & ~1), x, y;
        vec_res(a, &x, &y);
        return x * x + y * y <= 165 * 165;                /* within 1.3 spread units */
    }
    if (i >= CT_ST0) return e(i) == 0;
    return abs(e(i)) <= 1;
}
int grp_ok(int g)
{
    int i;
    for (i = 0; i < NCTL; i++) if (ctl[i].grp == g && !ctl_ok(i)) return 0;
    if (g == G_MAG && magnetized) return 0;
    return 1;
}

/* geometry: raster coordinate to face position, 1/16 pixel */
static void geo(int u, int v, int *px, int *py)
{
    int xs, ys, x, y, s;
    xs = u * (256 + 3 * e(CT_HS)) / 256;
    ys = v * (256 + 3 * e(CT_VS)) / 256;
    xs += xs * (v * v / 256) / 256 * (e(CT_PIN) * 6) / 256;
    xs += xs * v / 256 * (e(CT_TRAP) * 4) / 256;
    x = xs * HW / 16;            /* 256 -> HW pixels, times 16 */
    y = ys * HH / 16;
    s = e(CT_TILT) * 7;          /* thousandths of a radian */
    *px = x - y * s / 1000 + e(CT_HC) * 48 + jitter_x;
    *py = y + x * s / 1000 + e(CT_VC) * 48 + jitter_y;
}
/* convergence: offset of one gun (0 R, 1 G, 2 B) at a raster position, 1/16 pixel */
static int r4x, r4y, r6x, r6y, sd[8];
static const signed char sdx[8] = { 0, 1, 1, 1, 0, -1, -1, -1 }, sdy[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };
static void conv_prep(void)
{
    int i;
    vec_res(CT_P4A, &r4x, &r4y); vec_res(CT_P6A, &r6x, &r6y);
    for (i = 0; i < 8; i++) sd[i] = e(CT_ST0 + i);
}
static void conv(int c, int u, int v, int *dx, int *dy)
{
    int i, sg, x, y;
    if (c == 1) { *dx = 0; *dy = 0; return; }
    sg = c == 0 ? 1 : -1;
    x = (sg * r4x + r6x) * 12 / 127;
    y = (sg * r4y + r6y) * 12 / 127;
    for (i = 0; i < 8; i++) if (sd[i]) {
        int p = u * sdx[i] + v * sdy[i], w, k;
        if (i & 1) p /= 2;
        if (p <= 0) continue;
        if (p > 300) p = 300;
        w = p * p / 256 * p / 256;                      /* 0..256, strong only near that edge */
        k = sg * sd[i] * w * 20 / 256;
        if (i & 1) k = k * 181 / 256;
        x += -sdy[i] * k; y += sdx[i] * k;              /* tangential */
    }
    *dx = x; *dy = y;
}
static void pt(int c, int u, int v, int *x, int *y)
{
    int gx, gy, dx, dy;
    geo(u, v, &gx, &gy); conv(c, u, v, &dx, &dy);
    *x = gx + dx; *y = gy + dy;
}
#define SX(x) (CXF + (((x) + 8) >> 4))
#define SY(y) (CYF + (((y) + 8) >> 4))

static int inten(void) { int b = e(CT_BRIGHT); return b < 0 ? (255 + b * 16 < 60 ? 60 : 255 + b * 16) : 255; }
static int blurw(void) { return abs(e(CT_FOCUS)) / 3; }

static int seg_w, seg_step, seg_k[32];           /* blur half-width and the brightness of each parallel line, set once per redraw */
static void seg_prep(void)
{
    int w = blurw(), I = inten() * 2 / (w + 2), o;
    if (w > 15) w = 15;
    seg_step = w >= 8 ? 3 : w >= 4 ? 2 : 1;      /* a wide blur is drawn with fewer, brighter lines; the dither hides the gaps */
    w -= w % seg_step; seg_w = w;
    for (o = -w; o <= w; o++) { int k = I * seg_step * (w + 1 - abs(o)) / (w + 1); seg_k[o + w] = k > 255 ? 255 : k; }
}
static void seg(int c, int x0, int y0, int x1, int y1, int vert)
{
    int w = seg_w, o, ax = SX(x0), ay = SY(y0), bx = SX(x1), by = SY(y1);
    for (o = -w; o <= w; o += seg_step) {
        if (vert) addline1(ax + o, ay, bx + o, by, c, seg_k[o + w]);
        else addline1(ax, ay + o, bx, by + o, c, seg_k[o + w]);
    }
}
static void hatch(void)
{
    int c, k, j, x0, y0, x1, y1;
    seg_prep();
    for (c = 0; c < 3; c++) {
        for (k = 0; k <= 12; k++) {
            int u = -256 + k * 512 / 12;
            pt(c, u, -256, &x0, &y0);
            for (j = 1; j <= 18; j++) { pt(c, u, -256 + j * 512 / 18, &x1, &y1); seg(c, x0, y0, x1, y1, 1); x0 = x1; y0 = y1; }
        }
        for (j = 0; j <= 9; j++) {
            int v = -256 + j * 512 / 9;
            pt(c, -256, v, &x0, &y0);
            for (k = 1; k <= 24; k++) { pt(c, -256 + k * 512 / 24, v, &x1, &y1); seg(c, x0, y0, x1, y1, 0); x0 = x1; y0 = y1; }
        }
        /* circle, three squares in radius: shows size and linearity */
        pt(c, 127, 0, &x0, &y0);
        for (k = 1; k <= 48; k++) {
            int a = k * 256 / 48;
            pt(c, COS(a), SIN(a) * 170 / 127, &x1, &y1);
            seg(c, x0, y0, x1, y1, abs(SIN(a)) < 90);
            x0 = x1; y0 = y1;
        }
        sys_idle();
    }
}
static void dots(void)
{
    int c, k, j, x, y, w = blurw() + 1, I = inten() * 3 / (w + 2), a, b, n = 0, i;
    static signed char oa[400], ob[400]; static u8 oq[400];     /* the spot, worked out once */
    if (w > 8) w = 8;
    for (a = -w; a <= w; a++) for (b = -w; b <= w; b++) {
        int d = abs(a) + abs(b);
        if (d > w + 1 || n >= 400) continue;
        oa[n] = (signed char)a; ob[n] = (signed char)b; oq[n++] = (u8)(I * (w + 2 - d) / (w + 2));
    }
    for (c = 0; c < 3; c++) for (k = 0; k <= 12; k++) for (j = 0; j <= 9; j++) {
        pt(c, -256 + k * 512 / 12, -256 + j * 512 / 9, &x, &y);
        x = SX(x); y = SY(y);
        for (i = 0; i < n; i++) addpix1(x + oa[i], y + ob[i], c, oq[i]);
    }
}
/* left and right edge of the raster on each face row */
static short xl[FH], xr[FH];
static void edge(int x0, int y0, int x1, int y1)
{
    int n = abs(y1 - y0) + abs(x1 - x0), i;
    if (n < 1) n = 1;
    for (i = 0; i <= n; i++) {
        int x = x0 + (x1 - x0) * i / n, y = y0 + (y1 - y0) * i / n - FY;
        if (y < 0 || y >= FH) continue;
        if (x < xl[y]) xl[y] = (short)x;
        if (x > xr[y]) xr[y] = (short)x;
    }
}
static void outline(void)
{
    int i, x0, y0, x1, y1, s;
    for (i = 0; i < FH; i++) { xl[i] = 32000; xr[i] = -32000; }
    for (s = 0; s < 4; s++) for (i = 0; i < 16; i++) {
        int a = -256 + i * 32, b = a + 32, u0, v0, u1, v1;
        if (s == 0) { u0 = a; u1 = b; v0 = v1 = -256; }
        else if (s == 1) { u0 = u1 = 256; v0 = a; v1 = b; }
        else if (s == 2) { u0 = a; u1 = b; v0 = v1 = 256; }
        else { u0 = u1 = -256; v0 = a; v1 = b; }
        geo(u0, v0, &x0, &y0); geo(u1, v1, &x1, &y1);
        edge(SX(x0), SY(y0), SX(x1), SY(y1));
    }
}
/* colour contamination 0..255 at a raster position */
static int rpx, rpy, rpm;
static int contam(int u, int v)
{
    int k = 0;
    if (rpm) {
        int t = (rpx * u + rpy * v) >> 15;            /* -m..m; 127 * 256 is close to 32768 */
        k = (rpm + t) * 12;
    }
    if (magnetized) {
        int du = u - blot_u, dv = v - blot_v, d2 = du * du + dv * dv;
        if (d2 < 150 * 150) k += 230 * (150 * 150 - d2) / (150 * 150);
    }
    k += jitter_k;
    return k < 0 ? 0 : k > 255 ? 255 : k;
}
/* raster coordinate of each face column and row, worked out once per redraw */
static short ucol[FW], vrow[FH];
static void uv_tables(void)
{
    int x, y, hs = 256 + 3 * e(CT_HS), vs = 256 + 3 * e(CT_VS);
    int ox = CXF + e(CT_HC) * 3 + jitter_x / 16, oy = CYF + e(CT_VC) * 3 + jitter_y / 16;
    for (x = 0; x < FW; x++) ucol[x] = (short)((FX + x - ox) * 256 * 256 / (HW * hs));
    for (y = 0; y < FH; y++) vrow[y] = (short)((FY + y - oy) * 256 * 256 / (HH * vs));
}
/* black outside the raster on every face row */
static void mask_rows(void)
{
    int y;
    for (y = 0; y < FH; y++) {
        int a = xl[y], b = xr[y];
        if (a > b) { hline(FX, FX + FW - 1, FY + y, 0); continue; }
        if (a > FX) hline(FX, a - 1, FY + y, 0);
        if (b < FX + FW - 1) hline(b + 1, FX + FW - 1, FY + y, 0);
    }
}
/* A purity field changes slowly across the screen, so its colour is worked out once per 4x4 block
   and dithered from a table. A 486 redraws it in a few hundredths of a second. */
static void field(int white)
{
    int bx, by, j, I = inten(), lr_ = -1, lg_ = -1, lb_ = -1;
    u8 pat[16];
    vec_res(CT_PURA, &rpx, &rpy);
    rpm = (abs(rpx) > abs(rpy) ? abs(rpx) + abs(rpy) / 2 : abs(rpy) + abs(rpx) / 2) / 127;
    outline(); uv_tables();
    for (by = 0; by < FH; by += 4) {
        int v = vrow[by + 2];
        for (bx = 0; bx < FW; bx += 4) {
            int k = contam(ucol[bx + 2], v), r, g, b;
            if (white) { r = I; g = I - k * I / 300; b = I - k * I / 600; }
            else { r = I - k * I / 420; g = k * I / 360; b = k * I / 1200; }
            r &= ~3; g &= ~3; b &= ~3;
            if (r != lr_ || g != lg_ || b != lb_) { dith16(r, g, b, pat); lr_ = r; lg_ = g; lb_ = b; }
            for (j = 0; j < 4; j++) memcpy(back + (u32)(FY + by + j) * SW + FX + bx, pat + (((FY + by + j) & 3) << 2), 4);
        }
        if ((by & 63) == 0) sys_idle();
    }
    mask_rows();
}
static void pluge(void)
{
    int y, x, br = e(CT_BRIGHT);
    u8 row[FW];
    outline(); uv_tables();
    for (x = 0; x < FW; x++) {
        int u = ucol[x], sig = 4, g;
        if (u > -230 && u < -200) sig = 2;            /* blacker than black */
        else if (u > -170 && u < -140) sig = 6;       /* just above black */
        else if (u > -110 && u < -80) sig = 8;
        else if (u > -40) sig = 4 + ((u + 40) / 37 + 1) * 19 / 8;   /* eight grey steps */
        g = sig + br - 4;
        row[x] = (u8)GRAY(g < 0 ? 0 : g > 23 ? 23 : g);
    }
    { int g = br; u8 flat = (u8)GRAY(g < 0 ? 0 : g > 23 ? 23 : g);
      for (y = 0; y < FH; y++) {
        if (vrow[y] > -200 && vrow[y] < 200) memcpy(back + (u32)(FY + y) * SW + FX, row, FW);
        else memset(back + (u32)(FY + y) * SW + FX, flat, FW);
      } }
    mask_rows();
}
void crt_draw(void)
{
    int y, x, br = e(CT_BRIGHT);
    clip(FX, FY, FX + FW - 1, FY + FH - 1);
    if (br > 0 && pattern != PAT_PLUGE) {      /* black level too high: the whole face glows */
        u8 pat[16], row[4][FW];
        dith16(br * 12, br * 12, br * 13, pat);
        for (y = 0; y < 4; y++) for (x = 0; x < FW; x++) row[y][x] = pat[(y << 2) | ((FX + x) & 3)];
        for (y = FY; y < FY + FH; y++) memcpy(back + (u32)y * SW + FX, row[y & 3], FW);
    } else fill(FX, FY, FW, FH, pattern == PAT_PLUGE && br > 4 ? GRAY(0) : 0);
    conv_prep();
    switch (pattern) {
    case PAT_HATCH: hatch(); break;
    case PAT_DOTS: dots(); break;
    case PAT_RED: field(0); break;
    case PAT_WHITE: field(1); break;
    case PAT_PLUGE: pluge(); break;
    }
    unclip();
}
