/* Full-screen test patterns for the real monitor attached to this PC. */
#include "g.h"

static int guns = 7, showhelp = 1, cur;

static void frame_rect(int c) { box(0, 0, SW, SH, c); box(1, 1, SW - 2, SH - 2, c); }

static void p_hatch(int dotsonly)
{
    int i, j, nx = 16, ny = 12, x, y;
    fill(0, 0, SW, SH, 0);
    for (i = 0; i <= nx; i++) {
        x = i * (SW - 1) / nx;
        for (j = 0; j <= ny; j++) {
            y = j * (SH - 1) / ny;
            if (dotsonly) { fill(x - (x > 0), y - (y > 0), 2, 2, C_WHITE); }
        }
        if (!dotsonly) vline(x, 0, SH - 1, C_WHITE);
    }
    if (!dotsonly) {
        for (j = 0; j <= ny; j++) hline(0, SW - 1, j * (SH - 1) / ny, C_WHITE);
        circle(SW / 2, SH / 2, SH * 5 / 12, C_WHITE);
        fill(SW / 2 - 3, SH / 2 - 3, 7, 7, C_WHITE);
    }
}
static void p_bars(void)
{
    static const u8 col[8][3] = { {255,255,255},{255,255,0},{0,255,255},{0,255,0},{255,0,255},{255,0,0},{0,0,255},{0,0,0} };
    int i, x, h = SH * 3 / 4;
    for (i = 0; i < 8; i++) fill(i * SW / 8, 0, SW / 8 + 1, h, rgbc(col[i][0], col[i][1], col[i][2]));
    for (x = 0; x < SW; x++) vline(x, h, SH - 1, GRAY(x * 24 / SW));
}
static void p_pluge(void)
{
    int i, bw = SW / 16, y0 = SH / 6, y1 = SH * 5 / 6;
    fill(0, 0, SW, SH, GRAY(1));
    fill(bw * 1, y0, bw, y1 - y0, GRAY(0));     /* blacker than the background */
    fill(bw * 3, y0, bw, y1 - y0, GRAY(2));     /* just above */
    fill(bw * 5, y0, bw, y1 - y0, GRAY(3));
    for (i = 0; i < 8; i++) fill(SW / 2 + i * (SW / 2 - bw) / 8, y0, (SW / 2 - bw) / 8 + 1, y1 - y0, GRAY(1 + i * 22 / 7));
}
static void burst(int x, int y, int w, int h)
{
    int i, j, t = w / 3;
    for (i = 0; i < t; i += 2) vline(x + i, y, y + h - 1, C_WHITE);
    for (j = 0; j < h; j += 2) hline(x + t + 2, x + 2 * t, y + j, C_WHITE);
    for (j = 0; j < h; j++) for (i = (j & 1); i < t - 2; i += 2) pset(x + 2 * t + 4 + i, y + j, C_WHITE);
}
static void p_focus(void)
{
    int y, bw = 150, bh = 48; char row[130]; int n = SW / 8;
    fill(0, 0, SW, SH, 0);
    if (n > 128) n = 128;
    memset(row, 'H', n); row[n] = 0;
    for (y = 0; y < SH; y += 96) { text(0, y + 60, row, C_WHITE); text8(0, y + 80, row, GRAY(14)); }
    burst(4, 4, bw, bh); burst(SW - bw - 4, 4, bw, bh); burst(4, SH - bh - 4, bw, bh); burst(SW - bw - 4, SH - bh - 4, bw, bh);
    burst(SW / 2 - bw / 2, SH / 2 - bh / 2, bw, bh);
}
static void p_geom(void)
{
    int i, r = SH / 8;
    fill(0, 0, SW, SH, 0);
    frame_rect(C_WHITE);
    line(0, 0, SW - 1, SH - 1, GRAY(12)); line(SW - 1, 0, 0, SH - 1, GRAY(12));
    circle(SW / 2, SH / 2, SH / 2 - 3, C_WHITE);
    circle(r + 6, r + 6, r, C_WHITE); circle(SW - r - 7, r + 6, r, C_WHITE);
    circle(r + 6, SH - r - 7, r, C_WHITE); circle(SW - r - 7, SH - r - 7, r, C_WHITE);
    for (i = 1; i < 16; i++) { vline(i * SW / 16, 0, 9, C_WHITE); vline(i * SW / 16, SH - 10, SH - 1, C_WHITE); }
    for (i = 1; i < 12; i++) { hline(0, 9, i * SH / 12, C_WHITE); hline(SW - 10, SW - 1, i * SH / 12, C_WHITE); }
    hline(SW / 2 - 12, SW / 2 + 12, SH / 2, C_WHITE); vline(SW / 2, SH / 2 - 12, SH / 2 + 12, C_WHITE);
}
static void draw(int blink)
{
    int i;
    unclip();
    switch (cur) {
    case 0: p_hatch(0); break;
    case 1: p_hatch(1); break;
    case 2: fill(0, 0, SW, SH, rgbc(255, 0, 0)); break;
    case 3: fill(0, 0, SW, SH, rgbc(0, 255, 0)); break;
    case 4: fill(0, 0, SW, SH, rgbc(0, 0, 255)); break;
    case 5: fill(0, 0, SW, SH, C_WHITE); break;
    case 6: fill(0, 0, SW, SH, 0); break;
    case 7: p_bars(); break;
    case 8: p_pluge(); break;
    case 9: p_focus(); break;
    case 10: p_geom(); break;
    case 11: fill(0, 0, SW, SH, 0); frame_rect(C_WHITE); fill(SW / 5, SH / 5, SW * 3 / 5, SH * 3 / 5, blink ? C_WHITE : 0); break;
    }
    if (showhelp) {
        int w = 616, h = 132, x = (SW - w) / 2, y = SH - h - 12; char t[90];
        fill(x, y, w, h, C_INK); bevel(x, y, w, h, C_PANELH, C_BLACK); box(x + 2, y + 2, w - 4, h - 4, C_PANEL);
        sprintf(t, "%d/%d  %s   %dx%d %s", cur + 1, tp_count, tp_name[cur], SW, SH, vid_lfb ? "linear" : "banked");
        text(x + 10, y + 6, t, C_AMBER);
        for (i = 0; i < 5; i++) text(x + 10, y + 26 + i * 16, tp_help[cur][i], C_WHITE);
        sprintf(t, "\x1B\x1A pattern  R G B guns (%c%c%c)  +/- resolution  F1 hide box  ESC back",
                guns & 1 ? 'R' : '-', guns & 2 ? 'G' : '-', guns & 4 ? 'B' : '-');
        text8(x + 10, y + h - 14, t, C_SEL);
    }
}
void testpatterns(void)
{
    static const int rw[3] = { 640, 800, 1024 }, rh[3] = { 480, 600, 768 };
    int res = 0, dirty = 1, blink = 0, k; u32 tb = ms_now();
    snd_song(SONG_NONE);
    guns = 7; showhelp = 1;
    for (;;) {
        if (cur == 11 && ms_now() - tb > 1000) { tb = ms_now(); blink ^= 1; dirty = 1; }
        if (dirty) { draw(blink); vid_vsync(); vid_present(); dirty = 0; }
        k = key_get();
        if (!k) { vid_vsync(); continue; }
        dirty = 1;
        if (k == K_ESC) break;
        else if (k == K_RIGHT || k == ' ') cur = (cur + 1) % tp_count;
        else if (k == K_LEFT) cur = (cur + tp_count - 1) % tp_count;
        else if (k == K_F1 || k == 'h' || k == 'H') showhelp ^= 1;
        else if (k == 'r' || k == 'R') { guns ^= 1; pal_apply(64, guns); }
        else if (k == 'g' || k == 'G') { guns ^= 2; pal_apply(64, guns); }
        else if (k == 'b' || k == 'B') { guns ^= 4; pal_apply(64, guns); }
        else if (k == K_F12) vid_shot("PATTERN.BMP");
        else if (k == '+' || k == '=' || k == '-') {
            int n = res + (k == '-' ? -1 : 1);
            if (n >= 0 && n < 3 && vid_mode_ok(rw[n], rh[n])) {
                if (vid_init(rw[n], rh[n]) == 0) res = n; else vid_init(rw[res], rh[res]);
                pal_apply(64, guns);
            }
        }
    }
    if (SW != 640) vid_init(640, 480);
    pal_apply(64, 7); vid_invalidate();
}
