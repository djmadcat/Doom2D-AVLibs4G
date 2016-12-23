.AUTODEPEND

#		*Translator Definitions*
CC = d:\watcom\binw\wcc386 -zp=1 -d1 -w3 -s -oilrte -5s -j -zq -i=d:\watcom\prog\libs
PP = d:\watcom\binw\wpp386 -zp=1 -d1 -w2 -s -oilrte -5s -j -zq -i=d:\watcom\prog\libs;c:\dxsdk\inc
TASM = d:\asm\tasm /ml/z/q/t/m5/zi
LINK = d:\watcom\binw\wlink system pmodew_avl op stack=16K
FNT2OBJ = d:\watcom\binb\fnt2obj3
WLIB = d:\watcom\binw\wlib -n -b -q

#		*Implicit Rules*
.cpp.obj:
  $(PP) $<

.c.obj:
  $(CC) $<

.asm.obj:
  $(TASM) $<


#		*Explicit Rules*

all: ..\svga8.lib ..\win\svga8.lib

..\svga8.lib: svga.obj loadvga.obj vga.obj vgados.obj vgafont.obj vgapal.obj \
redraw.obj svga2.obj
  $(WLIB) ..\svga8 +svga +loadvga +vga +vgados +vgafont +vgapal +redraw +svga2

..\win\svga8.lib: svgawin.obj loadvga.obj vga.obj vgafont.obj vgapal.obj \
redraw.obj svga2.obj
  $(WLIB) ..\win\svga8 +svgawin +loadvga +vga +vgafont +vgapal +redraw +svga2

test.exe: test.obj ..\svga8.lib
  $(LINK) name test file test

#		*Individual File Dependencies*

svgawin.obj: svgawin.cpp
  $(PP) -bt=nt $*

#smallfnt.obj: smallfnt.vga
#  $(FNT2OBJ) smallfnt.vga smallfnt.obj smallfont

