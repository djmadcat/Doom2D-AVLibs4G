/*
  ��ࠢ����� ��㪮� � ��몮� ��� DOS4GW
  ����� 2.0
  (C) ����ᥩ ����᪮�, 1997
*/

#pragma library("osound2.lib")

#ifdef __cplusplus
extern "C" {
#endif

// ����� �ࠩ��஢
enum{
  SDRV_NONE=0,		// �設�...
  SDRV_ADLIB,		// Adlib: 6 ���
  SDRV_COVOX,		// Covox Speech Thing: 8 ���
  			// snd_port = ����� LPT(1-4)
  SDRV_PC1,		// PC Speaker: 1 ���
  SDRV_PC8,		// PC Speaker: �த� ��� 8 ��� (��� 7)
  SDRV_SB,		// Sound Blaster: 8 ���, DMA
  			// snd_port = ������ ���� (���筮 0x220)
  			// snd_iqr = ����� IRQ (���筮 7 ��� 5)
  			// sorry, DMA ���� ⮫쪮 1
  SDRV_SBNODMA,		// Sound Blaster: 8 ���, ��� DMA (���������)
  SDRV_SBPRO,		// Sound Blaster Pro: 8 ��� ��८, DMA
  			// snd_port = ������ ���� (���筮 0x220)
  			// snd_iqr = ����� IRQ (���筮 7 ��� 5)
  			// sorry, DMA ���� ⮫쪮 1
  SDRV_SB16,		// Sound Blaster 16: 8 ��� ��८, DMA
  			// snd_port = ������ ���� (���筮 0x220)
  			// snd_iqr = ����� IRQ (���筮 7 ��� 5)
  			// sorry, DMA ���� ⮫쪮 1
  SDRV__END
};

// ���� �ࠩ��஢
extern void *snd_drv_tab[];

// ���� ⠩���
#define TIMER_FREQ 1193180L

// 1/f ���� ᥪ㭤� � ������� ⠩��� (�. timer)
// ���ਬ��: for(timer=0;timer<tlength(25);); // ��㧠 �� 1/25 ᥪ㭤�
#define tlength(f) (TIMER_FREQ/f)

// ⨯ �ࠩ���
enum{
  ST_NONE=-1,	// ��� �����
  ST_DAC,	// �����⮢� ���
  ST_DMA	// DMA-����
};

// ��������� �����㬥�� (DMI)
typedef struct{
  unsigned long  len,		// ����� � �����
                 rate,		// ���� � ��.
                 lstart,	// ��砫� ����� � ����� �� ��砫� ������
                 llen;		// ����� ����� � �����
}snd;

// ��⠭����� �ࠩ��� ����� n (�. ���)
void set_snd_drv(short n);

// �஢���� ����稥 ��㪮��� �����
// �����頥� 0, �᫨ ����� ���
short S_detect(void);

// ������� �ࠩ���
void S_init(void);

// �몫���� �ࠩ���
void S_done(void);

// ����㧨�� ��� �� 䠩�� f (���� 8-��⮢� ���)
// � ��⠭����� ��� ����� r (��), ��砫� � ����� ����� ls � ll
// (�᫨ ll==0, � ��� �� ���������)
// (�᫨ ll==1, � ��� ��������� ���������)
snd *load_snd(char *f,unsigned r,unsigned ls,unsigned ll);

// ����㧨�� ��� �� 䠩�� f (�ଠ� DMI)
snd *load_dmi(char *f);

// ����㧨�� ���� �� 䠩�� f (DMM)
// �����頥� 0 �� �訡��
short load_dmm(char *f);

// �᢮������� ������, ���������� ��몮�
void free_music(void);

// �ந���� ��� s �� ������ c (1-8), ���� r � �஬���� v (0-255)
// �����頥� ����� ������, �� ���஬ ��ࠥ��� ��� (0 - �� ��ࠥ���)
// �᫨ c==0, � ��� ������� � ���� ᢮����� �����, � �᫨ ⠪��� ���,
// � �� �㤥� �ந��뢠����
// �᫨ c==-1, � ��४஥��� ᠬ� ���� ��� (�᫨ ��� ᢮������� ������)
// r - �� �⭮�⥫쭠� ���� (���筮 1024)
short S_play(snd *s,short c,unsigned r,short v);

// ��⠭����� ��� �� ������ c (1-8)
void S_stop(short c);

// ��⠭����� ����� r � ��㪠 �� ������ c
void S_setrate(short c,unsigned r);

// ��⠭����� �஬����� v (0-255) � ��㪠 �� ������ c
void S_setvolume(short c,int v);

// ����� ����
void S_startmusic(void);

// ��⠭����� ����
void S_stopmusic(void);

// ������� �ࠩ��� ⠩���
// ���筮 ����砥��� �� �맮�� S_init()
void T_init(void);

// �몫���� �ࠩ��� ⠩���
// ���筮 �몫�砥��� �� �맮�� S_done()
void T_done(void);

// �஬����� ��㪠 � ��모 (0-16)
extern short snd_vol,mus_vol;

// ����, DMA, IRQ
extern unsigned short snd_port,snd_dma,snd_irq;

// ⨯ ����� (�. ���)
extern short snd_type;

// 0-����, ���� ��८
extern char snd_stereo;

// �������� �����
extern char *snd_name;

// ���� ᬥ蠭���� ��㪠 � �����
extern unsigned short sfreq;

// ⠡��� �⭮�⥫��� ���� ���
extern unsigned short note_tab[96];

// ⠩���
extern volatile int timer;

#ifdef __cplusplus
}
#endif
