#include <stdlib.h>
#include <string.h>
#include <svgapal.h>
#include "dll.h"

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;

DLLEXPORT PaletteMaker *sv_palmaker=NULL;

DLLEXPORT PaletteMaker::PaletteMaker() {
  hist=NULL;
}

DLLEXPORT PaletteMaker::~PaletteMaker() {
  if(hist) {free(hist);hist=NULL;}
}

DLLEXPORT int PaletteMaker::allocate() {
  hist=(ushort*)malloc(32*64*32*2);
  if(!hist) return 0;
  memset(hist,0,32*64*32*2);
  return 1;
}

DLLEXPORT void PaletteMaker::add(int r,int g,int b,int n) {
  ushort *p=hist+(((r<<8)&0xF800)|((g<<3)&0x07E0)|((b>>3)&0x001F));
  uint u=*p+n;
  if(u>=0x10000) *p=0xFFFF; *p=u;
}

DLLEXPORT void PaletteMaker::add16(color16 c,int n) {
  ushort *p=hist+c;
  uint u=*p+n;
  if(u>=0x10000) *p=0xFFFF; *p=u;
}

DLLEXPORT void PaletteMaker::add15(color16 c,int n) {
  ushort *p=hist+(((c>>1)&0x7FE0)|(c&0x1F));
  uint u=*p+n;
  if(u>=0x10000) *p=0xFFFF; *p=u;
}

DLLEXPORT void PaletteMaker::addcolors24(color24 *p,int n) {
  for(;n;--n,++p) {
    ushort *h=hist+(((p->r<<8)&0xF800)|((p->g<<3)&0x07E0)|((p->b>>3)&0x001F));
    if(++(*h)<=0) --(*h);
  }
}

DLLEXPORT void PaletteMaker::addcolors32(color32 *p,int n) {
  for(;n;--n,++p) {
    ushort *h=hist+(((p->r<<8)&0xF800)|((p->g<<3)&0x07E0)|((p->b>>3)&0x001F));
    if(++(*h)<=0) --(*h);
  }
}

DLLEXPORT void PaletteMaker::addcolors16(color16 *p,int n) {
  for(;n;--n,++p) {
    ushort *h=hist+*p;
    if(++(*h)<=0) --(*h);
  }
}

DLLEXPORT void PaletteMaker::addcolors15(color16 *p,int n) {
  for(;n;--n,++p) {
    ushort *h=hist+(((*p>>1)&0x7FE0)|(*p&0x1F));
    if(++(*h)<=0) --(*h);
  }
}

DLLEXPORT void PaletteMaker::addcolors8(unsigned char *p,int n,color24 *pal) {
  ushort *pmap=(ushort*)malloc(256*2);
  if(pmap) {
    for(int i=0;i<256;++i)
      pmap[i]=(((pal[i].r<<8)&0xF800)|((pal[i].g<<3)&0x07E0)|((pal[i].b>>3)&0x001F));
    for(;n;--n,++p) {
      ushort *h=hist+pmap[*p];
      if(++(*h)<=0) --(*h);
    }
  }else{
    for(;n;--n,++p) {
      ushort *h=hist+(((pal[*p].r<<8)&0xF800)|((pal[*p].g<<3)&0x07E0)|((pal[*p].b>>3)&0x001F));
      if(++(*h)<=0) --(*h);
    }
  }
}

struct MinMax{uchar min,max;};

struct Box{
  MinMax lim[3];
  int vol;
  int cn;
  MinMax& operator [] (int i) {return lim[i];}
};

static void update_box(Box &box,ushort *hist) {
  for(int m=box[0].min;m<box[0].max;++m)
    for(int i=box[1].min;i<=box[1].max;++i)
      for(int j=box[2].min;j<=box[2].max;++j)
	if(hist[(j<<11)|(i<<5)|m]) goto min0;
min0:
  box[0].min=m;
  for(m=box[0].max;m>box[0].min;--m)
    for(int i=box[1].min;i<=box[1].max;++i)
      for(int j=box[2].min;j<=box[2].max;++j)
	if(hist[(j<<11)|(i<<5)|m]) goto max0;
max0:
  box[0].max=m;

  for(m=box[1].min;m<box[1].max;++m)
    for(int i=box[0].min;i<=box[0].max;++i)
      for(int j=box[2].min;j<=box[2].max;++j)
	if(hist[(j<<11)|(m<<5)|i]) goto min1;
min1:
  box[1].min=m;
  for(m=box[1].max;m>box[1].min;--m)
    for(int i=box[0].min;i<=box[0].max;++i)
      for(int j=box[2].min;j<=box[2].max;++j)
	if(hist[(j<<11)|(m<<5)|i]) goto max1;
max1:
  box[1].max=m;

  for(m=box[2].min;m<box[2].max;++m)
    for(int i=box[1].min;i<=box[1].max;++i)
      for(int j=box[0].min;j<=box[0].max;++j)
	if(hist[(m<<11)|(i<<5)|j]) goto min2;
min2:
  box[2].min=m;
  for(m=box[2].max;m>box[2].min;--m)
    for(int i=box[1].min;i<=box[1].max;++i)
      for(int j=box[0].min;j<=box[0].max;++j)
	if(hist[(m<<11)|(i<<5)|j]) goto max2;
max2:
  box[2].max=m;
  int db=((box[0].max-box[0].min)<<3);
  int dg=((box[1].max-box[1].min)<<2)*3;
  int dr=((box[2].max-box[2].min)<<3)*2;
  box.vol=db*db+dg*dg+dr*dr;
  box.cn=0;
  for(int i=box[0].min;i<=box[0].max;++i)
    for(int j=box[1].min;j<=box[1].max;++j)
      for(int k=box[2].min;k<=box[2].max;++k)
	if(hist[(k<<11)|(j<<5)|i]) ++box.cn;
}

DLLEXPORT int PaletteMaker::calcpal(color24 *pal,int num) {
  Box *box=(Box*)malloc(num*sizeof(Box));
  if(!box) return -1;
  box[0][0].min=0;box[0][1].min=0;box[0][2].min=0;
  box[0][0].max=31;box[0][1].max=63;box[0][2].max=31;
  update_box(box[0],hist);
  int nb=1;
  while(nb<num) {
    Box *b1=NULL;
    if(nb*2<=num) {
      int m=0;
      for(int i=0;i<nb;++i) if(box[i].cn>m) {m=box[i].cn;b1=box+i;}
    }else{
      int m=0;
      for(int i=0;i<nb;++i) if(box[i].vol>m) {m=box[i].vol;b1=box+i;}
    }
    if(!b1) break;
    Box &b2=box[nb++];
    b2=*b1;
    int db=((b2[0].max-b2[0].min)<<3);
    int dg=((b2[1].max-b2[1].min)<<2)*3;
    int dr=((b2[2].max-b2[2].min)<<3)*2;
    int m=dg,c=1;
    if(dr>m) {m=dr;c=2;}
    if(db>m) {m=db;c=0;}
    m=(b2[c].min+b2[c].max)>>1;
    b1->lim[c].max=m;
    b2[c].min=m+1;
    update_box(*b1,hist);
    update_box(b2,hist);
  }
  for(int i=0;i<nb;++i) {
    uint t=0,tb=0,tg=0,tr=0,c;
    for(int r=box[i][2].min;r<=box[i][2].max;++r)
      for(int g=box[i][1].min;g<=box[i][1].max;++g)
	for(int b=box[i][0].min;b<=box[i][0].max;++b)
	  if((c=hist[(r<<11)|(g<<5)|b])!=0) {
	    t+=c;
	    tb+=((b<<3)+(1<<2))*c;
	    tg+=((g<<2)+(1<<1))*c;
	    tr+=((r<<3)+(1<<2))*c;
	  }
    if(t) {
      tb=(tb+(t>>1))/t;
      tg=(tg+(t>>1))/t;
      tr=(tr+(t>>1))/t;
    }
    if(tb>255) tb=255;
    if(tg>255) tg=255;
    if(tr>255) tr=255;
    pal[i].b=tb;
    pal[i].g=tg;
    pal[i].r=tr;
  }
  free(box);
  return nb;
}

DLLEXPORT void SV_add_screen_colors() {
  if(!svga) return;
  if(!sv_palmaker) return;
  switch(sv_bits) {
    case 16:{
      color16 *s=((svga_drv16*)svga)->scr;
      sv_palmaker->addcolors16(s,svga->scrwidth*svga->scrheight);
    }break;
    case 15:{
      color16 *s=((svga_drv15*)svga)->scr;
      sv_palmaker->addcolors15(s,svga->scrwidth*svga->scrheight);
    }break;
    case 32:{
      color32 *s=((svga_drv32*)svga)->scr;
      sv_palmaker->addcolors32(s,svga->scrwidth*svga->scrheight);
    }break;
    case 24:{
      color24 *s=((svga_drv24*)svga)->scr;
      sv_palmaker->addcolors24(s,svga->scrwidth*svga->scrheight);
    }break;
  }
}


#define sqr(a) ((a)*(a))

DLLEXPORT void *SV_make_vga_color_data(color24 *pal,int &len) {
  vgargb *vp;
  uchar *ft;

  vp=(vgargb*)malloc(256*3+32*32*32);
  if(!vp) return NULL;
  ft=(uchar*)vp+256*3;
  for(int i=0;i<256;++i) {
    vp[i].r=pal[i].r>>2;
    vp[i].g=pal[i].g>>2;
    vp[i].b=pal[i].b>>2;
  }
  for(int r=0;r<32;++r) {
    for(int g=0;g<32;++g)
      for(int b=0;b<32;++b) {
	int m=64*64*3,n=0,j;
	for(int i=0;i<256;++i)
	  if((j=sqr(r*2-vp[i].r)+sqr(g*2-vp[i].g)+sqr(b*2-vp[i].b)) < m)
	    {m=j;n=i;}
	ft[r*32*32+g*32+b]=n;
      }
  }
  len=256*3+32*32*32;
  return vp;
}
