#!/bin/sh

dd if=/dev/zero of=$1/muse_test1.tmp ibs=512 obs=512 count=4 2> /dev/zero # write: 512 bytes x 4
cat $1/muse_test1.tmp > /dev/zero # read: 512 bytes x4

for i in `seq 3`
do
dd if=/dev/zero ibs=31 count=4 >> $1/muse_test2.tmp 2> /dev/zero # write: (31 + 1) bytes x 4
done

tail -n 1 $1/muse_test2.tmp > /dev/zero # read: 32 bytes 

rm $1/muse_test1.tmp
rm $1/muse_test2.tmp
