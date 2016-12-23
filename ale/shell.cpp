#include <stdio.h>
#include <direct.h>
#include <stdarg.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
//#include <setjmp.h>
#include <ale.h>
#include <harderr.h>
#include "shell.h"

#define DEFINE_ATTRS
#include "attr.h"

int __havekey();
#pragma aux __havekey= \
  "mov ah,1" \
  "int 0x16" \
  "mov eax,0" \
  "jz x" \
  "inc eax" \
  "x:" \
  value [eax]

int __getkey();
#pragma aux __getkey= \
  "mov ah,0" \
  "int 0x16" \
  "movzx eax,ax" \
  value [eax]

int __getshift();
#pragma aux __getshift= \
  "mov ah,2" \
  "int 0x16" \
  "movzx eax,al" \
  value [eax]

int have_key() {
  return __havekey();
}

int get_key() {
  return __getkey();
}

int get_shift() {
  return __getshift();
}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//
//       video
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//

void vmode(int);
#pragma aux vmode= \
  "int 0x10" \
  parm [eax]

int getvmode(void);
#pragma aux getvmode= \
  "mov ah,0x0F" \
  "int 0x10" \
  "movzx eax,al" \
  value [eax]

void hardcur(char,char);
#pragma aux hardcur= \
  "mov ah,2" \
  "mov bh,0" \
  "int 0x10" \
  parm [dl] [dh] \
  modify [ah bh]

static int curx,cury;
static int bpl;
int scrw,scrh;
static unsigned char *vbuf,*vscrbuf;

void init_video(void) {
  bpl=80*2;
  scrw=80;
  scrh=25;
  if(!(vbuf=(unsigned char*)malloc(bpl*scrh)))
    {puts("Not enough memory");exit(1);}
  if(!(vscrbuf=(unsigned char*)malloc(bpl*scrh)))
    {free(vbuf);puts("Not enough memory");exit(1);}
  if(getvmode()!=3) vmode(3);
  memcpy(vscrbuf,(void*)0xB8000,bpl*scrh);
  hidecur();
}

void restore_scr(void) {
  memcpy(vbuf,vscrbuf,bpl*scrh);
}

void updatescr(void) {
  memcpy((void*)0xb8000,vbuf,bpl*scrh);
}

void *grabscr() {
  void *p=malloc(bpl*scrh);
  if(!p) return NULL;
  memcpy(p,vbuf,bpl*scrh);
  return p;
}

void drawscr(void *p) {
  if(p) memcpy(vbuf,p,bpl*scrh);
}

void close_video(void) {
  restore_scr();updatescr();
  free(vbuf);free(vscrbuf);
}

void prpos(int x,int y) {curx=x;cury=y;}

void movecur(int x,int y) {hardcur(x,y);}
void hidecur() {hardcur(0,scrh);}

void prch(int c,int a) {
  vbuf[cury*bpl+curx*2]=c;
  vbuf[cury*bpl+curx*2+1]=a;
  if(++curx>=scrw) curx=scrw-1;
}

void skipch(void) {
  if(++curx>=scrw) curx=scrw-1;
}

void prstr(int a,char *s) {
  for(;*s;++s) prch(*s,a);
}

void prf(int a,char *s,...) {
  va_list ap;
  static char buf[2000];

  va_start(ap,s);
  vsprintf(buf,s,ap);
  va_end(ap);
  for(s=buf;*s;++s) prch(*s,a);
}

void clear(int x,int y,int w,int h,int a) {
  for(;h;--h,++y) {
    prpos(x,y);
    for(int i=0;i<w;++i) prch(' ',a);
  }
}

void border(int x,int y,int w,int h,int t,int a) {
  static tab[2][11]={
    'Ú','Â','¿',
    'Ã','Å','´',
    'À','Á','Ù','Ä','³',
    'É','Ë','»',
    'Ì','Î','¹',
    'È','Ê','¼','Í','º'
  };
  int i;

  if(h<=0) return;
  if(w<=0) return;
  prpos(x,y);
  prch(tab[t][0],a);
  for(i=1;i<w-1;++i) prch(tab[t][9],a);
  if(w>=2) prch(tab[t][2],a);
  for(++y,--h;h>1;--h,++y) {
    prpos(x,y);
    prch(tab[t][10],a);
    for(i=1;i<w-1;++i) prch(' ',a);
    if(w>=2) prch(tab[t][10],a);
  }
  if(h>0) {
    prpos(x,y);
    prch(tab[t][6],a);
    for(i=1;i<w-1;++i) prch(tab[t][9],a);
    if(w>=2) prch(tab[t][8],a);
  }
}

//static jmp_buf main_jb;

char chgdrv_failed=0;

static int harderr(int e,int d,int f) {
  static char *etext[]={
    "Disk is write-protected",
    "Unknown device ID",
    "Drive not ready",
    "Unknown command",
    "CRC error",
    "Invalid request structure length",
    "Seek error",
    "Unknown media type",
    "Sector not found",
    "Out of paper",
    "Error",
    "Error",
    "General failure"
  };
  static char s[80];
  if(f&HARDERRF_DISK)
    f=(f&HARDERRF_WRITE)|HARDERRF_DISK|HARDERRF_MAY_RETRY|HARDERRF_MAY_IGNORE|HARDERRF_MAY_FAIL;
  f|=HARDERRF_MAY_FAIL;
  sprintf(s,"%s ",(e<=12)?etext[e]:"Unknown error");
  if(e>12) sprintf(s+strlen(s)," #%d ",e);
  sprintf(s+strlen(s),"%s %s",(f&HARDERRF_WRITE)?"writing":"reading",
    (f&HARDERRF_DISK)?"device":"drive");
  if(!(f&HARDERRF_DISK)) sprintf(s+strlen(s)," %c:",d+'A');

  int r=message(s,"Error",4,3,0,NULL,//"~Abort",
    (f&HARDERRF_MAY_RETRY)?"~Retry":NULL,
    (f&HARDERRF_MAY_IGNORE)?"~Ignore":NULL,
    (f&HARDERRF_MAY_FAIL)?"~Fail":NULL
  );
  switch(r) {
    case 1: return HARDERR_RETRY;
    case 2: return HARDERR_IGNORE;
    case 3:
      if(!(f&HARDERRF_DISK)) chgdrv_failed=1;
      return HARDERR_FAIL;
  }
  close_video();
  return HARDERR_ABORT;
}

void clear_insert() {
  *((char*)0x418)&=0x7F;
}

Panel *panel[2]={NULL,NULL};
KeyBar *keybar=NULL;

int main() {
  puts("Ale Navigator  Version 0.02  Copyright (C) 1997-99 Aleksey Volynskov");

  init_video();
  harderr_inst(harderr);
  init_ale();

  panel[0]=new FilePanel(0,0,40,24,getcwd(NULL,0));
  panel[1]=new FilePanel(40,0,40,24,"C:\\");
  keybar=get_keybar(KB_MAIN);
  if(!panel[0] || !panel[1] || !keybar) {
    close_video();
    puts("FATAL ERROR: unable to create panels - not enough memory?");
    return 1;
  }
  panel[0]->selected=1;
  chgdrv_failed=0;
  ((FilePanel*)panel[0])->re_read();

//  setjmp(main_jb);

  for(;;) {
    panel[0]->act();
    panel[1]->act();
    keybar->act();

    if(have_key()) {
      int c=get_key();

      if((c&0xFF)==9) {
	panel[0]->selected=!panel[0]->selected;
	panel[1]->selected=!panel[1]->selected;
//        chgdrv_failed=0;
	if(panel[0]->selected) {
	  ((FilePanel*)panel[0])->setcurpath();
	}else{
	  ((FilePanel*)panel[1])->setcurpath();
	}
      }else if(!panel[0]->key(c))
	if(!panel[1]->key(c))
	  keybar->key(c);
    }

//    clear(0,0,scrw,scrh,attrs[A_BACK]);
    panel[0]->draw();
    panel[1]->draw();
    keybar->draw();
    updatescr();
  }

//  close_video();
//  return 0;
}
