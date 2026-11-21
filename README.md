# Charge! Convergence

A game and learning tool about aligning a colour CRT. **Inspired by the original
*Charge!* by C. Anthony Ray**, published in BYTE magazine in December 1982 as the
second-place winner of the BYTE Game Contest. In his game you steered one
electron past charged ions onto a target. Here there are three beams, the ions
have become magnets, and the target is a white dot on a phosphor screen.

This is a different game from the original, not a new version of it. The
original has its own repository and site; nothing here replaces it.

## What is in this repository

| Path | What it is | Runs on |
|---|---|---|
| `dos/CONVERGE.EXE` | **DOS edition.** The Convergence Bench (the same twelve jobs as the main page), a wider monitor service game, and a test-pattern generator for a real monitor | DOS, 486 class PC, SVGA, AdLib / Sound Blaster |
| `index.html` | **Convergence Bench (main page).** The real experience: the whole test pattern in a mirror, ring tabs, yoke and strips under your hands; twelve authored jobs with diagnosis and medals | Any modern browser |
| `inside/index.html` | **Inside the tube.** The beam-path game: a slice through the tube showing why the beams part and what a ring or a strip does to them | Any modern browser |
| `v2/index.html` | Physics lab: a beam steered by real electric and magnetic deflection, with lessons | Browser |
| `classic/index.html` | Charge! Phosphor: land 16 shots evenly on a phosphor wall | Browser |
| `original/` | The 1982 Applesoft BASIC listing running in an emulator, kept for reference | Browser |

## Documentation

| File | Covers |
|---|---|
| [`docs/INSTALL.md`](docs/INSTALL.md) | Requirements, installing, writing the DOS edition to a disk for a real PC, running in DOSBox, playing in a browser |
| [`docs/DEVELOPMENT.md`](docs/DEVELOPMENT.md) | How the games work, source layout, building, testing, design notes, known limits |
| [`CHANGELOG.md`](CHANGELOG.md) | Every change, in order |
| [`docs/REAL-CRT.md`](docs/REAL-CRT.md) | How the flat game maps to a real, three-dimensional tube |
| [`ROADMAP.md`](ROADMAP.md) | Where the project is going: CRT repair, electronics, fault tracing |
| [`docs/DESIGN.md`](docs/DESIGN.md) | Design specification: goals, core loop, curriculum, engagement, build order, open decisions |
| [`dos/README.md`](dos/README.md) | DOS edition: features, switches, what was tested |
| [`original/README.md`](original/README.md) | The 1982 listing: transcription notes and credit |

## Quick start

- **Browser:** open `index.html` for the bench, or `inside/index.html` for the look inside the tube. No install, no network needed.
- **DOS:** copy `dos/CONVERGE.EXE` to the PC and run `CONVERGE`. See `docs/INSTALL.md`.

## Status

The DOS edition has been tested in the DOSBox-X emulator set up as a 486 with
an S3 SVGA card, AdLib and a Sound Blaster 16. It has not yet been run on a
real 486. Details are in `dos/README.md`.

## Credit and rights

- *Charge!* is by C. Anthony Ray, copyright 1982, published by BYTE Publications
  Inc. His listing in `original/` remains his work; no licence to it is granted
  here, and it will be removed if he or the publisher asks.
- Everything else in this repository is new work inspired by his game.
- DOS edition: built with Open Watcom v2; the CauseWay DOS extender (public
  domain) is bound into the EXE.

## Safety

A CRT monitor holds lethal voltages, even unplugged. The only things this
project asks you to do to a real monitor are done from outside the case. The
rings, yoke and strips are taught on a simulated set.
