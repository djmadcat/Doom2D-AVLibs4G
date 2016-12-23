#include <stdio.h>
#include <direct.h>
#include <stdarg.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ale.h>
#include "shell.h"
#include "attr.h"
#include "fileop.h"

int change_drive_dialog(int p,int d);
char *fd2name(char *);
char *name2fd(char *);

//컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�//
//       KeyBar
//컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�//

KeyBar::KeyBar() {
  left=0;top=scrh-1;width=scrw;height=1;
  memset(text,' ',sizeof(text));
  shift=0;
}

void KeyBar::draw() {
  prpos(left,top);
  for(int i=0;i<10;++i) {
    if(i==9) prstr(attrs[A_KB_NUM],"10");
    else prch(i+'1',attrs[A_KB_NUM]);
    for(int j=0;j<6;++j) prch(text[shift][i][j],attrs[A_KB_TEXT]);
    if(i!=9) prch(' ',attrs[A_BACK]);
  }
}

void KeyBar::act() {
  int s=get_shift();
  if(s&SH_ALT) shift=3;
  else if(s&SH_CTRL) shift=2;
  else if(s&(SH_LEFT|SH_RIGHT)) shift=1;
  else shift=0;
}

int KeyBar::key(int k) {
  if(k&0xFF) return 0;
  k>>=8;
  if(k>=0x3B && k<=0x44) return key_pressed(0,k-0x3B);
  else if(k>=0x54 && k<=0x5D) return key_pressed(1,k-0x54);
  else if(k>=0x5E && k<=0x67) return key_pressed(2,k-0x5E);
  else if(k>=0x68 && k<=0x71) return key_pressed(3,k-0x68);
  return 0;
}

int KeyBar::key_pressed(int s,int n) {
  return 1;
}

//컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�//
//       MainKeyBar
//컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�//

class MainKeyBar : public KeyBar {
public:
  MainKeyBar() {
    memcpy(text,
    "Help  " "      " "      " "      " "Copy  "
    "Move  " "Mkdir " "Delete" "      " "Quit  "
    "      " "      " "      " "      " "      "
    "      " "      " "      " "      " "      "
    "      " "      " "      " "      " "      "
    "      " "MkAle " "      " "      " "      "
    "Left  " "Right " "      " "      " "      "
    "      " "Find  " "      " "      " "      "
    ,sizeof(text));
  }
  int key_pressed(int s,int n);
};

void quit_command() {
  if(message("Do you wish to quit\nAle Navigator?","Confirm",2,1,0,"~Yes","~No")!=0)
    return;
  delete panel[0];
  delete panel[1];
  close_video();exit(1);
}

int MainKeyBar::key_pressed(int s,int n) {
  FilePanel *fp,*ofp;
  if(panel[0]->selected) {
    fp=(FilePanel*)panel[0];
    ofp=(FilePanel*)panel[1];
  }else{
    fp=(FilePanel*)panel[1];
    ofp=(FilePanel*)panel[0];
  }
  n+=s*10+1;
  switch(n) {
    case 5: {
      int fn,dn,sz;
      if(!fp->count_selected(fn,dn,sz)) {
	if(fp->tab[fp->curline].type!=FT_FILE
	  && fp->tab[fp->curline].type!=FT_SUBDIR) return 1;
      }
      String s(ofp->path);
      if(!input(s,scrw/2,scrh/2-4,64,"Copy files to","Copy"))
	return 1;
      if(!s.length()) return 1;
      if(fp->grp) {delete fp->grp;fp->grp=NULL;fp->in_ale=0;}
      if(ofp->grp) {delete ofp->grp;ofp->grp=NULL;ofp->in_ale=0;}
      FileCopyOp *op=new FileCopyOp(s);
      if(!op) {ofp->reset_grp();fp->reset_grp();return 1;}
      if(!start_fileop(op)) {ofp->reset_grp();fp->reset_grp();return 1;}
      if(!fp->reset_grp()) {ofp->reset_grp();return 1;}
      if(fn+dn) {
	for(int i=0;i<fp->tab.count();++i) if(fp->tab[i].selected) {
	  switch(fp->tab[i].type) {
	    case FT_FILE:
	      if(fileop_file(fp->grp,fp->subpath,fd2name(fp->tab[i].name)))
		fp->tab[i].selected=0;
	      break;
	    case FT_SUBDIR:
	      if(fileop_dir(fp->grp,fp->subpath,fd2name(fp->tab[i].name)))
		fp->tab[i].selected=0;
	      break;
	  }
	  if(fp->tab[i].selected) break;
	}
      }else{
	switch(fp->tab[fp->curline].type) {
	  case FT_FILE:
	    fileop_file(fp->grp,fp->subpath,fd2name(fp->tab[fp->curline].name));
	    break;
	  case FT_SUBDIR:
	    fileop_dir(fp->grp,fp->subpath,fd2name(fp->tab[fp->curline].name));
	    break;
	}
      }
      delete op;
      ofp->reset_grp();
      fp->reset_grp();
      ofp->update_files();
      fp->update_files();
      }return 1;
    case 8: {
      FileDeleteOp *op=new FileDeleteOp;
      if(!op) return 1;
      if(!start_fileop(op)) return 1;
      int fn,dn,sz;
      if(fp->count_selected(fn,dn,sz)) {
	String s("You have selected\n");
	if(fn) s+=String(64,"%d file%s",fn,(fn>1)?"s":"");
	if(dn) s+=String(64,"%s%d director%s",fn?" and ":"",dn,(dn>1)?"ies":"y");
	if(message(s,"Delete",2,1,0,"Delete","Cancel")!=0) {delete op;return 1;}
	for(int i=0;i<fp->tab.count();++i) if(fp->tab[i].selected) {
	  switch(fp->tab[i].type) {
	    case FT_FILE:
	      if(fileop_file(fp->grp,fp->subpath,fd2name(fp->tab[i].name)))
		fp->tab[i].selected=0;
	      break;
	    case FT_SUBDIR:
	      if(fileop_dir(fp->grp,fp->subpath,fd2name(fp->tab[i].name)))
		fp->tab[i].selected=0;
	      break;
	  }
	  if(fp->tab[i].selected) break;
	}
      }else{
	if(fp->tab[fp->curline].type==FT_DRIVE) {
	  message(String(128,"Sorry, but you must remove drive %s manually",
	    fd2name(fp->tab[fp->curline].name)),
	    "Hardware problem",1,0,0,"OK");
	  delete op;return 1;
	}
	if(fp->tab[fp->curline].type!=FT_FILE
	  && fp->tab[fp->curline].type!=FT_SUBDIR) {delete op;return 1;}
	if(message(String(256,"Do you wish to delete%s\n%s",
	  (fp->tab[fp->curline].type==FT_SUBDIR)?" the directory":"",
	  fd2name(fp->tab[fp->curline].name)),
	  "Delete",2,1,0,"Delete","Cancel")!=0) {delete op;return 1;}
	switch(fp->tab[fp->curline].type) {
	  case FT_FILE:
	    fileop_file(fp->grp,fp->subpath,fd2name(fp->tab[fp->curline].name));
	    break;
	  case FT_SUBDIR:
	    fileop_dir(fp->grp,fp->subpath,fd2name(fp->tab[fp->curline].name));
	    break;
	}
      }
      delete op;
      fp->update_files();
      }return 1;
    case 1: {
      MessageBox mb("Press a key",0,0);
      mb.draw();updatescr();
      int c=get_key();
      String s;s.printf(16,"Code: %04X",c);
      message(s,"Help",1,0,0,"OK");
      }return 1;
    case 7: {
      if(!fp->path[0]) return 1;
      String s("");
      if(!input(s,scrw/2,scrh/2-4,64,"Create the directory","Make directory"))
	return 1;
      if(!s.length()) return 1;
      if(!fp->in_ale || (s[0]=='\\' || s[1]==':')) {
	if(mkdir(s)) {
	  message(String(160,"Can't create the directory\n%s",(char*)s),
	    "Make directory",1,0,0,"OK");
	  return 1;
	}
	chgdrv_failed=0;
	if(fp==panel[0]) ((FilePanel*)panel[1])->re_read();
	else ((FilePanel*)panel[0])->re_read();
	String old(fd2name(fp->tab[fp->curline].name));
	chgdrv_failed=0;fp->re_read();
	if(fp->go_to_name(s)<0) fp->go_to_name(old);
      }else{
	fp->go_sub_dir(s,1);
      }
      }return 1;
    case 27: {
      if(!fp->path[0]) return 1;
      String s("");
      if(!input(s,scrw/2,scrh/2-4,64,"Create the Ale","Make Ale"))
	return 1;
      if(!s.length()) return 1;
      char ext[5];
      _splitpath(s,NULL,NULL,NULL,ext);
      if(!ext[0]) s+=".Ale";
      else if(str_icmp(ext,".Ale")!=0) {
	message("Ale must have '.Ale' extension","Make Ale",1,0,0,"OK");
	return 1;
      }
      AleGrpAleWr *g=new AleGrpAleWr(s);
      if(g) if(!g->is_grp_ok()) {delete g;g=NULL;}
      if(!g) {
	message(String(160,"Can't create the Ale\n%s",(char*)s),
	  "Make Ale",1,0,0,"OK");
	return 1;
      }
      delete g;
      chgdrv_failed=0;
      if(fp==panel[0]) ((FilePanel*)panel[1])->re_read();
      else ((FilePanel*)panel[0])->re_read();
      String old(fd2name(fp->tab[fp->curline].name));
      chgdrv_failed=0;fp->re_read();
      if(fp->go_to_name(s)<0) fp->go_to_name(old);
      }return 1;
    case 10:
      quit_command();return 1;
    case 31: case 32: {
      FilePanel *fp=(FilePanel*)panel[n-31];
      if(fp->path[0] && fp->path[1]==':') {
	int d=change_drive_dialog(n-31,chr_upr(fp->path[0])-'A');
	if(d>=0) {
	  chgdrv_failed=0;
	  char *s=_getdcwd(d+1,NULL,0);
	  if(chgdrv_failed) if(s) {free(s);s=NULL;}
	  if(s) {
	    fp->set_path(s);
	    free(s);
	  }else{
	    static char p[]="A:\\";
	    p[0]=d+'A';
	    fp->set_path(p);
	  }
	}
      }
      }return 1;
    case 37:
      message("Not implemented.","Sorry!",1,0,0,"OK");
      return 1;
  }
  return 0;
}

static MainKeyBar main_kb;

KeyBar *get_keybar(int t) {
  KeyBar *kb=NULL;
  switch(t) {
    case KB_MAIN:               kb=&main_kb;break;
  }
  if(kb) {kb->left=0;kb->top=scrh-1;kb->width=scrw;kb->height=1;}
  return kb;
}
