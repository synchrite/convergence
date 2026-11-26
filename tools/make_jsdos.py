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
# Each build gets its own file name. A browser, or the emulator's own storage, cannot then serve an old game
# under the new page: the page names the exact file it wants.
import hashlib, re, datetime
exe = (root / 'dos' / 'CONVERGE.EXE').read_bytes()
tag = hashlib.sha1(exe + CONF.encode()).hexdigest()[:8]
for old in (root / 'play').glob('converge*.jsdos'): old.unlink()
out = root / 'play' / ('converge-%s.jsdos' % tag)
with zipfile.ZipFile(out, 'w', zipfile.ZIP_DEFLATED) as z:
    z.writestr(zipfile.ZipInfo('.jsdos/dosbox.conf', (2026, 1, 1, 0, 0, 0)), CONF)
    z.writestr(zipfile.ZipInfo('CONVERGE.EXE', (2026, 1, 1, 0, 0, 0)), exe, zipfile.ZIP_DEFLATED)
page = root / 'index.html'; h = page.read_text()
h2 = re.sub(r"play/converge[-0-9a-f]*\.jsdos", 'play/' + out.name, h)
h2 = re.sub(r'<span id="build">[^<]*</span>', '<span id="build">Build %s, %s</span>' % (tag, datetime.date.today().isoformat()), h2)
if h2 != h: page.write_text(h2)
print('wrote', out, out.stat().st_size, 'bytes; index.html points at it')
