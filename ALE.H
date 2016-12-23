
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

// инициализовать таблицы для работы с символами текущей кодовой страницы DOS
DLLEXTERN void init_local_conv(void);

// преобразовать все буквы в заглавные
DLLEXTERN char *str_upr(char *s);

// преобразовать все буквы в строчные
DLLEXTERN char *str_lwr(char *s);

DLLEXTERN char chr_upr(unsigned char c);

DLLEXTERN char chr_lwr(unsigned char c);

// сравнить строки без учета заглавных/строчных букв
// аналог stricmp()
DLLEXTERN int str_icmp(const char *a,const char *b);

// сравнить не более n первых символов строк без учета заглавных/строчных букв
// аналог strnicmp()
DLLEXTERN int str_nicmp(const char *a,const char *b,int n);

// сравнить n символов без учета заглавных/строчных букв
// аналог memicmp()
DLLEXTERN int mem_icmp(const char *a,const char *b,int n);

// сравнить имена файлов с учетом символов * и ?
// возвращает 0, если имена совпадают, и 1, если нет
DLLEXTERN int file_cmp(const char *a,const char *b);

enum{ LA_SUBDIR=0x10, LA_ERASED=0x80 };

// структура для поиска файлов
typedef struct{
  int grp;
  void *data;
  char fmask[128];
  int fattr;
  int size;             // размер файла
  char name[13];        // имя файла
  char attr;            // атрибуты файла
}alefind_t;

typedef void *LFILE;

// LF_CREATE - создать файл (стирает старый)
// LF_ALE    - выполнить операцию в ALE-файле
// LF_REAL   - выполнить операцию с реальными файлами
enum{ LF_READ=1, LF_WRITE=2, LF_RDWR=3, LF_CREATE=4, LF_ALE=8, LF_REAL=16 };
enum{ LF_SEEK_SET, LF_SEEK_CUR, LF_SEEK_END };

// инициализовать систему (вызывает init_local_conv())
DLLEXTERN void init_ale(void);

// закрыть систему (сохранить изменения в ALE-файлах)
DLLEXTERN void shutdown_ale(void);

// добавить группу реальных файлов из каталога path
// возвращает номер группы или -1 при ошибке
DLLEXTERN int add_grp_real(char *path);

// добавить группу файлов из эля (только для чтения)
// возвращает номер группы или -1 при ошибке
DLLEXTERN int add_grp_ale(char *alename);

// добавить группу файлов из эля
// возвращает номер группы или -1 при ошибке
DLLEXTERN int add_grp_ale_wr(char *alename);

// убрать группу
DLLEXTERN void remove_grp(int grp_handle);

typedef void (*_callback_open)(char *fn,int flg);

DLLEXTERN _callback_open callback_open;

// открыть файл filename
// ищет файл в группах в том порядке, в котором группы были созданы (см. выше)
// если установлены флаги LF_ALE или LF_REAL, файл ищется только
// в соответствующих группах.
// если установлен флаг LF_CREATE, файл создается.
// при ошибке возвращается NULL
DLLEXTERN LFILE L_open(char *filename,int flags);

// закрыть файл, открытый при помощи L_open
// возвращает -1 при ошибке
DLLEXTERN int L_close(LFILE);

// чтение из файла
// возвращает число прочитанных байт или -1 при ошибке
DLLEXTERN int L_read(LFILE,void *ptr,int len);

// запись в файл
// возвращает число записанных байт или -1 при ошибке
DLLEXTERN int L_write(LFILE,void *ptr,int len);

//the same to fgets return NULL if error
DLLEXTERN char *L_gets(LFILE h,char *ptr,int len);

//the same to fputs return 0 if error
DLLEXTERN int L_puts(LFILE h,char *ptr);


// передвинуть указатель чтения/записи
// возвращает текущий указатель или -1 при ошибке
// ВНИМАНИЕ!
// в отличие от реальных файлов, в эль-файлах при перемещении указателя за
// конец файла при разрешенной записи не создается пустое место, а происходит
// ошибка
DLLEXTERN int L_seek(LFILE,int offset,int how);

// возвращает текущий указатель или -1 при ошибке
DLLEXTERN int L_tell(LFILE);

// возвращает длину файла или -1 при ошибке
DLLEXTERN int L_length(LFILE);

DLLEXTERN int L_is_ale(LFILE);
DLLEXTERN int L_is_real(LFILE);

// переименовать файл
// возвращает 0 при ошибке
DLLEXTERN int L_rename(char *oldname,char *newname,int flags);

// стереть файл
// возвращает 0 при ошибке
DLLEXTERN int L_erase(char *filename,int flags);

// найти первый файл
// возвращает 0 при ошибке
DLLEXTERN int L_find_first(char *mask,char attr,alefind_t *);

// найти следующий файл
// возвращает 0 при ошибке
DLLEXTERN int L_find_next(alefind_t *);

// закончить поиск файлов
// возвращает 0 при ошибке
DLLEXTERN int L_find_close(alefind_t *);

DLLEXTERN int L_file_exist(char *fn,int flags);

DLLEXTERN int L_dir_exist(char *fn,int flags);

#ifdef __cplusplus
}

enum{ALE_REAL,ALE_ALE};

// группа файлов
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

// реальные файлы
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

// эль-файл (только чтение)
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

// эль-файл
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

// добавить группу файлов
// возвращает номер группы или -1 при ошибке
DLLEXTERN int add_grp(AleGrp*);

DLLEXTERN AleGrp *get_ale_grp(int grp_handle);

#endif

#undef DLLEXTERN

#endif
