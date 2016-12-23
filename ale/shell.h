
#ifndef __ALE_SHELL_H
#define __ALE_SHELL_H

#include <stdarg.h>
#include <str.h>

int have_key();
int get_key();
int get_shift();

enum{SH_RIGHT=1,SH_LEFT=2,SH_CTRL=4,SH_ALT=8};

void init_video(void);
void close_video(void);
void updatescr(void);
void *grabscr();
void drawscr(void *);
void prpos(int x,int y);
void movecur(int x,int y);
void hidecur();
void prch(int c,int a);
void skipch(void);
void prf(int a,char *s,...);
void prstr(int a,char *s);
void clear(int x,int y,int w,int h,int a);
void border(int x,int y,int w,int h,int t,int a);

void clear_insert();

extern int scrw,scrh;

class Box{
public:
  int left,top,width,height;
  Box() {}
  Box(int x,int y,int w,int h) {left=x;top=y;width=w;height=h;}
  virtual ~Box();
  virtual void draw(void);
  virtual void act(void);
  virtual int key(int k);
};

#define FRAME_SX 5
#define FRAME_SY 2

class FrameBox : virtual public Box {
public:
  String header;
  char frameattr,frametype;
  FrameBox(char a=0x70,char t=0,char *hdr=NULL);
  FrameBox(int x,int y,int w,int h,char a=0x70,char t=0,char *hdr=NULL);
  virtual void draw(void);
  virtual void set_hdr(char *s);
};

class Panel : virtual public Box {
public:
  String header;
  char selected;
  Panel(int x,int y,int w,int h,char *s=NULL);
  virtual ~Panel();
  virtual void set_hdr(char *s);
  virtual void draw(void);
};

enum{SEL_SELECT,SEL_UNSELECT,SEL_INVERT,SEL_DIR=0x40,SEL_EXCLUDE=0x80};

enum{FT_FILE,FT_SUBDIR,FT_DRIVE,FT_UPDIR};

typedef struct{
  char name[8+3];
  char type;
  int size;
  char selected;
}FileData;

class FilePanel : public Panel {
public:
  char path[256],subpath[64];
  char in_ale;
  AleGrp *grp;
  Tab<FileData> tab;
  int topline,curline;
  int free_space;

  FilePanel(int x,int y,int w,int h,char *path);
  virtual ~FilePanel();
  virtual void re_read(void);
  virtual void update_files(void);
  virtual void setcurpath();
  virtual AleGrp *reset_grp();
  virtual int set_path(char *,int rr=1);
  virtual int go_up_dir(void);
  virtual int go_sub_dir(char *,int isdir);
  virtual int go_to_name(char *);
  virtual void draw(void);
  virtual int key(int k);
  virtual void scroll(void);
  virtual int count_selected(int &f,int &d,int &sz);
  virtual int select_by_mask(char *mask,int sel_type);
};

typedef struct{
  char *name;
  int id,hotkey;
}ButtonData;

class ButtonBox : virtual public Box {
public:
  Tab<ButtonData> but;
  int cur,butx,buty;
  char butattr[4];
  ButtonBox();
  ButtonBox(int x,int y,int w,int h);
  ButtonBox(int x,int y,int w,int h,int n,...);
  ButtonBox(int x,int y,int w,int h,int n,va_list);
  virtual void setbut(int w,int h,int n,...);
  virtual void setbut(int w,int h,int n,va_list);
  virtual ~ButtonBox();
  virtual void draw(void);
  virtual int key(int k);
  virtual void press_button(int id);
};

class MessageBox : public ButtonBox, public FrameBox {
public:
  int esc_id,pressed_id;
  char *text;
  char textattr;
  MessageBox(char *msg,int num,int esc,...);
  MessageBox(char *msg,int num,int esc,va_list);
  virtual void setmsg(char *msg,int num,int esc,...);
  virtual void setmsg(char *msg,int num,int esc,va_list);
  virtual void draw(void);
  virtual int key(int k);
  virtual void press_button(int id);
};

class InputBox : virtual public Box {
public:
  String buf;
  int cur,skip;
  char selected,touched;
  char inpattrs[2];
  InputBox();
  InputBox(int x,int y,int w,char *s=NULL);
  virtual void draw(void);
  virtual int key(int k);
  virtual void settext(char *s);
  virtual void setsel(int);
  virtual void scroll();
  virtual int caninsert(unsigned char c);
};

class KeyBar : virtual public Box {
public:
  char text[4][10][6];
  int shift;
  KeyBar();
  virtual void draw(void);
  virtual void act(void);
  virtual int key(int k);
  virtual int key_pressed(int sh,int n);
};

int message(char *s,char *hdr,int n,int esc,int def,...);
int input(String& ed,int x,int y,int minw,char *text,char *hdr);

extern String file_sel_mask;
extern Panel *panel[2];
extern KeyBar *keybar;

extern char chgdrv_failed;

enum{KB_MAIN};

KeyBar *get_keybar(int t);

#endif
