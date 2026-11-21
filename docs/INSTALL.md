# Installing and running

Charge! Convergence is inspired by the original *Charge!* by C. Anthony Ray
(BYTE, December 1982). This file covers every way to run it.

## 1. Playing in a browser

Nothing to install. All browser editions are single HTML files with no
dependencies and no network requests.

| Open | To play |
|---|---|
| `index.html` | Charge! Convergence: the Convergence Bench (main page) |
| `inside/index.html` | Inside the tube, the beam-path game |
| `v2/index.html` | The physics lab |
| `classic/index.html` | Charge! Phosphor |
| `original/index.html` | The 1982 original in an emulator |

- **From a copy of the repository:** double-click the file. It works from a
  `file://` address.
- **From GitHub Pages:** in the repository settings, Pages, set Source to
  "Deploy from a branch", branch `main`, folder `/ (root)`. The site is then at
  `https://<owner>.github.io/<repo>/`, with the other editions under `/v2/`,
  `/classic/` and `/original/`.
- Sound in `original/` starts after the first click, because browsers block
  audio until then. Best scores and settings are kept in the browser's local
  storage on that device only.

The DOS edition does not run in the browser as shipped. It runs in a DOS
emulator (section 3). An in-browser DOS emulator such as js-dos could host
`CONVERGE.EXE`, but that page is not part of this repository.

## 2. DOS edition on a real PC

### Requirements

| | Minimum |
|---|---|
| CPU | 386; intended for a 486. No coprocessor needed (486SX is fine) |
| Memory | 4 MB |
| Video | SVGA card with a VESA BIOS: 640x480, 256 colours |
| Sound | Optional: AdLib or compatible (music), Sound Blaster (digitised effects). PC speaker otherwise |
| DOS | MS-DOS 5 or later, PC DOS, DR-DOS or FreeDOS |
| Disk | 150 KB for the program, plus a little for the save file and screenshots |

### Installing

It is one file. Copy `dos/CONVERGE.EXE` to any directory and run it:

    C:\>MD CONVERGE
    C:\>COPY A:\CONVERGE.EXE C:\CONVERGE
    C:\>CD CONVERGE
    C:\CONVERGE>CONVERGE

No other files are needed. The DOS extender is inside the EXE.

For Sound Blaster effects, the `BLASTER` variable must be set, normally in
`AUTOEXEC.BAT`:

    SET BLASTER=A220 I5 D1

The program does not touch the Sound Blaster ports without it. Use an 8-bit DMA
channel (0, 1 or 3) and an IRQ below 8 (5 or 7 are usual); the effects are
8-bit, so the high DMA of a 16-bit card is not used. AdLib music is detected
by itself at port 388h.

If the program says no VESA BIOS was found, load the VESA driver that came with
the video card (or UNIVBE) and try again. If the picture is wrong on an older
card, start with `CONVERGE /BANK`.

Switches: `/BANK`, `/NOSOUND`, `/NOSB`, `/NOADLIB`. Any other argument prints help.

### Writing to disk: getting the file onto the PC

`CONVERGE.EXE` is about 176 KB, so it fits on any floppy.

**Floppy disk, from Windows.** Format a 1.44 MB (or 720 KB) disk as FAT and copy
the file to it. A USB floppy drive works.

**Floppy disk, from Linux or macOS.** Either mount the FAT floppy and copy the
file, or use mtools:

    mformat -f 1440 a:
    mcopy dos/CONVERGE.EXE a:

**Floppy image** (for a Gotek or other floppy emulator, or an emulator):

    mformat -C -f 1440 -i convergence.img ::
    mcopy -i convergence.img dos/CONVERGE.EXE ::
    mdir -i convergence.img ::

Copy `convergence.img` to the Gotek's USB stick (as `DSKA0000.HFE` or `.IMG`
according to its firmware), or write it to a real disk:

    dd if=convergence.img of=/dev/fd0 bs=512

**CompactFlash or SD card on an IDE adapter.** Take the card out, put it in a
modern reader, and copy the file into a directory. Keep the card FAT16 as DOS
formatted it; do not reformat it on the modern machine.

**Serial or parallel cable.** Use INTERLNK/INTERSVR (MS-DOS 6), LapLink, or a
terminal program with ZMODEM over a null-modem cable.

**Network.** With a packet driver and mTCP, `FTP` or `HTGET` the file.

**CD-R.** Burn the file as an ISO 9660 disc if the PC has a CD-ROM drive and
MSCDEX loaded.

### What the program writes to disk

It writes only to the directory it is started from:

| File | When | Size |
|---|---|---|
| `CONVERGE.SAV` | When stars are earned or Setup changes | 15 bytes |
| `BENCH.BMP`, `MENU.BMP`, `REPORT.BMP`, `PATTERN.BMP`, and similar | When F12 is pressed | 308 KB each at 640x480 |

If the disk is write-protected or full, saving fails silently and the game
carries on; progress is then lost when you exit. Run it from the hard disk, or
from a floppy with the write-protect tab closed, to keep progress. To reset
progress, delete `CONVERGE.SAV`.

It changes nothing else: no `CONFIG.SYS` or `AUTOEXEC.BAT` edits, no files
outside its directory.

## 3. DOS edition in an emulator

**DOSBox-X** (the emulator it was tested in) or **DOSBox Staging**:

    mount c /path/to/repository/dos
    c:
    CONVERGE

Suggested settings for a 486 feel, in the emulator's `.conf`:

    [dosbox]
    machine=svga_s3
    memsize=8
    [cpu]
    cputype=486
    cycles=fixed 26800
    [sblaster]
    sbtype=sb16
    sbbase=220
    irq=5
    dma=1
    oplmode=opl2

DOSBox sets `BLASTER` by itself. **86Box** and **PCem** emulate specific 486
boards and cards and are closer to real hardware; use a floppy image made as
above.

## 4. Controls, DOS edition

| Key | Service bench |
|---|---|
| TAB | Next control group |
| Up, Down | Choose a control (or a strip position) |
| Left, Right | Turn the control one step (or add and remove strips) |
| PgUp, PgDn | Four steps |
| F1 to F5 | Test pattern on the simulated set |
| D | Degauss |
| H | Hint (costs points) |
| I | Inspect (Journeyman level) |
| M | Music on or off |
| ENTER | Ship the set to the customer |
| ESC | Give up the job |
| F12 | Save a screenshot |

| Key | Test patterns for your monitor |
|---|---|
| Left, Right, Space | Change pattern |
| R, G, B | Switch a gun off and on |
| + and - | 640x480, 800x600, 1024x768 |
| F1 | Show or hide the notes |
| ESC | Back |
