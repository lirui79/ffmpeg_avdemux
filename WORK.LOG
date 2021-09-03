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


13. cuda  CUDA Toolkit 10.2
    gpu  ffmpeg
    
    https://developer.download.nvidia.cn/compute/cuda/repos/rhel7/x86_64/
    http://dl.fedoraproject.org/pub/epel/7/x86_64/Packages/o/
    https://download-ib01.fedoraproject.org/pub/epel/7/aarch64/Packages/d/
    https://www.videolan.org/developers/
    
    wget https://download-ib01.fedoraproject.org/pub/epel/7/aarch64/Packages/d/dkms-2.7.1-1.el7.noarch.rpm
    sudo  yum install elfutils-libelf-devel
    sudo rpm -Uvh dkms-2.7.1-1.el7.noarch.rpm

    wget https://developer.download.nvidia.com/compute/cuda/10.2/Prod/local_installers/cuda-repo-rhel7-10-2-local-10.2.89-440.33.01-1.0-1.x86_64.rpm
    sudo rpm -i cuda-repo-rhel7-10-2-local-10.2.89-440.33.01-1.0-1.x86_64.rpm

    wget http://dl.fedoraproject.org/pub/epel/7/x86_64/Packages/o/ocl-icd-2.2.12-1.el7.x86_64.rpm
    sudo rpm -Uvh ocl-icd-2.2.12-1.el7.x86_64.rpm
    
    wget http://dl.fedoraproject.org/pub/epel/7/x86_64/Packages/o/opencl-filesystem-1.0-5.el7.noarch.rpm
    
    sudo rpm -Uvh opencl-filesystem-1.0-5.el7.noarch.rpm
    
    sudo yum -y install nvidia-driver-latest-dkms cuda
    
    sudo yum -y install cuda-drivers
    
    sudo yum install hg
    sudo yum install cmake

    
    curl -O -L http://www.nasm.us/pub/nasm/releasebuilds/2.13.02/nasm-2.13.02.tar.bz2
    tar xjvf nasm-2.13.02.tar.bz2
    cd nasm-2.13.02
    ./autogen.sh
    ./configure --prefix="$HOME/ffmpeg_build" --bindir="$HOME/bin"
    make
    make install
    
    git clone https://git.videolan.org/git/ffmpeg/nv-codec-headers.git
    make PREFIX="$HOME/ffmpeg_build" BINDDIR="$HOME/bin"
    make install PREFIX="$HOME/ffmpeg_build" BINDDIR="$HOME/bin"
    
    git clone --depth 1 --branch v0.1.6 https://github.com/mstorsjo/fdk-aac.git
    cd fdk-aac
    autoreconf -fiv
    ./configure --prefix="$HOME/ffmpeg_build" --disable-shared
    make
    make install
  
  
  
    wgit  http://mirror.yongbok.net/nongnu/freetype/freetype-2.8.1.tar.gz
    tar xzvf freetype-2.8.1.tar.gz   
    ./configure --prefix="$HOME/ffmpeg_build" && make && make install
    
    
    curl -O -L http://downloads.sourceforge.net/project/lame/lame/3.100/lame-3.100.tar.gz
    tar xzvf lame-3.100.tar.gz
    cd lame-3.100
    ./configure --prefix="$HOME/ffmpeg_build" --bindir="$HOME/bin" --disable-shared --enable-nasm
    make
    make install
    
    
    
    curl -O -L https://archive.mozilla.org/pub/opus/opus-1.2.1.tar.gz
    tar xzvf opus-1.2.1.tar.gz
    cd opus-1.2.1
    ./configure --prefix="$HOME/ffmpeg_build" --disable-shared
    make
    make install
    
    
    
        
    curl -O -L http://downloads.xiph.org/releases/ogg/libogg-1.3.3.tar.gz
    tar xzvf libogg-1.3.3.tar.gz
    cd libogg-1.3.3
    ./configure --prefix="$HOME/ffmpeg_build" --disable-shared
    make
    make install
    
   
    curl -O -L http://downloads.xiph.org/releases/vorbis/libvorbis-1.3.5.tar.gz
    tar xzvf libvorbis-1.3.5.tar.gz
    cd libvorbis-1.3.5
    ./configure --prefix="$HOME/ffmpeg_build" --with-ogg="$HOME/ffmpeg_build" --disable-shared
    make
    make install
   
   
    git clone --depth 1 https://github.com/webmproject/libvpx.git
    cd libvpx
    ./configure --prefix="$HOME/ffmpeg_build" --disable-examples --disable-unit-tests --enable-vp9-highbitdepth --as=yasm
    make
    make install
    
    git clone https://code.videolan.org/videolan/x264.git
    cd x264
    PKG_CONFIG_PATH="$HOME/ffmpeg_build/lib/pkgconfig" ./configure --prefix="$HOME/ffmpeg_build" --bindir="$HOME/bin" --enable-static
    make
    make install
    
     hg clone http://hg.videolan.org/x265
     cd x265/build/linux
      cmake -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX="$HOME/ffmpeg_build" -DENABLE_SHARED:bool=off ../../source
      make
      make install
    
    
    ffmpeg comile  cf.sh
    
export PATH="$HOME/bin:$PATH" 
export PKG_CONFIG_PATH="$HOME/ffmpeg_build/lib/pkgconfig:/usr/local/freetype/lib/pkgconfig:$PKG_CONFIG_PATH" 
./configure \
  --prefix="$HOME/ffmpeg_build" \
  --pkg-config-flags="--static" \
  --extra-cflags="-I$HOME/ffmpeg_build/include -I/usr/local/cuda/include" \
  --extra-ldflags="-L$HOME/ffmpeg_build/lib -L/usr/local/cuda/lib64" \
  --extra-libs=-lpthread \
  --extra-libs=-lm \
  --bindir="$HOME/bin" \
  --enable-gpl \
  --enable-libfdk_aac \
  --enable-libfreetype \
  --enable-libmp3lame \
  --enable-libopus \
  --enable-libvorbis \
  --enable-libvpx \
  --enable-libx264 \
  --enable-libx265 \
  --enable-nonfree \
  --enable-cuda \
  --enable-cuvid \
  --enable-nvenc \
  --enable-libnpp






