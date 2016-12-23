#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <av.h>
#include <averr.h>
#include <stdlib.h>
#include <string.h>
#include <svga8.h>
#include <keyb.h>
#include <sgui.h>

#define MAXCOM 50

extern "C" {
vgaimg *mouseshape=&stdarrow;
unsigned char gray[256];
int msg;
int fonth=10,msgbutw=60,msgbuth=20;
char keyb_mode=0;
}

static char textbuf[1024];

static void *gui_scr;
static int top_com,mx=160,my=100;
static struct{gui_obj *o;int c;long p;} combuf[MAXCOM];

gui_obj::gui_obj() {next=prev=child=owner=NULL;}

gui_obj::gui_obj(int x,int y,int w,int h) {
  next=prev=child=owner=NULL;
  if(x+w>sv_width) x=sv_width-w;
  if(y+h>sv_height) y=sv_height-h;
  if(x<0) x=0;
  if(y<0) y=0;
  orgx=x;orgy=y;width=w;height=h;
}

void gui_obj::redraw(void) {RD_rect(left,width,top,height);}

void gui_obj::update(void) {redraw();GUI_update();}

void gui_obj::draw(void) {
  if(!child) return;
  for(gui_obj *p=child;p->next;p=p->next);
  for(;p;p=p->prev) p->draw();
}

void gui_obj::update_pos(void) {
  left=orgx;top=orgy;
  for(gui_obj *p=owner;p;p=p->owner) {left+=p->orgx;top+=p->orgy;}
  for(p=child;p;p=p->next) p->update_pos();
}

int gui_obj::chkmouse(int x,int y,int b) {
  for(gui_obj *p=child;p;p=p->next) if(p->chkmouse(x,y,b)) return 1;
  x-=left;y-=top;
  if(x<0 || y<0 || x>=width || y>=height) return 0;
  return mouse(x,y,b);
}

#pragma argsused
int gui_obj::mouse(int x,int y,int b) {return 0;}

int gui_obj::key(int k) {
  for(gui_obj *p=child;p;p=p->next) if(p->key(k)) return 1;
  return 0;
}

void gui_obj::command(int c,long p) {
  if(owner) owner->command(c,p);
}

gui_obj& gui_obj::operator +=(gui_obj& a) {
  for(gui_obj *p=&a;p->next;p=p->next) {p->owner=owner;p->update_pos();}
  p->owner=owner;p->update_pos();
  p->next=next;
  if(next) next->prev=p;
  next=&a;a.prev=this;
  return *this;
}

gui_obj& gui_obj::operator <<=(gui_obj& a) {
  for(gui_obj *p=&a;p->next;p=p->next) {p->owner=this;p->update_pos();}
  p->owner=this;p->update_pos();
  p->next=child;
  if(child) child->prev=p;
  child=&a;a.prev=NULL;
  return *this;
}

void gui_obj::remove(void) {
  if(next) next->prev=prev;
  if(owner) if(owner->child==this) owner->child=next;
  if(prev) prev->next=next;
//  if(owner) for(gui_obj *p=owner->child;p;p=p->next)
//    if(p->next==this) {p->next=next;break;}
  owner=next=prev=NULL;
}

// status bar

statbar::statbar(int x,int y,int w,int h):gui_obj(x,y,w,h) {
  clrb=6;clrt=0;
  text=NULL;
}

statbar::~statbar() {
  if(text) {free(text);text=NULL;}
}

void statbar::draw(void) {
  V_clr(left,width,top,height,gray[clrb]);
  if(text) {
    vf_x=left+2;vf_y=top+(height-fonth)/2;
    vf_font=&smallfont;
    vf_color=gray[clrt];
    V_puts(text);
  }
  gui_obj::draw();
}

void statbar::settext(char *s,...) {
  va_list ap;

  if(text) free(text);
  va_start(ap,s);
  vsprintf(textbuf,s,ap);
  va_end(ap);
  text=strdup(textbuf);
  redraw();
}

// frame

frame::frame(int x,int y,int w,int h):gui_obj(x,y,w,h) {
  clrh=10;clrn=6;clrl=2;
}

void frame::draw(void) {
  V_clr(left+2,width-4,top+2,height-4,gray[clrn]);
  V_clr(left,width-2,top,2,gray[clrh]);V_dot(left+width-2,top,gray[clrh]);
  V_clr(left,2,top+2,height-4,gray[clrh]);V_dot(left,top+height-2,gray[clrh]);
  V_clr(left+1,width-1,top+height-2,2,gray[clrl]);V_dot(left,top+height-1,gray[clrl]);
  V_clr(left+width-2,2,top+1,height-3,gray[clrl]);V_dot(left+width-1,top,gray[clrl]);
  gui_obj::draw();
}

// container box

vcontbox::vcontbox(int x,int y,int w,int h,int wc,cont_t *t,int c,int cm)
:frame(x,y,w,h) {
  gui_obj *p;

  cw=wc;cont=t;cur=c;com=cm;
  child=cont[cur].o;
  for(num=0;t->o;++t,++num)
	for(p=t->o;p;p=p->next) p->owner=this;
}

void vcontbox::draw(void) {
  int i,j;

  frame::draw();
  V_clr(left+cw,1,top+2,j=(height-4)*cur/num,gray[clrh]);
  i=(height-4)*(cur+1)/num;
  V_clr(left+2,1,top+2+j,i-j,gray[clrh]);
  V_clr(left+cw,1,top+1+i,height-3-i,gray[clrh]);
  for(i=0;i<num;++i) {
	V_clr(left+2,cw-2,j=(top+1+(height-4)*(i+1)/num),1,gray[(i+1==cur)?clrh:clrl]);
	V_spr(left+cw/2+((i==cur)?1:0),j-height/num/2,(vgaimg*)cont[i].t);
  }
}

int vcontbox::mouse(int x,int y,int b) {
  if(x>=cw) return gui_obj::mouse(x,y,b);
  if(!b) return 1;
  if(y<2 || y>=height-2) return 1;
  y=(y-2)*num/(height-4);
  if(y==cur) return 1;
  child=cont[cur=y].o;
  redraw();
  send_command(this,com,cur);
  while(I_mget(&x,&y)) GUI_wait();
  return 1;
}

// mdialog

mdialog::mdialog(int x,int y,int w,int h):frame(x,y,w,h) {}

int mdialog::chkmouse(int x,int y,int b) {
  if(gui_obj::chkmouse(x,y,b)) return 1;
  mouse(x-left,y-top,b);
  return 1;
}

int mdialog::key(int k) {
  gui_obj::key(k);
  return 1;
}

// msgbox

msgbox::msgbox(int minw,int minh,char *t,int x,int y)
:mdialog(x-minw/2,y-minh/2,minw,minh) {

  vf_font=&smallfont;
  text=strdup(t);minh+=fonth+3;
  for(int w=8;*t;++t) switch(*t) {
	case '\n': minh+=fonth+3;
	case '\r':
	  if(w>minw) minw=w;
	  w=8;break;
	default:
	  w+=V_chrlen(*t);
  }if(w>minw) minw=w;
  orgx=x-minw/2;orgy=y-minh/2;
  width=minw;height=minh;
}

msgbox::~msgbox() {
  if(text) {free(text);text=NULL;}
}

void msgbox::update_pos(void) {
  gui_obj::update_pos();
  if(left<0) {orgx-=left;left=0;}
  if(top<0) {orgy-=top;top=0;}
  if(left+width>sv_width) orgx-=left+width-sv_width;
  if(top+height>sv_height) orgy-=top+height-sv_height;
  gui_obj::update_pos();
}

void msgbox::draw(void) {
  frame::draw();
  if(text) {
    vf_font=&smallfont;
    vf_x=left+4;vf_y=top+4;vf_color=gray[0];
    for(char *p=text;*p;++p) switch(*p) {
      case '\n': vf_y+=fonth+3;
      case '\r': vf_x=left+4;break;
      default: V_putch(*p);
    }
  }
}

void msgbox::command(int c,long p) {
  switch(c) {
	case C_CANCEL: case C_OK: case C_YES: case C_NO:
	  msg=c;return;
  }
  gui_obj::command(c,p);
}

// base button class

base_button::base_button(int x,int y,int w,int h,int c,long p,int k)
:gui_obj(x,y,w,h) {
  clrh=12;clrn=10;clrl=8;
  com=c;par=p;hotkey=k;
  pressed=0;
}

void base_button::draw(void) {
  int i;

  i=pressed;
  if(!i) {
    V_clr(left+2,width-4,top+2,height-4,gray[clrn]);
    V_clr(left,width-1,top,1,gray[clrh]);
    V_clr(left,1,top,height-1,gray[clrh]);
    V_clr(left+1,width-2,top+1,1,gray[(clrn+clrh)/2]);
    V_clr(left+1,1,top+1,height-2,gray[(clrn+clrh)/2]);
    V_clr(left,width,top+height-1,1,gray[0]);
    V_clr(left+width-1,1,top,height,gray[0]);
    V_clr(left+1,width-2,top+height-2,1,gray[clrl]);
    V_clr(left+width-2,1,top+1,height-2,gray[clrl]);
  }else{
    V_clr(left+3,width-4,top+3,height-4,gray[clrn]);
    V_clr(left+1,width-1,top+1,1,gray[clrl/2]);
    V_clr(left+1,1,top+1,height-1,gray[clrl/2]);
    V_clr(left+2,width-2,top+2,1,gray[clrl]);
    V_clr(left+2,1,top+2,height-2,gray[clrl]);
    V_clr(left,width,top,1,gray[0]);
    V_clr(left,1,top,height,gray[0]);
    V_clr(left+2,width-2,top+height-1,1,gray[clrl/2]);
    V_clr(left+width-1,1,top+2,height-2,gray[clrl/2]);
  }
}

#pragma argsused
int base_button::mouse(int x,int y,int b) {
  if(!b) return 1;
  pressed=1;redraw();
  while(I_mget(&x,&y)) {
	x-=left;y-=top;
	if(x<0 || x>=width || y<0 || y>=height) {
	  if(pressed) {pressed=0;redraw();}
	}else
	  if(!pressed) {
	    pressed=1;redraw();
	  }
	GUI_wait();
  }
  if(pressed) {
    send_command(this,com,par);
    pressed=0;redraw();
  }return 1;
}

int base_button::key(int k) {
  if((k&0x7F)==hotkey) {
    pressed=1;redraw();
    while(keys[k]) {
      GUI_wait();
      if(!pressed) {
	pressed=1;redraw();
      }
    }
    pressed=0;redraw();
    send_command(this,com,par);return 1;
  }
  return 0;
}

// text button

button::button(int x,int y,int w,int h,char *s,int c,long p,int k)
:base_button(x,y,w,h,c,p,k) {
  text=s;
  vf_font=&smallfont;
  textx=(w-V_strlen(s)-vf_step)/2;texty=(h-fonth)/2;
}

void button::draw(void) {
  base_button::draw();
  vf_font=&smallfont;
  vf_x=left+textx+pressed;vf_y=top+texty+pressed;vf_color=gray[0];
  V_puts(text);
}

void button::settext(char *s) {
  text=s;
  vf_font=&smallfont;
  textx=(width-V_strlen(s)-vf_step)/2;texty=(height-fonth)/2;
  redraw();
}

// sprite button

sprbutton::sprbutton(int x,int y,int w,int h,vgaimg *s,int c,long p,int k)
:base_button(x,y,w,h,c,p,k) {
  spr=s;
  sprx=w/2;spry=h/2;
}

void sprbutton::draw(void) {
  V_spr1color(sprx+2,spry+2,spr,gray[0]);
  V_spr(left+sprx+pressed,top+spry+pressed,spr);
}

void sprbutton::redraw(void) {
  RD_rect(left+sprx-spr->sx,spr->w+2,top+spry-spr->sy,spr->h+2);
}

// icon button

iconbutton::iconbutton(int x,int y,int w,int h,vgaimg *s,int c,long p,int k)
:base_button(x,y,w,h,c,p,k) {
  spr=s;
}

void iconbutton::draw(void) {
  base_button::draw();
  V_spr(left+width/2+pressed,top+height/2+pressed,spr);
}

// user button

userbutton::userbutton(int x,int y,vgaimg *s1,vgaimg *s2,int c,long p,int k)
:base_button(x,y,1,1,c,p,k) {
  spr1=s1;spr2=s2;
  width=(s1->w > s2->w)?s1->w:s2->w;
  height=(s1->h > s2->h)?s1->h:s2->h;
}

void userbutton::draw(void) {
  V_spr(left,top,(pressed)?spr2:spr1);
}

// menubox

menubox::menubox(int x,int y,menu_t *m):frame(x,y,8,8) {
  int i,w;

  clrh=12;clrn=8;clrl=4;
  mnu=m;
  vf_font=&smallfont;
  for(i=0;mnu[i].c;++i,height+=fonth+3)
    if((w=V_strlen(mnu[i].t)+8)>width) width=w;
  lines=i;
  if(orgx+width>sv_width) orgx=sv_width-width;
  if(orgy+height>sv_height) orgy=sv_height-height;
  line=-1;
}

void menubox::update_pos(void) {
  gui_obj::update_pos();
  if(left<0) {orgx-=left;left=0;}
  if(top<0) {orgy-=top;top=0;}
  if(left+width>sv_width) orgx-=left+width-sv_width;
  if(top+height>sv_height) orgy-=top+height-sv_height;
  gui_obj::update_pos();
}

void menubox::draw(void) {
  int i;

  frame::draw();
  vf_font=&smallfont;
  for(i=0;mnu[i].c;++i) {
	vf_color=gray[(i==line)?15:0];
	vf_x=left+4;vf_y=top+i*(fonth+3)+4;
	V_puts(mnu[i].t);
  }
}

int menubox::key(int k) {
  if(k==1) {
    while(keys[k]) GUI_wait();
    gui_obj *o=owner;
    remove();
    if(o) o->redraw();
    int x,y;
    while(I_mget(&x,&y)) GUI_wait();
    if(o) send_command(o,C_CANCEL,0);
    delete this;
    return 1;
  }else if((k&0x7F)==0x48) {
    if(line<0) line=lines-1;
    else if(--line<0) line=lines-1;
    redraw();
//    while(keys[k]) GUI_wait();
    keys[k]=0;
    return 1;
  }else if((k&0x7F)==0x50) {
    if(line<0) line=0;
    else if(++line>=lines) line=0;
    redraw();
//    while(keys[k]) GUI_wait();
    keys[k]=0;
    return 1;
  }else if((k&0x7F)==0x1C) {
    while(keys[k]) GUI_wait();
    if(line>=0) send_command(this,mnu[line].c,mnu[line].p);
    return 1;
  }
  return 0;
}

#pragma argsused
int menubox::mouse(int x,int y,int b) {
  int l;

  if(!b) return 1;
  while(I_mget(&x,&y)) {
	x-=left;y-=top;
	if(x<4 || x>=width-4 || y<4 || y>=height-4) l=-1;
	  else l=(y-4)/(fonth+3);
	if(l!=line) {line=l;redraw();}
	GUI_wait();
  }
  if(line>=0) send_command(this,mnu[line].c,mnu[line].p);
  return 1;
}

int menubox::chkmouse(int x,int y,int b) {
  x-=left;y-=top;
  if(b) if(x<0 || y<0 || x>=width || y>=height) {
    gui_obj *o=owner;
    remove();
    if(o) o->redraw();
    while(I_mget(&x,&y)) GUI_wait();
    if(o) send_command(o,C_CANCEL,0);
    delete this;return 1;
  }
  return mouse(x,y,b);
}

// menubar

menubar::menubar(int x,int y,int w,int h,menubar_t *m):gui_obj(x,y,w,h) {
  int i;

  mnu=m;subm=NULL;
  vf_font=&smallfont;
  for(i=0;mnu[i].t;++i)
    mnu[i].tl=V_strlen(mnu[i].t)+4;
  lines=i;
  line=-1;oldline=0;
  barw=width;barh=height;
}

int menubar::chkmouse(int x,int y,int b) {
  if(subm) return mouse(x,y,b);
  return gui_obj::chkmouse(x,y,b);
}

void menubar::activate(void) {
  if(line<0) {
    line=oldline;redraw();
    if(mnu[line].m) openmenu(line,mnu[line].m);
    gui_obj *o=owner;
    remove();
    if(o) *o<<=*this;
  }
}

void menubar::draw(void) {
  int i,x;

  V_clr(left,barw,top,barh,gray[6]);
  vf_font=&smallfont;
  vf_y=top+(barh-fonth)/2;
  for(i=0,x=left;mnu[i].t;x+=mnu[i].tl,++i) {
    if(i==line) V_clr(x,mnu[i].tl,top,barh,gray[2]);
    vf_color=gray[(i==line)?15:0];
    vf_x=x+2;
    V_puts(mnu[i].t);
  }
  if(subm) {
    V_clr(left+smx+1,smw-2,top+barh,i=(fonth+4)*sml+8-1,gray[6]);
    V_clr(left+smx,1,top+barh,i,gray[10]);
    V_clr(left+smx+smw-1,1,top+barh,i,gray[0]);
    V_clr(left+smx,smw,top+barh+i,1,gray[0]);
    if(smc>=0) V_clr(left+smx+1,smw-2,top+barh+smc*(fonth+4)+3,fonth+6,gray[2]);
    for(i=0;i<sml;++i) {
      vf_x=left+smx+4;vf_y=top+barh+4+2+(fonth+4)*i;
      vf_color=gray[(i==smc)?15:0];
      V_puts(subm[i].t);
    }
  }
  gui_obj::draw();
}

void menubar::openmenu(int l,menu_t *m) {
  redraw();
  if(!m) {subm=NULL;width=barw;height=barh;return;}
  for(int i=0,o=left;mnu[i].t;o+=mnu[i].tl,++i) if(i==l) break;
  subm=m;smx=o;smc=0;smw=8;
  for(sml=0;subm[sml].t;++sml)
    if((i=V_strlen(subm[sml].t)+8) > smw) smw=i;
  if(!sml) subm=NULL;
  else{
    height=barh+(fonth+4)*sml+8;
    if(left+smx+smw>desktop.width) smx=desktop.width-left-smw;
    redraw();
  }
}

int menubar::key(int k) {
  if(line<0) return gui_obj::key(k);
  if(subm) switch(k&0x7F) {
    case 1:
      redraw();
      oldline=line;line=-1;
      openmenu(0,NULL);
      while(keys[k]) GUI_wait();
      return 1;
    case 0x4B:
      if(--line<0) line=lines-1;
      openmenu(0,NULL);
      if(mnu[line].m) openmenu(line,mnu[line].m);
      keys[k]=0;
      return 1;
    case 0x4D:
      if(++line>=lines) line=0;
      openmenu(0,NULL);
      if(mnu[line].m) openmenu(line,mnu[line].m);
      keys[k]=0;
      return 1;
    case 0x48:
      if(--smc<0) smc=sml-1;
      redraw();
      keys[k]=0;
      return 1;
    case 0x50:
      if(++smc>=sml) smc=0;
      redraw();
      keys[k]=0;
      return 1;
    case 0x1C:
      send_command(this,subm[smc].c,subm[smc].p);
      redraw();
      oldline=line;line=-1;
      openmenu(0,NULL);
      while(keys[k]) GUI_wait();
      return 1;
    default:
      return 0;
  }
  switch(k&0x7F) {
    case 1:
      redraw();
      oldline=line;line=-1;
      openmenu(0,NULL);
      while(keys[k]) GUI_wait();
      return 1;
    case 0x4B:
      if(--line<0) line=lines-1;
      redraw();
      keys[k]=0;
      return 1;
    case 0x4D:
      if(++line>=lines) line=0;
      redraw();
      keys[k]=0;
      return 1;
    case 0x1C:
      while(keys[k]) GUI_wait();
    case 0x50:
      keys[k]=0;
      if((k&0x7F)==0x1C)
	if(mnu[line].c) {
	  send_command(this,mnu[line].c,mnu[line].p);
	  redraw();
	  oldline=line;line=-1;
	  openmenu(0,NULL);
	  return 1;
	}
      if(mnu[line].m) openmenu(line,mnu[line].m);
      redraw();
      return 1;
  }
  return 0;
}

#pragma argsused
int menubar::mouse(int x,int y,int b) {
  int l,o,i;

  if(!b) return 1;
  if(!subm) {
    gui_obj *o=owner;
    remove();
    if(o) *o<<=*this;
  }
  while(I_mget(&x,&y)) {
    x-=left;y-=top;
    l=-1;
    if(y>=0 && y<barh) {
      for(i=0,o=left;mnu[i].t;o+=mnu[i].tl,++i)
	if(x>=o && x<o+mnu[i].tl) l=i;
    }else if(subm) l=line;
    if(l!=line) {
      line=l;openmenu(0,NULL);
      if(line>=0) if(mnu[line].m) openmenu(line,mnu[line].m);
    }
    if(subm) {
      l=-1;
      if(y>=barh && y<height) if(x>=smx && x<smx+smw) {
	l=(y-barh-4)/(fonth+4);
	if(l<0) l=0; else if(l>=sml) l=sml-1;
      }
      if(l!=smc) {smc=l;redraw();}
    }
    GUI_wait();
  }
  if(subm && smc>=0) {
    send_command(this,subm[smc].c,subm[smc].p);
    redraw();
    oldline=line;line=-1;
    openmenu(0,NULL);
    return 1;
  }
  if(subm) {
    line=-1;openmenu(0,NULL);
    return 1;
  }
  if(line>=0) {
    if(mnu[line].c) {
      send_command(this,mnu[line].c,mnu[line].p);
      redraw();
      oldline=line;line=-1;
      openmenu(0,NULL);
      return 1;
    }
    if(mnu[line].m) openmenu(line,mnu[line].m);
    redraw();
  }
  return 1;
}

// base slider class

base_slider::base_slider(long t,long c,int cm) {
  com=cm;total=t;cur=c;adjust();
}

void base_slider::adjust(void) {
  if(cur<0) cur=0;
  if(cur>total) cur=total;
  send_command(this,com,cur);
}

// horizontal slider

hslider::hslider(int x,int y,int w,int h,long t,long c,int cm):base_slider(t,c,cm) {
  orgx=x;orgy=y;width=w;height=h;
}

void hslider::draw(void) {
  int x;

  V_clr(left,width,top,height,gray[4]);
  V_clr(left,width-1,top,height-1,gray[12]);
  V_clr(left+1,width-2,top+1,height-2,gray[8]);
  x=(width-height)*cur/(total?total:1)+left+1;
  V_clr(x,height-2,top+1,height-2,gray[6]);
  V_clr(x,height-3,top+1,height-3,gray[12]);
  V_clr(x+1,height-4,top+2,height-4,gray[10]);
}

#pragma argsused
int hslider::mouse(int x,int y,int b) {
  if(!b) return 1;
  cur=(x-height/2+1)*total/(width-height);
  adjust();redraw();
  while(I_mget(&x,&y)) {
	x-=left;
	if(x<0) x=0;
	if(x>=width) x=width-1;
	cur=(x-height/2+1)*total/(width-height);
	adjust();redraw();
	GUI_wait();
  }
  return 1;
}

// horizontal scroll bar

hscroll::hscroll(int x,int y,int w,int h,long t,long c,int cm,long s)
:gui_obj(x,y,w,h) {
  *this<<=*(sld=new hslider(h,0,w-h*2,h,t,c,cm))
	+=*(b1=new button(0,0,h,h,"\x1B",C_SLIDE,-s,0))
	+=*(b2=new button(w-h,0,h,h,"\x1A",C_SLIDE,s,0));
}

hscroll::~hscroll(void) {
  delete sld;delete b1;delete b2;
}

void hscroll::command(int c,long p) {
  if(c==C_SLIDE) {
	sld->cur+=p;sld->adjust();sld->redraw();
	return;
  }
  gui_obj::command(c,p);
}

// vertical slider

vslider::vslider(int x,int y,int w,int h,long t,long c,int cm):base_slider(t,c,cm) {
  orgx=x;orgy=y;width=w;height=h;
}

void vslider::draw(void) {
  int y;

  V_clr(left,width,top,height,gray[4]);
  V_clr(left,width-1,top,height-1,gray[12]);
  V_clr(left+1,width-2,top+1,height-2,gray[8]);
  y=(height-width)*cur/(total?total:1)+top+1;
  V_clr(left+1,width-2,y,width-2,gray[6]);
  V_clr(left+1,width-3,y,width-3,gray[12]);
  V_clr(left+2,width-4,y+1,width-4,gray[10]);
}

#pragma argsused
int vslider::mouse(int x,int y,int b) {
  if(!b) return 1;
  cur=(y-width/2+1)*total/(height-width);
  adjust();redraw();
  while(I_mget(&x,&y)) {
	y-=top;
	if(y<0) y=0;
	if(y>=height) y=height-1;
	cur=(y-width/2+1)*total/(height-width);
	adjust();redraw();
	GUI_wait();
  }
  return 1;
}

// vertical scroll bar

vscroll::vscroll(int x,int y,int w,int h,long t,long c,int cm,long s)
:gui_obj(x,y,w,h) {
  *this<<=*(sld=new vslider(0,w,w,h-w*2,t,c,cm))
	+=*(b1=new button(0,0,w,w,"\x18",C_SLIDE,-s,0))
	+=*(b2=new button(0,h-w,w,w,"\x19",C_SLIDE,s,0));
}

vscroll::~vscroll(void) {
  delete sld;delete b1;delete b2;
}

void vscroll::command(int c,long p) {
  if(c==C_SLIDE) {
	sld->cur+=p;sld->adjust();sld->redraw();
	return;
  }
  gui_obj::command(c,p);
}

// string list

strlist::strlist(int x,int y,int w,int h,char **p,int c):frame(x,y,w,h) {
  com=c;newlist(p);
  clrh=2;clrn=0;clrl=8;
}

void strlist::newlist(char **p) {
  list=p;
  cline=-1;tline=0;
  for(lines=0;*p;++lines,++p);
}

void strlist::draw(void) {
  int i,y;

  frame::draw();
  vf_font=&smallfont;
  for(i=tline,y=4;list[i] && y+fonth+4<=height;++i,y+=fonth+3) {
	vf_color=gray[(i==cline)?15:8];
	vf_x=left+4;vf_y=top+y;
	V_puts(list[i]);
  }
}

#pragma argsused
int strlist::mouse(int x,int y,int b) {
  int l;

  if(!b) return 1;
  while(I_mget(&x,&y)) {
	x-=left;y-=top;
	if(x<4 || x>=width-4 || y<4 || y>=height-4) l=-1;
	  else l=(y-4)/(fonth+3)+tline;
	if(l>=lines) l=-1;
	if(l!=cline) {cline=l;redraw();}
	GUI_wait();
  }
  if(cline>=0) send_command(this,com,cline);
  return 1;
}

void strlist::scroll(void) {
  if(cline<tline) tline=cline;
  if((cline-tline)*(fonth+3)+fonth+8>height) {
    tline=cline-(height-fonth-8)/(fonth+3);
    if(tline<0) tline=0;
  }
  redraw();
}

// strlist_k

strlist_k::strlist_k(int x,int y,int w,int h,char **p,int c)
:strlist(x,y,w,h,p,c) {}

int strlist_k::key(int k) {
  if(gui_obj::key(k)) return 1;
  switch(k&0x7F) {
    case 0x48:
      if(!lines) return 1;
      if(cline<0) cline=0;
      else if(cline>0) --cline;
      scroll();
      send_command(this,com,cline);
//      while(keys[k]) GUI_wait();
      keys[k]=0;
      return 1;
    case 0x50:
      if(!lines) return 1;
      if(cline<0) cline=0;
      else if(cline<lines-1) ++cline;
      scroll();
      send_command(this,com,cline);
//      while(keys[k]) GUI_wait();
      keys[k]=0;
      return 1;
    case 0x49:
      if(!lines) return 1;
      if(cline<0) cline=0;
      else if(cline>tline) cline=tline;
      else if((cline-=(height-fonth-8)/(fonth+3)) < 0) cline=0;
      scroll();send_command(this,com,cline);keys[k]=0;
      return 1;
    case 0x51:
      if(!lines) return 1;
      if(cline<0) cline=0;
      else if(cline<tline+(height-fonth-8)/(fonth+3))
	cline=tline+(height-fonth-8)/(fonth+3);
      else cline+=(height-fonth-8)/(fonth+3);
      if(cline>=lines) cline=lines-1;
      scroll();send_command(this,com,cline);keys[k]=0;
      return 1;
  }
  return 0;
}

// string list with scroll bar

static int adj1(int a) {return (a>0)?a:0;}

strlist2::strlist2(int x,int y,int w,int h,char **p,int c,long s)
:gui_obj(x,y,w,h) {
  *this<<=*(lst=new strlist(0,0,w-fonth*2,h,p,c));
  *this<<=*(sb=new vscroll(w-fonth*2,0,fonth*2,h,adj1(lst->lines-1),0,C_SCROLL,s));
}

strlist2::~strlist2(void) {
  delete sb;delete lst;
}

void strlist2::newlist(char **p) {
  lst->newlist(p);
  sb->sld->total=adj1(lst->lines-1);sb->sld->cur=0;sb->sld->adjust();
  redraw();
}

void strlist2::command(int c,long p) {
  if(c==C_SCROLL) {
	lst->tline=p;lst->redraw();
	return;
  }
  gui_obj::command(c,p);
}

void strlist2::scroll(void) {
  lst->scroll();
  sb->sld->cur=lst->tline;sb->sld->adjust();
  redraw();
}

// strlist2_k

strlist2_k::strlist2_k(int x,int y,int w,int h,char **p,int c,long s)
:strlist2(x,y,w,h,p,c,s) {
  lst->remove();delete lst;
  *this<<=*(lst=new strlist_k(0,0,w-fonth*2,h,p,c));
}

int strlist2_k::key(int k) {
  if(gui_obj::key(k)) {
    scroll();return 1;
  }
  return 0;
}

// checkbox

chkbox::chkbox(int x,int y,int w,int h,int c,int s,int k):gui_obj(x,y,w,h) {
  com=c;st=s?1:0;hotkey=k&0x7F;
}

void chkbox::draw(void) {
  V_clr(left,width,top,height,gray[2]);
  V_clr(left+1,width-1,top+1,height-1,gray[8]);
  V_clr(left+1,width-2,top+1,height-2,gray[4]);
  if(st) {
	vf_font=&smallfont;vf_color=gray[15];
	vf_x=left+(width-V_chrlen('√'))/2;
	vf_y=top+(height-fonth)/2;
	V_putch('√');
  }
}

int chkbox::key(int k) {
  if((k&0x7F)!=hotkey) return 0;
  while(keys[k]) GUI_wait();
  send_command(this,com,st=!st);
  redraw();
  return 1;
}

#pragma argsused
int chkbox::mouse(int x,int y,int b) {
  if(!b) return 1;
  while(I_mget(&x,&y)) GUI_wait();
  x-=left;y-=top;
  if(x<0 || x>=width || y<0 || y>=height) return 1;
  send_command(this,com,st=!st);
  redraw();
  return 1;
}

// text string

textstr::textstr(int x,int y,int w,int h,char *t,void *f,char c) {
  fnt=vf_font=f;clr=c;
  orgy=y;height=h;
  orgx=x-w*(width=V_strlen(txt=t))/2;
}

void textstr::newtext(int x,int y,int w,int h,char *t,void *f,char c) {
  redraw();
  fnt=vf_font=f;clr=c;
  orgy=y;height=h;
  orgx=x-w*(width=V_strlen(txt=t))/2;
  update_pos();
  redraw();
}

void textstr::draw(void) {
  vf_font=fnt;vf_color=gray[clr];
  vf_x=left;vf_y=top+(height-fonth)/2;V_puts(txt);
}

// input box

inputbox::inputbox(int x,int y,int w,int h,char *b,int s)
:gui_obj(x,y,w,h) {
  sz=s-1;cur=strlen(buf=b);
}

void inputbox::draw(void) {
  V_clr(left,width,top,height,gray[2]);
  V_clr(left+1,width-1,top+1,height-1,gray[8]);
  V_clr(left+1,width-2,top+1,height-2,gray[0]);
  vf_font=&smallfont;vf_color=gray[12];
  vf_x=left+2;vf_y=top+(height-fonth)/2;
  V_puts(buf);
  char c=buf[cur];buf[cur]=0;
  V_clr(left+2+V_strlen(buf),V_chrlen(c?c:' '),vf_y+fonth,1,gray[15]);
  buf[cur]=c;
}

int inputbox::key(int k) {
  static unsigned char kch[2][2][128]={{{
     0 , 0 ,'1','2','3','4','5','6','7','8','9','0','-','=', 0 , 0 ,
    'q','w','e','r','t','y','u','i','o','p','[',']', 0 , 0 ,'a','s',
    'd','f','g','h','j','k','l',';','\'','`', 0 ,'\\','z','x','c','v',
    'b','n','m',',','.','/', 0 ,'*', 0 ,' ', 0 , 0 , 0 , 0 , 0 , 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'-', 0 , 0 , 0 ,'+', 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
  },{
     0 , 0 ,'!','@','#','$','%','^','&','*','(',')','_','+', 0 , 0 ,
    'Q','W','E','R','T','Y','U','I','O','P','{','}', 0 , 0 ,'A','S',
    'D','F','G','H','J','K','L',':','\"','~', 0 ,'|','Z','X','C','V',
    'B','N','M','<','>','?', 0 ,'*', 0 ,' ', 0 , 0 , 0 , 0 , 0 , 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'-', 0 , 0 , 0 ,'+', 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
  }},{{
     0 , 0 ,'1','2','3','4','5','6','7','8','9','0','-','=', 0 , 0 ,
    'й','ц','у','к','е','н','г','ш','щ','з','х','ъ', 0 , 0 ,'ф','ы',
    'в','а','п','р','о','л','д','ж','э','ё', 0 ,'\\','я','ч','с','м',
    'и','т','ь','б','ю','.', 0 ,'*', 0 ,' ', 0 , 0 , 0 , 0 , 0 , 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'-', 0 , 0 , 0 ,'+', 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
  },{
     0 , 0 ,'!','\"','/','$','%',';',':','?','(',')','_','+', 0 , 0 ,
    'Й','Ц','У','К','Е','Н','Г','Ш','Щ','З','Х','Ъ', 0 , 0 ,'Ф','Ы',
    'В','А','П','Р','О','Л','Д','Ж','Э','Ё', 0 ,'|','Я','Ч','С','М',
    'И','Т','Ь','Б','Ю',',', 0 ,'*', 0 ,' ', 0 , 0 , 0 , 0 , 0 , 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,'-', 0 , 0 , 0 ,'+', 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,
     0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0
  }}};

  if(k==0x1C) return 0;
  if(k==0x3A) {keyb_mode=!keyb_mode;return 1;}
  if(k==1) {
    if(buf[0]) {buf[cur=0]=0;redraw();return 1;}
    else return 0;
  }
  int l=strlen(buf);
  if((k&0x7F)==0x4B) {
    if(cur>0) --cur;
    keys[k]=0;redraw();return 1;
  }
  if((k&0x7F)==0x4D) {
    if(cur<l) ++cur;
    keys[k]=0;redraw();return 1;
  }
  if((k&0x7F)==0x47) {
    keys[k]=0;cur=0;redraw();return 1;
  }
  if((k&0x7F)==0x4F) {
    keys[k]=0;cur=l;redraw();return 1;
  }
  if((k&0x7F)==0x53) {
    if(l>0 && cur<l) {
      memmove(buf+cur,buf+cur+1,l-cur);
      redraw();
    }
    keys[k]=0;return 1;
  }
  if(k==0x0E) {
    if(cur>0) {
      --cur;
      memmove(buf+cur,buf+cur+1,l-cur);
      redraw();
    }
    keys[k]=0;return 1;
  }
  int c;
  if(!(c=kch[keyb_mode?1:0][(keys[0x2A] || keys[0x36])?1:0][k&0x7F])) return 0;
  keys[k]=0;
  if(l>=sz) return 1;
  memmove(buf+cur+1,buf+cur,l-cur+1);
  buf[cur++]=c;redraw();
  return 1;
}

//---------------------------------------------------------------------------//

void GUI_close(void) {
  SV_done();I_close();K_done();
}

static void GUI_draw(void) {
  desktop.draw();
  V_spr(mx,my,mouseshape);
}

static volatile short lastkey;

static void gui_keyp(int k,int p) {
  if(!p) return;
  lastkey=k;
}

int _GUI_common_init() {
  lastkey=0;
  top_com=0;
  K_setkeyproc(gui_keyp);
  for(int i=0;i<16;++i) gray[i]=VP_findcolor(mainpal,256,i*4,i*4,i*4);
  VP_setall(mainpal);
  gui_scr=RD_init(0,sv_width,0,sv_height);
  if(!gui_scr) return 0;
  I_mget(&mx,&my);
  RD_start(gui_scr,GUI_draw);
  RD_rect(0,sv_width,0,sv_height);
  return 1;
}

void GUI_update(void) {
  I_mget(&mx,&my);
  RD_spr(mx,my,mouseshape);
  RD_end();
  RD_start(gui_scr,GUI_draw);
}

void GUI_wait(void) {
  int b;
  long p;
  gui_obj *o;

  lastkey=0;
  while(top_com) {
	b=combuf[0].c;p=combuf[0].p;o=combuf[0].o;
	if(--top_com) memmove(combuf,combuf+1,top_com*sizeof(combuf[0]));
	o->command(b,p);
  }
  GUI_update();
}

void _GUI_idle();

void GUI(void) {
  int b;
  long p;
  gui_obj *o;

  _GUI_idle();

  if((b=lastkey)!=0) {lastkey=0;desktop.key(b);}

  b=I_mget(&mx,&my)&3;
  RD_spr(mx,my,mouseshape);
  desktop.chkmouse(mx,my,b);

  while(top_com) {
    b=combuf[0].c;p=combuf[0].p;o=combuf[0].o;
    if(--top_com) memmove(combuf,combuf+1,top_com*sizeof(combuf[0]));
    o->command(b,p);
  }
  GUI_update();
}

void send_command(gui_obj *o,int c,long p) {
  if(top_com>=MAXCOM) return;
  combuf[top_com].c=c;
  combuf[top_com].p=p;
  combuf[top_com++].o=o;
}

void def_command(int c,long p) {
  switch(c) {
	case C_CANCEL: case C_OK: case C_YES: case C_NO:
	  msg=c;break;
  }
}

//---------------------------------------------------------------------------//

extern "C" int input(char *s,int sz,char *title) {
  vf_font=&smallfont;
  int ibw=(V_chrlen('W')+2)*sz;
  int w=ibw+fonth*2;
  if(w<fonth*15) w=fonth*15;
  if(title) if(w<V_strlen(title)+fonth*2) w=V_strlen(title)+fonth*2;
  int h=fonth*7;
  if(title) h+=fonth*3/2;
  mdialog box(mx-w/2,my-h/2,w,h);
  w=(box.width-fonth*13)/2;
  button b1(w,box.height-fonth*3,fonth*6,fonth*2,"О'кей",C_OK,0,0x1C),
	 b2(w+fonth*7,box.height-fonth*3,fonth*6,fonth*2,"Отмена",C_CANCEL,0,1);
  inputbox ib((box.width-ibw)/2,(title?5:2)*fonth/2,ibw,fonth*2,s,sz);
  desktop<<=box<<=(ib+=b1+=b2);
  textstr *t1=NULL;
  if(title) t1=new textstr(box.width/2,fonth/2,1,fonth*3/2,title,&smallfont,12);
  if(t1) box<<=*t1;
  box.redraw();
  for(;;) {
    for(msg=0;!msg;) GUI();
    if(msg==C_OK || msg==C_CANCEL) break;
  }
  int r=msg;
  box.remove();desktop.redraw();
  if(t1) delete t1;
  return (r==C_OK)?1:0;
}

extern "C" int message(int t,char *s,...) {
  int i=msgbutw+16;
  va_list ap;

  msg=0;
  switch(t) {
	case MB_YNC: i+=msgbutw+4;
	case MB_OKC: case MB_YN: i+=msgbutw+4;
  }
  va_start(ap,s);
  vsprintf(textbuf,s,ap);
  va_end(ap);
  msgbox m(i,msgbuth+14,textbuf,mx,my);
  desktop<<=m;
  i=m.width/2-i/2+8;
  int y=m.height-msgbuth-7;
  button bo(i,y,msgbutw,msgbuth,"О'кей",C_OK,0,0x1C),
		 bc(i+msgbutw+4,y,msgbutw,msgbuth,"Отмена",C_CANCEL,0,1),
		 by(i,y,msgbutw,msgbuth,"Да",C_YES,0,0x15),
		 bn(i+msgbutw+4,y,msgbutw,msgbuth,"Нет",C_NO,0,0x31);
  switch(t) {
	case MB_OKC: m<<=bc;
	case MB_OKEY: m<<=bo;break;
	case MB_C: bc.orgx=i;m<<=bc;break;
	case MB_YNC: bc.orgx+=msgbutw+4;m<<=bc;
	case MB_YN: m<<=bn;m<<=by;break;
  }
  desktop.update();
  while(!msg) GUI();
  m.remove();
  desktop.update();
  return msg;
}
