/* Charge! Convergence, DOS edition. Entry point and command line. */
#include <ctype.h>
#include "cc.h"

static void upper(char *s) { for (; *s; s++) *s = (char)toupper((unsigned char)*s); }

int main(int argc, char **argv)
{
    int i, e, nosound = 0, bench = 0;
    for (i = 1; i < argc; i++) {
        char a[40]; strncpy(a, argv[i], 39); a[39] = 0; upper(a);
        if (!strcmp(a, "/BANK")) vid_force_bank = 1;
        else if (!strcmp(a, "/NOSOUND")) nosound = 1;
        else if (!strcmp(a, "/NOSB")) snd_no_sb = 1;
        else if (!strcmp(a, "/NOADLIB")) snd_no_adlib = 1;
        else if (!strncmp(a, "/AUTO:", 6)) auto_load(argv[i] + 6);
        else if (!strcmp(a, "/BENCH")) bench = 1;
        else {
            printf("CHARGE! CONVERGENCE, DOS edition\n\n"
                   "CONVERGE [/BANK] [/NOSOUND] [/NOSB] [/NOADLIB]\n\n"
                   "  /BANK     use VESA bank switching even if a linear frame buffer exists\n"
                   "  /NOSOUND  no sound\n  /NOSB     do not use the Sound Blaster\n  /NOADLIB  do not use AdLib FM\n\n"
                   "Needs a 486 or better, a VESA SVGA card (640x480, 256 colours) and 4 MB of RAM.\n");
            return 1;
        }
    }
    if (nosound) snd_no_sb = snd_no_adlib = 1;
    sys_init();
    snd_init();
    if (nosound) snd_music_on = snd_sfx_on = 0;
    e = vid_init(640, 480);
    if (e) {
        snd_done(); sys_done();
        printf("CHARGE! CONVERGENCE cannot start.\n\n%s\n", vid_error(e));
        return 2;
    }
    if (bench) {     /* time the drawing code: CONVERGE /BENCH > OUT.TXT */
        extern void game_bench(u32 *t);
        extern void slowmo_bench(u32 *t);
        u32 t[9], u[8]; game_bench(t); slowmo_bench(u);
        vid_done(); snd_done(); sys_done();
        printf("ms per frame: hatch %lu  dots %lu  red %lu  white %lu  pluge %lu  panel %lu  present(full) %lu  present(same) %lu  hatch in focus %lu\n", t[0], t[1], t[2], t[3], t[4], t[5], t[6], t[7], t[8]);
        printf("slow motion, ms per frame: gun %lu  flight %lu  mask %lu  phosphor %lu  atoms %lu  Monte Carlo for one shot %lu\nMonte Carlo check, 600 electrons: %lu%% come back out, %lu eV left in the phosphor on average\n", u[0], u[1], u[2], u[3], u[4], u[5], u[6], u[7]);
        return 0;
    }
    game_main();
    vid_done(); snd_done(); sys_done();
    printf("CHARGE! CONVERGENCE. After CHARGE! by C. Anthony Ray, BYTE, December 1982.\n");
    return 0;
}
