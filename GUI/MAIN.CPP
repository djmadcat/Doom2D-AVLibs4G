#include <stdio.h>
#include <time.h>
#include <av.h>
#include <averr.h>
#include <stdlib.h>
#include <string.h>
#include <vga.h>
#include <keyb.h>
#include "gui.h"

extern "C" void error(int z,int t,int n,char *s1,char *s2) {
  char *m;

  GUI_close();
  printf(av_ez_msg[z],s1,s2); // ���� �訡��
  if(t==ET_STD) m=strerror(n); else m=av_err_msg[n]; // ᮮ�饭��
  printf(":\n  %s\n",m);
  exit(3); // ��室 � DOS
}

struct desk:gui_obj{
  desk();
  virtual void draw(void);
  virtual int key(int);
  virtual void command(int,long);
};

desk::desk():gui_obj(0,0,320,200) {}

void desk::draw(void) {
  V_clr(0,320,0,200,gray[5]);
  gui_obj::draw();
}

int desk::key(int k) {
  if(k==0x4400) {GUI_close();exit(0);}
  return gui_obj::key(k);
}

void desk::command(int c,long p) {
  static menu_t m[]={
	{"ALPHA",C_USER},
	{"BETA",C_USER+1},
	{"GAMMA",C_USER+2},
	{NULL,0}
  };
  int x,y;

  if(c<C_USER) {def_command(c,p);return;}
  if(c==C_USER) {
	I_mget(&x,&y);
	desktop<<=*new menubox(x,y,m);
	desktop.update();
	return;
  }
  message(c-C_USER+1,"������ %d\n�����!!!",c-C_USER+1);
}

static desk dsk;
gui_obj& desktop=dsk;

int main() {
/*  static char *lst[]={
	"���窠 ����� 1",
	"���窠 ����� 2",
	"���窠 ����� 3",
	"���窠 ����� 4",
	"���窠 ����� 5",
	"���窠 ����� 6",
	"���窠 ����� 7",
	"���窠 ����� 8",
	"���窠 ����� 9",
	"���窠 ����� 10",
	"���窠 ����� 11",
	"���窠 ����� 12",
	"���窠 ����� 13",
	"���窠 ����� 14",
	"���窠 ����� 15",
	"���窠 ����� 16",
	"���窠 ����� 17",
	"���窠 ����� 18",
	"���窠 ����� 19",
	"���窠 ����� 20",
	NULL
  };*/
  static frame f1(20,20,60,45),f2(70,55,60,45);
  static button b1(30,30,40,9,"BUTTON 1",C_USER,0,-1),
				b2(30,45,40,9,"BUTTON 2",C_USER,1,-1),
				b3(80,65,40,9,"BUTTON 3",C_USER,2,-1),
				b4(80,80,40,9,"BUTTON 4",C_USER,3,-1);
  static chkbox cb1(20,140,10,9,C_NONE,1);
  static textstr t1(35,142,0,5,"Checkbox",&smallfont,12);
  static char str[10];
  static inputbox ib1(200,125,80,20,str,10);

  mainpal[255].r=mainpal[255].g=mainpal[255].b=0;
  what_cpu();
  GUI_start();
  desktop<<=(f1<<=b1+=b2)+=(f2<<=b3+=b4)+=cb1+=t1+=ib1;
  for(;;) GUI();
}
