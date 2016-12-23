#include <ale.h>

extern unsigned char uptab[256],lwtab[256];

char __chr_upr(char c);
#pragma aux __chr_upr= \
  "cmp dl,128" \
  "jae ext" \
  "cmp dl,'a'" \
  "jb x" \
  "cmp dl,'z'" \
  "ja x" \
  "xor dl,32" \
  "jmp x" \
  "ext:" \
  "mov eax,0x6520" \
  "int 0x21" \
  "x:" \
  parm [dl] \
  value [dl] \
  modify [eax]

extern "C" void init_local_conv(void) {
  int i;

  for(i=0;i<256;++i) uptab[i]=__chr_upr(i);
  for(i=0;i<256;++i) lwtab[i]=i;
  for(i=0;i<256;++i) if(uptab[i]!=i)
    if(uptab[i]>=128 || i<128)
      lwtab[uptab[i]]=i;
}
