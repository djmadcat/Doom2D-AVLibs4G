/*
  Управление мышью для DOS4GW
  Версия 1.0
  (C) Алексей Волынсков, 1996
*/

#ifndef _DLL
#pragma library("av.lib")
#define DLLEXTERN extern
#else
#ifndef __B_AVLIB
#pragma library("avdll.lib")
#define DLLEXTERN __declspec(dllimport)
#else
#define DLLEXTERN extern
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

// состояния кнопок мыши
#define MB_LEFT 1
#define MB_RIGHT 2
#define MB_MIDDLE 4

// включить драйвер
// возвращает количество кнопок (0, если мыши нет)
// и записывает его в m_buttons
DLLEXTERN short M_init(void);

// получить координату X курсора мыши на экране (0-319)
DLLEXTERN short M_getx(void);

// получить координату Y курсора мыши на экране (0-199)
DLLEXTERN short M_gety(void);

// получить состояние кнопок мыши
DLLEXTERN short M_getb(void);

// получить данные о положении мыши
// возвращает состояние кнопок
// если x!=NULL, записывает в x координату курсора
// если y!=NULL, записывает в y координату курсора
DLLEXTERN short M_getpos(short *x,short *y);

// получить данные о перемещении мыши
// если sx!=NULL, записывает в sx смещение по горизонтали
// если sy!=NULL, записывает в sy смещение по вертикали
DLLEXTERN short M_gets(short *sx,short *sy);

// количество кнопок мыши
DLLEXTERN short m_buttons;

#ifdef __cplusplus
}
#endif

#undef DLLEXTERN
