
class FileOp {
public:
  virtual ~FileOp();
  virtual int file(AleGrp *,char *sp,char *fn)=0;
  virtual int dir(AleGrp *,char *sp,char *dir)=0;
  virtual int dir_post_op(AleGrp *,char *sp,char *dir);
  virtual int is_ok()=0;
};

class FileCopyOp : virtual public FileOp {
public:
  String dest;
  AleGrp *dgrp;
  void *buf;
  char ow_all;
  FileCopyOp(char *to);
  virtual ~FileCopyOp();
  virtual int file(AleGrp *,char *sp,char *fn);
  virtual int dir(AleGrp *,char *sp,char *dir);
  virtual int is_ok();
};

class FileDeleteOp : virtual public FileOp {
public:
  char del_all;
  FileDeleteOp();
  virtual ~FileDeleteOp();
  virtual int file(AleGrp *,char *sp,char *fn);
  virtual int dir(AleGrp *,char *sp,char *dir);
  virtual int dir_post_op(AleGrp *,char *sp,char *dir);
  virtual int is_ok();
};

int start_fileop(FileOp *);
int fileop_file(AleGrp *,char *,char *);
int fileop_dir(AleGrp *,char *,char *);
