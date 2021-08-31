

all:libavdemux avdemux_test

libavdemux:avdemuxing.o
	 ar -rcs libavdemux.a avdemuxing.o

avdemuxing.o:avdemuxing.c
	gcc -c avdemuxing.c


avdemux_test:libavdemux
	gcc avdemux_test.c -L. -lva -lva-drm -lavformat -lavdevice -lavcodec -lavutil -lswresample -lpthread -ldl -lm -lavdemux  -o avdemux_test

clean:
	rm *.o *.so *_test
