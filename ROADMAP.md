# Roadmap

Charge! Convergence is inspired by the original *Charge!* by C. Anthony Ray
(BYTE, December 1982). This file records where the project is meant to go.
Nothing below is built unless it is listed under "Done".

## Done

- Browser game: tutorial, 30 stages in three levels, strips, neck rings,
  title screen, music.
- DOS edition: service bench (geometry, focus, black level, purity, static and
  dynamic convergence, degauss), test patterns for a real monitor, handbook.

## Direction (owner, 2026-11-20)

The game should grow into a teaching tool for CRT repair as a whole, not only
alignment:

1. **CRT repair fundamentals.** How a set works block by block: power supply,
   horizontal and vertical deflection, high voltage and flyback, video and
   chroma, the tube and its neck board.
2. **Basic electronics.** Voltage, current, resistance, Ohm's law, series and
   parallel, capacitors and inductors, diodes, transistors.
3. **Tracing bad components.** From a symptom on the screen to a stage, from the
   stage to a part: signal tracing, voltage tables, half-splitting.
4. **Using an oscilloscope.** Timebase, volts per division, triggering, probes,
   reading the waveforms at the usual test points.
5. **Using a multimeter.** Volts, ohms, continuity, diode test, in-circuit
   against out-of-circuit readings.
6. **Finding shorts.** Resistance to ground, a series lamp or current-limited
   supply, dividing the rail, heat.
7. **Replacing chips.** Identifying the part, desoldering, sockets, orientation,
   static precautions.

## Sketch of how it could fit the game

- A **bench mode** beside the alignment game: a schematic and a board view of a
  faulty set, with a probe the player places on test points.
- **Instruments as tools**: a multimeter and an oscilloscope with working
  controls, showing what the simulated circuit would show.
- **Faults as puzzles** with the same structure as the alignment stages: a
  symptom, tools, a diagnosis line, and a clear moment when the fault is found.
- A **second view** for the convergence game: the vertical slice, so rings gain
  their real second adjustment (direction as well as strength).
- **Safety taught first**, as a required stage: discharge, isolation, one hand.

## Open questions

- Which real chassis to model, and how closely.
- How much circuit simulation is needed for believable meter and scope readings.
- Whether the DOS edition follows, or stays an alignment and test-pattern tool.
