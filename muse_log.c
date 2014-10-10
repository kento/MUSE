#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>

#include "muse_io.h"
#include "muse_err.h"

int muse_log_fd;
char muse_log_path[256];

double muse_get_time(void)
{
  double t;
  struct timeval tv;
  gettimeofday(&tv, NULL);
  t = ((double)(tv.tv_sec) + (double)(tv.tv_usec) * 0.001 * 0.001);
  return t;
}

void muse_log_open(char *path) {
  char log[1024];
  double time_stamp;

  muse_log_fd = muse_open(path, O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);
  strcpy(muse_log_path, path);
  if (muse_log_fd < 0) {
    muse_err("muse_log_open failed (%s:%s:%d)", __FILE__, __func__, __LINE__);
  }

  time_stamp = muse_get_time();
  sprintf(log, "####  MUSE start: %f ####\n", time_stamp);
  muse_write(muse_log_path, muse_log_fd, log, strlen(log));
}

void muse_log_write(int pid, char *io_path, size_t offset, size_t length, char *mode, double duration)
{
  char log[1024];
  double time_stamp;
  
  time_stamp = muse_get_time();
  sprintf(log, "%f\t%f\t%d\t%s\t%s\t%lu\t%lu\n", time_stamp, duration, pid, mode, io_path, offset, length);
  muse_write(muse_log_path, muse_log_fd, log, strlen(log));

  return;
}

void muse_log_close() 
{
  int ret;
  ret = muse_close(muse_log_path, muse_log_fd);
  if (muse_log_fd < 0) {
    muse_err("muse_log_close failed (%s:%s:%d)", __FILE__, __func__, __LINE__);
  }
}

