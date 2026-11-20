/* Charge! Convergence, DOS edition. Entry point and command line. */
#include <ctype.h>
#include "cc.h"

static void upper(char *s) { for (; *s; s++) *s = (char)toupper((unsigned char)*s); }

int main(int argc, char **argv)
{
    int i, e, nosound = 0;
    for (i = 1; i < argc; i++) {
        char a[40]; strncpy(a, argv[i], 39); a[39] = 0; upper(a);
        if (!strcmp(a, "/BANK")) vid_force_bank = 1;
        else if (!strcmp(a, "/NOSOUND")) nosound = 1;
        else if (!strcmp(a, "/NOSB")) snd_no_sb = 1;
        else if (!strcmp(a, "/NOADLIB")) snd_no_adlib = 1;
        else if (!strncmp(a, "/AUTO:", 6)) auto_load(argv[i] + 6);
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
    game_main();
    vid_done(); snd_done(); sys_done();
    printf("CHARGE! CONVERGENCE. After CHARGE! by C. Anthony Ray, BYTE, December 1982.\n");
    return 0;
}
