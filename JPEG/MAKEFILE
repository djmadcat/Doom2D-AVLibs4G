.AUTODEPEND

#               *Translator Definitions*
CC = wcc386 -zp1 -d1 -w3 -s -oilrte -5s -j -zq -i=d:\watcom\prog\libs
PP = wpp386 -zp1 -d1 -w2 -s -oilrte -5s -j -zq -i=d:\watcom\prog\libs
LINK = wlink system pmodew_avl op stack=16K
WLIB = wlib -n -b -q

#               *Implicit Rules*
.cpp.obj:
  $(PP) $<

.c.obj:
  $(CC) $<


#               *Explicit Rules*

all: ..\jpeg.lib

..\jpeg.lib: jidctfst.obj jidctflt.obj jidctred.obj \
jcomapi.obj jdapimin.obj jdapistd.obj jdatasrc.obj jdcoefct.obj \
jdcolor.obj jddctmgr.obj jdhuff.obj jdinput.obj jdmainct.obj jdmarker.obj \
jdmaster.obj jdmerge.obj jdphuff.obj jdpostct.obj jdsample.obj jdtrans.obj \
jerror.obj jidctint.obj jmemmgr.obj jmemnobs.obj jutils.obj load.obj
  $(WLIB) ..\jpeg @&&|
+jcomapi +jdapimin +jdapistd +jdatasrc +jdcoefct +jdcolor +jddctmgr
+jdhuff +jdinput +jdmainct +jdmarker +jdmaster +jdmerge +jdphuff +jdpostct
+jdsample +jdtrans +jerror +jidctint +jmemmgr +jmemnobs +jutils +load
+jidctfst +jidctflt +jidctred
|

test.exe: test.obj ..\jpeg.lib
  $(LINK) name test file test

#               *Individual File Dependencies*
