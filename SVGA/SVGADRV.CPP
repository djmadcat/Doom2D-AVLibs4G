#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#include "svga.h"
#include "dll.h"

extern "C" DLLEXPORT char use_pages=0;

typedef unsigned char uchar;

extern "C" DLLEXPORT unsigned SV_rgb8(int r,int g,int b) {
  if(r<0) r=0; else if(r>255) r=255;
  if(g<0) g=0; else if(g>255) g=255;
  if(b<0) b=0; else if(b>255) b=255;
  return svga->rgb(r,g,b);
}

extern "C" DLLEXPORT void SV_putc(char c) {
  if(!sv_font[(uchar)c]) {sv_curx+=sv_space;return;}
  svga->monospr(sv_curx,sv_cury,sv_font[(uchar)c],sv_prclr);
  sv_curx+=sv_font[(uchar)c]->w+sv_step;
}

extern "C" DLLEXPORT int SV_txtlen(char *s) {
  for(int l=-sv_step;*s;++s)
    if(!sv_font[(uchar)*s]) l+=sv_space;
    else l+=sv_font[(uchar)*s]->w+sv_step;
  if(l<0) l=0;
  return l;
}

extern "C" DLLEXPORT int SV_txtnlen(char *s,int n) {
  for(int l=-sv_step;n && *s;--n,++s)
    if(!sv_font[(uchar)*s]) l+=sv_space;
    else l+=sv_font[(uchar)*s]->w+sv_step;
  if(l<0) l=0;
  return l;
}

extern "C" DLLEXPORT int SV_txtlen_spec(char *s,int n,skipch_f *fn) {
  for(int l=-sv_step;n>0;--n,++s) {
    int sk=fn(*s);
    if(sk<0) break;
    if(sk) {s+=sk-1;n-=sk-1;continue;}
    if(!sv_font[(uchar)*s]) l+=sv_space;
    else l+=sv_font[(uchar)*s]->w+sv_step;
  }
  if(l<0) l=0;
  return l;
}

extern "C" DLLEXPORT void SV_puts(char *s) {
  for(;*s;++s) SV_putc(*s);
}

extern "C" DLLEXPORT void SV_putn(char *s,int n) {
  for(;n;--n,++s) SV_putc(*s);
}

extern "C" DLLEXPORT void SV_putn_spec(char *s,int n,skipch_f *fn) {
  for(;n>0;--n,++s) {
    int sk=fn(*s);
    if(sk<0) break;
    if(sk) {s+=sk-1;n-=sk-1;continue;}
    SV_putc(*s);
  }
}

extern "C" DLLEXPORT void SV_putsxy(int x,int y,char *s) {
  sv_curx=x;sv_cury=y;
  for(;*s;++s) SV_putc(*s);
}

extern "C" DLLEXPORT void SV_gotoxy(int x,int y) {
  sv_curx=x;sv_cury=y;
}

static char prbuf[256];

extern "C" DLLEXPORT void SV_prf(char *s,...) {
  va_list ap;
  va_start(ap,s);
  vsprintf(prbuf,s,ap);
  va_end(ap);
  SV_puts(prbuf);
}

extern "C" DLLEXPORT void SV_prfxy(int x,int y,char *s,...) {
  va_list ap;
  va_start(ap,s);
  vsprintf(prbuf,s,ap);
  va_end(ap);
  SV_putsxy(x,y,prbuf);
}

//------------------------------ svga_drv -----------------------------------//

DLLEXPORT svga_drv::svga_drv() {
  cutl=cutt=0;cutr=sv_width-1;cutb=sv_height-1;
  pitch=sv_width*sv_bytespp;scrwidth=sv_width;scrheight=sv_height;
}

DLLEXPORT svga_drv::~svga_drv() {}

DLLEXPORT void svga_drv::getcutrect(int &l,int &t,int &r,int &b) {
  l=cutl;t=cutt;r=cutr;b=cutb;
}

DLLEXPORT void svga_drv::setcutrect(int l,int t,int r,int b) {
  cutl=l;cutt=t;cutr=r;cutb=b;
}

DLLEXPORT int svga_drv::intersectrectwh(int x,int y,int w,int h) {
  return intersectrect(x,y,x+w-1,y+h-1);
}

DLLEXPORT int svga_drv::intersectrect(int l,int t,int r,int b) {
  if(cutl>r || cutt>b) return 0;
  if(cutr<l || cutb<t) return 0;
  if(cutl<l) cutl=l;
  if(cutr>r) cutr=r;
  if(cutt<t) cutt=t;
  if(cutb>b) cutb=b;
  return 1;
}

#pragma off (unreferenced)
DLLEXPORT void svga_drv::dot(int x,int y,ucolor c) {}
#pragma on (unreferenced)

#pragma off (unreferenced)
DLLEXPORT color24 svga_drv::getdot(int x,int y) {color24 c={0,0,0};return c;}
#pragma on (unreferenced)

DLLEXPORT image *svga_drv::convpic32(image *p) {return p;}

DLLEXPORT image *svga_drv::convspr32(image *p) {return p;}

#pragma off (unreferenced)
DLLEXPORT void svga_drv::drawpic(int x,int y,image *p) {}
#pragma on (unreferenced)

#pragma off (unreferenced)
DLLEXPORT void svga_drv::drawspr(int x,int y,image *p) {}
#pragma on (unreferenced)

#pragma off (unreferenced)
DLLEXPORT void svga_drv::clearwh(int x,int y,int w,int h,ucolor c) {}
#pragma on (unreferenced)

#pragma off (unreferenced)
DLLEXPORT void svga_drv::monorect(int x,int y,int w,int h,image *p,int sx,int sy,ucolor *c) {}
#pragma on (unreferenced)

#pragma off (unreferenced)
DLLEXPORT void svga_drv::monospr(int x,int y,image *p,ucolor c) {}
#pragma on (unreferenced)

#pragma off (unreferenced)
DLLEXPORT void svga_drv::shaderect(int x,int y,int w,int h,uchar *p) {}
#pragma on (unreferenced)

DLLEXPORT void svga_drv::clear(int l,int t,int r,int b,ucolor c) {
  clearwh(l,t,r-l+1,b-t+1,c);
}

#pragma off (unreferenced)
DLLEXPORT ucolor svga_drv::rgb(int r,int g,int b) {return 0;}
#pragma on (unreferenced)

#pragma off (unreferenced)
DLLEXPORT color24 svga_drv::toc24(ucolor s) {color24 c;return c;}
#pragma on (unreferenced)

#pragma off (unreferenced)
DLLEXPORT ucolor svga_drv::shade(ucolor c,uchar s) {return c;}
#pragma on (unreferenced)

DLLEXPORT void svga_drv::setbuf(void *p,int w,int h,int s) {
  scrwidth=w;scrheight=h;pitch=s;
}

//------------------------------ svga_drv32 ---------------------------------//

DLLEXPORT void svga_drv32::setbuf(void *p,int w,int h,int s) {
  scrwidth=w;scrheight=h;pitch=s;
  scr=(color32*)p;
}

DLLEXPORT svga_drv32::svga_drv32() {
  int i,j;
  scr=NULL;
  shtab=(uchar*)malloc(256*256);
  if(!shtab) return;
  for(i=0;i<256;++i)
    for(j=0;j<256;++j)
      shtab[i+j*256]=(j==255)?i:((i*j)>>8);
}

DLLEXPORT svga_drv32::~svga_drv32() {
  if(shtab) free(shtab);
}

DLLEXPORT int svga_drv32::is_ok() {
  return shtab!=NULL;
}

DLLEXPORT ucolor svga_drv32::rgb(int r,int g,int b) {
  return (r<<16)|(g<<8)|b;
}

DLLEXPORT color24 svga_drv32::toc24(ucolor s) {
  color24 c;
  c.b=s;c.g=s>>8;c.r=s>>16;
  return c;
}

DLLEXPORT ucolor svga_drv32::shade(ucolor c,uchar s) {
  color32 n;n.uc=c;
  n.b=shtab[n.b+(s<<8)];
  n.g=shtab[n.g+(s<<8)];
  n.r=shtab[n.r+(s<<8)];
  return n.uc;
}

DLLEXPORT void svga_drv32::dot(int x,int y,ucolor c) {
  if(x<cutl || x>cutr || y<cutt || y>cutb) return;
  ((color32*)((char*)scr+y*pitch))[x]=*(color32*)&c;
}

DLLEXPORT color24 svga_drv32::getdot(int x,int y) {
  color24 c={0,0,0};
  if(x<0 || y<0 || x>=sv_width || y>=sv_height) return c;
  color32 &sc=(((color32 *) ((char*)scr+y*pitch))[x]);
  c.r=sc.r;c.g=sc.g;c.b=sc.b;
  return c;
}

DLLEXPORT void svga_drv32::clearwh(int x,int y,int w,int h,ucolor c) {
  if(x>cutr || y>cutb) return;
  if(x+w<=cutl || y+h<=cutt) return;
  if(x<cutl) {w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  if(y<cutt) {h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  char *p=(char*)(((color32*)((char*)scr+y*pitch))+x);
  for(;h;--h,p+=pitch)
    for(int i=0;i<w;++i) ((ucolor*)p)[i]=c;
}

DLLEXPORT void svga_drv32::monorect(int x,int y,int w,int h,image *img,int sx,int sy,ucolor *c) {
  if(x>cutr || y>cutb) return;
  if(x+w<=cutl || y+h<=cutt) return;
  if(x<cutl) {w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  if(y<cutt) {h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  char *d=(char*)(((color32*)((char*)scr+y*pitch))+x);
  uchar *s=(uchar*)(img+1)+sy*img->w+sx;
  for(;h;--h,d+=pitch,s+=img->w)
    for(int i=0;i<w;++i) ((ucolor*)d)[i]=c[s[i]];
}

DLLEXPORT void svga_drv32::drawpic(int x,int y,image *im) {
  x-=im->sx;y-=im->sy;
  if(x>cutr || y>cutb) return;
  if(x+im->w<=cutl || y+im->h<=cutt) return;
  color32 *p=(color32*)(im+1);
  int w=im->w;
  if(x<cutl) {p+=cutl-x;w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  int h=im->h;
  if(y<cutt) {p+=(cutt-y)*im->w;h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  y=y*pitch+x*sv_bytespp;x=im->w;
  for(;h;--h,y+=pitch,p+=x)
    memcpy((char*)scr+y,p,w*4);
}

DLLEXPORT void svga_drv32::drawspr(int x,int y,image *im) {
  x-=im->sx;y-=im->sy;
  if(x>cutr || y>cutb) return;
  if(x+im->w<=cutl || y+im->h<=cutt) return;
  color32 *p=(color32*)(im+1);
  int w=im->w;
  if(x<cutl) {p+=cutl-x;w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  int h=im->h;
  if(y<cutt) {p+=(cutt-y)*im->w;h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  y=y*pitch+x*sv_bytespp;x=im->w;
  int i;
  for(;h;--h,y+=pitch,p+=x)
    for(i=0;i<w;++i) switch(p[i].a) {
      case 255: break;
      case 0: ((color32*)((char*)scr+y))[i]=p[i];break;
      default:
	((color32*)((char*)scr+y))[i].b=shtab[((color32*)((char*)scr+y))[i].b+(p[i].a<<8)]+p[i].b;
	((color32*)((char*)scr+y))[i].g=shtab[((color32*)((char*)scr+y))[i].g+(p[i].a<<8)]+p[i].g;
	((color32*)((char*)scr+y))[i].r=shtab[((color32*)((char*)scr+y))[i].r+(p[i].a<<8)]+p[i].r;
    }
}

DLLEXPORT void svga_drv32::monospr(int x,int y,image *im,ucolor c) {
  x-=im->sx;y-=im->sy;
  if(x>cutr || y>cutb) return;
  if(x+im->w<=cutl || y+im->h<=cutt) return;
  uchar *p=(uchar*)(im+1);
  int w=im->w;
  if(x<cutl) {p+=cutl-x;w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  int h=im->h;
  if(y<cutt) {p+=(cutt-y)*im->w;h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  y=y*pitch+x*sv_bytespp;x=im->w;
  int i;
  color32 nc;nc.uc=c;
  for(;h;--h,y+=pitch,p+=x)
    for(i=0;i<w;++i) switch(p[i]) {
      case 0: break;
      case 255: *(ucolor*)((color32*)((char*)scr+y)+i)=c;break;
      default:
	((color32*)((char*)scr+y))[i].b=shtab[((color32*)((char*)scr+y))[i].b+((255-p[i])<<8)]+shtab[nc.b+(p[i]<<8)];
	((color32*)((char*)scr+y))[i].g=shtab[((color32*)((char*)scr+y))[i].g+((255-p[i])<<8)]+shtab[nc.g+(p[i]<<8)];
	((color32*)((char*)scr+y))[i].r=shtab[((color32*)((char*)scr+y))[i].r+((255-p[i])<<8)]+shtab[nc.r+(p[i]<<8)];
    }
}

DLLEXPORT void svga_drv32::shaderect(int x,int y,int w,int h,uchar *p) {
  y=y*pitch+x*sv_bytespp;
  for(;h;--h,y+=pitch) {
    for(int i=0;i<w;++i,++p) {
      ((color32*)((char*)scr+y))[i].b=shtab[((color32*)((char*)scr+y))[i].b+(*p<<8)];
      ((color32*)((char*)scr+y))[i].g=shtab[((color32*)((char*)scr+y))[i].g+(*p<<8)];
      ((color32*)((char*)scr+y))[i].r=shtab[((color32*)((char*)scr+y))[i].r+(*p<<8)];
    }
  }
}

DLLEXPORT image *svga_drv32::convspr32(image *im) {
  if(!im) return NULL;
  color32 *p=(color32*)(im+1);
  for(int i=im->w*im->h;i;--i,++p) p->a=255-p->a;
  return im;
}

//----------------------------- svga_drv24 ----------------------------------//

DLLEXPORT void svga_drv24::setbuf(void *p,int w,int h,int s) {
  scrwidth=w;scrheight=h;pitch=s;
  scr=(color24*)p;
}

DLLEXPORT svga_drv24::svga_drv24() {
  int i,j;
  scr=NULL;
  shtab=(uchar*)malloc(256*256);
  if(!shtab) return;
  for(i=0;i<256;++i)
    for(j=0;j<256;++j)
      shtab[i+j*256]=(j==255)?i:((i*j)>>8);
}

DLLEXPORT svga_drv24::~svga_drv24() {
  if(shtab) free(shtab);
}

DLLEXPORT int svga_drv24::is_ok() {
  return shtab!=NULL;
}

DLLEXPORT ucolor svga_drv24::rgb(int r,int g,int b) {
  return (r<<16)|(g<<8)|b;
}

DLLEXPORT color24 svga_drv24::toc24(ucolor s) {
  color24 c;
  c.b=s;c.g=s>>8;c.r=s>>16;
  return c;
}

DLLEXPORT ucolor svga_drv24::shade(ucolor c,uchar s) {
  color32 n;n.uc=c;
  n.b=shtab[n.b+(s<<8)];
  n.g=shtab[n.g+(s<<8)];
  n.r=shtab[n.r+(s<<8)];
  return n.uc;
}

DLLEXPORT void svga_drv24::dot(int x,int y,ucolor c) {
  if(x<cutl || x>cutr || y<cutt || y>cutb) return;
  ((color24*)((char*)scr+y*pitch))[x]=*(color24*)&c;
}

DLLEXPORT color24 svga_drv24::getdot(int x,int y) {
  color24 c={0,0,0};
  if(x<0 || y<0 || x>=sv_width || y>=sv_height) return c;
  color24 &sc=(((color24 *) ((char*)scr+y*pitch))[x]);
  c.r=sc.r;c.g=sc.g;c.b=sc.b;
  return c;
}

DLLEXPORT void svga_drv24::clearwh(int x,int y,int w,int h,ucolor c) {
  if(x>cutr || y>cutb) return;
  if(x+w<=cutl || y+h<=cutt) return;
  if(x<cutl) {w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  if(y<cutt) {h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  char *p=(char*)(((color24*)((char*)scr+y*pitch))+x);
  for(;h;--h,p+=pitch)
    for(int i=0;i<w;++i) ((color24*)p)[i]=*((color24*)&c);
}

DLLEXPORT void svga_drv24::monorect(int x,int y,int w,int h,image *img,int sx,int sy,ucolor *c) {
  if(x>cutr || y>cutb) return;
  if(x+w<=cutl || y+h<=cutt) return;
  if(x<cutl) {w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  if(y<cutt) {h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  char *d=(char*)(((color24*)((char*)scr+y*pitch))+x);
  uchar *s=(uchar*)(img+1)+sy*img->w+sx;
  for(;h;--h,d+=pitch,s+=img->w)
    for(int i=0;i<w;++i) ((color24*)d)[i]=*((color24*)&c[s[i]]);
}

DLLEXPORT void svga_drv24::drawpic(int x,int y,image *im) {
  x-=im->sx;y-=im->sy;
  if(x>cutr || y>cutb) return;
  if(x+im->w<=cutl || y+im->h<=cutt) return;
  color24 *p=(color24*)(im+1);
  int w=im->w;
  if(x<cutl) {p+=cutl-x;w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  int h=im->h;
  if(y<cutt) {p+=(cutt-y)*im->w;h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  y=y*pitch+x*sv_bytespp;x=im->w;
  for(;h;--h,y+=pitch,p+=x)
    memcpy((char*)scr+y,p,w*3);
}

DLLEXPORT void svga_drv24::drawspr(int x,int y,image *im) {
  x-=im->sx;y-=im->sy;
  if(x>cutr || y>cutb) return;
  if(x+im->w<=cutl || y+im->h<=cutt) return;
  color24 *p=(color24*)(im+1);
  unsigned char *a=(unsigned char*)(p+im->w*im->h);
  int w=im->w;
  if(x<cutl) {p+=cutl-x;a+=cutl-x;w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  int h=im->h;
  if(y<cutt) {p+=(cutt-y)*im->w;a+=(cutt-y)*im->w;h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  y=y*pitch+x*sv_bytespp;x=im->w;
  int i;
  for(;h;--h,y+=pitch,p+=x,a+=x)
    for(i=0;i<w;++i) switch(a[i]) {
      case 255: break;
      case 0: ((color24*)((char*)scr+y))[i]=p[i];break;
      default:
	((color24*)((char*)scr+y))[i].b=shtab[((color24*)((char*)scr+y))[i].b+(a[i]<<8)]+p[i].b;
	((color24*)((char*)scr+y))[i].g=shtab[((color24*)((char*)scr+y))[i].g+(a[i]<<8)]+p[i].g;
	((color24*)((char*)scr+y))[i].r=shtab[((color24*)((char*)scr+y))[i].r+(a[i]<<8)]+p[i].r;
    }
}

DLLEXPORT void svga_drv24::monospr(int x,int y,image *im,ucolor c) {
  x-=im->sx;y-=im->sy;
  if(x>cutr || y>cutb) return;
  if(x+im->w<=cutl || y+im->h<=cutt) return;
  uchar *p=(uchar*)(im+1);
  int w=im->w;
  if(x<cutl) {p+=cutl-x;w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  int h=im->h;
  if(y<cutt) {p+=(cutt-y)*im->w;h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  y=y*pitch+x*sv_bytespp;x=im->w;
  int i;
  color24 nc=*(color24 *)&c;
  for(;h;--h,y+=pitch,p+=x)
    for(i=0;i<w;++i) switch(p[i]) {
      case 0: break;
      case 255: *((color24*)((char*)scr+y)+i)=nc;break;
      default:
	((color24*)((char*)scr+y))[i].b=shtab[((color24*)((char*)scr+y))[i].b+((255-p[i])<<8)]+shtab[nc.b+(p[i]<<8)];
	((color24*)((char*)scr+y))[i].g=shtab[((color24*)((char*)scr+y))[i].g+((255-p[i])<<8)]+shtab[nc.g+(p[i]<<8)];
	((color24*)((char*)scr+y))[i].r=shtab[((color24*)((char*)scr+y))[i].r+((255-p[i])<<8)]+shtab[nc.r+(p[i]<<8)];
    }
}

DLLEXPORT void svga_drv24::shaderect(int x,int y,int w,int h,uchar *p) {
  y=y*pitch+x*sv_bytespp;
  for(;h;--h,y+=pitch) {
    for(int i=0;i<w;++i,++p) {
      ((color24*)((char*)scr+y))[i].b=shtab[((color24*)((char*)scr+y))[i].b+(*p<<8)];
      ((color24*)((char*)scr+y))[i].g=shtab[((color24*)((char*)scr+y))[i].g+(*p<<8)];
      ((color24*)((char*)scr+y))[i].r=shtab[((color24*)((char*)scr+y))[i].r+(*p<<8)];
    }
  }
}

DLLEXPORT image *svga_drv24::convpic32(image *im) {
  if(!im) return NULL;
  image *h;
  if(!(h=(image*)malloc(im->w*im->h*3+sizeof(image)))) {free(im);return NULL;}
  *h=*im;
  color24 *p=(color24*)(h+1);
  color32 *s=(color32*)(im+1);
  for(int i=h->w*h->h;i;--i,++p,++s) {
    p->b=s->b;
    p->g=s->g;
    p->r=s->r;
  }
  free(im);
  return h;
}

DLLEXPORT image *svga_drv24::convspr32(image *im) {
  if(!im) return NULL;
  image *h;
  if(!(h=(image*)malloc(im->w*im->h*4+sizeof(image)))) {free(im);return NULL;}
  *h=*im;
  color24 *p=(color24*)(h+1);
  unsigned char *a=(unsigned char*)(p+im->w*im->h);
  color32 *s=(color32*)(im+1);
  for(int i=h->w*h->h;i;--i,++p,++s,++a) {
    p->b=s->b;
    p->g=s->g;
    p->r=s->r;
    *a=255-s->a;
  }
  free(im);
  return h;
}

//------------------------------ svga_drv16 ---------------------------------//

DLLEXPORT void svga_drv16::setbuf(void *p,int w,int h,int s) {
  scrwidth=w;scrheight=h;pitch=s;
  scr=(color16*)p;
}

DLLEXPORT svga_drv16::svga_drv16() {
  scr=NULL;
  stabb=(color16*)malloc(2*32*256*3);
  if(!stabb) return;
  stabg=stabb+32*256;
  stabr=stabg+32*256;
  for(int i=0;i<32;++i)
    for(int j=0;j<256;++j) {
      stabb[i*256+j]=(i*j/256);
      stabg[i*256+j]=(i*j/256)<<6;
      stabr[i*256+j]=(i*j/256)<<11;
    }
}

DLLEXPORT svga_drv16::~svga_drv16() {
  if(stabb) free(stabb);
}

DLLEXPORT int svga_drv16::is_ok() {
  return stabb!=NULL;
}

DLLEXPORT ucolor svga_drv16::rgb(int r,int g,int b) {
  return ((r&0xF8)<<8)|((g&0xFC)<<3)|(b>>3);
}

DLLEXPORT color24 svga_drv16::toc24(ucolor s) {
  color24 c;
  c.b=s<<3;c.g=(s>>3)&0xFC;c.r=(s>>8)&0xF8;
  return c;
}

DLLEXPORT ucolor svga_drv16::shade(ucolor c,uchar s) {
  return stabb[((c<<8)&0x1F00)+s]
    |stabg[((c<<2)&0x1F00)+s]
    |stabr[((c>>3)&0x1F00)+s];
}

DLLEXPORT void svga_drv16::dot(int x,int y,ucolor c) {
  if(x<cutl || x>cutr || y<cutt || y>cutb) return;
  ((color16*)((char*)scr+y*pitch))[x]=(color16)c;
}

DLLEXPORT color24 svga_drv16::getdot(int x,int y) {
  color24 c={0,0,0};
  if(x<0 || y<0 || x>=sv_width || y>=sv_height) return c;
  unsigned n=(((color16 *) ((char*)scr+y*pitch))[x]);
  c.r=(n>>11)*255/31;
  c.g=((n>>5)&63)*255/63;
  c.b=(n&31)*255/31;
  return c;
}

DLLEXPORT void svga_drv16::clearwh(int x,int y,int w,int h,ucolor c) {
  if(x>cutr || y>cutb) return;
  if(x+w<=cutl || y+h<=cutt) return;
  if(x<cutl) {w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  if(y<cutt) {h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  char *p=(char*)(((color16*)((char*)scr+y*pitch))+x);
  for(;h;--h,p+=pitch)
    for(int i=0;i<w;++i) ((color16*)p)[i]=c;
}

DLLEXPORT void svga_drv16::monorect(int x,int y,int w,int h,image *img,int sx,int sy,ucolor *c) {
  if(x>cutr || y>cutb) return;
  if(x+w<=cutl || y+h<=cutt) return;
  if(x<cutl) {w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  if(y<cutt) {h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  char *d=(char*)(((color16*)((char*)scr+y*pitch))+x);
  uchar *s=(uchar*)(img+1)+sy*img->w+sx;
  for(;h;--h,d+=pitch,s+=img->w)
    for(int i=0;i<w;++i) ((color16*)d)[i]=c[s[i]];
}

DLLEXPORT void svga_drv16::drawpic(int x,int y,image *im) {
  x-=im->sx;y-=im->sy;
  if(x>cutr || y>cutb) return;
  if(x+im->w<=cutl || y+im->h<=cutt) return;
  color16 *p=(color16*)(im+1);
  int w=im->w;
  if(x<cutl) {p+=cutl-x;w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  int h=im->h;
  if(y<cutt) {p+=(cutt-y)*im->w;h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  y=y*pitch+x*sv_bytespp;x=im->w;
  for(;h;--h,y+=pitch,p+=x)
    memcpy(((char*)scr)+y,p,w*2);
}

DLLEXPORT void svga_drv16::drawspr(int x,int y,image *im) {
  x-=im->sx;y-=im->sy;
  if(x>cutr || y>cutb) return;
  if(x+im->w<=cutl || y+im->h<=cutt) return;
  color16 *p=(color16*)(im+1);
  unsigned char *a=(unsigned char*)(p+im->w*im->h);
  int w=im->w;
  if(x<cutl) {p+=cutl-x;a+=cutl-x;w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  int h=im->h;
  if(y<cutt) {p+=(cutt-y)*im->w;a+=(cutt-y)*im->w;h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  y=y*pitch+x*sv_bytespp;x=im->w;
  int i;
  for(;h;--h,y+=pitch,p+=x,a+=x)
    for(i=0;i<w;++i) switch(a[i]) {
      case 255: break;
      case 0: ((color16*)((char*)scr+y))[i]=p[i];break;
      default:
	((color16*)((char*)scr+y))[i]=(stabb[((((color16*)((char*)scr+y))[i]<<8)&0x1F00)+a[i]]
	  |stabg[((((color16*)((char*)scr+y))[i]<<2)&0x1F00)+a[i]]
	  |stabr[((((color16*)((char*)scr+y))[i]>>3)&0x1F00)+a[i]])+p[i];
    }
}

DLLEXPORT void svga_drv16::monospr(int x,int y,image *im,ucolor c) {
  x-=im->sx;y-=im->sy;
  if(x>cutr || y>cutb) return;
  if(x+im->w<=cutl || y+im->h<=cutt) return;
  uchar *p=(uchar*)(im+1);
  int w=im->w;
  if(x<cutl) {p+=cutl-x;w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  int h=im->h;
  if(y<cutt) {p+=(cutt-y)*im->w;h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  y=y*pitch+x*sv_bytespp;x=im->w;
  int i;
  for(;h;--h,y+=pitch,p+=x)
    for(i=0;i<w;++i) switch(p[i]) {
      case 0: break;
      case 255: ((color16*)((char*)scr+y))[i]=c;break;
      default:
	((color16*)((char*)scr+y))[i]=(stabb[((((color16*)((char*)scr+y))[i]<<8)&0x1F00)+(255-p[i])]
	  |stabg[((((color16*)((char*)scr+y))[i]<<2)&0x1F00)+(255-p[i])]
	  |stabr[((((color16*)((char*)scr+y))[i]>>3)&0x1F00)+(255-p[i])])+
	  (stabb[((c<<8)&0x1F00)+p[i]]
	  |stabg[((c<<2)&0x1F00)+p[i]]
	  |stabr[((c>>3)&0x1F00)+p[i]]);
    }
}

DLLEXPORT void svga_drv16::shaderect(int x,int y,int w,int h,uchar *p) {
  y=y*pitch+x*sv_bytespp;
  for(;h;--h,y+=pitch) {
    for(int i=0;i<w;++i,++p) {
      ((color16*)((char*)scr+y))[i]=stabb[((((color16*)((char*)scr+y))[i]<<8)&0x1F00)+*p]
	|stabg[((((color16*)((char*)scr+y))[i]<<2)&0x1F00)+*p]
	|stabr[((((color16*)((char*)scr+y))[i]>>3)&0x1F00)+*p];
    }
  }
}

DLLEXPORT image *svga_drv16::convpic32(image *im) {
  if(!im) return NULL;
  image *h;
  if(!(h=(image*)malloc(im->w*im->h*2+sizeof(image)))) {free(im);return NULL;}
  *h=*im;
  color16 *p=(color16*)(h+1);
  color32 *s=(color32*)(im+1);
  for(int i=h->w*h->h;i;--i,++p,++s)
    *p=((s->r&0xF8)<<8)|((s->g&0xFC)<<3)|(s->b>>3);
  free(im);
  return h;
}

DLLEXPORT image *svga_drv16::convspr32(image *im) {
  if(!im) return NULL;
  image *h;
  if(!(h=(image*)malloc(im->w*im->h*3+sizeof(image)))) {free(im);return NULL;}
  *h=*im;
  color16 *p=(color16*)(h+1);
  unsigned char *a=(unsigned char*)(p+im->w*im->h);
  color32 *s=(color32*)(im+1);
  for(int i=h->w*h->h;i;--i,++p,++s,++a) {
    *p=((s->r&0xF8)<<8)|((s->g&0xFC)<<3)|(s->b>>3);
    *a=255-s->a;
  }
  free(im);
  return h;
}

//-------------------------------- svga_drv15 -------------------------------//

DLLEXPORT svga_drv15::svga_drv15() {
  if(!stabb) return;
  for(int i=0;i<32;++i)
    for(int j=0;j<256;++j) {
      stabb[i*256+j]=(i*j/256);
      stabg[i*256+j]=(i*j/256)<<5;
      stabr[i*256+j]=(i*j/256)<<10;
    }
}

DLLEXPORT svga_drv15::~svga_drv15() {}

DLLEXPORT ucolor svga_drv15::rgb(int r,int g,int b) {
  return ((r&0xF8)<<7)|((g&0xF8)<<2)|(b>>3);
}

DLLEXPORT color24 svga_drv15::toc24(ucolor s) {
  color24 c;
  c.b=s<<3;c.g=(s>>2)&0xF8;c.r=(s>>7)&0xF8;
  return c;
}

DLLEXPORT ucolor svga_drv15::shade(ucolor c,uchar s) {
  return stabb[((c<<8)&0x1F00)+s]
    |stabg[((c<<3)&0x1F00)+s]
    |stabr[((c>>2)&0x1F00)+s];
}

DLLEXPORT color24 svga_drv15::getdot(int x,int y) {
  color24 c={0,0,0};
  if(x<0 || y<0 || x>=sv_width || y>=sv_height) return c;
  unsigned n=(((color16 *) ((char*)scr+y*pitch))[x]);
  c.r=((n>>10)&31)*255/31;
  c.g=((n>>5)&31)*255/31;
  c.b=(n&31)*255/31;
  return c;
}

DLLEXPORT void svga_drv15::drawspr(int x,int y,image *im) {
  x-=im->sx;y-=im->sy;
  if(x>cutr || y>cutb) return;
  if(x+im->w<=cutl || y+im->h<=cutt) return;
  color16 *p=(color16*)(im+1);
  unsigned char *a=(unsigned char*)(p+im->w*im->h);
  int w=im->w;
  if(x<cutl) {p+=cutl-x;a+=cutl-x;w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  int h=im->h;
  if(y<cutt) {p+=(cutt-y)*im->w;a+=(cutt-y)*im->w;h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  y=y*pitch+x*sv_bytespp;x=im->w;
  int i;
  for(;h;--h,y+=pitch,p+=x,a+=x)
    for(i=0;i<w;++i) switch(a[i]) {
      case 255: break;
      case 0: ((color16*)((char*)scr+y))[i]=p[i];break;
      default:
	((color16*)((char*)scr+y))[i]=(stabb[((((color16*)((char*)scr+y))[i]<<8)&0x1F00)+a[i]]
	  |stabg[((((color16*)((char*)scr+y))[i]<<3)&0x1F00)+a[i]]
	  |stabr[((((color16*)((char*)scr+y))[i]>>2)&0x1F00)+a[i]])+p[i];
    }
}

DLLEXPORT void svga_drv15::monospr(int x,int y,image *im,ucolor c) {
  x-=im->sx;y-=im->sy;
  if(x>cutr || y>cutb) return;
  if(x+im->w<=cutl || y+im->h<=cutt) return;
  uchar *p=(uchar*)(im+1);
  int w=im->w;
  if(x<cutl) {p+=cutl-x;w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  int h=im->h;
  if(y<cutt) {p+=(cutt-y)*im->w;h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  y=y*pitch+x*sv_bytespp;x=im->w;
  int i;
  for(;h;--h,y+=pitch,p+=x)
    for(i=0;i<w;++i) switch(p[i]) {
      case 0: break;
      case 255: ((color16*)((char*)scr+y))[i]=c;break;
      default:
	((color16*)((char*)scr+y))[i]=(stabb[((((color16*)((char*)scr+y))[i]<<8)&0x1F00)+(255-p[i])]
	  |stabg[((((color16*)((char*)scr+y))[i]<<3)&0x1F00)+(255-p[i])]
	  |stabr[((((color16*)((char*)scr+y))[i]>>2)&0x1F00)+(255-p[i])])+
	  (stabb[((c<<8)&0x1F00)+p[i]]
	  |stabg[((c<<3)&0x1F00)+p[i]]
	  |stabr[((c>>2)&0x1F00)+p[i]]);
    }
}

DLLEXPORT void svga_drv15::shaderect(int x,int y,int w,int h,uchar *p) {
  y=y*pitch+x*sv_bytespp;
  for(;h;--h,y+=pitch) {
    for(int i=0;i<w;++i,++p) {
      ((color16*)((char*)scr+y))[i]=stabb[((((color16*)((char*)scr+y))[i]<<8)&0x1F00)+*p]
	|stabg[((((color16*)((char*)scr+y))[i]<<3)&0x1F00)+*p]
	|stabr[((((color16*)((char*)scr+y))[i]>>2)&0x1F00)+*p];
    }
  }
}

DLLEXPORT image *svga_drv15::convpic32(image *im) {
  if(!im) return NULL;
  image *h;
  if(!(h=(image*)malloc(im->w*im->h*2+sizeof(image)))) {free(im);return NULL;}
  *h=*im;
  color16 *p=(color16*)(h+1);
  color32 *s=(color32*)(im+1);
  for(int i=h->w*h->h;i;--i,++p,++s)
    *p=((s->r&0xF8)<<7)|((s->g&0xF8)<<2)|(s->b>>3);
  free(im);
  return h;
}

DLLEXPORT image *svga_drv15::convspr32(image *im) {
  if(!im) return NULL;
  image *h;
  if(!(h=(image*)malloc(im->w*im->h*3+sizeof(image)))) {free(im);return NULL;}
  *h=*im;
  color16 *p=(color16*)(h+1);
  unsigned char *a=(unsigned char*)(p+im->w*im->h);
  color32 *s=(color32*)(im+1);
  for(int i=h->w*h->h;i;--i,++p,++s,++a) {
    *p=((s->r&0xF8)<<7)|((s->g&0xF8)<<2)|(s->b>>3);
    *a=255-s->a;
  }
  free(im);
  return h;
}

//-------------------------------- svga_drv8 --------------------------------//

DLLEXPORT void svga_drv8::setbuf(void *p,int w,int h,int s) {
  scrwidth=w;scrheight=h;pitch=s;
  scr=(unsigned char*)p;
}

DLLEXPORT svga_drv8::svga_drv8(char *fn) {
  scr=NULL;
  if(!(ftab=(uchar*)malloc(32*32*32))) return;
  if(!(shtab=(uchar*)malloc(256*256))) return;
  if(!(mtab=(uchar*)malloc(256*256))) return;
  if(fn) {
    FILE *h=fopen(fn,"rb");
    if(h) {
      fread(pal,256*3,1,h);
      fread(ftab,32*32*32,1,h);
      fclose(h);
    }else fn=NULL;
  }
  if(!fn) {
    for(int i=0;i<64;++i) pal[i].r=pal[i].g=pal[i].b=i;
    for(;i<256;++i) {pal[i].r=63;pal[i].g=pal[i].b=0;}
    for(int r=0;r<32;++r)
      for(int g=0;g<32;++g)
	for(int b=0;b<32;++b)
	  ftab[r*32*32+g*32+b]=((r*77+g*151+b*28)>>(8-1));
  }
  for(int i=0;i<256;++i)
    for(int j=0;j<256;++j) {
      shtab[i*256+j]=ftab[pal[j].r*i/255/2*32*32+pal[j].g*i/255/2*32+pal[j].b*i/255/2];
      int r=(pal[i].r+pal[j].r)/2,g=(pal[i].g+pal[j].g)/2,b=(pal[i].b+pal[j].b)/2;
      if(r>31) r=31;
      if(g>31) g=31;
      if(b>31) b=31;
      mtab[i*256+j]=ftab[r*32*32+g*32+b];
    }
  SV_setpal(pal,0,256);
}

DLLEXPORT svga_drv8::~svga_drv8() {
  if(ftab) free(ftab);
  if(shtab) free(shtab);
  if(mtab) free(mtab);
}

DLLEXPORT int svga_drv8::is_ok() {
  return(ftab!=NULL && shtab!=NULL && mtab!=NULL);
}

DLLEXPORT ucolor svga_drv8::rgb(int r,int g,int b) {
  return ftab[(r>>3)*32*32+(g>>3)*32+(b>>3)];
}

DLLEXPORT color24 svga_drv8::toc24(ucolor s) {
  color24 c;
  c.b=pal[s].b<<2;
  c.g=pal[s].g<<2;
  c.r=pal[s].r<<2;
  return c;
}

DLLEXPORT ucolor svga_drv8::shade(ucolor c,uchar s) {
  return shtab[(s<<8)+c];
}

DLLEXPORT void svga_drv8::dot(int x,int y,ucolor c) {
  if(x<cutl || x>cutr || y<cutt || y>cutb) return;
  scr[y*pitch+x]=(uchar)c;
}

DLLEXPORT color24 svga_drv8::getdot(int x,int y) {
  color24 c={0,0,0};
  if(x<0 || y<0 || x>=sv_width || y>=sv_height) return c;
  unsigned n=scr[y*pitch+x];
  c.r=pal[n].r*255/63;
  c.g=pal[n].g*255/63;
  c.b=pal[n].b*255/63;
  return c;
}

DLLEXPORT image *svga_drv8::convpic32(image *im) {
  if(!im) return NULL;
  image *h;
  if(!(h=(image*)malloc(im->w*im->h+sizeof(image)))) {free(im);return NULL;}
  *h=*im;
  uchar *p=(uchar*)(h+1);
  color32 *s=(color32*)(im+1);
  for(int i=h->w*h->h;i;--i,++p,++s)
    *p=ftab[(s->r>>3)*32*32+(s->g>>3)*32+(s->b>>3)];
  free(im);
  return h;
}

DLLEXPORT image *svga_drv8::convspr32(image *im) {
  if(!im) return NULL;
  image *h;
  if(!(h=(image*)malloc(im->w*im->h*2+sizeof(image)))) {free(im);return NULL;}
  *h=*im;
  uchar *p=(uchar*)(h+1);
  unsigned char *a=(unsigned char*)(p+im->w*im->h);
  color32 *s=(color32*)(im+1);
  for(int i=h->w*h->h;i;--i,++p,++s,++a) {
    *p=ftab[(s->r>>3)*32*32+(s->g>>3)*32+(s->b>>3)];
    *a=255-s->a;
  }
  free(im);
  return h;
}

DLLEXPORT void svga_drv8::drawpic(int x,int y,image *im) {
  x-=im->sx;y-=im->sy;
  if(x>cutr || y>cutb) return;
  if(x+im->w<=cutl || y+im->h<=cutt) return;
  uchar *p=(uchar*)(im+1);
  int w=im->w;
  if(x<cutl) {p+=cutl-x;w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  int h=im->h;
  if(y<cutt) {p+=(cutt-y)*im->w;h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  y=y*pitch+x;x=im->w;
  for(;h;--h,y+=pitch,p+=x)
    memcpy(scr+y,p,w);
}

DLLEXPORT void svga_drv8::drawspr(int x,int y,image *im) {
  x-=im->sx;y-=im->sy;
  if(x>cutr || y>cutb) return;
  if(x+im->w<=cutl || y+im->h<=cutt) return;
  uchar *p=(uchar*)(im+1);
  unsigned char *a=(unsigned char*)(p+im->w*im->h);
  int w=im->w;
  if(x<cutl) {p+=cutl-x;a+=cutl-x;w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  int h=im->h;
  if(y<cutt) {p+=(cutt-y)*im->w;a+=(cutt-y)*im->w;h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  y=y*pitch+x;x=im->w;
  int i;
  for(;h;--h,y+=pitch,p+=x,a+=x)
    for(i=0;i<w;++i) switch(a[i]) {
      case 255: break;
      case 0: scr[y+i]=p[i];break;
      default:
	scr[y+i]=mtab[(shtab[(a[i]<<8)+scr[y+i]]<<8)+p[i]];
    }
}

DLLEXPORT void svga_drv8::clearwh(int x,int y,int w,int h,ucolor c) {
  if(x>cutr || y>cutb) return;
  if(x+w<=cutl || y+h<=cutt) return;
  if(x<cutl) {w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  if(y<cutt) {h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  uchar *p=scr+y*pitch+x;
  for(;h;--h,p+=pitch)
    memset(p,c,w);
}

DLLEXPORT void svga_drv8::monorect(int x,int y,int w,int h,image *img,int sx,int sy,ucolor *c) {
  if(x>cutr || y>cutb) return;
  if(x+w<=cutl || y+h<=cutt) return;
  if(x<cutl) {w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  if(y<cutt) {h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  uchar *d=scr+y*pitch+x;
  uchar *s=(uchar*)(img+1)+sy*img->w+sx;
  for(;h;--h,d+=pitch,s+=img->w)
    for(int i=0;i<w;++i) d[i]=c[s[i]];
}

DLLEXPORT void svga_drv8::monospr(int x,int y,image *im,ucolor c) {
  x-=im->sx;y-=im->sy;
  if(x>cutr || y>cutb) return;
  if(x+im->w<=cutl || y+im->h<=cutt) return;
  uchar *a=(uchar*)(im+1);
  int w=im->w;
  if(x<cutl) {a+=cutl-x;w-=cutl-x;x=cutl;}
  if(x+w-1>cutr) w=cutr-x+1;
  int h=im->h;
  if(y<cutt) {a+=(cutt-y)*im->w;h-=cutt-y;y=cutt;}
  if(y+h-1>cutb) h=cutb-y+1;
  y=y*pitch+x;x=im->w;
  int i;
  for(;h;--h,y+=pitch,a+=x)
    for(i=0;i<w;++i) switch(a[i]) {
      case 0: break;
      case 255:
	scr[y+i]=c;break;
      default:
	scr[y+i]=mtab[(shtab[((255-a[i])<<8)+scr[y+i]]<<8)+shtab[(a[i]<<8)+c]];
    }
}

DLLEXPORT void svga_drv8::shaderect(int x,int y,int w,int h,uchar *p) {
  y=y*pitch+x;
  for(;h;--h,y+=pitch) {
    for(int i=0;i<w;++i,++p) {
      scr[y+i]=shtab[(*p<<8)+scr[y+i]];
    }
  }
}

//---------------------------------------------------------------------------//

DLLEXPORT svga_drv *svga;

extern "C" DLLEXPORT int SV_setdrv(void) {
  switch(sv_bits) {
    case 32: if(!(svga=new svga_drv32)) return 0;  return 1;
    case 24: if(!(svga=new svga_drv24)) return 0;  return 1;
    case 16: if(!(svga=new svga_drv16)) return 0;  return 1;
    case 15: if(!(svga=new svga_drv15)) return 0;  return 1;
    case 8:  if(!(svga=new svga_drv8))  return 0;  return 1;
  }
  return 0;
}
