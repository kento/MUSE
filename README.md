MUSE (Monitoring Filesystem in Userspace)
=========================================



usage: ./muse mountpoint [options]

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


Multi-level Asynchronous Checkpoint/Restart Model (MACR)
====================================
This code simulate Multi-level Checkpoint/Restart

MACR APIs & Variables
-----------------------
## APIs

### X state computation
    tuple computeWState(L, V, T, ckptOverheadTimes, ckptRestartTimes, failRates)
This function compute X state based on given a configuration. Current function supports only 2-level case    

* `L` [input]: Maximal checkpoint level 
    * Example: 2
* `V` [input]: Array of checkpoint intervals [Level 1 count, Level 2 count (=1)] 
    * Example: [4, 1]
* `T` [intput]: Lowest level checkpoint interval
    * Example: 60
* `ckptOverheadTimes` [input]: Array of checkpoint overhead time [Level 1 checkpoint overhead, Level 2 checkpoint overhead] 
    * Example: [5.0, 100.0])   
* `ckptRestartTimes` [input]: Array of restart times [Level 1 restart, Level 2 restart] 
    * Example:  [10.0, 100.0]
* `failRates` [input]: Array of failure rates [Level 1 failure rate, Level 2 failure rate] 
    * Example:  [2.13e-06, 4.27e-07]
* `tuple` [output]: (Expected runtime, Efficiency).
    * Example: 420.122295693 0.714077788957
    
![Alt text](https://bitbucket.org/sato5/mcr_model/wiki/img/computeXState.png)
![Alt text](https://bitbucket.org/sato5/mcr_model/wiki/img/optimizeXState.png)

### W state computation
    tuple computeWState(L, V, T, ckptOverheadTimes, ckptL2Latency, ckptRestartTimes, failRates, alpha)
This function compute W state based on given a configuration. Current function supports only 2-level case    

* `L` [input]: Maximal checkpoint level 
    * Example: 2
* `V` [input]: Array of checkpoint intervals [Level 1 count, Level 2 count (=1)] 
    * Example: [4, 1]
* `T` [intput]: Lowest level checkpoint interval
    * Example: 60
* `ckptOverheadTimes` [input]: Array of checkpoint overhead time [Level 1 checkpoint overhead, Level 2 checkpoint overhead] 
    * Example: [5.0, 5.0])   
* `ckptL2Latency` [input]: Level 2 checkpoint latency time 
    * Example: 100.0
* `ckptRestartTimes` [input]: Array of restart times [Level 1 restart, Level 2 restart] 
    * Example:  [10.0, 100.0]
* `failRates` [input]: Array of failure rates [Level 1 failure rate, Level 2 failure rate] 
    * Example:  [2.13e-06, 4.27e-07]
* `alpha` [input]: Overhead factor incurred by Asynchronous L2 checkpointing (Overhead ratio to computation) 
    * Example: 0.01
* `tuple` [output]: (Expected runtime, Efficiency, Segment A count). If no answer, returns (inf, 0, None).
    * Example: 326.887714958 0.917746327783 2.0
    
![Alt text](https://bitbucket.org/sato5/mcr_model/wiki/img/computeWState.png)
![Alt text](https://bitbucket.org/sato5/mcr_model/wiki/img/optimizeWState.png)

### W state simulation
    tuple computeWState(L, V, T, ckptOverheadTimes, ckptL2Latency, ckptRestartTimes, failRates, alpha)
This function simulate W state based on given a configuration. Current function supports only 2-level case    

* `L` [input]: Maximal checkpoint level 
    * Example: 2
* `V` [input]: Array of checkpoint intervals [Level 1 count, Level 2 count (=1)] 
    * Example: [4, 1]
* `T` [intput]: Lowest level checkpoint interval
    * Example: 60
* `ckptOverheadTimes` [input]: Array of checkpoint overhead time [Level 1 checkpoint overhead, Level 2 checkpoint overhead] 
    * Example: [5.0, 5.0])   
* `ckptL2Latency` [input]: Level 2 checkpoint latency time 
    * Example: 100.0
* `ckptRestartTimes` [input]: Array of restart times [Level 1 restart, Level 2 restart] 
    * Example:  [10.0, 100.0]
* `failRates` [input]: Array of failure rates [Level 1 failure rate, Level 2 failure rate] 
    * Example:  [2.13e-06, 4.27e-07]
* `alpha` [input]: Overhead factor incurred by Asynchronous L2 checkpointing (Overhead ratio to computation) 
    * Example: 0.01
* `tuple` [output]: (Expected runtime, Efficiency, Segment A count). If no answer, returns (inf, 0, None).
    * Example: 326.887714958 0.917746327783 2.0 (the result varies each time because the simulation is not deterministic)

![Alt text](https://bitbucket.org/sato5/mcr_model/wiki/img/simulateWState-example.png)
![Alt text](https://bitbucket.org/sato5/mcr_model/wiki/img/simulateWState.png)
