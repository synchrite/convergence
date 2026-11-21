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

## 2026-11-20: browser edition simplified

Feedback: the puzzle was too complicated and too hard to win, and fifteen beams
on screen at once were confusing.

- Only one set of three beams is drawn at a time: the red, green and blue beams
  aimed at the active test dot. The arrow follows the worst dot; click a dot (or
  press Tab) to choose one. Other dots show only where they land.
- New difficulty ramp. Level 1 is one charge and one dot; level 2 two charges;
  level 3 two dots; level 4 three dots. Purity is not required until level 5.
  Five dots and the tight tolerance (2 pixels) start at level 7.
- Convergence tolerance is 3 pixels on levels 1 to 3, 2.5 on 4 and 5, then 2.
- Levels are chosen so they start visibly misconverged, and the early ones not
  wildly so. Checked with the auto-placer: levels 1 to 6 can each be solved
  with a single strip, levels 7 to 9 with three or four.
- Strips can be dragged. R, P, T and 1 2 3 change the strip under the cursor.
  A plain click on a strip still removes it.
- The tool starts as a quadrupole across the beams, which is what level 1 needs.
  Each of the first five levels opens with one line of advice.
- Stars now depend on strips used against the level's budget.
- The DOS edition is unchanged by this.

## 2026-11-20: browser edition made more fun, and seen from behind the set

Feedback: make it fun, and make the playfield look like a real television
being converged, seen from the side or rear with the front in a mirror.

- **A picture to fix.** Each test dot is now a small pixel-art character (invader,
  heart, face, star, ghost), drawn once by each beam. Misconverged, it is three
  coloured ghosts; within tolerance it snaps to one white character and dances.
- **The technician's view.** The playfield is the set from behind with the back
  off: gun and neck with purity, four-pole and six-pole ring magnets, the
  copper deflection yoke, the glass funnel and faceplate, the anode lead, and
  the chassis board. The picture is seen in a mirror on a stand in front of
  the set, reversed left to right, as in real convergence work.
- **Sound.** Synthesised effects for placing, turning and removing strips, a
  rising chime as each character locks, a fanfare on a win. A Sound button
  turns them off.
- **Motion.** Electrons stream along the three active beams. Sparks fly when a
  character locks and when the level is won; the stars earned are drawn on screen.
- Enter goes to the next level after a win. The mouse wheel changes the
  strength of the strip under the cursor.
- The DOS edition is unchanged by this.

## 2026-11-20: electro groove music, AdLib style

- **Browser edition.** A small two-operator FM synthesiser in Web Audio, arranged
  like an OPL2 voice (a sine carrier frequency-modulated by a second sine, with
  a decaying modulation index). The tune is an electro groove at 112 beats a
  minute over Em7, Em7, C, D: four-on-the-floor FM kick, snare on two and four,
  swung sixteenth hi-hats, a syncopated FM bass with two patterns, chord stabs
  from the third bar, and a fill every eighth bar.
- The music reacts to play: a lead arpeggio joins at half volume when half the
  pictures are locked, and at full volume with a bell line when the level is won.
- A Music button turns it off. Music starts with the first click or key, because
  browsers do not allow sound before that.
- **DOS edition.** The same groove on the real OPL2: six tracks (bass, stab,
  lead, kick, snare, hi-hat) with four new instrument patches. It is the title
  theme; the bench plays it without the lead and quieter. The sequencer now
  takes six tracks, and jingles moved to channels 6 and 7.
- Checked by measurement, not by ear: the browser tune was rendered offline
  (no clipping, kicks stand out on the beat); the DOS tune was recorded from
  the emulated card (bass E, E, C, D by bar, about 113 beats a minute).

## 2026-11-20: tutorial

- The browser edition has a tutorial, shown as level 0, that explains the game
  by showing how the beams interact. Five steps on one playfield:
  1. Three beams leave the gun and, left alone, meet at the screen: one white picture.
  2. A stray field appears and pulls the nearest beam most: three coloured ghosts.
  3. You place and drag a dipole strip: all three beams move together, and the ghosts stay apart.
  4. You place a quadrupole strip across the beams, flip it if needed, and the picture goes white.
  5. A summary, then level 1.
- It opens by itself the first time the game is played (remembered in the
  browser), and from the Tutorial button or the `#tutorial` address at any time.
  Enter, or a click on the text box, moves to the next step; step 4 ends only
  when the picture is fixed.
- Fixed while testing: sprite choice crashed on level 0.

## 2026-11-20: title screen, plain tutorial, stages, rings, real causes

- **Title screen.** A painting of a television repair shop (a wood-cabinet set
  showing a crosshatch, an oscilloscope, the technician's mirror, a multimeter,
  valves), digitised to 320 x 200 in 256 colours with dithering. The logo
  converges from three colours to white. The art is made by `tools/paint_title.py`:
  a composed scene gone over with brush strokes by program. It is not painted
  by hand; an image model was tried first and was not available.
- **Tutorial rewritten in plain words.** Six short steps with no technical
  terms: the three beams, white when they meet, a magnetised part splits them,
  put a strip down, drag and flip it, done. Two steps are hands-on.
- **Levels and stages.** Three levels of ten stages. Level 1 gives one strip,
  level 2 two, level 3 three. New elements arrive inside each level: tighter
  tolerance, a second and third picture, the squeeze ring (stage 1-6), the
  shift ring (2-5), the purity ring and a purity target (3-1).
- **Neck rings.** Squeeze (four-pole), shift (six-pole) and purity rings, each
  -6 to +6, in a RINGS box. A ring changes every picture equally; a strip
  changes only the beams that pass it.
- **Every stage is checked.** `tools/find_seeds.js` runs the solver on each
  stage and stores a layout that starts visibly broken and can be fixed with
  that stage's strips and rings. All 30 have one.
- **Less trial and error.** The strip under the pointer bends the beams before
  it is placed, and while it is dragged. Arrows on each strip show how it
  pushes each beam. A line at the bottom names the fault ("red high, blue low:
  squeeze them together"). A tone rises as the beams close up.
- **Obvious success.** A spot that would fix the set turns the strip green and
  says so; a fixed set outlines its strips in green and turns the target mark green.
- **The + and - have a reason.** The fault sources are now real parts on or
  outside the glass (uneven yoke winding, magnetised bracket or shield,
  loudspeaker magnet), each with a small + or - polarity mark as the nod to
  the 1982 game. Their field now falls off with distance squared.
- **Strips renamed** squeezer and mover; the technical names are in the notes.
- Added `docs/REAL-CRT.md` (how the flat game maps to a real tube, what is true
  and what is simplified) and `ROADMAP.md` (the repair-teaching direction).

## 2026-11-21: easy, medium and hard modes

- **Easy** (the default): a strip always stands upright across the beams and
  cannot be turned. The fundamentals are taught this way: slide it, flip it.
- **Medium**: a strip can also be given a quarter turn, to lie along the beams.
- **Hard**: four angles, as before.
- No stage needs a turn its mode does not allow. `tools/find_seeds.js` takes the
  mode as an argument and holds the solver to that mode's angles; all 30 stages
  pass in all three modes.
- As it turned out, the same 30 layouts pass in every mode, so for now the modes
  share layouts and differ only in what the player may do. Medium and hard do
  not yet have stages built to reward turning.
- A Mode button switches mode and restarts the stage; the choice is remembered.
  Stars are kept separately for each mode. The Rotate button is disabled in easy.

## 2026-11-21: level 4, the real alignment procedure

Priority set by the owner: real CRT repair technique first.

- **Level 4** (ten stages) is a real alignment in the real order. A job list at
  the top ticks off: 1 degauss, 2 purity, 3 centre, 4 edges.
- **Degauss.** A magnetised shadow mask puts patches on the red field that no
  ring cures. D (or the Degauss button) clears it. Stage 4-2 teaches it.
- **Test patterns in the mirror.** M (or the Pattern button, or a click on the
  mirror's heading) switches between dots, for convergence, and a red field,
  for purity. Each fault shows only on its own pattern.
- **The yoke moves.** Sliding it along the neck changes purity at the edges and
  leaves the centre alone (stage 4-3). Tilting it makes red and blue cross
  opposite ways at the two edges (stage 4-4). The drawn yoke slides and tilts.
- **Order matters.** Sliding the yoke also shifts edge convergence a little, so
  purity is done before convergence, as on a real set.
- **Advice follows the procedure.** The bottom line now reasons like a
  technician: degauss first; centre tinted, purity ring; centre pure and edges
  tinted, slide the yoke; fix the centre picture with the rings first; centre
  good and edges split opposite ways, tilt the yoke; strips last.
- Stages 4-1 to 4-6 teach one step each and need no strips; this was checked by
  applying the procedure with no strips placed. 4-7 to 4-10 are whole jobs with
  one or two magnetised parts as well. All ten were checked with the solver in
  each mode.
- `docs/REAL-CRT.md` has a table setting each step on a real set beside its
  stage in the game.

## 2026-11-21: locking, gun switches, crosshatch, and the second direction

The four things listed as not yet modelled after level 4.

- **Lock (K).** Level 4 and 5 jobs (stages 5 to 10) end by wedging the yoke and
  sealing the rings with paint. Nothing turns after that. Breaking the seal
  costs a star. The job list gains "5 LOCK"; wedges and paint dabs are drawn.
- **Gun switches (7, 8, 9, or the R G B buttons).** A gun that is off draws no
  beam and no picture. With green off, red and blue alone make magenta, which
  isolates the squeeze adjustment. A stage cannot be finished with a gun off.
- **Crosshatch.** A third test pattern in the mirror. Level lines show the error
  along the gun row, upright lines the error across it. Patterns now cycle
  dots, crosshatch, red field, and are available on every level.
- **Level 5: the second direction.** Ten more stages. Each convergence ring has a
  direction (0, 45, 90, 135 degrees) as well as a strength, as the real tabs do.
  The part of its effect along the gun row bends the beams in the playfield;
  the part across it shows in the mirror as a sideways split. Stages 5-1 to 5-4
  teach it with rings alone; 5-5 to 5-10 are whole jobs in both directions.
- Convergence is now measured as distance in both directions together.
- Checked: stages 5-1 to 5-4 are fixed by matching the ring's direction and
  setting the opposite strength, with no strips; all ten level 5 stages pass
  the solver in each mode; a level 4 job does not pass until it is locked, and
  a sealed ring refuses to turn.
- Simplified: magnetised parts, strips and the yoke still act only along the
  gun row.

## 2026-11-21: strip bug fixed, measurement tools, heavier music

**Bug: the upright squeezer barely worked.** Reported as a glitch in level 1
strip placement. A squeezer's push varied across the strip's width. For an
upright strip (the only kind in easy mode) that is along the beam's travel, so
a beam was pushed one way entering and the other way leaving, and almost
nothing was left. What was left depended on where the simulation's 5 pixel
steps fell, so it flickered between two values as the strip slid. Measured
before the fix: a gap of 5.2 changed by about 0.5, jumping every few pixels.

- A squeezer's push now grows with distance from the strip's centre line across
  the beams, whichever way the strip is turned.
- The bend is summed over five samples a step, so it follows the true length of
  path inside the strip and changes smoothly.
- Measured after: sliding from the screen to the neck changes the gap smoothly
  from 0 to about 10 at strength 2; flipping the strip reverses it.
- The earlier stage checks had passed only because the solver exploited the
  flicker. All 50 stages were re-checked in all three modes after the fix.

**Measure, look up, place.** Feedback: placing strips was fiddly hunt-and-peck.

- The bottom line now shows measurements of the marked picture, signed, in
  pixels at the screen: GAP (blue minus red), OFF (their middle minus green),
  LAND (all three minus the white mark), and from level 5 the sideways pair.
- A ruler across the top of the tube shows how much the strip in hand would
  change the gap (or the landing, for a mover) at each place along the tube.
  A gap of +5 is cancelled by putting the strip at the "-5" mark.
- Each strip carries a label with its exact effect ("GAP -5.0"). While a strip
  is in hand the bottom line also shows the value it would leave.
- Strips sit on a 4 pixel grid. In easy mode and the tutorial a strip centres
  itself on the picture's beam line, so only its distance along the tube is
  chosen. The solver obeys the same rule, and all stages pass with it.
- The rings box states the unit: one click moves the beams 1.5.
- Stage 1-1 to 1-3 and the tutorial now teach this method.

**Music.** The bass has a sub-octave under every note and a held low root under
each half bar, and more bite. An "intrigue" melody (E minor with its leading
tone, sparse, with echoes) plays for four bars in every sixteen. The level was
lowered to stop clipping (rendered peak 0.92). The DOS edition's music is unchanged.

## 2026-11-21: strips are magnets on the glass

Feedback: make placing convergence strips more fun and more realistic.

- **Realistic.** A strip is now what it is on a real set: a small magnet stuck to
  the outside of the funnel glass, between the yoke and the screen. It can go on
  the top glass or the bottom glass. It no longer floats in the vacuum, and the
  two abstract kinds (squeezer and mover) are gone. Its field reaches the beams
  from the glass and falls off with distance squared, like the fault parts'.
- **What you do with it.** Slide it along the glass under the pointer; press it
  on (click); flip it over, so the face toward the tube pulls or pushes (P);
  trim it to one of three lengths, shorter being weaker (1, 2, 3 or the wheel);
  drag to slide it, including across to the other glass; click to peel it off.
  A strip cannot go where a part is already fixed to the glass.
- **Fun.** Each action has its own sound (a thud when pressed on, a tick for each
  step it slides, a snip when trimmed, a whoosh when flipped, a rip when peeled)
  and a puff of dust. The strip's field is drawn pulsing toward it or away from
  it. Arrows on the beams show how hard it pulls each one.
- **Still by measurement.** The ruler across the top now tabulates the strip in
  hand along the top or bottom glass, whichever the pointer is nearer.
- **Modes.** Easy: strips lie flat. Medium: a quarter turn, which leaves four
  tenths of the strength for finer steps. Hard: 45 degrees as well (seven tenths).
- **Fault parts** now have strengths that are not whole numbers (0.7 to 1.6), so a
  strip cannot simply copy the part it is cancelling.
- Advice reads in terms of the strip: "gap too wide: push red down, or blue up".
- Purity is now a matter for the purity ring and the yoke only, as on a real set.
- All 50 stages were re-checked in all three modes with the new strips. The
  tutorial and stage 1-1 to 1-3 texts teach the new handling.

## 2026-11-21: one standard strip

Request: keep to one type of convergence strip, the usual one.

- There is now exactly one strip, always the same size. Trimming to three lengths
  is removed (the 1 2 3 keys, the mouse wheel and the Trim button no longer act
  on strips).
- What is left is what is done to that one strip: slide it along the top or bottom
  glass, flip it over (pull or push), press it on, peel it off, and in medium
  and hard modes turn it, which weakens it.
- Its strength at the beams is set by position alone in easy mode.
- All 50 stages were re-checked in all three modes with the single strip, and pass.

## 2026-11-21: the rings alone are never enough

Feedback: too often a stage could be won by changing only the squeeze ring.

- **Cause.** The fault parts sat near the neck, where the beams for every picture
  pass through almost the same place. A part there bends all the pictures alike,
  and a ring corrects exactly that kind of error, so the strips had nothing to do.
- **Fix in the design.** On stages that have a ring, the parts now sit further
  along the funnel and are stronger. There they act on the pictures unequally,
  which a ring cannot undo.
- **Fix in the checking.** `tools/find_seeds.js` now keeps a layout only if the
  solver, given the rings and yoke but no strips, fails to fix it. It prefers
  layouts that also cannot be fixed with one strip fewer than the level gives.
  For each stage it reports how many layouts it threw away because the rings
  were enough: between 0 and 25.
- **Result, checked by brute force.** On all 32 stages that have both parts and
  rings, no setting of the squeeze ring alone wins the stage. On levels 1 and
  2, no combination of every available ring wins without a strip.
- **Result, by the solver.** On levels 3 to 5 the solver, given every ring and the
  yoke but no strips, did not fix any of those stages. Some stages were also not
  solved with one strip fewer than the level gives (3-7, 3-9, 3-10, 5-5, 5-8,
  5-10, and all of level 1, where one strip is all there is). On the rest,
  including all of level 2, fewer strips than the level gives can still be enough.
- A first run of this search left stages 1-7 to 1-10 with layouts from before
  the part strengths were finalised; one of them could still be won with the
  squeeze ring. A direct test caught it and those stages were searched again.
  Level 1 parts on ring stages are stronger (2 to 4) than on later levels.
- These results come from a search that can fail to find a solution that
  exists, so "not solvable with fewer" means the solver did not manage it, not
  a proof.
- The ring and yoke lesson stages (1-6, 2-5, 3-1, 4-1 to 4-6, 5-1 to 5-4) are
  unchanged: they have no parts and are meant to be done with the adjustment alone.
- Medium and hard use the same layouts as easy. Anything solvable with flat
  strips is solvable when turning is also allowed.

## 2026-11-21: design specification

- Added `docs/DESIGN.md`: what the game is for and what a player should learn,
  an account of what is wrong with the current build, a five-step core loop
  with a diagnosis step, a keep, change and cut list, a six-chapter curriculum
  on a shown, practised, twisted, tested pattern, help-based difficulty,
  engagement features, interface rules, a technical plan, play-test and
  learning measures, a build order, and seven decisions for the owner.
- No game code changed.
