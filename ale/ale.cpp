#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <io.h>
#include <fcntl.h>
#include <ale.h>
#include <str.h>
#include "dll.h"

typedef struct{
  char id[4];
  int tabofs,tabsize;
}AleHdr;

typedef struct{
  AleGrp *g;
  int h;
}Lfile;

static Tab<AleGrp*> grp;

extern "C" DLLEXPORT void init_ale(void) {
  init_local_conv();
}

extern "C" DLLEXPORT void shutdown_ale(void) {
  for(int i=0;i<grp.count();++i) if(grp[i]) delete(grp[i]);
  grp.clear();
}

DLLEXPORT AleGrp *get_ale_grp(int h) {
  if(h<0 || h>=grp.count()) return NULL;
  return grp[h];
}

extern "C" DLLEXPORT int add_grp_real(char *path) {
  AleGrpReal *p=new AleGrpReal(path);
  if(!p) return -1;
  return grp.append(1,(AleGrp**)&p);
}

extern "C" DLLEXPORT int add_grp_ale(char *path) {
  AleGrpAle *p=new AleGrpAle(path);
  if(!p) return -1;
  if(!p->is_grp_ok()) return -1;
  return grp.append(1,(AleGrp**)&p);
}

extern "C" DLLEXPORT int add_grp_ale_wr(char *path) {
  AleGrpAleWr *p=new AleGrpAleWr(path);
  if(!p) return -1;
  if(!p->is_grp_ok()) return -1;
  return grp.append(1,(AleGrp**)&p);
}

DLLEXPORT int add_grp(AleGrp *p) {
  if(!p) return -1;
  if(!p->is_grp_ok()) return -1;
  return grp.append(1,&p);
}

extern "C" DLLEXPORT void remove_grp(int h) {
  if(h<0 || h>=grp.count()) return;
  if(grp[h]) {delete(grp[h]);grp[h]=NULL;}
}

extern "C" DLLEXPORT int L_find_first(char *mask,char attr,alefind_t *fs) {
  if(!fs) return 0;
  fs->grp=-1;
  strncpy(fs->fmask,mask,sizeof(fs->fmask)-1);fs->fmask[sizeof(fs->fmask)-1]=0;
  fs->fattr=attr;
  for(int i=0;i<grp.count();++i) if(grp[i])
    if(grp[i]->find_first(mask,attr,fs)) {
      fs->grp=i;return 1;
    }
  return 0;
}

extern "C" DLLEXPORT int L_find_next(alefind_t *fs) {
  if(grp[fs->grp]->find_next(fs)) return 1;
  static alefind_t f;
  f=*fs;
  for(int i=fs->grp+1;i<grp.count();++i) if(grp[i]) {
    if(grp[i]->find_first(fs->fmask,fs->fattr,&f)) {
      grp[fs->grp]->find_close(fs);
      *fs=f;fs->grp=i;
      return 1;
    }
  }
  return 0;
}

extern "C" DLLEXPORT int L_find_close(alefind_t *fs) {
  if(fs->grp<0 || fs->grp>=grp.count()) return 0;
  return grp[fs->grp]->find_close(fs);
}

DLLEXPORT _callback_open callback_open=NULL;

extern "C" DLLEXPORT LFILE L_open(char *fn,int flg) {
  Lfile *h=new Lfile;
  if(!h) return NULL;
  for(int i=0;i<grp.count();++i) if(grp[i])
    if((h->h=grp[i]->open(fn,flg))!=-1) {
      h->g=grp[i];
      if(callback_open) callback_open(fn,flg);
      return (LFILE)h;
    }
  delete h;
  return NULL;
}

extern "C" DLLEXPORT int L_rename(char *oldname,char *newname,int flg) {
  for(int i=0;i<grp.count();++i) if(grp[i])
    if(grp[i]->rename(oldname,newname,flg)) return 1;
  return 0;
}

extern "C" DLLEXPORT int L_erase(char *filename,int flg) {
  for(int i=0;i<grp.count();++i) if(grp[i])
    if(grp[i]->erase(filename,flg)) return 1;
  return 0;
}

extern "C" DLLEXPORT int L_close(LFILE h) {
  return ((Lfile*)h)->g->close(((Lfile*)h)->h);
}

extern "C" DLLEXPORT int L_read(LFILE h,void *ptr,int len) {
  return ((Lfile*)h)->g->read(((Lfile*)h)->h,ptr,len);
}

extern "C" DLLEXPORT int L_write(LFILE h,void *ptr,int len) {
  return ((Lfile*)h)->g->write(((Lfile*)h)->h,ptr,len);
}

extern "C" DLLEXPORT char *L_gets(LFILE h,char *ptr,int len) {
  for (int i=0;i<len;++i) {
    int r=((Lfile*)h)->g->read(((Lfile*)h)->h,&ptr[i],1);
    if (!r) if(!i) return NULL; else break;
    if (ptr[i]=='\n') break;
    if (ptr[i]==0) break;
  }
  return ptr;
}

extern "C" DLLEXPORT int L_puts(LFILE h,char *ptr) {
  for (int i=0;;++i) {
    if (ptr[i]=='\n') break;
    if (ptr[i]==0) break;
    int r=((Lfile*)h)->g->write(((Lfile*)h)->h,&ptr[i],1);
    if (!r) return 0;
  }
  return i;
}


extern "C" DLLEXPORT int L_seek(LFILE h,int offset,int how) {
  return ((Lfile*)h)->g->seek(((Lfile*)h)->h,offset,how);
}

extern "C" DLLEXPORT int L_tell(LFILE h) {
  return ((Lfile*)h)->g->tell(((Lfile*)h)->h);
}

extern "C" DLLEXPORT int L_length(LFILE h) {
  return ((Lfile*)h)->g->length(((Lfile*)h)->h);
}

extern "C" DLLEXPORT int L_is_ale(LFILE h) {
  return ((Lfile*)h)->g->class_id()==ALE_ALE;
}

extern "C" DLLEXPORT int L_is_real(LFILE h) {
  return ((Lfile*)h)->g->class_id()==ALE_REAL;
}

extern "C" DLLEXPORT int L_file_exist(char *fn,int flg) {
  for(int i=0;i<grp.count();++i) if(grp[i]) {
    if(grp[i]->class_id()==ALE_REAL) {
      if(flg & LF_ALE) continue;
    }else{
      if(flg & LF_REAL) continue;
    }
    if(grp[i]->file_exist(fn)) return 1;
  }
  return 0;
}

extern "C" DLLEXPORT int L_dir_exist(char *fn,int flg) {
  for(int i=0;i<grp.count();++i) if(grp[i]) {
    if(grp[i]->class_id()==ALE_REAL) {
      if(flg & LF_ALE) continue;
    }else{
      if(flg & LF_REAL) continue;
    }
    if(grp[i]->dir_exist(fn)) return 1;
  }
  return 0;
}

DLLEXPORT AleGrp::~AleGrp() {}

//ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ//
//       AleGrpReal
//ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ//

DLLEXPORT AleGrpReal::AleGrpReal(char *path) {
  basepath[0]=0;
  if(path) {
    _fullpath(basepath,path,sizeof(basepath));
    int l=strlen(basepath);
    if(l>0) if(basepath[l-1]!='\\' && basepath[l-1]!='/') strcat(basepath,"\\");
  }
}

DLLEXPORT int AleGrpReal::is_grp_ok(void) {
  return 1;
}

DLLEXPORT int AleGrpReal::open(char *fn,int flg) {
  if(flg & LF_ALE) return -1;
  int of=O_BINARY,sf=0;
  switch(flg & LF_RDWR) {
    case LF_RDWR: of|=O_RDWR; sf|=S_IREAD|S_IWRITE; break;
    case LF_READ: of|=O_RDONLY; sf|=S_IREAD; break;
    case LF_WRITE: of|=O_WRONLY; sf|=S_IWRITE; break;
  }
  if(flg & LF_CREATE) of|=O_CREAT|O_TRUNC;
  String str=String(basepath)+String(fn);
//  printf("open %s\n",(char*)str);
  int h=::open(str,of,sf);
  return h;
}

DLLEXPORT int AleGrpReal::close(int h) {
  return ::close(h);
}

DLLEXPORT int AleGrpReal::read(int h,void *ptr,int len) {
  return ::read(h,ptr,len);
}

DLLEXPORT int AleGrpReal::write(int h,void *ptr,int len) {
  return ::write(h,ptr,len);
}

DLLEXPORT int AleGrpReal::seek(int h,int ofs,int how) {
  return ::lseek(h,ofs,how);
}

DLLEXPORT int AleGrpReal::tell(int h) {
  return ::tell(h);
}

DLLEXPORT int AleGrpReal::length(int h) {
  return ::filelength(h);
}

DLLEXPORT int AleGrpReal::rename(char *oldname,char *newname,int flg) {
  if(flg & LF_ALE) return 0;
  String o=String(basepath)+String(oldname),
    n=String(basepath)+String(newname);
//  printf("rename %s to %s\n",(char*)o,(char*)n);
  return !::rename(o,n);
}

DLLEXPORT int AleGrpReal::erase(char *fn,int flg) {
  if(flg & LF_ALE) return 0;
  String str=String(basepath)+String(fn);
//  printf("erase %s\n",(char*)str);
  return !::remove(str);
}

DLLEXPORT int AleGrpReal::mkdir(char *s) {
  String str=String(basepath)+String(s);
//  printf("mkdir %s\n",(char*)str);
  return !::mkdir(str);
}

DLLEXPORT int AleGrpReal::rmdir(char *s) {
  String str=String(basepath)+String(s);
//  printf("rmdir %s\n",(char*)str);
  return !::rmdir(str);
}

DLLEXPORT int AleGrpReal::get_full_path(char *fn,String &s) {
  s=String(basepath)+String(fn);
  return 1;
}

DLLEXPORT int AleGrpReal::class_id() {return ALE_REAL;}

DLLEXPORT int AleGrpReal::file_exist(char *fn) {
  return !access(String(basepath)+String(fn),0);
}

DLLEXPORT int AleGrpReal::dir_exist(char *fn) {
  return !access(String(basepath)+String(fn),0);
}

//ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ//
//       AleGrpAle
//ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ//

static int ale_sort_f(const void *a,const void *b) {
  AleElem *pa=(AleElem*)a,*pb=(AleElem*)b;
  int c=str_icmp(pa->name,pb->name);
  if(c==0) {
    if(pa->erased==pb->erased) return 0;
    return (pa->erased)?1:-1;
  }
  return c;
}

DLLEXPORT AleGrpAle::AleGrpAle() {
  handle=-1;tab=NULL;num=0;
  alepath[0]=0;
  aledir=NULL;
}

DLLEXPORT AleGrpAle::AleGrpAle(char *fn) {
  aledir=NULL;
  handle=::open(fn,O_BINARY|O_RDONLY);
  if(handle==-1) return;
  _fullpath(alepath,fn,sizeof(alepath));
  static AleHdr hdr;
  if(::read(handle,&hdr,sizeof(hdr))!=sizeof(hdr))
    {::close(handle);handle=-1;return;}
  if(memcmp(hdr.id,"ALE\x1A",sizeof(hdr.id))!=0)
    {::close(handle);handle=-1;return;}
  if(::lseek(handle,hdr.tabofs,SEEK_SET)==-1)
    {::close(handle);handle=-1;return;}
  if(hdr.tabsize) if(!(tab=(AleElem*)malloc(hdr.tabsize)))
    {::close(handle);handle=-1;return;}
  if(::read(handle,tab,hdr.tabsize)!=hdr.tabsize)
    {free(tab);::close(handle);handle=-1;return;}
  num=hdr.tabsize/sizeof(AleElem);
  tabofs=hdr.tabofs;
  qusort(tab,num,sizeof(AleElem),ale_sort_f);
}

DLLEXPORT int AleGrpAle::is_grp_ok(void) {
  return (handle!=-1);
}

DLLEXPORT AleGrpAle::~AleGrpAle() {
  if(handle!=-1) {::close(handle);handle=-1;}
  if(tab) {free(tab);tab=NULL;}
  if(aledir) {delete aledir;aledir=NULL;}
}

//€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€//

DLLEXPORT AleDir::AleDir() {
  name[0]=0;
}

DLLEXPORT AleDir::AleDir(char *s) {
  strcpy(name,s);
}

DLLEXPORT AleDir::~AleDir() {
  for(int i=0;i<dir.count();++i) if(dir[i]) delete dir[i];
}

DLLEXPORT int AleDir::add_fn(char *s,int id) {
  char *p=strchr(s,'\\');
  if(!p) {
    int i=fn.append(1);
    if(i<0) return 0;
    strcpy(fn[i].name,s);
    fn[i].i=id;
    return 1;
  }
  *p=0;
  for(int i=0;i<dir.count();++i) if(str_icmp(dir[i]->name,s)==0) {
    *p='\\';
    return dir[i]->add_fn(p+1,id);
  }
  AleDir *d=new AleDir(s);
  if(!d) return 0;
  if(dir.append(1,&d)<0) {delete d;return 0;}
  *p='\\';
  return d->add_fn(p+1,id);
}

static int cmp_fns(AleDirFn const *a,AleDirFn const *b) {
  return str_icmp(a->name,b->name);
}

static int cmp_dirs(AleDir *const* a,AleDir *const* b) {
  return str_icmp((*a)->name,(*b)->name);
}

DLLEXPORT void AleDir::sort() {
  fn.sort(cmp_fns);
  dir.sort(cmp_dirs);
  for(int i=0;i<dir.count();++i) dir[i]->sort();
}

DLLEXPORT int AleGrpAle::build_aledir() {
  if(aledir) delete aledir;
  aledir=new AleDir;
  if(!aledir) return 0;
  for(int i=0;i<num;++i) if(!tab[i].erased)
    if(!aledir->add_fn(tab[i].name,i))
      {delete aledir;return 0;}
  aledir->sort();
  return 1;
}

//€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€//

static int ale_find_f(const void *a,const void *b) {
  AleElem *p=(AleElem*)b;
  int c=str_icmp((const char*)a,p->name);
  if(c==0) return (p->erased)?-1:0;
  return c;
}

/*
DLLEXPORT int AleGrpAle::find_fn(char *fn) {
  AleElem *p=(AleElem*)bsearch(fn,tab,num,sizeof(AleElem),ale_find_f);
  if(!p) return -1;
  return p-tab;
}
*/

static int fnd_fn(const void *a,const void *b) {
  return str_icmp((char*)a,((AleDirFn*)b)->name);
}

static int fnd_dir(const void *a,const void *b) {
  return str_icmp((char*)a,(*(AleDir**)b)->name);
}

DLLEXPORT int AleGrpAle::find_fn(char *fn) {
  if(!aledir) {
    if(!build_aledir()) {
      AleElem *p=(AleElem*)bsearch(fn,tab,num,sizeof(AleElem),ale_find_f);
      if(!p) return -1;
      return p-tab;
    }
  }
  AleDir *d=aledir;
  for(;;) {
    char *p=strchr(fn,'\\');
    if(!p) {
      AleDirFn *f=(AleDirFn*)bsearch(fn,&d->fn[0],d->fn.count(),sizeof(AleDirFn),fnd_fn);
      if(!f) return -1;
      return f->i;
    }
    *p=0;
    AleDir **nd=(AleDir**)bsearch(fn,&d->dir[0],d->dir.count(),sizeof(AleDir*),fnd_dir);
    *p='\\';
    if(!nd) return -1;
    d=*nd;fn=p+1;
  }
}

//€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€€//

DLLEXPORT int AleGrpAle::open(char *fn,int flg) {
  if(flg & LF_REAL) return -1;
  if(flg & (LF_WRITE|LF_CREATE)) return -1;
  int i=find_fn(fn);
  if(i==-1) return -1;
  int h=otab.append(1);
  if(h==-1) return -1;
  otab[h].h=i;otab[h].cur=0;
  return h;
}

DLLEXPORT int AleGrpAle::close(int h) {
  if(h<0 || h>=otab.count()) return -1;
  if(otab[h].h==-1) return -1;
  otab[h].h=-1;
  for(int i=otab.count()-1;i>=0;--i) if(otab[i].h!=-1) break;
  if(otab.count()-i-1>0) {otab.erase(i+1,otab.count()-i-1);otab.shrink();}
  return 0;
}

DLLEXPORT int AleGrpAle::read(int h,void *ptr,int len) {
  if(h<0 || h>=otab.count()) return -1;
  if(otab[h].h==-1) return -1;
  if(::lseek(handle,tab[otab[h].h].ofs+otab[h].cur,SEEK_SET)==-1) return -1;
  int sz=tab[otab[h].h].len-otab[h].cur;
  if(len<sz) sz=len;
  sz=::read(handle,ptr,sz);
  if(sz!=-1) otab[h].cur+=sz;
  return sz;
}

DLLEXPORT int AleGrpAle::write(int h,void *ptr,int len) {
  h=h;ptr=ptr;len=len;
  return -1;
}

DLLEXPORT int AleGrpAle::seek(int h,int ofs,int how) {
  if(h<0 || h>=otab.count()) return -1;
  if(otab[h].h==-1) return -1;
  switch(how) {
    case LF_SEEK_SET: otab[h].cur=ofs;break;
    case LF_SEEK_CUR: otab[h].cur+=ofs;break;
    case LF_SEEK_END: otab[h].cur=tab[otab[h].h].len+ofs;break;
    default: return -1;
  }
  if(otab[h].cur<0 || otab[h].cur>tab[otab[h].h].len) return -1;
  return otab[h].cur;
}

DLLEXPORT int AleGrpAle::tell(int h) {
  if(h<0 || h>=otab.count()) return -1;
  if(otab[h].h==-1) return -1;
  return otab[h].cur;
}

DLLEXPORT int AleGrpAle::length(int h) {
  if(h<0 || h>=otab.count()) return -1;
  if(otab[h].h==-1) return -1;
  return tab[otab[h].h].len;
}

DLLEXPORT int AleGrpAle::rename(char *on,char *nn,int flg) {
  on=on;nn=nn;flg=flg;
  return 0;
}

DLLEXPORT int AleGrpAle::erase(char *fn,int flg) {
  fn=fn;flg=flg;
  return 0;
}

DLLEXPORT int AleGrpAle::mkdir(char *s) {
  return 1;
}

DLLEXPORT int AleGrpAle::rmdir(char *s) {
  return 1;
}

DLLEXPORT int AleGrpAle::get_full_path(char *fn,String &s) {
  if(handle==-1) return 0;
  s.printf(256,"%s|%s",alepath,fn);
  return 1;
}

DLLEXPORT int AleGrpAle::class_id() {return ALE_ALE;}

DLLEXPORT int AleGrpAle::file_exist(char *fn) {
  for(int i=0;i<num;++i) if(!tab[i].erased)
    if(str_icmp(fn,tab[i].name)==0) return 1;
  return 0;
}

DLLEXPORT int AleGrpAle::dir_exist(char *fn) {
  int l=strlen(fn);
  for(int i=0;i<num;++i) if(!tab[i].erased)
    if(str_nicmp(fn,tab[i].name,l)==0) if(tab[i].name[l]=='\\') return 1;
  return 0;
}

//ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ//
//       AleGrpAleWr
//ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ//

DLLEXPORT AleGrpAleWr::AleGrpAleWr(char *fn) {
  static AleHdr hdr;

  wrh=-1;wrlen=0;
  handle=::open(fn,O_BINARY|O_RDWR);
  if(handle==-1) {
    handle=::open(fn,O_BINARY|O_RDWR|O_CREAT|O_TRUNC,S_IREAD|S_IWRITE);
    if(handle==-1) return;
    _fullpath(alepath,fn,sizeof(alepath));
    memcpy(hdr.id,"ALE\x1A",sizeof(hdr.id));
    tabofs=hdr.tabofs=sizeof(hdr);
    hdr.tabsize=0;
    if(::write(handle,&hdr,sizeof(hdr))!=sizeof(hdr))
      {::close(handle);handle=-1;return;}
    num=0;tab=NULL;
    return;
  }else _fullpath(alepath,fn,sizeof(alepath));
  tab=NULL;num=0;
  if(::read(handle,&hdr,sizeof(hdr))!=sizeof(hdr))
    {::close(handle);handle=-1;return;}
  if(memcmp(hdr.id,"ALE\x1A",sizeof(hdr.id))!=0)
    {::close(handle);handle=-1;return;}
  if(::lseek(handle,hdr.tabofs,SEEK_SET)==-1)
    {::close(handle);handle=-1;return;}
  if(hdr.tabsize) if(!(tab=(AleElem*)malloc(hdr.tabsize)))
    {::close(handle);handle=-1;return;}
  if(hdr.tabsize) if(::read(handle,tab,hdr.tabsize)!=hdr.tabsize)
    {free(tab);::close(handle);handle=-1;return;}
  num=hdr.tabsize/sizeof(AleElem);
  tabofs=hdr.tabofs;
}

DLLEXPORT AleGrpAleWr::~AleGrpAleWr() {
  if(handle!=-1) {
    if(wrh!=-1) close(0);

    ::lseek(handle,tabofs,SEEK_SET);
    static AleHdr hdr;
    hdr.tabsize=num*sizeof(AleElem);
    hdr.tabofs=tabofs;
    memcpy(hdr.id,"ALE\x1A",sizeof(hdr.id));
    ::write(handle,tab,hdr.tabsize);
    ::lseek(handle,0,SEEK_SET);
    ::write(handle,&hdr,sizeof(hdr));

    ::close(handle);handle=-1;
  }
  if(tab) {free(tab);tab=NULL;}
}

DLLEXPORT int AleGrpAleWr::find_fn(char *fn) {
  for(int i=0;i<num;++i) if(!tab[i].erased)
    if(str_icmp(fn,tab[i].name)==0) return i;
  return -1;
}

DLLEXPORT int AleGrpAleWr::open(char *fn,int flg) {
  if(flg & LF_REAL) return -1;
  if(!(flg & (LF_CREATE|LF_WRITE))) {
    int h=AleGrpAle::open(fn,flg);
    if(h==-1) return -1;
    return h+1;
  }
  if(wrh!=-1) return -1;
  int i=find_fn(fn);
  if(i==-1 && !(flg & LF_CREATE)) return -1;
    AleElem *nt=(AleElem*)realloc(tab,(num+1)*sizeof(AleElem));
    if(!nt) return -1;
    memset(nt[num].name,0,sizeof(nt[num].name));
    strcpy(nt[num].name,fn);
    nt[num].erased=0;
    nt[num].ofs=wrofs=tabofs;
    nt[num].len=0;
    wrh=num++;
    wrlen=0;wrcur=0;
    tab=nt;
  if(i!=-1) {
    tab[i].erased=1;
    if(!(flg & LF_CREATE)) {
      #define BUFSZ 0x10000
      void *buf=malloc(BUFSZ);
      if(buf) {
	for(int l=tab[i].len;l>0;) {
	  int n=(l<BUFSZ)?l:BUFSZ;
	  if(::lseek(handle,tab[i].ofs+wrlen,SEEK_SET)==-1) break;
	  if(::read(handle,buf,n)!=n) break;
	  if(::lseek(handle,wrofs+wrlen,SEEK_SET)==-1) break;
	  int w=::write(handle,buf,n);
	  if(w==-1) break;
	  wrlen+=w;
	  if(w!=n) break;
	  l-=n;
	}
	free(buf);
      }
      #undef BUFSZ
    }
  }
  return 0;
}

DLLEXPORT int AleGrpAleWr::close(int h) {
  if(h<0) return -1;
  if(h!=0) return AleGrpAle::close(h-1);
  if(wrh==-1) return -1;
  tab[wrh].len=wrlen;
  if(::lseek(handle,wrofs+wrlen,SEEK_SET)==-1) return -1;
  static AleHdr hdr;
  memcpy(hdr.id,"ALE\x1A",sizeof(hdr.id));
  hdr.tabofs=tabofs=wrofs+wrlen;
  hdr.tabsize=num*sizeof(AleElem);
  if(::write(handle,tab,hdr.tabsize)!=hdr.tabsize) return -1;
  if(::lseek(handle,0,SEEK_SET)==-1) return -1;
  if(::write(handle,&hdr,sizeof(hdr))!=sizeof(hdr)) return -1;
  wrh=-1;
  return 0;
}

DLLEXPORT int AleGrpAleWr::read(int h,void *ptr,int len) {
  if(h<0) return -1;
  if(h!=0) return AleGrpAle::read(h-1,ptr,len);
  if(wrh==-1) return -1;
  int sz;
  if(::lseek(handle,wrofs+wrcur,SEEK_SET)==-1) return -1;
  sz=wrlen-wrcur;
  if(len<sz) sz=len;
  sz=::read(handle,ptr,sz);
  if(sz!=-1) wrcur+=sz;
  return sz;
}

DLLEXPORT int AleGrpAleWr::write(int h,void *ptr,int len) {
  if(h<0) return -1;
  if(h!=0) return AleGrpAle::write(h-1,ptr,len);
  if(wrh==-1) return -1;
  int sz;
  if(::lseek(handle,wrofs+wrcur,SEEK_SET)==-1) return -1;
  sz=::write(handle,ptr,len);
  if(sz!=-1) {wrcur+=sz;if(wrcur>wrlen) wrlen=wrcur;}
  return sz;
}

DLLEXPORT int AleGrpAleWr::seek(int h,int ofs,int how) {
  if(h<0) return -1;
  if(h!=0) return AleGrpAle::seek(h-1,ofs,how);
  if(wrh==-1) return -1;
  int o=::lseek(handle,wrofs+ofs,how);
  if(o!=-1) {
    wrcur=o-wrofs;
    if(wrcur>wrlen) wrlen=wrcur;
  }
  return wrcur;
}

DLLEXPORT int AleGrpAleWr::tell(int h) {
  if(h<0) return -1;
  if(h!=0) return AleGrpAle::tell(h-1);
  if(wrh==-1) return -1;
  return wrcur;
}

DLLEXPORT int AleGrpAleWr::length(int h) {
  if(h<0) return -1;
  if(h!=0) return AleGrpAle::length(h-1);
  if(wrh==-1) return -1;
  return wrlen;
}

DLLEXPORT int AleGrpAleWr::rename(char *on,char *nn,int flg) {
  if(flg & LF_REAL) return 0;
  int i=find_fn(on);
  if(i==-1) return 0;
  strcpy(tab[i].name,nn);
  return 1;
}

DLLEXPORT int AleGrpAleWr::erase(char *fn,int flg) {
  if(flg & LF_REAL) return 0;
  int i=find_fn(fn);
  if(i==-1) return 0;
  if(wrh==i) return 0;
  for(int j=0;j<otab.count();++j) if(otab[j].h==i) return 0;
  tab[i].erased=1;
  return 1;
}

DLLEXPORT int AleGrpAleWr::rmdir(char *s) {
  return !dir_exist(s);
}
