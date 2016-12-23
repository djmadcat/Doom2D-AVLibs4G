#		*Translator Definitions*

TASM = d:\asm\tasm /ml/z/q/t/m5
TLIB = d:\watcom\binw\wlib -q -b
TCC = d:\watcom\binw\wcc386 -zp=1 -5s -olrt -s -zq
LIB = ..\osound2


#		*Implicit Rules*

.asm.obj:
  $(TASM) $<
  $(TLIB) $(LIB) -+$*

.c.obj:
  $(TCC) $<
  $(TLIB) $(LIB) -+$*


#		*List Macros*


dependencies = \
 adlib.obj \
 covox.obj \
 none.obj \
 pc1.obj \
 pc8.obj \
 sb.obj \
 sbnodma.obj \
 sbpro.obj \
 sb16.obj \
 drvtab.obj \
 timer.obj \
 sound.obj \
 loadsnd.obj

#		*Explicit Rules*
$(LIB).lib: $(dependencies)

#		*Individual File Dependencies*

