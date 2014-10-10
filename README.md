MUSE (Monitoring Filesystem in Userspace)
=========================================

The MUSE filesystem monitor open, close, read, write operations including read/write address, and the sizes.

Required software
------------
* FUSE(Filesystem in Userspace): http://fuse.sourceforge.net/ 
* fuse, fuse-dev


Quick Start
------------

    $ cd muse
    $ make
    $ mkdir muse_root_dir muse_mnt
    $ ./muse ./muse_log ./muse_root_dir/ ./muse_mnt/ 
    $ ./muse_test.sh ./muse_mnt
    $ cat ./muse_log
    $ ./muse_umount # OR sudo umount ./muse_mnt

![Alt text](https://bitbucket.org/sato5/muse/wiki/img/muse.png)


Log format: (pid = 0 when close)
--------------------------------

    <Time Stamp (seconds)> <Duration (seconds)> <pid> <Open(o),Close(c),Write(w),Read(r)> <File path> <Offset(bytes)> <Size(bytes)>

    1412899393.685865       0.000409        16061   o       muse_root_dir/muse_test1.tmp      0       0
    1412899393.686556       0.000333        16061   w       muse_root_dir/muse_test1.tmp      0       512
    1412899393.717921       0.000202        16061   w       muse_root_dir/muse_test1.tmp      512     512
    1412899393.718611       0.000152        16061   w       muse_root_dir/muse_test1.tmp      1024    512
    1412899393.719273       0.000150        16061   w       muse_root_dir/muse_test1.tmp      1536    512
    1412899393.723505       0.000002        0       c       muse_root_dir/muse_test1.tmp      0       0
    1412899393.724367       0.000179        16062   o       muse_root_dir/muse_test1.tmp      0       0
    1412899393.724575       0.000144        16062   r       muse_root_dir/muse_test1.tmp      0       2048
    1412899393.724607       0.000001        0       c       muse_root_dir/muse_test1.tmp      0       0
    1412899393.789527       0.000349        16064   o       muse_root_dir/muse_test2.tmp      0       0
    1412899393.791102       0.000396        16064   w       muse_root_dir/muse_test2.tmp      0       124
    1412899393.824437       0.000006        0       c       muse_root_dir/muse_test2.tmp      0       0
    1412899393.825117       0.000230        16065   o       muse_root_dir/muse_test2.tmp      0       0
    1412899393.825729       0.000200        16065   w       muse_root_dir/muse_test2.tmp      124     124
    1412899393.826246       0.000002        0       c       muse_root_dir/muse_test2.tmp      0       0
    1412899393.826604       0.000154        16066   o       muse_root_dir/muse_test2.tmp      0       0
    1412899393.827853       0.000274        16066   w       muse_root_dir/muse_test2.tmp      248     124
    1412899393.842867       0.000006        0       c       muse_root_dir/muse_test2.tmp      0       0
    1412899393.844400       0.000220        16067   o       muse_root_dir/muse_test2.tmp      0       0
    1412899393.844634       0.000162        16067   r       muse_root_dir/muse_test2.tmp      0       372
    1412899393.844692       0.000002        0       c       muse_root_dir/muse_test2.tmp      0       0

Usage
-----------

### Command
    usage: muse <log file> <root dir> <mount point> [Fuse options]


###  Fuse options
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

### Module options:

[subdir]

    -o subdir=DIR	    prepend this directory to all paths (mandatory)
    -o [no]rellinks	    transform absolute symlinks to relative

[iconv]

    -o from_code=CHARSET   original encoding of file names (default: UTF-8)
    -o to_code=CHARSET	    new encoding of the file names (default: ANSI_X3.4-1968)
    
    
