/*
  ���饭��� �ࠢ����� ��㪮� ��� DOS4GW
  ����� 1.1
  (C) ����ᥩ ����᪮�, 1996
*/

#pragma library("fsound.lib")

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
  ST_NONE=-1,	// ��� �����
  ST_DAC,	// �����⮢� ���
  ST_DMA	// DMA-����
};

// ��������� �����㬥�� (DMI)
typedef struct{
  unsigned long  len,	 // ����� � �����
                 rate,	 // ���� � �����
                 lstart, // ��砫� ����� � ����� �� ��砫� ������
                 llen;	 // ����� ����� � �����
}snd;

// ��⠭����� �ࠩ��� n
// (�. snddrv.h)
void set_snd_drv(short n);

// �஢���� ����稥 ��㪮��� �����
// �����頥� 0, �᫨ ����� ���
int S_detect(void);

// ������� �ࠩ���
void S_init(void);

// �몫���� �ࠩ���
void S_done(void);

// �ந���� ��� s
void S_play(snd *s);

// �ந���� ��� s ����� l, ������� c ls ������ ll
void S_playraw(char *s,int l,int ls,int ll);

// ��⠭����� ���
void S_stop(void);

// �ந��뢠���� �� ���?
// �����頥� 0, �᫨ ���
int S_issound(void);

// ����㧨�� ��� �� 䠩�� f (���� 8-��⮢� ���)
// � ��⠭����� ��� ����� r (��), ��砫� � ����� ����� ls � ll
// (�᫨ ll==0, � ��� �� ���������)
// (�᫨ ll==1, � ��� ��������� ���������)
snd *load_snd(char *f,unsigned r,unsigned ls,unsigned ll);

// ����㧨�� ��� �� 䠩�� f (�ଠ� DMI)
snd *load_dmi(char *f);

// ������� �ࠩ��� ⠩���
// ���筮 ����砥��� �� �맮�� S_init()
void T_init(void);

// �몫���� �ࠩ��� ⠩���
// ���筮 �몫�砥��� �� �맮�� S_done()
void T_done(void);

// ����, DMA, IRQ
extern unsigned short snd_port,snd_dma,snd_irq;

// ⨯ ����� (�. ���)
extern short snd_type;

// �������� �����
extern char *snd_name;

// ���� ᬥ蠭���� ��㪠 � �����
extern unsigned short sfreq;

// ⠩���
extern volatile int timer;

#ifdef __cplusplus
}
#endif
