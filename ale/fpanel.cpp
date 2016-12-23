#include <stdio.h>
#include <direct.h>
#include <stdarg.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <ale.h>
#include "shell.h"
#include "attr.h"

void quit_command();

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//
//       FileMaskBox
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//

String file_sel_mask("*.*");

class InputDialog : public FrameBox {
public:
  String msg;
  InputBox inp;
  int status;

  InputDialog(int x,int y,int minw,char *hdr,char *text,char *ed)
  :FrameBox(x,y,FRAME_SX*2,FRAME_SY*2+2,attrs[A_PBOX],1,hdr) {
    status=0;
    msg=text;
    if(width<msg.length()+FRAME_SX*2) width=msg.length()+FRAME_SX*2;
    if(width<minw+FRAME_SX*2) width=minw+FRAME_SX*2;
    left=x-width/2;
    top=y-height/2;
    if(left+width>scrw) left=scrw-width;
    if(left<0) left=0;
    if(top+height>scrh) top=scrh-height;
    if(top<0) top=0;
    inp.inpattrs[0]=inp.inpattrs[1]=attrs[A_PBOX_INP];
    inp.width=width-FRAME_SX*2;inp.height=1;
    inp.left=left+FRAME_SX;inp.top=top+FRAME_SY+1;
    inp.settext(ed);inp.setsel(1);
  }
  ~InputDialog() {
    inp.setsel(0);
  }
  virtual void draw() {
    FrameBox::draw();
    prpos(left+(width-msg.length())/2,top+FRAME_SY);
    prstr(frameattr,msg);
    inp.draw();
  }
  virtual int key(int k) {
    if(inp.key(k)) return 1;
    if((k&0xFF)==13) {status=1;return 1;}
    else if((k&0xFF)==27) {status=-1;return 1;}
    return 0;
  }
  virtual void act() {
    FrameBox::act();
    inp.act();
  }
};

int input(String& ed,int x,int y,int minw,char *text,char *hdr) {
  InputDialog box(x,y,minw,hdr,text,ed);
  while(!box.status) {
    box.act();
    if(have_key()) {
      int c=get_key();
      box.key(c);
    }
    box.draw();
    updatescr();
  }
  if(box.status==1) {
    if(!box.inp.buf.length()) ed="";
    else ed=box.inp.buf;
    return 1;
  }
  return 0;
}

int file_mask_dialog(char *hdr,char *text,String& mask) {
  if(input(mask,scrw/2,scrh/2-4,14,text,hdr)) {
    if(!mask.length()) mask="*.*";
    return 1;
  }
  return 0;
}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//
//       change_drive_dialog
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//

short check_drive(char);
#pragma aux check_drive= \
  "mov ax,0x4409" \
  "int 0x21" \
  "jc err" \
  "mov ax,dx" \
  "and ax,0x1000" \
  "jmp ex" \
  "err:" \
  "mov ax,-1" \
  "ex:" \
  parm [bl] \
  modify [edx] \
  value [ax]

short check_fantom(char);
#pragma aux check_fantom= \
  "mov ax,0x440E" \
  "int 0x21" \
  "jnc ex" \
  "mov al,0" \
  "ex: xor ah,ah" \
  parm [bl] \
  value [ax]

int change_drive_dialog(int p,int cur) {
  MessageBox mb(p?"Choose right drive":"Choose left drive",0,-1);
  int w=0;
  for(int j='A';j<='Z';++j) if(check_drive(j-'A'+1)>=0) {
    char c=check_fantom(j-'A'+1)+'A'-1;
    if(c!='@' && c!=j) continue;
    ButtonData b;
    b.name=strdup("~A");
    b.name[1]=j;
    b.id=j-'A';b.hotkey=j;
    mb.but.append(1,&b);
    w+=1+3;
    if(j-'A'==cur) mb.cur=mb.but.count()-1;
  }
  if(w) w-=1;
  if(mb.width<w+FRAME_SX*2) mb.width=w+FRAME_SX*2;
  mb.left=p*scrw/2+scrw/4-mb.width/2;
  if(mb.left+mb.width>scrw) mb.left=scrw-mb.width;
  if(mb.left<0) mb.left=0;
  mb.top=(scrh-4-mb.height)/2;
  mb.butx=(mb.width-w)/2;mb.buty=mb.height-3;
  mb.set_hdr("Drive letter");
  for(mb.pressed_id=-2;mb.pressed_id==-2;) {
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

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//
//       FilePanel
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//

unsigned get_disk_free(char);
#pragma aux get_disk_free= \
  "mov ah,0x36" \
  "int 0x21" \
  "cmp ax,0xFFFF" \
  "jne ok" \
  "xor eax,eax" \
  "jmp x" \
  "ok:" \
  "movzx eax,ax" \
  "movzx ebx,bx" \
  "movzx ecx,cx" \
  "mul ecx" \
  "mul ebx" \
  "x:" \
  parm [dl] \
  modify [ebx ecx edx] \
  value [eax]

String int_to_str(int n,char c='.') {
  String s;
  int a=abs(n);
  if(a>=1000000000)
    s.printf(16,"%s%d%c%03d%c%03d%c%03d",(n<0)?"-":"",a/1000000000,c,
      a/1000000%1000,c,a/1000%1000,c,a%1000);
  else if(a>=1000000)
    s.printf(16,"%s%d%c%03d%c%03d",(n<0)?"-":"",a/1000000,c,a/1000%1000,c,a%1000);
  else if(a>=1000)
    s.printf(16,"%s%d%c%03d",(n<0)?"-":"",a/1000,c,a%1000);
  else
    s.printf(16,"%d",n);
  return s;
}

char *fd2name(char *s) {
  static char fn[16];
  char *p=fn;
  for(int i=0;s[i] && i<8;++i) *p++=s[i];
  if(s[8]) *p++='.';
  for(i=8;s[i] && i<11;++i) *p++=s[i];
  *p=0;
  return fn;
}

char *name2fd(char *p) {
  static char fn[16];
  for(int i=0;i<8;++i,++p)
    if(!*p || *p=='.') break; else fn[i]=*p;
  for(;i<8;++i) fn[i]=0;
  if(*p=='.') ++p;
  for(;i<11;++i,++p)
    if(!*p) break; else fn[i]=*p;
  for(;i<11;++i) fn[i]=0;
  return fn;
}

FilePanel::FilePanel(int x,int y,int w,int h,char *p):Panel(x,y,40,h) {
  path[0]=0;in_ale=0;grp=NULL;
  topline=curline=0;
  free_space=0;
  set_hdr("");
  set_path(p);
}

FilePanel::~FilePanel() {
  if(grp) {delete grp;grp=NULL;}
  path[0]=0;in_ale=0;
}

int FilePanel::set_path(char *_fn,int rr) {
  String fn;
  if(!_fn) fn=""; else{
    fn=_fn;
    if(fn.length())
      if(fn[fn.length()-1]!='\\' && fn[fn.length()-1]!='|') fn+="\\";
  }
  str_upr(fn);
  if(!fn[0]) {
    if(grp) {delete grp;grp=NULL;}
    in_ale=0;
    path[0]=subpath[0]=0;
    set_hdr("Drives");
    if(rr) re_read();
    return 1;
  }
  if(in_ale) {
    char *p1=strchr(fn,'|'),*p2=strchr(path,'|');
    if(p1 && p2) if(p1-fn==p2-path) if(str_nicmp(fn,path,p1-fn)==0) {
      strcpy(p2,p1);
      strcpy(subpath,p1+1);
      set_hdr(fn);
      if(rr) re_read();
      return 1;
    }
  }
  char *p=strchr(fn,'|');
  if(p) {
    *p=0;
    AleGrp *g=(AleGrp*)new AleGrpAleWr(fn);
    if(g) if(g->is_grp_ok()) {
      if(grp) delete grp;
      grp=g;*p='|';in_ale=1;
      strcpy(path,fn);strcpy(subpath,p+1);
      *p='|';set_hdr(fn);
      if(rr) re_read();
      return 1;
    }
    if(g) delete g;
    g=(AleGrp*)new AleGrpAle(fn);
    if(g) if(g->is_grp_ok()) {
      if(grp) delete grp;
      grp=g;*p='|';in_ale=1;
      strcpy(path,fn);strcpy(subpath,p+1);
      *p='|';set_hdr(fn);
      if(rr) re_read();
      return 1;
    }
    if(g) delete g;
    *p='|';
  }else{
    AleGrp *g=(AleGrp*)new AleGrpReal(fn);
    if(!g) return 0;
    if(grp) delete grp;
    grp=g;in_ale=0;
    strcpy(path,fn);subpath[0]=0;
    set_hdr(fn);
    if(rr) re_read();
    return 1;
  }
  return 0;
}

AleGrp *FilePanel::reset_grp() {
//  if(grp) {delete grp;grp=NULL;}
  chgdrv_failed=0;
  if(!set_path(path,0)) return NULL;
  return grp;
}

int FilePanel::go_to_name(char *s) {
  static char fn[8+3];

  char *p=s;
  for(int i=0;i<8;++i,++p)
    if(!*p || *p=='.') break; else fn[i]=*p;
  for(;i<8;++i) fn[i]=0;
  if(*p=='.') ++p;
  for(;i<11;++i,++p)
    if(!*p) break; else fn[i]=*p;
  for(;i<11;++i) fn[i]=0;
/*
  int ml=0,mi=-1;
  for(i=0;i<tab.count();++i) if(tab[i].type!=FT_UPDIR) {
    for(int j=0;j<11;++j) if(chr_upr(tab[i].name[j])!=chr_upr(fn[j])) break;
    if(j>ml) {ml=j;mi=i;}
  }
  if(mi>=0) {curline=mi;scroll();}
*/
  for(i=0;i<tab.count();++i) if(tab[i].type!=FT_UPDIR)
    if(mem_icmp(tab[i].name,fn,11)==0) break;
  if(i>=tab.count()) i=-1;
  else {curline=i;scroll();}
  return i;
}

int FilePanel::go_up_dir(void) {
  static char s[128],d[16];

  strcpy(s,path);
  if(!s[0]) return 0;
  else if(memcmp(s+1,":\\",3)==0) {
    s[0]=0;
  }else{
    for(int i=strlen(s)-2;i>=0;--i)
      if(s[i]=='\\' || s[i]=='|') {s[i+1]=0;break;}
  }
  strcpy(d,path+strlen(s));
  int l=strlen(d);
  if(l>0) if(d[l-1]=='\\' || d[l-1]=='|') d[l-1]=0;
  chgdrv_failed=0;
  if(!set_path(s)) return 0;
  go_to_name(d);
  return 1;
}

int FilePanel::go_sub_dir(char *fn,int dir) {
  static char s[128];

  strcpy(s,path);
  if(dir) {
    strcat(s,fn);strcat(s,"\\");
  }else{
//    int l=strlen(s)-1;
//    if(l<0) l=0;
//    if(s[l]=='\\') s[l]='|';
    strcat(s,fn);strcat(s,dir?"\\":"|");
  }
  chgdrv_failed=0;
  return set_path(s);
}

static int fd_sort_f(const FileData *a,const FileData *b) {
  if(a->type==FT_FILE && b->type!=FT_FILE) return 1;
  if(a->type!=FT_FILE && b->type==FT_FILE) return -1;
  int d=mem_icmp(a->name+8,b->name+8,3);
  if(d) return d;
  return mem_icmp(a->name,b->name,8);
}

void FilePanel::setcurpath() {
  if(path[1]==':') {
    char *p=strchr(path,'|');
    if(p) *p=0;
    int l=strlen(path)-1;
    if(path[l]=='\\') {
      path[l]=0;chdir(path);path[l]='\\';
    }else chdir(path);
    _dos_setdrive(chr_upr(path[0])-'A'+1,(unsigned*)&l);
    if(p) *p='|';
  }
}

void FilePanel::re_read(void) {
  static FileData f;
  static char mask[64];
  static alefind_t ff;

  tab.clear();
  free_space=0;
  curline=topline=0;

  if(!path[0]) {
    for(int j='A';j<='Z';++j) if(check_drive(j-'A'+1)>=0) {
      char c=check_fantom(j-'A'+1)+'A'-1;
      if(c!='@' && c!=j) continue;
      memset(&f,0,sizeof(f));
      f.name[0]=j;f.name[1]=':';
      f.type=FT_DRIVE;f.size=0;
      tab.append(1,&f);
    }
    return;
  }

  if(path[1]==':') {
    if(!chgdrv_failed) {
      free_space=get_disk_free(chr_upr(path[0])-'A'+1);
      if(!in_ale) {
        int l=strlen(path)-1;
        if(path[l]=='\\' && l!=2) {
          path[l]=0;chdir(path);path[l]='\\';
        }else chdir(path);
        _dos_setdrive(chr_upr(path[0])-'A'+1,(unsigned*)&l);
      }
    }else free_space=0;
  }

  memset(&f,0,sizeof(f));
  if(memcmp(path+1,":\\",3)==0) {
    f.name[0]=path[0];
    f.name[1]=':';
  }else{
    f.name[0]=f.name[1]='.';
  }
  f.type=FT_UPDIR;f.size=0;
  tab.append(1,&f);

  if(!chgdrv_failed) {
    strcat(strcpy(mask,subpath),"*.*");
    for(int ok=grp->find_first(mask,LA_SUBDIR,&ff);ok;ok=grp->find_next(&ff)) {
      char *p=ff.name;
      for(int i=0;i<8;++i,++p)
        if(!*p || *p=='.') break; else f.name[i]=*p;
      for(;i<8;++i) f.name[i]=0;
      if(*p=='.') ++p;
      for(;i<11;++i,++p)
        if(!*p) break; else f.name[i]=*p;
      for(;i<11;++i) f.name[i]=0;
      f.type=(ff.attr&LA_SUBDIR)?FT_SUBDIR:FT_FILE;
      f.size=ff.size;
      if(f.type==FT_SUBDIR)
        for(i=0;i<11;++i) f.name[i]=chr_upr(f.name[i]);
      else
        for(i=0;i<11;++i) f.name[i]=chr_lwr(f.name[i]);
      tab.append(1,&f);
    }
    grp->find_close(&ff);
  }
  int n=0;
  if(tab.count()) if(tab[0].type==FT_UPDIR) n=1;
  tab.sort(n,tab.count()-n,fd_sort_f);
}

void FilePanel::update_files() {
  chgdrv_failed=0;
  String s(fd2name(tab[curline].name));
  re_read();
  go_to_name(s);
}

int FilePanel::count_selected(int &fn,int &dn,int &sz) {
  fn=dn=sz=0;
  for(int i=0;i<tab.count();++i) if(tab[i].selected) {
    switch(tab[i].type) {
      case FT_SUBDIR: ++dn;break;
      case FT_FILE: ++fn;sz+=tab[i].size;break;
    }
  }
  return fn+dn;
}

int FilePanel::select_by_mask(char *mask,int t) {
  int n=0;
  for(int i=0;i<tab.count();++i)
   if(tab[i].type==FT_FILE || (tab[i].type==FT_SUBDIR && (t&SEL_DIR)))
    if(file_cmp(fd2name(tab[i].name),mask)==((t&SEL_EXCLUDE)?1:0)) {
      ++n;
      switch(t&~(SEL_EXCLUDE|SEL_DIR)) {
        case SEL_SELECT:
          tab[i].selected=1;break;
        case SEL_UNSELECT:
          tab[i].selected=0;break;
        case SEL_INVERT:
          tab[i].selected=!tab[i].selected;break;
      }
    }
  return n;
}

void FilePanel::draw(void) {
  Panel::draw();
  int n=height-6,l=0,x=left+1,y=0;
  for(int i=topline;i<tab.count() && l<n*3;++i,++l) {
    prpos(x,top+y+1);
    int a=(i==curline && selected);
    a=attrs[ a?(tab[i].selected?A_PANEL_MARK_CUR:A_PANEL_CUR)
      :(tab[i].selected?A_PANEL_MARK:A_PANEL) ];
    for(int j=0;j<8;++j) prch(tab[i].name[j],a);
    prch(' ',a);
    for(;j<11;++j) prch(tab[i].name[j],a);
    if(++y>=n) {y=0;x+=13;}
  }
  prpos(left,top+height-5);
  prch('Ç',attrs[A_PANEL]);
  for(i=2;i<width;++i) prch('Ä',attrs[A_PANEL]);
  prch('¶',attrs[A_PANEL]);
  for(i=0;i<n;++i) {
    prpos(left+13,top+i+1);prch('³',attrs[A_PANEL]);
    prpos(left+26,top+i+1);prch('³',attrs[A_PANEL]);
  }
  prpos(left+13,top+i+1);prch('Á',attrs[A_PANEL]);
  prpos(left+26,top+i+1);prch('Á',attrs[A_PANEL]);
  if(curline>=0 && curline<tab.count()) {
    prpos(left+1,top+height-4);
    prf(attrs[A_PANEL],"%-12s ",fd2name(tab[curline].name));
    switch(tab[curline].type) {
      case FT_FILE:
        prf(attrs[A_PANEL],"%14s",(char*)int_to_str(tab[curline].size));break;
      case FT_SUBDIR:
        prf(attrs[A_PANEL],"     <SUB-DIR>");break;
      case FT_DRIVE:
        prf(attrs[A_PANEL],"       <DRIVE>");break;
      case FT_UPDIR:
        prf(attrs[A_PANEL],"     <UP--DIR>");break;
    }
  }
  {
    int fn,dn,sz;
    if(count_selected(fn,dn,sz)) {
      String s,fs;
      if(fn && dn)
        fs.printf(40," in %d file%s and %d dir%s",fn,(fn!=1)?"s":"",dn,(dn!=1)?"s":"");
      else if(fn)
        fs.printf(40," in %d file%s",fn,(fn!=1)?"s":"");
      else
        fs.printf(40," in %d dir%s",dn,(dn!=1)?"s":"");
      s.printf(80,"%s byte%s%s",(char*)int_to_str(sz),(sz!=1)?"s":"",(char*)fs);
      if(s.length()>width-2) {
        s.printf(80,"%sK%s",(char*)int_to_str(sz>>10),(char*)fs);
        if(s.length()>width-2) {
          s.printf(80,"%sM%s",(char*)int_to_str(sz>>20),(char*)fs);
          if(s.length()>width-2) {
            s.printf(80,"%sK selected",(char*)int_to_str(sz>>10));
          }
        }
      }
      prpos(left+(width-s.length())/2,top+height-3);
      prstr(attrs[A_PANEL_MARK],s);
    }
  }
  if(path[0] && path[1]==':') {
    String s=int_to_str(free_space)+" bytes free";
    prpos(left+(width-s.length())/2,top+height-2);
    prstr(attrs[A_PANEL],s);
  }
}

void FilePanel::scroll(void) {
  int n=height-6;
  if(curline<topline) topline=curline;
  if(curline>=topline+n*3) topline=curline-n*3+1;
}

int FilePanel::key(int k) {
  if(!selected) return 0;
  if(k==0x4800) {
    if(--curline<0) curline=0;
    scroll();
    return 1;
  }else if(k==0x5000) {
    if(++curline>=tab.count()) curline=tab.count()-1;
    if(curline<0) curline=0;
    scroll();
    return 1;
  }else if(k==0x4B00) {
    if((curline-=height-6)<0) curline=0;
    scroll();
    return 1;
  }else if(k==0x4D00) {
    if((curline+=height-6)>=tab.count()) curline=tab.count()-1;
    if(curline<0) curline=0;
    scroll();
    return 1;
  }else if(k==0x4700) {
    curline=0;
    scroll();
    return 1;
  }else if(k==0x4F00) {
    curline=tab.count()-1;
    if(curline<0) curline=0;
    scroll();
    return 1;
  }else if((k&0xFF)==13) {
    if(curline<0 || curline>=tab.count()) return 1;
    if(tab[curline].type==FT_SUBDIR || tab[curline].type==FT_DRIVE)
      go_sub_dir(fd2name(tab[curline].name),1);
    else if(tab[curline].type==FT_UPDIR)
      go_up_dir();
    else if(tab[curline].type==FT_FILE)
      if(mem_icmp(tab[curline].name+8,"Ale",3)==0)
        go_sub_dir(fd2name(tab[curline].name),0);
    return 1;
  }else if((k&0xFF)==0x12) {
    chgdrv_failed=0;re_read();
    return 1;
  }else if(k==0x2D00) {
    quit_command();
    return 1;
  }else if(k==0x5200) {
    if(curline<0 || curline>=tab.count()) return 1;
    if(tab[curline].type==FT_FILE || tab[curline].type==FT_SUBDIR)
      tab[curline].selected=!tab[curline].selected;
    if(++curline>=tab.count()) curline=tab.count()-1;
      else clear_insert();
    if(curline<0) curline=0;
    scroll();
    return 1;
  }else if(k==0x4E2B) {
    if(file_mask_dialog("Select","Select the files",file_sel_mask))
      select_by_mask(file_sel_mask,SEL_SELECT);
    return 1;
  }else if(k==0x4A2D) {
    if(file_mask_dialog("Unselect","Unselect the files",file_sel_mask))
      select_by_mask(file_sel_mask,SEL_UNSELECT|SEL_DIR);
    return 1;
  }else if(k==0x372A) {
    for(int i=0;i<tab.count();++i) if(tab[i].type==FT_FILE)
      tab[i].selected=!tab[i].selected;
    return 1;
  }else if((k&0xFF)==0x1C) {
    if(path[0] && path[1]==':') {
      String s; s.printf(4,"%c:\\",path[0]);
      set_path(s);
    }
    return 1;
  }
  return 0;
}
