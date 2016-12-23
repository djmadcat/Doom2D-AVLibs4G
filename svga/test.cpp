#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <config.h>
#include "svga.h"

#define W (320)
#define H (200)

typedef unsigned char uchar;

char cfn[]="svga.cfg";

image *back,*tank;

char makepal=0;

int main() {
  int w,h,b;
  int x,y,sx,sy,t,f;
  uchar *map;

  map=(uchar*)malloc(W*H);
  for(y=0;y<H;++y)
    for(x=0;x<W;++x)
      map[y*W+x]=x+y;

  w=h=b=0;
  if(CFG_read(cfn,"video","width")) w=cfg_val;
  if(CFG_read(cfn,"video","height")) h=cfg_val;
  if(CFG_read(cfn,"video","bits")) b=cfg_val;
  if(CFG_read(cfn,"video","mode")) sv_mode=cfg_val;
  if(CFG_read(cfn,"video","use_vbe2")) use_vbe2=cfg_val;
  if(CFG_read(cfn,"video","use_linear")) use_linear=cfg_val;
  if(CFG_read(cfn,"video","use_pages")) use_pages=cfg_val;
  if(CFG_read(cfn,"video","makepal")) makepal=1;
  if(!SV_findmode(w,h,b)) {printf("Video mode %dx%dx%d not found\n",w,h,b);return 1;}
  if(!SV_setdrv()) {
    SV_done();
    printf("Video mode %dx%dx%d not supported\n",sv_width,sv_height,sv_bits);
    return 1;
  }
  if(!(back=svga->convpic32(load_img("back.img")))) {
    delete svga;SV_done();
    puts("Cannot load image back.img");
    return 1;
  }
  if(!(tank=svga->convspr32(load_img("tank.img")))) {
    delete svga;SV_done();
    puts("Cannot load sprite tank.img");
    return 1;
  }
  svga->cutr=319;svga->cutb=199;
  x=5;y=1;sx=sy=1;
  f=0;t=time(NULL);

  if(makepal) if(!init_palcalc(256)) makepal=0;

  while(!kbhit()) {

    svga->drawpic(0,0,back);
    svga->drawspr(x,y,tank);
    svga->shaderect(0,0,W,H,map);

    svga->update_rectwh(0,0,W,H);
    svga->updatescr();
    if(use_pages) svga->update_rectwh(0,0,W,H);

    if(makepal) calc_use();

    if(x==-10 || x==159) sx=-sx;
    if(y==-10 || y==99) sy=-sy;
    x+=sx;y+=sy;
    ++f;
  }
  t=time(NULL)-t;
  if(!getch()) getch();
  SV_done();
  printf("%df/%ds\n",f,t);
  if(t) printf("%d fps\n",f/t);

  if(makepal) {
    int len;
    void *pal=end_palcalc(&len);
    FILE *fh=fopen("vgapal.dat","wb");
    fwrite(pal,len,1,fh);
    fclose(fh);
  }

  return 0;
}
