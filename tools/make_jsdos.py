#!/usr/bin/env python3
"""Pack dos/CONVERGE.EXE into play/converge.jsdos, the bundle the browser emulator (js-dos) loads.
A .jsdos file is a zip holding the program and .jsdos/dosbox.conf. Run after every DOS build."""
import pathlib, zipfile
root = pathlib.Path(__file__).resolve().parent.parent
CONF = """[sdl]
autolock=false
[dosbox]
machine=svga_s3
memsize=8
[cpu]
core=auto
cputype=auto
cycles=fixed 26800
[mixer]
rate=44100
nosound=false
[sblaster]
sbtype=sb16
sbbase=220
irq=5
dma=1
oplmode=opl2
oplrate=44100
[autoexec]
echo off
mount c .
c:
SET BLASTER=A220 I5 D1 T6
CONVERGE.EXE
"""
out = root / 'play' / 'converge.jsdos'
with zipfile.ZipFile(out, 'w', zipfile.ZIP_DEFLATED) as z:
    z.writestr('.jsdos/dosbox.conf', CONF)
    z.write(root / 'dos' / 'CONVERGE.EXE', 'CONVERGE.EXE')
print('wrote', out, out.stat().st_size, 'bytes')
