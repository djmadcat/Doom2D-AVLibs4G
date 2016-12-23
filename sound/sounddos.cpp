#include <stdlib.h>
#include <string.h>
#include <sound.h>

static char active=0;

extern "C" {

void sblaster_init(int port,int dma,int irq,int card,int stype);
void sblaster_shutdown(void);
int  sblaster_startdma();
int  sblaster_setrate(int rate);

int  __timer_init();
void __timer_close();
void __timer_set_timeproc(int r,void *p);

void pcspeaker_init();
void pcspeaker_close();
void pcspeaker_timeproc();

}

extern "C" void *__snd_sampler=snd_silent_sampler;

int snd_sb_port=0x220,snd_sb_dma=1,snd_sb_irq=7,snd_sb_hdma=5,snd_sb_type=0;

SND_DEVICE snd_device=SND_NONE;

int iswinrunning();
#pragma aux iswinrunning = \
  "mov eax,0x1600" \
  "int 0x2F" \
  "movzx eax,al" \
  value [eax]

static int iswin() {
  int w=iswinrunning();
  if(w!=0 && w!=0x80) return 1;
  return 0;
}

void SND_auto_detect(void) {
  snd_sb_port=0x220;snd_sb_dma=1;snd_sb_irq=7;snd_sb_hdma=5;snd_sb_type=0;
  snd_device=SND_NONE;
  char *p=getenv("BLASTER");
  if(p) {
    snd_device=SND_SB;
    for(;*p;++p) switch(*p) {
      case 'A': case 'a':
	snd_sb_port=strtoul(p+1,&p,16);--p;
	break;
      case 'D': case 'd':
	snd_sb_dma=strtoul(p+1,&p,10);--p;
	break;
      case 'I': case 'i':
	snd_sb_irq=strtoul(p+1,&p,10);--p;
	break;
      case 'H': case 'h':
	snd_sb_hdma=strtoul(p+1,&p,10);--p;
	break;
      case 'T': case 't':
	int t=strtoul(p+1,&p,10);--p;
	if(t>=6) {snd_sb_type=2;}
	else if(t>=4) {snd_sb_type=1;}
	else {snd_sb_type=0;}
	break;
    }
  }else if(!iswin()) snd_device=SND_PC;
}

static void adjust_timer_rate(int &r) {
  if(r<1000) r=1000;
  else if(r>50000) r=50000;
}

int SND_init(int& rate,int& bits,int& chan) {
  SND_close();
  switch(snd_device) {
    case SND_NONE: break;
    case SND_SB:
      if(!bits) bits=(snd_sb_type==2)?16:8;
      if(!chan) chan=(snd_sb_type==0)?1:2;
      if(chan==2 && snd_sb_type==0) chan=1;
      if(bits==16 && snd_sb_type<2) bits=8;
      sblaster_init(snd_sb_port,
	(snd_sb_type==2 && bits==16)?snd_sb_hdma:snd_sb_dma,snd_sb_irq,
	snd_sb_type,((bits==16)?1:0)|((chan==2)?2:0));
      rate=sblaster_setrate(rate);
      if(!sblaster_startdma()) {sblaster_shutdown();return SNDERR_DOSMEM;}
      break;
    case SND_PC:
      if(iswin()) return SNDERR_UNDER_WINDOWS;
      bits=8;chan=1;
      adjust_timer_rate(rate);
      if(!__timer_init()) return SNDERR_DOSMEM;
      pcspeaker_init();
      __timer_set_timeproc(rate,pcspeaker_timeproc);
      break;
  }
  active=1;
  return 0;
}

void SND_close(void) {
  if(!active) return;
  switch(snd_device) {
    case SND_SB:
      sblaster_shutdown();
      break;
    case SND_PC:
      __timer_close();
      pcspeaker_close();
      break;
  }
  __snd_sampler=snd_silent_sampler;
  active=0;
}

void SND_set_sampler(void *sampler) {
  __snd_sampler=sampler?sampler:snd_silent_sampler;
}
