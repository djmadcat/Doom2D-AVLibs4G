#define WIN32_LEAN_AND_MEAN
#define DIRECTINPUT_VERSION 0x0300
#include <windows.h>
#include <windowsx.h>
#include <dinput.h>
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
#include <str.h>

int _GUI_common_init();

static LPDIRECTINPUTDEVICE mouse=NULL;
static int mousex=0,mousey=0;

extern "C" int I_mget(int *x,int *y) {
  static DIMOUSESTATE s;
  if(!mouse) {
    if(x) *x=mousex;
    if(y) *y=mousey;
    return 0;
  }
  HRESULT hr=mouse->GetDeviceState(sizeof(s),&s);
  if(hr==DIERR_INPUTLOST) {
    mouse->Acquire();
    hr=mouse->GetDeviceState(sizeof(s),&s);
  }
  if(hr!=DI_OK) {
    if(x) *x=mousex;
    if(y) *y=mousey;
    return 0;
  }
  mousex+=s.lX;
  if(mousex<0) mousex=0; else if(mousex>sv_width) mousex=sv_width-1;
  mousey+=s.lY;
  if(mousey<0) mousey=0; else if(mousey>sv_height) mousey=sv_height-1;
  if(x) *x=mousex;
  if(y) *y=mousey;
  for(int i=0,b=0;i<3;++i) if(s.rgbButtons[i]&0x80) b|=1<<i;
  return b;
}

extern "C" int I_init(HWND hwnd) {
  mousex=mousey=0;
  if(!DInput) return 0;
  HRESULT hr=DInput->CreateDevice(GUID_SysMouse, &mouse, NULL);
  if(hr!=DI_OK) {I_close();return 0;}
  hr=mouse->SetDataFormat(&_c_dfDIMouse);
  if(hr!=DI_OK) {I_close();return 0;}
  hr=mouse->SetCooperativeLevel(hwnd,DISCL_FOREGROUND|DISCL_NONEXCLUSIVE);
  if(hr!=DI_OK) {I_close();return 0;}
  hr=mouse->Acquire();
//  if(hr!=DI_OK) {I_close();return 0;}
  return 1;
}

extern "C" void I_acquire() {
  if(mouse) mouse->Acquire();
}

extern "C" void I_unacquire() {
  if(mouse) mouse->Unacquire();
}

extern "C" void I_close() {
  if(mouse) {
    mouse->Unacquire();mouse->Release();mouse=NULL;
  }
}

void _GUI_idle() {
  while(!process_windows_messages());
}

int GUI_start(int w,int h,HINSTANCE hinst,HWND hwnd,int win) {
  int i;

  if(!K_init(hinst,hwnd)) return 0;
  if(!I_init(hwnd)) {K_done();return 0;}
  if(!SV_findmode(w,h,hwnd,win)) {I_close();K_done();return 0;}
  if(!_GUI_common_init()) return 0;
  return 1;
}
