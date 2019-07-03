#!/bin/bash

echo trigger!
pid=`ps -aux|grep ffmpeg|grep foobar|grep -v grep|awk '{print $2}'`
kill -20 $pid
