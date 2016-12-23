#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <direct.h>
#include <ale.h>
#include "dll.h"

DLLEXPORT unsigned char uptab[256],lwtab[256];

extern "C" DLLEXPORT char chr_upr(unsigned char c) {
  return uptab[c];
}

extern "C" DLLEXPORT char chr_lwr(unsigned char c) {
  return lwtab[c];
}

extern "C" DLLEXPORT char *str_upr(char *s) {
  unsigned char *p;

  if(!s) return s;
  for(p=s;*p;++p) *p=uptab[*p];
  return s;
}

extern "C" DLLEXPORT char *str_lwr(char *s) {
  unsigned char *p;

  if(!s) return s;
  for(p=s;*p;++p) *p=lwtab[*p];
  return s;
}

extern "C" DLLEXPORT int str_icmp(const char *a,const char *b) {
  int d;

  for(;;++a,++b) {
    d=(int)uptab[(unsigned char)*a]-(int)uptab[(unsigned char)*b];
    if(d) return d;
    if(!*a || !*b) break;
  }
  return d;
}

extern "C" DLLEXPORT int str_nicmp(const char *a,const char *b,int n) {
  int d;

  for(d=0;n>0;++a,++b,--n) {
    d=(int)uptab[(unsigned char)*a]-(int)uptab[(unsigned char)*b];
    if(d) return d;
    if(!*a || !*b) break;
  }
  return d;
}

extern "C" DLLEXPORT int mem_icmp(const char *a,const char *b,int n) {
  int d;

  for(d=0;n>0;++a,++b,--n) {
    d=(int)uptab[(unsigned char)*a]-(int)uptab[(unsigned char)*b];
    if(d) return d;
  }
  return d;
}

extern "C" DLLEXPORT int file_cmp(const char *a,const char *b) {
  for(;;) {
    if(!*a || !*b) break;
    if(*a=='\\' || *a=='.') {
      while(*b=='*' || *b=='?') ++b;
    }else if(*b=='\\' || *b=='.') {
      while(*a=='*' || *a=='?') ++a;
    }
    if(*a!='*' && *a!='?' && *b!='*' && *b!='?')
      if(uptab[(unsigned char)*a]!=uptab[(unsigned char)*b]) return 1;
    if(*a && *a!='*') ++a;
    if(*b && *b!='*') ++b;
  }
  if(*a!='*' && *a!='?' && *b!='*' && *b!='?')
    if(uptab[(unsigned char)*a]!=uptab[(unsigned char)*b]) return 1;
  return 0;
}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//
//       AleGrpReal
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//

struct RealFind{
  DIR *h;
  char subdir;
};

DLLEXPORT int AleGrpReal::find_first(char *mask,char attr,alefind_t *fs) {
  if(!fs) return 0;
  fs->data=NULL;
  if(attr & LA_ERASED) return 0;
  RealFind *rf=new RealFind;
  if(!rf) return 0;
  static char msk[256];
  strcat(strcpy(msk,basepath),mask);
  DIR *h=opendir(msk);
  if(!h) return 0;
  DIR *ff=readdir(h);
  if(!ff) {closedir(h);delete rf;return 0;}
  while((ff->d_attr&_A_SUBDIR) && ff->d_name[0]=='.') {
    ff=readdir(h);
    if(!ff) {closedir(h);delete rf;return 0;}
  }
  if(!(attr&LA_SUBDIR)) {
    while(ff->d_attr&_A_SUBDIR) {
      ff=readdir(h);
      if(!ff) {closedir(h);delete rf;return 0;}
    }
  }
  rf->h=h;rf->subdir=(attr&LA_SUBDIR)?1:0;
  fs->data=rf;
  strcpy(fs->name,ff->d_name);
  fs->attr=(ff->d_attr&_A_SUBDIR)?LA_SUBDIR:0;
  fs->size=ff->d_size;
  return 1;
}

DLLEXPORT int AleGrpReal::find_next(alefind_t *fs) {
  RealFind *rf=(RealFind*)fs->data;
  DIR *ff=readdir(rf->h);
  if(!ff) return 0;
  while((ff->d_attr&_A_SUBDIR) && ff->d_name[0]=='.') {
    ff=readdir(rf->h);
    if(!ff) return 0;
  }
  if(!rf->subdir) {
    while(ff->d_attr&_A_SUBDIR) {
      ff=readdir(rf->h);
      if(!ff) return 0;
    }
  }
  strcpy(fs->name,ff->d_name);
  fs->size=ff->d_size;
  fs->attr=(ff->d_attr&_A_SUBDIR)?LA_SUBDIR:0;
  return 1;
}

DLLEXPORT int AleGrpReal::find_close(alefind_t *fs) {
  if(!fs) return 0;
  if(fs->data) {
    RealFind *rf=(RealFind*)fs->data;
    closedir(rf->h);
    delete rf;
    fs->data=NULL;
  }
  return 1;
}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//
//       AleGrpAle
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//

struct AleFind{
  char mask[64];
  Tab<char[64]> dir;
  char attr,subdir;
  int i,ns;
};

static int ale_find_next(AleElem *tab,int num,alefind_t *fs) {
  static char buf[64];
  AleFind *f=(AleFind*)fs->data;
  if(f->subdir) {
    if(f->i==0) {
      f->ns=0;
      for(char *p=f->mask;*p;++p) if(*p=='\\') ++f->ns;
      for(int i=0;i<num;++i) if(!tab[i].erased) {
	int n=0;
	for(char *s=tab[i].name,*d=buf;*s;++s,++d)
	  if((*d=*s)=='\\') if(++n>f->ns) {*d=0;break;}
	*d=0;
	if(n>f->ns) if(file_cmp(buf,f->mask)==0) {
	  for(int j=0;j<f->dir.count();++j) if(strcmp(buf,f->dir[j])==0) break;
	  if(j>=f->dir.count()) {
	    if(f->dir.append(1,&buf)==-1) return 0;
	  }
	}
      }
    }
    if(f->i>=f->dir.count()) {
      f->subdir=0;f->i=0;
    }else{
      for(int j=strlen(f->dir[f->i])-1;j>=0;--j)
	if(f->dir[f->i][j]=='\\') break;
      strcpy(fs->name,f->dir[f->i]+j+1);
      fs->attr=LA_SUBDIR;
      fs->size=0;
      ++f->i;
      return 1;
    }
  }
  for(int i=f->i;i<num;++i) {
    if(f->attr & LA_ERASED) {
      if(tab[i].erased)
	if(file_cmp(tab[i].name,f->mask)==0) break;
    }else{
      if(!tab[i].erased)
	if(file_cmp(tab[i].name,f->mask)==0) break;
    }
  }
  if(i>=num) {
    f->i=num;
    return 0;
  }
  char *p=tab[i].name;
  for(int j=strlen(p)-1;j>=0;--j) if(p[j]=='\\') break;
  strcpy(fs->name,p+j+1);
  fs->attr=(tab[i].erased)?LA_ERASED:0;
  fs->size=tab[i].len;
  f->i=i+1;
  return 1;
}

static int ale_find_first(AleElem *tab,int num,char *mask,char attr,alefind_t *fs) {
  if(!fs) return 0;
  fs->data=NULL;
  if((attr&(LA_ERASED|LA_SUBDIR))==(LA_ERASED|LA_SUBDIR)) return 0;
  AleFind *f=new AleFind;
  if(!f) return 0;
  strcpy(f->mask,mask);
  f->attr=attr;
  f->i=0;
  f->subdir=(attr&LA_SUBDIR)?1:0;
  fs->data=f;
  if(!ale_find_next(tab,num,fs)) {
    delete f;fs->data=NULL;return 0;
  }
  return 1;
}

static int ale_find_close(alefind_t *fs) {
  if(!fs) return 0;
  if(fs->data) {delete((AleFind*)fs->data);fs->data=NULL;}
  return 1;
}

DLLEXPORT int AleGrpAle::find_first(char *mask,char attr,alefind_t *fs) {
  return ale_find_first(tab,num,mask,attr,fs);
}

DLLEXPORT int AleGrpAle::find_next(alefind_t *fs) {
  return ale_find_next(tab,num,fs);
}

DLLEXPORT int AleGrpAle::find_close(alefind_t *fs) {
  return ale_find_close(fs);
}

//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//
//       AleGrpAleWr
//ÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ//
/*
int AleGrpAleWr::find_first(char *mask,char attr,alefind_t *fs) {
  return ale_find_first(tab,num,mask,attr,fs);
}

int AleGrpAleWr::find_next(alefind_t *fs) {
  return ale_find_next(tab,num,fs);
}

int AleGrpAleWr::find_close(alefind_t *fs) {
  return ale_find_close(fs);
}
*/
