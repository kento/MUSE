/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>
  Copyright (C) 2011       Sebastian Pipping <sebastian@pipping.org>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall fusexmp.c `pkg-config fuse --cflags --libs` -o fusexmp
*/

#define FUSE_USE_VERSION 26

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite()/utimensat() */
#define _XOPEN_SOURCE 700
#endif

#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

#include "muse_err.h"
#include "muse_log.h"

char mount_point[256];

static int muse_convert_to_access_path(char *apath, const char *path)
{
  sprintf(apath, "%s%s", mount_point, path);
  fprintf(stderr, "Mount Path: %s\n", apath);
  return 0;
}

static int muse_getattr(const char *path, struct stat *stbuf)
{
	int res;
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);
	res = lstat(access_path, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int muse_access(const char *path, int mask)
{
	int res;
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);
	res = access(access_path, mask);
	if (res == -1)
		return -errno;

	return 0;
}

static int muse_readlink(const char *path, char *buf, size_t size)
{
	int res;
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);
	res = readlink(access_path, buf, size - 1);
	if (res == -1)
		return -errno;

	buf[res] = '\0';
	return 0;
}


static int muse_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);

	dp = opendir(access_path);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, 0))
			break;
	}

	closedir(dp);
	return 0;
}

static int muse_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);

	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	if (S_ISREG(mode)) {
		res = open(access_path, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(access_path, mode);
	else
		res = mknod(access_path, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

static int muse_mkdir(const char *path, mode_t mode)
{
	int res;
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);
	res = mkdir(access_path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int muse_unlink(const char *path)
{
	int res;
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);
	res = unlink(access_path);
	if (res == -1)
		return -errno;

	return 0;
}

static int muse_rmdir(const char *path)
{
	int res;
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);
	res = rmdir(access_path);
	if (res == -1)
		return -errno;

	return 0;
}

static int muse_symlink(const char *from, const char *to)
{
	int res;

	char ato[256] = ".";
	strcat(ato, to);

	res = symlink(from, ato);
	if (res == -1)
		return -errno;

	return 0;
}

static int muse_rename(const char *from, const char *to)
{
	int res;

	char afrom[256]=".";
	char ato[256]=".";
	strcat(afrom, from);
	strcat(ato, to);

	res = rename(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int muse_link(const char *from, const char *to)
{
	int res;

	char afrom[256]=".";
	char ato[256]=".";
	strcat(afrom, from);
	strcat(ato, to);

	res = link(afrom, ato);
	if (res == -1)
		return -errno;

	return 0;
}

static int muse_chmod(const char *path, mode_t mode)
{
	int res;
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);

	res = chmod(access_path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int muse_chown(const char *path, uid_t uid, gid_t gid)
{
	int res;
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);
	res = lchown(access_path, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

static int muse_truncate(const char *path, off_t size)
{
	int res;
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);
	res = truncate(access_path, size);
	if (res == -1)
		return -errno;

	return 0;
}

#ifdef HAVE_UTIMENSAT
static int muse_utimens(const char *path, const struct timespec ts[2])
{
	int res;
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);
	/* don't use utime/utimes since they follow symlinks */
	res = utimensat(0, access_path, ts, AT_SYMLINK_NOFOLLOW);
	if (res == -1)
		return -errno;

	return 0;
}
#endif

static int muse_open(const char *path, struct fuse_file_info *fi)
{
	int res;
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);
	
	res = open(access_path, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
	return 0;
}

static int muse_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	int fd;
	int res;

	(void) fi;
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);
	fd = open(access_path, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	muse_log_write(fuse_get_context()->pid, access_path, offset, res, "r");

	close(fd);
	return res;
}

static int muse_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;

	(void) fi;
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);
	fd = open(access_path, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	muse_log_write(fuse_get_context()->pid, access_path, offset, res, "w");

	close(fd);
	return res;
}

static int muse_statfs(const char *path, struct statvfs *stbuf)
{
	int res;
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);
	res = statvfs(access_path, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int muse_release(const char *path, struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	(void) fi;
	return 0;
}

static int muse_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	(void) isdatasync;
	(void) fi;
	return 0;
}

#ifdef HAVE_POSIX_FALLOCATE
static int muse_fallocate(const char *path, int mode,
			off_t offset, off_t length, struct fuse_file_info *fi)
{
	int fd;
	int res;

	(void) fi;
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);
	if (mode)
		return -EOPNOTSUPP;

	fd = open(access_path, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = -posix_fallocate(fd, offset, length);

	close(fd);
	return res;
}
#endif

#ifdef HAVE_SETXATTR
/* xattr operations are optional and can safely be left unimplemented */
static int muse_setxattr(const char *path, const char *name, const char *value,
			size_t size, int flags)
{
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);
	int res = lsetxattr(access_path, name, value, size, flags);
	if (res == -1)
		return -errno;
	return 0;
}

static int muse_getxattr(const char *path, const char *name, char *value,
			size_t size)
{
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);
	int res = lgetxattr(access_path, name, value, size);
	if (res == -1)
		return -errno;
	return res;
}

static int muse_listxattr(const char *path, char *list, size_t size)
{
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);
	int res = llistxattr(access_path, list, size);
	if (res == -1)
		return -errno;
	return res;
}

static int muse_removexattr(const char *path, const char *name)
{
	char access_path[256];
	
	muse_convert_to_access_path(access_path, path);
	int res = lremovexattr(access_path, name);
	if (res == -1)
		return -errno;
	return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations muse_oper = {
	.getattr	= muse_getattr,
	.access		= muse_access,
	.readlink	= muse_readlink,
	.readdir	= muse_readdir,
	.mknod		= muse_mknod,
	.mkdir		= muse_mkdir,
	.symlink	= muse_symlink,
	.unlink		= muse_unlink,
	.rmdir		= muse_rmdir,
	.rename		= muse_rename,
	.link		= muse_link,
	.chmod		= muse_chmod,
	.chown		= muse_chown,
	.truncate	= muse_truncate,
#ifdef HAVE_UTIMENSAT
	.utimens	= muse_utimens,
#endif
	.open		= muse_open,
	.read		= muse_read,
	.write		= muse_write,
	.statfs		= muse_statfs,
	.release	= muse_release,
	.fsync		= muse_fsync,
#ifdef HAVE_POSIX_FALLOCATE
	.fallocate	= muse_fallocate,
#endif
#ifdef HAVE_SETXATTR
	.setxattr	= muse_setxattr,
	.getxattr	= muse_getxattr,
	.listxattr	= muse_listxattr,
	.removexattr	= muse_removexattr,
#endif
};

int main(int argc, char *argv[])
{
  int i;
  char *mount;

  muse_err_init(0);
  muse_log_open("./log");
  
  mount = argv[1];
  if (argc == 1) {
    fprintf(stderr, "use 'muse -h' for help\n");
    exit(1);
  }
  
  if (realpath(mount, mount_point) != NULL ){
    fprintf(stderr, "mount_point=%s\n", mount_point);
  }

  for (i = 2; i < argc; i++) {
    argv[i - 1] = argv[i];
  }
  argc--;

  umask(0);
  return fuse_main(argc, argv, &muse_oper, NULL);
}
