/* Host check of the slow-motion physics: cc -O1 -Idos/src tools/sm_hosttest.c dos/src/smphys.c dos/src/sm_tab.c dos/src/sintab.c
   Compare with the figures the floating-point prototype prints (archive/slowmo-prototype). */
#include <stdio.h>
#include "sm.h"
static SmPt pts[SM_MAXPT], sp[SM_MAXSP];
int main(void)
{
    int g, m, i;
    for (g = 0; g < 3; g++) printf("beam %d at the mask: 0 mT %.3f mm, 5 mT %.2f mm, -3 mT %.2f mm\n", g, sm_trace(g, 0, SM_XM100) / 256.0, sm_trace(g, 500, SM_XM100) / 256.0, sm_trace(g, -300, SM_XM100) / 256.0);
    for (m = 1; m <= 2; m++) {
        double al = 0, ph = 0, bk = 0, tt = 0; int nb = 0, deep = 0, N = 2000, np = 0, ns = 0;
        for (i = 0; i < N; i++) { SmTrack o; int k; sm_track(m, 0, pts, sp, &o); al += o.e_al; ph += o.e_ph; bk += o.e_back; if (o.e_back) nb++; tt += o.tend / 16.0; np += o.n; ns += o.nsp;
            for (k = 0; k < o.n; k++) if (pts[k].x > deep) deep = pts[k].x; }
        printf("material %d: Al %.2f keV, phosphor %.1f, back %.1f, backscattered %.0f%%, deepest %.1f um, mean time %.0f fs, points %d, sparks %d\n", m, al / N / 1000, ph / N / 1000, bk / N / 1000, 100.0 * nb / N, deep / 1000.0, tt / N, np / N, ns / N);
    }
    return 0;
}
