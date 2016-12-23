#include <stdio.h>
#include <time.h>
#include <av.h>
#include <averr.h>
#include <stdlib.h>
#include <string.h>
#include <svga8.h>
#include <keyb.h>
#include "sgui.h"

extern "C" void error(int z,int t,int n,char *s1,char *s2) {
  char *m;

  GUI_close();
  printf(av_ez_msg[z],s1,s2); // зона ошибки
  if(t==ET_STD) m=strerror(n); else m=av_err_msg[n]; // сообщение
  printf(":\n  %s\n",m);
  exit(3); // выход в DOS
}

struct desk:gui_obj{
  desk();
  virtual void draw(void);
  virtual int key(int);
  virtual void command(int,long);
};

desk::desk():gui_obj(0,0,640,400) {}

void desk::draw(void) {
  V_clr(0,width,0,height,gray[5]);
  gui_obj::draw();
}

int desk::key(int k) {
  if(k==0x44) {GUI_close();exit(0);}
  return gui_obj::key(k);
}

void desk::command(int c,long p) {
  static menu_t m[]={
	{"ALPHA",C_USER,0},
	{"BETA",C_USER+1,0},
	{"GAMMA",C_USER+2,0},
	{NULL,0,0}
  };
  int x,y;

  if(c<C_USER) {def_command(c,p);return;}
  if(c==C_USER) {
	I_mget(&x,&y);
	desktop<<=*new menubox(x,y,m);
	desktop.update();
	return;
  }
  message(c-C_USER+0,"Кнопка %d\nнажата!!!",c-C_USER+1);
}

static desk dsk;
gui_obj& desktop=dsk;

int main() {
/*  static char *lst[]={
	"Строчка номер 1",
	"Строчка номер 2",
	"Строчка номер 3",
	"Строчка номер 4",
	"Строчка номер 5",
	"Строчка номер 6",
	"Строчка номер 7",
	"Строчка номер 8",
	"Строчка номер 9",
	"Строчка номер 10",
	"Строчка номер 11",
	"Строчка номер 12",
	"Строчка номер 13",
	"Строчка номер 14",
	"Строчка номер 15",
	"Строчка номер 16",
	"Строчка номер 17",
	"Строчка номер 18",
	"Строчка номер 19",
	"Строчка номер 20",
	NULL
  };*/

  sv_width=640;sv_height=480;
  vf_step=2;

  static frame f1(40,40,120,90),f2(140,110,120,90);
  static button b1(20,20,80,18,"BUTTON 1",C_USER,0,-1),
		b2(20,50,80,18,"BUTTON 2",C_USER,1,-1),
		b3(20,20,80,18,"BUTTON 3",C_USER,2,-1),
		b4(20,50,80,18,"BUTTON 4",C_USER,3,-1);
  static chkbox cb1(40,280,20,18,C_NONE,1,0);
  static textstr t1(70,284,0,10,"Checkbox",&smallfont,12);
  static char str[10];
  static inputbox ib1(400,250,160,40,str,10);

  mainpal[255].r=mainpal[255].g=mainpal[255].b=0;
  what_cpu();
  if(!GUI_start(640,480)) {puts("Unable to initialize GUI");return 1;}
  desktop.width=sv_width;
  desktop.height=sv_height;
  desktop<<=(f1<<=b1+=b2)+=(f2<<=b3+=b4)+=cb1+=t1+=ib1;
  for(;;) GUI();
}
