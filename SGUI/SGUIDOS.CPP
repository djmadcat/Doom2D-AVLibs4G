#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <av.h>
#include <averr.h>
#include <stdlib.h>
#include <string.h>
#include <svga8.h>
#include <keyb.h>
#include <sgui.h>

int _GUI_common_init();

void _GUI_idle() {}

int GUI_start(int w,int h) {
  I_init();K_init();
  if(!SV_findmode(w,h)) {K_done();I_close();return 0;}
  if(!_GUI_common_init()) return 0;
  return 1;
}
