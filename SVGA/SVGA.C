#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include "svga.h"

#define VESAOK 0x4F

unsigned sv_mode=0xFFFFFFFF;
unsigned sv_mem=256*1024;
static unsigned gran=64,step=1;
unsigned sv_bpl;
static char vesa=0,stdvga=0;
char use_vbe2=1,use_linear=1,vbe2=0,linear=0;
static unsigned char *scrptr=NULL;
unsigned char *scrbuf=NULL;
static void *vbe2_0=NULL,*vbe2_1=NULL,*vbe2_2=NULL;
static void *vbe2code=NULL;

unsigned sv_width,sv_height,sv_bits,sv_pgbank,sv_pgofs,sv_pagesz,sv_bytespp;

int sv_curx,sv_cury,sv_step=1,sv_space;
ucolor sv_prclr=0xFFFFFFFF;
image **sv_font;

void vbe2_setbank(int);
#pragma aux vbe2_setbank= \
  "xor ebx,ebx" \
  "call [vbe2_0]" \
  parm [edx] \
  modify [eax edx ebx]

void vbe2_setpage(int);
#pragma aux vbe2_setpage= \
  "mov ebx,0x80" \
  "mov edx,ecx" \
  "shr edx,16" \
  "call [vbe2_1]" \
  parm [ecx] \
  modify [eax edx ebx]

void vbe2_setpal(void *,int,int);
#pragma aux vbe2_setpal= \
  "push es" \
  "push ds" \
  "pop es" \
  "mov ebx,0x80" \
  "call [vbe2_2]" \
  "pop es" \
  parm [edi] [edx] [ecx] \
  modify [eax ebx]

void vga_setpal(void *,int,int);
#pragma aux vga_setpal= \
  "mov dx,0x3DA" \
  "w1: in al,dx" \
  "test al,8" \
  "jnz w1" \
  "w2: in al,dx" \
  "test al,8" \
  "jz w2" \
  "mov dx,0x3C8" \
  "mov al,bl" \
  "out dx,al" \
  "inc dx" \
  "cld" \
  "mov eax,ecx" \
  "add ecx,ecx" \
  "add ecx,eax" \
  "s1: lodsb" \
  "out dx,al" \
  "nop" \
  "nop" \
  "nop" \
  "loop s1" \
  parm [esi] [ebx] [ecx] \
  modify [eax ecx edx esi]

#pragma pack(1)

static struct{
    unsigned long EDI;
    unsigned long ESI;
    unsigned long EBP;
    unsigned long reserved_by_system;
    unsigned long EBX;
    unsigned long EDX;
    unsigned long ECX;
    unsigned long EAX;
    unsigned short flags;
    unsigned short ES,DS,FS,GS,IP,CS,SP,SS;
}RMI;

void simint(int);
#pragma aux simint= \
  "mov eax,0x300" \
  "mov ecx,0" \
  "push ds" \
  "pop es" \
  "lea edi,RMI" \
  "int 0x31" \
  parm caller [ebx] \
  modify [eax ecx edi]

static unsigned short dosseg;

unsigned short getdosmem(int);
#pragma aux getdosmem= \
  "add ebx,15" \
  "shr ebx,4" \
  "mov eax,0x100" \
  "int 0x31" \
  "mov dosseg,ax" \
  parm caller [ebx] \
  modify [eax] \
  value [dx]

void freedosmem(unsigned);
#pragma aux freedosmem= \
  "mov eax,0x101" \
  "int 0x31" \
  parm caller [edx] \
  modify [eax]

short vesamode(int);
#pragma aux vesamode= \
  "mov eax,0x4F02" \
  "int 0x10" \
  parm caller [ebx] \
  value [ax]

void vmode(int);
#pragma aux vmode= \
  "int 0x10" \
  parm caller [eax]

void vesabank(unsigned);
#pragma aux vesabank= \
  "mov eax,0x4F05" \
  "xor ebx,ebx" \
  "int 0x10" \
  parm caller [edx] \
  modify [eax ebx]

void vesapage(unsigned,unsigned);
#pragma aux vesapage= \
  "mov eax,0x4F07" \
  "mov ebx,0x80" \
  "int 0x10" \
  parm caller [ecx] [edx] \
  modify [eax ebx]

unsigned map_phys_mem(unsigned addr,unsigned len);
#pragma aux map_phys_mem= \
  "mov eax,0x800" \
  "mov ebx,ecx" \
  "shr ebx,16" \
  "mov esi,edi" \
  "shr esi,16" \
  "int 0x31" \
  "mov eax,0" \
  "jc err" \
  "mov eax,ebx" \
  "shl eax,16" \
  "mov ax,cx" \
  "err:" \
  parm caller [ecx] [edi] \
  modify [ebx ecx esi edi] \
  value [eax]

void free_phys_mem(unsigned lin_addr);
#pragma aux free_phys_mem= \
  "mov eax,0x801" \
  "mov ebx,ecx" \
  "shr ebx,16" \
  "int 0x31" \
  parm caller [ecx] \
  modify [ebx ecx eax]

typedef struct{
  unsigned short f;
  unsigned char af,bf;
  unsigned short gran,size,aseg,bseg;
  int func;
  unsigned short bpl;
  unsigned short hres,vres;
  unsigned char celw,celh,planes,bitspp;
  char NumberOfBanks;
  char MemoryModel;
  char BankSize;
  char NumberOfImagePages;
  char res1;
  char RedMaskSize;
  char RedFieldPosition;
  char GreenMaskSize;
  char GreenFieldPosition;
  char BlueMaskSize;
  char BlueFieldPosition;
  char RsvdMaskSize;
  char RsvdFieldPosition;
  char DirectColorModeInfo;
// VBE 2.0 extension information //
  unsigned PhysBasePtr;
  unsigned OffScreenMemOffset;
  short OffScreenMemSize; // Amount of offscreen mem in 1K's
  char reserved[206];
}mi_t;

typedef struct{
  char id[4];
  short ver;
  int name;
  int flags;
  unsigned short mofs,mseg;
  unsigned short mem;
}vesai_t;

static mi_t minfo;
static short vesa_ver=0x100;

static int getmi(int m) {
  mi_t *mi;
  unsigned short h;

  stdvga=0;linear=0;
  if(m==19) {
    sv_bpl=320;scrptr=(void*)0xA0000;
    sv_width=320;sv_height=200;
    sv_bits=8;sv_bytespp=1;
    stdvga=1;
    use_pages=0;
    minfo.bpl=320;minfo.hres=320;minfo.vres=200;
    minfo.planes=1;minfo.bitspp=8;
    return 1;
  }
  h=getdosmem(sizeof(mi_t));
  mi=(void*)(((unsigned)dosseg)<<4);
  memset(mi,0,sizeof(mi_t));
  memset(&RMI,0,sizeof(RMI));
  RMI.ECX=m;RMI.EAX=0x4F01;RMI.ES=dosseg;RMI.EDI=0;
  simint(0x10);
  if((RMI.EAX&0xFFFF)!=VESAOK) {freedosmem(h);vesa=0;return 0;}
  if((mi->f&1)==0 || (mi->f&0x10)==0) {freedosmem(h);vesa=0;return 0;}
  if((mi->f&0xC0)==0 || ((mi->f&0x40)==0 && !use_linear)) {
    linear=0;
    gran=mi->gran;sv_bpl=mi->bpl;scrptr=(void*)(((unsigned)mi->aseg)<<4);
    if(!gran) gran=64;
    step=64/gran;
  }else if((mi->f&0x80) && use_linear) {
    linear=1;
    sv_bpl=mi->bpl;
    scrptr=(void*)map_phys_mem(mi->PhysBasePtr,1024*1024*4);
    if(!scrptr) {
      if((mi->f&0x40)==0) {
	linear=0;
	gran=mi->gran;sv_bpl=mi->bpl;scrptr=(void*)(((unsigned)mi->aseg)<<4);
	if(!gran) gran=64;
	step=64/gran;
      }else {freedosmem(h);vesa=0;return 0;}
    }
  }else {freedosmem(h);vesa=0;return 0;}
  sv_width=mi->hres;sv_height=mi->vres;
  sv_bits=mi->bitspp;
  if(sv_bits==16 && mi->RsvdMaskSize) sv_bits=15;
  sv_bytespp=(sv_bits+7)>>3;
  memcpy(&minfo,mi,sizeof(mi_t));
  freedosmem(h);
  return 1;
}

static void chk_vesa(void) {
  vesai_t *vi;
  unsigned short h;

  h=getdosmem(1024);
  vi=(void*)(((unsigned)dosseg)<<4);
  memset(vi,0,1024);
  memset(&RMI,0,sizeof(RMI));
  RMI.EAX=0x4F00;RMI.ES=dosseg;RMI.EDI=0;

  memcpy(vi->id,"VBE2",4);
  RMI.EBX=0x1234;RMI.ECX=0x4321;
//  RMI.EBX=0;RMI.ECX=0;

  simint(0x10);
  if((RMI.EAX&0xFFFF)!=VESAOK) {freedosmem(h);vesa=0;return;}
  if(memcmp(vi->id,"VESA",4)!=0) {freedosmem(h);vesa=0;return;}
  vesa_ver=vi->ver;
  if(vesa_ver>0x200) vesa_ver=0x102;
  sv_mem=vi->mem*0x10000;
  freedosmem(h);
}

static void getvi(int xr,int yr,int bpp) {
  vesai_t *vi;
  unsigned short h,*mp;

  h=getdosmem(1024);
  vi=(void*)(((unsigned)dosseg)<<4);
  memset(vi,0,1024);
  memset(&RMI,0,sizeof(RMI));
  RMI.EAX=0x4F00;RMI.ES=dosseg;RMI.EDI=0;

  memcpy(vi->id,"VBE2",4);
  RMI.EBX=0x1234;RMI.ECX=0x4321;
//  RMI.EBX=0;RMI.ECX=0;

  simint(0x10);
  if((RMI.EAX&0xFFFF)!=VESAOK) {freedosmem(h);vesa=0;return;}
  if(memcmp(vi->id,"VESA",4)!=0) {freedosmem(h);vesa=0;return;}
  sv_mem=vi->mem*0x10000;
  mp=(void*)(vi->mseg*16+vi->mofs);
  for(;*mp!=0xFFFF;++mp) {
    if(getmi(*mp)) {
      if(xr) if(minfo.hres!=xr) {
	if(linear) free_phys_mem((unsigned)scrptr);
	continue;
      }
      if(yr) if(minfo.vres!=yr) {
	if(linear) free_phys_mem((unsigned)scrptr);
	continue;
      }
      if(bpp) if(minfo.bitspp!=bpp) {
	if(linear) free_phys_mem((unsigned)scrptr);
	continue;
      }
      break;
    }
  }
  if(*mp!=0xFFFF) sv_mode=*mp;
  freedosmem(h);
}

static void chk_vbe2(void) {
  unsigned short *vp,l;
  char *p;

  vbe2=0;
  if(!use_vbe2) return;
  if(vesa_ver<0x200) return;
  memset(&RMI,0,sizeof(RMI));
  RMI.EAX=0x4F0A;RMI.EBX=0;RMI.EDX=RMI.ECX=sv_mode;
  simint(0x10);
  if((RMI.EAX&0xFFFF)!=VESAOK) return;
  vp=(void*)(RMI.ES*16+(RMI.EDI&0xFFFF));
  l=RMI.ECX&0xFFFF;
  if(!(p=malloc(l))) return;
  memcpy(p,vp,l);
  vbe2code=p;
  vbe2_0=(vp[0] && vp[0]!=0xFFFF)?(void*)(p+vp[0]):NULL;
  vbe2_1=(vp[1] && vp[1]!=0xFFFF)?(void*)(p+vp[1]):NULL;
  vbe2_2=(vp[2] && vp[2]!=0xFFFF)?(void*)(p+vp[2]):NULL;
  vbe2=1;
}

//static int page=0;

int SV_findmode(int w,int h,int b) {
  vesa=0;vbe2=0;stdvga=0;linear=0;vesa_ver=0x100;
  if(sv_mode!=19) chk_vesa();
  if(sv_mode==0xFFFFFFFF) getvi(w,h,b);
    else if(!getmi(sv_mode)) return 0;
  if(stdvga) {
    if(!(scrbuf=malloc(sv_width*sv_height))) return 0;
    memset(scrbuf,0,sv_width*sv_height);
    vesa=1;
    vmode(19);
    return 1;
  }
  use_pages=0;
  if(vesamode(linear?(sv_mode|0x4000):sv_mode)!=VESAOK) {
    if(linear) free_phys_mem((unsigned)scrptr);
    vesa=0;
  }else{
    if(!(scrbuf=malloc(sv_width*sv_height*sv_bytespp))) {
      if(linear) free_phys_mem((unsigned)scrptr);
      return 0;
    }
    memset(scrbuf,0,sv_width*sv_height*sv_bytespp);
/*
    if(use_pages) {
      page=0;
      SV_setvpage(page^1);
      SV_setpage(page);
    }
*/
    vesa=1;
    sv_pagesz=(sv_height*sv_bpl+0xFFFF)&0xFFFF0000;
    sv_pgbank=0;
    sv_pgofs=0;
    chk_vbe2();
  }
  return vesa;
}

void SV_done(void) {
  if(scrbuf) {free(scrbuf);scrbuf=NULL;}
  if(vesa) {
    if(linear) free_phys_mem((unsigned)scrptr);
    vmode(3);vesa=0;
  }
  if(vbe2) {
    puts("Used VBE 2.0");
    if(vbe2_0) puts("  VBE2 bank switching");
    if(vbe2_1) puts("  VBE2 page flip");
    if(vbe2_2) puts("  VBE2 set palette");
    if(vbe2code) {free(vbe2code);vbe2code=NULL;}
  }
  if(linear) puts("Used linear framebuffer");
  if(use_pages) puts("Used video page flipping");
}

void SV_setvpage(unsigned n) {
  if(vbe2 && vbe2_1) {
    vbe2_setpage((n*sv_pagesz)>>2);
  }else{
    vesapage(n*sv_pagesz%sv_bpl/sv_bytespp,n*sv_pagesz/sv_bpl);
  }
}

void SV_setpage(unsigned n) {
  if(linear) {
    sv_pgofs=n*sv_pagesz;
  }else{
    sv_pgbank=(n*sv_pagesz)>>16;
  }
}

int SV_updatescr() {
/*
  if(use_pages) {
    SV_setvpage(page);
    SV_setpage(page^=1);
  }
*/
  return 1;
}

void SV_copytoscr(int x,int y,int w,int h,void *p,int bpl);

int SV_update_rect_before(int x,int y,int w,int h) {
  SV_copytoscr(x,y,w,h,scrbuf+(y*sv_width+x)*sv_bytespp,sv_width*sv_bytespp);
  return 1;
}

int SV_update_rect_after(int x,int y,int w,int h) {
  return 1;
}

static void bank(unsigned b) {
  if(vbe2 && vbe2_0) vbe2_setbank(b+sv_pgbank);
  else vesabank((b+sv_pgbank)*step);
}

#define chko(o,b) { if((o)&0xFFFF0000) {bank((b)+=((o)>>16));(o)&=0xFFFF;} }

static void line(unsigned o,void *p,unsigned w) {
  unsigned l;

  if((o>>16)!=((o+w)>>16)) {
    bank(o>>16);l=0x10000-(o&0xFFFF);
    memcpy(scrptr+(o&0xFFFF),p,l);
    bank((o+w)>>16);memcpy(scrptr,(unsigned char*)p+l,w-l);
  }else{
    bank(o>>16);memcpy(scrptr+(o&0xFFFF),p,w);
  }
}

void SV_setpal(void *p,int f,int n) {
  static struct{unsigned char b,g,r,a;} pal[256];

  if(vbe2 && vbe2_2) {
    int i;
    for(i=0;i<n;++i) {
      pal[i].b=((unsigned char*)p)[i*3+2];
      pal[i].g=((unsigned char*)p)[i*3+1];
      pal[i].r=((unsigned char*)p)[i*3];
      pal[i].a=0;
    }
    vbe2_setpal(pal,f,n);
  }else{
    vga_setpal(p,f,n);
  }
}

void SV_copytoscr(int x,int y,int w,int h,void *p,int b) {
  x=y*sv_bpl+x*sv_bytespp;
  w*=sv_bytespp;
  if(!stdvga && !linear) {
    for(;h;x+=sv_bpl,--h) {
      line(x,p,w);
      p=(void*)((char*)p+b);
    }
  }else{
    for(;h;x+=sv_bpl,--h) {
      memcpy(scrptr+sv_pgofs+x,p,w);
      p=(void*)((char*)p+b);
    }
  }
}
