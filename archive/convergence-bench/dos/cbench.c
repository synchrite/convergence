/* The Convergence Bench: the same game as the browser edition's main page.
   You stand behind a colour set with its back off. The picture is in a mirror on the left, reversed.
   On the right is the neck of the tube, in one of two views: from behind (ring tabs, the glass round
   the yoke) or from the side (ring pairs, yoke on the funnel, a strip going in under it).

   The model, in whole numbers. Offsets are in units of 1/127 pixel (U). Screen position u, v is in
   hundredths, -100..100, v down.
     Ring pair: two tabs at a and b (48 steps of 7.5 degrees). Vector = 7 px * (unit(a) - unit(b)) / 2.
       purity (2-pole): all three beams.  4-pole: red +, blue -.  6-pole: red and blue both +.
     Yoke tilt: red and blue cross in proportion to distance from the centre.
     Strip: a place round the tube (72 steps of 5 degrees), a depth under the yoke (1..5), a face (+ or -),
       and tape. It parts red and blue in the part of the picture nearest to it, with a cubic falloff. */
#include "g.h"

#define UPP 127
#define TOL 159            /* 1.25 px */
#define NCJ 12
#define SHEET 4

enum { T_PU = 1, T_P4 = 2, T_P6 = 4, T_TILT = 8, T_STRIP = 16, T_DEG = 32, T_LOCK = 64 };
enum { DG_STAT, DG_DYN, DG_PUR, DG_MAG };
enum { P_GRID, P_DOTS, P_RED };
enum { H_PU, H_P4, H_P6, H_YOKE, H_STRIP, NHAND };
enum { PH_DIAG, PH_WORK, PH_DONE };

typedef struct {
    char type; const char *name, *cust; int tools;
    int pu[2], p4[2], p6[2], tilt[2], sang, samt, mag, pat, par;
    int ndiag, diag[4], ans; const char *intro, *why;
} CJob;

static const char *dtext[4] = {
    "Static convergence: the colours are apart all over the picture, centre included",
    "Dynamic convergence: the centre is right, the edges are not",
    "Purity: the colours are in register but a field of one colour is tinted",
    "A magnetised shadow mask: patches of wrong colour" };

static const CJob cj[NCJ] = {
 { 'A', "Red and blue", "\"There is a red shadow on one side of everything and a blue one on the other.\"",
   T_P4, {0,0}, {6,18}, {0,0}, {0,0}, 0, 0, 0, P_GRID, 2, 0, {0}, 0,
   "The 4-pole rings move red and blue in OPPOSITE directions. Spread the two magenta tabs apart (Up) and watch red and blue slide toward each other in the mirror. Then turn the pair (Left, Right) until they close up.", 0 },
 { 'B', "Red and blue again", "\"Same trouble as next door's set.\"",
   T_P4, {0,0}, {40,4}, {0,0}, {0,0}, 0, 0, 0, P_GRID, 2, 0, {0}, 0,
   "The same fault in another direction. Spread first to see which way the pair pushes, then turn it.", 0 },
 { 'A', "Magenta on green", "\"Everything has a green edge.\"",
   T_P6, {0,0}, {0,0}, {30,42}, {0,0}, 0, 0, 0, P_GRID, 2, 0, {0}, 0,
   "Red and blue already lie on each other and make magenta. The 6-pole rings (cyan tabs) move them TOGETHER. Lay the magenta on the green.", 0 },
 { 'C', "Both rings", "\"Three of everything.\"",
   T_P4 | T_P6, {0,0}, {10,22}, {2,38}, {0,0}, 0, 0, 0, P_DOTS, 4, 3, {DG_STAT, DG_DYN, DG_PUR}, DG_STAT,
   "Do it in the order a technician does. Switch green off (8): bring red and blue together with the 4-pole. Switch green on: lay the magenta on it with the 6-pole.",
   "The split is the same at the centre as at the edges, so it is static convergence, the job of the rings." },
 { 'D', "Mr Okafor's 20 inch", "\"Since the grandchildren were at it, the writing has coloured edges.\"",
   T_P4 | T_P6 | T_LOCK, {0,0}, {44,14}, {20,26}, {0,0}, 0, 0, 0, P_GRID, 5, 4, {DG_PUR, DG_STAT, DG_DYN, DG_MAG}, DG_STAT,
   "A customer's set. Finish it properly: when it is right, lock it (K).",
   "Every line is split by the same amount, centre and edges alike: static convergence." },
 { 'A', "A tinted red", "\"Red is not red on one side.\"",
   T_PU, {12,24}, {0,0}, {0,0}, {0,0}, 0, 0, 0, P_RED, 2, 0, {0}, 0,
   "This is the red field. If the red beam lands beside its own phosphor, the red is tinted. The purity rings (yellow tabs) move all three beams together. Spread and turn them until the whole field is pure red.", 0 },
 { 'A', "The loudspeaker", "\"A purple cloud appeared after I moved my speakers.\"",
   T_PU | T_DEG, {0,0}, {0,0}, {0,0}, {0,0}, 0, 0, 1, P_RED, 1, 0, {0}, 0,
   "A patch of wrong colour that the purity rings cannot shift: the shadow mask is magnetised. Press D to degauss. On a real set this is always the first thing you do.", 0 },
 { 'C', "Purity, then convergence", "\"Colours wrong and edges fuzzy with colour.\"",
   T_PU | T_P4 | T_P6 | T_DEG, {30,40}, {8,16}, {0,0}, {0,0}, 0, 0, 1, P_RED, 5, 3, {DG_MAG, DG_PUR, DG_STAT}, DG_MAG,
   "Three faults. The order matters: degauss, then purity on the red field, then convergence on the crosshatch.",
   "There is a patch, not an even tint. Patches mean a magnetised mask, and it must be cleared before anything else can be judged." },
 { 'A', "The slipped yoke", "\"Fine in the middle, coloured edges top and bottom.\"",
   T_TILT, {0,0}, {0,0}, {0,0}, {4,0}, 0, 0, 0, P_GRID, 1, 0, {0}, 0,
   "The centre is white but the top and bottom split in opposite directions. No ring can do that: rings treat the whole picture alike. The yoke has tilted. Move the dot in the YOKE TILT pad with the arrow keys until the edges close.", 0 },
 { 'C', "Centre first", "\"Fringes everywhere, worse at the sides.\"",
   T_P4 | T_TILT, {0,0}, {16,28}, {0,0}, {0,-4}, 0, 0, 0, P_GRID, 3, 3, {DG_DYN, DG_STAT, DG_PUR}, DG_STAT,
   "Two faults at once. Get the CENTRE right with the ring first. Only then can you see what the yoke is doing at the edges. TAB changes hands.",
   "The centre is split, so the rings must be wrong. There is an edge fault as well, but the centre is always dealt with first." },
 { 'A', "One bad corner", "\"Just the one corner.\"",
   T_STRIP, {0,0}, {0,0}, {0,0}, {0,0}, 63, 2, 0, P_GRID, 4, 0, {0}, 0,
   "Centre and three corners are white; one is not. That is what a strip is for. 1: N lays a strip on the glass; move it round (Left, Right) to the place nearest the bad corner. 2: V for the side view; Left slides it in under the yoke. 3: if it gets worse, flip it (F). 4: when the corner is white, tape it (T).", 0 },
 { 'D', "Mrs Lindqvist's console set", "\"It was dropped in the removal. Nothing is where it should be.\"",
   T_PU | T_P4 | T_P6 | T_TILT | T_STRIP | T_DEG | T_LOCK, {6,14}, {22,36}, {40,46}, {3,-2}, 27, -2, 1, P_RED, 14, 4, {DG_STAT, DG_MAG, DG_DYN, DG_PUR}, DG_MAG,
   "The whole alignment. Degauss. Purity. Centre with the rings. Edges with the yoke. The last corner with a strip. Lock.",
   "Start with what would spoil every later judgement: the magnetised mask." }
};

u8 cb_medal[NCJ];          /* bit 0 diagnosis, 1 economy, 2 result, 3 passed */

/* ---------------- the set on the bench ---------------- */
typedef struct { int ang, depth, pol, taped; } Strip;
typedef struct { int pux, puy, p4x, p4y, p6x, p6y, tx, ty, n, ang[SHEET + 1], amt[SHEET + 1]; } Res;
typedef struct { Res R; int centre, edge, pc, pe, rb, mg, convok, purok; } Meas;

static int job, phase, pat, guns, help, unflip, hand, view, ssel, dsel;
static int rings[3][2], sol[3][2], tilt[2], tsol[2], nstr, mag, locked, unlocks, moves, hints, diagok, lastact, medals;
static Strip strips[SHEET];
static char msg[700];
static const CJob *J;

#define FLIP (unflip ? -1 : 1)
static int tool(int t) { return (J->tools & t) != 0; }
static void say(const char *s) { strncpy(msg, s, sizeof msg - 1); msg[sizeof msg - 1] = 0; }
static int isqrt(u32 v) { u32 r = 0, b = 1UL << 30; while (b > v) b >>= 2; while (b) { if (v >= r + b) { v -= r + b; r = (r >> 1) + b; } else r >>= 1; b >>= 2; } return (int)r; }
static int hyp(int x, int y) { return isqrt((u32)(x * x) + (u32)(y * y)); }
static int rdiv(int a, int b) { return a >= 0 ? (a + b / 2) / b : -((-a + b / 2) / b); }
static int i48(int a) { return (a * 256 + 24) / 48; }
static int i72(int a) { return (a * 256 + 36) / 72; }
static void ringvec(const int *r, int *x, int *y)
{
    int a = i48(r[0]), b = i48(r[1]);
    *x = 7 * (COS(a) - COS(b)) / 2; *y = 7 * (SIN(a) - SIN(b)) / 2;
}
static void resid(Res *R)
{
    int i, ax, ay, bx, by, v[6];
    for (i = 0; i < 3; i++) { ringvec(rings[i], &ax, &ay); ringvec(sol[i], &bx, &by); v[i * 2] = ax - bx; v[i * 2 + 1] = ay - by; }
    R->pux = v[0]; R->puy = v[1]; R->p4x = v[2]; R->p4y = v[3]; R->p6x = v[4]; R->p6y = v[5];
    R->tx = tilt[0] - tsol[0]; R->ty = tilt[1] - tsol[1];
    R->n = 0;
    for (i = 0; i < nstr; i++) { R->ang[R->n] = strips[i].ang; R->amt[R->n++] = strips[i].pol * strips[i].depth; }
    if (J->samt) { R->ang[R->n] = J->sang; R->amt[R->n++] = -J->samt; }
}
/* landing error of gun c (0 red, 1 green, 2 blue) at u, v */
static void conv(const Res *R, int c, int u, int v, int *ox, int *oy)
{
    int s, x, y, i;
    if (c == 1) { *ox = *oy = 0; return; }
    s = c == 0 ? 1 : -1;
    x = s * R->p4x + R->p6x; y = s * R->p4y + R->p6y;
    x += s * R->tx * v * 114 / 100; y += s * R->ty * u * 114 / 100;      /* 0.9 px per step at the edge */
    for (i = 0; i < R->n; i++) {
        int a = i72(R->ang[i]), cs = COS(a), sn = SIN(a), p = (u * cs + v * sn) / 127;
        if (p > 0) {
            int k;
            if (p > 130) p = 130;
            k = s * R->amt[i] * (p * p * p / 1000) * 165 / 1000;          /* 1.3 px per depth step at full reach */
            x += -sn * k / 127; y += cs * k / 127;
        }
    }
    *ox = x; *oy = y;
}
/* purity error at u, v: 0..256 */
static int tint(const Res *R, int u, int v)
{
    int k = hyp(R->pux, R->puy) * 256 / (5 * UPP);
    if (mag) { int d = hyp(u - 40, v + 30); if (d < 75) k += 282 * (75 - d) / 75; }
    return k > 256 ? 256 : k;
}
static const signed char pts[9][2] = { {0,0}, {-90,0}, {90,0}, {0,-90}, {0,90}, {-90,-90}, {90,-90}, {-90,90}, {90,90} };
static void measure(Meas *M)
{
    int i, rx, ry, bx, by, d, t;
    resid(&M->R); M->centre = M->edge = M->pc = M->pe = 0;
    for (i = 0; i < 9; i++) {
        conv(&M->R, 0, pts[i][0], pts[i][1], &rx, &ry); conv(&M->R, 2, pts[i][0], pts[i][1], &bx, &by);
        d = hyp(rx, ry); t = hyp(bx, by); if (t > d) d = t; t = hyp(rx - bx, ry - by); if (t > d) d = t;
        if (i == 0) M->centre = d; else if (d > M->edge) M->edge = d;
        t = tint(&M->R, pts[i][0], pts[i][1]);
        if (i == 0) M->pc = t; else if (t > M->pe) M->pe = t;
    }
    conv(&M->R, 0, 0, 0, &rx, &ry); conv(&M->R, 2, 0, 0, &bx, &by);
    M->rb = hyp(rx - bx, ry - by); M->mg = hyp((rx + bx) / 2, (ry + by) / 2);
    M->convok = M->centre <= TOL && M->edge <= TOL;
    M->purok = M->pc <= 55 && M->pe <= 68 && !mag;
}
static int needs_purity(void) { return tool(T_PU | T_DEG); }
static int hand_ok(int h) { return h == H_YOKE ? tool(T_TILT) : h == H_STRIP ? tool(T_STRIP) : tool(1 << h); }

static void new_set(int i)
{
    static const int order[NHAND] = { H_P4, H_P6, H_PU, H_YOKE, H_STRIP };
    int k;
    job = i; J = &cj[i];
    memset(rings, 0, sizeof rings); memset(tilt, 0, sizeof tilt); nstr = 0; ssel = -1;
    sol[0][0] = J->pu[0]; sol[0][1] = J->pu[1]; sol[1][0] = J->p4[0]; sol[1][1] = J->p4[1]; sol[2][0] = J->p6[0]; sol[2][1] = J->p6[1];
    tsol[0] = J->tilt[0]; tsol[1] = J->tilt[1];
    mag = J->mag; locked = unlocks = moves = hints = 0; diagok = -1; lastact = -1; medals = 0;
    pat = J->pat; guns = 7; view = 0; dsel = 0;
    hand = H_P4; for (k = 0; k < NHAND; k++) if (hand_ok(order[k])) { hand = order[k]; break; }
    if (J->ndiag) { phase = PH_DIAG; sprintf(msg, "Job %d of %d: %s. %s  Diagnose first. Look at the picture. Change the pattern (M) and switch guns (7 8 9) if it helps. Up, Down and ENTER choose.", i + 1, NCJ, J->name, J->cust); }
    else { phase = PH_WORK; sprintf(msg, "Job %d of %d: %s. %s %s", i + 1, NCJ, J->name, J->cust, J->intro); }
}

/* ---------------- drawing ---------------- */
#define MX 14
#define MY 40
#define MW 372
#define MH 279
#define NX 518
#define NY 176
#define YR 86
#define GR 104
static const int RR[3] = { 40, 54, 68 };
static const u8 RC[3][3] = { {255,216,60}, {255,88,255}, {60,230,255} };
static const char *RN[NHAND] = { "PURITY RINGS", "4-POLE RINGS", "6-POLE RINGS", "YOKE TILT", "STRIP" };

static void t8c(int xc, int y, const char *s, int c) { text8(xc - (int)strlen(s) * 4, y, s, c); }
static void fmt1(char *t, int u) { int v = rdiv(u * 10, UPP); sprintf(t, "%d.%d", v / 10, v % 10); }
static int wrap8(int x, int y, int cols, int maxl, const char *s, int c)
{
    char ln[90]; int n, l = 0;
    while (*s && l < maxl) {
        const char *e; while (*s == ' ') s++;
        n = (int)strlen(s);
        if (n > cols) { n = cols; while (n > 0 && s[n] != ' ') n--; if (!n) n = cols; }
        e = s + n; memcpy(ln, s, n); ln[n] = 0; text8(x, y + l * 9, ln, c); l++; s = e;
    }
    return l;
}
static void poly(const int *p, int n, int c)
{
    int y, y0 = p[1], y1 = p[1], i, j, k, m, xs[8], t;
    for (i = 1; i < n; i++) { if (p[i * 2 + 1] < y0) y0 = p[i * 2 + 1]; if (p[i * 2 + 1] > y1) y1 = p[i * 2 + 1]; }
    for (y = y0; y <= y1; y++) {
        m = 0;
        for (i = 0; i < n; i++) {
            int ax = p[i * 2], ay = p[i * 2 + 1], bx, by; j = (i + 1) % n; bx = p[j * 2]; by = p[j * 2 + 1];
            if ((ay <= y && by > y) || (by <= y && ay > y)) xs[m++] = ax + (y - ay) * (bx - ax) / (by - ay);
        }
        for (i = 1; i < m; i++) for (k = i; k > 0 && xs[k - 1] > xs[k]; k--) { t = xs[k]; xs[k] = xs[k - 1]; xs[k - 1] = t; }
        for (k = 0; k + 1 < m; k += 2) hline(xs[k], xs[k + 1], y, c);
    }
}
static void outline(const int *p, int n, int c) { int i, j; for (i = 0; i < n; i++) { j = (i + 1) % n; line(p[i * 2], p[i * 2 + 1], p[j * 2], p[j * 2 + 1], c); } }
static int sxu(int u) { return MX + MW / 2 + (unflip ? u : -u) * 164 / 100; }
static int syv(int v) { return MY + MH / 2 + v * 123 / 100; }

static void draw_mirror(const Meas *M)
{
    static const u8 col[3][3] = { {255,0,0}, {0,255,0}, {0,0,255} };
    const Res *R = &M->R; int dir = unflip ? 1 : -1, c, k, i, j, ox, oy;
    fill(MX - 8, MY - 8, MW + 16, MH + 16, rgbc(136, 146, 168)); fill(MX - 5, MY - 5, MW + 10, MH + 10, rgbc(42, 48, 70));
    fill(MX, MY, MW, MH, rgbc(90, 60, 32)); fill(MX + 12, MY + 10, MW - 24, MH - 20, 0);
    clip(MX + 12, MY + 10, MX + MW - 13, MY + MH - 11);
    if (pat == P_RED) {
        if (guns & 1) for (j = 0; j < 18; j++) for (i = 0; i < 24; i++) {
            int u = (((2 * i + 1) * 100 / 24) - 100) * 11 / 10 * dir, v = (((2 * j + 1) * 100 / 18) - 100) * 11 / 10;
            int kk = tint(R, u, v), X = R->pux * 100 / UPP + u, Y = R->puy * 100 / UPP + v, h = hyp(X, Y), s = h ? Y * 256 / h : 0;
            int r = 255 - 255 * kk * 45 / 25600, g = 255 * kk / 256 * (141 + 90 * s / 256) / 256, b = 255 * kk / 256 * (115 - 90 * s / 256) / 256;
            int x0 = MX + 12 + i * (MW - 24) / 24, x1 = MX + 12 + (i + 1) * (MW - 24) / 24, y0 = MY + 10 + j * (MH - 20) / 18, y1 = MY + 10 + (j + 1) * (MH - 20) / 18, x, y;
            if (!kk) fill(x0, y0, x1 - x0, y1 - y0, rgbc(255, 0, 0));
            else for (y = y0; y < y1; y++) for (x = x0; x < x1; x++) pset(x, y, dith(x, y, r, g, b));
        }
    } else for (c = 0; c < 3; c++) {
        int px, py, qx, qy;
        if (!(guns & (1 << c))) continue;
#define PT(u, v, X, Y) { conv(R, c, (u), (v), &ox, &oy); X = sxu(u) + rdiv(ox * dir, UPP); Y = syv(v) + rdiv(oy, UPP); }
        if (pat == P_GRID) {
            for (k = -4; k <= 4; k++) for (i = 0; i < 24; i++) {
                PT(k * 25, i * 200 / 24 - 100, px, py); PT(k * 25, (i + 1) * 200 / 24 - 100, qx, qy);
                addline(px, py, qx, qy, col[c][0], col[c][1], col[c][2]); addline(px + 1, py, qx + 1, qy, col[c][0], col[c][1], col[c][2]);
            }
            for (k = -3; k <= 3; k++) for (i = 0; i < 24; i++) {
                PT(i * 200 / 24 - 100, k * 100 / 3, px, py); PT((i + 1) * 200 / 24 - 100, k * 100 / 3, qx, qy);
                addline(px, py, qx, qy, col[c][0], col[c][1], col[c][2]); addline(px, py + 1, qx, qy + 1, col[c][0], col[c][1], col[c][2]);
            }
        } else for (k = -4; k <= 4; k++) for (j = -3; j <= 3; j++) {
            int a, b; PT(k * 25, j * 100 / 3, px, py);
            for (a = -2; a <= 2; a++) for (b = -2; b <= 2; b++) addrgb(px + a, py + b, col[c][0], col[c][1], col[c][2]);
        }
#undef PT
    }
    unclip();
    text(MX, 8, unflip ? "MIRROR (un-reversed)" : "MIRROR", C_WHITE);
    { const char *pn = pat == P_RED ? "RED FIELD" : pat == P_GRID ? "CROSSHATCH" : "DOTS"; text(MX + MW - (int)strlen(pn) * 8, 8, pn, C_AMBER); }
}
static void clock_name(const Strip *q, char *t)
{
    int a5 = unflip ? q->ang : 36 - q->ang, h = ((a5 + 18 + 3 + 720) / 6) % 12;
    sprintf(t, "%d o'clock", h ? h : 12);
}
static int strip_col(const Strip *q) { return q->pol > 0 ? rgbc(255, 138, 60) : rgbc(76, 156, 255); }
static void draw_rear(const Meas *M)
{
    int i, n, h, glass = rgbc(11, 26, 34), dark = rgbc(16, 22, 31), off = rgbc(42, 51, 72); char t[60];
    t8c(NX, 12, "REAR OF THE SET", C_WHITE);
    disc(NX, NY, GR + 13, rgbc(92, 142, 160)); disc(NX, NY, GR + 11, glass);
    for (h = 0; h < 12; h++) { int a = h * 256 / 12; fill(NX + COS(a) * (GR + 9) / 127 - 1, NY + SIN(a) * (GR + 9) / 127 - 1, 3, 3, rgbc(53, 80, 106)); }
    if (tool(T_STRIP) && help == 0) {   /* apprentice: a mark at the part of the edge that is worst */
        int w = 0, wu = 0, wv = 0, rx, ry, bx, by, d;
        for (i = 1; i < 9; i++) { conv(&M->R, 0, pts[i][0], pts[i][1], &rx, &ry); conv(&M->R, 2, pts[i][0], pts[i][1], &bx, &by); d = hyp(rx - bx, ry - by); if (d > w) { w = d; wu = pts[i][0]; wv = pts[i][1]; } }
        if (w > TOL) { d = hyp(wu, wv); disc(NX - wu * (GR + 22) / d * FLIP, NY + wv * (GR + 22) / d, 3, C_WHITE); }
    }
    for (i = 0; i < nstr; i++) {
        const Strip *q = &strips[i]; int a = i72(q->ang), rad = GR + 8 - q->depth * 4, p[8], k;
        int cx = NX * 127 - COS(a) * rad * FLIP, cy = NY * 127 + SIN(a) * rad;      /* in 1/127 px */
        int tx = SIN(a) * FLIP, ty = COS(a), rx = -COS(a) * FLIP, ry = SIN(a);
        for (k = 0; k < 4; k++) { int st = (k == 0 || k == 3) ? -12 : 12, sr = k < 2 ? -3 : 3; p[k * 2] = rdiv(cx + st * tx + sr * rx, 127); p[k * 2 + 1] = rdiv(cy + st * ty + sr * ry, 127); }
        poly(p, 4, strip_col(q)); outline(p, 4, i == ssel ? C_WHITE : C_BLACK);
        if (q->taped) disc(rdiv(cx, 127), rdiv(cy, 127), 3, rgbc(244, 240, 216));
    }
    /* yoke */
    { int ox = tool(T_TILT) ? tilt[1] * 12 / 10 : 0, oy = tool(T_TILT) ? tilt[0] * 12 / 10 : 0;
      disc(NX + ox, NY + oy, YR + 6, rgbc(200, 120, 50)); disc(NX + ox, NY + oy, YR - 6, glass);
      for (i = 0; i < 24; i++) { int a = i * 256 / 24; line(NX + ox + COS(a) * (YR - 6) / 127, NY + oy + SIN(a) * (YR - 6) / 127, NX + ox + COS(a) * (YR + 6) / 127, NY + oy + SIN(a) * (YR + 6) / 127, rgbc(122, 68, 22)); }
      if (locked) for (i = 0; i < 4; i++) { int a = 24 + i * 64; disc(NX + COS(a) * (YR + 9) / 127, NY + SIN(a) * (YR + 9) / 127, 4, rgbc(232, 232, 240)); } }
    /* neck and the three ring pairs */
    disc(NX, NY, RR[2] + 9, dark);
    for (i = 2; i >= 0; i--) {
        int on = tool(1 << i), s = i == hand, c = !on ? off : s ? rgbc(RC[i][0], RC[i][1], RC[i][2]) : rgbc(RC[i][0] / 2, RC[i][1] / 2, RC[i][2] / 2);
        disc(NX, NY, RR[i] + (s && on ? 4 : 3), c); disc(NX, NY, RR[i] - (s && on ? 5 : 4), dark);
    }
    for (i = 0; i < 3; i++) {
        int on = tool(1 << i);
        for (n = 0; n < 2; n++) {       /* the second tab is drawn smaller, so two tabs lying together can both be seen */
            int a = i48(rings[i][n]), x = NX + COS(a) * RR[i] / 127, y = NY + SIN(a) * RR[i] / 127, w = n ? 4 : 6;
            fill(x - w, y - w, w * 2, w * 2, !on ? rgbc(58, 70, 100) : n ? C_WHITE : rgbc(RC[i][0], RC[i][1], RC[i][2]));
            box(x - w - 1, y - w - 1, w * 2 + 2, w * 2 + 2, locked ? C_WHITE : C_BLACK);
        }
        if (on && help == 0) {          /* apprentice: which way, and how far, this pair is out */
            int kx = i == 0 ? M->R.pux : i == 1 ? M->R.p4x : M->R.p6x, ky = i == 0 ? M->R.puy : i == 1 ? M->R.p4y : M->R.p6y, m = hyp(kx, ky), len;
            if (m > 38 && i == hand) { len = m * 5 / UPP; if (len > RR[0] - 6) len = RR[0] - 6; line(NX, NY, NX - FLIP * kx * len / m, NY + ky * len / m, C_WHITE); }
        }
    }
    for (i = 0; i < 3; i++) disc(NX - 12 + i * 12, NY, 4, (guns & (1 << i)) ? (i == 0 ? rgbc(255, 64, 64) : i == 1 ? rgbc(64, 255, 96) : rgbc(80, 128, 255)) : rgbc(34, 34, 34));
    if (tool(T_STRIP)) { sprintf(t, "STRIPS LEFT ON THE SHEET: %d", SHEET - nstr); t8c(NX, NY + GR + 30, t, GRAY(13)); }
}
static void side_strip(const Strip *q, int ol)
{
    int cy = NY, sn = SIN(i72(q->ang)), sg = sn < -38 ? -1 : sn > 38 ? 1 : 0, in = q->depth * 7, p[8], k;
    if (sg) {
        int ox = (552 * 10 - in * 8) * 16, oy = (cy * 10 + sg * (530 - in * 6)) * 16;    /* in 1/160 px; along the glass 128,95 over 160 */
        for (k = 0; k < 4; k++) { int a = (k == 0 || k == 3) ? -4 : 36, b = k < 2 ? 1 : 6; p[k * 2] = rdiv(ox + a * 128 - b * 95, 160); p[k * 2 + 1] = rdiv(oy + sg * (a * 95 + b * 128), 160); }
    } else { int x = 562 - in; p[0] = x; p[1] = cy - 7; p[2] = x + 40; p[3] = cy - 7; p[4] = x + 40; p[5] = cy + 7; p[6] = x; p[7] = cy + 7; }
    if (ol) { outline(p, 4, C_WHITE); return; }
    poly(p, 4, strip_col(q)); outline(p, 4, C_BLACK);
    if (q->taped) disc((p[0] + p[4]) / 2, (p[1] + p[5]) / 2, 4, rgbc(244, 240, 216));
}
static void draw_side(void)
{
    int cy = NY, x0 = 412, i, sg, k, glass = rgbc(11, 26, 34), edge = rgbc(92, 142, 160); char t[60], u[24];
    t8c(NX, 12, "SIDE OF THE NECK", C_WHITE);
    { int f[8]; f[0] = 506; f[1] = cy - 17; f[2] = 634; f[3] = cy - 112; f[4] = 634; f[5] = cy + 112; f[6] = 506; f[7] = cy + 17;
      fill(x0, cy - 17, 96, 35, glass); poly(f, 4, glass); outline(f, 4, edge); line(506, cy - 16, 506, cy + 16, glass);
      hline(x0, 506, cy - 17, edge); hline(x0, 506, cy + 17, edge); }
    for (i = -2; i <= 2; i++) fill(x0 - 8, cy + i * 6 - 1, 8, 2, rgbc(138, 148, 168));
    fill(x0, cy - 19, 10, 38, rgbc(42, 51, 72));
    for (i = 0; i < 3; i++) { int x = x0 + 22 + i * 18, on = tool(1 << i);
        fill(x, cy - 25, 9, 50, on ? rgbc(RC[i][0], RC[i][1], RC[i][2]) : rgbc(42, 51, 72));
        fill(x + 1, cy - 31, 7, 6, on ? C_WHITE : rgbc(58, 70, 100)); fill(x + 1, cy + 25, 7, 6, on ? C_WHITE : rgbc(58, 70, 100)); }
    for (i = 0; i < nstr; i++) side_strip(&strips[i], 0);
    for (sg = -1; sg <= 1; sg += 2) {       /* the yoke: windings wrapped round the start of the funnel */
        int y[10]; y[0] = 480; y[1] = cy + sg * 19; y[2] = 508; y[3] = cy + sg * 19; y[4] = 548; y[5] = cy + sg * 49; y[6] = 548; y[7] = cy + sg * 72; y[8] = 480; y[9] = cy + sg * 46;
        poly(y, 5, rgbc(200, 120, 50));
        for (k = 0; k < 7; k++) line(484 + k * 9, cy + sg * (21 + k * 2), 484 + k * 9, cy + sg * (46 + k * 36 / 10), rgbc(122, 68, 22));
        if (locked) { int w[6]; w[0] = 548; w[1] = cy + sg * 50; w[2] = 562; w[3] = cy + sg * 60; w[4] = 548; w[5] = cy + sg * 66; poly(w, 3, rgbc(232, 232, 240)); }
    }
    if (ssel >= 0) {
        const Strip *q = &strips[ssel];
        side_strip(q, 1);
        clock_name(q, u); sprintf(t, "STRIP %d: %s, IN %d OF 5", ssel + 1, u, q->depth); t8c(NX, NY + 118, t, C_WHITE);
        sprintf(t, "%c FACE TO THE GLASS, %s", q->pol > 0 ? '+' : '-', q->taped ? "TAPED" : "LOOSE"); t8c(NX, NY + 130, t, q->taped ? C_LEDG : C_AMBER);
        t8c(NX, 46, "LEFT = FURTHER IN, RIGHT = OUT", GRAY(13));
    } else t8c(NX, NY + 124, tool(T_STRIP) ? "NO STRIP CHOSEN: N LAYS ONE" : "NO STRIPS ON THIS JOB", GRAY(13));
}
static void draw_controls(void)
{
    int px = 436, py = 340, on = tool(T_TILT), c = on ? rgbc(255, 176, 96) : rgbc(58, 70, 100); char t[40];
    text8(px, py - 12, "TILT", hand == H_YOKE ? C_WHITE : c);
    box(px, py, 57, 57, c); if (hand == H_YOKE) box(px - 1, py - 1, 59, 59, C_WHITE);
    vline(px + 28, py, py + 56, c); hline(px, px + 56, py + 28, c);
    disc(px + 28 + tilt[1] * 4, py + 28 + tilt[0] * 4, 4, c);
    text8(508, py - 12, "HAND (TAB):", GRAY(13)); text8(508, py, RN[hand], C_AMBER);
    sprintf(t, "VIEW (V): %s", view ? "SIDE" : "REAR"); text8(508, py + 14, t, C_AMBER);
    sprintf(t, "HELP: %s", help == 0 ? "APPRENTICE" : help == 1 ? "TECHNICIAN" : "MASTER"); text8(508, py + 28, t, GRAY(13));
    if (locked) text8(508, py + 42, "LOCKED", C_LEDR);
}
static void draw_sheet(const Meas *M)
{
    const char *nm[5]; int ok[5], n = 0, i, y = 332; char t[100], a[12], b[12], c[12];
    if (tool(T_DEG)) { nm[n] = "DEGAUSS"; ok[n++] = !mag; }
    if (tool(T_PU)) { nm[n] = "PURITY"; ok[n++] = M->purok; }
    if (tool(T_P4 | T_P6)) { nm[n] = "CENTRE"; ok[n++] = M->centre <= TOL; }
    if (tool(T_TILT | T_STRIP)) { nm[n] = "EDGES"; ok[n++] = M->edge <= TOL; }
    if (tool(T_LOCK)) { nm[n] = "LOCK"; ok[n++] = locked; }
    sprintf(t, "JOB %d/%d  %s", job + 1, NCJ, J->name); text8(MX, y, t, C_AMBER);
    for (i = 0; i < n; i++) { fill(MX + i * 78, y + 14, 9, 9, ok[i] ? C_LEDG : rgbc(58, 70, 100)); text8(MX + i * 78 + 13, y + 15, nm[i], ok[i] ? C_LEDG : GRAY(13)); }
    if (help < 2) { fmt1(a, M->rb); fmt1(b, M->mg); fmt1(c, M->edge); sprintf(t, "RED-BLUE %s  MAG-GREEN %s  EDGES %s  LIMIT 1.25", a, b, c); text8(MX, y + 30, t, C_WHITE); }
    sprintf(t, "MOVES %d  PAR %d", moves, J->par); text8(MX, y + 44, t, moves <= J->par ? C_LEDG : C_AMBER);
    if (phase == PH_DONE) { sprintf(t, "DIAGNOSIS %c  ECONOMY %c  RESULT %c", medals & 1 ? '*' : '-', medals & 2 ? '*' : '-', medals & 4 ? '*' : '-'); text8(MX + 136, y + 44, t, C_AMBER); }
}
static void draw_diag(void)
{
    int i, y = 96;
    fill(400, 30, 236, 364, C_INK); box(400, 30, 236, 364, C_AMBER);
    t8c(NX, 42, "BEFORE YOU TOUCH ANYTHING:", C_AMBER); t8c(NX, 56, "WHAT IS WRONG?", C_AMBER);
    for (i = 0; i < J->ndiag; i++) {
        if (i == dsel) { fill(404, y - 4, 228, 44, rgbc(20, 40, 110)); box(404, y - 4, 228, 44, rgbc(90, 130, 255)); }
        wrap8(410, y, 27, 4, dtext[J->diag[i]], i == dsel ? C_WHITE : GRAY(15)); y += 52;
    }
    t8c(NX, 370, "\x18\x19 choose   ENTER answer", C_SEL);
}
static void draw_all(void)
{
    Meas M; measure(&M);
    unclip(); fill(0, 0, SW, SH, rgbc(7, 11, 20));
    draw_mirror(&M);
    if (phase == PH_DIAG) draw_diag(); else { if (view) draw_side(); else draw_rear(&M); draw_controls(); }
    draw_sheet(&M);
    fill(4, 396, 632, 66, C_INK); box(4, 396, 632, 66, phase == PH_DONE ? C_LEDG : rgbc(31, 43, 72));
    wrap8(8, 399, 78, 7, msg, C_WHITE);
    text8(8, 463, "TAB hand  \x1B\x1A\x18\x19 adjust  V view  N new strip  S next  F flip  T tape  X peel", GRAY(13));
    text8(8, 472, phase == PH_DONE ? "ENTER next job   ESC back to the shelf" : "M pattern  7 8 9 guns  D degauss  K lock  H hint  U mirror  ENTER ship  ESC", phase == PH_DONE ? C_SEL : GRAY(13));
    vid_vsync(); vid_present();
}

/* ---------------- hands ---------------- */
static void act(int id) { if (id != lastact) { moves++; lastact = id; } }
static void once(void) { moves++; lastact = -1; }
static int blocked(void)
{
    if (!locked) return 0;
    say("The yoke is wedged and the rings are sealed with paint. Press K again to break the seal. It costs the result medal.");
    snd_sfx(SFX_BUZZ); return 1;
}
static Strip *pick(void)
{
    if (!tool(T_STRIP)) { say("No strips on this job."); return 0; }
    if (ssel < 0) { say("No strip is chosen. N lays a new strip on the glass; S chooses the next one already there."); return 0; }
    return &strips[ssel];
}
static void arrows(int k)
{
    int d = (k == K_LEFT || k == K_UP) ? -1 : 1, horiz = k == K_LEFT || k == K_RIGHT;
    if (hand <= H_P6) {
        int *r = rings[hand];
        if (!tool(1 << hand) || blocked()) return;
        if (horiz) { r[0] = (r[0] + 48 + d) % 48; r[1] = (r[1] + 48 + d) % 48; act(hand * 2); }
        else { r[0] = (r[0] + 48 + d) % 48; r[1] = (r[1] + 48 - d) % 48; act(hand * 2 + 1); }
        snd_sfx(SFX_TICK);
    } else if (hand == H_YOKE) {
        int *t = &tilt[horiz ? 1 : 0], v = *t + d;
        if (blocked()) return;
        if (v < -6 || v > 6) return;
        *t = v; act(6); snd_sfx(SFX_TICK);
    } else {
        Strip *q = pick(); char u[24];
        if (!q || blocked()) return;
        if (q->taped) { say("That strip is taped down. Take the tape off (T) before you move it."); return; }
        if (!view) {
            if (!horiz) { say("From behind you can only move a strip round the tube. Press V for the side view to slide it in or out."); return; }
            q->ang = (q->ang + 72 - d * FLIP) % 72; act(8 + ssel * 2); snd_sfx(SFX_TICK);
            clock_name(q, u); sprintf(msg, "Strip %d moved round the glass to %s.", ssel + 1, u);
        } else {
            int v = q->depth - d;
            if (!horiz) { say("Left pushes the strip further in under the yoke. Right draws it out. Press V to move it round the tube."); return; }
            if (v < 1 || v > 5) { say(v > 5 ? "It will not go in any further." : "Any further out and it falls off. X peels it off the glass."); return; }
            q->depth = v; act(9 + ssel * 2); snd_sfx(SFX_TICK);
            sprintf(msg, "Strip %d is in %d of 5 under the yoke. Further in is stronger.", ssel + 1, v);
        }
    }
}
static void lay_strip(void)
{
    char u[24];
    if (!tool(T_STRIP)) { say("No strips on this job."); return; }
    if (blocked()) return;
    if (nstr >= SHEET) { say("The sheet is empty. Peel a strip off the glass (X) to use it somewhere else."); return; }
    strips[nstr].ang = 54; strips[nstr].depth = 1; strips[nstr].pol = 1; strips[nstr].taped = 0; ssel = nstr++; hand = H_STRIP; once(); snd_sfx(SFX_CLICK);
    clock_name(&strips[ssel], u);
    sprintf(msg, "Strip %d laid on the glass at %s, just under the edge of the yoke. Move it round with Left and Right to the place nearest the fault, then press V for the side view and slide it in.", ssel + 1, u);
}
static void flip_strip(void)
{
    Strip *q = pick();
    if (!q || blocked()) return;
    if (q->taped) { say("Taped. T takes the tape off."); return; }
    q->pol = -q->pol; once(); snd_sfx(SFX_CLICK);
    sprintf(msg, "Strip %d turned over: the %c face is now against the glass. It pushes the colours the other way.", ssel + 1, q->pol > 0 ? '+' : '-');
}
static void tape_strip(void)
{
    Strip *q = pick();
    if (!q || blocked()) return;
    q->taped = !q->taped; once(); snd_sfx(SFX_THUNK);
    sprintf(msg, q->taped ? "Strip %d taped to the glass. It will stay where it is." : "Tape off strip %d. It can be moved again.", ssel + 1);
}
static void peel_strip(void)
{
    Strip *q = pick(); int i;
    if (!q || blocked()) return;
    if (q->taped) { say("Taped. T takes the tape off first."); return; }
    for (i = ssel; i + 1 < nstr; i++) strips[i] = strips[i + 1];
    nstr--; ssel = -1; once(); snd_sfx(SFX_CLICK); say("Strip peeled off and put back on the sheet.");
}
static void degauss(void)
{
    int f;
    snd_sfx(SFX_DEGAUSS); if (tool(T_DEG)) once();
    if (!auto_mode) for (f = 0; f < 10; f++) { pal_apply(40 + (f & 1) * 24, 7); vid_vsync(); vid_vsync(); vid_vsync(); sys_idle(); }
    pal_apply(64, 7);
    if (mag) { mag = 0; say("The patch is gone: the shadow mask was magnetised. Degauss comes before every other adjustment."); }
    else say("Degaussed. Nothing changed, so the mask was not magnetised.");
}
static void lock(void)
{
    Meas M;
    if (!tool(T_LOCK)) { say("Nothing to lock on this job."); return; }
    if (!locked) {
        locked = 1; snd_sfx(SFX_THUNK); measure(&M);
        say(M.convok && (M.purok || !needs_purity()) ? "Yoke wedged, rings sealed. Ship it (ENTER)." : "Locked, but the picture is not right. You have sealed the error in. Press K again to break the seal.");
    } else { locked = 0; unlocks++; snd_sfx(SFX_BUZZ); say("Seal broken. On a real set that is paint to scrape off."); }
    lastact = -1;
}
static void hint(void)
{
    Meas M; const char *t; int i, loose = 0;
    hints++; measure(&M);
    for (i = 0; i < nstr; i++) if (!strips[i].taped) loose = 1;
    if (mag) t = "A patch on the red field means a magnetised mask. Degauss (D).";
    else if (needs_purity() && !M.purok) t = "Red field, tinted: purity rings. Spread the yellow tabs, then turn the pair until the tint leaves.";
    else if (M.rb > TOL) t = "Red and blue are apart at the centre: 4-pole rings. Green off (8) makes it easy to see.";
    else if (M.mg > TOL) t = "Red and blue are together but beside green: 6-pole rings.";
    else if (M.R.tx || M.R.ty) t = "Centre is right, two opposite edges are split: tilt the yoke. Up and down splits at top and bottom are one axis, left and right the other.";
    else if (M.edge > TOL) t = "One part of the edge is still split, and the yoke cannot help: a strip. Rear view: lay it (N) and move it to the glass nearest the bad part. Side view (V): slide it in under the yoke until the fringe closes. Flip it (F) if it gets worse.";
    else if (loose) t = "The picture is right, but a strip is loose. Tape it (T) or it will move.";
    else if (tool(T_LOCK) && !locked) t = "Everything is right. Lock it (K).";
    else t = "It is right. Ship it (ENTER).";
    sprintf(msg, "Hint: %s (Hints cost the economy medal.)", t); snd_sfx(SFX_OK);
}
static void ship(void)
{
    Meas M; int ok, i, loose = 0;
    measure(&M);
    ok = M.convok && (!needs_purity() || M.purok) && (!tool(T_LOCK) || locked) && guns == 7;
    for (i = 0; i < nstr; i++) if (!strips[i].taped) loose = 1;
    if (ok && loose) { snd_sfx(SFX_BUZZ); say("Returned by the customer: \"The fringe came back when I carried it in.\" A strip was left loose. Tape every strip (T) before the set leaves."); return; }
    if (!ok) {
        snd_sfx(SFX_BUZZ);
        sprintf(msg, "Returned by the customer. %s", guns != 7 ? "A gun is still switched off." : mag ? "\"There is still a cloud of colour.\"" : needs_purity() && !M.purok ? "\"The red is still not red.\"" :
            M.centre > TOL ? "\"Still coloured edges in the middle.\"" : M.edge > TOL ? "\"Still fringes at the edges.\"" : "It is right, but not locked: it will drift.");
        return;
    }
    medals = ((J->ndiag ? diagok == 1 : 1) ? 1 : 0) | ((moves <= J->par && hints == 0) ? 2 : 0) | (unlocks == 0 ? 4 : 0);
    cb_medal[job] |= (u8)(medals | 8);
    phase = PH_DONE; snd_jingle(JING_WIN);
    sprintf(msg, "PASSED. Medals: diagnosis %s, economy %s (%d moves, par %d, %d hints), result %s. %s",
        medals & 1 ? "yes" : "no", medals & 2 ? "yes" : "no", moves, J->par, hints, medals & 4 ? "yes" : "no (seal broken)",
        job + 1 < NCJ ? "ENTER takes the next set off the shelf." : "That was the last job on the shelf.");
}
static void next_hand(void) { int n; for (n = 0; n < NHAND; n++) { hand = (hand + 1) % NHAND; if (hand_ok(hand)) break; } snd_sfx(SFX_CLICK); }
static void common_key(int k)
{
    if (k == 'm' || k == 'M') { pat = (pat + 1) % 3; snd_sfx(SFX_CLICK);
        if (phase != PH_DIAG) say(pat == P_RED ? "Red field: purity. It must be the same pure red everywhere." : pat == P_GRID ? "Crosshatch: convergence. Every line must be white." : "Dots: convergence. Every dot must be one white dot."); }
    else if (k >= '7' && k <= '9') { guns ^= 1 << (k - '7'); snd_sfx(SFX_CLICK);
        if (phase != PH_DIAG) say(guns == 7 ? "All three guns on." : "A gun is off. With green off, red and blue make magenta where they meet."); }
    else if (k == 'u' || k == 'U') { unflip ^= 1; say(unflip ? "Mirror un-reversed: the picture and the rear view no longer match left for left." : "Mirror as on the bench: reversed, so left in the mirror is left at the rear."); }
    else if (k == K_F12) { vid_shot("CBENCH.BMP"); }
}
/* Work one job. Returns 1 to go on to the next job, 0 to go back to the shelf. */
static int run_job(int i)
{
    int k;
    new_set(i); snd_song(SONG_BENCH); pal_apply(64, 7); vid_invalidate();
    for (;;) {
        draw_all();
        do { k = key_get(); if (!k) vid_vsync(); } while (!k);
        if (k == K_ESC) return 0;
        if (phase == PH_DONE) { if (k == K_ENTER || k == ' ') return job + 1 < NCJ; common_key(k); continue; }
        if (phase == PH_DIAG) {
            if (k == K_UP) { dsel = (dsel + J->ndiag - 1) % J->ndiag; snd_sfx(SFX_TICK); }
            else if (k == K_DOWN) { dsel = (dsel + 1) % J->ndiag; snd_sfx(SFX_TICK); }
            else if (k == K_ENTER) {
                if (J->diag[dsel] == J->ans) { if (diagok < 0) diagok = 1; phase = PH_WORK; snd_sfx(SFX_OK); sprintf(msg, "%s %s %s", diagok == 1 ? "Right." : "That is it.", J->why, J->intro); }
                else { diagok = 0; snd_sfx(SFX_BUZZ); say("Not that. Look again: is the centre right? Is it a patch or an even tint? Are the colours apart, or in register but the wrong colour?"); }
            } else common_key(k);
            continue;
        }
        if (k == K_LEFT || k == K_RIGHT || k == K_UP || k == K_DOWN) arrows(k);
        else if (k == K_TAB) { next_hand(); sprintf(msg, "Hand on: %s.%s", RN[hand], hand == H_STRIP ? " N lays a new strip; S chooses the next one on the glass." : hand == H_YOKE ? " The arrow keys tilt the yoke." : " Left and Right turn the pair; Up and Down spread or close its two tabs."); }
        else if (k == 'v' || k == 'V') { view ^= 1; snd_sfx(SFX_CLICK);
            say(view ? "Side of the neck. The yoke sits on the start of the funnel; a strip goes into the gap between its front edge and the glass. Left pushes the chosen strip further in, Right draws it out." : "Rear of the set. Ring tabs, and the glass round the yoke where a strip is laid."); }
        else if (k == 'n' || k == 'N') lay_strip();
        else if (k == 's' || k == 'S') { if (nstr) { ssel = (ssel + 1) % nstr; hand = H_STRIP; sprintf(msg, "Strip %d chosen.", ssel + 1); } else say("There is no strip on the glass. N lays one."); }
        else if (k == 'f' || k == 'F') flip_strip();
        else if (k == 't' || k == 'T') tape_strip();
        else if (k == 'x' || k == 'X') peel_strip();
        else if (k == 'd' || k == 'D') degauss();
        else if (k == 'k' || k == 'K') lock();
        else if (k == 'h' || k == 'H') hint();
        else if (k == K_ENTER) ship();
        else if (k == '`' && auto_mode) {       /* test hook: the cure */
            memcpy(rings, sol, sizeof rings); tilt[0] = tsol[0]; tilt[1] = tsol[1]; mag = 0; nstr = 0; ssel = -1;
            if (J->samt) { strips[0].ang = J->sang; strips[0].depth = abs(J->samt); strips[0].pol = J->samt > 0 ? 1 : -1; strips[0].taped = 1; nstr = 1; ssel = 0; }
            if (tool(T_LOCK)) locked = 1;
        }
        else common_key(k);
    }
}
/* The shelf: choose a job. */
void cbench_menu(int help_level)
{
    int sel = 0, k, i; char t[90];
    help = help_level;
    for (i = 0; i < NCJ; i++) if (cb_medal[i] & 8) sel = i + 1 < NCJ ? i + 1 : i;
    for (;;) {
        snd_song(SONG_TITLE); pal_apply(64, 7);
        unclip(); dfill(0, 0, SW, SH, 30, 40, 62, 8, 11, 22);
        fill(50, 16, 540, 448, C_PANEL); bevel(50, 16, 540, 448, C_PANELH, C_PANELL);
        textbig(124, 26, "THE CONVERGENCE BENCH", C_AMBER, 2);
        text8(70, 64, "Sets on the shelf. Each one teaches one idea, then tests it.", C_WHITE);
        text8(70, 76, "Medals: D diagnosis right first time. E within par, no hints.", GRAY(15));
        for (i = 0; i < NCJ; i++) {
            int y = 108 + i * 26, m = cb_medal[i];
            if (i == sel) { fill(66, y - 5, 508, 25, rgbc(20, 40, 110)); bevel(66, y - 5, 508, 25, rgbc(90, 130, 255), rgbc(10, 20, 60)); }
            sprintf(t, "%2d  %s", i + 1, cj[i].name); text(78, y, t, C_WHITE);
            text8(380, y + 4, cj[i].type == 'A' ? "shown" : cj[i].type == 'B' ? "practised" : cj[i].type == 'C' ? "twisted" : "customer", GRAY(13));
            sprintf(t, "%s  %c %c %c", m & 8 ? "PASSED" : "      ", m & 1 ? 'D' : '-', m & 2 ? 'E' : '-', m & 4 ? 'R' : '-'); text8(462, y + 4, t, m & 8 ? C_LEDG : GRAY(9));
        }
        text8(70, 88, "        R no seal broken after locking.", GRAY(15));
        text8(70, 432, "\x18\x19 choose   ENTER take the set to the bench   ESC back", C_SEL);
        sprintf(t, "Help: %s (change it in Setup)", help == 0 ? "Apprentice, with marks and readings" : help == 1 ? "Technician, readings only" : "Master, eyes only"); text8(70, 446, t, GRAY(13));
        vid_vsync(); vid_present();
        k = key_wait();
        if (k == K_ESC) return;
        if (k == K_UP) { sel = (sel + NCJ - 1) % NCJ; snd_sfx(SFX_TICK); }
        if (k == K_DOWN) { sel = (sel + 1) % NCJ; snd_sfx(SFX_TICK); }
        if (k == K_F12) vid_shot("SHELF.BMP");
        if (k == K_ENTER) { while (run_job(sel)) sel++; vid_invalidate(); }
    }
}
