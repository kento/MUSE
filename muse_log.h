
double muse_get_time(void);
void muse_log_open(char *path);
void muse_log_write(int pid, char *io_path, size_t offset, size_t length, char *mode, double duration);
void muse_log_close();

