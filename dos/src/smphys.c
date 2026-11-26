/* Slow-motion view: the physics, in whole numbers. No hardware access here, so this file also compiles
   on any other machine for testing (tools/sm_hosttest.c). */
#include "sm.h"
extern const signed char sintab[256];
#define SIN(a) ((int)sintab[(a) & 255])
#define COS(a) ((int)sintab[((a) + 64) & 255])

int sm_isqrt(unsigned v) { unsigned r = 0, b = 1u << 30; while (b > v) b >>= 2; while (b) { if (v >= r + b) { v -= r + b; r = (r >> 1) + b; } else r >>= 1; b >>= 2; } return (int)r; }
static unsigned rs = 0x2545F491u;
unsigned sm_rnd(void);
void sm_seed(unsigned s) { int i; rs = s * 2654435761u; if (!rs) rs = 1; for (i = 0; i < 24; i++) sm_rnd(); }   /* mix a small seed before use */
unsigned sm_rnd(void) { rs ^= rs << 13; rs ^= rs >> 17; rs ^= rs << 5; return (rs >> 8) & 0xFFFF; }

/* ---------------- flight ---------------- */
void sm_fly_start(SmFly *p, int g)
{
    static const int sy[3] = { SM_SG, 0, -SM_SG };
    p->x = 0; p->y = sy[g];
    p->dx = g == 1 ? 1048576 : 1048383;            /* cos and sin of atan(5.5 / 286.36), 20 fraction bits */
    p->dy = g == 0 ? -20136 : g == 2 ? 20136 : 0;
}
void sm_fly_step(SmFly *p, int b100)
{
    if (b100 && p->x >= 40 * 65536 && p->x < 100 * 65536) {
        long long k = (long long)b100 * 524288 / 53970;                     /* half a millimetre over the radius, 20 fraction bits */
        p->dy += (int)(((long long)p->dx * k) >> 20);
        p->dx -= (int)(((long long)p->dy * k) >> 20);
    }
    p->x += p->dx >> 5; p->y += p->dy >> 5;
}
int sm_trace(int g, int b100, int xend100)
{
    SmFly p; int xe = (int)((long long)xend100 * 65536 / 100), n = 0;
    sm_fly_start(&p, g);
    while (p.x < xe && p.dx > 65536 && n++ < 2000) sm_fly_step(&p, b100);
    return (int)((p.y - (long long)(p.x - xe) * p.dy / p.dx) >> 8);
}

/* ---------------- phosphor: single-scattering Monte Carlo ----------------
   Position in 1/16 nm, direction with 14 fraction bits, energy in eV, time in 1/16 fs.
   x = 0 is the back of the aluminium film (200 nm thick); the phosphor begins behind it. */
#define FILM (200 * 16)
void sm_track(int mat, int y0, SmPt *pts, SmPt *sp, SmTrack *o)
{
    int x = 1, y = y0 * 16, cx = 16384, cy = 0, cz = 0, E = sm_e[SM_NB - 1] < 25000 ? sm_e[SM_NB - 1] : 25000, b = SM_NB - 1, t = 0;
    int guard = 0, acc = 0, lx = -1000000, ly = 0;
    o->n = o->nsp = 0; o->e_al = o->e_ph = o->e_back = 0;
#define PUT() { if (o->n < SM_MAXPT) { pts[o->n].x = (short)(x >> 4); pts[o->n].y = (short)(y >> 4); pts[o->n].e = (unsigned short)E; pts[o->n].t = (unsigned short)t; o->n++; } lx = x; ly = y; }
    PUT();
    while (E > 300 && guard++ < 20000) {
        int m = x < FILM ? 0 : mat, s, dE, scatter = 1, px = x, py = y;
        unsigned al;
        while (b > 0 && sm_e[b] > E) b--;
        al = sm_al[m][b];
        s = (int)(((unsigned)sm_lam[m][b] * sm_ln[sm_rnd() & 255]) >> 12);
        if (s < 1) s = 1;
        if (cx) {                                   /* stop at a change of material, and go on from there without turning */
            int bx = x < FILM ? (cx > 0 ? FILM : 0) : (cx < 0 ? FILM : -1);
            if (bx >= 0) { long long d = (long long)(bx - x) * 16384 / cx; if (d >= 0 && d < s) { s = (int)d + 2; scatter = 0; } }
        }
        dE = (int)(((unsigned)sm_de[m][b] * (unsigned)s) >> 12);
        if (dE < 1) dE = 1;
        if (dE > E - 50) dE = E - 50;
        x += (cx * s) >> 14; y += (cy * s) >> 14;
        t += s * 256 / sm_vel[b]; E -= dE;
        if (m == 0) o->e_al += dE;
        else {
            o->e_ph += dE; acc += dE;
            while (acc >= 250) { acc -= 250; if (o->nsp < SM_MAXSP) { int f = sm_rnd() & 255; sp[o->nsp].x = (short)((px + ((x - px) * f >> 8)) >> 4); sp[o->nsp].y = (short)((py + ((y - py) * f >> 8)) >> 4); sp[o->nsp].e = 0; sp[o->nsp].t = (unsigned short)t; o->nsp++; } }
        }
        if (x < 0) { o->e_back = E; PUT(); o->tend = t; return; }
        if (abs(x - lx) + abs(y - ly) > 24 * 16) PUT();
        if (scatter) {
            unsigned R = sm_rnd(); int den = 65536 + (int)al - (int)R, ct, st, ph, cp, sn, nx, ny, nz, q, n;
            ct = 16384 - (int)(al * R / (2u * (unsigned)den)); st = sm_isqrt((unsigned)(16384 * 16384 - ct * ct));
            ph = sm_rnd() & 255; cp = COS(ph); sn = SIN(ph);
            if (cz > 16380 || cz < -16380) { nx = st * cp / 127; ny = st * sn / 127; nz = cz > 0 ? ct : -ct; }
            else {
                q = sm_isqrt((unsigned)(16384 * 16384 - cz * cz));
                nx = st * (((cx * cz) >> 14) * cp / 127 - cy * sn / 127) / q + ((cx * ct) >> 14);
                ny = st * (((cy * cz) >> 14) * cp / 127 + cx * sn / 127) / q + ((cy * ct) >> 14);
                nz = -(((q * st) >> 14) * cp / 127) + ((cz * ct) >> 14);
            }
            n = sm_isqrt((unsigned)(nx * nx + ny * ny + nz * nz));
            if (n > 0) { cx = nx * 16384 / n; cy = ny * 16384 / n; cz = nz * 16384 / n; }
        }
    }
    if (x < FILM) o->e_al += E; else o->e_ph += E;
    PUT(); o->tend = t;
#undef PUT
}
