#ifndef MUSE_ERR_H
#define MUSE_ERR_H

void muse_err_init(int r);
void muse_err(const char* fmt, ...);
void muse_alert(const char* fmt, ...);
void muse_dbg(const char* fmt, ...);
void muse_print(const char* fmt, ...);
void muse_debug(const char* fmt, ...);
void muse_btrace();

void muse_exit(int no);

#endif
