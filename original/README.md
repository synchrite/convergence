# Charge! (BYTE, December 1982)

**Charge!** was written by **C. Anthony Ray** (Urbana, Illinois) in 1982. It
was published in **BYTE magazine, December 1982, pages 112 to 120**, as the
second-place winner of the BYTE Game Contest. The program is his original
Applesoft BASIC listing, copyright 1982 C. Anthony Ray, and the game design,
rules and code are his.

This repository contains only an emulator and a transcription of the printed
listing, so that the game can be run in a browser. The BASIC is not rewritten.
It is not affiliated with the author or with BYTE Publications Inc. The listing
remains the author's work and BYTE's publication; no licence to it is
granted here. If the author or the publisher asks for it to be removed, it
will be.

| File | What it is |
|---|---|
| `CHARGE.BAS` | Listing 1, typed from the magazine pages (lines 10 to 9290) |
| `SHAPES.TXT` | Listing 3, the shape table (the `CHARGE SHAPES` file, loaded at $6000) |
| `template.html` | The emulator: Applesoft interpreter, 40x24 text screen, hi-res screen, shape DRAW/XDRAW, speaker, 6502 core |
| `build.py` | Inlines the two data files into `template.html` and writes `index.html` |
| `index.html` | Generated, open it in a browser |

Listing 2 (the fanfare machine code at $300) is not a separate file: the BASIC
program POKEs it from the DATA statement in line 9050, and the emulator
executes those bytes (the 9 opcodes the routine uses).

## Corrections to the printed listing

The listing was transcribed as printed, with three exceptions where the print
has an evident typo that stops the program from running:

| Line | Printed | Used |
|---|---|---|
| 6020 | `FV = 5: S0 + 10: CG = 25` | `FV = 5: S0 = 10: CG = 25` |
| 3050 | `FOR I = OE TO 50; IM = ...` | `FOR I = OE TO 50: IM = ...` |
| 9220 | `FOR I + 1 TO 30: NEXT I` | `FOR I = 1 TO 30: NEXT I` |

## Judgement calls (not visible in the print)

- Line 6050 and 6070 star counts and the `!` spaces in line 6110: the column
  arithmetic (`HTAB 39` for the right border, box from `HTAB 14`, `CHARGE!` at
  `HTAB 17`) gives 39 stars and 11 spaces.
- `HCOLOR` is never set before the first field's border is drawn (line 2020),
  so the emulator starts with white (3), as a real Apple would after `HGR`
  in the figures.
- The `CALL` register state on entry to the fanfare routine is arbitrary on a
  real machine; the emulator starts with X = Y = 0.

## Timing

Statement costs are estimates of Applesoft's speed on a 1 MHz Apple II, so
delay loops, score tick-up and the flight of the electron run at roughly the
original pace. The speed selector scales all of it.
