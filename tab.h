//컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�//
//       Tab - �젩ギ� ㄸ췅Ж曄稅�． 쵟遜Ð�
//       뤲º�贍 ��誓ㄵゥ�⑨:
//         Tab<int> int_array;          // 쵟遜Ð 璵ル� 葉醒�
//         Tab< MyClass * > units;      // 쵟遜Ð 丞젳졻�ゥ� 췅 か졹� MyClass
//컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�//

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

// 押�ゆ⑨ 貰設ⓣ�˚�
// ＋蹟ⓥ Д�麟�, 曄� Watcom�㏇え� qsort (�.�. ��첓 �� ＋蹟Œ�)
// αエ �� 罌졻ⓥ 캙э殊 췅 2(ㄲ�) 姉�Д���, 獸 貰設ⓣ�쥯筍 �� ▲ㄵ�
DLLEXTERN void qusort(void *p,int n,int w,int (*f) (const void*,const void*));

#ifdef __cplusplus
}

#include <stdlib.h>
#include <string.h>

// ⓤ��レ㎯β碎 獸レぎ か졹�젹� Tab � TabSorted
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
  // ����졻�� �黍聲젴쥯�⑨ 貰ℓ젰� ぎ�⑧ 쵟遜Ð�
  Tab& operator =(const Tab& a) {
    if(ptr) free(ptr);
    total=used=0;if(!(ptr=(T*)malloc(a.used*sizeof(T)))) return *this;
    total=used=a.used;memcpy(ptr,a.ptr,used*sizeof(T));
    return *this;
  }
  // ����졻�贍 ㄾ飡承� � 姉�Д��젹 쵟遜Ð�
  T& operator[](int i) {
#ifdef _DEBUG_TAB_
    assert(i>=0 && i<used);
#endif
    return ptr[i];
  }
  operator T*() {return ptr;}
  // ㏇�젪ⓥ� n 姉�Д�獸� ��誓� at-臾 姉�Д�獸� 쵟遜Ð�
  // ¡㎖�좈젰� ��Д� ��舒�． ㄾ줎˙����． 姉�Д��� (�.�. at) Œ� -1 �黍 �鼇―�
  int insert(int at,int n,T *p=NULL,int step=8)
    {return TABinsert((void*&)ptr,total,used,at,n,sizeof(T),p,step);}
  // ㄾ줎˘筍 n 姉�Д�獸� � ぎ�ζ 쵟遜Ð�
  // ¡㎖�좈젰� ��Д� ��舒�． ㄾ줎˙����． 姉�Д��� Œ� -1 �黍 �鼇―�
  int append(int n,T *p=NULL,int step=8)
    {return TABinsert((void*&)ptr,total,used,used,n,sizeof(T),p,step);}
  // 飡�誓筍 n 姉�Д�獸�, 췅葉췅� � at-�．
  // �� �聲�‘┐젰� 캙э筍 - ⓤ��レ㎯� shrink()
  void erase(int at,int n) {
    if(used-at-n>0) memmove(ptr+at,ptr+at+n,(used-at-n)*sizeof(T));
    used-=n;
  }
  // �葉飡ⓥ� 쵟遜Ð
  void clear(void) {if(ptr) free(ptr); ptr=NULL;total=used=0;}
  // �㎚��ⓥ� �젳Д� 쵟遜Ð�
  int resize(int c) {
    if(total>=c) {used=c;return 1;}
    T *p=(T*)realloc(ptr,c*sizeof(T));
    if(p) {
      used=total=c;ptr=p;
      return 1;
    }
    return 0;
  }
  // 葉笹� 姉�Д�獸� 쵟遜Ð�
  int count(void) {return used;}
  // �聲�‘ㄸ筍 �ⅷ召�レ㎯�с� 캙э筍
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
