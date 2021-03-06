/*
  �ࠩ��� ���������� V1.1 ��� DOS4GW (� ⠪�� DirectX 3)
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

// ⨯ �㭪樨 ��ࠡ�⪨ ������
typedef void key_f(int k,int pressed);

#ifndef __NT__
// ������� �ࠩ���
DLLEXTERN void K_init(void);
#else
#ifdef _WINDOWS_
DLLEXTERN int K_init(HINSTANCE,HWND);
#endif
DLLEXTERN void K_acquire();
DLLEXTERN void K_unacquire();
#endif

// �몫���� �ࠩ���
DLLEXTERN void K_done(void);

// ��⠭����� �㭪�� ��ࠡ�⪨ ������
DLLEXTERN void K_setkeyproc(key_f *);

DLLEXTERN key_f *K_getkeyproc();

// �⠭���⭠� �㭪�� ��ࠡ�⪨ ������
DLLEXTERN key_f def_key_proc;

// ���ᨢ ������: 0 - ���饭�, ���� - �����
DLLEXTERN char volatile keys[256];

#ifdef __NT__
#ifdef __DINPUT_INCLUDED__
DLLEXTERN LPDIRECTINPUT DInput;
#endif
#endif

#ifdef __cplusplus
}
#endif

#undef DLLEXTERN
