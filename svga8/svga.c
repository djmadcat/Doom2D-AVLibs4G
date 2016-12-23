#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <svga8.h>

#define VESAOK 0x4F

unsigned sv_mode=0xFFFFFFFF;
unsigned sv_mem=256*1024;
static unsigned gran=64,step=1;
unsigned sv_bpl,sv_pitch;
static char vesa=0,stdvga=0;
char use_vbe2=1,vbe2=0;
static unsigned char *sptr;
static void *vbe2_0=NULL,*vbe2_1=NULL,*vbe2_2=NULL;
void *vbe2_drv=NULL;

unsigned sv_width,sv_height,sv_bits,sv_pgbank,sv_pagesz,sv_bytespp;

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
  "mov ebx,0x80" \
  "push ds" \
  "pop es" \
  "call [vbe2_2]" \
  "pop es" \
  parm [edi] [edx] [ecx] \
  modify [eax ebx]

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

int curmode(void);
#pragma aux curmode= \
  "mov ah,15" \
  "int 0x10" \
  "movzx eax,al" \
  value [eax]

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
  "xor ebx,ebx" \
  "int 0x10" \
  parm caller [ecx] [edx] \
  modify [eax ebx]

typedef struct{
  unsigned short f;
  unsigned char af,bf;
  unsigned short gran,size,aseg,bseg;
  int func;
  unsigned short bpl;
  unsigned short hres,vres;
  unsigned char celw,celh,planes,bitspp;
  char reserved[256-18-8];
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

static int getmi(int m) {
  mi_t *mi;
  unsigned short h;

  if(m==19) {
    sv_pitch=320;sptr=(void*)0xA0000;
    sv_width=320;sv_height=200;
    sv_bpl=320;
    sv_bits=8;sv_bytespp=1;
    stdvga=1;
    return 1;
  }
  stdvga=0;
  h=getdosmem(sizeof(mi_t));
  mi=(void*)(((unsigned)dosseg)<<4);
  memset(mi,0,sizeof(mi_t));
  memset(&RMI,0,sizeof(RMI));
  RMI.ECX=m;RMI.EAX=0x4F01;RMI.ES=dosseg;RMI.EDI=0;
  simint(0x10);
  if((RMI.EAX&0xFFFF)!=VESAOK) {freedosmem(h);vesa=0;return 0;}
  if((mi->f&1)==0 || (mi->f&0x10)==0) {freedosmem(h);vesa=0;return 0;}
  gran=mi->gran;sv_pitch=mi->bpl;sptr=(void*)(((unsigned)mi->aseg)<<4);
  step=64/gran;
  sv_width=mi->hres;sv_height=mi->vres;
  sv_bpl=sv_width;
  sv_bits=mi->bitspp;sv_bytespp=(sv_bits+7)>>3;
  memcpy(&minfo,mi,sizeof(mi_t));
  freedosmem(h);
  return 1;
}

static void getvi(int xr,int yr,int bpp) {
  vesai_t *vi;
  unsigned short h,*mp;

  h=getdosmem(1024);
  vi=(void*)(((unsigned)dosseg)<<4);
  memset(vi,0,1024);
  memset(&RMI,0,sizeof(RMI));
  RMI.EAX=0x4F00;RMI.ES=dosseg;RMI.EDI=0;
  simint(0x10);
  if((RMI.EAX&0xFFFF)!=VESAOK) {freedosmem(h);vesa=0;return;}
  if(memcmp(vi->id,"VESA",4)!=0) {freedosmem(h);vesa=0;return;}
  sv_mem=vi->mem*0x10000;
  mp=(void*)(vi->mseg*16+vi->mofs);
  for(;*mp!=0xFFFF;++mp) if(getmi(*mp)) {
    if(xr) if(minfo.hres!=xr) continue;
    if(yr) if(minfo.vres!=yr) continue;
    if(bpp) if(minfo.bitspp!=bpp) continue;
    break;
  }
  if(*mp!=0xFFFF) sv_mode=*mp;
  freedosmem(h);
}

static void chk_vbe2(void) {
  unsigned short *vp,l;
  char *p;

  vbe2=0;
  if(!vbe2_drv) {
    if(!use_vbe2) return;
    memset(&RMI,0,sizeof(RMI));
    RMI.EAX=0x4F0A;RMI.EBX=0;RMI.EDX=RMI.ECX=sv_mode;
    simint(0x10);
    if((RMI.EAX&0xFFFF)!=VESAOK) return;
    vp=(void*)(RMI.ES*16+(RMI.EDI&0xFFFF));
    l=RMI.ECX&0xFFFF;
    if(!(p=malloc(l))) return;
    memcpy(p,vp,l);
  }else {p=vbe2_drv;vp=vbe2_drv;}
  vbe2_0=(vp[0] && vp[0]!=0xFFFF)?(void*)(p+vp[0]):NULL;
  vbe2_1=(vp[1] && vp[1]!=0xFFFF)?(void*)(p+vp[1]):NULL;
  vbe2_2=(vp[2] && vp[2]!=0xFFFF)?(void*)(p+vp[2]):NULL;
  vbe2=1;
}

int SV_findmode(int w,int h) {
  vesa=0;vbe2=0;stdvga=0;
  if(sv_mode==0xFFFFFFFF) getvi(w,h,8);
    else if(!getmi(sv_mode)) return 0;
  if(stdvga) {
    if(!(scrbuf=malloc(sv_width*sv_height))) return 0;
    vmode(sv_mode);
    if(curmode()!=sv_mode) return 0;
    vesa=1;
    scra=scrbuf;
    scrx=scry=0;
    scrw=sv_width;
    scrh=sv_height;
    return 1;
  }
  if(vesamode(sv_mode)!=VESAOK) vesa=0;
  else{
    if(!(scrbuf=malloc(sv_width*sv_height))) return 0;
    vesa=1;
    sv_pagesz=(sv_height*sv_pitch+0xFFFF)&0xFFFF0000;
    sv_pgbank=0;
    scra=scrbuf;
    scrx=scry=0;
    scrw=sv_width;
    scrh=sv_height;
    chk_vbe2();
  }
  return vesa;
}

void SV_done(void) {
  if(vesa) {vmode(3);vesa=0;}
  if(scrbuf) {free(scrbuf);scrbuf=NULL;}
//  if(vbe2) puts("Used VBE 2.0");
}

void SV_setvpage(unsigned n) {
  if(vbe2) {
    vbe2_setpage(n*sv_pagesz);
  }else{
    vesapage(n*sv_pagesz%sv_pitch/sv_bytespp,n*sv_pagesz/sv_pitch);
  }
}

void SV_setpage(unsigned n) {
  sv_pgbank=(n*sv_pagesz)>>16;
}

static void bank(unsigned b) {
  if(vbe2) vbe2_setbank(b);
  else vesabank(b*step);
}

#define chko(o,b) { if((o)&0xFFFF0000) {bank((b)+=((o)>>16));(o)&=0xFFFF;} }

static void line(unsigned o,void *p,unsigned w) {
  unsigned l;

  if((o>>16)!=((o+w)>>16)) {
    bank(o>>16);l=0x10000-(o&0xFFFF);
    memcpy(sptr+(o&0xFFFF),p,l);
    bank((o+w)>>16);memcpy(sptr,(unsigned char*)p+l,w-l);
  }else{
    bank(o>>16);memcpy(sptr+(o&0xFFFF),p,w);
  }
}

void SV_copytoscr(int x,int y,int w,int h,void *p,int b) {
  x=y*sv_pitch+x;
  if(!stdvga) {
    for(;h;x+=sv_pitch,--h) {
      line(x,p,w);
      p=(void*)((char*)p+b);
    }
  }else{
    for(;h;x+=sv_pitch,--h) {
      memcpy(sptr+x,p,w);
      p=(void*)((char*)p+b);
    }
  }
}

void V_copytoscr(short x,short w,short y,short h) {
  SV_copytoscr(x,y,w,h,scra+y*sv_width+x,sv_width);
}

void _VP_set(void *p,short f,short n);

void VP_set(void *p,short f,short n) {
//  if(vbe2) vbe2_setpal(p,f,n);
//  else
  _VP_set(p,f,n);
}

void VP_setall(void *p) {
  VP_set(p,0,256);
}

void _VP_fill(char r,char g,char b);

void VP_fill(char r,char g,char b) {
//  static vgapal p;
//  int i;

//  if(vbe2) {
//    for(i=0;i<256;++i) {p[i].r=r;p[i].g=g;p[i].b=b;}
//    vbe2_setpal(p,0,256);
//  }else
  _VP_fill(r,g,b);
}
