#!/bin/bash
# probe.sh - "minimal compile probe" for the miniOS MiniGUI port.
#
# Compiles core MiniGUI modules with the EXACT miniOS user-program flags
# (gcc -m32 -nostdinc -nostdlib -fno-builtin -mno-sse ...) plus the
# minios-compat/ header shims + mgconfig_minios.h, then reports how many
# translation units build and which external symbols stay undefined
# (= the libc/system dependency surface). It does NOT link or run.
#
# Assumes layout:  <parent>/minigui-port/  and  <parent>/minios-master/
set -u
MG="$(cd "$(dirname "$0")/.." && pwd)"          # minigui-port root
OS="$(cd "$MG/../minios-master" && pwd)"        # miniOS tree
COMPAT="$MG/minios-compat/include"
WORK="${TMPDIR:-/tmp}/mgprobe-work"; OBJ="$WORK/obj"
rm -rf "$WORK"; mkdir -p "$OBJ"

# make `#include "mgconfig.h"` resolve to our curated config
printf '#include "%s/mgconfig_minios.h"\n' "$MG" > "$WORK/mgconfig.h"

FLAGS="-m32 -std=gnu99 -nostdinc -nostdlib -fno-pie -fno-builtin \
-fno-stack-protector -mno-sse -mno-sse2 -mno-mmx -w -c"
INC="-I$WORK -I$COMPAT -I$MG -I$MG/include -I$MG/src/include \
-I$MG/src/newgal -I$MG/src/newgdi -I$MG/src/ial -I$MG/src/font -I$MG/src/kernel \
-I$OS/include/ulib -I$OS/generic/include -I$OS/include"

FILES=$(ls $MG/src/misc/*.c $MG/src/kernel/*.c $MG/src/newgdi/*.c $MG/src/gui/*.c \
  $MG/src/standalone/*.c $MG/src/newgal/commlcd/commlcd.c \
  $MG/src/newgal/commlcd/minios_fb.c $MG/src/newgal/commlcd/extern.c \
  $MG/src/ial/comminput.c $MG/src/ial/minios_input.c $MG/src/ial/extern.c \
  $MG/src/sysres/*.c 2>/dev/null)

ok=0; fail=0
for f in $FILES; do
  o="$OBJ/$(echo "$f" | tr '/' '_').o"
  if gcc $FLAGS $INC "$f" -o "$o" 2>>"$WORK/build.log"; then ok=$((ok+1));
  else fail=$((fail+1)); echo "FAIL $f" >>"$WORK/build.log"; fi
done
echo "compiled .o: $ok   failed: $fail"

nm --defined-only "$OBJ"/*.o 2>/dev/null | awk '{print $NF}' | sort -u > "$WORK/defined.txt"
nm -u             "$OBJ"/*.o 2>/dev/null | awk '{print $NF}' | sort -u > "$WORK/undef.txt"
comm -23 "$WORK/undef.txt" "$WORK/defined.txt" > "$WORK/extern.txt"
echo "external undefined symbols: $(wc -l < "$WORK/extern.txt")  (see $WORK/extern.txt)"
echo "build log: $WORK/build.log"
