#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "svga.h"
#include "lzapack.h"

enum{IMGT_NORMAL=0};
enum{IMGP_NONE=0,IMGP_RAW1,IMGP_PAL1};

static struct{
  short w,h,sx,sy;
  unsigned char bits,type;
  short pack;
  unsigned size;
}it;

static FILE *fh;

static int unpack_none(color32 *img) {
  if(it.size!=it.w*it.h*4) return 0;
  fread(img,it.size,1,fh);
  return 1;
}

static int unpack_raw1(color32 *img) {
  void *p;

  if(!(p=malloc(it.size))) return 0;
  fread(p,it.size,1,fh);
  if(!lza_unpack(p,it.size,img,it.w*it.h*4)) {free(p);return 0;}
  return 1;
}

/*
static int unpack_huf1(color32 *img) {
  void *p;
  unsigned char *v;
  unsigned pl,pixs;
  int i;

  if(!(v=malloc(pixs=it.w*it.h))) return 0;

  fread(&pl,4,1,fh);
  if(!(p=malloc(pl))) {free(v);return 0;}
  fread(p,pl,1,fh);
//  huf_init();
  if(!lza_unpack(p,pl,v,it.w*it.h)) {free(p);free(v);return 0;}
  img[0].b=v[0];
  for(i=1;i<pixs;++i) img[i].b=v[i]+img[i-1].b;
  free(p);

  fread(&pl,4,1,fh);
  if(!(p=malloc(pl))) {free(v);return 0;}
  fread(p,pl,1,fh);
//  huf_init();
  if(!lza_unpack(p,pl,v,it.w*it.h)) {free(p);free(v);return 0;}
  img[0].g=v[0];
  for(i=1;i<pixs;++i) img[i].g=v[i]+img[i-1].g;
  free(p);

  fread(&pl,4,1,fh);
  if(!(p=malloc(pl))) {free(v);return 0;}
  fread(p,pl,1,fh);
//  huf_init();
  if(!lza_unpack(p,pl,v,it.w*it.h)) {free(p);free(v);return 0;}
  img[0].r=v[0];
  for(i=1;i<pixs;++i) img[i].r=v[i]+img[i-1].r;
  free(p);

  fread(&pl,4,1,fh);
  if(!(p=malloc(pl))) {free(v);return 0;}
  fread(p,pl,1,fh);
//  huf_init();
  if(!lza_unpack(p,pl,v,it.w*it.h)) {free(p);free(v);return 0;}
  img[0].a=v[0];
  for(i=1;i<pixs;++i) img[i].a=v[i]+img[i-1].a;
  free(p);

  free(v);
  return 1;
}
*/

static int unpack_pal1(color32 *img) {
  void *p;
  unsigned char *v;
  unsigned pl;
  int i,cn;
  static color32 pal[256];

  if(!(v=malloc(it.w*it.h))) return 0;

  cn=0;fread(&cn,1,1,fh);if(!cn) cn=256;

  pl=0;fread(&pl,2,1,fh);
  if(!(p=malloc(pl))) {free(v);return 0;}
  fread(p,pl,1,fh);
//  huf_init();
  if(!lza_unpack(p,pl,pal,cn*4)) {free(p);free(v);return 0;}
  free(p);

  pl=it.size-pl-3;
  if(!(p=malloc(pl))) {free(v);return 0;}
  fread(p,pl,1,fh);
//  huf_init();
  if(!lza_unpack(p,pl,v,it.w*it.h)) {free(p);free(v);return 0;}
  free(p);
  for(i=it.w*it.h-1;i>=0;--i) {
    img[i]=pal[v[i]];
    if(v[i]>=cn) {free(v);return 0;}
  }

  free(v);
  return 1;
}

image *load_img(const char *fn) {
  image *p;

  if(!(fh=fopen(fn,"rb"))) return NULL;
  fread(&it,sizeof(it),1,fh);
  if(!(p=malloc(it.w*it.h*4+sizeof(image)))) {fclose(fh);return NULL;}
  p->w=it.w;p->h=it.h;
  p->sx=it.sx;p->sy=it.sy;
  switch(it.pack) {
    case IMGP_NONE:
      if(!unpack_none((color32*)(p+1))) {free(p);fclose(fh);return NULL;}
      break;
    case IMGP_RAW1:
      if(!unpack_raw1((color32*)(p+1))) {free(p);fclose(fh);return NULL;}
      break;
    case IMGP_PAL1:
      if(!unpack_pal1((color32*)(p+1))) {free(p);fclose(fh);return NULL;}
      break;
    default: free(p);fclose(fh);return NULL;
  }
  fclose(fh);
  return p;
}
