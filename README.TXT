1. download   ffmpeg-4.1.6.tar.xz

2. configure make  install

tar  vxf ffmpeg-4.1.6.tar.xz

cd  ffmpeg-4.1.6

./configure

make -j 32

make install

3. install  libva
yum search libva
yum install libva-devel.x86_64

4. make  libavdemux and  avdemux_test

5.  test  verify
./avdemux_test  rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_1.h264 rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_2.h264

6. stop avdemux_test
press  CTRL + C
