
#ifndef __LOADJPEG_LIB_H
#define __LOADJPEG_LIB_H

#ifndef _DLL
#pragma library("jpeg.lib")
#define DLLEXTERN extern
#else
#ifndef __B_JPEGLIB
#pragma library("jpegdll.lib")
#define DLLEXTERN __declspec(dllimport)
#else
#define DLLEXTERN extern
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

DLLEXTERN image *load_jpeg(char *fn);

#ifdef __cplusplus
}
#endif

#undef DLLEXTERN

#endif
