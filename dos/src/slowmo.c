/* Slow motion: one trip through the tube, from the cathode to the light leaving the screen, in five views.
   Built for a 486: whole-number arithmetic, tables made at build time (sm_tab.c), a static background per
   view that is copied back each frame, and a few hundred small dots on top. */
#include "g.h"
#include "sm.h"

#define VY 18                     /* the view: rows VY .. VY + VH - 1 */
#define VH 332
#define CY (VY + VH / 2)
enum { V_GUN, V_FLIGHT, V_MASK, V_PHOS, V_ATOMS, NVIEW };
static const char *vname[NVIEW] = { "1 GUN", "2 FLIGHT", "3 MASK", "4 PHOSPHOR", "5 ATOMS" };

static u8 *bg;
static int view, paused, mul4 = 4, guns = 7, dirty_bg;
static int GC[3], GCD[3];         /* gun colours, bright and dim */
static const char *gname[3] = { "red", "green", "blue" };
static char rd[4][90];            /* readout lines */

static void t8c(int xc, int y, const char *s, int c) { text8(xc - (int)strlen(s) * 4, y, s, c); }
static void dot2(int x, int y, int c) { if (x >= 0 && x < SW - 1 && y >= VY && y < VY + VH - 1) { u8 *p = back + (u32)y * SW + x; p[0] = p[1] = p[SW] = p[SW + 1] = (u8)c; } }
static void wrap(int x, int y, int cols, int maxl, const char *s, int c)
{
    char ln[90]; int n, l = 0;
    while (*s && l < maxl) {
        while (*s == ' ') s++;
        n = (int)strlen(s);
        if (n > cols) { n = cols; while (n > 0 && s[n] != ' ') n--; if (!n) n = cols; }
        memcpy(ln, s, n); ln[n] = 0; text8(x, y + l * 9, ln, c); l++; s += n;
    }
}
static void poly(const int *p, int n, int c)
{
    int y, y0 = p[1], y1 = p[1], i, j, k, m, xs[8], t;
    for (i = 1; i < n; i++) { if (p[i * 2 + 1] < y0) y0 = p[i * 2 + 1]; if (p[i * 2 + 1] > y1) y1 = p[i * 2 + 1]; }
    for (y = y0; y <= y1; y++) {
        m = 0;
        for (i = 0; i < n; i++) { int ax = p[i * 2], ay = p[i * 2 + 1], bx, by; j = (i + 1) % n; bx = p[j * 2]; by = p[j * 2 + 1];
            if ((ay <= y && by > y) || (by <= y && ay > y)) xs[m++] = ax + (y - ay) * (bx - ax) / (by - ay); }
        for (i = 1; i < m; i++) for (k = i; k > 0 && xs[k - 1] > xs[k]; k--) { t = xs[k]; xs[k] = xs[k - 1]; xs[k - 1] = t; }
        for (k = 0; k + 1 < m; k += 2) hline(xs[k], xs[k + 1], y, c);
    }
}
/* a number too big to mean anything as digits: two figures and a name. Value = mant x 10^e. */
static void big(char *t, u32 mant, int e)
{
    static const char *nm[6] = { "", " thousand", " million", " billion", " thousand billion", " million billion" };
    int g;
    while (mant >= 100) { mant = (mant + 5) / 10; e++; }
    while (mant && mant < 10) { mant *= 10; e--; }
    e += 1;                                   /* now value = (mant / 10) x 10^e, mant 10..99 */
    g = e / 3; if (g > 5) g = 5; if (g < 0) g = 0;
    switch (e - g * 3) {
    case 0: sprintf(t, "%lu.%lu%s", mant / 10, mant % 10, nm[g]); break;
    case 1: sprintf(t, "%lu%s", mant, nm[g]); break;
    default: sprintf(t, "%lu0%s", mant, nm[g]); break;
    }
}
static void scalebar(int px, const char *label)
{
    int x = SW - 20 - px, y = VY + VH - 12;
    hline(x, x + px, y, C_WHITE); vline(x, y - 4, y, C_WHITE); vline(x + px, y - 4, y, C_WHITE);
    t8c(x + px / 2, y - 12, label, C_WHITE);
}
static void save_bg(void) { memcpy(bg, back + (u32)VY * SW, (u32)SW * VH); }
static void put_bg(void) { memcpy(back + (u32)VY * SW, bg, (u32)SW * VH); }

/* ======================= 1. the gun ======================= */
#define MAXGP 420
typedef struct { int x, lat; u32 t; u8 g, on; } GunP;
static GunP gp[MAXGP];
static int g_bright = 70, g_acc;
static int GX(int x) { return 40 + (((x >> 10) * 602) >> 12); }       /* x in 1/65536 mm; 9.4 pixels a millimetre */
static const int gun_y[3] = { CY - 52, CY, CY + 52 };
static void gun_bg(void)
{
    int i, x; char t[40];
    for (i = 0; i < 3; i++) {
        int y = gun_y[i], c1 = rgbc(123, 138, 168), c2 = rgbc(195, 205, 228);
#define PLATE(x0, x1, ap, c) { fill(40 + (x0) * 94 / 10, y - 23, ((x1) - (x0)) * 94 / 10 + 1, 23 - (ap), c); fill(40 + (x0) * 94 / 10, y + (ap), ((x1) - (x0)) * 94 / 10 + 1, 23 - (ap), c); }
        fill(40 - 24, y - 10, 24, 21, rgbc(180, 80, 42)); fill(40 - 3, y - 10, 3, 21, rgbc(255, 208, 160));
        fill(40 + 7, y - 23, 3, 20, c1); fill(40 + 7, y + 4, 3, 20, c1);                 /* grid 1 */
        fill(40 + 25, y - 23, 4, 19, c1); fill(40 + 25, y + 5, 4, 19, c1);               /* grid 2 */
        fill(40 + 47, y - 23, 197, 3, c1); fill(40 + 47, y + 20, 197, 3, c1); PLATE(5, 5, 9, c1);            /* focus electrode */
        fill(40 + 273, y - 23, 197, 3, c2); fill(40 + 273, y + 20, 197, 3, c2); PLATE(29, 29, 15, c2);       /* final anode */
        text8(2, y + 26, gname[i], GCD[i]);
#undef PLATE
    }
    sprintf(t, "grid 1, %d V", -5 - (100 - g_bright) * 60 / 100);
    text8(8, VY + 4, "cathode, 0 V", C_WHITE); text8(8, VY + 16, t, C_WHITE); text8(8, VY + 28, "grid 2, +600 V", C_WHITE);
    vline(28, VY + 13, gun_y[0] - 12, GRAY(8)); vline(48, VY + 25, gun_y[0] - 24, GRAY(8)); vline(66, VY + 37, gun_y[0] - 24, GRAY(8));
    t8c(40 + 145, gun_y[0] - 38, "focus electrode, +7,000 V", C_WHITE); t8c(40 + 371, gun_y[0] - 38, "final anode, +25,000 V", C_WHITE);
    /* speed against distance */
    box(40, VY + VH - 62, 584, 42, GRAY(6));
    for (x = 0; x < 583; x++) { int idx = x * 496 / 583, v = sm_gun_v[idx] * 38 / 25800; pset(40 + x, VY + VH - 22 - v, C_AMBER); pset(40 + x, VY + VH - 23 - v, C_AMBER); }
    text8(46, VY + VH - 59, "speed (top of box = 32% of the speed of light)", GRAY(13));
    scalebar(94, "10 mm");
}
static void gun_step(int rdt)
{
    int i, k, dtq = rdt * 200 * mul4 / 16;                                   /* a quarter of the frame, in fs */
    g_acc += rdt * mul4 * 14 * g_bright / 4;
    while (g_acc >= 100000) {
        g_acc -= 100000;
        for (k = 0; k < 3; k++) if (guns & (1 << k)) for (i = 0; i < MAXGP; i++) if (!gp[i].on) {
            gp[i].on = 1; gp[i].g = (u8)k; gp[i].x = 0; gp[i].t = 0;
            gp[i].lat = (int)(sm_rnd() + sm_rnd() + sm_rnd() + sm_rnd()) / 256 - 512; break; }
    }
    for (i = 0; i < MAXGP; i++) if (gp[i].on) {
        for (k = 0; k < 4; k++) { int idx = gp[i].x >> 13; if (idx >= SM_GN) break; gp[i].x += (int)(((u32)sm_gun_v[idx] * (u32)dtq) >> 12); }
        gp[i].t += dtq * 4;
        if (gp[i].x >= 62 * 65536 - 8192) gp[i].on = 0;
    }
}
static void gun_draw(void)
{
    int i, f = -1; u32 ft = 0; char a[40], b[40];
    for (i = 0; i < MAXGP; i++) if (gp[i].on) {
        int idx = gp[i].x >> 13, y = gun_y[gp[i].g] + ((gp[i].lat * sm_gun_w[idx] * 9) >> 17);
        dot2(GX(gp[i].x), y, GC[gp[i].g]);
        if (gp[i].t >= ft) { ft = gp[i].t; f = i; }
    }
    { u32 ua = (u32)g_bright * 4; big(a, ua * 624, 10);                     /* microamps x 6.24e12 electrons a second */
      strcpy(rd[0], "Three guns side by side, seen from above. Cathodes: oxide on nickel, at 800 C.");
      sprintf(rd[1], "Beam current per gun: %lu.%02lu mA = %s electrons a second.", ua / 1000, ua % 1000 / 10, a); }
    if (f >= 0) {
        int idx = gp[f].x >> 13, ev = sm_gun_ev[idx], v = sm_gun_v[idx], y = gun_y[gp[f].g] + ((gp[f].lat * sm_gun_w[idx] * 9) >> 17), x = GX(gp[f].x);
        box(x - 4, y - 4, 10, 10, C_WHITE);
        if (ev < 1) strcpy(a, "under 1 eV"); else if (ev < 1000) sprintf(a, "%d eV", ev); else sprintf(a, "%d.%d keV", ev / 1000, ev % 1000 / 100);
        if (gp[f].t < 1000000) sprintf(b, "%lu ps", gp[f].t / 1000); else sprintf(b, "%lu.%02lu ns", gp[f].t / 1000000, gp[f].t / 10000 % 100);
        sprintf(rd[2], "Boxed: %s, %ld km/s (%d.%d%% of light), %s after leaving the cathode.", a, (long)v * 3725 / 1000, v * 302 / 24290 / 10, v * 302 / 24290 % 10, b);
    } else strcpy(rd[2], "No electrons: grid 1 has cut the beam off. That is a black screen.");
    strcpy(rd[3], "\x18\x19 brightness (the voltage on grid 1)");
}

/* ======================= 2. the flight ======================= */
#define MAXFP 300
typedef struct { SmFly f; u8 g, on; } FlyP;
static FlyP fp[MAXFP];
static struct { short y; u8 g; short ttl; } fglow[48];
static int f_b = 0, f_sweep = 1, f_acc, f_path, f_phase, f_traced = 99999;
static short f_tr[3][40][2]; static int f_trn[3], f_ym[3];
#define FX_(x) (60 + ((x) >> 16))
#define FY_(y) (CY - ((((y) >> 12) * 15) >> 8))
static void fly_bg(void)
{
    static const int tube[16] = { 0, CY - 14, 130, CY - 14, 345, CY - 158, 366, CY - 158, 366, CY + 158, 345, CY + 158, 130, CY + 14, 0, CY + 14 };
    int i, sg, y;
    poly(tube, 8, rgbc(7, 19, 26));
    for (i = 0; i < 8; i++) { int j = (i + 1) % 8; if (i != 7) line(tube[i * 2], tube[i * 2 + 1], tube[j * 2], tube[j * 2 + 1], rgbc(92, 142, 160)); }
    fill(5, CY - 9, 55, 18, rgbc(42, 51, 72));
    for (sg = -1; sg <= 1; sg += 2) { int p[10]; p[0] = 100; p[1] = CY + sg * 15; p[2] = 135; p[3] = CY + sg * 15; p[4] = 172; p[5] = CY + sg * 41; p[6] = 172; p[7] = CY + sg * 62; p[8] = 100; p[9] = CY + sg * 34; poly(p, 5, rgbc(200, 120, 50)); }
    for (y = CY - 150; y < CY + 150; y += 6) vline(346, y, y + 2, GRAY(12));     /* mask */
    fill(359, CY - 150, 3, 300, C_WHITE);                                         /* screen */
    text8(18, CY + 14, "gun", GRAY(13)); text8(92, CY + 68, "deflection yoke", rgbc(200, 120, 50)); text8(300, VY + 4, "mask", GRAY(13)); text8(366, VY + 16, "screen", C_WHITE);
    scalebar(100, "100 mm");
}
static void fly_trace(void)
{
    int g, n;
    for (g = 0; g < 3; g++) { SmFly p; sm_fly_start(&p, g); n = 0; f_trn[g] = 0;
        while (p.x < 300 * 65536 && p.dx > 65536 && n < 640) { if ((n & 15) == 0 && f_trn[g] < 39) { f_tr[g][f_trn[g]][0] = (short)FX_(p.x); f_tr[g][f_trn[g]][1] = (short)FY_(p.y); f_trn[g]++; } sm_fly_step(&p, f_b); n++; }
        f_tr[g][f_trn[g]][0] = (short)FX_(p.x); f_tr[g][f_trn[g]][1] = (short)FY_(p.y); f_trn[g]++;
        f_ym[g] = sm_trace(g, f_b, SM_XM100); }
    f_traced = f_b;
}
static void fly_step(int rdt)
{
    int i, k, steps;
    if (f_sweep) { f_phase += rdt * mul4 * 5; f_b = 510 * (int)sintab[(f_phase >> 12) & 255] / 127; }
    f_acc += rdt * mul4 * 12 / 4;
    while (f_acc >= 1000) { f_acc -= 1000;
        for (k = 0; k < 3; k++) if (guns & (1 << k)) for (i = 0; i < MAXFP; i++) if (!fp[i].on) { fp[i].on = 1; fp[i].g = (u8)k; sm_fly_start(&fp[i].f, k); break; } }
    f_path += rdt * 2966 * mul4 / 4;                 /* 0.04525 mm a millisecond at 1 s = 500 ps */
    steps = f_path >> 15; f_path &= 32767;
    for (i = 0; i < MAXFP; i++) if (fp[i].on) {
        for (k = 0; k < steps && fp[i].f.x < 300 * 65536; k++) sm_fly_step(&fp[i].f, f_b);
        if (fp[i].f.x >= 300 * 65536 || fp[i].f.y > 165 * 65536 || fp[i].f.y < -165 * 65536) {
            if (fp[i].f.x >= 300 * 65536) for (k = 0; k < 48; k++) if (fglow[k].ttl <= 0) { fglow[k].y = (short)FY_(fp[i].f.y); fglow[k].g = fp[i].g; fglow[k].ttl = 500; break; }
            fp[i].on = 0; }
    }
    for (k = 0; k < 48; k++) if (fglow[k].ttl > 0) fglow[k].ttl -= (short)rdt;
}
static void fly_draw(void)
{
    int i, g, k = abs(f_b), c = rgbc(120 + k * 135 / 510, 100 + k * 110 / 510, 40), mn, mx, y = VY + 30; char t[60];
    if (f_traced != f_b) fly_trace();
    for (i = 0; i < 3; i++) for (g = -1; g <= 1; g += 2) { int x = 110 + i * 20, yy = CY + g * 7; box(x - 3, yy - 3, 7, 7, c); if (f_b >= 0) pset(x, yy, c); else { line(x - 2, yy - 2, x + 2, yy + 2, c); line(x + 2, yy - 2, x - 2, yy + 2, c); } }
    for (g = 0; g < 3; g++) if (guns & (1 << g)) for (i = 1; i < f_trn[g]; i++) line(f_tr[g][i - 1][0], f_tr[g][i - 1][1], f_tr[g][i][0], f_tr[g][i][1], GCD[g]);
    for (i = 0; i < MAXFP; i++) if (fp[i].on) dot2(FX_(fp[i].f.x), FY_(fp[i].f.y), GC[fp[i].g]);
    for (i = 0; i < 48; i++) if (fglow[i].ttl > 0) fill(362, fglow[i].y - 2, 4, 5, GC[fglow[i].g]);
#define L(s, col) { text8(404, y, s, col); y += 12; }
    L("Top view of a 20 inch tube", C_AMBER); L("Speed: 90,490 km/s", C_WHITE); L(" = 30.2% of light speed", C_WHITE); L("Gun to screen: 3.32 ns", C_WHITE); y += 6;
    sprintf(t, "Yoke field: %s%d.%02d mT", f_b < 0 ? "-" : "", k / 100, k % 100); L(t, C_AMBER);
    if (k < 3) { L("Path in the field: straight", C_WHITE); } else { sprintf(t, "Path: an arc, radius %d mm", 53970 / k); L(t, C_WHITE); } y += 6;
    L("At the mask the beams land:", C_AMBER);
    mn = mx = f_ym[1];
    for (g = 0; g < 3; g++) { int v = f_ym[g], a = abs(v); if (v < mn) mn = v; if (v > mx) mx = v;
        sprintf(t, " %-6s%c%d.%02d mm", gname[g], v < 0 ? '-' : '+', a >> 8, (a & 255) * 100 >> 8); L(t, (guns & (1 << g)) ? GC[g] : GRAY(7)); }
    i = mx - mn; sprintf(t, "Spread: %d.%02d mm", i >> 8, (i & 255) * 100 >> 8); L(t, i < 26 ? C_LEDG : C_LEDR); L("(it should be zero)", GRAY(13));
#undef L
    strcpy(rd[0], "The Earth's field is about 0.05 mT. The yoke needs about 5 mT for the edge.");
    sprintf(rd[1], "Spread of the beams at the mask: %d.%02d mm. A mask slot is about 0.19 mm wide.", i >> 8, (i & 255) * 100 >> 8);
    strcpy(rd[2], f_sweep ? "Sweep on. A real line takes 52 us, 15,000 flights; here it is speeded up." : "Sweep off: the field is held still, as it nearly is during one real flight.");
    strcpy(rd[3], "\x1B\x1A yoke field    S sweep on or off");
}

/* ======================= 3. the mask ======================= */
#define MAXMP 420
typedef struct { int x, ym, ys; u8 g, st; } MaskP;     /* st 0 free, 1 before the mask, 2 through, 3 stopped (kept to be counted when it would have landed) */
static MaskP mp[MAXMP];
static int m_cur = 30, m_shift = 0, m_acc; static u32 m_cnt[4];
static short m_lit[24]; static struct { short x, y, ttl; u8 c; } msp[40];
#define M_X0 (283 * 65536 + 39322)       /* left edge of the view: 283.6 mm */
#define M_XM 18766275                    /* 286.3636 mm */
#define M_XS (300 * 65536)
#define MX_(x) (20 + ((((x) - M_X0) >> 6) * 36 >> 10))
#define MY_(y) (CY - (((y) * 110) >> 12))
static int stripe_of(int ys)
{
    int u = ((ys % 3218) + 3218) % 3218;
    if (u < 322 || u > 3218 - 322) return 1;
    if (abs(u - 1073) < 322) return 2;
    if (abs(u - 2145) < 322) return 0;
    return -1;
}
static void mask_bg(void)
{
    int k, n, xm = MX_(M_XM), xs = MX_(M_XS);
    for (k = -3; k <= 3; k++) { int y0 = MY_(k * 3072 + 3072 - 389), y1 = MY_(k * 3072 + 389); fill(xm - 2, y0, 6, y1 - y0, rgbc(111, 122, 144)); }
    fill(xs, VY, SW - xs, VH, rgbc(13, 28, 36));
    for (n = -9; n <= 9; n++) { int ys = n * 1073, c = stripe_of(ys), y0 = MY_(ys + 322), y1 = MY_(ys - 322); if (c >= 0) fill(xs + 1, y0, 10, y1 - y0, GCD[c]); }
    vline(xs, VY, VY + VH - 1, rgbc(207, 214, 230));
    t8c(xm + 40, VY + 2, "shadow mask: steel, 0.15 mm", GRAY(13)); text8(xs - 250, VY + 2, "aluminium film, stripes, glass", C_WHITE);
    text8(24, VY + VH - 12, "from the guns, 286 mm to the left", GRAY(13));
    hline(40, 112, VY + VH - 30, C_WHITE); t8c(76, VY + VH - 42, "2 mm deep", C_WHITE);
    vline(150, VY + VH - 85, VY + VH - 30, C_WHITE); text8(156, VY + VH - 62, "0.5 mm across", C_WHITE);
}
static void mask_step(int rdt)
{
    int i, k, dx = rdt * 2965 * mul4 / 40;          /* 0.0045247 mm a millisecond at 1 s = 50 ps */
    m_acc += rdt * mul4 * m_cur / 6;                 /* 20 dots a second per gun at 0.30 mA */
    while (m_acc >= 1000) { m_acc -= 1000;
        for (k = 0; k < 3; k++) if (guns & (1 << k)) for (i = 0; i < MAXMP; i++) if (!mp[i].st) {
            int s = (int)(sm_rnd() + sm_rnd() + sm_rnd() + sm_rnd() + sm_rnd() + sm_rnd()) - 196608, ym = (s * 181) >> 12;
            if (abs(ym) > 6200) break;
            mp[i].st = 1; mp[i].g = (u8)k; mp[i].x = M_X0 + 6000; mp[i].ym = ym;
            mp[i].ys = ym + ym * 195 / 4096 - (k == 0 ? 1073 : k == 2 ? -1073 : 0) + m_shift; break; } }
    for (i = 0; i < MAXMP; i++) if (mp[i].st) {
        int x1 = mp[i].x + dx;
        if (mp[i].st == 1 && x1 >= M_XM) {
            int kk = (mp[i].ym + (mp[i].ym >= 0 ? 1536 : -1536)) / 3072;
            if (abs(mp[i].ym - kk * 3072) > 389) { mp[i].st = 3; for (k = 0; k < 40; k++) if (msp[k].ttl <= 0) { msp[k].x = (short)(MX_(M_XM) - 4); msp[k].y = (short)MY_(mp[i].ym); msp[k].ttl = 250; msp[k].c = (u8)rgbc(255, 144, 64); break; } }
            else mp[i].st = 2;
        }
        if (x1 >= M_XS) {
            if (mp[i].st == 3) m_cnt[0]++;
            else { int s = stripe_of(mp[i].ys);
                if (s < 0) m_cnt[3]++; else { int n = (mp[i].ys + (mp[i].ys >= 0 ? 536 : -536)) / 1073 + 12; m_cnt[s == mp[i].g ? 1 : 2]++; if (n >= 0 && n < 24) m_lit[n] = 400; } }
            mp[i].st = 0; continue;
        }
        mp[i].x = x1;
    }
    for (k = 0; k < 40; k++) if (msp[k].ttl > 0) msp[k].ttl -= (short)rdt;
    for (k = 0; k < 24; k++) if (m_lit[k] > 0) m_lit[k] -= (short)rdt;
}
static void mask_draw(void)
{
    int i, n, xs = MX_(M_XS); u32 tot = m_cnt[0] + m_cnt[1] + m_cnt[2] + m_cnt[3]; char a[40];
    for (n = 0; n < 24; n++) if (m_lit[n] > 0) { int ys = (n - 12) * 1073, c = stripe_of(ys), y0 = MY_(ys + 322), y1 = MY_(ys - 322); if (c >= 0) { fill(xs + 1, y0, 10, y1 - y0, GC[c]); fill(xs + 12, y0 - 1, 20, y1 - y0 + 2, GCD[c]); } }
    for (i = 0; i < MAXMP; i++) if (mp[i].st == 1 || mp[i].st == 2) {
        int rel = (mp[i].x - M_XM) >> 8;                      /* 1/256 mm from the mask; mask to screen is 3491 */
        dot2(MX_(mp[i].x), MY_(mp[i].ym + (mp[i].ys - mp[i].ym) * rel / 3491), GC[mp[i].g]);
    }
    for (i = 0; i < 40; i++) if (msp[i].ttl > 0) dot2(msp[i].x, msp[i].y, msp[i].c);
    n = (guns & 1) + ((guns >> 1) & 1) + ((guns >> 2) & 1);
    if (tot < 200) { strcpy(rd[0], "Counting the first electrons to land..."); rd[1][0] = 0; }
    else {
        u32 mw = (u32)m_cur * 10 * n * 25;                    /* milliwatts: current per gun in units of 10 uA, 25 kV */
        sprintf(rd[0], "Mask stopped %lu%%   Own colour %lu%%   Wrong colour %lu%%   Lost in the black %lu%%", m_cnt[0] * 100 / tot, m_cnt[1] * 100 / tot, m_cnt[2] * 100 / tot, m_cnt[3] * 100 / tot);
        sprintf(rd[1], "Beam power %lu.%lu W. About %lu.%lu W of it heats the mask and never makes light.", mw / 1000, mw % 1000 / 100, mw * m_cnt[0] / tot / 1000, mw * m_cnt[0] / tot % 1000 / 100);
    }
    (void)a;
    sprintf(rd[2], "Per gun %d.%02d mA. One dot is about 4,700 electrons. Landing shift %s%d.%03d mm.", m_cur / 100, m_cur % 100, m_shift < 0 ? "-" : "", abs(m_shift) * 1000 / 4096 / 1000, abs(m_shift) * 1000 / 4096 % 1000);
    strcpy(rd[3], "\x18\x19 beam current    \x1B\x1A stray magnetic field (a purity fault)");
}

/* ======================= 4. the phosphor ======================= */
#define MAXSHOT 15
typedef struct { const char *name, *act, *l0, *l1; int mat, eg10, eta, hv100, tau_us; u8 r, g, b; } Phos;
static const Phos PH[3] = {
    { "green: zinc sulphide with copper", "Cu", "Zn", "S", 1, 37, 20, 234, 60, 93, 255, 122 },
    { "blue: zinc sulphide with silver", "Ag", "Zn", "S", 1, 37, 20, 276, 25, 90, 140, 255 },
    { "red: yttrium oxysulphide with europium", "Eu", "Y", "O", 2, 47, 13, 203, 900, 255, 90, 74 } };
static SmPt p_pts[MAXSHOT][SM_MAXPT], p_sp[MAXSHOT][SM_MAXSP];
static SmTrack p_tr[MAXSHOT];
static int p_c = 0, p_n = 5, p_auto = 1, p_phase, p_t16, p_tmax, p_wait, p_lt, p_photons, p_nem, p_ei;
static u32 p_tot[5];                       /* electrons, eV in aluminium, in phosphor, carried back, electrons that came back */
static struct { short x, y; u16 at; } p_em[MAXSHOT * SM_MAXSP];
static struct { short x0, y0, x1, y1, x2, y2, ttl; } p_ray[40];
#define PX_(nm) (90 + ((nm) * 38 / 1000))
#define PY_(nm) (CY - ((nm) * 38 / 1000))
static void phos_fire(void)
{
    int i, k, j;
    p_tmax = 0; p_nem = 0;
    for (i = 0; i < p_n; i++) {
        int y0 = ((int)(sm_rnd() + sm_rnd() + sm_rnd() + sm_rnd()) - 131072) * 1100 / 37837;      /* spread 1.1 um */
        sm_track(PH[p_c].mat, y0, p_pts[i], p_sp[i], &p_tr[i]);
        if (p_tr[i].tend > p_tmax) p_tmax = p_tr[i].tend;
        p_tot[0]++; p_tot[1] += p_tr[i].e_al; p_tot[2] += p_tr[i].e_ph; p_tot[3] += p_tr[i].e_back; if (p_tr[i].e_back) p_tot[4]++;
        for (k = 0; k < p_tr[i].nsp; k++) if (sm_rnd() & 1) { p_em[p_nem].x = p_sp[i][k].x; p_em[p_nem].y = p_sp[i][k].y; p_em[p_nem].at = (u16)((1500u * sm_ln[sm_rnd() & 255]) >> 8); p_nem++; }
        sys_idle();
    }
    for (i = 1; i < p_nem; i++) for (j = i; j > 0 && p_em[j - 1].at > p_em[j].at; j--) { short tx = p_em[j].x, ty = p_em[j].y; u16 ta = p_em[j].at; p_em[j] = p_em[j - 1]; p_em[j - 1].x = tx; p_em[j - 1].y = ty; p_em[j - 1].at = ta; }
    p_phase = 0; p_t16 = 0; p_ei = 0; p_photons = 0; p_lt = 0;
    for (i = 0; i < 40; i++) p_ray[i].ttl = 0;
}
static void phos_bg(void)
{
    static const short gr[7][3] = { {3200, -2900, 3200}, {3100, 3300, 3100}, {8600, 300, 3000}, {8500, 6200, 3200}, {8800, -5700, 3100}, {13800, 3200, 3200}, {13900, -2900, 3100} };
    int i, xf = PX_(0), xe = PX_(200);
    for (i = 0; i < 7; i++) { disc(PX_(gr[i][0]), PY_(gr[i][1]), gr[i][2] * 38 / 1000, GRAY(3)); }
    for (i = 0; i < 7; i++) circle(PX_(gr[i][0]), PY_(gr[i][1]), gr[i][2] * 38 / 1000, GRAY(6));
    fill(0, VY, xf, VH, 0); fill(xf, VY, xe - xf + 1, VH, rgbc(170, 180, 200));
    text8(xf - 60, VY + 4, "vacuum", GRAY(13)); text8(xf - 86, VY + 16, "aluminium", C_WHITE); text8(xf - 86, VY + 26, "0.2 um", C_WHITE);
    text8(xe + 20, VY + 4, "phosphor grains, each about 6 um across", GRAY(13)); text8(SW - 210, CY - 4, "to the glass and your eye", GRAY(13));
    scalebar(76, "2 um (1/500 mm)");
}
static void phos_step(int rdt)
{
    int i, k;
    if (p_phase == 0) { p_t16 += rdt * mul4 / 10; if (p_t16 > p_tmax) { p_phase = 1; p_wait = 1800; } }
    else if (p_phase == 1) { p_wait -= rdt; if (p_wait <= 0) p_phase = 2; }
    else if (p_phase == 2) {
        p_lt += rdt * mul4 / 4;
        while (p_ei < p_nem && p_em[p_ei].at <= p_lt) {
            int a = sm_rnd() & 255, dx = COS(a), dy = SIN(a), x = p_em[p_ei].x, y = p_em[p_ei].y; p_ei++;
            for (k = 0; k < 40; k++) if (p_ray[k].ttl <= 0) break;
            if (k == 40) continue;
            p_ray[k].x0 = (short)PX_(x); p_ray[k].y0 = (short)PY_(y); p_ray[k].ttl = 420;
            if (dx < -3) { int yb = y + dy * (x - 200) / (-dx);                    /* toward the film: reflected, mostly */
                p_ray[k].x1 = (short)PX_(200); p_ray[k].y1 = (short)PY_(yb);
                if ((sm_rnd() & 255) < 225) { p_ray[k].x2 = (short)PX_(15000); p_ray[k].y2 = (short)PY_(yb + dy * 14800 / (-dx)); p_photons++; } else { p_ray[k].x2 = p_ray[k].x1; p_ray[k].y2 = p_ray[k].y1; } }
            else { p_ray[k].x1 = (short)PX_(x + dx * 130); p_ray[k].y1 = (short)PY_(y + dy * 130); p_ray[k].x2 = p_ray[k].x1; p_ray[k].y2 = p_ray[k].y1; p_photons++; }
        }
        if (p_lt > 6000) { p_phase = 3; p_wait = 1500; }
    } else { p_wait -= rdt; if (p_wait <= 0 && p_auto) phos_fire(); }
    for (i = 0; i < 40; i++) if (p_ray[i].ttl > 0) p_ray[i].ttl -= (short)rdt;
}
static void phos_draw(void)
{
    const Phos *P = &PH[p_c]; int i, k, tt = p_phase == 0 ? p_t16 : 0x7FFFFFFF, col = rgbc(P->r, P->g, P->b), dim = p_phase >= 2;
    int c_hi = dim ? GRAY(9) : C_WHITE, c_mid = dim ? GRAY(8) : rgbc(255, 220, 140), c_lo = dim ? GRAY(7) : rgbc(255, 150, 60), c_sp = dim ? rgbc(120, 90, 40) : rgbc(255, 200, 100);
    char a[40], b[40]; u32 n = p_tot[0] ? p_tot[0] : 1, eph = p_tot[2] / n;
    clip(0, VY, SW - 1, VY + VH - 1);
    for (i = 0; i < p_n; i++) {
        const SmPt *q = p_pts[i]; int n2 = p_tr[i].n, hx = -1, hy = 0;
        line(PX_(-2200), PY_(q[0].y), PX_(q[0].x), PY_(q[0].y), tt > 0 ? c_hi : GRAY(6));
        for (k = 1; k < n2 && (int)q[k - 1].t <= tt; k++) {
            int x1 = q[k].x, y1 = q[k].y;
            if ((int)q[k].t > tt) { int f = (tt - q[k - 1].t) * 256 / (q[k].t - q[k - 1].t + 1); x1 = q[k - 1].x + (x1 - q[k - 1].x) * f / 256; y1 = q[k - 1].y + (y1 - q[k - 1].y) * f / 256; hx = PX_(x1); hy = PY_(y1); }
            line(PX_(q[k - 1].x), PY_(q[k - 1].y), PX_(x1), PY_(y1), q[k - 1].e > 15000 ? c_hi : q[k - 1].e > 6000 ? c_mid : c_lo);
        }
        if (hx >= 0) { fill(hx - 2, hy - 2, 5, 5, C_WHITE); }
        for (k = 0; k < p_tr[i].nsp; k++) if ((int)p_sp[i][k].t <= tt) dot2(PX_(p_sp[i][k].x), PY_(p_sp[i][k].y), c_sp);
    }
    for (i = 0; i < 40; i++) if (p_ray[i].ttl > 0) {
        line(p_ray[i].x0, p_ray[i].y0, p_ray[i].x1, p_ray[i].y1, col);
        if (p_ray[i].x2 != p_ray[i].x1 || p_ray[i].y2 != p_ray[i].y1) line(p_ray[i].x1, p_ray[i].y1, p_ray[i].x2, p_ray[i].y2, col);
        fill(p_ray[i].x0 - 1, p_ray[i].y0 - 1, 3, 3, col);
    }
    unclip();
    if (p_phase == 1) {
        fill(120, CY - 34, 400, 68, C_INK); box(120, CY - 34, 400, 68, C_AMBER);
        sprintf(a, "The electrons have stopped. That took %d fs.", p_tmax / 16); t8c(320, CY - 24, a, C_AMBER);
        t8c(320, CY - 6, "The light comes out about a billion times", C_WHITE); t8c(320, CY + 6, "more slowly. The clock now changes to match.", C_WHITE);
    }
    sprintf(rd[0], "Phosphor: %s. %s", P->name, p_phase == 0 ? "Now: the electrons slowing down." : p_phase == 2 ? "Now: light leaving." : "");
    sprintf(rd[1], "Mean of %lu: %lu.%02lu keV in the film, %lu.%lu keV in the phosphor, %lu%% bounce back out.",
        p_tot[0], p_tot[1] / n / 1000, p_tot[1] / n % 1000 / 10, eph / 1000, eph % 1000 / 100, p_tot[4] * 100 / n);
    big(a, eph * 10 / (3 * P->eg10), 0); big(b, eph * P->eta / P->hv100, 0);
    sprintf(rd[2], "Each electron: about %s pairs, about %s photons. %d%% becomes heat.", a, b, 100 - P->eta);
    strcpy(rd[3], "F fire   N electrons per shot   P phosphor colour   A repeat on or off");
}

/* ======================= 5. atoms (a diagram) ======================= */
static int a_t;
#define ASP 29
#define AX0 44
#define AY0 (VY + 44)
#define ACX 13
#define ACY 4
static void atoms_bg(void)
{
    const Phos *P = &PH[p_c]; int i, j;
    for (j = 0; j < 9; j++) for (i = 0; i < 20; i++) {
        int x = AX0 + i * ASP, y = AY0 + j * ASP, cat = ((i + j) & 1) == 0;
        if (i == ACX && j == ACY) { disc(x, y, 11, rgbc(232, 232, 240)); text8(x - 8, y - 4, P->act, C_BLACK); }
        else { disc(x, y, cat ? 8 : 10, cat ? rgbc(93, 111, 150) : rgbc(184, 154, 58)); }
    }
    text8(8, VY + 4, "DIAGRAM, NOT A SIMULATION.", C_AMBER);
    { char t[80]; sprintf(t, "Blue-grey: %s. Yellow: %s. White: the activator, about 1 atom in 10,000.", P->l0, P->l1); text8(8, VY + 16, t, GRAY(13)); }
    scalebar(ASP * 2, "0.54 nm");
}
static void a_walk(int x0, int y0, int x1, int y1, int t, int t0, int t1, int seed, int *x, int *y)
{
    int f = t <= t0 ? 0 : t >= t1 ? 256 : (t - t0) * 256 / (t1 - t0), w = (256 - f) * (int)sintab[(f * 3 + seed) & 255] / 127 * 22 / 256;
    *x = x0 + (x1 - x0) * f / 256 + w * (int)sintab[(seed * 3) & 255] / 127; *y = y0 + (y1 - y0) * f / 256 + w * (int)sintab[(seed * 5 + 64) & 255] / 127;
}
static void atoms_draw(void)
{
    const Phos *P = &PH[p_c]; int t = a_t, x, y, ax = AX0 + ACX * ASP, ay = AY0 + ACY * ASP, col = rgbc(P->r, P->g, P->b), ce = rgbc(159, 232, 255), ch = rgbc(255, 176, 96);
    int py = AY0 + 2 * ASP + ASP / 2;
    if (t < 2000) { int hx = -40 + t * (SW + 80) / 2000; line(hx - 160, py - 12, hx, py + 12, C_WHITE); line(hx - 160, py - 11, hx, py + 13, GRAY(14)); fill(hx - 2, py + 10, 5, 5, C_WHITE); }
    if (t > 900 && t < 12400) {          /* pair 1: its hole goes to the activator */
        a_walk(AX0 + 4 * ASP, AY0 + 2 * ASP, ax, ay, t, 2200, 8600, 33, &x, &y); circle(x, y, 5, ch); circle(x, y, 4, ch);
        if (t < 3400) { a_walk(AX0 + 4 * ASP, AY0 + 2 * ASP, AX0 + 1 * ASP, AY0 + 6 * ASP, t, 900, 3400, 57, &x, &y); disc(x, y, 3, ce); } }
    if (t > 1300 && t < 12400) {         /* pair 2: its electron goes to the activator */
        a_walk(AX0 + 9 * ASP, AY0 + 3 * ASP, ax, ay, t, 3000, 11600, 105, &x, &y); disc(x, y, 3, ce);
        if (t < 3600) { a_walk(AX0 + 9 * ASP, AY0 + 3 * ASP, AX0 + 12 * ASP, AY0 + 7 * ASP, t, 1300, 3600, 133, &x, &y); circle(x, y, 5, ch); } }
    if (t > 1100 && t < 12200) {         /* pair 3: meets at a flaw, and makes heat */
        a_walk(AX0 + 6 * ASP, AY0 + 3 * ASP, AX0 + 6 * ASP, AY0 + 6 * ASP, t, 2200, 11800, 74, &x, &y); circle(x, y, 5, ch);
        a_walk(AX0 + 6 * ASP, AY0 + 3 * ASP, AX0 + 6 * ASP, AY0 + 6 * ASP, t, 2600, 12200, 161, &x, &y); disc(x, y, 3, ce); }
    if (t > 12200 && t < 15000) { int k; for (k = 0; k < 3; k++) circle(AX0 + 6 * ASP, AY0 + 6 * ASP, 12 + k * 5 + ((t / 120) & 3), rgbc(255, 120, 60)); text8(AX0 + 6 * ASP - 16, AY0 + 6 * ASP + 30, "heat", rgbc(255, 120, 60)); }
    if (t > 9000 && t < 12600) { circle(ax, ay, 14, C_WHITE); circle(ax, ay, 16, GRAY(14)); }
    if (t > 12400 && t < 16000) {        /* the photon */
        int k = (t - 12400) * 420 / 3600, s, lx = -1, ly = 0;
        for (s = 0; s < 70; s++) { int xx = ax + k - s * 2, yy = ay - (k - s * 2) / 8 + (int)sintab[(s * 14) & 255] * 8 / 127 * (int)sintab[(s * 128 / 70) & 255] / 127;
            if (xx < ax) break;
            if (lx >= 0) { line(lx, ly, xx, yy, col); line(lx, ly + 1, xx, yy + 1, col); } lx = xx; ly = yy; }
        if (t < 13000) { circle(ax, ay, 12 + (13000 - t) / 40, col); circle(ax, ay, 13 + (13000 - t) / 40, col); }
    }
    if (t < 2000) { strcpy(rd[0], "1. The beam electron passes, crossing this picture in 0.0001 ps. Its electric"); strcpy(rd[1], "   field pulls electrons off atoms near its path."); }
    else if (t < 9000) { strcpy(rd[0], "2. Each atom that lost an electron has a hole (orange ring). Freed electrons"); strcpy(rd[1], "   (blue) and holes wander through the crystal: picoseconds to nanoseconds."); }
    else if (t < 12400) { sprintf(rd[0], "3. A hole is caught at the %s atom, then an electron. They wait about %d us.", P->act, P->tau_us); strcpy(rd[1], "   Elsewhere a pair meets at a flaw in the crystal: that energy becomes heat."); }
    else if (t < 16000) { sprintf(rd[0], "4. The electron drops into the hole: one photon of light leaves, %d.%02d eV.", P->hv100 / 100, P->hv100 % 100); strcpy(rd[1], "   The pair that met at a flaw makes the atoms shake instead."); }
    else { strcpy(rd[0], "One 25 keV electron does this about two thousand times before it stops."); rd[1][0] = 0; }
    sprintf(rd[2], "Phosphor: %s. The activator sets the colour.", P->name);
    strcpy(rd[3], "P phosphor (green, blue, red)");
}

/* ======================= the screen ======================= */
static const char *about[NVIEW] = {
 "THE GUN. Heat shakes electrons out of the cathode coating; they drift off at about 200 km/s. Grid 1, a plate with a small hole held negative, pushes most of them back: its voltage is the brightness. Each later electrode is more positive, and the electron falls toward it. Inside the 25,000 V anode it is at 30% of the speed of light, and it keeps that speed to the screen. The dots crowd at the cathode because they start so slowly.",
 "THE FLIGHT. Past the gun the electrons coast. The yoke's magnetic field cannot speed them up or slow them down, only turn them: inside the field each path is part of a circle. The guns are aimed to cross at the mask. Move the field off zero and read the spread: each beam enters the field at a different place and angle, so they no longer cross together. That is misconvergence. A real yoke shapes its field to cancel it; rings and strips trim the rest.",
 "THE MASK. The beam is far wider than one phosphor stripe, so it cannot be aimed at one colour. The mask does the aiming: the beams come through each slot from three directions about one degree apart, so behind a slot the red gun sees only red phosphor. The price: three electrons in four hit steel and only make heat. Add a stray field: first the black between the stripes absorbs the error, then one gun lights its neighbour's colour. That is a purity fault.",
 "INTO THE PHOSPHOR. The aluminium film is a mirror for the light and carries the charge away; a 25 keV electron passes through it almost untouched. In the phosphor the atoms knock it about hundreds of times and it stops within a few thousandths of a millimetre. The specks are where it left energy. That takes a tenth of a picosecond. The light takes tens of microseconds: the loosened electrons must find an activator atom and wait there.",
 "ATOMS. This smallest view is a diagram: events this small cannot honestly be drawn as little balls, but the sizes and the order of events are right. A phosphor is a crystal, not molecules: two kinds of atom in a repeating pattern, plus a trace of a third added on purpose. That trace, the activator, is where the light is made: silver gives blue, copper green, europium red. Most pairs never reach one, so four fifths of the energy is heat." };

static void clock_line(char *t)
{
    static const char *unit[NVIEW] = { "200 ps", "500 ps", "50 ps", "25 fs", "" };
    static const u32 slow[NVIEW] = { 50, 20, 200, 4000, 0 };      /* slowed by this many times 10^8, 10^8, 10^8, 10^10 */
    char a[40];
    if (view == V_ATOMS) { strcpy(t, "No single clock here: each step gives its own duration."); return; }
    if (view == V_PHOS && p_phase >= 2) { sprintf(t, "1 second here = %d us.  Speed x%d.%02d", PH[p_c].tau_us * 2 / 3, mul4 / 4, mul4 % 4 * 25); return; }
    big(a, slow[view], view == V_PHOS ? 10 : 8);
    sprintf(t, "1 second here = %s (slowed %s times).  Speed x%d.%02d%s", unit[view], a, mul4 / 4, mul4 % 4 * 25, paused ? "  PAUSED" : "");
}
static void set_view(int v)
{
    int i;
    view = v; dirty_bg = 1;
    for (i = 0; i < MAXGP; i++) gp[i].on = 0;
    for (i = 0; i < MAXFP; i++) fp[i].on = 0;
    for (i = 0; i < MAXMP; i++) mp[i].st = 0;
    memset(m_cnt, 0, sizeof m_cnt); memset(m_lit, 0, sizeof m_lit); memset(msp, 0, sizeof msp); memset(fglow, 0, sizeof fglow);
    f_traced = 99999; a_t = 0;
    if (v == V_PHOS) { memset(p_tot, 0, sizeof p_tot); phos_fire(); }
}
static void frame_static(void)        /* everything outside the view */
{
    int i, x = 4; char t[100];
    unclip(); fill(0, 0, SW, VY, C_INK); fill(0, VY + VH, SW, SH - VY - VH, C_INK);
    for (i = 0; i < NVIEW; i++) { text(x, 1, vname[i], i == view ? C_AMBER : GRAY(11)); x += (int)strlen(vname[i]) * 8 + 16; }
    text8(SW - 8 * 23, 5, "SLOW MOTION CRT   ESC", GRAY(13));
    wrap(8, 410, 78, 6, about[view], C_WHITE);
    sprintf(t, "1-5 view  SPACE pause  +/- speed  R G B guns (%c%c%c)  F12 picture", guns & 1 ? 'R' : '-', guns & 2 ? 'G' : '-', guns & 4 ? 'B' : '-');
    text8(8, 470, t, GRAY(13));
}
static void make_bg(void)
{
    unclip(); fill(0, VY, SW, VH, rgbc(3, 5, 10));
    clip(0, VY, SW - 1, VY + VH - 1);
    switch (view) { case V_GUN: gun_bg(); break; case V_FLIGHT: fly_bg(); break; case V_MASK: mask_bg(); break; case V_PHOS: phos_bg(); break; default: atoms_bg(); }
    unclip(); save_bg(); frame_static(); dirty_bg = 0;
}
void slowmo(void)
{
    u32 last, now, rdl = 0; int k, rdt, i; char t[100];
    if (!bg) bg = (u8 *)malloc((u32)SW * VH);
    if (!bg) return;
    GC[0] = rgbc(255, 90, 74); GC[1] = rgbc(93, 255, 122); GC[2] = rgbc(90, 140, 255);
    GCD[0] = rgbc(110, 40, 30); GCD[1] = rgbc(30, 110, 50); GCD[2] = rgbc(40, 60, 130);
    snd_song(SONG_NONE); pal_apply(64, 7); sm_seed((unsigned)ms_now() | 1); if (auto_mode) sm_seed(12345);
    paused = 0; set_view(view);
    last = ms_now();
    for (;;) {
        if (dirty_bg) { make_bg(); rdl = 0; }
        now = ms_now(); rdt = (int)(now - last); last = now;
        if (rdt > 80) rdt = 80;
        if (auto_mode) rdt = 40;
        if (!paused && rdt > 0) switch (view) {
            case V_GUN: gun_step(rdt); break; case V_FLIGHT: fly_step(rdt); break; case V_MASK: mask_step(rdt); break;
            case V_PHOS: phos_step(rdt); break; default: a_t += rdt * mul4 / 4; if (a_t > 19000) a_t = 0; }
        put_bg();
        switch (view) { case V_GUN: gun_draw(); break; case V_FLIGHT: fly_draw(); break; case V_MASK: mask_draw(); break; case V_PHOS: phos_draw(); break; default: atoms_draw(); }
        if (now - rdl > 250 || !rdl) {            /* the words change four times a second, not every frame */
            rdl = now ? now : 1; fill(0, VY + VH, SW, 56, C_INK);
            clock_line(t); text8(8, VY + VH + 3, t, C_AMBER);
            for (i = 0; i < 4; i++) text8(8, VY + VH + 15 + i * 10, rd[i], i == 3 ? C_SEL : C_WHITE);
        }
        vid_vsync(); vid_present();
        while ((k = key_get()) != 0) {
            if (k == K_ESC) { snd_song(SONG_TITLE); return; }
            if (k >= '1' && k <= '5') { if (k - '1' != view) set_view(k - '1'); }
            else if (k == K_TAB) set_view((view + 1) % NVIEW);
            else if (k == ' ') { paused ^= 1; rdl = 0; }
            else if (k == '+' || k == '=') { if (mul4 < 16) mul4 *= 2; rdl = 0; }
            else if (k == '-') { if (mul4 > 1) mul4 /= 2; rdl = 0; }
            else if (k == 'r' || k == 'R') { guns ^= 1; dirty_bg = 1; }
            else if (k == 'g' || k == 'G') { guns ^= 2; dirty_bg = 1; }
            else if (k == 'b' || k == 'B') { guns ^= 4; dirty_bg = 1; }
            else if (k == K_F12) vid_shot("SLOWMO.BMP");
            else if (view == V_GUN && (k == K_UP || k == K_DOWN)) { g_bright += k == K_UP ? 5 : -5; if (g_bright < 0) g_bright = 0; if (g_bright > 100) g_bright = 100; dirty_bg = 1; }
            else if (view == V_FLIGHT && (k == K_LEFT || k == K_RIGHT)) { f_sweep = 0; f_b += k == K_RIGHT ? 15 : -15; if (f_b > 510) f_b = 510; if (f_b < -510) f_b = -510; }
            else if (view == V_FLIGHT && (k == 's' || k == 'S')) f_sweep ^= 1;
            else if (view == V_MASK && (k == K_UP || k == K_DOWN)) { m_cur += k == K_UP ? 5 : -5; if (m_cur < 5) m_cur = 5; if (m_cur > 100) m_cur = 100; }
            else if (view == V_MASK && (k == K_LEFT || k == K_RIGHT)) { m_shift += k == K_RIGHT ? 41 : -41; if (m_shift > 615) m_shift = 615; if (m_shift < -615) m_shift = -615; memset(m_cnt, 0, sizeof m_cnt); }
            else if (view == V_PHOS && (k == 'f' || k == 'F')) phos_fire();
            else if (view == V_PHOS && (k == 'n' || k == 'N')) { p_n = p_n == 1 ? 5 : p_n == 5 ? MAXSHOT : 1; phos_fire(); }
            else if (view == V_PHOS && (k == 'a' || k == 'A')) p_auto ^= 1;
            else if ((view == V_PHOS || view == V_ATOMS) && (k == 'p' || k == 'P')) { p_c = (p_c + 1) % 3; memset(p_tot, 0, sizeof p_tot); if (view == V_PHOS) phos_fire(); else { a_t = 0; dirty_bg = 1; } }
            if (auto_mode) break;
        }
    }
}

/* /BENCH: milliseconds for one frame of each view, after it has filled with electrons */
void slowmo_bench(u32 *t)
{
    int v, i; u32 t0;
    if (!bg) bg = (u8 *)malloc((u32)SW * VH);
    GC[0] = rgbc(255, 90, 74); GC[1] = rgbc(93, 255, 122); GC[2] = rgbc(90, 140, 255); GCD[0] = rgbc(110, 40, 30); GCD[1] = rgbc(30, 110, 50); GCD[2] = rgbc(40, 60, 130);
    sm_seed(12345);
    for (v = 0; v < NVIEW; v++) {
        set_view(v); make_bg();
        for (i = 0; i < 150; i++) switch (v) { case V_GUN: gun_step(40); break; case V_FLIGHT: fly_step(40); break; case V_MASK: mask_step(40); break; case V_PHOS: phos_step(4); break; default: a_t = 5000; }
        t0 = ms_now();
        for (i = 0; i < 8; i++) {
            switch (v) { case V_GUN: gun_step(40); break; case V_FLIGHT: fly_step(40); break; case V_MASK: mask_step(40); break; case V_PHOS: phos_step(4); break; default: a_t += 40; }
            put_bg();
            switch (v) { case V_GUN: gun_draw(); break; case V_FLIGHT: fly_draw(); break; case V_MASK: mask_draw(); break; case V_PHOS: phos_draw(); break; default: atoms_draw(); }
            vid_present();
        }
        t[v] = (ms_now() - t0) / 8;
    }
    t0 = ms_now(); for (i = 0; i < 4; i++) phos_fire(); t[5] = (ms_now() - t0) / 4;
    memset(p_tot, 0, sizeof p_tot); p_n = MAXSHOT; for (i = 0; i < 40; i++) phos_fire(); p_n = 5;
    t[6] = p_tot[4] * 100 / p_tot[0]; t[7] = p_tot[2] / p_tot[0];
}
