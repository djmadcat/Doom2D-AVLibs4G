.AUTODEPEND

#               *Translator Definitions*
CC = wcc386 -zp=1 -d1 -w3 -s -oilert -5s -j -zq -i=..
PP = wpp386 -zp=1 -d1 -w2 -s -oilert -5s -j -zq -i=..
TASM = tasm /ml/z/q/t/m5
LINK = wlink system pmodew_avl op stack=16K d a
LINKW = wlink system nt_win_avl op stack=16K d a
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

all: playerd.exe player.exe

playerd.exe: player.obj
  $(LINK) name playerd file player

playerw.obj: player.cpp
  $(PP) -bt=nt -i=c:\dxsdk\inc player.cpp -fo=playerw.obj

player.exe: plyrw.obj playerw.obj
  $(LINKW) library ddraw,dinput,dxguid name player file playerw,plyrw
  $(RES) /q /bt=nt /r player.rc /fo=player.res
  $(RES) /q player.res player.exe

plyrw.obj: plyrw.cpp
  $(PP) -bt=nt -i=c:\dxsdk\inc $*

#               *Individual File Dependencies*

#smallfnt.obj: smallfnt.vga
#  $(FNT2OBJ) smallfnt.vga smallfnt.obj smallfont

