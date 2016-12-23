.AUTODEPEND

#               *Translator Definitions*
CC = wcc386 -zp=1 -d1 -w3 -s -oilert -5s -j -zq -i=..
PP = wpp386 -zp=1 -d1 -w2 -s -oilert -5s -j -zq -i=..
TASM = tasm /ml/z/q/t/m5
RES = wrc
WLIB = wlib -n -b -q

#               *Implicit Rules*
.cpp.obj:
  $(PP) $<

.c.obj:
  $(CC) $<

.asm.obj:
  $(TASM) $<


#               *Explicit Rules*

all: ..\vid.lib

..\vid.lib: decode.obj
  $(WLIB) ..\vid +decode



