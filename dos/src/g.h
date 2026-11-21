/* Game declarations: the simulated monitor and its controls. */
#ifndef G_H
#define G_H
#include "cc.h"

enum { CT_HS, CT_VS, CT_HC, CT_VC, CT_TILT, CT_PIN, CT_TRAP,
       CT_FOCUS, CT_BRIGHT,
       CT_PURA, CT_PURS, CT_P4A, CT_P4S, CT_P6A, CT_P6S,
       CT_ST0, CT_ST1, CT_ST2, CT_ST3, CT_ST4, CT_ST5, CT_ST6, CT_ST7, NCTL };
enum { G_GEO, G_PIC, G_MAG, G_STRIP, NGRP };
enum { PAT_HATCH, PAT_DOTS, PAT_RED, PAT_WHITE, PAT_PLUGE, NPAT };

typedef struct { const char *name; int grp, lo, hi, wrap; const char *what; const char *help[4]; const char *hint; } Ctl;
extern const Ctl ctl[NCTL];
extern const char *grpname[NGRP];
extern const char *patname[NPAT];
extern const char *pathelp[NPAT];

extern int val[NCTL], ideal[NCTL];
extern int magnetized, blot_u, blot_v;   /* stray magnetisation of the shadow mask */
extern int pattern;
extern int jitter_x, jitter_y, jitter_k; /* degauss wobble */

/* the tube face on the bench screen */
#define FX 28
#define FY 46
#define FW 416
#define FH 312

void crt_draw(void);                 /* draw the picture into the face rectangle */
int  ctl_err(int i);                 /* error of one control, 0 = exact; vector controls report on the angle index */
int  ctl_ok(int i);
int  grp_ok(int g);
int  grp_used(int g);                /* does this job have a fault in the group */
void vec_res(int a, int *x, int *y); /* residual of a ring pair, 127 = one spread unit */

/* text.c */
typedef struct { const char *name; const char *ticket[3]; u32 faults; int par; int pat; } Job;
#define NJOB 10
extern const Job jobs[NJOB];
extern const char *handbook[]; extern const int handbook_pages;
extern const char *tp_name[]; extern const char *tp_help[][6]; extern const int tp_count;
#define F_HS 1
#define F_VS 2
#define F_HC 4
#define F_VC 8
#define F_TILT 16
#define F_PIN 32
#define F_TRAP 64
#define F_FOCUS 128
#define F_BRIGHT 256
#define F_MAGNET 512
#define F_PUR 1024
#define F_P4 2048
#define F_P6 4096
#define F_STRIP1 8192
#define F_STRIP2 16384
#define F_STRIP3 32768

/* cbench.c: the Convergence Bench, the same game as the browser edition's main page */
void cbench_menu(int help_level);
extern u8 cb_medal[12];

/* patterns.c */
void testpatterns(void);
#endif
