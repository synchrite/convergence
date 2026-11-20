# Development notes

Charge! Convergence is inspired by the original *Charge!* by C. Anthony Ray
(BYTE, December 1982). These notes explain how each part works, how to build
and test it, and what is still open.

## 1. Lineage

| Step | Kept from the step before | New |
|---|---|---|
| *Charge!* 1982 (`original/`) | | One electron, ions with a 1/r force, score by where it lands |
| Charge! Phosphor (`classic/`) | The 1982 step law and constants (speed 10, CG 25), ion placement | CRT setting, 16 bands, evenness, burn |
| Physics lab (`v2/`) | The goal of even coverage | Real units, Lorentz force, lessons |
| Convergence, browser (`index.html`) | Ion placement, 1/r force | Three beams, strips, VGA-style display |
| Convergence, DOS (`dos/`) | Three beams, strips, convergence as the goal | Whole-monitor alignment, real test patterns, DOS hardware |

What comes from the 1982 listing in the browser editions: the velocity law of
lines 1010 to 1090 (classic only, not accumulated), and the charge placement of
lines 2080 to 2110 (odd x, even y, alternating sign, positive first). The DOS
edition shares the idea, not the code.

## 2. Browser editions

Each is one HTML file: markup, CSS and script together, no build step, no
libraries. Open the file to run it.

- **`index.html`** draws into a 320x200 byte buffer of RGB332 colour indices and
  copies it to a canvas through a palette table. Beams are simulated in a
  256x160 field: speed 5 pixels a step, charges add `0.5*q*d/(|d|^2+36)` to the
  velocity each step, strips rotate the velocity by `0.012*strength` radians a
  step (quadrupole: scaled by position across the strip). Levels come from
  `cfg(L)`: charges, aim points, tolerances and strip budget; `genLevel` tries
  seeds until the start is misconverged enough. Only the active dot's three
  beams are drawn. `window.__cv` exposes the state for tests. Music is a
  step sequencer (`mStep`) scheduled 150 ms ahead on the audio clock; voices
  are built per note by `fm()` (carrier, modulator, two envelopes). `window.__mus`
  lets a test render it into an `OfflineAudioContext`.
- **`v2/index.html`** integrates `du/dt = (q/m)(E + v x B)` with a relativistic
  Boris pusher, 420 steps a trace. Spot position against drive is tabulated per
  axis and inverted for S-correction. `window.__v2` exposes the state.
- **`classic/index.html`** uses the 1982 step law directly. `window.__crt`
  exposes the state.
- **`original/`** has its own build: `build.py` inlines `CHARGE.BAS` and
  `SHAPES.TXT` into `template.html` to make `index.html`.

Testing was done with Playwright in headless Chromium: load the page, drive it
with the keyboard and the exposed state, check for page errors, take
screenshots, and compare numbers with hand calculations (for example plate
deflection against `L*l*Vp/(2*d*Va)`).

## 3. DOS edition

### Source layout (`dos/src`)

| File | Contents |
|---|---|
| `cc.h` | Shared types, palette layout, key codes, module interfaces |
| `main.c` | Command line, start-up and shut-down order |
| `vid.c` | VESA mode query and set, linear and banked output, palette, primitives, dithering, additive drawing, ROM fonts, BMP screenshots |
| `sys.c` | Timer, keyboard, scripted test mode |
| `snd.c` | OPL2 driver and sequencer, songs, Sound Blaster DSP and DMA, PC speaker |
| `crt.c` | The simulated monitor: geometry, convergence, purity, focus, black level |
| `game.c` | Menus, bench, panel and diagrams, hints, scoring, report, career, save file |
| `patterns.c` | Full-screen patterns for the real monitor |
| `text.c` | Control descriptions, jobs, handbook, pattern notes |
| `g.h` | Game declarations: controls, groups, patterns, fault flags |
| `sintab.c` | Generated 256-entry sine table |

### Building

Open Watcom v2 (any host; Linux was used):

    WATCOM=/path/to/openwatcom dos/build.sh

The script runs `wcl386 -bt=dos -l=causeway -4r -oneatx -za99`. `-l=causeway`
binds the CauseWay extender stub, which is why the result is one file.
`$WATCOM/binw` must be on the path so the linker finds `cwstub.exe`; the script
adds it.

### Design decisions

- **No interrupt handlers.** Music, effects and Sound Blaster completion are
  polled from the main loop (`sys_idle`, called from the key loop and inside
  long redraws). This avoids locking memory and reentrancy problems under a
  DOS extender, at the cost of music timing that can lag during a slow redraw.
- **Sound Blaster without its IRQ.** The card's IRQ line is masked at the PIC
  while the program runs. Playback is single-cycle 8-bit DMA; the end is found
  by reading the DMA controller's terminal-count bit, then the DSP is
  acknowledged by reading port 2xE. The mask is restored on exit.
- **Timer.** Channel 0 is set to mode 2 at the normal 18.2 Hz rate so its counter
  reads linearly; time is the BIOS tick count plus the counter. Mode 3 is
  restored on exit.
- **Integer math only.** Positions are in 1/16 pixel, raster coordinates run
  -256 to 256, angles are 256 to a turn. No floating point is executed, so a
  486SX needs no emulator.
- **Screen updates.** Everything is drawn into a back buffer. `vid_present`
  compares each row with a copy of what is on the card and writes only the
  changed span. The bench background is drawn once and cached.
- **Palette.** 0 to 215 are a 6x6x6 cube, 216 to 239 a grey ramp (used for the
  PLUGE patterns, which need steps finer than the cube), 240 to 255 fixed
  interface colours. Switching a gun in the pattern generator is done by
  rewriting the DAC, not the picture.
- **The monitor model** (`crt.c`). Each control has a value and a hidden ideal;
  the picture is drawn from the differences. Ring pairs are two-dimensional:
  the residual is the vector difference between the set and ideal positions.
  Four-pole residual moves red and blue opposite ways, six-pole the same way.
  A strip at one of eight positions shifts red and blue tangentially with a
  cubic falloff from that edge. Purity error and mask magnetisation tint the
  field patterns only.

### Testing

`CONVERGE /AUTO:file` reads a script of key names, `w:N` waits and `s:NAME.BMP`
screenshots, runs it, and exits at the end. In scripted mode only, the key
`` ` `` on the bench sets every control to its ideal value and all jobs are
unlocked, so pass paths can be reached.

Runs were made in DOSBox-X under a virtual display:

- machine `svga_s3`, 486 core, 26800 and 12000 cycles, 8 MB and 4 MB;
- `fpu=false`;
- `/BANK`, checked by capturing the emulator window, not the back buffer;
- sound checked by recording the emulated card to WAV and measuring level and
  pitch each second.

### Known limits and open items

- Not run on real hardware yet. First things to check there: VESA mode set on
  the actual card, bank switching speed on ISA, AdLib detection, Sound Blaster
  with the machine's real IRQ and DMA.
- A Sound Blaster on IRQ 8 or above is not masked and has not been tested.
- No mouse.
- The music has been measured, not listened to; instrument patches may need
  tuning by ear.
- The monitor model is a teaching model: controls do not interact as much as
  on a real chassis, and corner effects are simplified.
- The browser convergence puzzle is not ported to DOS; the DOS game is the
  service bench.
- An in-browser build of the DOS edition (for example with js-dos) does not exist.

## 4. Repository rules

- The 1982 original is published separately. Nothing from this repository is
  published at the original's address.
- `original/` is the author's work: do not edit the listing beyond the three
  documented corrections.
- Record every change in `CHANGELOG.md`.
- Documentation style: plain, factual, no promotional language.
