.AUTODEPEND

#		*Translator Definitions*
CC = d:\watcom\binw\wcc386 -zp=1 -s -olrt -5s -zq -j -i=d:\watcom\prog\libs
PP = d:\watcom\binw\wpp386 -zp=1 -s -olrt -5s -zq -j -i=d:\watcom\prog\libs
TASM = d:\ASM\TASM /ml/z/q/t/m5
LINK = d:\watcom\binw\wlink system pmodew_avl op stack=16K
WLIB = d:\watcom\binw\wlib -b -q

VGA2OBJ = vga2obj3
FNT2OBJ = fnt2obj3


#		*Implicit Rules*
.c.obj:
  $(CC) $<

.cpp.obj:
  $(PP) $<

.asm.obj:
  $(TASM) {$< }


#		*List Macros*


LIB_depend =  \
  gui.obj \
  input.obj \
  smallfnt.obj

#		*Explicit Rules*

test.exe: ..\dos\gui.lib main.obj cursor.obj
  $(LINK) file main,cursor name test

..\dos\gui.lib: $(LIB_depend)
  $(WLIB) -n ..\dos\gui.lib +gui +input +smallfnt

#		*Individual File Dependencies*

smallfnt.obj: smallfnt.vga 
	$(FNT2OBJ) smallfnt.vga smallfnt.obj smallfont

cursor.obj: cursor.vga 
	$(VGA2OBJ) cursor.vga cursor.obj mainpal

