# Charge! Convergence: design specification

Status: shelved, 2026-11-21. The Convergence Bench built from this specification was
removed from the site and the DOS edition on the owner's decision (not fun, and it will
not be developed). The text below is kept as a record. Originally: proposal, 2026-11-21. Nothing in sections 4 to 11 is built unless section 2
says so. The game is inspired by the original *Charge!* by C. Anthony Ray (BYTE,
December 1982).

## 1. What the game is

**One sentence.** You run a television repair bench: each set that comes in has a
picture fault, and you find the cause by measuring and fix it with the tools a
real technician uses.

**Who it is for.** People curious about how CRTs work, retro-computing and
arcade hobbyists who own a CRT, and students of basic electronics. No prior
knowledge is assumed. Reading age about 12 and up.

**What the player should be able to do after playing.**

1. Explain why a colour picture needs three beams to land together.
2. Look at a test pattern and name the fault: purity, static convergence,
   dynamic convergence, geometry, focus.
3. State the order of adjustment and why it is that order.
4. Say what each adjustment does: purity ring, four-pole ring, six-pole ring,
   yoke position, yoke tilt, strips, degauss.
5. Work by measurement: read an error, predict the effect of a change, make it,
   check it.
6. Know what is safe to touch on a real set and what is not.

**Design pillars.** Every feature is judged against these. A feature that serves
none is cut.

| Pillar | Meaning | Test |
|---|---|---|
| Measure, predict, act, check | The player reasons to an answer; they do not hunt for it | Can a stage be solved on paper from the numbers shown? |
| True to the bench | Tools, order and symptoms match a real set, with simplifications stated | Would a technician recognise each step? |
| One new idea at a time | Each idea is taught, practised, twisted, then tested | Does any stage introduce two new things? |
| Short, clear wins | A stage is one to three minutes and its ending is unmistakable | Does a first-time player know the moment they have won? |

## 1a. Decisions taken (2026-11-21)

The owner accepted the proposals in section 12 with one overriding instruction:
replicate the real experience of adjusting convergence.

That instruction changes the main view. A technician does not see beam paths.
They stand behind the set, watch the whole test pattern in a mirror, and turn
ring tabs by hand. So:

- The main game is the **Convergence Bench** (`index.html`, the main page): the full
  picture in the mirror, and a rear view of the neck with the three ring pairs,
  the yoke and eight places for a strip on the glass.
- The beam-path game (`archive/inside-the-tube/index.html`) was shelved on 2026-11-21: too
  punishing, not educational enough. Its reusable parts are listed in `archive/README.md`.
- The second direction is no longer a separate feature: the picture is two
  dimensional, and every ring has direction and strength from the start.
- "Level N has N strips" is dropped on the bench. Strips are the last resort,
  as on a real set.
- Help levels (apprentice, technician, master) replace the turning modes on the bench.

Built so far on the bench: twelve authored jobs on the shown, practised,
twisted, tested pattern; the diagnosis step on the twisted and tested jobs;
three medals (diagnosis, economy against par, result); a hint on request; the
job sheet; degauss, gun switches, three patterns and the lock. That is most of
milestones 2 to 4 for chapters 2 to 5 in a first form. Not yet built:
milestone 1 (split files, stages as data in their own file, automated checks),
the shop, customers beyond one line, the notebook, colour-blind support, the
daily set, and any play test.

## 2. Where the game is now

Built and working in the browser edition: tutorial, 50 stages in five levels,
one kind of strip that sticks to the glass, three neck rings, yoke slide and
tilt, degauss, three test patterns, gun switches, a lock step, a measurement
line, a ruler, advice text, three modes, music and effects, a painted title.

**Problems, stated plainly.**

1. **It grew by accretion.** Features were added one request at a time. There is
   no single idea of what a stage is for, so stages differ by parameters, not by
   what they teach.
2. **Too many stages teach nothing new.** Of 50 stages about 15 introduce or
   test an idea. The rest are the same task with a different layout.
3. **Level names promise what the stages do not deliver.** Level N gives N strips,
   but most level 2 stages can be finished with one.
4. **The three modes are the same puzzles.** Medium and hard add an option
   (turning) that no stage needs.
5. **The advice line can solve the stage for the player.** It names the tool to
   use. That is good in a lesson and bad in a test.
6. **Nothing is at stake.** Stars are kept but nothing is unlocked, lost or
   compared. There is no reason to play a stage twice.
7. **The screen is dense.** A first-time player sees a ruler, a measurement line,
   an advice line, a mirror, a rings box, arrows and labels at once.
8. **Colour is the only channel.** Red, green and blue carry all the meaning. A
   colour-blind player cannot play.
9. **It has not been play-tested by a person.** Solvability is checked by a
   solver. Whether it is fun, clear or the right difficulty is unknown.
10. **The second direction is half built.** Rings act in both directions; parts,
    strips and the yoke act in one.

## 3. Core loop

One stage is one set on the bench. The loop is the scientific method, and the
interface should show these five steps as five places on the screen.

| Step | Player does | Game shows | Real practice |
|---|---|---|---|
| 1. Observe | Chooses a test pattern, looks at the mirror | The fault as a technician would see it | Pattern generator and mirror |
| 2. Diagnose | Names the fault (choice of 3 or 4) | Right or wrong, and why | Reading the pattern |
| 3. Plan | Reads the measurement, picks the tool | The error as a number; the tool's effect per step | "It needs two clicks of the four-pole" |
| 4. Adjust | Makes the change | Beams move; the number changes | Turning rings, sliding the yoke, placing a strip |
| 5. Verify and lock | Checks every pattern, then locks | Pass or fail on each check; the stamp | Final check, wedge, paint |

**Step 2 is new and is the most important addition.** Today the game tells the
player the diagnosis. Asking for it first turns recognition into recall, which
is what makes it stick, and it gives a second kind of play (deduction) beside
the first (adjustment).

**Scoring one stage.** Three things, each shown at the end:

- **Diagnosis:** right first time, or not.
- **Economy:** number of adjustments made against the par for the stage. This
  rewards predicting over nudging, with no clock.
- **Result:** how far inside tolerance the set was left.

No timer on normal stages. A timer punishes reading and thinking, which are the
behaviours the game wants.

## 4. What stays, what changes, what goes

| Feature | Decision | Reason |
|---|---|---|
| Strip on the glass, one kind | Keep | True to the bench; simple |
| Ruler and effect label | Keep in lessons; remove in tests | Lets the player predict; must be earned away |
| Advice line | Split: a hint the player asks for, not a standing answer | Problem 5 |
| Rings: purity, four-pole, six-pole | Keep; rename on screen to the real names with a plain subtitle | Players should leave knowing the real words |
| Yoke slide and tilt | Keep | Real, and the main cure for edge error |
| Degauss | Keep; make it a diagnosis ("patches no ring cures") | Teaches order |
| Test patterns | Keep; choosing the right one becomes part of step 1 | Real skill |
| Gun switches | Keep; teach once, then optional | Real technique |
| Lock | Keep on jobs | Gives an ending and a cost to carelessness |
| Second direction (ring direction) | Hold back to an advanced chapter; finish it or cut it | Problem 10 |
| Three modes by strip turning | Replace: see section 6 | Problem 4 |
| Strip turning | Cut unless a stage needs it | No stage needs it |
| 50 generated stages | Replace with about 36 authored stages | Problems 1 to 3 |
| Auto-place | Keep as a debug and "show me" tool; it forfeits the stage score | Useful, not play |
| Music, effects, title art | Keep | Working |
| Sprites in the mirror | Keep for early chapters; real patterns later | Friendly start, honest finish |

## 5. Curriculum

Every idea goes through the same four stages. This is the unit of design.

| Stage | Purpose | Help given |
|---|---|---|
| A. Shown | The idea is demonstrated; the player makes one obvious move | Everything: ruler, labels, hint |
| B. Practised | Same idea, different numbers | Ruler and labels |
| C. Twisted | The idea meets something already learned, or its limit | Measurements only |
| D. Tested | A customer's set; diagnose first | Measurements only; hints cost |

Six chapters of six stages. Each chapter ends with a customer job that needs
everything so far.

| Chapter | New idea | The twist stage | What the player can now say |
|---|---|---|---|
| 1. Three beams | Beams must land together; a strip on the glass moves the nearest beam most | The fault is on the far glass; the strip must go opposite it | "A magnet near the tube splits the picture; I can cancel it" |
| 2. The neck rings | Four-pole moves red and blue opposite ways; six-pole moves them together; green is the reference | A ring fixes the centre and ruins nothing, but cannot fix an edge | "Rings set the centre. They change everything equally" |
| 3. Centre and edge | Static before dynamic; rings first, strips last | A stage that punishes strips-first: the ring then undoes the strip's work | "Fix what is common first, then what is local" |
| 4. Purity | Red field; purity ring for the centre, yoke slide for the edges; degauss first | Patches no ring cures: it is magnetised | "Purity comes before convergence, and degauss before both" |
| 5. The yoke | Tilt for the edges; wedge and seal | Sliding the yoke for purity disturbs convergence: the order matters | "The whole procedure, in order, and why" |
| 6. The bench | Mixed jobs with no chapter heading to give the fault away | A set with nothing wrong with its convergence at all | "I can find the fault before I touch anything" |

Later, as separate chapters: both directions; geometry, focus and black level
(the DOS edition already models these); then the electronics chapters in
`ROADMAP.md`.

**Misconceptions to design against.** Each gets one stage whose twist exposes it.

- "Stronger is better": a strip too near the yoke overshoots.
- "Fix the worst-looking picture first": the centre must be right first.
- "Convergence and purity are the same thing": white dots on a tinted red field.
- "A ring can fix a corner": it cannot, and trying costs moves.
- "Degauss is optional": every purity stage after 4A is checked for it.

**Recall, spaced out.** Each chapter's customer job includes one fault from an
earlier chapter without announcing it.

**Reference.** A bench notebook fills in as ideas are met: one page per idea, in
plain words, with the real term, a diagram, and the simplification the game
makes. It is the same content as `docs/REAL-CRT.md`, reached from inside the game.

## 6. Difficulty

Difficulty should come from how much the game explains, not from extra
controls. Proposed in place of the three modes:

| Level of help | Ruler | Effect label | Hint | Diagnosis choices |
|---|---|---|---|---|
| Apprentice | Yes | Yes | Free | 3, with pictures |
| Technician | No | Yes | Costs a star | 4 |
| Master | No | No | None | Type or pick from all |

This is a decision for the owner: the current modes (easy, medium, hard by strip
turning) were specified directly. The proposal is to keep that request's
intent, an easy mode that teaches the fundamentals, and change what the harder
modes take away.

Within a mode, difficulty rises by: number of pictures, tolerance, how many
faults are combined, and how much the faults interact. Never by clutter.

## 7. Engagement

What brings a player back, in order of cost to build.

1. **A clear ending to every stage.** Stamp, sound, the picture coming right. Built.
2. **A reason to replay.** Three medals a stage: diagnosis, economy, result. Par
   for economy is set from the solver's shortest solution.
3. **A place.** The shop. Sets arrive on a shelf, repaired sets leave. The shelf
   is the level-select screen. The title painting already sets this scene.
4. **People.** Each job has a customer and one line of complaint in their words
   ("the news reader has a red shadow"). The complaint is the first clue. The
   DOS edition has this already and it works.
5. **Something to earn.** Bench tools unlock by chapter: the mirror, the pattern
   generator, the degaussing coil, wedges. Each unlock is the lesson of that
   chapter, so progress and learning are the same thing.
6. **A daily set.** One generated job a day, the same for everyone, scored on
   economy. Uses the generator and solver that already exist.
7. **A sandbox.** A set with every fault adjustable, for playing with the physics.

Not proposed: timers on normal stages, lives, energy, currency, random rewards.
They work against thinking.

## 8. Interface

- **One job, one screen, five zones** matching the loop: mirror (observe),
  diagnosis card, measurement panel (plan), the tube (adjust), the job sheet
  (verify and lock).
- **Progressive disclosure.** Chapter 1 shows the tube, the mirror and one
  number. Panels appear in the chapter that teaches them.
- **Never colour alone.** Each beam also has a letter and a line style (R solid,
  G dashed, B dotted). Lamps have shapes as well as colours. A colour-blind
  option swaps the palette.
- **Text.** The 320 by 200 display limits a line to 52 characters. Long
  explanations go in the notebook and in the page below the display, not on it.
- **Input.** Everything by mouse or touch and by keyboard. No action needs a
  right click or a wheel.
- **Sound.** Keep the tone that rises as the gap closes; it is measurement by ear.

## 9. Technical plan

- **Split the single file** into modules: physics, stages, drawing, input, audio,
  interface. Keep a build step that produces one HTML file for distribution.
- **Stages as data.** Each stage is a record: what it teaches (A to D), parts,
  knocked adjustments, tools allowed, help level, par, customer text, notebook
  page. Authored by hand; the generator is used only for the daily set.
- **Checks that run on every change.** For every stage: solvable with the tools
  given; not solvable without the tool it is meant to teach; par equals the
  solver's shortest solution; no text over 52 characters. Today this is a
  script run by hand.
- **Saves.** Medals, notebook pages and settings in local storage, versioned.
- **The DOS edition** stays the alignment bench and pattern generator. It gets the
  curriculum only after the browser edition's is settled.

## 10. How to know it is fun, and that it teaches

These need people. Until they are run, no claim about fun or learning should be
made.

**Play test, five people who have not seen it, 20 minutes each, no help given.**

| Question | Measure | Target |
|---|---|---|
| Do they understand the goal? | Time to first win with no hint | Under 2 minutes |
| Is it reasoning or hunting? | Adjustments per stage against par | Under twice par by chapter 2 |
| Do they keep going? | Stages completed in 20 minutes; do they ask to continue | 8 or more; most say yes |
| Where do they stop? | Stage and moment of each stall over 60 seconds | No stage stalls more than one in five |

**Learning check, same people, before and after.** Six questions, one per
outcome in section 1, such as: "The centre of the picture is white but the
corners have colour fringes. What do you adjust, and what do you not touch?"
Target: four of six right afterwards, from a baseline near zero.

**Accuracy review.** One person who has aligned real sets reads `docs/REAL-CRT.md`
and plays chapter 5. Their corrections outrank this document.

## 11. Build order

Each milestone ends in something playable, and has a test.

| # | Milestone | Contains | Done when |
|---|---|---|---|
| 1 | Foundation | Split the file; stages as data; automated checks; three-medal scoring with par | All checks pass; an existing stage is expressed as data |
| 2 | The loop | Diagnosis step; hint on request in place of the advice line; job sheet as the verify step | A stage cannot be started without a diagnosis; advice no longer appears unasked |
| 3 | Chapters 1 and 2 | Twelve authored stages on the A to D pattern; progressive interface; colour-blind support | Play test, first two rows of the table |
| 4 | Chapters 3 to 5 | Eighteen stages; notebook; the lock; customers | Play test, all rows; learning check |
| 5 | Chapter 6 and the shop | Mixed jobs; shelf as level select; tool unlocks; help levels | A new player reaches chapter 6 without outside help |
| 6 | Reasons to return | Daily set; sandbox | Daily set is solvable and has a par, checked automatically |
| 7 | Beyond convergence | Second direction finished or cut; geometry, focus, black level; then `ROADMAP.md` | Accuracy review |

Milestones 1 and 2 change no content and are the foundation for everything
else. They should come first even though they add nothing a player would list
as a feature.

## 12. Decisions needed from the owner

1. **Modes.** Replace turning-based easy, medium, hard with help-based
   apprentice, technician, master (section 6)? Or keep turning and add help
   levels beside it?
2. **Stage count.** Replace the 50 generated stages with about 36 authored ones
   (section 5)?
3. **Diagnosis first.** Accept the extra step before every job (section 3)?
4. **Strip count as the level theme.** "Level N has N strips" does not fit a
   curriculum built on ideas. Drop it, or keep it as the theme of chapter 3
   onward by building stages with N separate faults?
5. **Second direction.** Finish it with a second view, or remove it until the
   electronics chapters?
6. **Which edition leads.** The plan assumes the browser edition leads and the
   DOS edition follows.
7. **Play testers.** Who can be asked? Without them section 10 cannot be done.
