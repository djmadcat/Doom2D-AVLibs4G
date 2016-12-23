
#ifndef __SVGAOP_LIB_H
#define __SVGAOP_LIB_H

#include <svga.h>

#define halfshade16(c) (((c)>>1)&0x7BEF)
#define halfshade15(c) (((c)>>1)&0x3DEF)
#define halfshade32(c) (((c).uc>>1)&0x7F7F7F)

color16 _svadd16(color16,color16);
#pragma aux _svadd16= \
  "mov eax,ebx" \
  "and eax,0x7E0" \
  "mov edx,ecx" \
  "and edx,0x7E0" \
  "add eax,edx" \
  "test ah,8" \
  "jz a" \
  "mov eax,0x7E0" \
  "a:" \
  "xchg eax,ebx" \
  "and eax,0xF81F" \
  "and ecx,0xF81F" \
  "add ax,cx" \
  "jnc b" \
  "mov ah,0xF8" \
  "b:" \
  "test al,0x20" \
  "jz c" \
  "mov al,0x1F" \
  "c:" \
  "or eax,ebx" \
  parm [bx] [cx] \
  modify [ebx edx ecx] \
  value [ax]

color16 _svadd15(color16,color16);
#pragma aux _svadd15= \
  "mov eax,ebx" \
  "and eax,0x3E0" \
  "mov edx,ecx" \
  "and edx,0x3E0" \
  "add eax,edx" \
  "test ah,4" \
  "jz a" \
  "mov eax,0x3E0" \
  "a:" \
  "xchg eax,ebx" \
  "and eax,0x7C1F" \
  "and ecx,0x7C1F" \
  "add eax,ecx" \
  "test ah,0x80" \
  "jz b" \
  "mov ah,0x7C" \
  "b:" \
  "test al,0x20" \
  "jz c" \
  "mov al,0x1F" \
  "c:" \
  "or eax,ebx" \
  parm [bx] [cx] \
  modify [ebx edx ecx] \
  value [ax]

#define _svadd8(a,b) (((svga_drv8*)svga)->mtab[((a)<<8)|(b)])

#define _svadd24(a,b) (((a)+(b)>255)?255:(a)+(b))

#endif
