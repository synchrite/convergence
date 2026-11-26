/* Slow-motion view: tables (sm_tab.c, generated) and physics (smphys.c). Whole numbers only. */
#ifndef SM_H
#define SM_H
#define SM_GN 497                       /* gun tables: one entry per 1/8 mm, 0..62 mm */
extern const unsigned short sm_gun_v[SM_GN];    /* speed, (1/65536 mm per fs) x 4096 */
extern const unsigned short sm_gun_ev[SM_GN];   /* energy, eV */
extern const short sm_gun_w[SM_GN];             /* beam half-width, 1/256 mm, signed (crossover) */
#define SM_NB 64                        /* energy bins, 300 eV to 25.6 keV, log spaced */
extern const unsigned short sm_e[SM_NB];        /* eV */
extern const unsigned short sm_vel[SM_NB];      /* nm per fs, x256 */
extern const unsigned short sm_lam[3][SM_NB];   /* elastic mean free path, nm x256; material 0 Al, 1 ZnS, 2 Y2O2S */
extern const unsigned short sm_de[3][SM_NB];    /* energy loss, eV per nm, x256 */
extern const unsigned short sm_al[3][SM_NB];    /* screening parameter, x65536 */
extern const unsigned short sm_ln[256];         /* -ln((i+0.5)/256) x256 */

/* flight: where beam g (0 red, 1 green, 2 blue) crosses the plane x = xend (mm), for a field of b100/100 mT. Result in 1/256 mm. */
int sm_trace(int g, int b100, int xend);
#define SM_BRHO100 53970                /* momentum / charge of a 25 keV electron, mT mm x100 */
#define SM_XM 28636                     /* mask plane, mm x100 */

/* phosphor: one electron's path. Lengths in nm, energy in eV, time in 1/16 fs. */
typedef struct { short x, y; unsigned short e, t; } SmPt;
typedef struct { int n, nsp, e_al, e_ph, e_back, tend; } SmTrack;
#define SM_MAXPT 700
#define SM_MAXSP 110
int sm_isqrt(unsigned v);
unsigned sm_rnd(void);                  /* 16 random bits */
void sm_seed(unsigned s);
void sm_track(int mat, int y0, SmPt *pts, SmPt *sp, SmTrack *out);   /* mat 1 or 2; aluminium film 200 nm in front */
#endif
