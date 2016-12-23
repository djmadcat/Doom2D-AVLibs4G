#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <square.h>

#ifdef __SW_BD
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

static int sqr_flag=0;
static int sqrt_flag=0;
static int acos_flag=0;
static int asin_flag=0;

DLLEXPORT unsigned sqr_tab[max_sqr];
DLLEXPORT fix16 sqrt_tab[max_sqrt];
DLLEXPORT fix16 acos_tab[max_acos];
DLLEXPORT fix16 asin_tab[max_acos];

DLLEXPORT fix16 sqrt_f(int c) {
  if ((c>=0)&&(c<=max_sqrt)) return(sqrt_tab[c]); else return (fix16)sqrt((double)c);
}

DLLEXPORT unsigned sqr_f(int c) {
  if ((c>=0)&&(c<=max_sqr-1)) return (sqr_tab[c]); else
  if (c>=-max_sqrt+1) return (sqr_tab[-c]); else return c*c;
}

DLLEXPORT fix16 acos_f(fix16 c) {
  if ((c>=-1)&&(c<=1)) return (acos_tab[(int)((1+c)*max_acos_2)]); else return (3.14159265);
}

DLLEXPORT fix16 asin_f(fix16 c) {
  if ((c>=-1)&&(c<=1)) return (asin_tab[(int)((1+c)*max_acos_2)]); else return (3.14159265/2);
}

DLLEXPORT void init_square(){
  if (!sqrt_flag) {
    sqrt_flag=1;
    for (int i=0;i<max_sqrt;++i) {sqrt_tab[i]=(fix16)sqrt((double)i);}
  }
  if (!sqr_flag) {
    sqr_flag=1;
    for (unsigned i=0;i<max_sqr;++i) {sqr_tab[i]=(unsigned)i*i;}
  }
  if (!acos_flag) {
    acos_flag=1;
    for (int i=0;i<max_acos;++i) {acos_tab[i]=(fix16)acos((((double)i)-max_acos_2)/max_acos_2);}
  }
  if (!asin_flag) {
    asin_flag=1;
    for (int i=0;i<max_acos;++i) {asin_tab[i]=(fix16)asin((((double)i)-max_acos_2)/max_acos_2);}
  }
}
