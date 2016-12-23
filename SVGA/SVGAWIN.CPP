#define WIN32_LEAN_AND_MEAN
#define DIRECTDRAW_VERSION 0x0500
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <ddraw.h>
#include <d3dtypes.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <svga.h>
#include "dll.h"

DLLEXPORT unsigned sv_mem=256*1024;
DLLEXPORT unsigned sv_bpl;
static unsigned sv_pitch;

DLLEXPORT unsigned sv_width,sv_height,sv_bits,sv_pgbank,sv_pagesz,sv_bytespp;

DLLEXPORT char sv_in_window=0;
DLLEXPORT char sv_need3d=0,sv_back_memory=SV_BM_ANY;

DLLEXPORT int sv_curx,sv_cury,sv_step=1,sv_space;
DLLEXPORT ucolor sv_prclr=0xFFFFFFFF;
DLLEXPORT image **sv_font;

static unsigned char *sptr;

static PALETTEENTRY palette[256];
DLLEXPORT LPDIRECTDRAWPALETTE DDpal=NULL;
static LPDIRECTDRAWCLIPPER DDclip=NULL;
static struct{char r,g,b;} curpal[256];

DLLEXPORT LPDIRECTDRAW DDraw=NULL;
DLLEXPORT LPDIRECTDRAWSURFACE DDSmain=NULL;
DLLEXPORT LPDIRECTDRAWSURFACE DDSback=NULL,DDSback2=NULL;

static HWND mainwnd=NULL;

static void conv_palette(int f,int n) {
  if(sv_in_window) {
    HDC hdc=GetDC(NULL);
    GetSystemPaletteEntries(hdc,0,256,palette);
    ReleaseDC(NULL,hdc);
    for(int i=0;i<10;++i) palette[i].peFlags=D3DPAL_READONLY;
    for(;i<246;++i) palette[i].peFlags=D3DPAL_FREE|PC_RESERVED;
    for(;i<256;++i) palette[i].peFlags=D3DPAL_READONLY;
  }
  for(int i=0;i<n;++i) {
    if(sv_in_window) if(i+f<10 || i+f>=246) continue;
    palette[i+f].peRed=curpal[i+f].r*255/63;
    palette[i+f].peGreen=curpal[i+f].g*255/63;
    palette[i+f].peBlue=curpal[i+f].b*255/63;
  }
}

static void finiObjects() {
  if(DDraw) {
    if(sv_back_memory==SV_BM_VIDEO && sv_need3d && !sv_in_window) {
      if(DDSmain) {DDSmain->Release();DDSback=NULL;DDSmain=NULL;DDclip=NULL;}
    }
    if(DDSmain) {DDSmain->Release();DDSmain=NULL;DDclip=NULL;}
    if(DDSback) {DDSback->Release();DDSback=NULL;}
    if(DDSback2) {DDSback2->Release();DDSback2=NULL;}
    if(DDpal) {DDpal->Release();DDpal=NULL;}
    DDraw->Release();
    DDraw=NULL;
  }
}

static char *dd_error(HRESULT hr) {
  static char buf[128];

  char *en;
  switch(hr) {
    case DDERR_EXCEPTION:        en="Exception";break;
    case DDERR_GENERIC:          en="Unknown error";break;
    case DDERR_OUTOFMEMORY:      en="Not enough memory";break;
    case DDERR_OUTOFVIDEOMEMORY: en="Not enough VIDEO memory";break;
    case DDERR_SURFACEBUSY:      en="Unable to access screen surface";break;
    case DDERR_SURFACELOST:      en="Surface lost";break;
    case DDERR_WRONGMODE:        en="Wrong mode";break;
    default:                     en="Internal error";break;
  }
  wsprintf(buf,"DirectDraw error:\n%s (Error %08X)",en,hr);
  return buf;
}

//лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл//

extern "C" DLLEXPORT int SV_ready_to_draw() {
  return 1;
}

static int restore_surf() {
  HRESULT ddrval;
  ddrval=DDSmain->Restore();
  if(ddrval!=DD_OK) return 0;
  ddrval=DDSback->Restore();
  if(ddrval!=DD_OK) return 0;
  if(DDSback2) {
    ddrval=DDSback2->Restore();
    if(ddrval!=DD_OK) return 0;
  }
  if(sv_bits==8) {
    DDSmain->SetPalette(DDpal);
//    DDSback->SetPalette(DDpal);
  }
  return 1;
}

extern "C" DLLEXPORT int SV_restore() {
  if(!DDraw) return 0;
  if(!DDSmain) return 0;
  if(!DDSback) return 0;
  if(DDSback2) if(DDSback2->IsLost()!=DD_OK) return restore_surf();
  if(DDSmain->IsLost()==DD_OK && DDSback->IsLost()==DD_OK) return 1;
  return restore_surf();
}

extern "C" DLLEXPORT int SV_updatescr() {
  if(sv_in_window) return 1;
  if(sv_back_memory==SV_BM_SYSTEM) return 1;
  HRESULT ddrval;
  while(1) {
    ddrval=DDSmain->Flip(NULL,0);
    if(ddrval==DD_OK) break;
    if(ddrval==DDERR_SURFACELOST) {
      if(!restore_surf()) return 0;
    }else if(ddrval!=DDERR_WASSTILLDRAWING) break;
  }
  return (ddrval==DD_OK);
}

extern "C" DLLEXPORT int SV_update_rect_before(int x,int y,int w,int h) {
  HRESULT ddrval=DD_OK;
  if(sv_in_window) {
    POINT pt; pt.x=pt.y=0;
    ClientToScreen(mainwnd,&pt);
    RECT rc; rc.left=x;rc.top=y;rc.right=x+w;rc.bottom=y+h;
    RECT dr; dr.left=x+pt.x;dr.top=y+pt.y;
    dr.right=x+pt.x+w;dr.bottom=y+pt.y+h;
    while(1) {
      ddrval=DDSmain->Blt(&dr,DDSback,&rc,DDBLT_WAIT,NULL);
      if(ddrval==DD_OK) break;
      if(ddrval==DDERR_SURFACELOST) {
	if(!restore_surf()) return 0;
      }else if(ddrval!=DDERR_WASSTILLDRAWING) break;
    }
  }else if(sv_back_memory==SV_BM_SYSTEM) {
    RECT rc; rc.left=x;rc.top=y;rc.right=x+w;rc.bottom=y+h;
    while(1) {
      ddrval=DDSmain->BltFast(x,y,DDSback,&rc,
	DDBLTFAST_NOCOLORKEY|DDBLTFAST_WAIT);
      if(ddrval==DD_OK) break;
      if(ddrval==DDERR_SURFACELOST) {
	if(!restore_surf()) return 0;
      }else if(ddrval!=DDERR_WASSTILLDRAWING) break;
    }
  }else{
    RECT rc; rc.left=x;rc.top=y;rc.right=x+w;rc.bottom=y+h;
    while(1) {
      ddrval=DDSback->BltFast(x,y,DDSback2,&rc,
	DDBLTFAST_NOCOLORKEY|DDBLTFAST_WAIT);
      if(ddrval==DD_OK) break;
      if(ddrval==DDERR_SURFACELOST) {
	if(!restore_surf()) return 0;
      }else if(ddrval!=DDERR_WASSTILLDRAWING) break;
    }
    while(1) {
      ddrval=DDSmain->BltFast(x,y,DDSback2,&rc,
	DDBLTFAST_NOCOLORKEY|DDBLTFAST_WAIT);
      if(ddrval==DD_OK) break;
      if(ddrval==DDERR_SURFACELOST) {
	if(!restore_surf()) return 0;
      }else if(ddrval!=DDERR_WASSTILLDRAWING) break;
    }
  }
  return (ddrval==DD_OK);
}

extern "C" DLLEXPORT int SV_update_rect_after(int x,int y,int w,int h) {
  HRESULT ddrval=DD_OK;
  if(sv_in_window) {
  }else if(sv_back_memory==SV_BM_SYSTEM) {
  }else{
/*
    RECT rc; rc.left=x;rc.top=y;rc.right=x+w;rc.bottom=y+h;
    while(1) {
      ddrval=DDSback->BltFast(x,y,DDSback2,&rc,
	DDBLTFAST_NOCOLORKEY|DDBLTFAST_WAIT);
      if(ddrval==DD_OK) break;
      if(ddrval==DDERR_SURFACELOST) {
	if(!restore_surf()) return 0;
      }else if(ddrval!=DDERR_WASSTILLDRAWING) break;
    }
*/
  }
  return (ddrval==DD_OK);
}

static int lockx,locky,lockw,lockh;

extern "C" DLLEXPORT int SV_lock(int x,int y,int w,int h) {
  DDSURFACEDESC sd;
  HRESULT ddrval;
  RECT rt;
  rt.left=x;rt.top=y;rt.right=x+w;rt.bottom=y+h;
  while(1) {
    memset(&sd,0,sizeof(sd));
    sd.dwSize=sizeof(sd);
    if(sv_back_memory==SV_BM_SYSTEM || sv_in_window)
      ddrval=DDSback->Lock(NULL,&sd,0,0);
    else
      ddrval=DDSback2->Lock(NULL,&sd,0,0);
    if(ddrval==DD_OK) break;
    if(ddrval==DDERR_SURFACELOST) {
      if(!restore_surf()) return 0;
    }else if(ddrval!=DDERR_WASSTILLDRAWING) break;
  }
  if(ddrval!=DD_OK) return 0;
  lockx=x;locky=y;lockw=w;lockh=h;
  sptr=(unsigned char*)sd.lpSurface;
  sv_pitch=sd.lPitch;
  svga->setbuf(sptr,sv_width,sv_height,sv_pitch);
  svga->setcutrect(x,y,x+w-1,y+h-1);
  return 1;
}

extern "C" DLLEXPORT int SV_unlock() {
  HRESULT ddrval;
  while(1) {
    if(sv_back_memory==SV_BM_SYSTEM || sv_in_window)
      ddrval=DDSback->Unlock((LPVOID)sptr);
    else
      ddrval=DDSback2->Unlock((LPVOID)sptr);
    if(ddrval==DD_OK) break;
    if(ddrval==DDERR_SURFACELOST) {
      if(!restore_surf()) return 0;
    }else if(ddrval!=DDERR_WASSTILLDRAWING) break;
  }
  return (ddrval==DD_OK);
}

DLLEXPORT int SV_release_lock() {
  return SV_unlock();
}

DLLEXPORT int SV_restore_lock() {
  return SV_lock(lockx,locky,lockw,lockh);
}

extern "C" DLLEXPORT int SV_lock_hw(int x,int y,int w,int h) {
  if(sv_back_memory==SV_BM_SYSTEM) return 0;
  DDSURFACEDESC sd;
  HRESULT ddrval;
  RECT rt;
  rt.left=x;rt.top=y;rt.right=x+w;rt.bottom=y+h;
  while(1) {
    memset(&sd,0,sizeof(sd));
    sd.dwSize=sizeof(sd);
    ddrval=DDSback->Lock(NULL,&sd,0,0);
    if(ddrval==DD_OK) break;
    if(ddrval==DDERR_SURFACELOST) {
      if(!restore_surf()) return 0;
    }else if(ddrval!=DDERR_WASSTILLDRAWING) break;
  }
  if(ddrval!=DD_OK) return 0;
  sptr=(unsigned char*)sd.lpSurface;
  sv_pitch=sd.lPitch;
  svga->setbuf(sptr,sv_width,sv_height,sv_pitch);
  svga->setcutrect(x,y,x+w-1,y+h-1);
  return 1;
}

extern "C" DLLEXPORT int SV_unlock_hw() {
  if(sv_back_memory==SV_BM_SYSTEM) return 0;
  HRESULT ddrval;
  while(1) {
    ddrval=DDSback->Unlock((LPVOID)sptr);
    if(ddrval==DD_OK) break;
    if(ddrval==DDERR_SURFACELOST) {
      if(!restore_surf()) return 0;
    }else if(ddrval!=DDERR_WASSTILLDRAWING) break;
  }
  return (ddrval==DD_OK);
}

static char *en="";
static HRESULT ddrval;

static BOOL init_svga(int &w,int &h,int &b) {
  DDSURFACEDESC ddsd;
  DDPIXELFORMAT pf;
  DDSCAPS sc;

  if(!mainwnd) return 0;

  if (!DDraw) {
    ddrval=DirectDrawCreate( NULL, &DDraw, NULL );
    if(ddrval!=DD_OK) return 0;
  }
/*
  en="QueryInterface : DirectDraw2";
  if(lpdd->QueryInterface(IID_IDirectDraw2,(void**)&DDraw)!=DD_OK) return 0;
  lpdd->Release();
*/
  en="SetCooperativeLevel";
  ddrval = DDraw->SetCooperativeLevel( mainwnd,
    sv_in_window ? (DDSCL_NORMAL|DDSCL_ALLOWREBOOT):
    (DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN|DDSCL_ALLOWREBOOT)
  );
  if(ddrval!=DD_OK) return 0;
//  DWORD freq=0;
  if(!sv_in_window) {
    en="SetDisplayMode";
/*
    LPDIRECTDRAW2 lpdd;
    if(DDraw->QueryInterface(IID_IDirectDraw2,(void**)&lpdd)==DD_OK) {
      if(lpdd->GetMonitorFrequency(&freq)!=DD_OK) freq=0;
      lpdd->Release();
    }
*/
    ddrval=DDraw->SetDisplayMode(w,h,b);
    if(ddrval!=DD_OK) return 0;
  }

  en="CreateSurface : Primary";
  memset(&ddsd,0,sizeof(ddsd));
  ddsd.dwSize=sizeof(ddsd);
  ddsd.dwFlags=DDSD_CAPS;
/*
  if(!sv_in_window) {
    ddsd.dwFlags|=DDSD_REFRESHRATE;
    ddsd.dwRefreshRate=100;
  }
*/
  ddsd.ddsCaps.dwCaps=DDSCAPS_PRIMARYSURFACE;
//  if(sv_need3d) ddsd.ddsCaps.dwCaps|=DDSCAPS_3DDEVICE;
  ddrval=DDraw->CreateSurface(&ddsd,&DDSmain,NULL);
  if(ddrval!=DD_OK) return 0;

  en="CreateSurface : Back buffer";
  memset(&ddsd,0,sizeof(ddsd));
  ddsd.dwSize=sizeof(ddsd);
  ddsd.dwFlags=DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT;
  ddsd.dwWidth=w;ddsd.dwHeight=h;
  ddsd.ddsCaps.dwCaps=DDSCAPS_BACKBUFFER;
//  if(!sv_need3d) {
    if(sv_back_memory==SV_BM_SYSTEM)
      ddsd.ddsCaps.dwCaps|=DDSCAPS_SYSTEMMEMORY;
    if(sv_back_memory==SV_BM_VIDEO)
      ddsd.ddsCaps.dwCaps|=DDSCAPS_VIDEOMEMORY;
//  }
  if(sv_need3d) ddsd.ddsCaps.dwCaps|=DDSCAPS_3DDEVICE;
  ddrval=DDraw->CreateSurface(&ddsd,&DDSback,NULL);
  if(ddrval!=DD_OK) return 0;
  en="GetCaps";
  ddrval=DDSback->GetCaps(&sc);
  if(ddrval!=DD_OK) return 0;
  if(sc.dwCaps&DDSCAPS_SYSTEMMEMORY) sv_back_memory=SV_BM_SYSTEM;
  if(sc.dwCaps&DDSCAPS_VIDEOMEMORY) sv_back_memory=SV_BM_VIDEO;

  if(sv_in_window) {
    en="CreateClipper";
    ddrval = DDraw->CreateClipper( 0, &DDclip, NULL );
    if(ddrval!=DD_OK) return 0;
    en="Clipper::SetHWnd";
    ddrval = DDclip->SetHWnd(0,mainwnd);
    if(ddrval!=DD_OK) return 0;
    en="SetClipper";
    ddrval = DDSmain->SetClipper(DDclip);
    if(ddrval!=DD_OK) return 0;
  }else if(sv_back_memory==SV_BM_VIDEO && sv_need3d) {
    DDSmain->Release();
    DDSback->Release();
    en="CreateSurface (2) : Primary";
    memset(&ddsd,0,sizeof(ddsd));
    ddsd.dwSize=sizeof(ddsd);
    ddsd.dwFlags=DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
    ddsd.ddsCaps.dwCaps=DDSCAPS_PRIMARYSURFACE|DDSCAPS_FLIP|DDSCAPS_COMPLEX
      |DDSCAPS_3DDEVICE|DDSCAPS_VIDEOMEMORY;
    ddsd.dwBackBufferCount=1;
    ddrval=DDraw->CreateSurface(&ddsd,&DDSmain,NULL);
    if(ddrval!=DD_OK) return 0;
    en="GetAttachedSurface";
    sc.dwCaps=DDSCAPS_BACKBUFFER;
    ddrval=DDSmain->GetAttachedSurface(&sc,&DDSback);
    if(ddrval!=DD_OK) return 0;
  }else if(sv_back_memory==SV_BM_VIDEO) {
    en="AddAttachedSurface";
    ddrval=DDSmain->AddAttachedSurface(DDSback);
    if(ddrval!=DD_OK) return 0;
  }

  if(sv_back_memory==SV_BM_VIDEO) {
    en="CreateSurface : Back buffer 2";
    memset(&ddsd,0,sizeof(ddsd));
    ddsd.dwSize=sizeof(ddsd);
    ddsd.dwFlags=DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT;
    ddsd.dwWidth=w;ddsd.dwHeight=h;
    ddsd.ddsCaps.dwCaps=DDSCAPS_OFFSCREENPLAIN|DDSCAPS_SYSTEMMEMORY;
    ddrval=DDraw->CreateSurface(&ddsd,&DDSback2,NULL);
    if(ddrval!=DD_OK) return 0;
  }

  en="GetPixelFormat";
  pf.dwSize=sizeof(pf);
  ddrval=DDSmain->GetPixelFormat(&pf);
  if(ddrval!=DD_OK) return 0;
  en="Unsupported pixel format";
  if(pf.dwFlags&DDPF_PALETTEINDEXED8) b=8;
  else if(pf.dwFlags&DDPF_RGB) {
    switch(pf.dwRGBBitCount) {
      case 16:
	if(pf.dwBBitMask!=0x001F) return 0;
	if(pf.dwRBitMask==0xF800 && pf.dwGBitMask==0x07E0) b=16;
	else if(pf.dwRBitMask==0x7C00 && pf.dwGBitMask==0x03E0) b=15;
	else return 0;
	break;
      case 32: b=32;break;
      case 24: b=24;break;
      default:
	return 0;
    }
  }else return 0;

  if(b!=8) return 1;
  en="CreatePalette";
  conv_palette(0,256);
  ddrval=DDraw->CreatePalette(DDPCAPS_8BIT|(sv_in_window?0:DDPCAPS_ALLOW256),
    palette,&DDpal,NULL);
  if(ddrval!=DD_OK) return 0;
  en="SetPalette : Primary";
  ddrval=DDSmain->SetPalette(DDpal);
  if(ddrval!=DD_OK) return 0;
  en="SetPalette : Back buffer";
  ddrval=DDSback->SetPalette(DDpal);
  if(ddrval!=DD_OK) return 0;
  return 1;
}

static BOOL doInit(int &w,int &h,int &b) {
  if(init_svga(w,h,b)) return 1;
  MessageBox( mainwnd, dd_error(ddrval), en, MB_OK );
  finiObjects();
  return FALSE;
}

extern "C" DLLEXPORT int SV_findmode(int w,int h,int b,HWND hwnd,int win) {
  mainwnd=hwnd;
  sv_in_window=win;
  if(!doInit(w,h,b)) return 0;
  sv_width=w;sv_height=h;sv_bits=b;
  sv_bytespp=(b+7)/8;
  sv_bpl=sv_width;
  return 1;
}

extern "C" DLLEXPORT void SV_done(void) {
  finiObjects();
}

extern "C" DLLEXPORT void SV_setpage(int p) {
}

extern "C" DLLEXPORT void SV_setvpage(int p) {
}

/*
static void copytoscr(int x,int y,int w,int h,void *p,int b) {
  x=y*sv_pitch+x*sv_bytespp;
  w*=sv_bytespp;
  for(;h;x+=sv_pitch,--h) {
    memcpy(sptr+x,p,w);
    p=(void*)((char*)p+b);
  }
}
*/

extern "C" DLLEXPORT void SV_copytoscr(int x,int y,int w,int h,void *p,int b) {
/*
  if(!lock_surf()) return;
  if(sv_in_window) {
    POINT pt; pt.x=pt.y=0;
    ClientToScreen(mainwnd,&pt);
    x+=pt.x;y+=pt.y;
    RECT rc; rc.left=x;rc.top=y;rc.right=x+w;rc.bottom=y+h;
    DWORD sz;
    if(DDclip->GetClipList(&rc,NULL,&sz)==DD_OK) {
      LPRGNDATA rd=(LPRGNDATA)malloc(sz);
      if(rd) {
	rd->rdh.dwSize=sizeof(rd->rdh);
	rd->rdh.iType=RDH_RECTANGLES;
	rd->rdh.nCount=(sz-sizeof(rd->rdh))/sizeof(RECT);
	rd->rdh.nRgnSize=0;
	if(DDclip->GetClipList(&rc,rd,&sz)==DD_OK) {
	  RECT *r=(RECT*)rd->Buffer;
	  for(int i=0;i<rd->rdh.nCount;++i,++r)
	    copytoscr(r->left,r->top,r->right-r->left,r->bottom-r->top,
	      (char*)p+(r->top-y)*b+(r->left-x)*sv_bytespp,b);
	}
	free(rd);
      }
    }
  }else copytoscr(x,y,w,h,p,b);
  if(!unlock_surf()) {exerr="unlock";return;}
*/
}

extern "C" DLLEXPORT void SV_setpal(void *p,int f,int n) {
  memcpy(curpal+f,p,n*3);
  conv_palette(f,n);
  if(DDpal) {
    DDpal->SetEntries(0,f,n,palette+f);
    DDSmain->SetPalette(DDpal);
//    DDSback->SetPalette(DDpal);
  }
}
