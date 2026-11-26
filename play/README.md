# Browser DOS emulator

These files run `dos/CONVERGE.EXE` on the main page.

| File | What it is |
|---|---|
| `converge-<id>.jsdos` | The DOS program and a DOSBox configuration, zipped. Made by `tools/make_jsdos.py`, which names the file after its contents and points `index.html` at it, so an old copy is never loaded |
| `js-dos.js`, `js-dos.css` | js-dos 8.5.4, the player |
| `emulators/` | DOSBox compiled to WebAssembly, from the same package |

js-dos and its emulators are by Alexander Guryanov (caiiiycuk) and are
licensed under the GNU General Public License, version 2. They are copied
unmodified from the npm package `js-dos` 8.5.4. Source:
https://github.com/caiiiycuk/js-dos and https://github.com/caiiiycuk/emulators.
DOSBox is by the DOSBox Team, also GPL version 2.

The emulated machine: S3 SVGA with VESA, 8 MB, Sound Blaster 16 at A220 I5 D1
with OPL2, fixed 26800 cycles (about a 486). The page must be served over
http or https; it does not work from a `file://` address.
