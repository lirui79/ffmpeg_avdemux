#!/bin/bash

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/decoder/lib/

/opt/decoder/driver/boot.sh

#./avdemux_test 2 rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp://admin:fiberhome025@192.17.1.72:554/ 


CMD_PARAM=(
" rtsp://admin:fiberhome025@192.17.1.72:554/ "
" rtsp://admin:fiberhome025@192.17.1.72:554/ "
" rtsp://admin:fiberhome025@192.17.1.72:554/ "
" rtsp://admin:fiberhome025@192.17.1.72:554/ "
" rtsp://admin:fiberhome025@192.17.1.72:554/ "
" rtsp://admin:fiberhome025@192.17.1.72:554/ "
)

INUM=${#CMD_PARAM[@]}
echo $INUM

INUM=2

STEP=5

DNUM=8

ALL_CMD=" "$STEP
ALL_CMD=$ALL_CMD" "$DNUM

for ((i=0; i < INUM; ++i))
do

ALL_CMD=$ALL_CMD${CMD_PARAM[i]}
done

#echo $ALL_CMD

./avdemux_test $ALL_CMD
