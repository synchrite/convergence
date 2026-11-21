# Archive

Work that is shelved, kept for its ideas and parts. Nothing here is linked from
the site.

## inside-the-tube

The beam-path game: a slice through the tube, three beams, magnets placed on
the glass, ten stages in each of several levels. Shelved on 2026-11-21 on the
owner's judgement: too punishing, and it taught little. The file still runs if
opened directly.

Worth reusing elsewhere:

- The title screen: painted repair-shop art reduced to a VGA palette
  (`art/title.png`, `tools/paint_title.py`).
- The music: an FM-style electro track with a heavy bass line and an occasional
  lead, made in Web Audio.
- The plus and minus charges as a nod to *Charge!* (BYTE, December 1982).
- The cross-section view as an explanation of why three beams part and what a
  ring or strip does to them. It may suit a short non-scored explainer better
  than a game.
- Measurement readouts in place of trial and error.
- Easy, medium and hard modes that limit how a strip can be turned.

Reasons it did not work, to avoid repeating them: stages were won or lost on
exact placement with little feedback on why; difficulty came from precision,
not from understanding; the slice view does not match what a technician sees.

## convergence-bench

The Convergence Bench: a mirror view of the test pattern, the neck of the tube
from behind and from the side, ring pairs, yoke tilt and strips fitted by
hand; twelve authored jobs with diagnosis and medals. Shelved on 2026-11-21 on
the owner's decision: not fun, and it will not be developed. `index.html` is
the browser page and still runs if opened directly. `dos/cbench.c` is the DOS
port, removed from the build, with screenshots of it running.

Worth reusing elsewhere: the ring-pair model (turning sets direction, spreading
sets strength), the order of work (degauss, purity, centre, edges, lock), and
the diagnose-before-touching question.
