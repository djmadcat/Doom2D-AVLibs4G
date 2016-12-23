/*
  Управление графикой SVGA (256 цветов) для DOS4GW (а также DirectX 3)
  Модуль версии 1.0
  (C) Алексей Волынсков, 1997
*/

#pragma library("svga8.lib")

#ifdef __cplusplus
extern "C" {
#endif

// заголовок изображения
typedef struct{
  unsigned short w,h; // W-ширина,H-высота
  short sx,sy;    // сдвиг центра изображения
}vgaimg;

// R-красный,G-зеленый,B-синий
typedef struct{
  unsigned char r,g,b;
}rgb_t;

// 256-и цветовая палитра VGA
typedef rgb_t vgapal[256];

// карта цветов
typedef unsigned char colormap[256];

// тип функции перерисовки экрана
typedef void redraw_f(void);

// тип функции прорисовки спрайта
typedef void spr_f(void);

#ifndef __NT__
// переключение в режим sv_mode или W x H
// возвращает не 0, если все о'кей
int SV_findmode(int w,int h);

// копирование на экран
void SV_copytoscr(int x,int y,int w,int h,void *p,int bpl);
#else
// переключение в режим W x H
// возвращает не 0, если все о'кей
#ifdef _WINDOWS_
int SV_findmode(int w,int h,HWND,int window);
#endif
#endif

// переключение в текстовый режим
void SV_done(void);

// вывести картинку i в координатах (x,y)
void V_pic(short x,short y,vgaimg *i);

// вывести спрайт i в координатах (x,y) при помощи функции f
//   параметры: AL=цвет_точки_спрайта EDI=>экран
//   !!! сохраняйте все регистры, кроме EAX
//   !!! EDI должен быть переведен на точку вправо (INC EDI или STOSB)
void V_sprf(short x,short y,vgaimg *i,spr_f *f);

// вывести спрайт i в координатах (x,y)
void V_spr(short x,short y,vgaimg *i);

// вывести зеркально перевернутый спрайт i в координатах (x,y)
void V_spr2(short x,short y,vgaimg *i);

// вывести форму спрайта i в координатах (x,y) одним цветом c
// (подходит для рисования теней)
void V_spr1color(short x,short y,vgaimg *i,unsigned char c);

// вывести точку цвета c в координатах (x,y)
void V_dot(short x,short y,unsigned char c);

// начертить линию от точки (x1,y1) до точки (x2,y2) цветом c
void V_line(int x1,int y1,int x2,int y2,unsigned char c);

// очистить прямоугольник цветом c
// x-левая сторона,w-ширина,y-верх,h-высота
void V_clr(short x,short w,short y,short h,unsigned char c);

// получить текущую палитру в массив p
void VP_getall(void *p);

// установить палитру из массива p
void VP_setall(void *p);

// установить n цветов, начиная с f, из массива p
void VP_set(void *p,short f,short n);

// заполнить палитру одним цветом (r,g,b)
void VP_fill(char r,char g,char b);

// приблизить палитру p к цвету (r,g,b) на s/n
// полученная палитра находится в pal_tmp
void VP_tocolor(void *p,char r,char g,char b,char n,char s);

// приблизить палитру p к палитре p2 на s/n
// полученная палитра находится в pal_tmp
void VP_topal(void *p,void *p2,char n,char s);

// установить область вывода
void V_setrect(short x,short w,short y,short h);

// установить адрес экранного буфера
void V_setscr(void *);

// скопировать прямоугольник на экран
void V_copytoscr(short x,short w,short y,short h);

// вывести символ c
void V_putch(short c);

// вывести строку s
void V_puts(char *s);

// вычислить длину строки s (в точках)
short V_strlen(char *s);

// вычислить ширину символа c (в точках)
short V_chrlen(char c);


// полное описание функций RD_* см. в файле REDRAW.DOC ;)

// инициализация экрана
void *RD_init(short x,short w,short y,short h);

// начать перерисовку экрана s при помощи функции f
void RD_start(void *s,redraw_f *f);

// перерисовать прямоугольник
void RD_rect(short x,short w,short y,short h);

// перерисовать спрайт
void RD_spr(short x,short y,vgaimg *s);
void RD_spr2(short x,short y,vgaimg *s);

// окончание перерисовки
void RD_end(void);


// возвращает яркость цвета (r,g,b)
short VP_brightness(char r,char g,char b);

// найти подходящий цвет (r,g,b) в массиве p, содержащем n элементов
short VP_findcolor(void *p,int n,int r,int g,int b);

// переделать изображение i по карте цветов m
void V_remap(vgaimg *i,colormap m);


// загружает палитру p из файла f (VGAED 2.0)
// возвращает 0 при ошибке
short load_pal(char *f,void *p);

// загружает изображение i из файла f (VGAED 2.0)
// возвращает NULL при ошибке
vgaimg *load_vga(char *f,char *i);

// область вывода
extern short scrw,scrh,scrx,scry;

// адрес экранного буфера
extern unsigned char *scra;

// временная палитра
extern vgapal pal_tmp;

// ждать ли обратного хода луча при установке палитры
// 0-нет, иначе-да
extern char vp_waitrr;

// координаты для вывода следующего символа
extern short vf_x,vf_y;

// расстояние между символами
extern short vf_step;

// цвет символов
// если 0 - не используется
extern unsigned char vf_color;

// шрифт
extern void *vf_font;

extern unsigned char *scrbuf;

extern unsigned sv_width,sv_height,sv_bits,sv_mem,sv_bpl;

#ifndef __NT__
// номер видеорежима VESA или -1
extern unsigned sv_mode;

extern char use_vbe2,vbe2;

// драйвер VBE 2.0 или NULL, если его нет
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
