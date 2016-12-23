/*
  ��ࠢ����� ��䨪�� SVGA (256 梥⮢) ��� DOS4GW (� ⠪�� DirectX 3)
  ����� ���ᨨ 1.0
  (C) ����ᥩ ����᪮�, 1997
*/

#pragma library("svga8.lib")

#ifdef __cplusplus
extern "C" {
#endif

// ��������� ����ࠦ����
typedef struct{
  unsigned short w,h; // W-�ਭ�,H-����
  short sx,sy;    // ᤢ�� 業�� ����ࠦ����
}vgaimg;

// R-����,G-������,B-ᨭ��
typedef struct{
  unsigned char r,g,b;
}rgb_t;

// 256-� 梥⮢�� ������ VGA
typedef rgb_t vgapal[256];

// ���� 梥⮢
typedef unsigned char colormap[256];

// ⨯ �㭪樨 ����ᮢ�� �࠭�
typedef void redraw_f(void);

// ⨯ �㭪樨 ���ᮢ�� �ࠩ�
typedef void spr_f(void);

#ifndef __NT__
// ��४��祭�� � ०�� sv_mode ��� W x H
// �����頥� �� 0, �᫨ �� �'���
int SV_findmode(int w,int h);

// ����஢���� �� �࠭
void SV_copytoscr(int x,int y,int w,int h,void *p,int bpl);
#else
// ��४��祭�� � ०�� W x H
// �����頥� �� 0, �᫨ �� �'���
#ifdef _WINDOWS_
int SV_findmode(int w,int h,HWND,int window);
#endif
#endif

// ��४��祭�� � ⥪�⮢� ०��
void SV_done(void);

// �뢥�� ���⨭�� i � ���न���� (x,y)
void V_pic(short x,short y,vgaimg *i);

// �뢥�� �ࠩ� i � ���न���� (x,y) �� ����� �㭪樨 f
//   ��ࠬ����: AL=梥�_�窨_�ࠩ� EDI=>�࠭
//   !!! ��࠭�� �� ॣ�����, �஬� EAX
//   !!! EDI ������ ���� ��ॢ���� �� ��� ��ࠢ� (INC EDI ��� STOSB)
void V_sprf(short x,short y,vgaimg *i,spr_f *f);

// �뢥�� �ࠩ� i � ���न���� (x,y)
void V_spr(short x,short y,vgaimg *i);

// �뢥�� ��ઠ�쭮 ��ॢ����� �ࠩ� i � ���न���� (x,y)
void V_spr2(short x,short y,vgaimg *i);

// �뢥�� ��� �ࠩ� i � ���न���� (x,y) ����� 梥⮬ c
// (���室�� ��� �ᮢ���� ⥭��)
void V_spr1color(short x,short y,vgaimg *i,unsigned char c);

// �뢥�� ��� 梥� c � ���न���� (x,y)
void V_dot(short x,short y,unsigned char c);

// ������� ����� �� �窨 (x1,y1) �� �窨 (x2,y2) 梥⮬ c
void V_line(int x1,int y1,int x2,int y2,unsigned char c);

// ������ ��אַ㣮�쭨� 梥⮬ c
// x-����� ��஭�,w-�ਭ�,y-����,h-����
void V_clr(short x,short w,short y,short h,unsigned char c);

// ������� ⥪���� ������� � ���ᨢ p
void VP_getall(void *p);

// ��⠭����� ������� �� ���ᨢ� p
void VP_setall(void *p);

// ��⠭����� n 梥⮢, ��稭�� � f, �� ���ᨢ� p
void VP_set(void *p,short f,short n);

// ��������� ������� ����� 梥⮬ (r,g,b)
void VP_fill(char r,char g,char b);

// �ਡ������ ������� p � 梥�� (r,g,b) �� s/n
// ����祭��� ������ ��室���� � pal_tmp
void VP_tocolor(void *p,char r,char g,char b,char n,char s);

// �ਡ������ ������� p � ������ p2 �� s/n
// ����祭��� ������ ��室���� � pal_tmp
void VP_topal(void *p,void *p2,char n,char s);

// ��⠭����� ������� �뢮��
void V_setrect(short x,short w,short y,short h);

// ��⠭����� ���� �࠭���� ����
void V_setscr(void *);

// ᪮��஢��� ��אַ㣮�쭨� �� �࠭
void V_copytoscr(short x,short w,short y,short h);

// �뢥�� ᨬ��� c
void V_putch(short c);

// �뢥�� ��ப� s
void V_puts(char *s);

// ���᫨�� ����� ��ப� s (� �窠�)
short V_strlen(char *s);

// ���᫨�� �ਭ� ᨬ���� c (� �窠�)
short V_chrlen(char c);


// ������ ���ᠭ�� �㭪権 RD_* �. � 䠩�� REDRAW.DOC ;)

// ���樠������ �࠭�
void *RD_init(short x,short w,short y,short h);

// ����� ����ᮢ�� �࠭� s �� ����� �㭪樨 f
void RD_start(void *s,redraw_f *f);

// ����ᮢ��� ��אַ㣮�쭨�
void RD_rect(short x,short w,short y,short h);

// ����ᮢ��� �ࠩ�
void RD_spr(short x,short y,vgaimg *s);
void RD_spr2(short x,short y,vgaimg *s);

// ����砭�� ����ᮢ��
void RD_end(void);


// �����頥� �મ��� 梥� (r,g,b)
short VP_brightness(char r,char g,char b);

// ���� ���室�騩 梥� (r,g,b) � ���ᨢ� p, ᮤ�ঠ饬 n ����⮢
short VP_findcolor(void *p,int n,int r,int g,int b);

// ��।����� ����ࠦ���� i �� ���� 梥⮢ m
void V_remap(vgaimg *i,colormap m);


// ����㦠�� ������� p �� 䠩�� f (VGAED 2.0)
// �����頥� 0 �� �訡��
short load_pal(char *f,void *p);

// ����㦠�� ����ࠦ���� i �� 䠩�� f (VGAED 2.0)
// �����頥� NULL �� �訡��
vgaimg *load_vga(char *f,char *i);

// ������� �뢮��
extern short scrw,scrh,scrx,scry;

// ���� �࠭���� ����
extern unsigned char *scra;

// �६����� ������
extern vgapal pal_tmp;

// ����� �� ���⭮�� 室� ��� �� ��⠭���� �������
// 0-���, ����-��
extern char vp_waitrr;

// ���न���� ��� �뢮�� ᫥���饣� ᨬ����
extern short vf_x,vf_y;

// ����ﭨ� ����� ᨬ������
extern short vf_step;

// 梥� ᨬ�����
// �᫨ 0 - �� �ᯮ������
extern unsigned char vf_color;

// ����
extern void *vf_font;

extern unsigned char *scrbuf;

extern unsigned sv_width,sv_height,sv_bits,sv_mem,sv_bpl;

#ifndef __NT__
// ����� �����०��� VESA ��� -1
extern unsigned sv_mode;

extern char use_vbe2,vbe2;

// �ࠩ��� VBE 2.0 ��� NULL, �᫨ ��� ���
extern void *vbe2_drv;
#else
extern char sv_in_window;
#ifdef __DDRAW_INCLUDED__
extern LPDIRECTDRAW DDraw;
#endif
#endif

#ifdef __cplusplus
}
#endif
