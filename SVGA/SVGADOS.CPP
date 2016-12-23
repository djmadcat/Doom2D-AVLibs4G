#include <svga.h>

extern "C" extern unsigned char *scrbuf;

extern "C" int SV_ready_to_draw() {
  return 1;
}

extern "C" int SV_restore() {
  return 1;
}

extern "C" int SV_lock(int x,int y,int w,int h) {
  svga->setbuf(scrbuf,sv_width,sv_height,sv_width*sv_bytespp);
  svga->setcutrect(x,y,x+w-1,y+h-1);
  return 1;
}

extern "C" int SV_unlock() {
  return 1;
}

extern "C" int SV_lock_hw(int x,int y,int w,int h) {
  return 1;
}

extern "C" int SV_unlock_hw() {
  return 1;
}
