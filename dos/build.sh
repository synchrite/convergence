#!/bin/sh
# Build CONVERGE.EXE with Open Watcom v2 (https://github.com/open-watcom/open-watcom-v2).
# Set WATCOM to the install directory. The CauseWay DOS extender is bound into the EXE,
# so the result is one file with no other runtime needed.
set -e
: "${WATCOM:?set WATCOM to your Open Watcom directory}"
export PATH="$WATCOM/binl64:$WATCOM/binl:$WATCOM/binw:$PATH"
export INCLUDE="$WATCOM/h"
cd "$(dirname "$0")"
mkdir -p obj
wcl386 -q -bt=dos -l=causeway -4r -oneatx -zq -w3 -za99 -fo=obj/ -fe=CONVERGE.EXE \
  src/main.c src/vid.c src/sys.c src/snd.c src/crt.c src/text.c src/patterns.c src/game.c src/cbench.c src/sintab.c
ls -l CONVERGE.EXE
