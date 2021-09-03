#ifndef AMS_DECODER_H
#define AMS_DECODER_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" 
{
#endif

/*
 * 用于存放解码后YUV数据，通过回调函数返回给用户数据。
 * 内存分配：由硬件解码器分配内存空间；
 * 内存释放：由实际用户释放内存。
 */
struct AmsDecodedYUV{ 
	uint64_t	ID;             //视频会话ID
	uint32_t	width;          //解码后YUV实际宽度 
	uint32_t	height;         //解码后YUV实际高度
    uint32_t    stride;         //解码后返回数据的宽度
    uint32_t    index;          //解码后视频帧序号
	uint32_t	closedFlag;		//解码标志：1 代表会话结束，此时不含有解码数据
	uint32_t	yuvLen;   		//解码后的图像数据长度
	uint8_t     yuvData[0];	    //解码的图像数据yuv420p
} ;

/*
 * 用于传递实际的码流数据信息。
 * 内存分配：由实际用户释放内存;
 * 内存释放：由实际用户释放内存。
 */
struct AmsStreamPar{
	uint64_t ID;				// 视频会话ID
	int      codecId;			// 视频编码类型：0: h264, 1: h265 
	uint32_t width;				// 视频实际宽度 
	uint32_t height;			// 视频实际高度
    uint32_t streamLen;         // 当前一帧码流数据长度
    uint8_t  *streamData;       // 当前发送数据的缓冲区地址
};
typedef struct AmsDecodedYUV AmsDecodedYUV;
typedef struct AmsStreamPar  AmsStreamPar;
/**
 * 函数作用：
 * 解码后回调函数接口.
 * 
 * 参数定义：
 * @ AmsDecodedYUV:
 * 用于存放解码后的YUV数据，具体参数详见该结构的定义；
 * 用户调用AmsDecodeFreeFrame 接口释放该回掉函数传入的yuv参数。
 */
typedef int (*AmsDecodeCallback)(AmsDecodedYUV *);

/**
 * 函数作用：
 * 加速卡初始化接口
 * 
 * 参数定义：
 * @ skipFrameNum:
 *   设置跳帧间隔参数。
 * @ cb:
 *   用户自定义的回调函数接口地址
 * 
 * 返回值定义：
 * 返回值大于等于0：初始化成功；
 * 返回值小于0：初始化失败
 */
int AmsDecoderInit(int skipFrameNum, AmsDecodeCallback cb);

/*
 * 函数作用：
 * 加速卡释放资源接口
 * 
 * 返回值定义：
 * 返回值大于等于0：函数执行成功；
 * 返回值小于0，函数执行失败
 */
int AmsDecoderRelease();

/**
 * 函数作用：
 * 开启一路视频解码
 *
 * 参数定义：
 * @par:
 *  具体定义详见该结构体的定义
 *
 * 返回值定义：
 * 返回值大于等于0：函数执行成功；
 * 返回值小于0，函数执行失败
 */
int AmsDecodeStart(AmsStreamPar *par);

/**
 * 函数作用：
 * 发送一个视频帧做解码
 *
 * 参数定义：
 * @par:
 *  具体定义详见该结构体的定义
 *
 * 返回值定义：
 * 返回值大于等于0：函数执行成功；
 * 返回值小于0，函数执行失败
 */
int AmsDecodeVideo(AmsStreamPar *par);

/**
 * 函数作用：
 * 释放视频解码后的yuv数据内存
 *
 * 参数定义：
 * @yuv:
 *  具体定义详见该结构体的定义
 *
 * 返回值定义：
 * 返回值大于等于0：函数执行成功；
 * 返回值小于0，函数执行失败
 */
int AmsDecodeFreeFrame(AmsDecodedYUV *yuv);

/**
 * 函数作用：
 * 关闭一路视频解码
 *
 * 参数定义：
 * @ID:
 *  视频解码会话ID
 *
 * 返回值定义：
 * 返回值大于等于0：函数执行成功；
 * 返回值小于0，函数执行失败.
 */
int AmsDecodeStop(uint64_t ID);

#ifdef __cplusplus
}
#endif
#endif
