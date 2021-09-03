

all:libavdemux avdemux_test

libavdemux:avdemuxing.o
	 ar -rcs libavdemux.a avdemuxing.o

avdemuxing.o:avdemuxing.c
	gcc -c avdemuxing.c


avdemux_test:libavdemux
	gcc avdemux_test.c -L. -L/opt/decoder/lib/ -lvideodecoder_vps -lva -lva-drm -lavformat -lavdevice -lavcodec -lavutil -lswresample -lpthread -ldl -lm -lc -lavdemux  -o avdemux_test

clean:
	rm *.o *.so *_test
