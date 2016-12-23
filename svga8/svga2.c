#include <stdlib.h>
#include <svga8.h>

short VP_findcolor(void *pal,int n,int r,int g,int b) {
  rgb_t *p;
  int i,best,bd,d;

  p=(rgb_t*)pal;
  if(r<0) r=0;
  if(r>63) r=63;
  if(g<0) g=0;
  if(g>63) g=63;
  if(b<0) b=0;
  if(b>63) b=63;
  best=0;bd=(p[0].r-r)*(p[0].r-r)+(p[0].g-g)*(p[0].g-g)+(p[0].b-b)*(p[0].b-b);
  for(i=1;i<n;++i) {
    d=(p[i].r-r)*(p[i].r-r)+(p[i].g-g)*(p[i].g-g)+(p[i].b-b)*(p[i].b-b);
    if(d<=bd) {bd=d;best=i;}
  }
  return best;
}

void V_line(int x1,int y1,int x2,int y2,unsigned char c) {
  int d,s,t,n,e;

  if(y1==y2) {
    if(x2==x1) {V_dot(x1,y1,c);return;}
    if(x2>x1) V_clr(x1,x2-x1+1,y1,1,c);
    else V_clr(x2,x1-x2+1,y1,1,c);
    return;
  }
  if(x1==x2) {
    if(y2>y1) V_clr(x1,1,y1,y2-y1+1,c);
    else V_clr(x1,1,y2,y1-y2+1,c);
    return;
  }
  if(x1<scrx && x2<scrx) return;
  if(x1>=scrx+scrw && x2>=scrx+scrw) return;
  if(y1<scry && y2<scry) return;
  if(y1>=scry+scrh && y2>=scry+scrh) return;
  if(abs(x2-x1) > abs(y2-y1)) {
    int ys;
    unsigned char *p;
    if(x1>x2) {d=x1;x1=x2;x2=d;d=y1;y1=y2;y2=d;}
    if((d=y2-y1)>=0) s=1;
    else {s=-1;d=-d;}
    ys=s*sv_bpl;
    n=(t=x2-x1)+1;
    e=0;
    if(x1<scrx) {
      x1=scrx-x1;
      n-=x1;
      y1+=d*x1/t*s;
      e+=d*x1%t;
      x1=scrx;
    }
    p=scra+y1*sv_bpl+x1;
    for(;n>0 && x1<scrx+scrw;--n) {
      if(y1>=scry && y1<scry+scrh) *p=c;
      for(e+=d,++x1,++p;e>=t;e-=t,y1+=s,p+=ys);
    }
  }else{
    unsigned char *p;
    if(y1>y2) {d=x1;x1=x2;x2=d;d=y1;y1=y2;y2=d;}
    if((d=x2-x1)>=0) s=1;
    else {s=-1;d=-d;}
    n=(t=y2-y1)+1;
    e=0;
    if(y1<scry) {
      y1=scry-y1;
      n-=y1;
      x1+=d*y1/t*s;
      e+=d*y1%t;
      y1=scry;
    }
    p=scra+y1*sv_bpl+x1;
    for(;n>0 && y1<scry+scrh;--n) {
      if(x1>=scrx && x1<scrx+scrw) *p=c;
      for(e+=d,++y1,p+=sv_bpl;e>=t;e-=t,x1+=s,p+=s);
    }
  }
}
