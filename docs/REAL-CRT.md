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
| Turn both tabs of a ring together | Chooses the **direction** of the shift | Fixed: the slice has one direction |
| Spread the two tabs apart | Chooses the **strength** | The ring value, -6 to +6 |

A second view (the vertical slice) with the same rules would complete the
picture; see the roadmap.

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
- **A strip only affects what passes it.** A strip near the screen changes the
  pictures whose beams go through its field and leaves the rest.
- **Lever arm.** A bend applied early has more distance to act over, so it
  moves the landing point further.
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

## What is simplified

- **Strips in the vacuum.** Real strips are stuck to the outside of the glass,
  under the yoke. Here you place them over the beams, as if looking through
  the glass at the spot the strip acts on.
- **Strip fields.** A strip is an ideal zone: a uniform field (mover) or a field
  that changes sign across its width (squeezer). A real strip has a messier
  field and affects neighbouring areas.
- **Ring interaction.** Real rings interact, and the work is repeated several
  times. The game's rings are independent.
- **Deflection.** There is no scanning. Each picture is one fixed beam direction.
- **Scale.** Beam spacing and bend angles are exaggerated so they can be seen.
- **Fault parts.** Real causes are more diffuse than a point with a 1/r squared field.

## Safety

Everything described here is inside the cabinet, beside lethal voltages. The
game is the place to practise it. On a real set, use the controls outside the
case unless you are trained.
