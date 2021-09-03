#!/bin/bash

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/decoder/lib/

/opt/decoder/driver/boot.sh

#./avdemux_test  rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_0.h264 rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_1.h264


CMD_PARAM=(
" rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_0.h264 "
" rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_1.h264 "
" rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_2.h264 "
" rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_3.h264 "
" rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_4.h264 "
" rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_5.h264 "
" rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_6.h264 "
" rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_7.h264 "
" rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_8.h264 "
" rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_9.h264 "
" rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_10.h264 "
" rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_11.h264 "
" rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_12.h264 "
" rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_13.h264 "
" rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_14.h264 "
" rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_15.h264 "
)

INUM=${#CMD_PARAM[@]}
echo $INUM

INUM=2
ALL_CMD=" "
for ((i=0; i < INUM; ++i))
do

ALL_CMD=$ALL_CMD${CMD_PARAM[i]}
done

#echo $ALL_CMD

./avdemux_test $ALL_CMD
