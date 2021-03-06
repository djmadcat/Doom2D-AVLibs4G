/*
  ��㪮�� �ࠩ��� ��� DOS4GW
  ����� 1.0
  (C) ����ᥩ ����᪮�, 1996
*/

#pragma library("osnddrv.lib")

#ifdef __cplusplus
extern "C" {
#endif

// ����� �ࠩ��஢
enum{
  SDRV_NONE=0,          // �設�...
  SDRV_ADLIB,           // Adlib: 6 ���
  SDRV_COVOX,           // Covox Speech Thing: 8 ���
			// snd_port = ����� LPT(1-4)
  SDRV_PC1,             // PC Speaker: 1 ���
  SDRV_PC8,             // PC Speaker: �த� ��� 8 ��� (��� 7)
  SDRV_SB,              // Sound Blaster: 8 ���, DMA
			// snd_port = ������ ���� (���筮 0x220)
			// snd_iqr = ����� IRQ (���筮 7 ��� 5)
			// sorry, DMA ���� ⮫쪮 1
//  SDRV_SBNODMA,               // Sound Blaster: 8 ���, ��� DMA (���������)
  SDRV_SB16,            // Sound Blaster 16/AWE32: 16 ���, DMA
  SDRV__END
};

// ���� �ࠩ��஢
extern void *snd_drv_tab[];

#ifdef __cplusplus
}
#endif
