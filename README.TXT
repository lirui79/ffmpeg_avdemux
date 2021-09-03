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
  ./avdemux_test  rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_0.h264 rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_1.h264

6. stop avdemux_test
  press  CTRL + C


7. boardcard test
  sudo bash 
  change user to root   smartcore

8. library path
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/decoder/lib/

9. start boardcard  30s

  /opt/decoder/driver/boot.sh

10. test start

  24 smartcard   30s

  ./avdemux_test  rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_0.h264 rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_1.h264

11. change config

    /opt/decoder/config/   vps.cfg   PT_H264=16 devices=1

12. test single card    16 router decoder  lost frame?