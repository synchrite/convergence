/* Timing, keyboard, and a scripted mode used for automated testing. */
#include <i86.h>
#include <conio.h>
#include <ctype.h>
#include "cc.h"

int auto_mode;
static char *script, *sp;
static int auto_wait;

void sys_init(void)
{
    /* Timer channel 0 in mode 2 at the normal 18.2 Hz rate, so the counter reads linearly. */
    _disable(); outp(0x43, 0x34); outp(0x40, 0); outp(0x40, 0); _enable();
}
void sys_done(void)
{
    _disable(); outp(0x43, 0x36); outp(0x40, 0); outp(0x40, 0); _enable();
}
u32 ms_now(void)
{
    u32 t1, t2; unsigned c;
    do {
        t1 = *(volatile u32 *)0x46C;
        outp(0x43, 0); c = inp(0x40); c |= (unsigned)inp(0x40) << 8;
        t2 = *(volatile u32 *)0x46C;
    } while (t1 != t2);
    return t1 * 55 + (65535u - (c & 0xFFFF)) / 1193;
}
void sys_idle(void) { snd_poll(ms_now()); }

void auto_load(const char *file)
{
    FILE *f = fopen(file, "rb"); long n;
    if (!f) return;
    fseek(f, 0, SEEK_END); n = ftell(f); fseek(f, 0, SEEK_SET);
    script = (char *)malloc(n + 1); fread(script, 1, n, f); script[n] = 0; fclose(f);
    sp = script; auto_mode = 1;
}
static int auto_key(void)
{
    static const struct { const char *n; int k; } names[] = {
        {"UP", K_UP}, {"DOWN", K_DOWN}, {"LEFT", K_LEFT}, {"RIGHT", K_RIGHT}, {"ENTER", K_ENTER}, {"ESC", K_ESC},
        {"TAB", K_TAB}, {"PGUP", K_PGUP}, {"PGDN", K_PGDN}, {"F1", K_F1}, {"F2", K_F2}, {"F3", K_F3}, {"F4", K_F4},
        {"F5", K_F5}, {"F6", K_F6}, {"SPACE", ' '}, {0, 0} };
    char tok[40]; int i;
    if (auto_wait > 0) { auto_wait--; return 0; }
    for (;;) {
        while (*sp && isspace((u8)*sp)) sp++;
        if (!*sp) { vid_done(); snd_done(); sys_done(); exit(0); }
        for (i = 0; *sp && !isspace((u8)*sp) && i < 39; i++) tok[i] = *sp++;
        tok[i] = 0;
        if (tok[0] == 's' && tok[1] == ':') { vid_shot(tok + 2); continue; }
        if (tok[0] == 'w' && tok[1] == ':') { auto_wait = atoi(tok + 2); return 0; }
        auto_wait = 2;
        for (i = 0; names[i].n; i++) if (!strcmp(tok, names[i].n)) return names[i].k;
        return tok[0];
    }
}
int key_get(void)
{
    union REGS r;
    sys_idle();
    if (auto_mode) return auto_key();
    if (*(volatile u16 *)0x41A == *(volatile u16 *)0x41C) return 0;   /* BIOS key buffer empty */
    r.h.ah = 0x10; int386(0x16, &r, &r);
    if (r.h.al == 0 || r.h.al == 0xE0) return r.h.ah << 8;
    return r.h.al;
}
int key_wait(void)
{
    int k;
    for (;;) { k = key_get(); if (k) return k; vid_vsync(); }
}
