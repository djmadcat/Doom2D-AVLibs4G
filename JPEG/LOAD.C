#include <stdlib.h>
#include <stdio.h>
#include <ale.h>
#include <string.h>
#include "jpeglib.h"
#include "jerror.h"
#include <setjmp.h>
#include <svga.h>

#ifdef __SW_BD
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif

struct my_error_mgr {
  struct jpeg_error_mgr pub;
  jmp_buf setjmp_buffer;
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF void my_error_exit (j_common_ptr cinfo) {
  my_error_ptr myerr = (my_error_ptr) cinfo->err;
  longjmp(myerr->setjmp_buffer,1);
}

//лллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллллл//

DLLEXPORT image *load_jpeg(char *fn) {
     struct jpeg_decompress_struct cinfo;
     struct my_error_mgr           jerr;

     unsigned char *bf;
     JSAMPARRAY buffer;
     int row_stride,x;
     LFILE fh;
     image *im;

     if(!(fh=L_open(fn,LF_READ))) return NULL;

     cinfo.err           = jpeg_std_error(&jerr.pub);
     jerr.pub.error_exit = my_error_exit;

     if(setjmp(jerr.setjmp_buffer)) {
	jpeg_destroy_decompress(&cinfo);
	L_close(fh);
	return NULL;
     }

     jpeg_create_decompress(&cinfo);
     jpeg_ale_src(&cinfo,fh);

     jpeg_read_header(&cinfo,TRUE);
     jpeg_start_decompress(&cinfo);

     row_stride = cinfo.output_width * 4;

     buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

     im=malloc(cinfo.output_width*cinfo.output_height*4+sizeof(image));
     if(!im) {
       jpeg_finish_decompress(&cinfo);
       jpeg_destroy_decompress(&cinfo);
       L_close(fh);
       return NULL;
     }
     im->w=cinfo.output_width;im->h=cinfo.output_height;
     im->sx=im->sy=0;

     bf=(unsigned char *)(im+1);

     while (cinfo.output_scanline < cinfo.output_height) {
	unsigned char *p;
	jpeg_read_scanlines(&cinfo,buffer,1);
	p=buffer[0];
	switch(cinfo.num_components) {
	  case 3:
	    for(x=0;x<im->w;++x,p+=3) {
	      *bf++=p[2];
	      *bf++=p[1];
	      *bf++=p[0];
	      *bf++=255;
	    }
	    break;
	  case 4:
	    for(x=0;x<im->w;++x,p+=4) {
	      *bf++=p[2];
	      *bf++=p[1];
	      *bf++=p[0];
	      *bf++=p[3];
	    }
	    break;
	  case 1:
	    for(x=0;x<im->w;++x,++p) {
	      *bf++=p[0];
	      *bf++=p[0];
	      *bf++=p[0];
	      *bf++=255;
	    }
	    break;
	}
     }

     jpeg_finish_decompress(&cinfo);
     jpeg_destroy_decompress(&cinfo);
     L_close(fh);

     return im;
}
