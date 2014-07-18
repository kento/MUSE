/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall `pkg-config fuse --cflags --libs` fusexmp.c -o fusexmp
*/

#define FUSE_USE_VERSION 26

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite() */
#define _XOPEN_SOURCE 500
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

//For V3Mfuse: kent
#include <stdlib.h>
#include <strings.h>
#include <sys/fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

int   path_len=1024;
char* mountp;
char* apath;
//char* v3m_host;
//int v3m_port = 151515;
// end



static char*  mountpt(const char* path)
{
  strcpy(apath, mountp);
  strcat(apath, path);
  return apath;
  //  return connect_str(mountp, path);
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
	int res;

	//added: kent
	path = mountpt(path);
	// end
	res = lstat(path, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_access(const char *path, int mask)
{
	int res;

	//added: kent
	path = mountpt(path);
	// end

	res = access(path, mask);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size)
{
	int res;

	//added: kent
	path = mountpt(path);
	// end

	res = readlink(path, buf, size - 1);
	if (res == -1)
		return -errno;

	buf[res] = '\0';
	return 0;
}


static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	DIR *dp;
	struct dirent *de;
        
	(void) offset;
	(void) fi;

	//added: kent
	path = mountpt(path);
	// end

	dp = opendir(path);
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

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;

	//added: kent
	path = mountpt(path);
	// end

	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	if (S_ISREG(mode)) {
		res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(path, mode);
	else
		res = mknod(path, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	int res;

	//added: kent
	path = mountpt(path);
	// end

	res = mkdir(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_unlink(const char *path)
{
	int res;

	//added: kent
	path = mountpt(path);
	// end

	res = unlink(path);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rmdir(const char *path)
{
	int res;

	//added: kent
	path = mountpt(path);
	// end

	res = rmdir(path);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_symlink(const char *from, const char *to)
{
	int res;

	//added: kent	
	//	char afrom[256]=".";
	char ato[256]=".";
	//	strcat(afrom, from);

	strcat(ato, to);

	res = symlink(from, ato);
	// endcd

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
	int res;
	

	//added: kent
	char afrom[256]=".";
	char ato[256]=".";
	strcat(afrom, from);
	strcat(ato, to);
	res = rename(afrom, ato);
	// end

	if (res == -1)
	  return -errno;
	return 0;
}

static int xmp_link(const char *from, const char *to)
{
	int res;

	//added: kent	
	char afrom[256]=".";
	char ato[256]=".";
	strcat(afrom, from);
	strcat(ato, to);
	res = link(afrom, ato);
	// end

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	int res;

	//added: kent
	path = mountpt(path);
	// end

	res = chmod(path, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	int res;

	//added: kent
	path = mountpt(path);
	// end

	res = lchown(path, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
	int res;

	//added: kent
	path = mountpt(path);
	// end

	res = truncate(path, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	int res;
	struct timeval tv[2];

	//added: kent
	path = mountpt(path);
	// end

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(path, tv);
	if (res == -1)
		return -errno;

	return 0;
}



static int xmp_open(const char *path, struct fuse_file_info *fi)
{

	int res;
	path = mountpt(path);
	res = open(path, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
	return 0;
}





static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	int fd;
	int res;

	(void) fi;

	//added: kent
	path = mountpt(path);
	// end

	fd = open(path, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;
	//	printf("buf:%s\n",buf);
	printf("%d\t",fuse_get_context()->uid);
	printf("%d\t",fuse_get_context()->pid);
	printf("%s\t",path);
	printf("%d\t%d\t",(int)offset,(int)offset+res);
	printf("r\n");
	close(fd);
	return res;
}

static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	int fd;
	int res;

	(void) fi;

	//added: kent
	path = mountpt(path);
	// end
	fd = open(path, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;
	printf("%d\t",fuse_get_context()->uid);
	printf("%d\t",fuse_get_context()->pid);
	printf("%s\t",path);
	printf("%d\t%d\t",(int)offset,(int)offset+res);
	printf("w\n");
	close(fd);
	return res;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf)
{
	int res;

	//added: kent
	path = mountpt(path);
	// end

	res = statvfs(path, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_flush(const char *path, struct fuse_file_info *fi)
{
  int res;
  (void) path;
  //  printf("===FLUSH===: %s ===\n", path);      
  res = close(dup(fi->fh));
  if (res == -1)
    return -errno;
  // printf("===FLUSH===: %s ===\n", path);      
  return 0;
}



static int xmp_release(const char *path, struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */
        
	(void) path;
        // added: kent
	//        printf("===RELEASE===: %s ===\n", path);
	close(fi->fh);
	// end
	//        printf("===RELEASE===: %s ===\n", path);
	return 0;
}

static inline DIR *get_dirp(struct fuse_file_info *fi)
{
  return (DIR *) (uintptr_t) fi->fh;
}

static int xmp_releasedir(const char *path, struct fuse_file_info *fi)
{
  DIR *dp = get_dirp(fi);
  (void) path;
  //  printf("===RELEASEDIR===: %s ===\n", path);
  closedir(dp);
  //  printf("===RELEASEDIR===: %s ===\n", path);
  return 0;
}

static int xmp_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	(void) isdatasync;
	(void) fi;
	return 0;
}

#ifdef HAVE_SETXATTR
/* xattr operations are optional and can safely be left unimplemented */
static int xmp_setxattr(const char *path, const char *name, const char *value,
			size_t size, int flags)
{
  int res;
	//added: kent
	path = mountpt(path);
	// end

	res = lsetxattr(path, name, value, size, flags);
	if (res == -1)
		return -errno;
	return 0;
}

static int xmp_getxattr(const char *path, const char *name, char *value,
			size_t size)
{
  int res;
	//added: kent
	path = mountpt(path);
	// end
	res = lgetxattr(path, name, value, size);
	if (res == -1)
		return -errno;
	return res;
}

static int xmp_listxattr(const char *path, char *list, size_t size)
{
  int res;	
        //added: kent
	path = mountpt(path);
	// end
	res = llistxattr(path, list, size);
	if (res == -1)
		return -errno;
	return res;
}

static int xmp_removexattr(const char *path, const char *name)
{
  int res;
	//added: kent
	path = mountpt(path);
	// end
	res = lremovexattr(path, name);
	if (res == -1)
		return -errno;
	return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.access		= xmp_access,
	.readlink	= xmp_readlink,
	.readdir	= xmp_readdir,
	.mknod		= xmp_mknod,
	.mkdir		= xmp_mkdir,
	.symlink	= xmp_symlink,
	.unlink		= xmp_unlink,
	.rmdir		= xmp_rmdir,
	.rename		= xmp_rename,
	.link		= xmp_link,
	.chmod		= xmp_chmod,
	.chown		= xmp_chown,
	.truncate	= xmp_truncate,
	.utimens	= xmp_utimens,
	.open		= xmp_open,
	.read		= xmp_read,
	.write		= xmp_write,
	.statfs		= xmp_statfs,
        .flush          = xmp_flush,
	.release	= xmp_release,
	.releasedir     = xmp_releasedir,
	.fsync		= xmp_fsync,
#ifdef HAVE_SETXATTR
	.setxattr	= xmp_setxattr,
	.getxattr	= xmp_getxattr,
	.listxattr	= xmp_listxattr,
	.removexattr	= xmp_removexattr,
#endif

};

int main(int argc, char *argv[])
{
  //editor: kent

  int i;
  mountp = malloc(sizeof(char)*path_len);
  strcat(mountp,argv[1]);
  printf("mountpoint=%s\n",mountp);
  for (i=2; i < argc; i++) {
    argv[i-1] = argv[i];
  }
  argc--;
  apath = malloc(sizeof(char)*path_len);

  chdir(mountp);
  umask(0);
  return fuse_main(argc, argv, &xmp_oper, NULL);
  // end
}
