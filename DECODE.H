#ifndef __SMBDECODE_H_
#define __SMBDECODE_H_

#pragma library("vid.lib")

#include <smbfrmt.h>

class SMBFLIK{
protected:
public:
  int spr;
  int _cyclic;
  image *img;
  SMBFLIK(){img=NULL;_cyclic=1;spr=0;};
  virtual ~SMBFLIK(){if (img) free(img);img=NULL;};
  virtual int open(char *fl)=0;
  virtual image *get_img(){return img;};
  virtual int __next__()=0;
  virtual int next()=0;
  virtual int start()=0;
  virtual int getwdth()=0;
  virtual int gethgth()=0;
  virtual int getcur_frm()=0;
  virtual int getfrms()=0;
  virtual void get(int &w,int &h,int &frm)=0;
  virtual int close(){if (img) free(img);img=NULL;return 1;}
};

extern int _flik_max_size;

//flags for open_flik
    #define SMBF_ONCOLOR_KEY 0x0100
    #define SMBF_ALWAYS_LOAD 0x0001
    #define SMBF_LOAD_ON_SIZ 0x0002
    #define SMBF_CYCLIC_SHOW 0x0010


SMBFLIK *open_flik(char *fl,int flgs=SMBF_CYCLIC_SHOW|SMBF_LOAD_ON_SIZ,int chkerr=1);

#endif