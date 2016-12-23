#include <stdio.h>
#include <time.h>
#include <av.h>
#include <averr.h>
#include <stdlib.h>
#include <string.h>
#include <svga8.h>
#include <keyb.h>
#include <fsound.h>

vgapal stdpal;
vgaimg *cp[6];
vgaimg *sh;

void error(int z,int t,int n,char *s1,char *s2) {
  char *m;

  SV_done();K_done();T_done();
  printf(av_ez_msg[z],s1,s2); // зона ошибки
  if(t==ET_STD) m=strerror(n); else m=av_err_msg[n]; // сообщение
  printf(":\n  %s\n",m);
  exit(1); // выход в DOS
}

int main() {
  int i,j,y;

  what_cpu(); // узнать тип процессора
  puts("Loading...");
  load_pal("cpu.vga",stdpal); // загрузить палитру
  cp[0]=load_vga("cpu.vga","086"); // загрузить картинки
  cp[2]=load_vga("cpu.vga","286");
  cp[3]=load_vga("cpu.vga","386");
  cp[4]=load_vga("cpu.vga","486");
  cp[5]=load_vga("cpu.vga","586");
  sh=load_vga("cpu.vga","shadow");
  if(!SV_findmode(640,480)) {puts("Unable to init SVGA");return 1;}
  VP_setall(stdpal); // установить палитру
  K_init();T_init();
  for(i=25;i>=5;--i) {
    timer=0;
    V_clr(0,640,0,480,0x2A); // очистить весь экран (грубо, но эффективно)
    V_spr(321,241,sh); // нарисовать тень
    V_spr(320,y=240-(i-5)*5,cp[_cpu]); // нарисовать процессор
    V_setrect(20,600,20,440);
    for(j=-200;j<=680;j+=10)
      V_line(-20,j,320,y,31);
    V_setrect(0,640,0,480);
    V_copytoscr(0,640,0,480); // скопировать на реальный экран
    while(timer<tlength(19));
  }
  while(!keys[0x39]); // пока не нажат пробел
  while(keys[0x39]); // пока нажат пробел
  K_done();T_done();
  SV_done();
  return 0;
}
