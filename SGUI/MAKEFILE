.AUTODEPEND

#		*Translator Definitions*
CC = d:\watcom\binw\wcc386 -zp=1 -s -olrt -4s -zq -j -i=d:\watcom\prog\libs
PP = d:\watcom\binw\wpp386 -zp=1 -s -oilrte -d1 -5s -zq -j -i=d:\watcom\prog\libs;c:\dxsdk\inc
TASM = d:\ASM\TASM /ml/z/q/t/m5
LINK = d:\watcom\binw\wlink system pmodew_avl op stack=16K
WLIB = d:\watcom\binw\wlib -b -q

VGA2OBJ = d:\watcom\binw\vga2obj3
FNT2OBJ = d:\watcom\binw\fnt2obj3


#		*Implicit Rules*
.c.obj:
  $(CC) $<

.cpp.obj:
  $(PP) $<

.asm.obj:
  $(TASM) {$< }


#		*Explicit Rules*

all: ..\sgui.lib ..\win\sgui.lib test.exe

test.exe: ..\sgui.lib main.obj cursor.obj smallfnt.obj
  $(LINK) file main,cursor,smallfnt name test

..\sgui.lib: gui.obj input.obj sguidos.obj
  $(WLIB) -n ..\sgui.lib +gui +input +sguidos

..\win\sgui.lib: gui.obj sguiwin.obj
  $(WLIB) -n ..\win\sgui.lib +gui +sguiwin

#		*Individual File Dependencies*

sguiwin.obj: sguiwin.cpp
  $(PP) -bt=nt $*

smallfnt.obj: smallfnt.vga 
	$(FNT2OBJ) smallfnt.vga smallfnt.obj smallfont

cursor.obj: cursor.vga 
	$(VGA2OBJ) cursor.vga cursor.obj mainpal

