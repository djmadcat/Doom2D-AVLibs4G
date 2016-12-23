#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ale.h"

int main(int ac,char *av[]) {
  if(ac<4) {
    puts(
      "Usage: ALE command ale_file.ale filename.ext\n"
      "Commands:\n"
      "    a - add file\n"
      "    e - extract file\n"
      "  del - delete file\n"
      "    l - list files in Ale"
    );
    return 1;
  }

  init_ale();
  if(add_grp_ale_wr(av[2])==-1) {puts("ALE error");return 1;}
  if(add_grp_real(".")==-1) {puts("Error adding REAL file group");return 1;}

  if(stricmp(av[1],"a")==0) {
    LFILE h=L_open(av[3],LF_READ|LF_REAL);
    if(!h) {puts("Error opening REAL file");return 1;}
    int len=L_length(h);
    if(len==-1) {puts("Error reading REAL file");return 1;}
    void *ptr=malloc(len);
    if(!ptr) {puts("Not enough memory");return 1;}
    if(L_read(h,ptr,len)!=len) {puts("Error reading REAL file");return 1;}
    L_close(h);
    if(!(h=L_open(av[3],LF_WRITE|LF_CREATE|LF_ALE)))
      {puts("Error creating file in ALE");return 1;}
    if(L_write(h,ptr,len)!=len) {puts("Error writing file in ALE");return 1;}
    L_close(h);
    free(ptr);
  }else if(stricmp(av[1],"e")==0) {
    LFILE h=L_open(av[3],LF_READ|LF_ALE);
    if(!h) {puts("Error opening file in ALE");return 1;}
    int len=L_length(h);
    if(len==-1) {puts("Error reading file in ALE");return 1;}
    void *ptr=malloc(len);
    if(!ptr) {puts("Not enough memory");return 1;}
    if(L_read(h,ptr,len)!=len) {puts("Error reading file in ALE");return 1;}
    L_close(h);
    if(!(h=L_open(av[3],LF_WRITE|LF_CREATE|LF_REAL)))
      {puts("Error creating REAL file");return 1;}
    if(L_write(h,ptr,len)!=len) {puts("Error writing REAL file");return 1;}
    L_close(h);
    free(ptr);
  }else if(stricmp(av[1],"l")==0) {
    alefind_t ff;
    for(int ok=L_find_first(av[3],LA_SUBDIR,&ff);ok;ok=L_find_next(&ff)) {
      printf("%-16s",ff.name);
      if(ff.attr&LA_SUBDIR) printf("<SUBDIR>\n");
      else printf("%d\n",ff.size);
    }
    L_find_close(&ff);
  }else if(stricmp(av[1],"del")==0) {
    if(!L_erase(av[3],LF_ALE)) {puts("Error erasing file in ALE");return 1;}
  }
  shutdown_ale();
  puts("OK");
  return 0;
}
