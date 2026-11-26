/* Menus, the service bench, scoring. */
#include "g.h"

static int diff;                 /* 0 apprentice, 1 journeyman, 2 master */
static u8 stars[NJOB];
static u8 cb_medal[12];          /* unused: kept so version 2 save files keep their layout */
static u8 *bgcache;
static u32 faultmask;
static const char *diffname[3] = { "APPRENTICE", "JOURNEYMAN", "MASTER" };

/* ---------------- save file ---------------- */
static void save(void)
{
    FILE *f = fopen("CONVERGE.SAV", "wb");
    if (!f) return;
    fputc('C', f); fputc('2', f); fputc(diff, f); fputc(snd_music_on, f); fputc(snd_sfx_on, f);
    fwrite(stars, 1, NJOB, f); fwrite(cb_medal, 1, 12, f); fclose(f);
}
static void load(void)
{
    FILE *f = fopen("CONVERGE.SAV", "rb");
    if (!f) return;
    int ver;
    if (fgetc(f) == 'C' && ((ver = fgetc(f)) == '1' || ver == '2')) {
        diff = fgetc(f) % 3; snd_music_on = fgetc(f) & 1; snd_sfx_on = fgetc(f) & 1;
        fread(stars, 1, NJOB, f);
        if (ver == '2') fread(cb_medal, 1, 12, f);
    }
    fclose(f);
}

/* ---------------- shared drawing ---------------- */
static void panel(int x, int y, int w, int h)
{
    fill(x, y, w, h, C_PANEL); bevel(x, y, w, h, C_PANELH, C_PANELL); bevel(x + 1, y + 1, w - 2, h - 2, C_PANELH, C_PANELL);
}
static void workshop(void)   /* background: a dark wall with a pegboard */
{
    int x, y;
    dfill(0, 0, SW, SH, 30, 40, 62, 8, 11, 22);
    for (y = 12; y < SH; y += 16) for (x = 12 + ((y / 16) & 1) * 8; x < SW; x += 16) { pset(x, y, 0); pset(x + 1, y, 0); pset(x, y + 1, 0); pset(x + 1, y + 1, rgbc(60, 75, 105)); }
}
static void cache_bg(void) { if (!bgcache) bgcache = (u8 *)malloc((u32)640 * 480); if (bgcache) memcpy(bgcache, back, (u32)640 * 480); }
static void restore(int x, int y, int w, int h)
{
    int j;
    if (!bgcache) { fill(x, y, w, h, C_PANEL); return; }
    for (j = 0; j < h; j++) memcpy(back + (u32)(y + j) * SW + x, bgcache + (u32)(y + j) * SW + x, w);
}
static void star(int cx, int cy, int on)
{
    int i, c = on ? C_SEL : GRAY(6);
    for (i = 0; i < 5; i++) {
        int a = i * 256 / 5 - 64, b = a + 102;
        line(cx + COS(a) * 9 / 127, cy + SIN(a) * 9 / 127, cx + COS(b) * 9 / 127, cy + SIN(b) * 9 / 127, c);
    }
    if (on) disc(cx, cy, 3, c);
}
static void present(void) { vid_vsync(); vid_present(); }

/* ---------------- title and menus ---------------- */
static void logo(int d)
{
    restore(100, 30, 440, 118);
    clip(100, 30, 539, 147);
    textbig(264, 36, "CHARGE!", C_AMBER, 2);
    textadd(144 - d, 76 - d / 2, "CONVERGENCE", 4, 255, 0, 0);
    textadd(144, 76, "CONVERGENCE", 4, 0, 255, 0);
    textadd(144 + d, 76 + d / 2, "CONVERGENCE", 4, 0, 0, 255);
    unclip();
}
static void menu_bg(void)
{
    int x, y;
    workshop();
    fill(100, 30, 440, 130, 0);
    for (y = 30; y < 160; y++) for (x = 100; x < 540; x++) if ((x % 3) == 0) addrgb(x, y, 12, 0, 0); else if ((x % 3) == 1) addrgb(x, y, 0, 12, 0); else addrgb(x, y, 0, 0, 16);
    for (y = -1; y <= 1; y++) { addline(0, 40 + y, 99, 95 + y, y ? 90 : 255, 0, 0); addline(0, 95 + y, 99, 95 + y, 0, y ? 90 : 255, 0); addline(0, 150 + y, 99, 95 + y, 0, 0, y ? 110 : 255); }
    for (y = -1; y <= 1; y++) for (x = 540; x < 640; x++) addrgb(x, 95 + y, y ? 70 : 220, y ? 70 : 220, y ? 70 : 220);
    { static const u8 bc[8][3] = { {255,255,255},{255,255,0},{0,255,255},{0,255,0},{255,0,255},{255,0,0},{0,0,255},{60,60,60} };
      for (x = 0; x < 8; x++) fill(104 + x * 54, 150, 54, 6, rgbc(bc[x][0], bc[x][1], bc[x][2])); }
    for (x = 0; x < 3; x++) bevel(97 + x, 27 + x, 446 - 2 * x, 136 - 2 * x, C_BEIGE0 + 6 - x * 2, C_BEIGE0 + x);
    cache_bg();
}
static int menu(const char *title, const char **items, int n, int sel, const char *foot)
{
    int k, i, dirty = 1;
    for (;;) {
        if (dirty) {
            restore(120, 170, 400, 250);
            if (title) textc(320, 174, title, C_AMBER);
            for (i = 0; i < n; i++) {
                int y = 200 + i * 28;
                if (i == sel) { fill(170, y - 4, 300, 24, rgbc(20, 40, 110)); bevel(170, y - 4, 300, 24, rgbc(90, 130, 255), rgbc(10, 20, 60));
                    fill(176, y + 2, 4, 12, rgbc(255, 0, 0)); fill(182, y + 2, 4, 12, rgbc(0, 255, 0)); fill(188, y + 2, 4, 12, rgbc(0, 0, 255)); }
                textc(320, y, items[i], i == sel ? C_WHITE : GRAY(15));
            }
            if (foot) { restore(0, 440, 640, 40); text8(320 - (int)strlen(foot) * 4, 462, foot, GRAY(13)); }
            present(); dirty = 0;
        }
        k = key_wait(); dirty = 1;
        if (k == K_UP) { sel = (sel + n - 1) % n; snd_sfx(SFX_TICK); }
        else if (k == K_DOWN) { sel = (sel + 1) % n; snd_sfx(SFX_TICK); }
        else if (k == K_ENTER || k == ' ') { snd_sfx(SFX_CLICK); return sel; }
        else if (k == K_ESC) return -1;
        else if (k == K_F12) vid_shot("MENU.BMP");
    }
}
static void status_line(char *t)
{
    char sb[60];
    if (snd_sb) sprintf(sb, "Sound Blaster %d.%02d A%X I%d D%d", sb_ver >> 8, sb_ver & 255, sb_base, sb_irq, sb_dma); else strcpy(sb, "no Sound Blaster");
    sprintf(t, "VESA %s   %s   %s", vid_lfb ? "linear" : "banked", snd_adlib ? "AdLib FM" : "no AdLib", sb);
}

/* ---------------- handbook ---------------- */
static void page_text(int x, int y, const char *s, int c0)
{
    char ln[100]; int n, first = 1;
    while (*s) {
        n = 0; while (*s && *s != '\n' && n < 99) ln[n++] = *s++;
        ln[n] = 0; if (*s == '\n') s++;
        text(x, y, ln, first ? c0 : C_WHITE); first = 0; y += 16;
    }
}
static void handbook_view(void)
{
    int p = 0, k; char t[60];
    for (;;) {
        workshop(); panel(8, 8, 624, 464);
        page_text(24, 22, handbook[p], C_AMBER);
        sprintf(t, "Page %d of %d    \x1B \x1A turn page    ESC back", p + 1, handbook_pages);
        text8(24, 454, t, C_SEL);
        present();
        k = key_wait();
        if (k == K_ESC) return;
        if (k == K_RIGHT || k == K_PGDN || k == K_ENTER || k == ' ') p = (p + 1) % handbook_pages;
        if (k == K_LEFT || k == K_PGUP) p = (p + handbook_pages - 1) % handbook_pages;
        if (k == K_F12) vid_shot("HANDBOOK.BMP");
        snd_sfx(SFX_TICK);
    }
}

/* ---------------- job setup ---------------- */
int grp_used(int g)
{
    switch (g) {
    case G_GEO: return (faultmask & (F_HS | F_VS | F_HC | F_VC | F_TILT | F_PIN | F_TRAP)) != 0;
    case G_PIC: return (faultmask & (F_FOCUS | F_BRIGHT)) != 0;
    case G_MAG: return (faultmask & (F_MAGNET | F_PUR | F_P4 | F_P6)) != 0;
    default: return (faultmask & (F_STRIP1 | F_STRIP2 | F_STRIP3)) != 0;
    }
}
static int rsign(void) { return (rand() & 1) ? 1 : -1; }
static void setup_job(u32 m)
{
    int i, n;
    faultmask = m;
    memset(val, 0, sizeof val); memset(ideal, 0, sizeof ideal);
    for (i = 0; i < 7; i++) if (m & (1ul << i)) ideal[CT_HS + i] = rsign() * (3 + rand() % 7);
    if (m & F_FOCUS) ideal[CT_FOCUS] = rsign() * (6 + rand() % 9);
    if (m & F_BRIGHT) ideal[CT_BRIGHT] = rsign() * (3 + rand() % 5);
    if (m & F_PUR) { ideal[CT_PURA] = rand() % 24; ideal[CT_PURS] = 3 + rand() % 6; }
    if (m & F_P4) { ideal[CT_P4A] = rand() % 24; ideal[CT_P4S] = 3 + rand() % 6; }
    if (m & F_P6) { ideal[CT_P6A] = rand() % 24; ideal[CT_P6S] = 3 + rand() % 6; }
    n = (m & F_STRIP3) ? 3 : (m & F_STRIP2) ? 2 : (m & F_STRIP1) ? 1 : 0;
    while (n > 0) { i = rand() % 8; if (!ideal[CT_ST0 + i]) { ideal[CT_ST0 + i] = rsign() * (1 + rand() % 3); n--; } }
    magnetized = (m & F_MAGNET) != 0;
    blot_u = rsign() * (60 + rand() % 120); blot_v = rsign() * (40 + rand() % 120);
    jitter_x = jitter_y = jitter_k = 0;
}

/* ---------------- the bench ---------------- */
static int sel, grp, face_dirty, panel_dirty, hints, inspects, strip_sel;
static u32 t_start, msg_until, inspect_until, degauss_at;
static char msg[100];
static const char *jobtitle;
static int par;

static void say(const char *s) { strncpy(msg, s, 99); msg[99] = 0; msg_until = ms_now() + 6000; panel_dirty = 1; }

static void bezel(void)
{
    int i;
    /* cabinet */
    fill(8, 30, 456, 354, C_BEIGE0 + 4);
    for (i = 0; i < 3; i++) bevel(8 + i, 30 + i, 456 - 2 * i, 354 - 2 * i, C_BEIGE0 + 6, C_BEIGE0 + 1);
    /* recess round the glass */
    fill(FX - 10, FY - 10, FW + 20, FH + 20, C_BEIGE0 + 2);
    for (i = 0; i < 3; i++) bevel(FX - 10 + i, FY - 10 + i, FW + 20 - 2 * i, FH + 20 - 2 * i, C_BEIGE0 + 0, C_BEIGE0 + 5);
    fill(FX - 2, FY - 2, FW + 4, FH + 4, 0);
    /* controls strip */
    text8(24, 372, "CHARGEMASTER 14", C_BEIGE0 + 1);
    disc(440, 375, 3, C_LEDG); pset(439, 374, C_WHITE);
    for (i = 0; i < 4; i++) { fill(300 + i * 26, 371, 16, 8, C_BEIGE0 + 3); bevel(300 + i * 26, 371, 16, 8, C_BEIGE0 + 6, C_BEIGE0 + 0); }
}
static void corners(void)   /* round the corners of the glass */
{
    int r = 14, x, y;
    for (y = 0; y < r; y++) for (x = 0; x < r; x++) if ((r - x) * (r - x) + (r - y) * (r - y) > r * r) {
        pset(FX + x, FY + y, C_BEIGE0 + 2); pset(FX + FW - 1 - x, FY + y, C_BEIGE0 + 2);
        pset(FX + x, FY + FH - 1 - y, C_BEIGE0 + 2); pset(FX + FW - 1 - x, FY + FH - 1 - y, C_BEIGE0 + 2);
    }
}
static void lamp(int x, int y, int state, const char *lbl)   /* 0 off, 1 green, 2 red */
{
    disc(x, y, 5, C_BLACK); disc(x, y, 4, state == 1 ? C_LEDG : state == 2 ? C_LEDR : GRAY(5));
    if (state) pset(x - 1, y - 2, C_WHITE);
    text8(x - 4, y + 9, lbl, GRAY(16));
}
static int show_lamps(void) { return diff == 0 || (diff == 1 && ms_now() < inspect_until); }

#define PX 470
#define PY 30
#define PW 162
#define PH 354
static void slider(int x, int y, int i)
{
    int w = 34, k = (val[i] - ctl[i].lo) * (w - 6) / (ctl[i].hi - ctl[i].lo);
    fill(x, y + 6, w, 4, C_BLACK); bevel(x, y + 6, w, 4, C_PANELL, C_PANELH);
    fill(x + k, y + 2, 6, 12, C_BEIGE0 + 5); bevel(x + k, y + 2, 6, 12, C_WHITE, C_BEIGE0 + 0);
}
static void neck_diagram(int cx, int cy)
{
    static const int ringc[3][3] = { {255,220,60}, {255,80,255}, {60,230,255} };
    int r, i, x, y;
    disc(cx, cy, 44, GRAY(3)); circle(cx, cy, 44, GRAY(14)); circle(cx, cy, 45, GRAY(8));
    for (r = 0; r < 3; r++) {
        int a = CT_PURA + r * 2, rad = 52 + r * 8, ang = val[a] * 32 / 3 - 64, d = val[a + 1] * 64 / 10;
        int c = rgbc(ringc[r][0], ringc[r][1], ringc[r][2]), on = (sel == a || sel == a + 1);
        circle(cx, cy, rad, on ? c : GRAY(7));
        for (i = -1; i <= 1; i += 2) {
            x = cx + COS(ang + i * d) * rad / 127; y = cy + SIN(ang + i * d) * rad / 127;
            fill(x - 3, y - 3, 7, 7, c); box(x - 3, y - 3, 7, 7, on ? C_WHITE : C_BLACK);
        }
    }
    if (diff < 2) {      /* where the three beams sit; the master works without it */
        int px, py, x4, y4, x6, y6;
        vec_res(CT_PURA, &px, &py); vec_res(CT_P4A, &x4, &y4); vec_res(CT_P6A, &x6, &y6);
        circle(cx - 16, cy, 5, GRAY(9)); circle(cx, cy, 5, GRAY(9)); circle(cx + 16, cy, 5, GRAY(9));
        disc(cx + (px) / 60, cy + (py) / 60, 3, rgbc(0, 255, 0));
        disc(cx + 16 + (px + x4 + x6) / 60, cy + (py + y4 + y6) / 60, 3, rgbc(255, 0, 0));
        disc(cx - 16 + (px - x4 + x6) / 60, cy + (py - y4 + y6) / 60, 3, rgbc(0, 0, 255));
        text8(cx - 60, cy + 82, "BEAMS IN THE NECK", GRAY(13));
    } else text8(cx - 60, cy + 82, "RING TABS", GRAY(13));
}
static void yoke_diagram(int cx, int cy)
{
    static const signed char dx[8] = { 0, 71, 100, 71, 0, -71, -100, -71 }, dy[8] = { -100, -71, 0, 71, 100, 71, 0, -71 };
    int i, j, used = 0;
    disc(cx, cy, 44, rgbc(150, 90, 40)); disc(cx, cy, 30, GRAY(3)); circle(cx, cy, 44, rgbc(255, 170, 90)); circle(cx, cy, 30, C_BLACK);
    for (i = 0; i < 16; i++) line(cx + COS(i * 16) * 31 / 127, cy + SIN(i * 16) * 31 / 127, cx + COS(i * 16) * 43 / 127, cy + SIN(i * 16) * 43 / 127, rgbc(100, 60, 20));
    text8(cx - 16, cy - 4, "YOKE", GRAY(12));
    for (i = 0; i < 8; i++) {
        int x = cx + dx[i] * 62 / 100, y = cy + dy[i] * 62 / 100, v = val[CT_ST0 + i], n = abs(v);
        used += n;
        fill(x - 9, y - 9, 19, 19, C_INK); box(x - 9, y - 9, 19, 19, i == strip_sel ? C_SEL : GRAY(8));
        if (i == strip_sel) box(x - 10, y - 10, 21, 21, C_SEL);
        for (j = 0; j < n; j++) fill(x - 6, y + 4 - j * 5, 13, 3, v > 0 ? C_LEDR : rgbc(60, 120, 255));
        if (!n) pset(x, y, GRAY(8));
    }
    { char t[30]; sprintf(t, "STRIPS USED %d/8", used); text8(cx - 60, cy + 84, t, used >= 8 ? C_LEDR : GRAY(14)); }
    text8(cx - 68, cy + 96, "RED +   BLUE -", GRAY(11));
}
static void draw_panel(void)
{
    int i, n = 0, y; char t[40];
    restore(PX, PY, PW, PH);
    panel(PX, PY, PW, PH);
    fill(PX + 4, PY + 4, PW - 8, 18, C_INK);
    text(PX + 8, PY + 5, grpname[grp], C_AMBER);
    if (grp == G_STRIP) {
        yoke_diagram(PX + PW / 2, PY + 120);
        text8(PX + 8, PY + 232, "UP/DOWN:  position", GRAY(14));
        text8(PX + 8, PY + 244, "LEFT/RIGHT: strips", GRAY(14));
        sprintf(t, "AT %s", ctl[CT_ST0 + strip_sel].name + 6); text(PX + 8, PY + 262, t, C_WHITE);
    } else {
        for (i = 0; i < NCTL; i++) if (ctl[i].grp == grp) {
            y = PY + 30 + n * 22;
            if (i == sel) { fill(PX + 4, y - 2, PW - 8, 20, rgbc(20, 40, 110)); bevel(PX + 4, y - 2, PW - 8, 20, rgbc(90, 130, 255), rgbc(10, 20, 60)); }
            text8(PX + 8, y + 4, ctl[i].name, i == sel ? C_WHITE : GRAY(16));
            slider(PX + 122, y, i);
            if (diff == 0) { fill(PX + 114, y + 5, 5, 5, ctl_ok(i) ? C_LEDG : C_LEDR); box(PX + 113, y + 4, 7, 7, C_BLACK); }
            n++;
        }
        if (grp == G_MAG) neck_diagram(PX + PW / 2, PY + 232);
        if (grp == G_PIC) {
            text8(PX + 8, PY + 100, "D  DEGAUSS", C_WHITE);
            text8(PX + 8, PY + 114, magnetized && diff == 0 ? "   mask is magnetised" : "", C_LEDR);
        }
    }
    /* quality control lamps */
    fill(PX + 4, PY + PH - 44, PW - 8, 40, C_INK);
    for (i = 0; i < NGRP; i++) lamp(PX + 24 + i * 38, PY + PH - 32, show_lamps() ? (grp_ok(i) ? 1 : 2) : 0, i == 0 ? "G" : i == 1 ? "P" : i == 2 ? "M" : "S");
}
static void draw_info(void)
{
    int i, c = grp == G_STRIP ? CT_ST0 + strip_sel : sel; char t[100];
    restore(8, 388, 624, 90);
    panel(8, 388, 624, 90);
    clip(10, 390, 629, 475);
    sprintf(t, "%s  -  %s", ctl[c].name, ctl[c].what); text(16, 392, t, C_AMBER);
    text8(640 - 16 - (int)strlen(patname[pattern]) * 8, 396, patname[pattern], C_SEL);
    for (i = 0; i < 4; i++) text8(16, 411 + i * 11, ctl[c].help[i], C_WHITE);
    if (ms_now() < msg_until) {
        char l1[80]; int n = (int)strlen(msg), cut = n;
        if (n > 75) { cut = 75; while (cut > 40 && msg[cut] != ' ') cut--; }
        memcpy(l1, msg, cut); l1[cut] = 0;
        fill(12, 453, 616, 22, rgbc(90, 20, 0)); bevel(12, 453, 616, 22, rgbc(255, 120, 40), C_BLACK);
        text8(16, 455, l1, C_SEL); if (cut < n) text8(16, 465, msg + cut + 1, C_SEL);
    }
    else text8(16, 464, "TAB group \x18\x19 pick \x1B\x1A turn  F1-F5 pattern  D degauss  H hint  ENTER ship  ESC", GRAY(13));
    unclip();
}
static void draw_header(void)
{
    int s = (int)((ms_now() - t_start) / 1000); char t[90];
    fill(0, 0, 640, 26, C_INK); hline(0, 639, 26, C_PANELH);
    text(8, 5, "SERVICE BENCH", C_AMBER);
    text(128, 5, jobtitle, C_WHITE);
    sprintf(t, "%s   %d:%02d  PAR %d:%02d", diffname[diff], s / 60, s % 60, par / 60, par % 60);
    text(640 - 8 - (int)strlen(t) * 8, 5, t, s > par ? C_LEDR : C_LEDG);
}
static void draw_face(void) { crt_draw(); corners(); }

static void degauss(void)
{
    int f; u32 now = ms_now();
    if (degauss_at && now - degauss_at < 15000) { say("The degauss thermistor is still hot. It needs 15 seconds to cool."); snd_sfx(SFX_BUZZ); return; }
    degauss_at = now; snd_sfx(SFX_DEGAUSS);
    for (f = 0; f < 7; f++) {
        jitter_x = ((f & 1) ? 1 : -1) * (7 - f) * 40; jitter_y = ((f & 2) ? 1 : -1) * (7 - f) * 24; jitter_k = (7 - f) * 28;
        draw_face(); present();
    }
    jitter_x = jitter_y = jitter_k = 0;
    if (magnetized) { magnetized = 0; say("The cloud is gone: the shadow mask was magnetised. Degauss comes first, always."); }
    else say("Degaussed. Nothing changed, so the mask was not magnetised.");
    face_dirty = 1;
}
static void hint(void)
{
    int i, best = -1, bv = 0;
    hints++;
    if (magnetized) { say("Look at a field pattern (F3, F4): a coloured cloud. Press D to degauss."); return; }
    for (i = 0; i < NCTL; i++) if (!ctl_ok(i)) {
        int v = ctl_err(i) * 100 / (ctl[i].hi - ctl[i].lo + 1) + 1;
        if (i >= CT_ST0) v = 2;                       /* strips last */
        if (i >= CT_PURA && i < CT_ST0) v = 3 + (CT_P6S - i);   /* purity before four-pole before six-pole */
        if (ctl[i].grp == G_GEO) v += 40;
        if (ctl[i].grp == G_PIC) v += 20;
        if (v > bv) { bv = v; best = i; }
    }
    if (best < 0) say("Everything is within tolerance. Press ENTER to ship it.");
    else say(ctl[best].hint);
}
static void adjust(int c, int d)
{
    int v = val[c] + d;
    if (ctl[c].wrap) { int n = ctl[c].hi + 1; v = ((v % n) + n) % n; }
    if (v < ctl[c].lo) v = ctl[c].lo;
    if (v > ctl[c].hi) v = ctl[c].hi;
    if (c >= CT_ST0) {
        int i, used = 0;
        for (i = 0; i < 8; i++) used += abs(i == c - CT_ST0 ? v : val[CT_ST0 + i]);
        if (used > 8) { say("You only have eight strips. Remove one somewhere else first."); snd_sfx(SFX_BUZZ); return; }
    }
    if (v == val[c]) return;
    val[c] = v; face_dirty = 1; panel_dirty = 1;
    snd_sfx(c >= CT_ST0 ? SFX_CLICK : SFX_TICK);
}
static int confirm(const char *q)
{
    int k;
    fill(170, 200, 300, 70, C_INK); bevel(170, 200, 300, 70, C_PANELH, C_BLACK); box(172, 202, 296, 66, C_AMBER);
    textc(320, 214, q, C_WHITE); textc(320, 240, "Y = yes     N = no", C_SEL);
    present();
    for (;;) { k = key_wait(); if (k == 'y' || k == 'Y' || k == K_ENTER) return 1; if (k == 'n' || k == 'N' || k == K_ESC) return 0; }
}
static int first_of(int g) { int i; for (i = 0; i < NCTL; i++) if (ctl[i].grp == g) return i; return 0; }

/* Report card. Returns stars earned (0 = not passed). */
static int report(int secs)
{
    static const char *fail[NGRP] = {
        "\"The picture still does not fit the screen properly.\"",
        "\"It is still soft, or the blacks are wrong.\"",
        "\"The colours are still wrong in the middle of the screen.\"",
        "\"There are still colour fringes at the edges.\"" };
    int g, i, ok = 0, tot = 0, pass = 1, score, st = 0, y = 130; char t[90];
    for (i = 0; i < NCTL; i++) if (grp_used(ctl[i].grp)) { tot++; if (ctl_ok(i)) ok++; }
    for (g = 0; g < NGRP; g++) if (!grp_ok(g)) pass = 0;
    score = tot ? 1000 * ok / tot : 1000;
    if (secs > par) score -= (secs - par) * 2;
    score -= hints * 50 + inspects * 15;
    if (pass && secs <= par) score += 250;
    score = score * (2 + diff) / 2;
    if (score < 0) score = 0;
    if (pass) st = 1 + (secs <= par) + (secs <= par && hints == 0);
    workshop();
    fill(54, 44, 540, 400, 0); fill(50, 40, 540, 400, C_BEIGE0 + 6); bevel(50, 40, 540, 400, C_WHITE, C_BEIGE0 + 2);
    textbig(248, 56, "JOB SHEET", C_INK, 2); hline(70, 570, 96, C_INK);
    text(70, 104, jobtitle, C_INK);
    for (g = 0; g < NGRP; g++) {
        int good = grp_ok(g);
        sprintf(t, "%-20s %s", grpname[g], good ? "PASS" : "FAIL");
        text(70, y, t, good ? rgbc(0, 110, 0) : rgbc(190, 0, 0)); y += 18;
        if (!good) { text8(78, y, fail[g], C_INK); y += 14; }
    }
    y += 8;
    sprintf(t, "Time %d:%02d   par %d:%02d", secs / 60, secs % 60, par / 60, par % 60); text(70, y, t, C_INK); y += 18;
    sprintf(t, "Hints %d   Inspections %d   Level %s", hints, inspects, diffname[diff]); text(70, y, t, C_INK); y += 18;
    sprintf(t, "SCORE %d", score); textbig(70, y + 6, t, C_INK, 2);
    for (i = 0; i < 3; i++) star(330 + i * 34, y + 22, i < st);
    /* rubber stamp */
    { int sx = 360, sy = 360, c = pass ? rgbc(0, 130, 40) : rgbc(200, 20, 20); const char *w = pass ? "PASSED" : "RETURNED";
      box(sx - 8, sy - 8, (int)strlen(w) * 24 + 16, 64, c); box(sx - 6, sy - 6, (int)strlen(w) * 24 + 12, 60, c); textbig(sx, sy, w, c, 3); }
    textc(320, 448, "ENTER to continue", C_WHITE);
    pal_apply(64, 7); present();
    snd_sfx(SFX_THUNK); snd_jingle(pass ? JING_WIN : JING_FAIL);
    for (;;) { int k = key_wait(); if (k == K_ENTER || k == K_ESC || k == ' ') break; if (k == K_F12) vid_shot("REPORT.BMP"); }
    return st;
}
/* Run one job. Returns stars, or -1 if abandoned. */
static int bench(const char *title, u32 mask, int partime, int startpat)
{
    int k, lastsec = -1; u32 lastmsg = 0;
    setup_job(mask);
    jobtitle = title; par = partime; pattern = startpat;
    grp = 0; while (grp < NGRP - 1 && !grp_used(grp)) grp++;
    sel = first_of(grp); strip_sel = 0; hints = inspects = 0; msg_until = 0; inspect_until = 0; degauss_at = 0;
    workshop(); bezel(); cache_bg();
    snd_song(SONG_BENCH);
    t_start = ms_now(); face_dirty = panel_dirty = 1;
    for (;;) {
        u32 now = ms_now(); int s = (int)((now - t_start) / 1000);
        if ((now < msg_until) != (lastmsg != 0)) { lastmsg = now < msg_until; panel_dirty = 1; }
        if (diff == 1 && inspect_until && now > inspect_until) { inspect_until = 0; panel_dirty = 1; }
        if (face_dirty && !key_more()) { draw_face(); face_dirty = 0; panel_dirty = 1; }
        if (panel_dirty) { draw_panel(); draw_info(); panel_dirty = 0; lastsec = -1; }
        if (s != lastsec) { draw_header(); lastsec = s; present(); }
        k = key_get();
        if (!k) { vid_vsync(); continue; }
        if (k == K_ESC) { if (confirm("Give up this job?")) return -1; face_dirty = 1; restore(0, 27, 640, 453); bezel(); }
        else if (k == K_TAB) { grp = (grp + 1) % NGRP; sel = first_of(grp); panel_dirty = 1; snd_sfx(SFX_CLICK); }
        else if (k == K_UP || k == K_DOWN) {
            int d = k == K_UP ? -1 : 1;
            if (grp == G_STRIP) strip_sel = (strip_sel + 8 + d) % 8;
            else { int i = sel; do { i = (i + NCTL + d) % NCTL; } while (ctl[i].grp != grp); sel = i; }
            panel_dirty = 1; snd_sfx(SFX_TICK);
        }
        else if (k == K_LEFT || k == K_RIGHT || k == K_PGUP || k == K_PGDN) {
            int d = k == K_LEFT ? -1 : k == K_RIGHT ? 1 : k == K_PGDN ? -4 : 4;
            adjust(grp == G_STRIP ? CT_ST0 + strip_sel : sel, d);
        }
        else if (k >= K_F1 && k <= K_F5 && ((k - K_F1) >> 8) < NPAT) { pattern = (k - K_F1) >> 8; face_dirty = 1; say(pathelp[pattern]); snd_sfx(SFX_CLICK); }
        else if (k == '`' && auto_mode) { memcpy(val, ideal, sizeof val); magnetized = 0; face_dirty = 1; }   /* test hook */
        else if (k == 'd' || k == 'D') degauss();
        else if (k == 'h' || k == 'H') hint();
        else if ((k == 'i' || k == 'I') && diff == 1) { inspects++; inspect_until = now + 4000; panel_dirty = 1; snd_sfx(SFX_OK); }
        else if (k == 'm' || k == 'M') { snd_music_on ^= 1; snd_song(SONG_NONE); snd_song(SONG_BENCH); say(snd_music_on ? "Music on." : "Music off."); }
        else if (k == K_F12) { vid_shot("BENCH.BMP"); say("Saved BENCH.BMP."); }
        else if (k == K_ENTER) {
            if (confirm("Ship it to the customer?")) return report(s);
            face_dirty = 1; restore(0, 27, 640, 453); bezel();
        }
    }
}
static void ticket(int j)
{
    int i; char t[60];
    workshop();
    fill(34, 90, 572, 270, 0); fill(30, 86, 572, 270, C_BEIGE0 + 6); bevel(30, 86, 572, 270, C_WHITE, C_BEIGE0 + 2);
    sprintf(t, "REPAIR TICKET %03d", 101 + j); text(50, 100, t, rgbc(190, 0, 0)); hline(50, 580, 120, C_INK);
    textbig(50, 130, jobs[j].name, C_INK, 2);
    text(50, 176, "The customer says:", C_BEIGE0 + 0);
    text8(50, 198, jobs[j].ticket[0], C_INK);
    text(50, 222, "Workshop notes:", C_BEIGE0 + 0);
    for (i = 1; i < 3; i++) text8(50, 244 + (i - 1) * 14, jobs[j].ticket[i], C_INK);
    sprintf(t, "Par time %d:%02d    Level: %s", jobs[j].par / 60, jobs[j].par % 60, diffname[diff]); text(50, 290, t, C_INK);
    text(50, 326, "ENTER  start work        ESC  back", rgbc(190, 0, 0));
    pal_apply(64, 7); present();
}
static void career(void)
{
    int sel_j = 0, k, i, st; char t[80];
    for (i = 0; i < NJOB; i++) if (stars[i]) sel_j = i + 1 < NJOB ? i + 1 : i;
    for (;;) {
        snd_song(SONG_TITLE);
        workshop(); panel(60, 30, 520, 420);
        textbig(180, 42, "REPAIR QUEUE", C_AMBER, 2);
        for (i = 0; i < NJOB; i++) {
            int y = 92 + i * 30, open = i == 0 || stars[i - 1];
            if (i == sel_j) { fill(76, y - 4, 488, 26, rgbc(20, 40, 110)); bevel(76, y - 4, 488, 26, rgbc(90, 130, 255), rgbc(10, 20, 60)); }
            sprintf(t, "%2d  %s", i + 1, open ? jobs[i].name : "(locked)"); text(88, y, t, open ? C_WHITE : GRAY(9));
            for (st = 0; st < 3; st++) star(470 + st * 28, y + 8, st < stars[i]);
        }
        text8(76, 420, "\x18\x19 choose   ENTER open ticket   ESC back", C_SEL);
        sprintf(t, "Level: %s (change it in Setup)", diffname[diff]); text8(76, 434, t, GRAY(13));
        present();
        k = key_wait();
        if (k == K_ESC) return;
        if (k == K_UP) { sel_j = (sel_j + NJOB - 1) % NJOB; snd_sfx(SFX_TICK); }
        if (k == K_DOWN) { sel_j = (sel_j + 1) % NJOB; snd_sfx(SFX_TICK); }
        if (k == K_F12) vid_shot("QUEUE.BMP");
        if (k == K_ENTER) {
            if (sel_j && !stars[sel_j - 1] && !auto_mode) { snd_sfx(SFX_BUZZ); continue; }
            ticket(sel_j);
            for (;;) { k = key_wait(); if (k == K_ENTER || k == K_ESC) break; if (k == K_F12) vid_shot("TICKET.BMP"); }
            if (k == K_ESC) continue;
            sprintf(t, "%d. %s", sel_j + 1, jobs[sel_j].name);
            st = bench(t, jobs[sel_j].faults, jobs[sel_j].par, jobs[sel_j].pat);
            if (st > stars[sel_j]) { stars[sel_j] = (u8)st; save(); }
            if (st > 0 && sel_j + 1 < NJOB) sel_j++;
        }
    }
}
static void freebench(void)
{
    static const u32 pool[13] = { F_HS, F_VS, F_HC, F_VC, F_TILT, F_PIN, F_TRAP, F_FOCUS, F_BRIGHT, F_MAGNET, F_PUR, F_P4, F_P6 };
    u32 m = 0; int n = 3 + diff * 2 + rand() % 3, i;
    for (i = 0; i < n; i++) m |= pool[rand() % 13];
    if (rand() % 3 == 0 || diff == 2) m |= F_STRIP2;
    bench("FREE BENCH: UNKNOWN SET", m, 90 + n * 45, PAT_HATCH);
}
static void setup(void)
{
    const char *it[4]; char a[40], b[40], c[40]; int s = 0;
    for (;;) {
        sprintf(a, "LEVEL: %s", diffname[diff]); sprintf(b, "MUSIC: %s", snd_music_on ? "ON" : "OFF"); sprintf(c, "SOUND EFFECTS: %s", snd_sfx_on ? "ON" : "OFF");
        it[0] = a; it[1] = b; it[2] = c; it[3] = "BACK";
        s = menu("SETUP", it, 4, s, diff == 0 ? "Apprentice: every control shows a pass lamp." : diff == 1 ? "Journeyman: press I to inspect. Each look costs points." : "Master: no lamps, no beam display. Trust your eyes.");
        if (s < 0 || s == 3) break;
        if (s == 0) diff = (diff + 1) % 3;
        if (s == 1) { snd_music_on ^= 1; snd_song(SONG_NONE); snd_song(SONG_TITLE); }
        if (s == 2) snd_sfx_on ^= 1;
    }
    save();
}
void game_main(void)
{
    static const char *items[6] = { "CAREER: THE REPAIR QUEUE", "FREE BENCH", "TEST PATTERNS FOR YOUR MONITOR", "HANDBOOK", "SETUP", "EXIT TO DOS" };
    int s = 0, f; char st[120];
    load();
    srand((unsigned)ms_now());
    for (;;) {
        snd_song(SONG_TITLE);
        pal_apply(0, 7); menu_bg(); logo(0); status_line(st);
        text8(320 - (int)strlen(st) * 4, 448, st, GRAY(11));
        cache_bg(); vid_present(); pal_fade(0, 64);
        if (!auto_mode) for (f = 24; f >= 0; f--) { logo(f * 2); present(); if (key_get()) { logo(0); break; } }
        logo(0); cache_bg();
        s = menu(0, items, 6, s, "A learning tool and game about CRT alignment.  F12 saves a screenshot.");
        if (s < 0 || s == 5) break;
        if (s == 0) career();
        else if (s == 1) freebench();
        else if (s == 2) testpatterns();
        else if (s == 3) handbook_view();
        else if (s == 4) { menu_bg(); logo(0); setup(); }
    }
    pal_fade(64, 0);
}

/* /BENCH: milliseconds for one redraw of each part of the service bench */
void game_bench(u32 *t)
{
    int p, i; u32 t0;
    setup_job(F_HS | F_P4 | F_PUR | F_MAGNET | F_FOCUS); jobtitle = "BENCH"; par = 60; grp = 0; sel = 0;
    workshop(); bezel(); cache_bg(); pal_apply(64, 7);
    for (p = 0; p < NPAT; p++) { pattern = p; t0 = ms_now(); for (i = 0; i < 4; i++) draw_face(); t[p] = (ms_now() - t0) / 4; }
    t0 = ms_now(); for (i = 0; i < 4; i++) { draw_panel(); draw_info(); } t[5] = (ms_now() - t0) / 4;
    t0 = ms_now(); for (i = 0; i < 4; i++) { vid_invalidate(); vid_present(); } t[6] = (ms_now() - t0) / 4;
    t0 = ms_now(); for (i = 0; i < 4; i++) vid_present(); t[7] = (ms_now() - t0) / 4;
    val[CT_FOCUS] = ideal[CT_FOCUS]; pattern = PAT_HATCH; t0 = ms_now(); for (i = 0; i < 4; i++) draw_face(); t[8] = (ms_now() - t0) / 4;
}
