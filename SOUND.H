
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
// �訡��, �������� ⮫쪮 � DOS
  SNDERR_DOSMEM,                  // �� 墠⠥� ����� � ��ࢮ� 1M
  SNDERR_UNDER_WINDOWS,           // MustDie �� ���� �᪮��� ⠩���
#endif

#ifdef __NT__
// �訡��, �������� ⮫쪮 ��� DirectX
  SNDERR_DSOUND_CREATE=100,       // �訡�� ᮧ����� ��ꥪ� DirectSound
  SNDERR_DSOUND_ERROR,            // �訡�� DirectSound
  SNDERR_DSOUNDBUF_CREATE,        // �� 㤠���� ᮧ���� DirectSoundBuffer
  SNDERR_DSOUNDBUF_FORMAT,        // �ଠ� rate/bits/chan �� �����ন������
  SNDERR_THREAD,                  // �� 㤠���� ᮧ���� ��⮪
#endif
};

// ��⠭�������� Sound Blaster, �᫨ ���� ���窠 "set BLASTER=..."
// ���� - PC Speaker
DLLEXTERN void SND_auto_detect();

#ifdef __NT__
#ifdef _WINDOWS_
// �㦭� �맢��� ��� �㭪�� �� ��ࢮ�� �맮�� SND_init()
DLLEXTERN void SND_set_hwnd(HWND mainwnd);
#endif
#endif

// ����砥� �ࠩ���
// snd_device � ���. ������ ���� ��⠭������ (�� 㬮�砭�� - ��� ��㪠)
// rate - ���� ��㪠, ��᫥ ����祭�� ����� ���������� (���筮 �������)
// bits - 8 ��� 16. �᫨ 0, �ᯮ������ ���ᨬ��쭮� ��� ��㪮��� �����
// chan - 1(����) ��� 2(��८). �᫨ 0 - ���ᨬ��쭮�
DLLEXTERN int SND_init(int& rate,int& bits,int& chan);

// �몫�砥� �ࠩ���
DLLEXTERN void SND_close();

// sampler - ��楤��, ᨭ⥧������ ���. ������ ��࠭��� �� ॣ�����,
//   �஬� EAX, EBX � 䫠���. �� �맮�� DS � ES 㦥 ��ଠ�쭮 ��⠭������.
//   �����頥� ������� (signed) ���祭�� � ��������� �� -32768 �� 32767,
//   EAX - ���� �����, EBX - �ࠢ�. � ०��� mono �ᯮ������ ⮫쪮 EAX.
//   ������ ࠡ���� ��� ����� ����॥ - ��뢠���� �� ��ࠡ�⪥ ���뢠���.
DLLEXTERN void SND_set_sampler(void *sampler);

extern "C" DLLEXTERN void snd_silent_sampler();

#ifndef __NT__
DLLEXTERN SND_DEVICE snd_device;
DLLEXTERN int snd_sb_port,snd_sb_dma,snd_sb_irq,snd_sb_hdma,snd_sb_type;
#endif

#undef DLLEXTERN

#endif
