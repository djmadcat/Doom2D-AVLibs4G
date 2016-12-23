#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <conio.h>
#include <time.h>
#include <config.h>
#include <svga.h>
#include <decode.h>

SMBFLIK *curent=NULL;

typedef unsigned char uchar;

char cfn[]="svga.cfg";

void *rd_data=NULL;

#include "consts.h"

void dr_close() {
  if(rd_data) {RD_release(rd_data);rd_data=NULL;}
}

#ifdef __NT__
void  close_win();
#endif

void close() {
  dr_close();
  SV_done();
  if(svga) {delete svga;svga=NULL;}

  #ifdef __NT__
    close_win();
  #endif
}

#ifdef __NT__
  void fatal(char *s,...);
#else
  void fatal(char *s,...) {
    static char buf[1024];
    va_list ap;
    va_start(ap,s);
    _vbprintf(buf,1024,s,ap);
    va_end(ap);
    close();
    puts("\nFATAL ERROR\n");
    puts(buf);
    exit(1);
  }
#endif

void Redraw(){
  if (curent) if (curent->get_img()) RD_spr(0,0,curent->get_img());
}

void Draw(){
  if (curent) if (curent->get_img()) svga->drawpic(0,0,curent->get_img());
}

void next_frame(){
  if (curent) curent->next();
  Redraw();
};

void start_flik(){
  if (curent) curent->start();
};

void delete_flik(){
  if (curent) delete curent;
  curent=NULL;
};

void idle();

#ifndef __NT__
void idle(){
}
#endif

char exit_flg=0;
int show_speed=1,pause=0;
int sk=0;

void act() {
  if (!pause) if (++sk>=show_speed) {sk=0;next_frame();}
}

#ifdef __NT__
extern char _app_active;
#endif

void Work(){
  idle();
#ifdef __NT__
  if(!sv_in_window && !_app_active) return;
#endif
  act();//Можно act вызывать реже
  if(!SV_restore()) return;
  RD_end();
  RD_start(rd_data,Draw);
  #ifdef __NT__
//  _swapped_out=0;
  #endif
}

void svga_init() {
#ifndef __NT__
  int w,h,b;
  w=h=b=0;
  if(CFG_read(cfn,"video","width")) w=cfg_val;
  if(CFG_read(cfn,"video","height")) h=cfg_val;
  if(CFG_read(cfn,"video","bits")) b=cfg_val;
  if(CFG_read(cfn,"video","mode")) sv_mode=cfg_val;
  if(CFG_read(cfn,"video","use_vbe2")) use_vbe2=cfg_val;
  if(CFG_read(cfn,"video","use_linear")) use_linear=cfg_val;
  if(CFG_read(cfn,"video","use_pages")) use_pages=cfg_val;
  if(!SV_findmode(w,h,b)) {fatal("Video mode %dx%dx%d not found\n",w,h,b);}
#endif
  if(!SV_setdrv()) {
    fatal("Video mode %dx%dx%d not supported\n",sv_width,sv_height,sv_bits);
  }

  if(SV_lock(0,0,sv_width,sv_height)) {
    svga->setcutrect(0,0,sv_width-1,sv_height-1);
    Draw();
    SV_unlock();
  }
  SV_update_rect_before(0,0,sv_width,sv_height);
  SV_updatescr();
  SV_update_rect_after(0,0,sv_width,sv_height);
}

void init(char *fl) {
  svga_init();
  curent=open_flik(fl);
  if(!(rd_data=RD_init(0,0,sv_width,sv_height)))
    fatal("Not enough memory for redraw tables!");
  RD_start(rd_data,Draw);
}


int main_init(){
  start:;

  while(!exit_flg) {
    Work();
    #ifndef __NT__
      exit_flg=kbhit();
    #endif
  }
  #ifndef __NT__
    int c;
    c=getch();
    if (c=='m') {exit_flg=0;show_speed+=5;if (show_speed>=__MAX_SPEED) show_speed=__MAX_SPEED-1;goto start;};
    if (c=='p') {exit_flg=0;show_speed-=5;if (show_speed<=0) show_speed=__MIN_SPEED;goto start;};
    if (c==32) {exit_flg=0;pause=!pause;goto start;};
    if (c=='s') {exit_flg=0;start_flik();goto start;};
    if(!c) {
      c=getch();
      if (c==77) {exit_flg=0;next_frame();goto start;};
    }
  #endif

  close();
  return 0;
}
#ifndef __NT__
int main(int c,char *argv[]) {
  if (c<=1) init("test.vid"); else init(argv[1]);
  main_init();
  delete_flik();
  return 0;
}
#endif