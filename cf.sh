#!/bin/bash


export PATH="$HOME/bin:$PATH"

export PKG_CONFIG_PATH="$HOME/ffmpeg_build/lib/pkgconfig:/usr/local/freetype/lib/pkgconfig:$PKG_CONFIG_PATH" 

./configure \
  --pkg-config-flags="--static" \
  --extra-cflags="-I$HOME/ffmpeg_build/include -I/usr/local/cuda/include" \
  --extra-ldflags="-L$HOME/ffmpeg_build/lib -L/usr/local/cuda/lib64" \
  --extra-libs=-lpthread \
  --extra-libs=-lm \
  --enable-gpl \
  --enable-libfdk_aac \
  --enable-libfreetype \
  --enable-libmp3lame \
  --enable-libopus \
  --enable-libvorbis \
  --enable-libvpx \
  --enable-libx264 \
  --enable-nonfree \
  --enable-cuda \
  --enable-cuvid \
  --enable-nvenc \
  --enable-libnpp
  
  
  
#  --prefix="$HOME/ffmpeg_build" \
#  --bindir="$HOME/bin" \
#  --enable-libx265 \