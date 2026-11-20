# Charge! Convergence

A fork of **Charge!** by **C. Anthony Ray** (BYTE magazine, December 1982,
second place in the BYTE Game Contest). His 1982 program is kept unchanged in
`original/`; see `original/README.md` for the credit, transcription notes and
rights notice, which apply to that folder.

| Path | What it is |
|---|---|
| `index.html` | **Charge! Convergence**, the main game of this fork (below) |
| `classic/index.html` | Charge! Phosphor: land 16 shots evenly on a phosphor wall |
| `v2/index.html` | The physics lab: relativistic beam, plates and coils, lessons |
| `original/` | The 1982 Applesoft listing running in an emulator |

## Charge! Convergence

A puzzle about the central problem of a colour CRT: three beams (red, green,
blue) from an in-line gun must land on the same triad of phosphor stripes.

- The tube is 256 x 160 pixels, like the original's 280 x 160 field. Stray charges
  are placed as in lines 2080 to 2110 of the listing (odd x, even y, alternating
  sign, positive first) and pull or push with a 1/r law, as in the listing.
- Each beam leaves from its own gun position and is aimed at the point it should hit
  (static convergence by design). The charges bend the three beams by different
  amounts, so the dots on the screen come apart.
- You place **convergence strips**: rectangular magnetic zones that rotate the
  velocity of any beam crossing them (speed is unchanged, as with a real magnet).
  A dipole strip turns all three beams the same way. A quadrupole strip has a
  field that changes sign across its width, so it pulls the outer beams together
  or pushes them apart, which is how convergence magnets work.
- The test pattern on the right shows five dots, one per aim point, magnified
  3x vertically. Additive colour: where red, green and blue overlap the dot is white.
  CONV is the red, green and blue spread in pixels (2 or less to pass at every
  dot). PURITY is how far the three miss the target together (6 or less).
- Auto-place runs a simulated-annealing search for a placement (and shows it
  working). Levels use more charges and a bigger strip budget. Stars: 3 for
  using about half as many strips as there are charges, none for auto-place.

Graphics: a 320 x 200 framebuffer of 256 indexed colours with an RGB332 palette,
drawn with ordered (Bayer) dithering, additive blending done on the palette
indices, a 5 x 7 bitmap font and a cycling-colour banner. The CRT filter
(scanlines and vignette) is a CSS overlay and can be switched off.

Simplifications: the field is a 2D slice, strips are ideal uniform or linear
fields, and beam spacing is exaggerated compared with a real gun.

Open `index.html` in a browser; there are no dependencies. Keys and mouse are
listed on the page.

---

(The sections below describe the earlier games in this fork.)

A fork of **Charge!** by **C. Anthony Ray** (BYTE magazine, December 1982,
second place in the BYTE Game Contest). The 1982 program is kept unchanged in
`original/` and runs from `original/index.html`. See `original/README.md` for
the credit, the listing transcription notes and the rights notice, which apply
to everything in that folder.

`index.html` is a new game built on his physics. The field is the inside of a
cathode ray tube: the gun fires from the left, charges in the vacuum bend the
beam, and the goal is to land 16 shots evenly on the 16 bands of the phosphor
wall, not to hit one corner.

Taken from the original listing: speed 10 (`S0`), `CG = 25`, the velocity law of
lines 1010 to 1090 (launch vector plus the sum of CG * q * d / |d|^2, not
accumulated), the charge placement of lines 2070 to 2150 (odd x from 15 to 263,
even y from 14 to 142, signs alternating, positive first). New: the gun position,
the right-wall target, 16 bands, evenness scoring, burn from over-driven bands.

Scoring: evenness = 100 * (1 - (sum over bands of |hits - 1| + wall strikes) /
(2 * 16 * (1 - 1/16))). One hit in every band is 100%; all 16 in one band, or
all missing, is 0%.

Open `index.html` in a browser; it has no dependencies.

## New Version (`v2/index.html`)

Linked from a large button in the corner of the classic page. It replaces the
1982 steering law with standard physics and teaches as it goes:

- Electron speed from the anode voltage, relativistic (gamma, fraction of c).
- Electric plates and magnetic coils, integrated with a relativistic Boris
  pusher (Lorentz force) through a small 8 inch tube: gun, plates or yoke,
  neck, funnel, flat phosphor screen.
- Real effects the beam runs into: it strikes its own plates, hits the neck
  glass, overscans, and a flat screen stretches the edges by tan(theta). The
  S-correction slider undoes the tan curve (100% gives 100% even coverage).
- Magnetic deflection leaves speed unchanged (shown in the live readout).
- Phosphor model: brightness rises with beam current and anode voltage^1.5,
  spot size grows with current (simplified space-charge model), persistence,
  and burn-in where far more than a uniform share of the beam lands.
- Seven lessons, each with a "Try it" button that sets the controls.

Simplifications: the two axes are tabulated one at a time (cross-coupling and
corner effects are ignored), the anode is an instant accelerating step, and the
phosphor and space-charge models are illustrative, not measured.
