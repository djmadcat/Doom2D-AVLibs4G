/*
  ������⥪� ��� DOS4GW
  ����� 1.0
  ��ࠡ�⪠ �訡��
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

#ifndef __cplusplus

// ��ࠡ�⪠ �訡�� � ���� z, ⨯� t, ����� n
// ��।������ ���짮��⥫��
void error(short z,short t,short n,char *s1,char *s2);

#endif

// ���� �訡�� EZ_*
enum{
  EZ_SYSTEM,            // � ��⥬�
  EZ_LIB,               // � ������⥪�
  EZ_VGALIB,            // � ������⥪� VGA
  EZ_LOADVGA,           // �� ����㧪� ����ࠦ���� s1 �� 䠩�� s2
  EZ_LOADANIM,          // �� ����㧪� �����樨 �� 䠩�� s1
  EZ_LOADSND,           // �� ����㧪� ��㪠 �� 䠩�� s1
  EZ_USER
};

// ⨯ �訡�� ET_*
enum{
  ET_STD,               // �⠭���⭠� �訡�� (�. strerror, errno.h)
  ET_AVLIB,             // �訡�� ������⥪� AV.LIB
  ET_USER
};

// ����� �訡�� ������⥪� AV.LIB
enum{
  EN_NONE,              // ��� �訡��
  EN_BADFORMAT,         // ���ࠢ���� �ଠ� ������
  EN_RESNOTFOUND,       // ����� �� ������
  EN_HARDERR,           // �訡�� ���������
  EN_LAST
};

// �������� ��� �訡��
DLLEXTERN char *av_ez_msg[];

// ᮮ�饭�� �� �訡��� AV.LIB
DLLEXTERN char *av_err_msg[];

#ifdef __cplusplus
}
#endif
