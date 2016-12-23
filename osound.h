/*
  Управление звуком и музыкой для DOS4GW
  Версия 1.1
  (C) Алексей Волынсков, 1996
*/

#pragma library("osound.lib")

#ifdef __cplusplus
extern "C" {
#endif

// частота таймера
#define TIMER_FREQ 1193180L

// 1/f доля секунды в еденицах таймера (см. timer)
// например: for(timer=0;timer<tlength(25);); // пауза на 1/25 секунды
#define tlength(f) (TIMER_FREQ/f)

// тип драйвера
enum{
  ST_NONE=-1,   // нет карты
  ST_DAC,       // побайтовый ЦАП
  ST_DMA,       // DMA-карта
  ST_DAC16,     // побайтовый ЦАП (16 бит)
  ST_DMA16      // DMA-карта (16 бит)
};

// заголовок инструмента (DMI)
typedef struct{
  unsigned long  len,           // длина в байтах
		 rate,          // частота в Гц.
		 lstart,        // начало повтора в байтах от начала данных
		 llen;          // длина повтора в байтах
}snd;

// установить драйвер n
// (см. snddrv.h)
void set_snd_drv(short n);

// проверить наличие звуковой карты
// возвращает 0, если карты нет
short S_detect(void);

// включить драйвер
void S_init(void);

// выключить драйвер
void S_done(void);

// загрузить звук из файла f (чистый 8-битовый звук)
// и установить его частоту r (Гц), начало и длину повтора ls и ll
// (если ll==0, то звук не повторяется)
// (если ll==1, то звук повторяется полностью)
snd *load_snd(char *f,unsigned r,unsigned ls,unsigned ll);

// загрузить звук из файла f (формат DMI)
snd *load_dmi(char *f);

// загрузить музыку из файла f (DMM)
// возвращает 0 при ошибке
short load_dmm(char *f);

// освобождает память, занимаемую музыкой
void free_music(void);

// проиграть звук s на канале c (1-8), частоте r и громкости v (0-255)
// возвращает номер канала, на котором играется звук (0 - не играется)
// если c==0, то звук попадет в первый свободный канал, а если такого нет,
// то не будет проигрываться
// если c==-1, то перекроется самый старый звук (если нет свободного канала)
// r - это относительная частота (обычно 1024)
short S_play(snd *s,short c,unsigned r,short v);

// остановить звук на канале c (1-8)
void S_stop(short c);

// установить частоту r у звука на канале c
void S_setrate(short c,unsigned r);

// установить громкость v (0-255) у звука на канале c
void S_setvolume(short c,int v);

// начать музыку
void S_startmusic(void);

// остановить музыку
void S_stopmusic(void);

// включить драйвер таймера
// обычно включается при вызове S_init()
void T_init(void);

// выключить драйвер таймера
// обычно выключается при вызове S_done()
void T_done(void);

// громкость звука и музыки (0-128)
extern short snd_vol,mus_vol;

// интерполировать звук, если не 0 (медленее, но качественно)
extern char s_interp;

// порт, DMA, IRQ
extern unsigned short snd_port,snd_dma,snd_irq;

// тип карты (см. выше)
extern short snd_type;

// название карты
extern char *snd_name;

// частота смешанного звука в Герцах
extern unsigned short sfreq;

// таблица относительных частот нот
extern unsigned short note_tab[96];

// таймер
extern volatile int timer;

#ifdef __cplusplus
}
#endif
