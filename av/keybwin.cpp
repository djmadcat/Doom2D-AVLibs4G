#define WIN32_LEAN_AND_MEAN
#define DIRECTINPUT_VERSION 0x0300
#include <windows.h>
#include <windowsx.h>
#include <dinput.h>
#include <keyb.h>
#include "dll.h"

DLLEXPORT char volatile keys[256];

static key_f *keyproc=def_key_proc;

DLLEXPORT LPDIRECTINPUT DInput=NULL;
static LPDIRECTINPUTDEVICE lpkeyb=NULL;
static HANDLE event=NULL,thrd=NULL;

static DWORD __stdcall keyb_thread(void *param) {
  char pr=0;
  while(1) {
    WaitForSingleObject(event,pr==2?33:250);
    if(!lpkeyb) break;
    static char k[256];
    HRESULT hr=lpkeyb->GetDeviceState(256,k);
    if(hr==DIERR_INPUTLOST) {
      K_acquire();
      continue;
    }
    if(hr==DI_OK) {
      char pk=0;
      for(int i=0;i<256;++i) {
	char nk=(k[i]&0x80)?255:0;
	if(keys[i]!=nk) {
	  keys[i]=nk;
	  keyproc(i,nk);
	}else keys[i]=nk;
	if(nk) pk=1;
      }
      if(pk) pr=pr?2:1; else pr=0;
    }else{
      K_acquire();
    }
  }
  return 0;
}

extern "C" DLLEXPORT int K_init(HINSTANCE hinst,HWND hwnd) {
  HRESULT hr=DirectInputCreate(hinst, DIRECTINPUT_VERSION, &DInput, NULL);
  if(hr!=DI_OK) return 0;
  hr=DInput->CreateDevice(GUID_SysKeyboard, &lpkeyb, NULL);
  if(hr!=DI_OK) {K_done();return 0;}
  hr=lpkeyb->SetDataFormat(&_c_dfDIKeyboard);
  if(hr!=DI_OK) {K_done();return 0;}
  hr=lpkeyb->SetCooperativeLevel(hwnd,DISCL_FOREGROUND|DISCL_NONEXCLUSIVE);
  if(hr!=DI_OK) {K_done();return 0;}
  event=CreateEvent(0,0,0,0);
  if(!event) {K_done();return 0;}
  DWORD dw;
  thrd=CreateThread(NULL,1024,keyb_thread,NULL,0,&dw);
  if(!thrd) {K_done();return 0;}
  hr=lpkeyb->SetEventNotification(event);
  if(hr!=DI_OK) {K_done();return 0;}
  hr=lpkeyb->Acquire();
//  if(hr!=DI_OK) {K_done();return 0;}
  return 1;
}

extern "C" DLLEXPORT void K_acquire(void) {
  if(lpkeyb) {
    lpkeyb->Acquire();
  }
}

extern "C" DLLEXPORT void K_unacquire(void) {
  if(lpkeyb) {
    lpkeyb->Unacquire();
  }
}

extern "C" DLLEXPORT void K_done(void) {
  if(event) {
    if(lpkeyb) lpkeyb->SetEventNotification(NULL);
    CloseHandle(event);event=NULL;
  }
  if(thrd) {TerminateThread(thrd,0);CloseHandle(thrd);thrd=NULL;}
  if(DInput) {
    if(lpkeyb) {
      lpkeyb->Unacquire();lpkeyb->Release();lpkeyb=NULL;
    }
    DInput->Release();DInput=NULL;
  }
}

extern "C" DLLEXPORT void K_setkeyproc(key_f *p) {
  if(thrd) SuspendThread(thrd);
  keyproc=p;
  if(thrd) ResumeThread(thrd);
}

extern "C" DLLEXPORT key_f *K_getkeyproc() {
  return keyproc;
}

extern "C" DLLEXPORT void def_key_proc(int k,int p) {
}
