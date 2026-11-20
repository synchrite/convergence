# Charge! Phosphor

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
