# *Translator Definitions*
CC = wcc386 -zq -wx -d1 -s -ohrtl+ -3s -j
LINK = wlink op stack=16K


# *Implicit Rules*
.c.obj:
  $(CC) $<


# *List Macros*

EXE_dependencies =  \
  main.obj


# *Explicit Rules*

all: vga2obj3 clean

vga2obj3: $(EXE_dependencies)
  $(LINK) system dos4g name vga2obj3 file main

clean:
  rm -f $(EXE_dependencies)


# *Individual File Dependencies*

main.obj: main.c
