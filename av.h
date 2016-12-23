/*
  ������⥪� ��� DOS4GW
  ����� 1.0
  (C) ����ᥩ ����᪮�, 1996
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

/* ����������� ⨯� ������:
  0 - 8086/8088
  2 - 80286
  3 - 80386
  4 - 80486
  5 - Pentium
  �����頥� � �����뢠�� � _cpu ⨯ ������ */
DLLEXTERN short what_cpu(void);

// �⠥� ASCIIZ-��ப� s �� 䠩�� h (���ᨬ��쭠� ����� ��ப� m)
DLLEXTERN void readstrz(int h,char *s,int m);

DLLEXTERN short _cpu;

#ifdef __cplusplus
}
#endif

#undef DLLEXTERN
