/* Charge! Convergence, DOS edition. Shared declarations.
   Target: 486, DOS, VESA 640x480x256 (banked or linear), AdLib, Sound Blaster.
   Integer math only, so a 486SX without a coprocessor runs it. */
#ifndef CC_H
#define CC_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

/* ---------- video (vid.c) ---------- */
extern u8 *back;            /* back buffer, SW*SH bytes */
extern int SW, SH;
extern int vid_lfb;         /* 1 if a linear frame buffer is in use */
extern int vid_force_bank;  /* /BANK: never use the linear frame buffer */
extern u8 pal[768];         /* current logical palette, 6 bit */
extern int cx0, cy0, cx1, cy1; /* clip rectangle, inclusive */

int  vid_init(int w, int h);  /* 0 ok, else error code */
int  vid_mode_ok(int w, int h);
void vid_done(void);
void vid_present(void);       /* copy changed spans to the screen */
void vid_invalidate(void);    /* force a full copy at the next present */
void vid_vsync(void);
void pal_build(void);
void pal_apply(int level, int guns); /* level 0..64, guns bit0 R, bit1 G, bit2 B */
void pal_fade(int from, int to);
const char *vid_error(int code);
void vid_shot(const char *name);     /* write the back buffer as a BMP */

void clip(int x0, int y0, int x1, int y1);
void unclip(void);
void pset(int x, int y, int c);
void hline(int x0, int x1, int y, int c);
void vline(int x, int y0, int y1, int c);
void fill(int x, int y, int w, int h, int c);
void box(int x, int y, int w, int h, int c);
void bevel(int x, int y, int w, int h, int hi, int lo);
void line(int x0, int y0, int x1, int y1, int c);
void circle(int cx, int cy, int r, int c);
void disc(int cx, int cy, int r, int c);
int  rgbc(int r, int g, int b);              /* nearest cube colour, 0..255 in */
int  dith(int x, int y, int r, int g, int b);/* ordered-dither cube colour */
void dith16(int r, int g, int b, u8 *out);   /* the same colour for all 16 Bayer cells, index (y & 3) * 4 + (x & 3) */
void dfill(int x, int y, int w, int h, int r0, int g0, int b0, int r1, int g1, int b1); /* vertical dithered gradient */
void addrgb(int x, int y, int r, int g, int b); /* additive light, 0..255 per gun */
void addline(int x0, int y0, int x1, int y1, int r, int g, int b);
void addpix1(int x, int y, int c, int v);                 /* one gun only: 0 red, 1 green, 2 blue */
void addline1(int x0, int y0, int x1, int y1, int c, int v);
void text(int x, int y, const char *s, int c);     /* 8x16 ROM font */
void text8(int x, int y, const char *s, int c);    /* 8x8 ROM font */
void textbig(int x, int y, const char *s, int c, int sc); /* scaled 8x16 */
void textsh(int x, int y, const char *s, int c);   /* with drop shadow */
void textc(int xc, int y, const char *s, int c);   /* centred */
void textadd(int x, int y, const char *s, int sc, int r, int g, int b);

/* palette layout */
#define GRAY(n)  (216 + (n))      /* n = 0..23 */
#define C_BEIGE0 240              /* 240..246: monitor plastic, dark to light */
#define C_PANEL  247
#define C_PANELH 248
#define C_PANELL 249
#define C_AMBER  250
#define C_LEDG   251
#define C_LEDR   252
#define C_INK    253
#define C_SEL    254
#define C_WHITE  255
#define C_BLACK  0

/* ---------- timing and keys (sys.c) ---------- */
void sys_init(void);
void sys_done(void);
u32  ms_now(void);
int  key_get(void);     /* 0 if none; ASCII, or scan<<8 for extended keys */
int  key_wait(void);
int  key_more(void);    /* a key is waiting: skip the redraw and take it first */
void sys_idle(void);    /* call often: runs the sound sequencer */
extern int auto_mode;   /* scripted run for testing */
void auto_load(const char *file);
#define K_ESC   27
#define K_ENTER 13
#define K_TAB   9
#define K_UP    0x4800
#define K_DOWN  0x5000
#define K_LEFT  0x4B00
#define K_RIGHT 0x4D00
#define K_PGUP  0x4900
#define K_PGDN  0x5100
#define K_HOME  0x4700
#define K_END   0x4F00
#define K_F1    0x3B00
#define K_F2    0x3C00
#define K_F3    0x3D00
#define K_F4    0x3E00
#define K_F5    0x3F00
#define K_F6    0x4000
#define K_F12   0x8600

/* ---------- sound (snd.c) ---------- */
extern int snd_adlib, snd_sb, snd_music_on, snd_sfx_on;
extern int snd_no_adlib, snd_no_sb;
extern int sb_base, sb_irq, sb_dma, sb_ver;
void snd_init(void);
void snd_done(void);
void snd_poll(u32 now);
void snd_song(int n);   /* SONG_xxx, loops */
void snd_jingle(int n); /* JING_xxx, plays once over the song */
void snd_sfx(int n);    /* SFX_xxx */
enum { SONG_NONE, SONG_TITLE, SONG_BENCH };
enum { JING_WIN, JING_FAIL, JING_STAR };
enum { SFX_TICK, SFX_CLICK, SFX_DEGAUSS, SFX_THUNK, SFX_BUZZ, SFX_OK };

/* ---------- game (game.c, crt.c, text.c) ---------- */
void game_main(void);
extern const signed char sintab[256]; /* sin(2*pi*i/256)*127 */
#define SIN(a) ((int)sintab[(a) & 255])
#define COS(a) ((int)sintab[((a) + 64) & 255])

#endif
