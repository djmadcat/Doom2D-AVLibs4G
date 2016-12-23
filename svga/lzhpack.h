// LZH pack/unpack

#ifdef __cplusplus
extern "C" {
#endif

// returns packed length or 0 on error
unsigned lzh_pack(void *ptr,unsigned len,void *buf);

// returns len or 0 on error
unsigned lzh_unpack(void *ptr,unsigned len,void *buf,unsigned blen);

#ifdef __cplusplus
}
#endif
