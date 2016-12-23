
#ifndef __SVGA_LIB_H
#define __SVGA_LIB_H

#ifndef _DLL
#pragma library("svga.lib")
#define DLLEXTERN extern
#else
#ifndef __B_SVGALIB
#pragma library("svgadll.lib")
#define DLLEXTERN __declspec(dllimport)
#else
#define DLLEXTERN extern
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned ucolor;
typedef union{
  ucolor uc;
  struct{unsigned char b,g,r,a;};
}color32;
typedef struct{unsigned char b,g,r;}color24;
typedef unsigned short color16;

typedef struct{unsigned char r,g,b;}vgargb;

typedef struct{short w,h,sx,sy;}image;

DLLEXTERN ucolor SV_rgb8(int r,int g,int b);

#ifndef __NT__
DLLEXTERN int SV_findmode(int w,int h,int b);
#else
#ifdef _WINDOWS_
DLLEXTERN int SV_findmode(int w,int h,int b,HWND,int window);
#endif
#endif

DLLEXTERN int SV_setdrv(void);

DLLEXTERN void SV_done(void);

DLLEXTERN void SV_setpal(void *,int,int);

DLLEXTERN int SV_lock(int x,int y,int w,int h); // calls svga->setbuf() and setcutrect()
DLLEXTERN int SV_unlock();
// lock hardware memory
DLLEXTERN int SV_lock_hw(int x,int y,int w,int h);
DLLEXTERN int SV_unlock_hw();

DLLEXTERN int SV_update_rect_before(int x,int y,int w,int h);
DLLEXTERN int SV_updatescr();
DLLEXTERN int SV_update_rect_after(int x,int y,int w,int h);

DLLEXTERN int SV_restore();

DLLEXTERN int SV_ready_to_draw();

DLLEXTERN void SV_setpage(int);
DLLEXTERN void SV_setvpage(int);

typedef void redraw_f(void);

DLLEXTERN void *RD_init(int x,int y,int w,int h);
DLLEXTERN void RD_release(void*);

DLLEXTERN int RD_add_hw_rectwh(void*,int x,int y,int w,int h); // returns id or -1 on error
DLLEXTERN void RD_release_hw_rect(void*,int id);

DLLEXTERN void RD_start(void *,redraw_f *);
DLLEXTERN void RD_end(void);

DLLEXTERN void RD_rect(int l,int t,int r,int b);
DLLEXTERN void RD_rectwh(int x,int y,int w,int h);
DLLEXTERN void RD_spr(int x,int y,image *);

DLLEXTERN image *load_img(const char *fn);

typedef int skipch_f(char);

DLLEXTERN int SV_txtlen(char *);
DLLEXTERN int SV_txtnlen(char *,int);
DLLEXTERN int SV_txtlen_spec(char *,int,skipch_f*);

DLLEXTERN void SV_putc(char);
DLLEXTERN void SV_putn(char *,int);
DLLEXTERN void SV_putn_spec(char *,int,skipch_f*);
DLLEXTERN void SV_puts(char *);
DLLEXTERN void SV_putsxy(int x,int y,char *);
DLLEXTERN void SV_prf(char *,...);
DLLEXTERN void SV_prfxy(int x,int y,char *,...);
DLLEXTERN void SV_gotoxy(int x,int y);

DLLEXTERN unsigned sv_width,sv_height,sv_bits,sv_bytespp,sv_bpl,sv_mem;

DLLEXTERN char use_pages;

#ifndef __NT__
DLLEXTERN char use_vbe2,use_linear,vbe2,linear,use_pages;
DLLEXTERN unsigned sv_mode;
#else
enum{SV_BM_ANY,SV_BM_SYSTEM,SV_BM_VIDEO};
DLLEXTERN char sv_in_window,sv_need3d,sv_back_memory;
#ifdef __DDRAW_INCLUDED__
DLLEXTERN LPDIRECTDRAW DDraw;
DLLEXTERN LPDIRECTDRAWSURFACE DDSmain,DDSback,DDSback2;
#endif
#endif

DLLEXTERN int sv_curx,sv_cury,sv_step,sv_space;
DLLEXTERN ucolor sv_prclr;
DLLEXTERN image **sv_font;

#define shade15(c,a) (((svga_drv15*)svga)->stabb[(((c)<<8)&0x1F00)+(a)] \
  |((svga_drv15*)svga)->stabg[(((c)<<3)&0x1F00)+(a)] \
  |((svga_drv15*)svga)->stabr[(((c)>>2)&0x1F00)+(a)])

#define shade16(c,a) (((svga_drv16*)svga)->stabb[(((c)<<8)&0x1F00)+(a)] \
  |((svga_drv16*)svga)->stabg[(((c)<<2)&0x1F00)+(a)] \
  |((svga_drv16*)svga)->stabr[(((c)>>3)&0x1F00)+(a)])

#define shade24(c,a,RGB) (((svga_drv24*)svga)->shtab[(c).RGB+((a)<<8)])

#define shade32(c,a,RGB) (((svga_drv32*)svga)->shtab[(c).RGB+((a)<<8)])

#define shade8(c,a) (((svga_drv8*)svga)->shtab[((a)<<8)+(c)])

#define mix8(a,b) (((svga_drv8*)svga)->mtab[(a<<8)|b])

#ifdef __cplusplus
}


struct svga_drv{
  int cutl,cutt,cutr,cutb;
  int pitch,scrwidth,scrheight;
  svga_drv();
  virtual ~svga_drv();
  virtual int is_ok()=0;
  virtual void getcutrect(int &l,int &t,int &r,int &b);
  virtual void setcutrect(int l,int t,int r,int b);
  virtual int intersectrect(int l,int t,int r,int b);
  virtual int intersectrectwh(int x,int y,int w,int h);
  virtual ucolor rgb(int r,int g,int b);
  virtual color24 toc24(ucolor);
  virtual ucolor shade(ucolor,unsigned char);
  virtual void dot(int x,int y,ucolor);
  virtual color24 getdot(int x,int y);
  virtual image *convpic32(image *);
  virtual image *convspr32(image *);
  virtual void drawpic(int x,int y,image *);
  virtual void drawspr(int x,int y,image *);
  virtual void clear(int l,int t,int r,int b,ucolor);
  virtual void clearwh(int x,int y,int w,int h,ucolor);
  virtual void monorect(int x,int y,int w,int h,image *,int sx,int sy,ucolor *);
  virtual void monospr(int x,int y,image *,ucolor);
  virtual void shaderect(int x,int y,int w,int h,unsigned char *);
  virtual void setbuf(void *buf,int w,int h,int pitch);
  virtual void drawspr(int x,int y,int w,int h,image *);
};

struct svga_drv8:svga_drv{
  unsigned char *scr;
  vgargb pal[256];
  unsigned char *ftab,*shtab,*mtab;
  svga_drv8(char *fn="vgapal.dat");
  virtual ~svga_drv8();
  int is_ok();
  virtual ucolor shade(ucolor,unsigned char);
  virtual ucolor rgb(int r,int g,int b);
  virtual color24 toc24(ucolor);
  virtual void dot(int x,int y,ucolor);
  virtual color24 getdot(int x,int y);
  virtual image *convpic32(image *);
  virtual image *convspr32(image *);
  virtual void drawpic(int x,int y,image *);
  virtual void drawspr(int x,int y,image *);
  virtual void clearwh(int x,int y,int w,int h,ucolor);
  virtual void monorect(int x,int y,int w,int h,image *,int sx,int sy,ucolor *);
  virtual void monospr(int x,int y,image *,ucolor);
  virtual void shaderect(int x,int y,int w,int h,unsigned char *);
  virtual void setbuf(void *buf,int w,int h,int pitch);
  virtual void drawspr(int x,int y,int w,int h,image *);
};

struct svga_drv32:svga_drv{
  color32 *scr;
  unsigned char *shtab;
  svga_drv32();
  virtual ~svga_drv32();
  int is_ok();
  virtual ucolor rgb(int r,int g,int b);
  virtual color24 toc24(ucolor);
  virtual ucolor shade(ucolor,unsigned char);
  virtual void dot(int x,int y,ucolor);
  virtual color24 getdot(int x,int y);
  virtual void drawpic(int x,int y,image *);
  virtual void drawspr(int x,int y,image *);
  virtual void clearwh(int x,int y,int w,int h,ucolor);
  virtual void monorect(int x,int y,int w,int h,image *,int sx,int sy,ucolor *);
  virtual void monospr(int x,int y,image *,ucolor);
  virtual void shaderect(int x,int y,int w,int h,unsigned char *);
  virtual image *convspr32(image *);
  virtual void setbuf(void *buf,int w,int h,int pitch);
  virtual void drawspr(int x,int y,int w,int h,image *);
};

struct svga_drv24:svga_drv{
  color24 *scr;
  unsigned char *shtab;
  svga_drv24();
  virtual ~svga_drv24();
  int is_ok();
  virtual ucolor rgb(int r,int g,int b);
  virtual color24 toc24(ucolor);
  virtual ucolor shade(ucolor,unsigned char);
  virtual void dot(int x,int y,ucolor);
  virtual color24 getdot(int x,int y);
  virtual image *convpic32(image *);
  virtual image *convspr32(image *);
  virtual void drawpic(int x,int y,image *);
  virtual void drawspr(int x,int y,image *);
  virtual void clearwh(int x,int y,int w,int h,ucolor);
  virtual void monorect(int x,int y,int w,int h,image *,int sx,int sy,ucolor *);
  virtual void monospr(int x,int y,image *,ucolor);
  virtual void shaderect(int x,int y,int w,int h,unsigned char *);
  virtual void setbuf(void *buf,int w,int h,int pitch);
  virtual void drawspr(int x,int y,int w,int h,image *);
};

struct svga_drv16:svga_drv{
  color16 *scr;
  color16 *stabb,*stabg,*stabr;
  svga_drv16();
  virtual ~svga_drv16();
  int is_ok();
  virtual ucolor rgb(int r,int g,int b);
  virtual color24 toc24(ucolor);
  virtual ucolor shade(ucolor,unsigned char);
  virtual void dot(int x,int y,ucolor);
  virtual color24 getdot(int x,int y);
  virtual image *convpic32(image *);
  virtual image *convspr32(image *);
  virtual void drawpic(int x,int y,image *);
  virtual void drawspr(int x,int y,image *);
  virtual void clearwh(int x,int y,int w,int h,ucolor);
  virtual void monorect(int x,int y,int w,int h,image *,int sx,int sy,ucolor *);
  virtual void monospr(int x,int y,image *,ucolor);
  virtual void shaderect(int x,int y,int w,int h,unsigned char *);
  virtual void setbuf(void *buf,int w,int h,int pitch);
  virtual void drawspr(int x,int y,int w,int h,image *);
};

struct svga_drv15:svga_drv16{
  svga_drv15();
  virtual ~svga_drv15();
  virtual ucolor rgb(int r,int g,int b);
  virtual color24 toc24(ucolor);
  virtual ucolor shade(ucolor,unsigned char);
  virtual color24 getdot(int x,int y);
  virtual void drawspr(int x,int y,image *);
  virtual void monospr(int x,int y,image *,ucolor);
  virtual void shaderect(int x,int y,int w,int h,unsigned char *);
  virtual image *convpic32(image *);
  virtual image *convspr32(image *);
  virtual void drawspr(int x,int y,int w,int h,image *);
};

DLLEXTERN svga_drv *svga;

#endif

#undef DLLEXTERN

#endif
