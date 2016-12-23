/*
  ��ࠢ����� ��㪮� � ��몮� ��� DOS4GW
  ����� 1.1
  (C) ����ᥩ ����᪮�, 1996
*/

#pragma library("osound.lib")

#ifdef __cplusplus
extern "C" {
#endif

// ���� ⠩���
#define TIMER_FREQ 1193180L

// 1/f ���� ᥪ㭤� � ������� ⠩��� (�. timer)
// ���ਬ��: for(timer=0;timer<tlength(25);); // ��㧠 �� 1/25 ᥪ㭤�
#define tlength(f) (TIMER_FREQ/f)

// ⨯ �ࠩ���
enum{
  ST_NONE=-1,   // ��� �����
  ST_DAC,       // �����⮢� ���
  ST_DMA,       // DMA-����
  ST_DAC16,     // �����⮢� ��� (16 ���)
  ST_DMA16      // DMA-���� (16 ���)
};

// ��������� �����㬥�� (DMI)
typedef struct{
  unsigned long  len,           // ����� � �����
		 rate,          // ���� � ��.
		 lstart,        // ��砫� ����� � ����� �� ��砫� ������
		 llen;          // ����� ����� � �����
}snd;

// ��⠭����� �ࠩ��� n
// (�. snddrv.h)
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

// �஬����� ��㪠 � ��모 (0-128)
extern short snd_vol,mus_vol;

// ���௮��஢��� ���, �᫨ �� 0 (��������, �� ����⢥���)
extern char s_interp;

// ����, DMA, IRQ
extern unsigned short snd_port,snd_dma,snd_irq;

// ⨯ ����� (�. ���)
extern short snd_type;

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
