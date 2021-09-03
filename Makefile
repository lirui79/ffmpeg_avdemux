
#LDFLAGS=-L. -L/opt/decoder/lib/ 

#LDFLAGS += -lfdk-aac -lvpx -lmp3lame -lx264 -logg -lvorbis -lx265 -lopus -lvorbisenc -lpostproc -lvorbisfile

#LDFLAGS += -lvideodecoder_vps -lva -lva-drm -lavformat -lavdevice -lavcodec -lavutil -lswresample -lpthread -ldl -lm -lc

all:avdemux_test

libffmpeg.a:libavdemux.a
	 cp /usr/local/lib/*.a ./ -rf
	 cp /home/ams/ffmpeg_build/lib/*.a ./ -rf
	 ar -x libavcodec.a
	 ar -x libavdevice.a
	 ar -x libavfilter.a
	 ar -x libavformat.a
	 ar -x libavutil.a
	 ar -x libswresample.a
	 ar -x libpostproc.a
	 ar -x libswscale.a
	 ar -x libyasm.a
	 ar -x libogg.a
	 ar -x libvorbisenc.a
	 ar -x libvpx.a
	 ar -x libfdk-aac.a
	 ar -x libmp3lame.a
	 ar -x libopus.a
	 ar -x libvorbis.a
	 ar -x libvorbisfile.a
	 ar -x libx264.a
	 ar -x libx265.a
	 ar -x libavdemux.a
	 ar -rcs  libffmpeg.a *.o
	 rm *.o -rf
#   gcc -shared *.o -o mylib.so
#	 ranlib libffmpeg.a


LIST  = >> cc.lst 2>&1

LIBRARY = libffmpeg.so

libffmpeg1.so:
	 cp /home/ams/ffmpeg_build/lib/*.a ./ -rf
	 cp /usr/local/lib/*.a ./ -rf
	 ar -x libavcodec.a
	 ar -x libavdevice.a
	 ar -x libavfilter.a
	 ar -x libavformat.a
	 ar -x libavutil.a
	 ar -x libswresample.a
	 ar -x libpostproc.a
	 ar -x libswscale.a
	 ar -x libyasm.a
	 ar -x libogg.a
	 ar -x libvorbisenc.a
	 ar -x libvpx.a
	 ar -x libx265.a
	 ar -x libfdk-aac.a
	 ar -x libmp3lame.a
	 ar -x libopus.a
	 ar -x libvorbis.a
	 ar -x libvorbisfile.a
	 ar -x libx264.a
	 gcc *.o -shared -fPIC -o libffmpeg.so
	 rm *.o -rf
   

$(LIBRARY):
	 cp /usr/local/lib/*.a ./ -rf
	 cp /home/ams/ffmpeg_build/lib/*.a ./ -rf
	 gcc  -shared -fPIC -o $@ -L. -Wl,--whole-archive -logg -lvorbisenc -lvpx -lx265 -lfdk-aac -lmp3lame -lopus -lvorbis -lvorbisfile -lx264 -lavcodec -lavdevice -lavfilter -lavformat -lavutil -lswresample -lpostproc -lswscale -lyasm -Wl,--no-whole-archive


#	 gcc -shared -fPIC libogg.a libvorbisenc.a libvpx.a libx265.a libfdk-aac.a libmp3lame.a libopus.a libvorbis.a libvorbisfile.a libx264.a libavcodec.a libavdevice.a libavfilter.a libavformat.a libavutil.a libswresample.a libpostproc.a  libswscale.a  libyasm.a -Wl,-soname -Wl,$@ -o $@


libavdemux.a:avdemuxing.c
	gcc -c avdemuxing.c
	ar -rcs libavdemux.a avdemuxing.o

libavdemux.so:libffmpeg.a avdemuxing.c
	g++ avdemuxing.c  -fPIC -shared -L./ -lffmpeg -o libavdemux.so

# libavdemux.a  -lx265
avdemux_test:
	 cp /usr/local/lib/*.a ./ -rf
	 cp /home/ams/ffmpeg_build/lib/*.a ./ -rf
	gcc -Wl,--no-as-needed avdemuxing.c avdemux_test.c -L. -L/opt/decoder/lib/ -lvideodecoder_vps -lva -lva-drm -lavformat -lavcodec -lavutil -lswscale -lswresample -lpostproc -lx264 -lvorbisenc -lvpx -lfdk-aac -lmp3lame -lopus -lvorbis -lvorbisfile -logg  -lm -lpthread -lrt -lz -llzma -lbz2 -ldl -o avdemux_test

clean:
	rm *.o *.so *.a *_test
