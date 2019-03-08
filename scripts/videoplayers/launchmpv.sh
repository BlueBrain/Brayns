#! /bin/bash
display=$1
eye=left
if [ $display == 0 ]
then
    eye=right
fi

PATH_TO_MPV=/nfs4/bbp.epfl.ch/user/nachbaur/mpv-build/mpv/build
DIRECTORY=`dirname $0`

DISPLAY=:0.$display $PATH_TO_MPV/mpv $DIRECTORY/`hostname -s`.$eye.sdp --no-cache --untimed --vd-lavc-threads=1 --profile=low-latency --geometry 0:0 --gamma-factor 1.0

