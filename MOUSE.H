/*
  ��ࠢ����� ����� ��� DOS4GW
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

// ���ﭨ� ������ ���
#define MB_LEFT 1
#define MB_RIGHT 2
#define MB_MIDDLE 4

// ������� �ࠩ���
// �����頥� ������⢮ ������ (0, �᫨ ��� ���)
// � �����뢠�� ��� � m_buttons
DLLEXTERN short M_init(void);

// ������� ���न���� X ����� ��� �� �࠭� (0-319)
DLLEXTERN short M_getx(void);

// ������� ���न���� Y ����� ��� �� �࠭� (0-199)
DLLEXTERN short M_gety(void);

// ������� ���ﭨ� ������ ���
DLLEXTERN short M_getb(void);

// ������� ����� � ��������� ���
// �����頥� ���ﭨ� ������
// �᫨ x!=NULL, �����뢠�� � x ���न���� �����
// �᫨ y!=NULL, �����뢠�� � y ���न���� �����
DLLEXTERN short M_getpos(short *x,short *y);

// ������� ����� � ��६�饭�� ���
// �᫨ sx!=NULL, �����뢠�� � sx ᬥ饭�� �� ��ਧ��⠫�
// �᫨ sy!=NULL, �����뢠�� � sy ᬥ饭�� �� ���⨪���
DLLEXTERN short M_gets(short *sx,short *sy);

// ������⢮ ������ ���
DLLEXTERN short m_buttons;

#ifdef __cplusplus
}
#endif

#undef DLLEXTERN
