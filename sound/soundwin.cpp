#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sound.h>
#include "dll.h"

DLLEXPORT void SND_auto_detect() {
}

typedef void storesmp_f(void*,int,void*);

extern "C" {
void store_samples_8_1(void*,int,void*);
void store_samples_8_2(void*,int,void*);
void store_samples_16_1(void*,int,void*);
void store_samples_16_2(void*,int,void*);
}

static storesmp_f *store_samples=NULL;

static LPDIRECTSOUND lpds=NULL;
static LPDIRECTSOUNDBUFFER dsbuf=NULL;
static HANDLE thrd=NULL;
static void *samplerproc=snd_silent_sampler;

static int _size,_bsz;
static char fillbyte;

#define FAC 64

static DWORD __stdcall snd_thread(void *param) {
  if(!dsbuf) return 0;
  int bsz2=_bsz/8*4;
  DWORD pofs,wofs;
//  dsbuf->GetCurrentPosition(&pofs,&wofs);
  dsbuf->SetCurrentPosition(0);
  LPVOID ptr1,ptr2;
  DWORD len1,len2;
  if(dsbuf->Lock(0,_bsz,&ptr1,&len1,&ptr2,&len2,0)==DS_OK) {
    memset(ptr1,fillbyte,len1);
    if(ptr2) memset(ptr2,fillbyte,len2);
    dsbuf->Unlock(ptr1,len1,ptr2,len2);
  }
  dsbuf->Play(0,0,DSBPLAY_LOOPING);
  int ofs=0;
  int wo=ofs+_size; if(wo>=_bsz) wo-=_bsz;
  for(;;) {
    int o1b=ofs-bsz2,o1e,o2e=0;
    if(o1b<0) o1b+=_bsz;
    o1e=o1b+bsz2;
    if(o1e>_bsz) {
      o2e=o1e-_bsz;
      o1e=_bsz;
    }
    for(;;) {
      dsbuf->GetCurrentPosition(&pofs,&wofs);
      if(pofs>=o1b && pofs<o1e) {Sleep(500/FAC);continue;}
      if(pofs>=o2e) break;
      Sleep(500/FAC);
    }
    HRESULT hr=dsbuf->Lock(wo,_size,&ptr1,&len1,&ptr2,&len2,0);
    if(hr==DSERR_BUFFERLOST) {
      dsbuf->Restore();continue;
    }
    wo+=_size;if(wo>=_bsz) wo-=_bsz;
    ofs+=_size;if(ofs>=_bsz) ofs-=_bsz;
    store_samples(ptr1,len1,samplerproc);
    if(ptr2) store_samples(ptr2,len2,samplerproc);
    dsbuf->Unlock(ptr1,len1,ptr2,len2);
    DWORD flg=0;
    dsbuf->GetStatus(&flg);
    if(!(flg&DSBSTATUS_PLAYING)) dsbuf->Play(0,0,DSBPLAY_LOOPING);
  }
}

static HWND mainwnd=NULL;

DLLEXPORT void SND_set_hwnd(HWND h) {
  mainwnd=h;
}

DLLEXPORT int SND_init(int& rate,int& bits,int& chan) {
  SND_close();

  HRESULT hr=DirectSoundCreate(NULL,&lpds,NULL);
  if(hr!=DS_OK) return SNDERR_DSOUND_CREATE;

  DSBUFFERDESC dsbdesc;
  DSBCAPS dsbcaps;
  WAVEFORMATEX pcmwf;

  memset(&pcmwf,0,sizeof(PCMWAVEFORMAT));
  pcmwf.wFormatTag=WAVE_FORMAT_PCM;
  pcmwf.nChannels=chan;
  pcmwf.nSamplesPerSec=rate;
  pcmwf.nBlockAlign=(bits==16?2:1)*chan;
  pcmwf.nAvgBytesPerSec=pcmwf.nBlockAlign*rate;
  pcmwf.wBitsPerSample=bits;
  _size=pcmwf.nBlockAlign*(rate/FAC);
  if(bits==16) {
    store_samples=(chan==2)?store_samples_16_2:store_samples_16_1;
  }else{
    store_samples=(chan==2)?store_samples_8_2:store_samples_8_1;
  }
  fillbyte=(bits==16)?0:0x80;

  memset(&dsbdesc,0,sizeof(DSBUFFERDESC));
  dsbdesc.dwSize=sizeof(DSBUFFERDESC);
  dsbdesc.dwFlags=DSBCAPS_PRIMARYBUFFER|DSBCAPS_STICKYFOCUS;
  dsbdesc.dwBufferBytes=0;
  dsbdesc.lpwfxFormat=NULL;

  hr=lpds->SetCooperativeLevel(mainwnd,DSSCL_WRITEPRIMARY);
  if(hr!=DS_OK)
    {SND_close();return SNDERR_DSOUND_ERROR;}
  hr=lpds->CreateSoundBuffer(&dsbdesc,&dsbuf,NULL);
  if(hr!=DS_OK)
    {SND_close();return SNDERR_DSOUNDBUF_CREATE;}
  hr=dsbuf->SetFormat(&pcmwf);
  if(hr!=DS_OK) {
    SND_close();
    return SNDERR_DSOUNDBUF_FORMAT;
  }
  dsbcaps.dwSize=sizeof(dsbcaps);
  dsbuf->GetCaps(&dsbcaps);
  _bsz=dsbcaps.dwBufferBytes;

  DWORD dw;
  thrd=CreateThread(NULL,1024,snd_thread,NULL,0,&dw);
  if(!thrd)
    {SND_close();return SNDERR_THREAD;}
  SetThreadPriority(thrd,THREAD_PRIORITY_TIME_CRITICAL);

  return 0;
}

DLLEXPORT void SND_close(void) {
  if(thrd) {TerminateThread(thrd,0);CloseHandle(thrd);thrd=NULL;}
  if(lpds) {lpds->Release();lpds=NULL;dsbuf=NULL;}
  samplerproc=snd_silent_sampler;
}

DLLEXPORT void SND_set_sampler(void *sampler) {
  samplerproc=sampler?sampler:snd_silent_sampler;
}
