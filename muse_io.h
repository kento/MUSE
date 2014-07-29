
int muse_open(const char* file, int flags, mode_t  mode);
int muse_close(const char* file, int fd);
ssize_t muse_write(const char* file, int fd, const void* buf, size_t size);
ssize_t muse_read(const char* file, int fd, void* buf, size_t size);
off_t muse_lseek(int fd, off_t offset, int whence);
