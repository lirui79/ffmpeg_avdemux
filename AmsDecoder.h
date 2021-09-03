#ifndef AMS_DECODER_H
#define AMS_DECODER_H

#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

struct AmsDecodedYUV{ 
	uint64_t	ID;
	uint32_t	width; 
	uint32_t	height;
    uint32_t    stride;
    uint32_t    index;
	uint32_t	closedFlag;		//1 代表会话结束，此时不含有解码数据
	uint32_t	yuvLen;   		//解码后的图像数据长度
	uint8_t     yuvData[0];	    //解码的图像数据yuv420p
} ;

struct AmsStreamPar{
	uint64_t ID;				// program id
	int      codecId;			//0: h264, 1: h265 
	uint32_t width;				// video frame width
	uint32_t height;			// video frame height
    uint32_t streamLen;         // video stream len
    uint8_t  *streamData;       // video stream data;
};

typedef struct AmsDecodedYUV AmsDecodedYUV;
typedef struct AmsStreamPar  AmsStreamPar;

typedef int (*AmsDecodeCallback)(AmsDecodedYUV *);

extern int AmsDecoderInit(int skipFrameNum, AmsDecodeCallback cb);

extern int AmsDecoderRelease();

extern int AmsDecodeStart(AmsStreamPar *par);

extern int AmsDecodeVideo(AmsStreamPar *par);

extern int AmsDecodeFreeFrame(AmsDecodedYUV *yuv);

extern int AmsDecodeStop(uint64_t ID);



#ifdef __cplusplus
}
#endif

#endif
