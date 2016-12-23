#define WIN32_LEAN_AND_MEAN
#define DIRECTINPUT_VERSION 0x0300
#include <windows.h>
#include <ddraw.h>
#include <stdlib.h>
#include <stdio.h>
#include <svga.h>
#include <config.h>

extern LPDIRECTDRAWPALETTE DDpal;

char devname[256]="",_app_active=1;

#include "consts.h"

void fatal(char*,...);

void Redraw();

void idle() {
  MSG msg;
  while(PeekMessage(&msg,NULL,0,0,PM_NOREMOVE)) {
    if(!GetMessage(&msg,NULL,0,0)) {
      ExitProcess(msg.wParam);
    }
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
}

void init_ddraw(){
  GUID *id=NULL;
  if(FAILED(DirectDrawCreate(id,&DDraw,NULL)))
    fatal("Can't initialize DirectDraw");
};

HWND mainwnd=NULL;
HINSTANCE hinstance;
char _initing=1;

void Redraw();

void close_win(){
 if(mainwnd) {ShowWindow(mainwnd,SW_HIDE);}
}

#include <stdarg.h>
void close();

void fatal(char *s,...) {
  static char buf[1024];
  va_list ap;
  va_start(ap,s);
  _vbprintf(buf,1024,s,ap);
  va_end(ap);
  close();
  MessageBox(NULL,buf,"FATAL ERROR",MB_OK|MB_ICONSTOP);
  exit(1);
}

extern char exit_flg;
extern int show_speed,pause;

void next_frame();
void start_flik();
void delete_flik();

long FAR PASCAL WindowProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
  PAINTSTRUCT ps;
  switch(message) {
    case WM_ACTIVATEAPP:
      _app_active=LOWORD(wParam);
      if(_app_active && !_initing) {
        Redraw();
        SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS);
        SV_restore();
        if(DDpal) DDSmain->SetPalette(DDpal);
      }else if(!_app_active && !_initing) {
        SetPriorityClass(GetCurrentProcess(),NORMAL_PRIORITY_CLASS);
//        _swapped_out=1;
      }
      break;
    case WM_KEYDOWN:
      exit_flg=0;
      if (wParam==VK_SUBTRACT) {exit_flg=0;show_speed+=5;if (show_speed>=__MAX_SPEED) show_speed=__MAX_SPEED-1;} else
      if (wParam==VK_ADD) {exit_flg=0;show_speed--;if (show_speed<=0) show_speed=__MIN_SPEED;} else
      if (wParam==32) {exit_flg=0;pause=!pause;} else
      if (wParam=='s') {exit_flg=0;start_flik();} else
      if (wParam==VK_RIGHT) {exit_flg=0;next_frame();}
      if (wParam==VK_LEFT) {exit_flg=0;start_flik();Redraw();}
      if (wParam==VK_ESCAPE) exit_flg=1;
      break;
    case WM_CREATE:
      break;
//    case WM_MOUSEMOVE:
//      if(sv_in_window) I_mset(LOWORD(lParam),HIWORD(lParam));
//      break;
//    case WM_SETCURSOR:
//      if(LOWORD(lParam)==HTNOWHERE || LOWORD(lParam)==HTERROR
//        || LOWORD(lParam)==HTTRANSPARENT) break;
//      if(sv_in_window) if(LOWORD(lParam)!=HTCLIENT) break;
//      SetCursor(NULL);
//      return TRUE;
    case WM_ERASEBKGND:
      return 1;
    case WM_PAINT:
      BeginPaint( hWnd, &ps );
      EndPaint( hWnd, &ps );
      Redraw();
      return 1;
    case WM_DESTROY:
      PostQuitMessage( 0 );
      break;
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
}

char load_file[256]="test.vid";

void common_startup(char *cfn,HINSTANCE hinst,char *iconname,char *wcname,
char *title,int ncmdshow) {
  hinstance=hinst;
  int win=0;
  for(int i=1;i<__argc;++i) {
    char *s=__argv[i];
    if(*s=='/' || *s=='-') ++s;
    if(stricmp(s,"win")==0) win=1; else
      strcpy(load_file,__argv[i]);
  }
  int w=640,h=480,b=16;
  if(CFG_read(cfn,"video","width")) w=cfg_val;
  if(CFG_read(cfn,"video","height")) h=cfg_val;
  if(CFG_read(cfn,"video","bits")) b=cfg_val;

  WNDCLASS wc;
  wc.style=CS_HREDRAW|CS_VREDRAW;
  wc.lpfnWndProc=WindowProc;
  wc.cbClsExtra=0;
  wc.cbWndExtra=0;
  wc.hInstance=hinst;
  wc.hIcon=LoadIcon(hinst,iconname);
  wc.hCursor=LoadCursor(NULL,IDC_ARROW);
  wc.hbrBackground=NULL;
  wc.lpszMenuName=NULL;
  wc.lpszClassName=wcname;
  RegisterClass(&wc);

  int sx=GetSystemMetrics(SM_CXSCREEN),sy=GetSystemMetrics(SM_CYSCREEN);
  DWORD stl=(!win)?(WS_POPUP):(WS_POPUP|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX);
  RECT wr; wr.left=0;wr.top=0;wr.right=w;wr.bottom=h;
  AdjustWindowRect(&wr,stl,FALSE);

  mainwnd=CreateWindowEx(
      WS_EX_APPWINDOW,
      wcname,
      title,
      stl,
      (sx-w)/2,(sy-h)/2,wr.right-wr.left,wr.bottom-wr.top,
      NULL,
      NULL,
      hinst,
      NULL);

  if(!mainwnd) fatal("Can't create window");

  ShowWindow(mainwnd,ncmdshow);
  UpdateWindow(mainwnd);

  SetCursor(NULL);

  sv_back_memory=SV_BM_SYSTEM;
  if(CFG_read(cfn,"video","videomem"))
    if(cfg_val) {sv_back_memory=SV_BM_VIDEO;}
  if(CFG_read(cfn,"video","systemmem"))
    if(cfg_val) {sv_back_memory=SV_BM_SYSTEM;}

  init_ddraw();

  if(!SV_findmode(w,h,b,mainwnd,win)) {
    fatal("Unable to initialize video mode %dx%dx%d",w,h,b);
  }

  SetPriorityClass(GetCurrentProcess(),HIGH_PRIORITY_CLASS);
 _initing=0;
}

extern char cfn[];


int main_init();
void init(char *fl);

int PASCAL WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance,
LPSTR lpCmdLine, int nCmdShow) {
  common_startup(cfn,hInstance,"DemoIcon","DemoWClass",
    "SMBFLIK PLAYER",nCmdShow);
  init(load_file);
  main_init();
  delete_flik();
  return 0;
}
