#!/bin/bash
# build_lib.sh - compile the whole libminigui.a for the miniOS port.
#
# Compiles the curated per-subdir source set (standalone + commlcd GAL + comm
# IAL, incore fonts, no pixman / TTF / image loaders) with the exact miniOS
# freestanding flags + minios-compat headers + mgconfig_minios.h, then archives
# every object plus the four compat translation units into libminigui.a.
#
# Layout assumed:  <parent>/minigui-port/  and  <parent>/minios-master/
set -u
MG="$(cd "$(dirname "$0")/.." && pwd)"
OS="$(cd "$MG/../minios-master" && pwd)"
COMPAT="$MG/minios-compat/include"
WORK="${TMPDIR:-/tmp}/mglib"; OBJ="$WORK/obj"
rm -rf "$WORK"; mkdir -p "$OBJ"
printf '#include "%s/mgconfig_minios.h"\n' "$MG" > "$WORK/mgconfig.h"

FLAGS="-m32 -std=gnu99 -nostdinc -nostdlib -fno-pie -fno-builtin \
-fno-stack-protector -mno-sse -mno-sse2 -mno-mmx -w -c"
INC="-I$WORK -I$COMPAT -I$MG -I$MG/include -I$MG/src/include \
-I$MG/src/newgal -I$MG/src/newgdi -I$MG/src/ial -I$MG/src/font \
-I$MG/src/kernel -I$MG/src/control -I$MG/src/gui \
-I$OS/include/ulib -I$OS/generic/include -I$OS/include"

ok=0; fail=0; faillist=""
cc() {   # cc <src-abs>
    local f="$1" o
    o="$OBJ/$(echo "$f" | sed "s#$MG/##; s#/#_#g").o"
    if gcc $FLAGS $INC "$f" -o "$o" 2>>"$WORK/build.log"; then
        ok=$((ok+1))
    else
        fail=$((fail+1)); faillist="$faillist ${f#$MG/src/}"
        echo "FAIL ${f#$MG/src/}" >>"$WORK/build.log"
    fi
}

# --- subdirs where we compile every *.c minus an explicit exclude set ---
# (each engine/feature file is internally #ifdef-guarded; disabled -> empty .o)
compile_glob() {   # compile_glob <subdir> <excluded basenames...>
    local d="$1"; shift
    local ex=" $* "
    for f in "$MG"/src/"$d"/*.c; do
        [ -f "$f" ] || continue
        case "$ex" in *" $(basename "$f") "*) continue;; esac
        cc "$f"
    done
}

# clean subdirs (exclude EXTRA_DIST / other-runmode / other-RTOS / obsolete)
compile_glob libc
compile_glob misc      drawsemop.c rc4.c misc-dt.c
compile_glob main
compile_glob kernel    desktop.c desktop-comm.c cursor-comm.c timer_posix.c
compile_glob sysres    mgetc_win32.c
# incore GUI resources (bmp/gif/icon/cursor). The _*_inner_res.c aggregates are
# #included by incoreres.c, so compile only the individual data files here.
# sysres/font is NOT built: incore fonts come from src/font/in-core (no _mgir_
# font symbols are referenced).
compile_glob sysres/bmp    _bmp_inner_res.c
compile_glob sysres/icon   _icon_inner_res.c
compile_glob sysres/cursor
compile_glob control
compile_glob ex_ctrl
compile_glob gui       transkey.c
compile_glob textedit
compile_glob mybmp      jpeg.c
compile_glob font       sysfont.old.c unicode-shape-complex-indic-table.c convgbmap.c
compile_glob font/in-core
compile_glob newgdi     glyph-old.c yuv.c
compile_glob standalone

# newgal core (top-level) minus optional; plus commlcd engine only
compile_glob newgal     gamma.c yuv.c yuv_sw.c
for f in commlcd extern minios_fb; do cc "$MG/src/newgal/commlcd/$f.c"; done

# IAL: core + comm engine + miniOS device backend (self-contained engine table)
for f in ial comminput minios_input; do cc "$MG/src/ial/$f.c"; done

# the compat translation units
for f in mg_stdio mg_libm mg_time mg_libc; do cc "$MG/minios-compat/src/$f.c"; done

echo "compiled .o: $ok   failed: $fail"
[ -n "$faillist" ] && echo "FAILURES:$faillist"

ar rcs "$WORK/libminigui.a" "$OBJ"/*.o 2>/dev/null
echo "archive: $WORK/libminigui.a  ($(ar t "$WORK/libminigui.a" 2>/dev/null | wc -l) objects)"
echo "build log: $WORK/build.log"
