#define WIN32_LEAN_AND_MEAN
#define DIRECTDRAW_VERSION 0x0300
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <ddraw.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <svga8.h>

unsigned sv_mem=256*1024;
unsigned sv_bpl,sv_pitch;

unsigned sv_width,sv_height,sv_bits,sv_pgbank,sv_pagesz,sv_bytespp;

char sv_in_window=0;

static unsigned char *sptr;

static PALETTEENTRY palette[256];
static LPDIRECTDRAWPALETTE     DDpal;
static vgapal curpal;

LPDIRECTDRAW            DDraw;
static LPDIRECTDRAWSURFACE     DDSmain;
//static LPDIRECTDRAWSURFACE     DDSback;

static HWND mainwnd=NULL;

static void conv_palette(int f,int n) {
  for(int i=0;i<n;++i) {
    palette[i+f].peRed=curpal[i+f].r*255/63;
    palette[i+f].peGreen=curpal[i+f].g*255/63;
    palette[i+f].peBlue=curpal[i+f].b*255/63;
  }
}

static void finiObjects( void ) {
  if(DDraw) {
    if(DDSmain) {DDSmain->Release();DDSmain=NULL;}
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

static char *exerr="Unknown";
static HRESULT exhr=DD_OK;

/*
static int flip_surf(void) {
  HRESULT ddrval;
  while(1) {
    ddrval=DDSmain->Flip( NULL, 0 );
    if(ddrval==DD_OK) break;
    if(ddrval==DDERR_SURFACELOST) {
      ddrval=DDSmain->Restore();
      if(ddrval!=DD_OK) break;
    }else if(ddrval!=DDERR_WASSTILLDRAWING) break;
  }
  exhr=ddrval;
  return (ddrval==DD_OK);
}
*/

static int lock_surf(void) {
  DDSURFACEDESC sd;
  HRESULT ddrval;

  exerr="Lock";
  while(1) {
    sd.dwSize=sizeof(sd);
    ddrval=DDSmain->Lock(NULL,&sd,0,0);
    if(ddrval==DD_OK) break;
    if(ddrval==DDERR_SURFACELOST) {
      ddrval=DDSmain->Restore();
      exerr="Lock: restore";
      if(ddrval!=DD_OK) break;
      DDSmain->SetPalette(DDpal);
    }else if(ddrval!=DDERR_WASSTILLDRAWING) break;
  }
  exhr=ddrval;
  if(ddrval!=DD_OK) return 0;
  sptr=(unsigned char *)sd.lpSurface;
  sv_pitch=sd.lPitch;
  return 1;
}

static int unlock_surf(void) {
  HRESULT ddrval;
  while(1) {
    ddrval=DDSmain->Unlock((LPVOID)scra);
    if(ddrval==DD_OK) break;
    if(ddrval==DDERR_SURFACELOST) {
      ddrval=DDSmain->Restore();
      if(ddrval!=DD_OK) break;
    }else if(ddrval!=DDERR_WASSTILLDRAWING) break;
  }
  exhr=ddrval;
  return (ddrval==DD_OK);
}

static BOOL doInit(int w,int h) {
  DDSURFACEDESC       ddsd;
//  DDSCAPS             ddscaps;
  HRESULT             ddrval;

  if( !mainwnd )
      return FALSE;

  char *en="DirectDrawCreate";
  ddrval = DirectDrawCreate( NULL, &DDraw, NULL );
  if( ddrval == DD_OK ) {
    en="SetCooperativeLevel";
    ddrval = DDraw->SetCooperativeLevel( mainwnd,
      sv_in_window ? (DDSCL_NORMAL|DDSCL_ALLOWREBOOT):
      (DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN|DDSCL_ALLOWREBOOT)
    );
    if(ddrval == DD_OK ) {
      if(!sv_in_window) {
	en="SetDisplayMode";
	ddrval = DDraw->SetDisplayMode( w, h, 8 );
      }
      if( ddrval == DD_OK ) {
	en="CreateSurface";
	ddsd.dwSize = sizeof( ddsd );
	ddsd.dwFlags = (!sv_in_window)?(DDSD_CAPS):
	  (DDSD_CAPS/*|DDSD_BACKBUFFERCOUNT|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT*/);
	ddsd.ddsCaps.dwCaps = (!sv_in_window)?(DDSCAPS_PRIMARYSURFACE):
	  (DDSCAPS_PRIMARYSURFACE/*|DDSCAPS_FLIP|DDSCAPS_COMPLEX*/);
/*
	ddsd.dwBackBufferCount=1;
	ddsd.dwWidth=w;ddsd.dwHeight=h;
	ddsd.ddpfPixelFormat.dwSize=sizeof(DDPIXELFORMAT);
	ddsd.ddpfPixelFormat.dwFlags=DDPF_PALETTEINDEXED8;
*/
	ddrval = DDraw->CreateSurface( &ddsd, &DDSmain, NULL );
	if(ddrval==DD_OK) {
/*
	  if(sv_in_window) {
	    en="GetAttachedSurface";
	    ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
	    ddrval = DDSmain->GetAttachedSurface(&ddscaps,&DDSback);
	  }
*/
	  if(ddrval==DD_OK) {
	    en="CreatePalette";
	    conv_palette(0,256);
	    ddrval=DDraw->CreatePalette(DDPCAPS_8BIT|DDPCAPS_ALLOW256,
	      palette,&DDpal,NULL);
	    if(ddrval==DD_OK) {
	      en="SetPalette";
	      ddrval=DDSmain->SetPalette(DDpal);
	      if(ddrval==DD_OK) {
		return TRUE;
	      }
	    }
	  }
	}
      }
    }
  }

  MessageBox( mainwnd, dd_error(ddrval), en, MB_OK );
  finiObjects();
  return FALSE;
}

extern "C" int SV_findmode(int w,int h,HWND hwnd,int win) {
  mainwnd=hwnd;
  sv_in_window=win;
  if(!doInit(w,h)) return 0;
  sv_width=w;sv_height=h;
  sv_bpl=sv_width;
  if(!(scrbuf=(unsigned char*)malloc(sv_width*sv_height)))
    {SV_done();return 0;}
  scra=scrbuf;
  scrx=scry=0;
  scrw=sv_width;
  scrh=sv_height;
  return 1;
}

extern "C" void SV_done(void) {
  finiObjects();
  if(scrbuf) {free(scrbuf);scrbuf=NULL;}
}

extern "C" void SV_copytoscr(int x,int y,int w,int h,void *p,int b) {
  if(!lock_surf()) return;
  x=y*sv_pitch+x;
  for(;h;x+=sv_pitch,--h) {
    memcpy(sptr+x,p,w);
    p=(void*)((char*)p+b);
  }
  if(!unlock_surf()) {exerr="unlock";return;}
}

extern "C" void V_copytoscr(short x,short w,short y,short h) {
  SV_copytoscr(x,y,w,h,scra+y*sv_width+x,sv_width);
}

extern "C" void VP_set(void *p,short f,short n) {
  memcpy(curpal+f,p,n*3);
  conv_palette(f,n);
  if(DDpal) DDpal->SetEntries(0,f,n,palette+f);
}

extern "C" void VP_setall(void *p) {
  VP_set(p,0,256);
}

extern "C" void VP_fill(char r,char g,char b) {
  for(int i=0;i<256;++i) {
    curpal[i].r=r;curpal[i].g=g;curpal[i].b=b;
  }
  conv_palette(0,256);
  if(DDpal) DDpal->SetEntries(0,0,256,palette);
}
