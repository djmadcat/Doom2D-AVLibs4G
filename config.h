/*
  ����� � 䠩���� ���䨣��樨
  ����� 1.0
  (C) ����ᥩ ����᪮�, 1996

  ���� ���䨣��樨 ������ ���� ⥪�⮢� 䠩��� ᫥���饣� �ଠ�:

  [�������� ࠧ����]

  �������1=12345 '�������਩
  �������2=ON
  '�������਩

  [��㣮� ࠧ���]

  �������1=c:\util\
  �������2=off

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

// �⠥� ������� n �� ࠧ���� s 䠩�� f
// �����頥� �� 0, �᫨ �� �'���
DLLEXTERN int CFG_read(char *f,char *s,char *n);

DLLEXTERN char cfg_str[];
DLLEXTERN int cfg_val;

#ifdef __cplusplus
}
#endif

#undef DLLEXTERN
