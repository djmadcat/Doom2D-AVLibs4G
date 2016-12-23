//컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�//
//       fix16 - 16.16
//       fix8  - 24.8
//컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�//

#ifndef __FIXED_LIB_H
#define __FIXED_LIB_H

//#pragma library("avcpp.lib")

#ifdef __cplusplus

#include <generic.h>

class __fixbase {
public:
  int v;
};

#define B 16
#define SHRDB "shrd eax,edx,16"
#define SHLDB "shld edx,eax,16" \
              "shl eax,     16"
#include <fixtemp.h>

#define B 8
#define SHRDB "shrd eax,edx,8"
#define SHLDB "shld edx,eax,8" \
              "shl eax,     8"
#include <fixtemp.h>

inline fix8  fix16to8(fix16 a) {fix8  f;f.v=a.v>>8;return f;}
inline fix16 fix8to16(fix8  a) {fix16 f;f.v=a.v<<8;return f;}

#endif

#endif
