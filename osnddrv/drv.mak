.AUTODEPEND

#		*Translator Definitions*
TASM = d:\asm\tasm /ml/z/q/t/m5
TLIB = d:\watcom\binw\wlib -q -b
TCC = d:\watcom\binw\wcc386 -zp=1 -5s -s -zq
LIB = ..\dos\osnddrv


#		*Implicit Rules*

.asm.obj:
  $(TASM) $<
  $(TLIB) $(LIB) -+$*

.c.obj:
  $(TCC) $<
  $(TLIB) $(LIB) -+$*


#		*List Macros*


dependencies = \
 none.obj \
 adlib.obj \
 covox.obj \
 pc1.obj \
 pc8.obj \
 sb.obj \
 sb16.obj \
 sbnodma.obj \
 drvtab.obj

#		*Explicit Rules*
$(LIB).lib: $(dependencies)

#		*Individual File Dependencies*

