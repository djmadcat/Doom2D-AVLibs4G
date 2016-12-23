/*
  Работа с файлами конфигурации
  Версия 1.0
  (C) Алексей Волынсков, 1996

  Файл конфигурации должен быть текстовым файлом следующего формата:

  [название раздела]

  команда1=12345 'комментарий
  команда2=ON
  'комментарий

  [другой раздел]

  команда1=c:\util\
  команда2=off

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

// читает команду n из раздела s файла f
// возвращает не 0, если все о'кей
DLLEXTERN int CFG_read(char *f,char *s,char *n);

DLLEXTERN char cfg_str[];
DLLEXTERN int cfg_val;

#ifdef __cplusplus
}
#endif

#undef DLLEXTERN
