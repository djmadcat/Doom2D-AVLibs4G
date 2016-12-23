
#ifndef __SVGAPAL_LIB_H
#define __SVGAPAL_LIB_H

#ifndef __cplusplus
#error svgapal.h requires C++
#endif

#include <svga.h>

#ifndef _DLL
#define DLLEXTERN extern
#else
#ifndef __B_SVGALIB
#define DLLEXTERN __declspec(dllimport)
#else
#define DLLEXTERN extern
#endif
#endif

class PaletteMaker{
protected:
  unsigned short *hist;
public:
  PaletteMaker();
  ~PaletteMaker();
  int allocate(); // returns 0 on error; currently allocates 128K
  void add(int r,int g,int b,int n); // range 0..255
  void add16(color16,int n);
  void add15(color16,int n);
  void addcolors24(color24*,int);
  void addcolors32(color32*,int);
  void addcolors16(color16*,int);
  void addcolors15(color16*,int);
  void addcolors8(unsigned char *,int,color24 *pal);
  int calcpal(color24 *pal,int n); // returns number of colors or -1 on error
    // requires some memory (about 14 bytes per palette entry)
};

DLLEXTERN PaletteMaker *sv_palmaker;

DLLEXTERN void SV_add_screen_colors();

DLLEXTERN void *SV_make_vga_color_data(color24 *pal,int &size); // returns NULL on error
  // palette must have 256 colors
  // data may be released with free()

#undef DLLEXTERN

#endif
