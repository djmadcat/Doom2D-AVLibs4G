#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <svga.h>
#include <tab.h>
#include "dll.h"

#define RSZ 32

#define MAXHW 16

typedef unsigned char uchar;

typedef struct{int l,t,r,b,w,h;} rect;

struct HWrect{
  int l,t,r,b;
};

static uchar *map,*omap;
static HWrect *hwr;
static redraw_f *fn;
static int sl,st,sr,sb,wd,ht;

static char _noupd=0;

extern "C" DLLEXPORT void *RD_init(int x,int y,int w,int h) {
  rect *p;
  int wd=(w+RSZ-1)/RSZ,ht=(h+RSZ-1)/RSZ;
  if(!(p=(rect*)malloc(sizeof(rect)+wd*ht*2+sizeof(HWrect)*MAXHW))) return NULL;
  p->l=x;p->r=x+w-1;p->t=y;p->b=y+h-1;
  p->w=wd;p->h=ht;
  memset(p+1,0,wd*ht*2);
  HWrect *r=(HWrect*)((char*)(p+1)+wd*ht*2);
  for(int i=0;i<MAXHW;++i) {r[i].l=1;r[i].r=0;}
  return p;
}

extern "C" DLLEXPORT void RD_release(void *p) {
  if(p) free(p);
}

extern "C" DLLEXPORT int RD_add_hw_rectwh(void *p,int x,int y,int w,int h) {
  rect *rt=(rect*)p;
  HWrect *r=(HWrect*)((char*)(rt+1)+rt->w*rt->h*2);
//  x-=rt->l;y-=rt->t;
  for(int i=0;i<MAXHW;++i) if(r[i].l>r[i].r) {
    r[i].l=x;r[i].t=y;r[i].r=x+w-1;r[i].b=y+h-1;
    return 1;
  }
  return 0;
}

extern "C" DLLEXPORT void RD_release_hw_rect(void *p,int i) {
  if(i<0 || i>=MAXHW) return;
  rect *rt=(rect*)p;
  HWrect *r=(HWrect*)((char*)(rt+1)+rt->w*rt->h*2);
  r[i].l=1;r[i].r=0;
}

extern "C" DLLEXPORT void RD_start(void *p,redraw_f *f) {
  rect *rt=(rect*)p;
  fn=f;
  sl=rt->l;sr=rt->r;
  st=rt->t;sb=rt->b;
  wd=rt->w;ht=rt->h;
  map=(uchar*)(rt+1);
  omap=map+wd*ht;
  hwr=(HWrect*)(omap+wd*ht);
}

extern "C" DLLEXPORT void RD_rectwh(int x,int y,int w,int h) {
  RD_rect(x,y,x+w-1,y+h-1);
}

extern "C" DLLEXPORT void RD_rect(int l,int t,int r,int b) {
  if(_noupd) return;
  if(l>sr) return;
  if(r<sl) return;
  if(t>sb) return;
  if(b<st) return;
  l-=sl;t-=st;r-=sl;b-=st;
  l/=RSZ;t/=RSZ;r/=RSZ;b/=RSZ;
  if(l<0) l=0;
  if(t<0) t=0;
  if(r>=wd) r=wd-1;
  if(b>=ht) b=ht-1;
  uchar *m=map+t*wd+l;
  r=r-l+1;
  for(int y=t;y<=b;++y,m+=wd) memset(m,255,r);
}

extern "C" DLLEXPORT void RD_spr(int x,int y,image *v) {
  RD_rect(x-v->sx,y-v->sy,x-v->sx+v->w-1,y-v->sy+v->h-1);
}

struct Rect{
  int l,t,r,b;
};

static void rect_before(int l,int t,int r,int b) {
  for(int i=0;i<MAXHW;++i) if(hwr[i].l<=hwr[i].r) {
    if(l>hwr[i].r) continue;
    if(r<hwr[i].l) continue;
    if(t>hwr[i].b) continue;
    if(b<hwr[i].t) continue;
    if(t<hwr[i].t) rect_before(l,t,r,hwr[i].t-1);
    if(l<hwr[i].l) rect_before(l,hwr[i].t,hwr[i].l-1,hwr[i].b);
    if(r>hwr[i].r) rect_before(hwr[i].r+1,hwr[i].t,r,hwr[i].b);
    if(b>hwr[i].b) rect_before(l,hwr[i].b+1,r,b);
    return;
  }
  SV_update_rect_before(l,t,r-l+1,b-t+1);
}

static void rect_after(int l,int t,int r,int b) {
  for(int i=0;i<MAXHW;++i) if(hwr[i].l<=hwr[i].r) {
    if(l>hwr[i].r) continue;
    if(r<hwr[i].l) continue;
    if(t>hwr[i].b) continue;
    if(b<hwr[i].t) continue;
    if(t<hwr[i].t) rect_after(l,t,r,hwr[i].t-1);
    if(l<hwr[i].l) rect_after(l,hwr[i].t,hwr[i].l-1,hwr[i].b);
    if(r>hwr[i].r) rect_after(hwr[i].r+1,hwr[i].t,r,hwr[i].b);
    if(b>hwr[i].b) rect_after(l,hwr[i].b+1,r,b);
    return;
  }
  SV_update_rect_after(l,t,r-l+1,b-t+1);
}

extern "C" DLLEXPORT void RD_end(void) {
  _noupd=1;
  omap=map+wd*ht;
  int sz=wd*ht;
  for(int i=0;i<MAXHW;++i) if(hwr[i].l<=hwr[i].r) {
    int l=(hwr[i].l-sl-1)/RSZ+1,t=(hwr[i].t-st-1)/RSZ+1;
    int r=(hwr[i].r-sl+1)/RSZ-1,b=(hwr[i].b-st+1)/RSZ-1;
    uchar *m=map+t*wd+l;
    r=r-l+1;
    for(int j=t;j<=b;++j,m+=wd) memset(m,0,r);
  }
  for(i=0;i<sz;++i) if(map[i]) omap[i]=255;
  uchar *m=omap;
  Tab<Rect> rect;
  for(int y=0;y<ht;++y,m+=wd) {
    for(int x1=0;x1<wd;) {
      for(;x1<wd;++x1) if(m[x1]==255) break;
      if(x1>=wd) continue;
      for(int x2=x1+1;x2<wd;++x2) if(m[x2]!=255) break;
      x2-=x1;
      uchar *m2=m+wd+x1;
      for(int y2=y+1;y2<ht;++y2,m2+=wd)
	if(memcmp(m+x1,m2,x2)!=0) break;
      y2-=y;
      m2=m+x1;
      for(int j=0;j<y2;++j,m2+=wd) memset(m2,1,x2);
      int l=x1*RSZ+sl,t=y*RSZ+st,r=x1+x2,b=y+y2;
      if(r>=wd) r=sr; else r=r*RSZ-1+sl;
      if(b>=ht) b=sb; else b=b*RSZ-1+st;
      for(i=0;i<MAXHW;++i) if(hwr[i].l<=hwr[i].r) {
	if(l>=hwr[i].l && r<=hwr[i].r && t>=hwr[i].t && b<=hwr[i].b)
	  break;
      }
      if(i<MAXHW) {x1+=x2;continue;}
      j=rect.append(1,NULL,64);
      if(j<0) break;
      rect[j].l=l;rect[j].t=t;rect[j].r=r;rect[j].b=b;
      if(SV_lock(l,t,r-l+1,b-t+1)) {
	fn();
	SV_unlock();
      }
      x1+=x2;
    }
  }
  for(i=0;i<MAXHW;++i) if(hwr[i].l<=hwr[i].r)
    if(SV_lock_hw(hwr[i].l,hwr[i].t,hwr[i].r-hwr[i].l+1,hwr[i].b-hwr[i].t+1)) {
      fn();
      SV_unlock_hw();
    }
  for(i=0;i<rect.count();++i)
    rect_before(rect[i].l,rect[i].t,rect[i].r,rect[i].b);
  SV_updatescr();
  for(i=0;i<rect.count();++i)
    rect_after(rect[i].l,rect[i].t,rect[i].r,rect[i].b);
  memcpy(omap,map,sz);
  memset(map,0,sz);
  _noupd=0;
}
