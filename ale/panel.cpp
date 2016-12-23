#include <stdio.h>
#include <direct.h>
#include <stdarg.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ale.h>
#include "shell.h"
#include "attr.h"

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//
//       Box
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//

Box::~Box() {}

void Box::draw(void) {}

void Box::act(void) {}

int Box::key(int k) {k=k;return 0;}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//
//       Panel
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//

Panel::Panel(int x,int y,int w,int h,char *s) {
  left=x;top=y;width=w;height=h;
  set_hdr(s);selected=0;
}

Panel::~Panel() {
}

void Panel::set_hdr(char *s) {
  if(!s) {header.clear();return;}
  header=s;
}

void Panel::draw(void) {
  border(left,top,width,height,1,attrs[A_PANEL]);
  int sz=header.length();
  if(sz) {
    if(sz>width-4) sz=width-4;
    prpos(left+(width-sz-2)/2,top);
    prf(attrs[selected?A_PANEL_SEL_HDR:A_PANEL_HDR],
      " %s ",header+header.length()-sz);
  }
}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//
//       ButtonBox
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//

ButtonBox::ButtonBox() {cur=0;butx=buty=0;}

ButtonBox::ButtonBox(int x,int y,int w,int h) {
  left=x;top=y;width=w;height=h;
  cur=0;
  butx=2;buty=h-2;
}

ButtonBox::ButtonBox(int x,int y,int w,int h,int num,...) {
  left=x;top=y;width=w;height=h;
  cur=0;
  butx=2;buty=h-2;
  va_list ap;
  va_start(ap,num);
  setbut(w,h,num,ap);
  va_end(ap);
}

ButtonBox::ButtonBox(int x,int y,int w,int h,int num,va_list ap) {
  left=x;top=y;width=w;height=h;
  cur=0;
  butx=2;buty=h-2;
  setbut(w,h,num,ap);
}

void ButtonBox::setbut(int wd,int h,int num,va_list ap) {
  but.clear();
  width=wd;height=h;
  int w=0;
  for(int i=0;i<num;++i) {
    char *s=va_arg(ap,char *);
    if(s) {
      int j=but.append(1);
      if(j>=0) {
        w+=strlen(s)+3;
        but[j].name=strdup(s);
        but[j].id=i;
        but[j].hotkey=0;
        char *p=strchr(s,'~');
        if(p) {
          --w;
          but[j].hotkey=chr_upr(p[1]);
        }
      }
    }
  }
  if(w) w-=1;
  if(width<w+FRAME_SX*2) width=w+FRAME_SX*2;
  if(left+width>scrw) left=scrw-width;
  if(left<0) left=0;
  butx=(width-w)/2;
}

void ButtonBox::setbut(int w,int h,int num,...) {
  va_list ap;
  va_start(ap,num);
  setbut(w,h,num,ap);
  va_end(ap);
}

ButtonBox::~ButtonBox() {
  for(int i=0;i<but.count();++i) if(but[i].name) free(but[i].name);
}

void ButtonBox::draw() {
  prpos(left+butx,top+buty);
  for(int i=0;i<but.count();++i) {
    int a=(i==cur)?2:0;
    prch('[',butattr[a]);
//    prch(' ',butattr[a]);
    for(char *p=but[i].name;*p;++p) {
      if(*p=='~') prch(*(++p),butattr[a+1]);
      else prch(*p,butattr[a]);
    }
//    prch(' ',butattr[a]);
    prch(']',butattr[a]);
    skipch();//skipch();skipch();
  }
}

int ButtonBox::key(int k) {
  if(k==0x4B00) {
    if(--cur<0) cur=but.count()-1;
    if(cur<0) cur=0;
    return 1;
  }else if(k==0x4D00) {
    if(++cur>=but.count()) cur=0;
    return 1;
  }else if((k&0xFF)==13) {
    if(cur>=0 && cur<but.count()) press_button(but[cur].id);
    return 1;
  }else{
    if((k&0xFF)>=32) k=chr_upr(k&0xFF);
    for(int i=0;i<but.count();++i) if(but[i].hotkey==k) {
      cur=i;
      press_button(but[i].id);
      return 1;
    }
  }
  return 0;
}

void ButtonBox::press_button(int id) {}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//
//       MessageBox
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//

MessageBox::MessageBox(char *msg,int num,int esc,va_list ap) {
  butattr[0]=attrs[A_ERR_BUT];
  butattr[1]=attrs[A_ERR_BUT+1];
  butattr[2]=attrs[A_ERR_BUT+2];
  butattr[3]=attrs[A_ERR_BUT+3];
  frameattr=textattr=attrs[A_ERR_TEXT];
  frametype=1;
  pressed_id=-1;
  setmsg(msg,num,esc,ap);
}

MessageBox::MessageBox(char *msg,int num,int esc,...) {
  butattr[0]=attrs[A_ERR_BUT];
  butattr[1]=attrs[A_ERR_BUT+1];
  butattr[2]=attrs[A_ERR_BUT+2];
  butattr[3]=attrs[A_ERR_BUT+3];
  frameattr=textattr=attrs[A_ERR_TEXT];
  frametype=1;
  pressed_id=-1;
  va_list ap;
  va_start(ap,esc);
  setmsg(msg,num,esc,ap);
  va_end(ap);
}

void MessageBox::setmsg(char *msg,int num,int esc,va_list ap) {
  esc_id=esc;
  text=strdup(msg);
  int w=0,h=1,l=0;
  for(char *p=msg;*p;++p)
    if(*p=='\n' || *p=='\r') {
      if(l>w) w=l;
      ++h;l=0;
    }else ++l;
  if(l>w) w=l;
  setbut(w+10,h+6,num,ap);
  buty=height-3;
  left=(scrw-width)/2;
  top=(scrh-height)/2;
}

void MessageBox::setmsg(char *msg,int num,int esc,...) {
  va_list ap;
  va_start(ap,esc);
  setmsg(msg,num,esc,ap);
  va_end(ap);
}

void MessageBox::draw(void) {
  FrameBox::draw();
  char *bp=text,*p=text;
  for(int l=0,y=top+2;*p;++p)
    if(*p=='\r' || *p=='\n') {
      prpos((width-l)/2+left,y);
      for(;bp<p;++bp) prch(*bp,textattr);
      ++bp;++y;l=0;
    }else ++l;
  prpos((width-l)/2+left,y);
  prf(textattr,"%s",bp);
  ButtonBox::draw();
}

int MessageBox::key(int k) {
  if((k&0xFF)==27) {
    press_button(esc_id);
    return 1;
  }
  return ButtonBox::key(k);
}

void MessageBox::press_button(int id) {
  pressed_id=id;
}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//
//       FrameBox
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//

FrameBox::FrameBox(char a,char t,char *s) {
  frameattr=a;frametype=t;
  set_hdr(s);
}

FrameBox::FrameBox(int x,int y,int w,int h,char a,char t,char *s) {
  left=x;top=y;width=w;height=h;
  frameattr=a;frametype=t;
  set_hdr(s);
}

void FrameBox::draw() {
  clear(left,top,width,height,frameattr);
  border(left+3,top+1,width-6,height-2,frametype,frameattr);
  if(header.length()) {
    prpos(left+(width-header.length()-2)/2,top+1);
    prch(' ',frameattr);
    prstr(frameattr,header);
    prch(' ',frameattr);
  }
}

void FrameBox::set_hdr(char *s) {
  if(!s) {header.clear();return;}
  header=s;
}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//
//       InputBox
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//

InputBox::InputBox() {
  buf="";cur=0;skip=0;
  selected=0;touched=0;
}

InputBox::InputBox(int x,int y,int w,char *s) {
  left=x;top=y;width=w;height=1;
  buf=s;cur=0;skip=0;
  selected=0;touched=0;
}

void InputBox::draw(void) {
  clear(left,top,width,1,inpattrs[selected?1:0]);
  prpos(left,top);
  if(buf.count()) prf(inpattrs[selected?1:0],"%.*s",width,&buf[skip]);
  if(selected) movecur(left+cur,top);
}

void InputBox::scroll() {
  if(cur>=skip+width) skip=cur-width+1;
  else if(cur<skip) skip=cur;
  if(skip<0) skip=0;
}

int InputBox::caninsert(unsigned char c) {
  if(c==127) return 0;
  return 1;
}

int InputBox::key(int k) {
  if(!selected) return 0;
  if((k&0xFF)==8) {
    if(!buf.length()) return 1;
    if(cur>0) {buf.erase(cur-1,1);--cur;scroll();}
    touched=1;
    return 1;
  }else if(k==0x5300) {
    if(!buf.length()) return 1;
    if(cur<buf.length()) {buf.erase(cur,1);scroll();}
    touched=1;
    return 1;
  }else if(k==0x4B00) {
    if(--cur<0) cur=0;
    touched=1;
    scroll();return 1;
  }else if(k==0x4D00) {
    if(++cur>buf.length()) cur=buf.length();
    touched=1;
    scroll();return 1;
  }else if(k==0x4700) {
    touched=1;
    cur=0;scroll();return 1;
  }else if(k==0x4F00) {
    touched=1;
    cur=buf.length();scroll();return 1;
  }else if((k&0xFF)>=32) if(caninsert(k&0xFF)) {
    if(!buf.count()) {
      if(!buf.resize(1)) return 1;
      buf[0]=0;
    }
    if(!touched) {
      if(!buf.resize(1)) return 1;
      buf[0]=0;cur=0;touched=1;
    }
    buf.insert(cur,1,(char*)&k);
    ++cur;scroll();return 1;
  }
  return 0;
}

void InputBox::settext(char *s) {
  buf=s;cur=0;skip=0;
  touched=0;
}

void InputBox::setsel(int a) {
  if(selected && !a) hidecur();
  if(!selected && a) movecur(left+cur,top);
  selected=a;
}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//
//       message()
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//

int message(char *msg,char *hdr,int num,int esc,int def,...) {
  va_list ap;
  va_start(ap,def);
  MessageBox mb(msg,num,esc,ap);
  va_end(ap);
  mb.set_hdr(hdr);
  mb.cur=def;
  if(mb.cur>=mb.but.count()) mb.cur=0;
  for(;mb.pressed_id==-1;) {
    mb.act();
    if(have_key()) {
      int c=get_key();
      mb.key(c);
    }
    mb.draw();
    updatescr();
  }
  return mb.pressed_id;
}
