
#ifndef __SOUND_LIB_H
#define __SOUND_LIB_H

#ifndef __cplusplus
#error sound.h requires C++
#endif

#ifndef _DLL
#pragma library("sound.lib")
#define DLLEXTERN extern
#else
#ifndef __B_SOUNDLIB
#pragma library("sounddll.lib")
#define DLLEXTERN __declspec(dllimport)
#else
#define DLLEXTERN extern
#endif
#endif

#ifndef __NT__
enum SND_DEVICE{
  SND_NONE,SND_SB,SND_PC
};
#endif

enum{
  SNDERR_OK=0,

#ifndef __NT__
// ошибки, возможные только в DOS
  SNDERR_DOSMEM,                  // не хватает памяти в первом 1M
  SNDERR_UNDER_WINDOWS,           // MustDie не даёт ускорить таймер
#endif

#ifdef __NT__
// ошибки, возможные только под DirectX
  SNDERR_DSOUND_CREATE=100,       // ошибка создания объекта DirectSound
  SNDERR_DSOUND_ERROR,            // ошибка DirectSound
  SNDERR_DSOUNDBUF_CREATE,        // не удалось создать DirectSoundBuffer
  SNDERR_DSOUNDBUF_FORMAT,        // формат rate/bits/chan не поддерживается
  SNDERR_THREAD,                  // не удалось создать поток
#endif
};

// устанавливает Sound Blaster, если есть строчка "set BLASTER=..."
// иначе - PC Speaker
DLLEXTERN void SND_auto_detect();

#ifdef __NT__
#ifdef _WINDOWS_
// нужно вызвать эту функцию до первого вызова SND_init()
DLLEXTERN void SND_set_hwnd(HWND mainwnd);
#endif
#endif

// включает драйвер
// snd_device и проч. должно быть установлено (по умолчанию - без звука)
// rate - частота звука, после включения может измениться (обычно немного)
// bits - 8 или 16. Если 0, используется максимальное для звуковой карты
// chan - 1(моно) или 2(стерео). Если 0 - максимальное
DLLEXTERN int SND_init(int& rate,int& bits,int& chan);

// выключает драйвер
DLLEXTERN void SND_close();

// sampler - процедура, синтезирующая звук. Должна сохранять все регистры,
//   кроме EAX, EBX и флагов. При вызове DS и ES уже нормально установлены.
//   Возвращает знаковые (signed) значения в диапазоне от -32768 до 32767,
//   EAX - левый канал, EBX - правый. В режиме mono используется только EAX.
//   Должна работать как можно быстрее - вызывается при обработке прерывания.
DLLEXTERN void SND_set_sampler(void *sampler);

extern "C" DLLEXTERN void snd_silent_sampler();

#ifndef __NT__
DLLEXTERN SND_DEVICE snd_device;
DLLEXTERN int snd_sb_port,snd_sb_dma,snd_sb_irq,snd_sb_hdma,snd_sb_type;
#endif

#undef DLLEXTERN

#endif
