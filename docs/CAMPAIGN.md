# Campaign and look: plan (2026-11-26)

Owner's choice: all three looks and all three campaign shapes, combined. The
game is a DOS program for a 486 (640x480, 256 colours, VESA).

## The three looks, each with its own job

| Look | Where it is used |
|---|---|
| Night shop diorama | Between jobs: the shop at night, rain on the window, the set on the bench the only light. The week, the customer and what they say. |
| Service manual | Every new technique arrives as a mentor's page in the style of a 1980s service manual: blueprint ground, linework diagram, numbered steps. |
| Workbench close-up | The job itself: the set from behind, tools picked up, a zoom on the phosphor that shows real dot or stripe structure. |

## The three campaign shapes, combined

1. **A year in the shop**, from 1982: the spine. Weeks run in order, faults
   arrive in the order a trainee meets them.
2. **Chapters** inside the year: each chapter is one skill, taught by a manual
   page, practised on several sets, then checked by a mixed exam set.
3. **Open shop** after the last chapter: sets arrive at random, you choose
   which to take and buy tools and parts with what you earn.

| Chapter | Skill | Science and technique it must get right |
|---|---|---|
| 1 Geometry | Centre, size, tilt, pincushion | Centre before size; yoke rotation |
| 2 Picture | Focus, black level | Focus at normal brightness; PLUGE |
| 3 Purity | Degauss, purity rings | Degauss first; judge on a red field; yoke slide for edges |
| 4 Static convergence | 4-pole, 6-pole | Centre only; green fixed; red-blue then magenta-green |
| 5 Dynamic convergence | Yoke tilt, strips | Edges after centre; one strip at the nearest place |
| 6 Fault finding (planned) | Meter and scope | Dry joints, failing capacitors, flyback; safety and discharge first |

## Built so far

- Chapters and weeks on the repair queue; a chapter exam closes each chapter.
- The night-shop scene before each job.
- The mentor's manual page the first time a chapter's skill appears.

## Still to build

Workbench close-up and phosphor zoom; open shop with money, tools and parts;
chapter 6 (meter and scope fault finding). Each must keep the redraw under
about 100 ms on the emulated 486 (`CONVERGE /BENCH`).
