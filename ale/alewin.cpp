#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ale.h>
#include "dll.h"

extern unsigned char uptab[256],lwtab[256];

extern "C" DLLEXPORT void init_local_conv(void) {
  int i;

  for(i=0;i<256;++i) uptab[i]=(unsigned char)CharUpper((LPSTR)i);
  for(i=0;i<256;++i) lwtab[i]=(unsigned char)CharLower((LPSTR)i);
}
