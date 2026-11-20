# Changelog

All changes to this project, oldest first. The game is inspired by the original
*Charge!* by C. Anthony Ray (BYTE, December 1982).

Dates are year-month-day. Entries marked *(charge repository)* were made in the
separate repository that holds the 1982 original; they are listed here because
this project grew out of that work.

## 2026-11-19: the 1982 original *(charge repository)*

- Started from scans of the BYTE article and listings.
- First attempt: a browser rewrite of the rules, with guessed physics. Corrected
  twice as more pages arrived (inverse-distance force, then the real constants
  from line 6020 and the move routine in lines 1010 to 1090).
- Replaced the rewrite. The original Applesoft BASIC listing is now typed in
  as printed and runs unchanged in a purpose-built emulator: Applesoft
  interpreter, 40x24 text screen, hi-res graphics with shape tables, speaker,
  and a small 6502 core that runs the listing's own fanfare routine.
- Three evident printing errors corrected so the program runs (lines 3050,
  6020, 9220); each is listed in `original/README.md`.
- Credit to C. Anthony Ray and BYTE added to the page and the README.
- Published to its own repository. A Pages workflow that had been added to an
  unrelated repository was removed again.

## 2026-11-20: Charge! Phosphor (`classic/`)

- New game on the original's physics. The field became the inside of a CRT:
  gun at the left, charges in the vacuum, a phosphor wall at the right.
- Goal changed from one corner to even coverage: 16 shots, 16 bands, one each;
  a second hit in a band leaves a burn.
- Quality-of-life: seeded tubes with a shareable `#tube-N` address, landing
  preview under the cursor, Converge and Auto-fill, Undo, Retry, keyboard
  aiming, click to skip the animation, remembered charge count and best score.
- A large "New Version" link added in the corner, leading to the physics lab.

## 2026-11-20: the physics lab (`v2/`)

- A separate page with standard physics instead of the 1982 steering law:
  relativistic electron speed from the anode voltage, electric plates and
  magnetic coils integrated with a Boris pusher, a model 8 inch tube.
- Effects shown: the beam striking its own plates, flared plates, the neck
  shadow, overscan, the tan(theta) stretch of a flat screen and S-correction,
  magnetic deflection leaving speed unchanged, spot size, brightness,
  persistence and burn-in.
- Seven lessons, each with a button that sets the controls.
- Fixed during testing: sign of the electric force on the electron; funnel
  geometry (the flare now starts inside the yoke); an overlay canvas that hid
  the phosphor.

## 2026-11-20: Charge! Convergence, browser edition (`index.html`)

- Forked into its own project. Earlier games moved to `classic/` and `v2/`,
  the 1982 original kept in `original/`.
- New game: three beams (red, green, blue) from an in-line gun, bent by stray
  charges placed as in the 1982 listing, corrected by magnetic convergence
  strips the player places. Dipole strips turn all beams; quadrupole strips
  pull the outer beams together or apart.
- Test pattern of five magnified dots; pass at 2 pixels of spread and 6 of
  purity error. Levels, strip budget, stars, Auto-place (simulated annealing,
  shown as it searches, with retries).
- VGA-style display: 320x200, 256 colours in an RGB332 palette, ordered
  dithering, additive blending on palette indices, bitmap font, scanline filter.

## 2026-11-20: Charge! Convergence, DOS edition (`dos/`)

- New program for real DOS on a 486: `CONVERGE.EXE`, 32-bit protected mode,
  Open Watcom, CauseWay extender bound into the one file.
- Video: VESA 640x480 in 256 colours, linear frame buffer or bank switching,
  6x6x6 colour cube with grey ramp, ordered dithering, additive drawing, the
  card's own ROM fonts, changed-span screen updates.
- Sound: AdLib (OPL2) music and effects from a polled sequencer; Sound Blaster
  8-bit digitised effects by single-cycle DMA with no interrupt handler; PC
  speaker fallback. Samples are generated at start-up.
- Game redesigned as a service bench: a simulated monitor with geometry,
  focus, black level, purity ring, four-pole and six-pole convergence rings,
  convergence strips and degauss; five test patterns; ten repair jobs; free
  bench; three difficulty levels; scoring, stars and a save file.
- Twelve full-screen test patterns for the real monitor attached to the PC,
  with notes, gun switches and three resolutions.
- Handbook: safety, order of adjustment, purity and convergence, pattern use.
- Integer math only, so no coprocessor is needed.
- After the first emulator run: removed a speckled glass highlight; widened the
  ticket and job sheet; wrapped long hints over two lines; shortened key
  legends; redrew the title with converging beams and colour bars; Sound
  Blaster is now used only when `BLASTER` is set.
- Tested in DOSBox-X (see `dos/README.md`). Not yet tested on real hardware.

## 2026-11-20: publication and documentation

- Pushed to its own repository, separate from the 1982 original, so the two
  never share an address.
- Added `README.md`, `docs/INSTALL.md`, `docs/DEVELOPMENT.md` and this file.
