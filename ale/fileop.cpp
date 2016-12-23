#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <ale.h>
#include <str.h>
#include "shell.h"
#include "attr.h"
#include "fileop.h"

#define BUFSZ 0x10000

class CopyStatBox : public FrameBox {
public:
  String text,file1,file2;
  int cur,total,cancel;
  CopyStatBox(char *hdr,char *txt):FrameBox(3,scrh/2-7,scrw-6,FRAME_SY*2+5,attrs[A_PBOX],1,hdr) {
    text=txt;
    cur=total=0;
    cancel=0;
  }
  void draw() {
    FrameBox::draw();
    if(text.length()) {
      prpos(left+(width-text.length())/2,top+FRAME_SY);prstr(frameattr,text);
    }
    if(file1.length()) {
      prpos(left+(width-file1.length())/2,top+FRAME_SY+1);prstr(frameattr,file1);
    }
    prpos(left+width/2-1,top+FRAME_SY+2);prstr(frameattr,"to");
    if(file2.length()) {
      prpos(left+(width-file2.length())/2,top+FRAME_SY+3);prstr(frameattr,file2);
    }
    int t=width-FRAME_SX*2,c=0;if(total) c=cur*t/total;
    if(c>t) c=t;
    prpos(left+FRAME_SX,top+FRAME_SY+4);
    for(int i=0;i<c;++i) prch(' ',attrs[A_PBOX_BAR2]);
    for(;i<t;++i) prch(' ',attrs[A_PBOX_BAR1]);
  }
  int key(int k) {
    if((k&0xFF)==27) cancel=1;
    return 1;
  }
};

class DeleteStatBox : public FrameBox {
public:
  String text,file;
  int cancel;
  DeleteStatBox(char *hdr,char *txt):FrameBox(3,scrh/2-5,scrw-6,FRAME_SY*2+2,attrs[A_PBOX],1,hdr) {
    text=txt;
    cancel=0;
  }
  void draw() {
    FrameBox::draw();
    if(text.length()) {
      prpos(left+(width-text.length())/2,top+FRAME_SY);prstr(frameattr,text);
    }
    if(file.length()) {
      prpos(left+(width-file.length())/2,top+FRAME_SY+1);prstr(frameattr,file);
    }
  }
  int key(int k) {
    if((k&0xFF)==27) cancel=1;
    return 1;
  }
};

static CopyStatBox *copybox=NULL;
static DeleteStatBox *delbox=NULL;
static void *savscr=NULL;

void drawbox(Box *b) {
  while(have_key()) {
    int c=get_key();
    if(b) b->key(c);
  }
  drawscr(savscr);
  if(b) b->draw();
  updatescr();
}

FileOp::~FileOp() {}

int FileOp::dir_post_op(AleGrp *g,char *sp,char *s) {
  return 1;
}

FileCopyOp::FileCopyOp(char *to) {
  ow_all=0;
  if(savscr) {free(savscr);savscr=NULL;}
  savscr=grabscr();
  copybox=new CopyStatBox("Copy","Copying the file");
  dest=to;
  char *p=strchr(to,'|');
  dgrp=NULL;buf=NULL;
  if(p) {
    *p=0;
    dgrp=new AleGrpAleWr(to);
    *p='|';
    if(dgrp) if(!dgrp->is_grp_ok()) {delete dgrp;dgrp=NULL;}
    if(!dgrp) return;
    dest=p+1;
  }else{
    dgrp=new AleGrpReal(to);
    if(dgrp) if(!dgrp->is_grp_ok()) {delete dgrp;dgrp=NULL;}
    if(!dgrp) return;
    dest="";
  }
  buf=malloc(BUFSZ);
}

FileCopyOp::~FileCopyOp() {
  if(dgrp) delete dgrp;
  if(buf) free(buf);
  if(copybox) {delete copybox;copybox=NULL;}
}

int FileCopyOp::is_ok() {
  return (dgrp!=NULL) && (buf!=NULL) && (copybox!=NULL);
}

int FileCopyOp::file(AleGrp *g,char *sp,char *s) {
  if(dgrp->file_exist(String(dest)+String(s))) {
    String fn;dgrp->get_full_path(String(dest)+String(s),fn);
    int m=0;
    if(!ow_all) m=message(String(256,"The following file exist\n%s\n"
      "Do you wish to write over the old file?",(char*)fn),"Copy",4,2,0,
      "~Overwrite","~All","~Skip","Cancel");
    switch(m) {
      case 1: ow_all=1;break;
      case 2: return 1;
      case 3: return 0;
    }
  }
  if(copybox) {
    g->get_full_path(String(sp)+String(s),copybox->file1);
    dgrp->get_full_path(String(dest)+String(s),copybox->file2);
    copybox->cur=0;copybox->total=0;
    drawbox(copybox);
  }
  int ih=g->open(String(sp)+String(s),LF_READ);
  if(ih==-1) {
    message(String("Can't open file\n")+String(s),"Copy",1,0,0,"OK");
    return 0;
  }
  int oh=dgrp->open(String(dest)+String(s),LF_WRITE|LF_CREATE);
  if(oh==-1) {
    message(String("Can't create file\n")+String(s),"Copy",1,0,0,"OK");
    g->close(ih);return 0;
  }
  int l=g->length(ih);
  if(copybox) copybox->total=l*2;
  for(;l>0;) {
    int n=(l<BUFSZ)?l:BUFSZ;
    if(g->read(ih,buf,n)!=n) {
      message(String("Can't read file\n")+String(s),"Copy",1,0,0,"OK");
      g->close(ih);dgrp->close(oh);return 0;
    }
    if(copybox) {
      copybox->cur+=n;drawbox(copybox);
      if(copybox->cancel)
        {g->close(ih);dgrp->close(oh);dgrp->erase(String(dest)+String(s),0);return 0;}
    }
    if(dgrp->write(oh,buf,n)!=n) {
      message(String("Can't write file\n")+String(s),"Copy",1,0,0,"OK");
      g->close(ih);dgrp->close(oh);return 0;
    }
    if(copybox) {
      copybox->cur+=n;drawbox(copybox);
      if(copybox->cancel)
        {g->close(ih);dgrp->close(oh);dgrp->erase(String(dest)+String(s),0);return 0;}
    }
    l-=n;
  }
  if(g->close(ih)==-1) {
    message(String("Can't close file\n")+String(s),"Copy",1,0,0,"OK");
    dgrp->close(oh);return 0;
  }
  if(dgrp->close(oh)==-1) {
    message(String("Can't close file\n")+String(s),"Copy",1,0,0,"OK");
    return 0;
  }
  return 1;
}

int FileCopyOp::dir(AleGrp *g,char *sp,char *s) {
  String fn(dest);fn+=s;
  if(dgrp->dir_exist(fn)) return 1;
  return dgrp->mkdir(fn);
}

//---------------------------------------------------------------------------//

FileDeleteOp::FileDeleteOp() {
  del_all=0;
  if(savscr) {free(savscr);savscr=NULL;}
  savscr=grabscr();
  delbox=new DeleteStatBox("Delete","");
}

FileDeleteOp::~FileDeleteOp() {
  if(delbox) {delete delbox;delbox=NULL;}
}

int FileDeleteOp::is_ok() {
  return (delbox!=NULL);
}

int FileDeleteOp::file(AleGrp *g,char *sp,char *s) {
  if(delbox) {
    g->get_full_path(String(sp)+String(s),delbox->file);
    drawbox(delbox);
  }
  return g->erase(String(sp)+String(s),0);
}

int FileDeleteOp::dir(AleGrp *g,char *sp,char *s) {
  String fn(sp);fn+=s;
  if(g->rmdir(fn)) return 1;
  if(g->dir_exist(fn)) {
    String dir;g->get_full_path(fn,dir);
    int m=0;
    if(!del_all) m=message(String(256,"The following subdirectory is not empty\n%s\n"
      "Do you wish to delete it?",(char*)fn),"Delete",3,2,0,
      "~Delete","~All","Cancel");
    switch(m) {
      case 1: del_all=1;
      case 0: return 1;
      case 2: return 0;
    }
  }
  return 0;
}

int FileDeleteOp::dir_post_op(AleGrp *g,char *sp,char *s) {
  String fn(sp);fn+=s;
  if(!g->dir_exist(fn)) return 1;
  return g->rmdir(fn);
}

//---------------------------------------------------------------------------//

static FileOp *oper=NULL;

int start_fileop(FileOp *op) {
  oper=op;
  if(op) if(!op->is_ok()) return 0;
  return op!=NULL;
}

int fileop_file(AleGrp *g,char *sp,char *s) {
  return oper->file(g,sp,s);
}

int fileop_dir(AleGrp *g,char *sp,char *_s) {
  String s(_s);
  int ok=oper->dir(g,sp,s);
  if(!ok) return 0;
  alefind_t ff;
  for(ok=g->find_first(String(sp)+s+String("\\*.*"),LA_SUBDIR,&ff);ok;ok=g->find_next(&ff)) {
    String ns(128,"%s\\%s",(char*)s,ff.name);
//    printf("%s %s\n",(char*)ns,(ff.attr&LA_SUBDIR)?"<SUBDIR>":"<FILE>");
    if(ff.attr&LA_SUBDIR) {
      if(!fileop_dir(g,sp,ns)) {
//        printf("dir '%s' error:\n%s\n",(char*)ns,strerror(errno));
        g->find_close(&ff);return 0;
      }
    }else{
      if(!oper->file(g,sp,ns)) {
//        printf("file '%s' error\n",(char*)ns);
        g->find_close(&ff);return 0;
      }
    }
  }
  g->find_close(&ff);
  ok=oper->dir_post_op(g,sp,s);
  return ok;
}
