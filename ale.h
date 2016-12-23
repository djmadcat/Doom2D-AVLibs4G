
#ifndef __ALE_LIB_H
#define __ALE_LIB_H

#include <str.h>

#ifndef _DLL
#pragma library("ale.lib")
#define DLLEXTERN extern
#else
#ifndef __B_ALELIB
#pragma library("aledll.lib")
#define DLLEXTERN __declspec(dllimport)
#else
#define DLLEXTERN extern
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ���樠�������� ⠡���� ��� ࠡ��� � ᨬ������ ⥪�饩 ������� ��࠭��� DOS
DLLEXTERN void init_local_conv(void);

// �८�ࠧ����� �� �㪢� � ��������
DLLEXTERN char *str_upr(char *s);

// �८�ࠧ����� �� �㪢� � �����
DLLEXTERN char *str_lwr(char *s);

DLLEXTERN char chr_upr(unsigned char c);

DLLEXTERN char chr_lwr(unsigned char c);

// �ࠢ���� ��ப� ��� ��� ���������/������ �㪢
// ������ stricmp()
DLLEXTERN int str_icmp(const char *a,const char *b);

// �ࠢ���� �� ����� n ����� ᨬ����� ��ப ��� ��� ���������/������ �㪢
// ������ strnicmp()
DLLEXTERN int str_nicmp(const char *a,const char *b,int n);

// �ࠢ���� n ᨬ����� ��� ��� ���������/������ �㪢
// ������ memicmp()
DLLEXTERN int mem_icmp(const char *a,const char *b,int n);

// �ࠢ���� ����� 䠩��� � ��⮬ ᨬ����� * � ?
// �����頥� 0, �᫨ ����� ᮢ������, � 1, �᫨ ���
DLLEXTERN int file_cmp(const char *a,const char *b);

enum{ LA_SUBDIR=0x10, LA_ERASED=0x80 };

// ������� ��� ���᪠ 䠩���
typedef struct{
  int grp;
  void *data;
  char fmask[128];
  int fattr;
  int size;             // ࠧ��� 䠩��
  char name[13];        // ��� 䠩��
  char attr;            // ��ਡ��� 䠩��
}alefind_t;

typedef void *LFILE;

// LF_CREATE - ᮧ���� 䠩� (��ࠥ� ����)
// LF_ALE    - �믮����� ������ � ALE-䠩��
// LF_REAL   - �믮����� ������ � ॠ��묨 䠩����
enum{ LF_READ=1, LF_WRITE=2, LF_RDWR=3, LF_CREATE=4, LF_ALE=8, LF_REAL=16 };
enum{ LF_SEEK_SET, LF_SEEK_CUR, LF_SEEK_END };

// ���樠�������� ��⥬� (��뢠�� init_local_conv())
DLLEXTERN void init_ale(void);

// ������� ��⥬� (��࠭��� ��������� � ALE-䠩���)
DLLEXTERN void shutdown_ale(void);

// �������� ��㯯� ॠ���� 䠩��� �� ��⠫��� path
// �����頥� ����� ��㯯� ��� -1 �� �訡��
DLLEXTERN int add_grp_real(char *path);

// �������� ��㯯� 䠩��� �� �� (⮫쪮 ��� �⥭��)
// �����頥� ����� ��㯯� ��� -1 �� �訡��
DLLEXTERN int add_grp_ale(char *alename);

// �������� ��㯯� 䠩��� �� ��
// �����頥� ����� ��㯯� ��� -1 �� �訡��
DLLEXTERN int add_grp_ale_wr(char *alename);

// ���� ��㯯�
DLLEXTERN void remove_grp(int grp_handle);

typedef void (*_callback_open)(char *fn,int flg);

DLLEXTERN _callback_open callback_open;

// ������ 䠩� filename
// ��� 䠩� � ��㯯�� � ⮬ ���浪�, � ���஬ ��㯯� �뫨 ᮧ���� (�. ���)
// �᫨ ��⠭������ 䫠�� LF_ALE ��� LF_REAL, 䠩� ����� ⮫쪮
// � ᮮ⢥������� ��㯯��.
// �᫨ ��⠭����� 䫠� LF_CREATE, 䠩� ᮧ������.
// �� �訡�� �����頥��� NULL
DLLEXTERN LFILE L_open(char *filename,int flags);

// ������� 䠩�, ������ �� ����� L_open
// �����頥� -1 �� �訡��
DLLEXTERN int L_close(LFILE);

// �⥭�� �� 䠩��
// �����頥� �᫮ ���⠭��� ���� ��� -1 �� �訡��
DLLEXTERN int L_read(LFILE,void *ptr,int len);

// ������ � 䠩�
// �����頥� �᫮ ����ᠭ��� ���� ��� -1 �� �訡��
DLLEXTERN int L_write(LFILE,void *ptr,int len);

//the same to fgets return NULL if error
DLLEXTERN char *L_gets(LFILE h,char *ptr,int len);

//the same to fputs return 0 if error
DLLEXTERN int L_puts(LFILE h,char *ptr);


// ��।������ 㪠��⥫� �⥭��/�����
// �����頥� ⥪�騩 㪠��⥫� ��� -1 �� �訡��
// ��������!
// � �⫨稥 �� ॠ���� 䠩���, � ��-䠩��� �� ��६�饭�� 㪠��⥫� ��
// ����� 䠩�� �� ࠧ�襭��� ����� �� ᮧ������ ���⮥ ����, � �ந�室��
// �訡��
DLLEXTERN int L_seek(LFILE,int offset,int how);

// �����頥� ⥪�騩 㪠��⥫� ��� -1 �� �訡��
DLLEXTERN int L_tell(LFILE);

// �����頥� ����� 䠩�� ��� -1 �� �訡��
DLLEXTERN int L_length(LFILE);

DLLEXTERN int L_is_ale(LFILE);
DLLEXTERN int L_is_real(LFILE);

// ��२�������� 䠩�
// �����頥� 0 �� �訡��
DLLEXTERN int L_rename(char *oldname,char *newname,int flags);

// ����� 䠩�
// �����頥� 0 �� �訡��
DLLEXTERN int L_erase(char *filename,int flags);

// ���� ���� 䠩�
// �����頥� 0 �� �訡��
DLLEXTERN int L_find_first(char *mask,char attr,alefind_t *);

// ���� ᫥���騩 䠩�
// �����頥� 0 �� �訡��
DLLEXTERN int L_find_next(alefind_t *);

// �������� ���� 䠩���
// �����頥� 0 �� �訡��
DLLEXTERN int L_find_close(alefind_t *);

DLLEXTERN int L_file_exist(char *fn,int flags);

DLLEXTERN int L_dir_exist(char *fn,int flags);

#ifdef __cplusplus
}

enum{ALE_REAL,ALE_ALE};

// ��㯯� 䠩���
class AleGrp{
public:
  virtual ~AleGrp()=0;
  virtual int is_grp_ok(void)=0;
  virtual int open(char *filename,int flags)=0;
  virtual int close(int handle)=0;
  virtual int read(int handle,void *ptr,int len)=0;
  virtual int write(int handle,void *ptr,int len)=0;
  virtual int seek(int handle,int offset,int how)=0;
  virtual int tell(int handle)=0;
  virtual int length(int handle)=0;
  virtual int rename(char *oldname,char *newname,int flags)=0;
  virtual int erase(char *filename,int flags)=0;
  virtual int find_first(char *mask,char attr,alefind_t *)=0;
  virtual int find_next(alefind_t *)=0;
  virtual int find_close(alefind_t *)=0;
  virtual int mkdir(char *)=0;
  virtual int rmdir(char *)=0;
  virtual int get_full_path(char *fn,String&)=0;
  virtual int file_exist(char *fn)=0;
  virtual int dir_exist(char *fn)=0;
  virtual int class_id()=0;
};

// ॠ��� 䠩��
class AleGrpReal : public AleGrp {
protected:
  char basepath[128];
public:
  AleGrpReal(char *path);
  virtual int is_grp_ok(void);
  virtual int open(char *filename,int flags);
  virtual int close(int handle);
  virtual int read(int handle,void *ptr,int len);
  virtual int write(int handle,void *ptr,int len);
  virtual int seek(int handle,int offset,int how);
  virtual int tell(int handle);
  virtual int length(int handle);
  virtual int rename(char *oldname,char *newname,int flags);
  virtual int erase(char *filename,int flags);
  virtual int find_first(char *mask,char attr,alefind_t *);
  virtual int find_next(alefind_t *);
  virtual int find_close(alefind_t *);
  virtual int mkdir(char *);
  virtual int rmdir(char *);
  virtual int get_full_path(char *fn,String&);
  virtual int file_exist(char *fn);
  virtual int dir_exist(char *fn);
  virtual int class_id();
};

typedef struct{
  char name[55];
  char erased;
  int ofs,len;
}AleElem;

typedef struct{
  int h;
  int cur;
}AleOpened;

typedef struct{
  char name[56];
  int i;
}AleDirFn;

struct AleDir{
  char name[56];
  Tab<AleDir*> dir;
  Tab<AleDirFn> fn;
  AleDir();
  AleDir(char*);
  ~AleDir();
  int add_fn(char*,int i);
  int find_fn(char*);
  void sort();
};

// ��-䠩� (⮫쪮 �⥭��)
class AleGrpAle : public AleGrp {
protected:
  int handle;
  AleElem *tab;
  int num,tabofs;
  Tab<AleOpened> otab;
  char alepath[128];
  AleDir *aledir;
public:
  AleGrpAle();
  AleGrpAle(char *alename);
  virtual int is_grp_ok(void);
  virtual ~AleGrpAle();
  virtual int find_fn(char *fn);
  virtual int build_aledir();
  virtual int open(char *filename,int flags);
  virtual int close(int handle);
  virtual int read(int handle,void *ptr,int len);
  virtual int write(int handle,void *ptr,int len);
  virtual int seek(int handle,int offset,int how);
  virtual int tell(int handle);
  virtual int length(int handle);
  virtual int rename(char *oldname,char *newname,int flags);
  virtual int erase(char *filename,int flags);
  virtual int find_first(char *mask,char attr,alefind_t *);
  virtual int find_next(alefind_t *);
  virtual int find_close(alefind_t *);
  virtual int mkdir(char *);
  virtual int rmdir(char *);
  virtual int get_full_path(char *fn,String&);
  virtual int file_exist(char *fn);
  virtual int dir_exist(char *fn);
  virtual int class_id();
};

// ��-䠩�
class AleGrpAleWr : public AleGrpAle {
protected:
  int wrh,wrlen,wrcur,wrofs;
public:
  AleGrpAleWr(char *alename);
  virtual ~AleGrpAleWr();
  virtual int find_fn(char *fn);
  virtual int open(char *filename,int flags);
  virtual int close(int handle);
  virtual int read(int handle,void *ptr,int len);
  virtual int write(int handle,void *ptr,int len);
  virtual int seek(int handle,int offset,int how);
  virtual int tell(int handle);
  virtual int length(int handle);
  virtual int rename(char *oldname,char *newname,int flags);
  virtual int erase(char *filename,int flags);
  virtual int rmdir(char *);
};

// �������� ��㯯� 䠩���
// �����頥� ����� ��㯯� ��� -1 �� �訡��
DLLEXTERN int add_grp(AleGrp*);

DLLEXTERN AleGrp *get_ale_grp(int grp_handle);

#endif

#undef DLLEXTERN

#endif
