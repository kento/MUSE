#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <getopt.h>
#include <errno.h>
#include <fcntl.h>

#include "muse_io.h"
#include "muse_err.h"

#define MUSE_OPEN_TRIES (30)
#define MUSE_OPEN_USLEEP (100000)

int muse_open(const char* file, int flags, mode_t  mode)
{
  int fd = -1;
  if (mode) { 
    fd = open(file, flags, mode);
  } else {
    fd = open(file, flags, S_IRUSR | S_IWUSR);
  }

  if (fd < 0) {
    muse_dbg("Opening file: open(%s) errno=%d %m @ %s:%d",
	     file, errno, __FILE__, __LINE__
	     );

    /* try again */
    int tries = MUSE_OPEN_TRIES;
    while (tries && fd < 0) {
      usleep(MUSE_OPEN_USLEEP);
      if (mode) { 
        fd = open(file, flags, mode);
      } else {
        fd = open(file, flags, S_IRUSR | S_IWUSR);
      }
      tries--;
    }

    /* if we still don't have a valid file, consider it an error */
    if (fd < 0) {
      muse_err("Opening file: open(%s) errno=%d %m @ %s:%d",
	       file, errno, __FILE__, __LINE__
	       );
    }
  }

  return fd;
}


int muse_close(const char* file, int fd)
{
  /* fsync first */
  fsync(fd);

  /* now close the file */
  if (close(fd) != 0) {
    /* hit an error, print message */
    muse_err("Closing file descriptor %d for file %s: errno=%d %m @ %s:%d",
	     fd, file, errno, __FILE__, __LINE__
	     );
    return 1;
  }

  return 0;
}

ssize_t muse_write(const char* file, int fd, const void* buf, size_t size)
{
  ssize_t n = 0;
  int retries = 10;
  while (n < size)
    {
      ssize_t rc = write(fd, (char*) buf + n, size - n);
      if (rc > 0) {
	n += rc;
      } else if (rc == 0) {
	/* something bad happened, print an error and abort */
	muse_err("Error writing %s: write(%d, %x, %ld) returned 0 @ %s:%d",
		 file, fd, (char*) buf + n, size - n, __FILE__, __LINE__
		 );
	exit(1);
      } else { /* (rc < 0) */
	/* got an error, check whether it was serious */
	if (errno == EINTR || errno == EAGAIN) {
	  continue;
	}

	/* something worth printing an error about */
	retries--;
	if (retries) {
	  /* print an error and try again */
	  muse_err("Error writing %s: write(%d, %x, %ld) errno=%d %m @ %s:%d",
		   file, fd, (char*) buf + n, size - n, errno, __FILE__, __LINE__
		   );
	} else {
	  /* too many failed retries, give up */
	  muse_err("Giving up write to %s: write(%d, %x, %ld) errno=%d %m @ %s:%d",
		   file, fd, (char*) buf + n, size - n, errno, __FILE__, __LINE__
		   );
	  exit(1);
	}
      }
    }
  return n;
}


/* reliable read from file descriptor (retries, if necessary, until hard error) */
ssize_t muse_read(const char* file, int fd, void* buf, size_t size)
{
  ssize_t n = 0;
  int retries = 10;
  while (n < size)
    {
      int rc = read(fd, (char*) buf + n, size - n);
      if (rc  > 0) {
	n += rc;
      } else if (rc == 0) {
	/* EOF */
	return n;
      } else { /* (rc < 0) */
	/* got an error, check whether it was serious */
	if (errno == EINTR || errno == EAGAIN) {
	  continue;
	}

	/* something worth printing an error about */
	retries--;
	if (retries) {
	  /* print an error and try again */
	  muse_err("Error reading %s: read(%d, %x, %ld) errno=%d %m @ %s:%d",
		   file, fd, (char*) buf + n, size - n, errno, __FILE__, __LINE__
		   );
	} else {
	  /* too many failed retries, give up */
	  muse_err("Giving up read of %s: read(%d, %x, %ld) errno=%d %m @ %s:%d",
		   file, fd, (char*) buf + n, size - n, errno, __FILE__, __LINE__
		   );
	  exit(1);
	}
      }
    }
  return n;
}

off_t muse_lseek(int fd, off_t offset, int whence)
{
  int cur_offset;
  if ((cur_offset = lseek(fd, offset, whence)) == (off_t) -1) {
    muse_err("lseek failed: fd:%d offset:%jd, whence:%d  (%s:%s:%d)", fd, offset, whence,  __FILE__, __func__, __LINE__);
  }
  return cur_offset;
}
