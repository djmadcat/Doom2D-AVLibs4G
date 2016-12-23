
enum{
  A_BACK,
  A_PANEL,A_PANEL_HDR,A_PANEL_SEL_HDR,A_PANEL_CUR,
  A_PANEL_MARK,A_PANEL_MARK_CUR,
  A_ERR_TEXT,A_ERR_BUT,A_ERR_BUT_H,A_ERR_BUT_S,A_ERR_BUT_SH,
  A_PBOX,A_PBOX_INP,
  A_KB_NUM,A_KB_TEXT,
  A_PBOX_BAR1,A_PBOX_BAR2,
  A__LAST
};

extern char attrs[];

#ifdef DEFINE_ATTRS
#undef DEFINE_ATTRS

char attrs[A__LAST]={
  0x07,
  0x17,0x17,0x30,0x30,
  0x1C,0x3C,
  0x4F,0x4F,0x4E,0x70,0x71,
  0x20,0x70,
  0x07,0x30,
  0x70,0x07
};

#endif
