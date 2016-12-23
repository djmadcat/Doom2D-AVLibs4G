//���������������������������������������������������������������������������//
//       Tab - 蠡��� �������᪮�� ���ᨢ�
//       �ਬ��� ��।������:
//         Tab<int> int_array;          // ���ᨢ 楫�� �ᥫ
//         Tab< MyClass * > units;      // ���ᨢ 㪠��⥫�� �� ����� MyClass
//���������������������������������������������������������������������������//

#ifndef __TAB_LIB_H
#define __TAB_LIB_H

#ifndef _DLL
#pragma library("avcpp.lib")
#define DLLEXTERN extern
#else
#ifndef __B_TABLIB
#pragma library("avcppdll.lib")
#define DLLEXTERN __declspec(dllimport)
#else
#define DLLEXTERN extern
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

// �㭪�� ���஢��
// ����� �����, 祬 Watcom��᪨� qsort (�.�. ���� �� ���稫�)
// �᫨ �� 墠�� ����� �� 2(���) �����, � ���஢��� �� �㤥�
DLLEXTERN void qusort(void *p,int n,int w,int (*f) (const void*,const void*));

#ifdef __cplusplus
}

#include <stdlib.h>
#include <string.h>

// �ᯮ������ ⮫쪮 ����ᠬ� Tab � TabSorted
DLLEXTERN int TABinsert(void*&,int&,int&,int,int,int,void*,int);

template <class T> class Tab{
protected:
  int total,used;
  T *ptr;
public:
  Tab() {ptr=NULL;total=used=0;}
  init() {ptr=NULL;total=used=0;}
  ~Tab() {if(ptr) free(ptr);}
  Tab(const Tab &a) {
    total=used=0;if(!(ptr=(T*)malloc(a.used*sizeof(T)))) return;
    total=used=a.used;memcpy(ptr,a.ptr,used*sizeof(T));
  }
  // ������ ��ᢠ������ ᮧ���� ����� ���ᨢ�
  Tab& operator =(const Tab& a) {
    if(ptr) free(ptr);
    total=used=0;if(!(ptr=(T*)malloc(a.used*sizeof(T)))) return *this;
    total=used=a.used;memcpy(ptr,a.ptr,used*sizeof(T));
    return *this;
  }
  // ������� ����㯠 � ����⠬ ���ᨢ�
  T& operator[](int i) {
#ifdef _DEBUG_TAB_
    assert(i>=0 && i<used);
#endif
    return ptr[i];
  }
  operator T*() {return ptr;}
  // ��⠢��� n ����⮢ ��। at-� ����⮬ ���ᨢ�
  // �����頥� ����� ��ࢮ�� ������������ ����� (�.�. at) ��� -1 �� �訡��
  int insert(int at,int n,T *p=NULL,int step=8)
    {return TABinsert((void*&)ptr,total,used,at,n,sizeof(T),p,step);}
  // �������� n ����⮢ � ����� ���ᨢ�
  // �����頥� ����� ��ࢮ�� ������������ ����� ��� -1 �� �訡��
  int append(int n,T *p=NULL,int step=8)
    {return TABinsert((void*&)ptr,total,used,used,n,sizeof(T),p,step);}
  // ����� n ����⮢, ��稭�� � at-���
  // �� �᢮������� ������ - �ᯮ��� shrink()
  void erase(int at,int n) {
    if(used-at-n>0) memmove(ptr+at,ptr+at+n,(used-at-n)*sizeof(T));
    used-=n;
  }
  // ������ ���ᨢ
  void clear(void) {if(ptr) free(ptr); ptr=NULL;total=used=0;}
  // �������� ࠧ��� ���ᨢ�
  int resize(int c) {
    if(total>=c) {used=c;return 1;}
    T *p=(T*)realloc(ptr,c*sizeof(T));
    if(p) {
      used=total=c;ptr=p;
      return 1;
    }
    return 0;
  }
  // �᫮ ����⮢ ���ᨢ�
  int count(void) {return used;}
  // �᢮������ ���ᯮ��㥬�� ������
  void shrink(void) {
    ptr=(T*)realloc(ptr,used*sizeof(T));
    total=used;
  }
  void sort(int at,int n,int (*f) (const T*,const T*)) {
    qusort(ptr+at,n,sizeof(T),(int (*) (const void*,const void*))f);
  }
  void sort(int (*f) (const T*,const T*)) {
    qusort(ptr,used,sizeof(T),(int (*) (const void*,const void*))f);
  }
};

#endif

#undef DLLEXTERN

#endif
