
#ifndef __LIB_SQUARE_H_
#define __LIB_SQUARE_H_

#ifdef __cplusplus

#include <fixed.h>

#ifndef _DLL
#pragma library("square.lib")
#define DLLEXTERN extern
#else
#ifndef __B_SQUARELIB
#pragma library("sqrdll.lib")
#define DLLEXTERN __declspec(dllimport)
#else
#define DLLEXTERN extern
#endif
#endif

#define max_sqrt 55536
#define max_sqr 8192
#define max_acos 32769
#define max_acos_2 16384
DLLEXTERN unsigned sqr_tab[max_sqr];
DLLEXTERN fix16 sqrt_tab[max_sqrt];
DLLEXTERN fix16 acos_tab[max_acos];
DLLEXTERN fix16 asin_tab[max_acos];

DLLEXTERN void init_square();
DLLEXTERN fix16 sqrt_f(int c);
DLLEXTERN unsigned sqr_f(int c);
DLLEXTERN fix16 acos_f(fix16 c);
DLLEXTERN fix16 asin_f(fix16 c);

#undef DLLEXTERN

#endif

#endif
