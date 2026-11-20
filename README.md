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
