#include <stdio.h>
#include <stdarg.h>
#include <malloc.h>
#include <string.h>
#include <svga.h>
#include <str.h>
#include <decode.h>

typedef unsigned char uchar;

class SMBFLIKBase:public SMBFLIK{
protected:
  ucolor cl_ar[32768];
  int cl_ar_ready;
  char *nxt;
  FILE *fil;
  header hdr;
  int cr_fr;
  String name;
public:
  SMBFLIKBase():SMBFLIK(){nxt=NULL;fil=NULL;cr_fr=0;cl_ar_ready=0;};
  virtual ~SMBFLIKBase(){
    if (fil) fclose(fil);fil=NULL;
    if (nxt) free(nxt);nxt=NULL;
  };
  virtual int close() {
    if (fil) fclose(fil);fil=NULL;
    if (nxt) free(nxt);nxt=NULL;
    cr_fr=0;cl_ar_ready=0;
    name="";
    return SMBFLIK::close();
  }
  virtual int open(char *fl) {
    int i;
    color *p0;
    image *im=NULL;

    cr_fr=0;
    if (img) free(img);img=NULL;
    if (!fil) {fil=fopen(fl,"rb");name=fl;}else fseek(fil,0,SEEK_SET);
    if(!fil) goto io_error;
    if (fread(&hdr,sizeof(header), 1, fil) < 1) goto io_error;
    if (hdr.ver!=0 || hdr.io!=__BASE_IO) {fclose(fil);fil=NULL;return 0;}
    im=(image*)malloc(hdr.w*hdr.h*sizeof(color)+sizeof(image));
    p0=(color *)(im+1);
    im->w=hdr.w;  im->h=hdr.h;im->sx=im->sy=0;
    if (!im) goto io_error;
    if (fread(im+1,hdr.w*hdr.h*sizeof(color), 1, fil) < 1) goto io_error;
    img=(image*)malloc(hdr.w*hdr.h*(sv_bytespp+((spr?1:0)*(sv_bits==32?0:1)))+sizeof(image));
    *img=*im;
    if (!img) goto io_error;
    if (!cl_ar_ready)for (i=0;i<32768;++i) cl_ar[i]=SV_rgb8((i&0x7c00)>>7,(i&0x3e0)>>2,(i&0x1f)<<3);
    cl_ar_ready=1;
    switch (sv_bits) {
      case 32:{
	color32 *p=(color32 *)(img+1);
	for (i=0;i<hdr.w*hdr.h;++i,p++,p0++) {
	  p->uc=cl_ar[*p0];
	  if (spr) if (!p->uc) p->a=255; else p->a=0;
	}
      }break;
      case 16:{
	color16 *p=(color16 *)(img+1);
	uchar *a=(uchar*)(p+hdr.w*hdr.h);
	for (i=0;i<hdr.w*hdr.h;++i,p++,p0++,a++) {
	  *p=(*p0&0x1f)|((*p0&0x7fe0)<<1);
	  if (spr) {if (!*p) *a=255; else *a=0;}
	}
      }break;
      case 15:{
	color16 *p=(color16 *)(img+1);
	memcpy(p,p0,hdr.w*hdr.h*sizeof(color16));
	if (spr) {
	  uchar *a=(uchar*)(p+hdr.w*hdr.h);
	  for (i=0;i<hdr.w*hdr.h;++i,a++,p++) {
	    if (!*p) *a=255; else *a=0;
	  }
	}
      }break;
    };
    /*
      24 ¨ 8
    */
    cr_fr=0;free(im);
    return 1;
  io_error:;
    if (img) free(img);img=NULL;
    if (im) free(im);im=NULL;
    if(fil) fclose(fil);
    fil=NULL;
    return 0;
  };
  virtual int next(){
    if (!__next__()) return 0;
    if (!cr_fr && !nxt) return 1;
    if (cr_fr==hdr.frms-1 && !_cyclic) return 1;
    if(!nxt) return 0;
    char *nx=nxt;
    int &ql=*((int*)nx);nx+=sizeof(int);
    sq_format__ *rr=(sq_format__ *)nx;nx+=ql*ql*4*sizeof(sq_format__);
    if (!ql) goto io_error;
    {
      sq_format__ *rsl=rr;
      for (int i=0;i<ql*ql;++i,rsl+=4) {
	sq_format__ l=*rsl,t=*(rsl+1),r=*(rsl+2),b=*(rsl+3);
	if (l>r || t>b) continue;
	int w=r-l+1;
	switch (sv_bits) {
	  case 32:{
	    color32 *p=(color32*)(img+1)+t*hdr.w+l;
	    color *p0=(color *)nx;
	    for (int j=t;j<=b;++j,p+=hdr.w-w) {
	      for (int x=0;x<w;++x,p++,p0++) {
		p->uc=cl_ar[*p0];
		if (spr) if (!p->uc) p->a=255; else p->a=0;
	      }
	    }
	    nx+=w*(b-t+1)*sizeof(color);
	  }break;
	  case 16:{
	    color16 *p=(color16*)(img+1)+t*hdr.w+l;
	    color *p0=(color *)nx;
	    uchar *a=(uchar*)(((color16*)(img+1)+hdr.w*hdr.h))+t*hdr.w+l;
	    for (int j=t;j<=b;++j,p+=hdr.w-w,a+=hdr.w-w)
	      for (int x=0;x<w;++x,p++,p0++,++a) {
		*p=(*p0&0x1f)|((*p0&0x7fe0)<<1);
		if (spr) {if (!*p) *a=255; else  *a=0;}
	      }
	    nx+=w*(b-t+1)*sizeof(color);
	  }break;
	  case 15:{
	    color16 *p=(color16*)(img+1)+t*hdr.w+l;
	    color *p0=(color *)nx;
	    for (int j=t;j<=b;++j,p0+=w,p+=hdr.w) {
	      memcpy(p,p0,w*sizeof(color16));
	      if (spr) {
		uchar *a=(uchar*)(p+hdr.w*hdr.h);
		color16 *pp=p;
		for (i=0;i<hdr.w*hdr.h;++i,a++,pp++) {
		  if (!*pp) *a=255; else *a=0;
		}
	      }
	    }
	    nx+=w*(b-t+1)*sizeof(color);
	  }break;
	};
      }
      cr_fr++;
    }
    return 1;
  io_error:;

    return 0;
  };
  virtual int getwdth(){return hdr.w;};
  virtual int gethgth(){return hdr.h;};
  virtual int getfrms(){return hdr.frms;};
  virtual int getcur_frm(){return cr_fr;};
  virtual void get(int &w,int &h,int &frm){w=hdr.w;h=hdr.h;frm=hdr.frms;};
};

class SMBFLIKShow:public SMBFLIKBase{
protected:
public:
  SMBFLIKShow():SMBFLIKBase(){};
  virtual ~SMBFLIKShow(){};
  virtual int __next__(){
    if(!fil) return 0;
    if (cr_fr>=hdr.frms-1) if (_cyclic) return start(); else return 1;
    {
      int help_length=ftell(fil);
      fseek(fil,0,SEEK_END);
      int file_length=ftell(fil);
      fseek(fil,help_length,SEEK_SET);
      if (ftell(fil)==file_length) if (_cyclic) return start(); else return 1;
    }
    if (nxt) free(nxt);nxt=NULL;
    int sz=0,skp=0;
    if (fread(&sz,sizeof(sz), 1, fil) < 1) goto io_error;
    if (fread(&skp,sizeof(skp), 1, fil) < 1) goto io_error;
    if(!sz) return 0;
    if (!(nxt=(char*)malloc(sz))) goto io_error;
    if (fread(nxt,sz, 1, fil) < 1) goto io_error;
    fseek(fil,skp-sz,SEEK_CUR);
    return 1;
  io_error:;
    return 0;
  };
  virtual int start(){
    if (nxt) free(nxt);nxt=NULL;
    return open(name);
  };
};

class SMBFLIKLoad:public SMBFLIKBase{
protected:
  char * all,*__nxt;
  image *frst;
  int lgth;
  int nxt_stp;
public:
  SMBFLIKLoad():SMBFLIKBase(){all=NULL;lgth=0;__nxt=NULL;frst=NULL;};
  virtual ~SMBFLIKLoad(){if (all) free(all);all=NULL;if (frst) free(frst);frst=NULL;};
  virtual int __next__(){
    if (!all) if (!load()) return 0;
    if (cr_fr>=hdr.frms-1) if (_cyclic) return start(); else return 1;
    if (__nxt==all+lgth) if (_cyclic) return start(); else return 1;
    if (!__nxt) __nxt=all;
    int skp=*(((int*)__nxt)+1);
    nxt=__nxt+2*sizeof(int);
    __nxt+=skp+2*sizeof(int);
    return 1;
  };
  virtual int load(){
    if(!fil) return 0;
    if (img) {
      frst=(image*)malloc(hdr.w*hdr.h*(sv_bytespp+((spr?1:0)*(sv_bits==32?0:1)))+sizeof(image));
      if (!frst) goto io_error;
      memcpy(frst,img,hdr.w*hdr.h*(sv_bytespp+((spr?1:0)*(sv_bits==32?0:1)))+sizeof(image));
    }
    {
      int help_length=ftell(fil);
      fseek(fil,0,SEEK_END);
      int file_length=ftell(fil);
      fseek(fil,help_length,SEEK_SET);
      lgth=file_length-help_length;
    }
    if (all) free(all);
    all=(char*)malloc(lgth);
    if (!all) goto io_error;
    if (fread(all,lgth, 1, fil) < 1) goto io_error;
    fclose(fil);fil=NULL;
    __nxt=all;
//    nxt=__nxt+2*sizeof(int);
    nxt=NULL;
    return 1;
  io_error:;
    return 0;
  }
  virtual int start(){
    if (!all) if (!load()) return 0;
    if (!frst || !img) return 0;
    cr_fr=0;__nxt=all;nxt=NULL;
    memcpy(img+1,frst+1,hdr.w*hdr.h*sv_bytespp+(spr?hdr.w*hdr.h:0)*(sv_bits==32?0:1));
    return 1;
  };
};

int _flik_max_size=0x100000;

void fatal(char *,...);

SMBFLIK *open_flik(char *fl,int flgs,int chkerr) {
  if (!fl) {
    if (chkerr) fatal("Can't load");
    return NULL;
  }
  int lm=0;
  if (flgs&SMBF_ALWAYS_LOAD) {
    lm=1;
  } else if (flgs&SMBF_LOAD_ON_SIZ){
    FILE *h=fopen(fl,"rb");
    if (h) {
      fseek(h,0,SEEK_END);
      int sz=ftell(h);
      if (sz>=_flik_max_size) lm=0; else lm=1;
      fclose(h);
    }
  } else {
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
  }
  int cl=0;
  if (flgs&SMBF_CYCLIC_SHOW) cl=1;
  SMBFLIK *sm=NULL;
  if (lm) {
    sm=new SMBFLIKLoad();
    if (!sm) {
      if (chkerr) fatal("No memory to make player for flic: %s",fl);
      return NULL;
    }
    sm->spr=flgs&SMBF_ONCOLOR_KEY;
    int c=sm->open(fl);
    if (!c) {
      if (chkerr) fatal("Can't open file: %s",fl);
      delete sm;
      return NULL;
    }
  } else {
    sm=new SMBFLIKShow();
    if (!sm) {
      if (chkerr) fatal("No memory to make player for flic: %s",fl);
      return NULL;
    }
    sm->spr=flgs&SMBF_ONCOLOR_KEY;
    int c=sm->open(fl);
    if (!c) {
      if (chkerr) fatal("Can't open file: %s",fl);
      delete sm;
      return NULL;
    }
  }
  sm->_cyclic=cl;
  return sm;
}