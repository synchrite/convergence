/* Sound: AdLib (OPL2) music and effects, Sound Blaster digitised effects, PC speaker fallback.
   Everything is polled from the main loop. No interrupt handlers are installed. */
#include <i86.h>
#include <conio.h>
#include <ctype.h>
#include "cc.h"

int snd_adlib, snd_sb, snd_music_on = 1, snd_sfx_on = 1;
int snd_no_adlib, snd_no_sb;
int sb_base = 0x220, sb_irq = 5, sb_dma = 1, sb_ver;

/* ---------------- OPL2 ---------------- */
static void opl(int reg, int val)
{
    int i;
    outp(0x388, reg); for (i = 0; i < 6; i++) inp(0x388);
    outp(0x389, val); for (i = 0; i < 35; i++) inp(0x388);
}
static int adlib_detect(void)
{
    int a, b, i;
    opl(4, 0x60); opl(4, 0x80); a = inp(0x388);
    opl(2, 0xFF); opl(4, 0x21);
    for (i = 0; i < 400; i++) inp(0x388);
    b = inp(0x388);
    opl(4, 0x60); opl(4, 0x80);
    return (a & 0xE0) == 0 && (b & 0xE0) == 0xC0;
}
typedef struct { u8 mc, cc, ml, cl, mad, cad, msr, csr, mw, cw, fb; } Ins;
enum { I_LEAD, I_BASS, I_PAD, I_PLUCK, I_TICK, I_CHIME, I_HUM, I_BUZZ, I_KICK };
static const Ins ins[] = {
    {0x21, 0x21, 0x1A, 0x00, 0xF2, 0xF2, 0x53, 0x74, 0, 0, 0x0C},  /* lead */
    {0x01, 0x01, 0x10, 0x00, 0xF2, 0xF3, 0x66, 0x77, 0, 0, 0x08},  /* bass */
    {0x61, 0x21, 0x20, 0x06, 0x53, 0x64, 0x24, 0x25, 0, 0, 0x02},  /* pad */
    {0x02, 0x01, 0x18, 0x02, 0xF4, 0xF5, 0x34, 0x45, 0, 0, 0x06},  /* pluck */
    {0x04, 0x02, 0x08, 0x00, 0xF8, 0xF8, 0xF8, 0xF8, 0, 0, 0x04},  /* tick */
    {0x07, 0x01, 0x20, 0x00, 0xF3, 0xF3, 0x35, 0x36, 0, 0, 0x00},  /* chime */
    {0x21, 0x21, 0x08, 0x00, 0xF0, 0xF0, 0x0F, 0x0F, 1, 0, 0x0E},  /* hum */
    {0x21, 0x22, 0x00, 0x00, 0xF1, 0xF1, 0x0F, 0x0F, 2, 2, 0x0E},  /* buzz */
    {0x00, 0x00, 0x00, 0x00, 0xF7, 0xF6, 0xF7, 0xF6, 0, 0, 0x0A}   /* kick */
};
static const u8 opoff[9] = { 0, 1, 2, 8, 9, 10, 16, 17, 18 };
static const u16 fnum[12] = { 0x157, 0x16B, 0x181, 0x198, 0x1B0, 0x1CA, 0x1E5, 0x202, 0x220, 0x241, 0x263, 0x287 };
static u8 chb0[9];
static void opl_ins(int ch, int n, int att)
{
    const Ins *p = &ins[n]; int m = opoff[ch], c = m + 3, lv = (p->cl & 0x3F) + att;
    if (lv > 63) lv = 63;
    opl(0x20 + m, p->mc); opl(0x20 + c, p->cc);
    opl(0x40 + m, p->ml); opl(0x40 + c, (p->cl & 0xC0) | lv);
    opl(0x60 + m, p->mad); opl(0x60 + c, p->cad);
    opl(0x80 + m, p->msr); opl(0x80 + c, p->csr);
    opl(0xE0 + m, p->mw); opl(0xE0 + c, p->cw);
    opl(0xC0 + ch, p->fb);
}
static void opl_off(int ch) { chb0[ch] &= ~0x20; opl(0xB0 + ch, chb0[ch]); }
static void opl_on(int ch, int midi)
{
    int blk = midi / 12 - 1, f = fnum[midi % 12];
    if (blk < 0) blk = 0;
    if (blk > 7) blk = 7;
    opl(0xB0 + ch, chb0[ch] & ~0x20);
    opl(0xA0 + ch, f & 255);
    chb0[ch] = (u8)(0x20 | (blk << 2) | (f >> 8));
    opl(0xB0 + ch, chb0[ch]);
}
static void opl_reset(void)
{
    int i;
    for (i = 1; i < 0xF6; i++) opl(i, 0);
    opl(1, 0x20);   /* enable waveform select */
    memset(chb0, 0, sizeof chb0);
}

/* ---------------- songs ---------------- */
#define MAXSTEP 128
typedef struct { int stepms, nch, loop; const char *trk[4]; u8 in[4], att[4]; } Song;
static const Song songs[] = {
  { 0, 0, 0, {0}, {0}, {0} },
  { 125, 4, 1, {   /* title: Am F C G */
    "A1 . A1 . A2 . A1 . A1 . A2 . E2 . A1 . F1 . F1 . F2 . F1 . F1 . F2 . C2 . F1 . C2 . C2 . C3 . C2 . C2 . C3 . G2 . C2 . G1 . G1 . G2 . G1 . G1 . G2 . D2 . B1 .",
    "A3 C4 E4 A4 E4 C4 A3 C4 E4 A4 E4 C4 A3 C4 E4 C4 F3 A3 C4 F4 C4 A3 F3 A3 C4 F4 C4 A3 F3 A3 C4 A3 C4 E4 G4 C5 G4 E4 C4 E4 G4 C5 G4 E4 C4 E4 G4 E4 G3 B3 D4 G4 D4 B3 G3 B3 D4 G4 D4 B3 G3 B3 D4 B3",
    "E5 . . . . . D5 . C5 . . . A4 . . . C5 . . . . . A4 . F4 . . . A4 . C5 . E5 . . . . . G5 . E5 . . . C5 . . . D5 . . . . . B4 . G4 . . . B4 . D5 .",
    "C4 . . . . . . . . . . . . . . . A3 . . . . . . . . . . . . . . . G3 . . . . . . . . . . . . . . . B3 . . . . . . . . . . . . . . ." },
    { I_BASS, I_PLUCK, I_LEAD, I_PAD }, { 4, 14, 8, 18 } },
  { 210, 3, 1, {   /* bench: Dm7 G C A7, quiet */
    "D2 . . . . . . . A2 . . . . . . . G1 . . . . . . . D2 . . . . . . . C2 . . . . . . . G2 . . . . . . . A1 . . . . . . . E2 . . . . . . .",
    "- - F4 . A4 . C5 . - - A4 . F4 . D4 . - - D4 . G4 . B4 . - - G4 . D4 . B3 . - - E4 . G4 . B4 . - - G4 . E4 . C4 . - - E4 . G4 . C#5 . - - A4 . E4 . C#4 .",
    "F3 . . . . . . . . . . . . . . . F3 . . . . . . . . . . . . . . . E3 . . . . . . . . . . . . . . . G3 . . . . . . . . . . . . . . .", 0 },
    { I_BASS, I_PLUCK, I_PAD, 0 }, { 14, 22, 26, 0 } }
};
static const Song jings[] = {
  { 90, 2, 0, { "C5 E5 G5 C6 . G5 C6 . . . . -", "C4 . . G4 . . C5 . . . . -", 0, 0 }, { I_CHIME, I_PLUCK }, { 0, 6 } },
  { 130, 2, 0, { "E4 . D#4 . D4 . C#4 . . . . -", "A2 . . . . . A1 . . . . -", 0, 0 }, { I_LEAD, I_BASS }, { 6, 4 } },
  { 70, 1, 0, { "G5 C6 E6 . -", 0, 0, 0 }, { I_CHIME }, { 0 } }
};
typedef struct { const Song *s; u8 note[4][MAXSTEP]; int len, pos, acc, base, on; } Player;
static Player mus, jin;

static int parse(const char *t, u8 *out)
{
    int n = 0;
    while (*t && n < MAXSTEP) {
        while (*t == ' ') t++;
        if (!*t) break;
        if (*t == '.') { out[n++] = 0; t++; }
        else if (*t == '-') { out[n++] = 1; t++; }
        else {
            static const signed char sem[7] = { 9, 11, 0, 2, 4, 5, 7 };   /* A..G */
            int v = sem[toupper(*t) - 'A']; t++;
            if (*t == '#') { v++; t++; }
            v += (*t - '0' + 1) * 12; t++;
            out[n++] = (u8)v;
        }
    }
    return n;
}
static void play_start(Player *p, const Song *s, int base)
{
    int c, l;
    for (c = 0; c < 4; c++) if (p->on && snd_adlib) opl_off(p->base + c);
    p->s = s; p->base = base; p->pos = 0; p->acc = 0; p->len = 0; p->on = 0;
    if (!s || !s->nch) return;
    for (c = 0; c < s->nch; c++) {
        l = parse(s->trk[c], p->note[c]);
        if (l > p->len) p->len = l;
        if (snd_adlib) opl_ins(base + c, s->in[c], s->att[c]);
    }
    p->on = 1; p->acc = s->stepms;   /* first step sounds at once */
}
static void play_step(Player *p)
{
    int c;
    for (c = 0; c < p->s->nch; c++) {
        int v = p->note[c][p->pos];
        if (v == 1) opl_off(p->base + c);
        else if (v > 1) opl_on(p->base + c, v);
    }
    if (++p->pos >= p->len) {
        if (p->s->loop) p->pos = 0;
        else { for (c = 0; c < p->s->nch; c++) opl_off(p->base + c); p->on = 0; }
    }
}

/* ---------------- PC speaker ---------------- */
static struct { u16 f, ms; } spq[8];
static int spn, spi, spt;
static void spk(int f)
{
    if (f <= 0) { outp(0x61, inp(0x61) & ~3); return; }
    f = 1193180 / f;
    outp(0x43, 0xB6); outp(0x42, f & 255); outp(0x42, f >> 8);
    outp(0x61, inp(0x61) | 3);
}
static void spk_seq(const u16 *fm, int n)
{
    int i;
    for (i = 0; i < n && i < 8; i++) { spq[i].f = fm[i * 2]; spq[i].ms = fm[i * 2 + 1]; }
    spn = n; spi = 0; spt = 0; spk(spq[0].f);
}

/* ---------------- Sound Blaster (8 bit, single cycle DMA, polled) ---------------- */
static u8 *dmabuf; static u32 dmaphys;
static int sb_busy; static u8 picmask_old; static int pic_saved;
static u8 *smp[3]; static int smplen[3];
static void tiny_delay(int n) { while (n-- > 0) inp(0x388); }
static int dsp_write(int v)
{
    int n = 0;
    while ((inp(sb_base + 0xC) & 0x80) && ++n < 100000) ;
    if (n >= 100000) return 0;
    outp(sb_base + 0xC, v); return 1;
}
static int dsp_read(void)
{
    int n = 0;
    while (!(inp(sb_base + 0xE) & 0x80) && ++n < 100000) ;
    return n >= 100000 ? -1 : inp(sb_base + 0xA);
}
static int dsp_reset(void)
{
    outp(sb_base + 6, 1); tiny_delay(200); outp(sb_base + 6, 0); tiny_delay(200);
    return dsp_read() == 0xAA;
}
static int blaster_env(void)
{
    const char *e = getenv("BLASTER");
    if (!e) return 0;
    while (*e) {
        int c = toupper(*e++);
        if (c == 'A') sb_base = (int)strtol(e, 0, 16);
        else if (c == 'I') sb_irq = atoi(e);
        else if (c == 'D') sb_dma = atoi(e);
        while (*e && *e != ' ') e++;
        while (*e == ' ') e++;
    }
    return 1;
}
static u32 rng = 12345;
static int noise(void) { rng = rng * 1103515245u + 12345u; return (int)((rng >> 16) & 255) - 128; }
static void make_samples(void)
{
    int i, n; u16 p1 = 0, p2 = 0, p3 = 0;
    /* degauss: mains hum that swells and dies away, with a thump at the start */
    n = smplen[0] = 14000; smp[0] = (u8 *)malloc(n);
    for (i = 0; i < n; i++) {
        int env = (n - i) / 55, wob, s;                 /* 254..0 */
        p1 += 60 * 65536L / 11025; p2 += 120 * 65536L / 11025; p3 += 7 * 65536L / 11025;
        wob = 96 + SIN(p3 >> 8) / 4;
        s = (SIN(p1 >> 8) * 3 + SIN(p2 >> 8) * 2 + ((p1 >> 8) & 128 ? 40 : -40)) / 6;
        s = s * wob / 128 * env / 254;
        if (i < 700) s += noise() * (700 - i) / 1400;
        if (s > 127) s = 127;
        if (s < -127) s = -127;
        smp[0][i] = (u8)(128 + s);
    }
    /* click: a relay or a pot detent */
    n = smplen[1] = 450; smp[1] = (u8 *)malloc(n);
    for (i = 0; i < n; i++) smp[1][i] = (u8)(128 + noise() * (n - i) / n * (n - i) / n / 2);
    /* thunk: rubber stamp */
    n = smplen[2] = 2600; smp[2] = (u8 *)malloc(n); p1 = 0;
    for (i = 0; i < n; i++) {
        int f = 120 - i * 80 / n, s;
        p1 += (u16)(f * 65536L / 11025);
        s = SIN(p1 >> 8) * (n - i) / n;
        if (i < 300) s += noise() * (300 - i) / 500;
        if (s > 127) s = 127;
        if (s < -127) s = -127;
        smp[2][i] = (u8)(128 + s);
    }
}
static int sb_init(void)
{
    union REGS r; u32 a;
    if (!blaster_env()) return 0;      /* only touch the card where the BLASTER variable says it is */
    if (sb_dma > 3 || sb_dma == 2) return 0;
    if (!dsp_reset()) return 0;
    dsp_write(0xE1); sb_ver = dsp_read() << 8; sb_ver |= dsp_read() & 255;
    /* 32 KB of conventional memory; use a 16 KB part that does not cross a 64 KB page */
    r.w.ax = 0x100; r.w.bx = 0x800; int386(0x31, &r, &r);
    if (r.x.cflag) return 0;
    a = (u32)r.w.ax << 4;
    if ((a & 0xFFFF) + 16384 > 0x10000) a = (a + 0xFFFF) & ~0xFFFFul;
    dmaphys = a; dmabuf = (u8 *)a;
    make_samples();
    /* keep the card's interrupt line masked: completion is polled */
    if (sb_irq < 8) { picmask_old = (u8)inp(0x21); outp(0x21, picmask_old | (1 << sb_irq)); pic_saved = 1; }
    dsp_write(0xD1);
    return 1;
}
static void sb_play(int n)
{
    static const u8 addr[4] = { 0, 2, 4, 6 }, cnt[4] = { 1, 3, 5, 7 }, page[4] = { 0x87, 0x83, 0x81, 0x82 };
    int len = smplen[n];
    if (len > 16000) len = 16000;
    if (sb_busy) { dsp_write(0xD0); inp(sb_base + 0xE); }
    memcpy(dmabuf, smp[n], len);
    outp(0x0A, 4 | sb_dma); outp(0x0C, 0); outp(0x0B, 0x48 | sb_dma);
    outp(addr[sb_dma], dmaphys & 255); outp(addr[sb_dma], (dmaphys >> 8) & 255); outp(page[sb_dma], (dmaphys >> 16) & 255);
    outp(cnt[sb_dma], (len - 1) & 255); outp(cnt[sb_dma], (len - 1) >> 8);
    outp(0x0A, sb_dma);
    dsp_write(0x40); dsp_write(256 - 1000000 / 11025);
    dsp_write(0x14); dsp_write((len - 1) & 255); dsp_write((len - 1) >> 8);
    sb_busy = 1;
}
static void sb_poll(void)
{
    if (!sb_busy) return;
    if (inp(0x08) & (1 << sb_dma)) { inp(sb_base + 0xE); sb_busy = 0; }   /* terminal count reached */
}

/* ---------------- effects ---------------- */
static int sfx_left;      /* ms until the FM effect channel is released */
static int hum_t;         /* FM degauss in progress */
void snd_sfx(int n)
{
    static const u16 s_tick[] = { 1800, 12 }, s_click[] = { 900, 15 }, s_ok[] = { 1319, 60, 1760, 90 }, s_buzz[] = { 110, 250 },
        s_thunk[] = { 140, 40, 90, 60 }, s_deg[] = { 60, 200, 55, 200, 50, 300, 45, 300 };
    if (!snd_sfx_on) return;
    if (snd_sb && (n == SFX_DEGAUSS || n == SFX_CLICK || n == SFX_THUNK)) { sb_play(n == SFX_DEGAUSS ? 0 : n == SFX_CLICK ? 1 : 2); return; }
    if (snd_adlib) {
        switch (n) {
        case SFX_TICK:    opl_ins(8, I_TICK, 6); opl_on(8, 96); sfx_left = 30; break;
        case SFX_CLICK:   opl_ins(8, I_KICK, 0); opl_on(8, 60); sfx_left = 40; break;
        case SFX_OK:      opl_ins(8, I_CHIME, 0); opl_on(8, 88); sfx_left = 220; break;
        case SFX_BUZZ:    opl_ins(8, I_BUZZ, 2); opl_on(8, 33); sfx_left = 280; break;
        case SFX_THUNK:   opl_ins(8, I_KICK, 0); opl_on(8, 36); sfx_left = 120; break;
        case SFX_DEGAUSS: opl_ins(8, I_HUM, 0); opl_on(8, 35); sfx_left = 1300; hum_t = 1300; break;
        }
        return;
    }
    switch (n) {
    case SFX_TICK: spk_seq(s_tick, 1); break;
    case SFX_CLICK: spk_seq(s_click, 1); break;
    case SFX_OK: spk_seq(s_ok, 2); break;
    case SFX_BUZZ: spk_seq(s_buzz, 1); break;
    case SFX_THUNK: spk_seq(s_thunk, 2); break;
    case SFX_DEGAUSS: spk_seq(s_deg, 4); break;
    }
}
void snd_song(int n)
{
    static int cur = -1;
    if (n == cur) return;
    cur = n;
    play_start(&mus, (snd_adlib && snd_music_on && n) ? &songs[n] : 0, 0);
}
void snd_jingle(int n)
{
    static const u16 w[] = { 523, 80, 659, 80, 784, 80, 1047, 200 }, f[] = { 330, 150, 294, 150, 262, 300 }, s[] = { 784, 60, 1047, 60, 1319, 120 };
    if (!snd_sfx_on) return;
    if (snd_adlib) { play_start(&jin, &jings[n], 5); return; }
    if (n == JING_WIN) spk_seq(w, 4); else if (n == JING_FAIL) spk_seq(f, 3); else spk_seq(s, 3);
}
void snd_poll(u32 now)
{
    static u32 last; int dt = (int)(now - last);
    if (dt <= 0) { if (dt < -1000) last = now; return; }
    last = now;
    if (dt > 250) dt = 250;
    if (snd_sb) sb_poll();
    if (spn) {
        spt += dt;
        if (spt >= spq[spi].ms) { spt = 0; if (++spi >= spn) { spn = 0; spk(0); } else spk(spq[spi].f); }
    }
    if (!snd_adlib) return;
    if (mus.on) { mus.acc += dt; while (mus.on && mus.acc >= mus.s->stepms) { mus.acc -= mus.s->stepms; play_step(&mus); } }
    if (jin.on) { jin.acc += dt; while (jin.on && jin.acc >= jin.s->stepms) { jin.acc -= jin.s->stepms; play_step(&jin); } }
    if (sfx_left > 0) {
        sfx_left -= dt;
        if (hum_t > 0) { hum_t -= dt; opl(0x40 + opoff[8] + 3, 63 - (hum_t > 0 ? hum_t * 63 / 1300 : 0)); }
        if (sfx_left <= 0) { opl_off(8); hum_t = 0; }
    }
}
void snd_init(void)
{
    snd_adlib = !snd_no_adlib && adlib_detect();
    if (snd_adlib) opl_reset();
    snd_sb = !snd_no_sb && sb_init();
}
void snd_done(void)
{
    spk(0);
    if (snd_adlib) opl_reset();
    if (snd_sb) { dsp_write(0xD0); dsp_write(0xD3); dsp_reset(); }
    if (pic_saved) outp(0x21, picmask_old);
}
