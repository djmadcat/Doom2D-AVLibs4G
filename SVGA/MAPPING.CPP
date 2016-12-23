#include <svga.h>

typedef unsigned char uchar;

static short ofs[2048];

void svga_drv::drawspr(int x,int y,int w,int h,image *im) {}

void svga_drv32::drawspr(int x,int y,int wd,int ht,image *im) {
  if(wd<=0) wd=1; if(ht<=0) ht=1;
  int x1=x-im->sx*wd/im->w,y1=y-im->sy*ht/im->h;
  if(x1>cutr || y1>cutb) return;
  if(x1+wd<=cutl || y1+ht<=cutt) return;
  color32 *p=(color32*)(im+1);
  int x2=x1+wd; if(x2>cutr+1) x2=cutr+1;
  int y2=y1+ht; if(y2>cutb+1) y2=cutb+1;
  int ex=(im->sx*wd-(x-x1)*im->w),ey=(im->sy*ht-(y-y1)*im->h);
  if(x1<cutl) {ex+=(cutl-x1)*im->w;p+=ex/wd;ex=ex%wd;x1=cutl;}
  if(y1<cutt) {ey+=(cutt-y1)*im->h;p+=(ey/ht)*im->w;ey=ey%ht;y1=cutt;}
  char *ptr=(char *)((color32 *)((char*)scr+y1*pitch)+x1);
  x2-=x1;
  for(x1=0;x1<x2;++x1) {
    short o=0;
    for(ex+=im->w;ex>=wd;ex-=wd,++o);
    ofs[x1]=o;
  }
  for(;y1<y2;++y1,ptr+=pitch) {
    color32 *s=(color32 *)ptr,*m=p;
    for(x1=0;x1<x2;++s,m+=ofs[x1++]) switch(m->a) {
      case 255: break;
      case 0: *s=*m;break;
      default:
        s->b=shtab[s->b+(m->a<<8)]+m->b;
        s->g=shtab[s->g+(m->a<<8)]+m->g;
        s->r=shtab[s->r+(m->a<<8)]+m->r;
    }
    for(ey+=im->h;ey>=ht;ey-=ht,p+=im->w);
  }
}

void svga_drv24::drawspr(int x,int y,int wd,int ht,image *im) {
  if(wd<=0) wd=1; if(ht<=0) ht=1;
  int x1=x-im->sx*wd/im->w,y1=y-im->sy*ht/im->h;
  if(x1>cutr || y1>cutb) return;
  if(x1+wd<=cutl || y1+ht<=cutt) return;
  color24 *p=(color24*)(im+1);
  unsigned char *a=(unsigned char*)(p+im->w*im->h);
  int x2=x1+wd; if(x2>cutr+1) x2=cutr+1;
  int y2=y1+ht; if(y2>cutb+1) y2=cutb+1;
  int ex=(im->sx*wd-(x-x1)*im->w),ey=(im->sy*ht-(y-y1)*im->h);
  if(x1<cutl) {ex+=(cutl-x1)*im->w;p+=ex/wd;a+=ex/wd;ex=ex%wd;x1=cutl;}
  if(y1<cutt) {ey+=(cutt-y1)*im->h;p+=(ey/ht)*im->w;a+=(ey/ht)*im->w;ey=ey%ht;y1=cutt;}
  char *ptr=(char *)((color24 *)((char*)scr+y1*pitch)+x1);
  x2-=x1;
  for(x1=0;x1<x2;++x1) {
    short o=0;
    for(ex+=im->w;ex>=wd;ex-=wd,++o);
    ofs[x1]=o;
  }
  for(;y1<y2;++y1,ptr+=pitch) {
    color24 *s=(color24 *)ptr,*m=p;
    unsigned char *ma=a;
    for(x1=0;x1<x2;++s,m+=ofs[x1],ma+=ofs[x1],++x1) switch(*ma) {
      case 255: break;
      case 0: *s=*m;break;
      default:
        s->b=shtab[s->b+(*ma<<8)]+m->b;
        s->g=shtab[s->g+(*ma<<8)]+m->g;
        s->r=shtab[s->r+(*ma<<8)]+m->r;
    }
    for(ey+=im->h;ey>=ht;ey-=ht,p+=im->w,a+=im->w);
  }
}

void svga_drv16::drawspr(int x,int y,int wd,int ht,image *im) {
  if(wd<=0) wd=1; if(ht<=0) ht=1;
  int x1=x-im->sx*wd/im->w,y1=y-im->sy*ht/im->h;
  if(x1>cutr || y1>cutb) return;
  if(x1+wd<=cutl || y1+ht<=cutt) return;
  color16 *p=(color16*)(im+1);
  unsigned char *a=(unsigned char*)(p+im->w*im->h);
  int x2=x1+wd; if(x2>cutr+1) x2=cutr+1;
  int y2=y1+ht; if(y2>cutb+1) y2=cutb+1;
  int ex=(im->sx*wd-(x-x1)*im->w),ey=(im->sy*ht-(y-y1)*im->h);
  if(x1<cutl) {ex+=(cutl-x1)*im->w;p+=ex/wd;a+=ex/wd;ex=ex%wd;x1=cutl;}
  if(y1<cutt) {ey+=(cutt-y1)*im->h;p+=(ey/ht)*im->w;a+=(ey/ht)*im->w;ey=ey%ht;y1=cutt;}
  char *ptr=(char *)(((color16 *)((char*)scr+y1*pitch))+x1);
  x2-=x1;
  for(x1=0;x1<x2;++x1) {
    short o=0;
    for(ex+=im->w;ex>=wd;ex-=wd,++o);
    ofs[x1]=o;
  }
  for(;y1<y2;++y1,ptr+=pitch) {
    color16 *s=(color16 *)ptr,*m=p;
    unsigned char *ma=a;
    for(x1=0;x1<x2;++s,m+=ofs[x1],ma+=ofs[x1],++x1) switch(*ma) {
      case 255: break;
      case 0: *s=*m;break;
      default:
        *s=(stabb[((*s<<8)&0x1F00)+*ma]
          |stabg[((*s<<2)&0x1F00)+*ma]
          |stabr[((*s>>3)&0x1F00)+*ma])+*m;
    }
    for(ey+=im->h;ey>=ht;ey-=ht,p+=im->w,a+=im->w);
  }
}

void svga_drv15::drawspr(int x,int y,int wd,int ht,image *im) {
  if(wd<=0) wd=1; if(ht<=0) ht=1;
  int x1=x-im->sx*wd/im->w,y1=y-im->sy*ht/im->h;
  if(x1>cutr || y1>cutb) return;
  if(x1+wd<=cutl || y1+ht<=cutt) return;
  color16 *p=(color16*)(im+1);
  unsigned char *a=(unsigned char*)(p+im->w*im->h);
  int x2=x1+wd; if(x2>cutr+1) x2=cutr+1;
  int y2=y1+ht; if(y2>cutb+1) y2=cutb+1;
  int ex=(im->sx*wd-(x-x1)*im->w),ey=(im->sy*ht-(y-y1)*im->h);
  if(x1<cutl) {ex+=(cutl-x1)*im->w;p+=ex/wd;a+=ex/wd;ex=ex%wd;x1=cutl;}
  if(y1<cutt) {ey+=(cutt-y1)*im->h;p+=(ey/ht)*im->w;a+=(ey/ht)*im->w;ey=ey%ht;y1=cutt;}
  char *ptr=(char *)((color16 *)((char*)scr+y1*pitch)+x1);
  x2-=x1;
  for(x1=0;x1<x2;++x1) {
    short o=0;
    for(ex+=im->w;ex>=wd;ex-=wd,++o);
    ofs[x1]=o;
  }
  for(;y1<y2;++y1,ptr+=pitch) {
    color16 *s=(color16 *)ptr,*m=p;
    unsigned char *ma=a;
    for(x1=0;x1<x2;++s,m+=ofs[x1],ma+=ofs[x1],++x1) switch(*ma) {
      case 255: break;
      case 0: *s=*m;break;
      default:
        *s=(stabb[((*s<<8)&0x1F00)+*ma]
          |stabg[((*s<<3)&0x1F00)+*ma]
          |stabr[((*s>>2)&0x1F00)+*ma])+*m;
    }
    for(ey+=im->h;ey>=ht;ey-=ht,p+=im->w,a+=im->w);
  }
}

void svga_drv8::drawspr(int x,int y,int wd,int ht,image *im) {
  if(wd<=0) wd=1; if(ht<=0) ht=1;
  int x1=x-im->sx*wd/im->w,y1=y-im->sy*ht/im->h;
  if(x1>cutr || y1>cutb) return;
  if(x1+wd<=cutl || y1+ht<=cutt) return;
  uchar *p=(uchar*)(im+1);
  unsigned char *a=(unsigned char*)(p+im->w*im->h);
  int x2=x1+wd; if(x2>cutr+1) x2=cutr+1;
  int y2=y1+ht; if(y2>cutb+1) y2=cutb+1;
  int ex=(im->sx*wd-(x-x1)*im->w),ey=(im->sy*ht-(y-y1)*im->h);
  if(x1<cutl) {ex+=(cutl-x1)*im->w;p+=ex/wd;a+=ex/wd;ex=ex%wd;x1=cutl;}
  if(y1<cutt) {ey+=(cutt-y1)*im->h;p+=(ey/ht)*im->w;a+=(ey/ht)*im->w;ey=ey%ht;y1=cutt;}
  uchar *ptr=scr+y1*pitch+x1;
  x2-=x1;
  for(x1=0;x1<x2;++x1) {
    short o=0;
    for(ex+=im->w;ex>=wd;ex-=wd,++o);
    ofs[x1]=o;
  }
  for(;y1<y2;++y1,ptr+=pitch) {
    uchar *s=ptr,*m=p;
    unsigned char *ma=a;
    for(x1=0;x1<x2;++s,m+=ofs[x1],ma+=ofs[x1],++x1) switch(*ma) {
      case 255: break;
      case 0: *s=*m;break;
      default:
        *s=mtab[(shtab[(*ma<<8)+*s]<<8)+*m];
    }
    for(ey+=im->h;ey>=ht;ey-=ht,p+=im->w,a+=im->w);
  }
}
