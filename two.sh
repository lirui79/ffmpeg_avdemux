#!/bin/bash

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/decoder/lib/

#start board card
/opt/decoder/driver/boot.sh

#./avdemux_test 5 4  rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp://admin:fiberhome025@192.17.1.72:554/



#CMD_PARAM  input_source url 
CMD_PARAM=(
" rtsp://admin:fiberhome025@192.17.1.72:554/ "
" rtsp://admin:fiberhome025@192.17.1.72:554/ "
)

# input_source url number
INUM=${#CMD_PARAM[@]}
echo $INUM
#INUM=4

#skipFrameNum
SKIPFRAMENUM=5

#decoder number for each input_source
DECNUM=4


ALL_CMD=" "$SKIPFRAMENUM
ALL_CMD=$ALL_CMD" "$DECNUM

for ((i=0; i < INUM; ++i))
do

ALL_CMD=$ALL_CMD${CMD_PARAM[i]}
done

echo $ALL_CMD

./avdemux_test $ALL_CMD
