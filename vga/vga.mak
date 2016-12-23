.AUTODEPEND

#		*Translator Definitions*
TASM = d:\asm\tasm /ml/z/q/t/m5
TLIB = d:\watcom\binw\wlib -q -b
TCC = d:\watcom\binw\wcc386 -zp=1 -5s -s -zq
LIB = ..\dos\vga


#		*Implicit Rules*

.asm.obj:
  $(TASM) $<
  $(TLIB) $(LIB) -+$*

.c.obj:
  $(TCC) $<
  $(TLIB) $(LIB) -+$*


#		*List Macros*


dependencies = \
 vga.obj \
 redraw.obj \
 vgafont.obj \
 vgapal.obj \
 scrbuf.obj \
 loadvga.obj

#		*Explicit Rules*
$(LIB).lib: $(dependencies)

#		*Individual File Dependencies*

