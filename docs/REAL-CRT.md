# How the game relates to a real CRT

The game is inspired by the original *Charge!* by C. Anthony Ray (BYTE, December
1982). This note says which parts of it are true of a real colour tube, which
are simplified, and how the flat picture maps onto a three-dimensional set.

## The slice

An in-line colour tube has three guns side by side in a horizontal row: blue,
green, red. The game shows the horizontal plane through those three guns, seen
from above. So "up" and "down" in the playfield are left and right on the real
screen, and the game covers **horizontal** convergence.

A real set also has vertical convergence. The physics is the same, turned
through 90 degrees. That is why each real ring magnet has two adjustments:

| Real adjustment | What it does | In the game |
|---|---|---|
| Turn both tabs of a ring together | Chooses the **direction** of the shift | Level 5: SQUEEZE DIR and SHIFT DIR. Fixed along the gun row before that |
| Spread the two tabs apart | Chooses the **strength** | The ring value, -6 to +6 |

Level 5 brings in the second direction through the rings; see below.

## What is true to life

- **Three beams, green is the reference.** Nothing in the game moves green
  relative to the others except the purity ring, which moves all three.
- **The rings and what they do.** Four-pole ring: red and blue in opposite
  directions (the game's squeeze ring). Six-pole ring: red and blue together
  (shift ring). Two-pole purity ring: all three beams.
- **Static before dynamic.** Rings act at the neck, before deflection, so they
  change every part of the screen by the same amount. They are set on the
  centre of the screen. What is left at the edges is dynamic error, corrected
  near the yoke and funnel: yoke tilt, wedges, and magnetic strips.
- **Strips go on the glass.** A convergence strip is a small magnet stuck to the
  outside of the funnel, between the yoke and the screen. It is slid along the
  glass while the picture is watched, flipped to change which pole faces the
  tube, and left stuck down. The game has one kind of strip, of one size. The game's strips
  work the same way, on the top or bottom glass of the slice.
- **A strip acts most on the nearest beam.** Its pull dies away quickly with
  distance. On the top glass it moves red most, on the bottom glass blue. Near
  the yoke, where the glass is close to the beams, it does a great deal; near
  the screen very little.
- **Lever arm.** A bend applied early has more distance to act over, so it
  moves the landing point further.
- **Measure first.** The gap between the outer beams is read as a number, and
  the effect of a strip at each place can be tabulated before it is stuck down.
- **Why convergence errors exist.** A field that is the same across all three
  beams moves them together (a purity error). Only a field that is stronger on
  one side separates them. That is why the game's fault parts sit near the neck
  and yoke, where the beams are far apart, and why distant parts mostly shift
  the picture. This fell out of the simulation, not out of the design.
- **The mirror.** The adjustments are at the back and the picture is at the
  front, so the work is done by looking at a mirror. The picture is reversed.
- **The order of work.** Degauss, purity, static convergence, repeat, then
  dynamic convergence.
- **A magnet does not change the beam's speed**, only its direction.

## Level 4: the real procedure

Level 4 is an alignment done in the order a technician does it. The job list at
the top of the screen ticks off as each part comes right.

| Step | On a real set | In the game |
|---|---|---|
| 1. Degauss | A magnetised shadow mask spoils purity in patches that no adjustment cures. Degauss before anything else | Stage 4-2. Press D. Until then the red field has patches |
| 2. Purity, centre | On a red field, turn the purity rings until the centre is pure red | Stage 4-1. Red field in the mirror, purity ring |
| 3. Purity, edges | Slide the yoke along the neck until the whole field is red, then clamp it | Stage 4-3. Yoke slide. It acts in proportion to distance from the centre |
| 4. Static convergence | On dots or a crosshatch, four-pole rings bring red and blue together, six-pole rings lay them on green, at the centre | Squeeze and shift rings, centre picture |
| 5. Dynamic convergence | Tilt the yoke until the edges converge, and wedge it | Stage 4-4. Yoke tilt. Red and blue cross opposite ways at the two edges, which is how the fault is recognised |
| 6. What is left | Magnetic strips on the glass | Strips, last |
| 7. Repeat | Purity and convergence disturb each other | Sliding the yoke changes the edges' convergence a little, so purity is done first |

Each fault shows only on the pattern a technician would use: purity on the red
field, convergence on the dots. The advice line at the bottom follows the same
reasoning: centre before edges, rings before yoke before strips.

The job ends as a real one does: press K to wedge the yoke and seal the rings
with paint. After that nothing turns. Breaking the seal to correct a mistake
costs a star, as scraping paint off a ring costs time on a bench.

## Level 5: the second direction

The playfield is still the slice through the gun row. The other direction,
across the row, is carried by the rings:

- Each convergence ring (squeeze and shift) has a **direction** as well as a
  **strength**: 0, 45, 90 or 135 degrees. This is the real pair of adjustments:
  turn both tabs together for direction, spread them for strength.
- The part of a ring's effect that lies along the gun row bends the beams in
  the playfield. The part that lies across it shows in the mirror as a sideways
  split.
- A knocked ring is cancelled by setting the same direction and the opposite
  strength.
- Purity has no second direction here, and that is true of a real in-line tube:
  the phosphor is in vertical stripes, so landing a little high or low does not
  change the colour.

Two techniques from the bench come with it:

- **Crosshatch.** A third test pattern. A coloured edge on the level lines is an
  error along the gun row; on the upright lines, across it.
- **Gun switches.** Switch green off (key 8) and red and blue alone should make
  one magenta picture: that is the squeeze (four-pole) adjustment by itself.
  Switch green on again and lay the magenta on it: that is the shift (six-pole)
  adjustment.

Still simplified in the second direction: magnetised parts, strips and the
yoke act only along the gun row. A second full playfield (the side view) would
be needed to give them a second direction too.

Not modelled yet: warm-up drift, and a second playfield for the side view.

## What is simplified

- **Strip fields.** A strip is treated as a single pole on the glass whose pull
  falls off with the square of distance. A real strip is a short bar magnet
  with a messier field, and where it sits around the neck matters as well as
  how far along the funnel.
- **Strip turning.** Turning a strip is modelled only as losing strength (to
  seven tenths at 45 degrees, four tenths at 90).
- **Ring interaction.** Real rings interact, and the work is repeated several
  times. The game's rings are independent.
- **Deflection.** There is no scanning. Each picture is one fixed beam direction.
- **Scale.** Beam spacing and bend angles are exaggerated so they can be seen.
- **Fault parts.** Real causes are more diffuse than a point with a 1/r squared field.

## Safety

Everything described here is inside the cabinet, beside lethal voltages. The
game is the place to practise it. On a real set, use the controls outside the
case unless you are trained.
