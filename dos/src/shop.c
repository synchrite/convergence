/* The night shop between jobs, and the mentor's service-manual pages. */
#include "g.h"

/* ---------------- the shop at night ---------------- */
static void set_glow(int cx, int cy, int r)          /* light from the set on the bench, falling off with distance */
{
    int x, y;
    for (y = cy - r; y <= cy + r; y++) for (x = cx - r; x <= cx + r; x += 1) {
        int d2 = (x - cx) * (x - cx) + (y - cy) * (y - cy), k;
        if (d2 > r * r || y < 0 || y >= SH || x < 0 || x >= SW) continue;
        k = 70 - d2 * 70 / (r * r);
        if (k > 4) addrgb(x, y, k / 3, k / 2, k);
    }
}
static void shop_bg(void)
{
    int i, x, y;
    dfill(0, 0, SW, 330, 14, 16, 30, 6, 7, 14);                          /* wall */
    dfill(0, 330, SW, 150, 40, 26, 16, 14, 9, 6);                        /* floor */
    fill(40, 40, 200, 150, rgbc(4, 8, 22)); box(38, 38, 204, 154, rgbc(60, 50, 40)); box(39, 39, 202, 152, rgbc(90, 75, 55));
    vline(140, 40, 189, rgbc(90, 75, 55)); hline(40, 239, 115, rgbc(90, 75, 55));     /* window frame */
    for (i = 0; i < 6; i++) { int bx = 50 + i * 30, bh = 20 + (i * 37) % 50; fill(bx, 190 - bh, 22, bh, rgbc(10, 12, 26));
        for (y = 190 - bh + 4; y < 186; y += 8) for (x = bx + 3; x < bx + 19; x += 6) if (((x * 7 + y * 3) % 5) == 0) fill(x, y, 3, 3, rgbc(170, 140, 60)); }
    textbig(54, 60, "TV", rgbc(255, 60, 90), 2); text8(92, 72, "REPAIR", rgbc(255, 60, 90));   /* neon, seen backwards from inside: drawn plain */
    for (i = 0; i < 4; i++) { fill(300, 60 + i * 60, 300, 6, rgbc(70, 50, 30)); }            /* shelves */
    for (i = 0; i < 9; i++) { int sx = 310 + (i % 3) * 98, sy = 22 + (i / 3) * 60 + 6; fill(sx, sy, 60, 32, rgbc(60 + (i * 17) % 40, 50, 40)); fill(sx + 6, sy + 5, 36, 22, rgbc(20, 24, 26)); fill(sx + 46, sy + 8, 8, 4, GRAY(8)); }
    fill(0, 300, SW, 30, rgbc(80, 55, 32)); hline(0, SW - 1, 300, rgbc(130, 95, 60));        /* bench */
    fill(250, 190, 140, 110, rgbc(70, 60, 50)); fill(260, 198, 120, 88, rgbc(10, 14, 12));    /* the set on the bench */
    fill(266, 204, 108, 76, rgbc(40, 80, 70)); fill(300, 286, 40, 14, rgbc(50, 44, 36));
    for (y = 206; y < 278; y += 3) hline(268, 371, y, rgbc(70, 130, 110));                     /* raster lines */
    fill(420, 262, 70, 38, GRAY(5)); fill(426, 268, 40, 24, rgbc(20, 60, 30)); line(428, 280, 464, 276, rgbc(80, 255, 120));   /* scope */
    disc(540, 286, 14, rgbc(150, 110, 60)); circle(540, 286, 14, rgbc(90, 60, 30));            /* degauss coil */
    fill(110, 270, 60, 30, rgbc(140, 30, 30)); text8(116, 282, "MULTI", C_WHITE);               /* meter */
    set_glow(320, 240, 190);
}
static void rain(int f)
{
    int i;
    clip(40, 40, 239, 189);
    for (i = 0; i < 40; i++) { int x = 40 + (i * 53 + f * 3) % 200, y = 40 + (i * 97 + f * 11) % 150; line(x, y, x - 2, y + 7, rgbc(90, 110, 160)); }
    unclip();
}
static u8 *shopbuf;
/* Returns 1 to start the job, 0 to go back. */
int shop_scene(int week, const char *name, const char *cust, const char *note1, const char *note2, int par, const char *level)
{
    int f = 0, k; char t[80]; u32 tl = 0;
    if (!shopbuf) shopbuf = (u8 *)malloc((u32)SW * SH);
    unclip(); shop_bg(); if (shopbuf) memcpy(shopbuf, back, (u32)SW * SH);
    for (;;) {
        if (shopbuf) memcpy(back, shopbuf, (u32)SW * 200);
        rain(f++);
        fill(20, 340, 600, 128, C_INK); box(20, 340, 600, 128, C_AMBER);
        sprintf(t, "WEEK %d, 1982.  %s", week, name); text(32, 346, t, C_AMBER);
        text8(32, 368, cust, C_WHITE);
        text8(32, 386, note1, GRAY(16)); text8(32, 398, note2, GRAY(16));
        sprintf(t, "Par %d:%02d    Level %s", par / 60, par % 60, level); text8(32, 420, t, GRAY(13));
        text8(32, 446, "ENTER  take it to the bench        ESC  back to the queue", C_SEL);
        if (ms_now() - tl > 60 || !tl) { tl = ms_now(); vid_vsync(); vid_present(); }
        k = key_get();
        if (k == K_ENTER) return 1;
        if (k == K_ESC) return 0;
        if (k == K_F12) vid_shot("SHOP.BMP");
        if (auto_mode && !k) continue;
        if (!k) vid_vsync();
    }
}

/* ---------------- the mentor's service-manual page ---------------- */
typedef struct { const char *title, *step[6], *fig; } Page;
static const Page pages[5] = {
 { "CHAPTER 1   GEOMETRY",
   { "1. Show the crosshatch. Every line should be straight and evenly spaced.", "2. CENTRE first: equal margins left and right, top and bottom.",
     "3. Then SIZE: the grid just inside the bezel, the circle round.", "4. TILT: the yoke has turned on the neck. Level the lines to the bezel.",
     "5. PINCUSHION and TRAPEZOID: sides straight, then parallel.", "Set size before centre and you set it against the wrong edge." }, "GEO" },
 { "CHAPTER 2   PICTURE",
   { "1. Focus at normal brightness, on fine lines in the corners and centre.", "   A tube focuses differently when it is driven hard.",
     "2. Black level on the PLUGE pattern (F5):", "   the left bar must vanish into the black, the next must just show.",
     "3. Too high and blacks turn grey; too low and shadow detail is lost.", "Contrast comes after black level, never before." }, "PLUGE" },
 { "CHAPTER 3   PURITY",
   { "1. DEGAUSS first. A magnetised mask fakes every other fault.", "   The coil's field must die away slowly, or it leaves new magnetism.",
     "2. Show the red field. Red must be the same red everywhere.", "3. Purity rings move all three beams together. Spread, then turn.",
     "4. Tint at the centre: rings. Tint only at the edges: slide the yoke.", "Never judge purity on a cold tube: give it twenty minutes." }, "PUR" },
 { "CHAPTER 4   STATIC CONVERGENCE",
   { "1. Work at the CENTRE only. The rings move the whole picture.", "2. Green is in the middle of the three guns and does not move.",
     "3. Green off. 4-pole rings move red and blue in OPPOSITE directions:", "   bring them together into one magenta line.",
     "4. Green on. 6-pole rings move red and blue TOGETHER: lay magenta on green.", "The rings interact. Go round 4-pole, 6-pole, purity again." }, "RINGS" },
 { "CHAPTER 5   DYNAMIC CONVERGENCE",
   { "1. Only when the centre is white. Edges come after the centre, always.", "2. Opposite edges split opposite ways: tilt the yoke.",
     "3. One corner left: a magnetic strip on the glass under the yoke,", "   at the place nearest the bad corner. Try one face, then the other.",
     "4. Use as few strips as you can. Each one disturbs its neighbours.", "Wedge the yoke and seal the rings with paint when it is right." }, "STRIP" } };
static void fig(const char *k, int x, int y)
{
    int c = rgbc(200, 230, 255), i;
    if (!strcmp(k, "GEO")) { box(x, y, 150, 110, c); for (i = 1; i < 6; i++) vline(x + i * 25, y, y + 109, c); for (i = 1; i < 4; i++) hline(x, x + 149, y + i * 27, c); circle(x + 75, y + 55, 40, c); text8(x, y + 116, "Fig. 1  crosshatch", c); }
    else if (!strcmp(k, "PLUGE")) { fill(x, y, 150, 110, GRAY(1)); fill(x + 10, y + 10, 18, 90, GRAY(0)); fill(x + 36, y + 10, 18, 90, GRAY(2)); for (i = 0; i < 4; i++) fill(x + 70 + i * 20, y + 10, 18, 90, GRAY(4 + i * 6)); box(x, y, 150, 110, c); text8(x, y + 116, "Fig. 2  PLUGE", c); }
    else if (!strcmp(k, "PUR")) { box(x, y, 150, 110, c); fill(x + 1, y + 1, 148, 108, rgbc(170, 30, 30)); disc(x + 110, y + 30, 22, rgbc(170, 40, 120)); text8(x, y + 116, "Fig. 3  red field, tinted", c); }
    else if (!strcmp(k, "RINGS")) { for (i = 0; i < 3; i++) circle(x + 75, y + 55, 20 + i * 14, c); disc(x + 63, y + 55, 4, rgbc(80, 140, 255)); disc(x + 75, y + 55, 4, rgbc(80, 255, 120)); disc(x + 87, y + 55, 4, rgbc(255, 80, 80)); text8(x, y + 116, "Fig. 4  rings on the neck", c); }
    else { circle(x + 75, y + 55, 48, c); circle(x + 75, y + 55, 34, c); for (i = 0; i < 8; i++) { int a = i * 32; box(x + 75 + COS(a) * 58 / 127 - 4, y + 55 + SIN(a) * 58 / 127 - 4, 8, 8, c); } text8(x, y + 116, "Fig. 5  strip places", c); }
}
void manual_page(int ch)
{
    const Page *p = &pages[ch]; int i, x, y, k;
    unclip(); fill(0, 0, SW, SH, rgbc(14, 34, 80));
    for (x = 0; x < SW; x += 16) vline(x, 0, SH - 1, rgbc(24, 50, 104));
    for (y = 0; y < SH; y += 16) hline(0, SW - 1, y, rgbc(24, 50, 104));
    box(16, 16, SW - 32, SH - 32, rgbc(200, 230, 255)); box(19, 19, SW - 38, SH - 38, rgbc(200, 230, 255));
    text8(32, 30, "CHARGEMASTER SERVICE MANUAL  -  NOTES FROM THE WORKSHOP", rgbc(200, 230, 255));
    textbig(32, 50, p->title, C_WHITE, 2);
    hline(32, SW - 33, 88, rgbc(200, 230, 255));
    for (i = 0; i < 6; i++) text8(32, 104 + i * 22, p->step[i], i == 5 ? C_SEL : C_WHITE);
    fig(p->fig, 450, 300);
    text8(32, 244, "Ed, the shop owner, wrote these in the margin in pencil.", rgbc(200, 230, 255));
    text8(32, 256, "Read them once. The sets will teach you the rest.", rgbc(200, 230, 255));
    text8(32, 440, "ENTER  to the bench", C_SEL);
    pal_apply(64, 7); vid_vsync(); vid_present();
    for (;;) { k = key_wait(); if (k == K_ENTER || k == K_ESC || k == ' ') return; if (k == K_F12) vid_shot("MANUAL.BMP"); }
}
