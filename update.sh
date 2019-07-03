#!/bin/bash

src="/home/zwz/ffmpeg-4.1.3/"
dst="/home/zwz/ffmpeg-foobar/ffmpeg-foobar/"

for i in `find $src -name "*.c"`;do
    diff=`diff ${i/$src/$dst} $i`
    if [ "$diff" != "" ];then
        echo ${i/$src/$dst} $i differ
        echo $diff
        #cp $i ${i/$src/$dst}
    fi
done

for i in `find $src -name "*.h"`;do
    diff=`diff ${i/$src/$dst} $i`
    if [ "$diff" != "" ];then
        echo ${i/$src/$dst} $i differ
        echo $diff
        #cp $i ${i/$src/$dst}
    fi
done
for i in `find $src -name "*.sh"`;do
    diff=`diff ${i/$src/$dst} $i`
    if [ "$diff" != "" ];then
        echo ${i/$src/$dst} $i differ
        echo $diff
        #cp $i ${i/$src/$dst}
    fi
done

