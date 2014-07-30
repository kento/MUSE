MUSE (Monitoring Filesystem in Userspace)
=========================================

Quick Start
------------

    $ cd muse
    $ make
    $ mkdir muse_root_dir muse_mnt
    $ ./muse ./muse_log ./muse_root_dir/ ./muse_mnt/ 
    $ cat ./muse_log


Log format: (pid = 0 when close)
--------------------------------

    <Time Stamp (seconds)> <pid> <Open(o),Close(c),Write(w),Read(r)> <File path> <Offset(bytes)> <Size(bytes)>

    1406690112.019573       21635   o       /root_dir/muse_test1.tmp  0       0
    1406690112.020497       21635   w       /root_dir/muse_test1.tmp  0       512
    1406690112.021343       21635   w       /root_dir/muse_test1.tmp  512     512
    1406690112.022277       21635   w       /root_dir/muse_test1.tmp  1024    512
    1406690112.023120       21635   w       /root_dir/muse_test1.tmp  1536    512
    1406690112.023753       0       c       /root_dir/muse_test1.tmp  0       0
    1406690112.026105       21636   o       /root_dir/muse_test1.tmp  0       0
    1406690112.026716       21636   r       /root_dir/muse_test1.tmp  0       2048
    1406690112.027104       0       c       /root_dir/muse_test1.tmp  0       0
    1406690112.030754       21634   o       /root_dir/muse_test2.tmp  0       0
    1406690112.033202       21638   w       /root_dir/muse_test2.tmp  0       124
    1406690112.034230       0       c       /root_dir/muse_test2.tmp  0       0
    1406690112.034678       21634   o       /root_dir/muse_test2.tmp  0       0
    1406690112.036899       21640   w       /root_dir/muse_test2.tmp  124     124
    1406690112.037982       0       c       /root_dir/muse_test2.tmp  0       0
    1406690112.038304       21634   o       /root_dir/muse_test2.tmp  0       0
    1406690112.040553       21641   w       /root_dir/muse_test2.tmp  248     124
    1406690112.041589       0       c       /root_dir/muse_test2.tmp  0       0
    1406690112.043288       21642   o       /root_dir/muse_test2.tmp  0       0
    1406690112.043920       21642   r       /root_dir/muse_test2.tmp  0       372
    1406690112.044190       0       c       /root_dir/muse_test2.tmp  0       0

Usage
-----------

# Command
usage: muse <log file> <root dir> <mount point> [Fuse options]


#  Fuse options
general options:

    -o opt,[opt...]        mount options
    -h   --help            print help
    -V   --version         print version

FUSE options:

    -d   -o debug          enable debug output (implies -f)
    -f                     foreground operation
    -s                     disable multi-threaded operation

    -o allow_other         allow access to other users
    -o allow_root          allow access to root
    -o nonempty            allow mounts over non-empty file/dir
    -o default_permissions enable permission checking by kernel
    -o fsname=NAME         set filesystem name
    -o subtype=NAME        set filesystem type
    -o large_read          issue large read requests (2.4 only)
    -o max_read=N          set maximum size of read requests

    -o hard_remove         immediate removal (don't hide files)
    -o use_ino             let filesystem set inode numbers
    -o readdir_ino         try to fill in d_ino in readdir
    -o direct_io           use direct I/O
    -o kernel_cache        cache files in kernel
    -o [no]auto_cache      enable caching based on modification times (off)
    -o umask=M             set file permissions (octal)
    -o uid=N               set file owner
    -o gid=N               set file group
    -o entry_timeout=T     cache timeout for names (1.0s)
    -o negative_timeout=T  cache timeout for deleted names (0.0s)
    -o attr_timeout=T      cache timeout for attributes (1.0s)
    -o ac_attr_timeout=T   auto cache timeout for attributes (attr_timeout)
    -o intr                allow requests to be interrupted
    -o intr_signal=NUM     signal to send on interrupt (10)
    -o modules=M1[:M2...]  names of modules to push onto filesystem stack

    -o max_write=N         set maximum size of write requests
    -o max_readahead=N     set maximum readahead
    -o async_read          perform reads asynchronously (default)
    -o sync_read           perform reads synchronously
    -o atomic_o_trunc      enable atomic open+truncate support
    -o big_writes          enable larger than 4kB writes
    -o no_remote_lock      disable remote file locking

Module options:

[subdir]

    -o subdir=DIR	    prepend this directory to all paths (mandatory)
    -o [no]rellinks	    transform absolute symlinks to relative

[iconv]

    -o from_code=CHARSET   original encoding of file names (default: UTF-8)
    -o to_code=CHARSET	    new encoding of the file names (default: ANSI_X3.4-1968)

