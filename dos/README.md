# Charge! Convergence, DOS edition

`CONVERGE.EXE` is a real-mode-launched, 32-bit protected-mode DOS program for a
486-class PC. It is both a game and a tool for learning to calibrate a CRT.

| Needs | Detail |
|---|---|
| CPU | 386 or better; written for a 486. Integer math only, so a 486SX works |
| Memory | 4 MB |
| Video | SVGA card with a VESA BIOS, 640x480 in 256 colours (VBE 1.2 bank switching or VBE 2.0 linear frame buffer) |
| Sound | AdLib (OPL2) music and effects; Sound Blaster digitised effects; PC speaker if neither |
| DOS | MS-DOS 5 or later, or compatible. One file: the CauseWay DOS extender is bound into the EXE |

Set `BLASTER` (for example `SET BLASTER=A220 I5 D1`) for Sound Blaster effects;
the card is not touched without it. Cards with no VESA BIOS need their VESA
driver (or UNIVBE) loaded first.

Switches: `/BANK` (force bank switching), `/NOSOUND`, `/NOSB`, `/NOADLIB`,
`/BENCH` (print how long each screen takes to draw, then exit).

## What is in it

**Career: the repair queue.** Ten customers' monitors. Each job puts only the
controls it needs on the panel, in one list: one control on the first two
jobs, two on most, five steps on the last. The controls are those a technician
has: geometry (size, centre, tilt, pincushion, trapezoid), focus and black
level, degauss, the purity, four-pole and six-pole ring pairs, and magnetic
strips under the yoke for the edges.

Five test patterns on the simulated set (F1 to F5): crosshatch, dots, red
field, white field, PLUGE. Each fault only shows on the pattern a technician
would use to find it. The instructor box explains the selected control: what
it is on a real monitor, and how to set it.

Scoring: accuracy, time against par, hints used. Three levels: Apprentice (a
pass lamp on every control), Journeyman (press I to inspect, at a cost),
Master (no lamps, no beam display). Stars and progress are saved in
`CONVERGE.SAV`.

**Free bench.** A random set of faults.

**Test patterns for your monitor.** Twelve full-screen patterns for the real CRT
attached to the PC, at 640x480, 800x600 or 1024x768 where the card offers
them: crosshatch, dots, red, green, blue, white, black, colour bars, PLUGE and
grey steps, focus, geometry, high-voltage regulation. R, G and B switch each
gun. F1 shows what to look for and which control to use.

**Handbook.** Safety, the order of adjustment, purity and convergence, how to use
the patterns.

## Safety

The only things this program asks you to do to a real monitor are done from
outside the case. The rings, yoke and strips are inside, next to lethal
voltages; the game teaches what they do on a simulated set.

## What has been tested, and what has not

Tested in DOSBox-X 2024.03.01 (S3 SVGA, 486 core):

- Linear frame buffer and `/BANK` bank switching (the bank-switched picture was
  checked in the emulator window, `screens/banked_emulator_window.png`).
- No coprocessor (`fpu=false`), 4 MB, and a slow CPU setting.
- AdLib and Sound Blaster 16: a recording of the emulated card shows the music
  at the right pitches and the 60 Hz degauss sample playing through the DSP.
- A scripted run through the menus, a job, the report, the patterns and the handbook.

Not tested: a real 486, real SVGA cards, real sound cards. Things that most often
differ on real hardware: VESA BIOS quirks (try `/BANK`), a Sound Blaster on
DMA 3 or a high IRQ, and speed on an ISA video card (only changed parts of
the screen are redrawn, but a full-screen pattern change is a full copy).
There is no mouse support; the keyboard does everything.

## Building

Open Watcom v2 on any host:

    WATCOM=/path/to/openwatcom ./build.sh

`/AUTO:file` runs a key script and writes BMP screenshots (used for testing).
F12 saves a screenshot in most screens.

Sources: `src/vid.c` (VESA, palette, drawing, ROM font), `src/snd.c` (OPL2
sequencer, Sound Blaster, speaker), `src/crt.c` (the monitor model),
`src/game.c`, `src/patterns.c`, `src/text.c`, `src/sys.c`.

## Credit

Descended from *Charge!* by C. Anthony Ray, BYTE, December 1982.
