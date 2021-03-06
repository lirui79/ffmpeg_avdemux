
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <pthread.h>
#include "avdemuxing.h"
#include "AmsDecoder.h"



pthread_mutex_t g_mutex;
int g_exit = 0;// 0 - process run : > 0 - process will be exit

static void addexit(int exit) {
    pthread_mutex_lock(&g_mutex);
    g_exit += exit;//exit(0);
    pthread_mutex_unlock(&g_mutex);
}

static int getexit() {
    int exit = 0;
    pthread_mutex_lock(&g_mutex);
    exit = g_exit;//exit(0);
    pthread_mutex_unlock(&g_mutex);
    return exit;
}

static void sign_func(int sig) {  
    printf("Signal deal!\n");  
    printf("Signal was stop by ctrl+c!\n");
    addexit(1);
}

typedef struct demux_ctx {
	int         pid;// program id
  uint64_t    fid;// decode frame index
  FILE       *fyuv;// yuv save file
}demux_ctx;

static demux_ctx *g_demux_ctx = NULL;

static int DecodeCallback(AmsDecodedYUV *yuv){
    int fw = 0;
    demux_ctx *ctx = &g_demux_ctx[yuv->ID];
    if(ctx->fyuv != NULL){
        if(yuv->yuvLen > 0){
            fw = 1;
            fwrite(yuv->yuvData, 1, yuv->yuvLen, ctx->fyuv);
        }
    }
    ctx->fid++;
    printf("ID: %lu, w: %d, h: %d, stride: %d, index: %d, fid: %d, yuvLen: %d  %s\n",
        yuv->ID,
        yuv->width,
        yuv->height,
        yuv->stride,
        yuv->index,
        ctx->fid,
        yuv->yuvLen, fw == 1? "write Y":"write N");
    AmsDecodeFreeFrame(yuv);
    return 1;
}

// work thread param
typedef struct thread_param_ctx {
  char       *url;//Camera url
  FILE       *fs;// h264 stream save file
  int         index;//index thread
  pthread_t   tid;// thread id
  uint64_t    fid; // frame index
  int         sid; // demux ctx start idx
  int         num; // demux ctx number
}thread_param_ctx;

//Frame skip
static int skipFrameNum = 5;
// work thread function
// 
static void *demux_thread(void *arg) {
  AVPacket pkt;
  AVFormatContext *fmt_ctx = NULL;
  thread_param_ctx *ctx = (thread_param_ctx*) arg;
  demux_ctx *dctx = &g_demux_ctx[ctx->sid];
  int vsidx = -1, asidx = -1;
  int code = -1, index = 0, i = 0;
  AVStream *avstream = NULL;
  AVCodecParameters *avcodecpar = NULL;
  char outfname[32] = {0};
  AmsStreamPar *par = NULL;
  
  // open camera url
  code = avdemux_open_input(&fmt_ctx, ctx->url, NULL, NULL);
  if (code < 0) {
        fprintf(stderr, "thread:%x pid:[%d-%d] Could not open source file %s\n", ctx->tid, ctx->sid, ctx->num, ctx->url);
        return NULL;
  }
  
  printf("thread:%x pid:[%d-%d] avdemux_open_input:%s success\n", ctx->tid, ctx->sid, ctx->num, ctx->url);
  // find video stream
  vsidx = avdemux_find_streamidx(fmt_ctx, AVMEDIA_TYPE_VIDEO);
  if (vsidx < 0) {
        fprintf(stderr, "thread:%x pid:[%d-%d]  Could not find %s stream in input file '%s'\n", ctx->tid, ctx->sid, ctx->num,
                av_get_media_type_string(AVMEDIA_TYPE_VIDEO),  ctx->url);
        goto end;
  }

  avstream = fmt_ctx->streams[vsidx];
  avcodecpar = avstream->codecpar;
  printf("thread:%x pid:[%d-%d]  avdemux_find_streamidx:%d width:%d height:%d success\n", ctx->tid, ctx->sid, ctx->num, vsidx, avcodecpar->width, avcodecpar->height);
  // open stream file
  sprintf(outfname, "rtsp_%d.h264", ctx->index);

  ctx->fs = fopen(outfname, "wb");
  if (!ctx->fs) {
      fprintf(stderr, "thread:%x pid:[%d-%d] Could not open destination flow file %s\n", ctx->tid, ctx->sid, ctx->num, outfname);
      goto end;
  }

// init decoder ctx param
  for (i = 0;i < ctx->num; ++i) {
    dctx[i].pid = ctx->sid + i;
    dctx[i].fid = 0;
    
    sprintf(outfname, "out_%d_%d_%d.yuv", dctx[i].pid, avcodecpar->width, avcodecpar->height);
    dctx[i].fyuv = fopen(outfname, "wb");
    if (!dctx[i].fyuv) {
        fprintf(stderr, "thread:%x pid:[%d-%d]  Could not open destination yuv file %s\n", ctx->tid, ctx->sid, ctx->num, outfname);
        goto end;
    }
    printf("thread:%x pid:[%d-%d]  open:%s success\n", ctx->tid, ctx->sid, ctx->num, outfname);
  }

  av_init_packet(&pkt);
  pkt.data = NULL;
  pkt.size = 0;

// start board card stream decode
  par = (AmsStreamPar*)malloc(sizeof(AmsStreamPar));
  par->ID = ctx->sid;
  par->codecId = 0;
  par->width = avcodecpar->width;
  par->height = avcodecpar->height;
  par->streamLen = 0;
  par->streamData = NULL;
  for (i = 0;i < ctx->num; ++i) {
    par->ID = dctx[i].pid;
    code = AmsDecodeStart(par);
    if(code < 0){
        printf("thread:%x pid:[%d-%d] error when start one decode\n", ctx->tid, ctx->sid, ctx->num);
        goto end;
    }
  }

  while(1) {
     if (getexit() > 0) {
        printf("press ctrl + c, thread:%x pid:[%d-%d] will be exit\n", ctx->tid, ctx->sid, ctx->num);
        break;
     }

//   recv from camera stream data
     code = avdemux_read_frame(fmt_ctx, &pkt, vsidx);
     printf("thread:%x pid:[%d-%d] avdemux_read_frame index:%d  return: %d ", ctx->tid, ctx->sid, ctx->num, ctx->fid, code);
     if (code < 0) {
        break;
     }
     
     printf(" size:%d\n", pkt.size);
     if (code == 1) {
        av_packet_unref(&pkt);
        continue;
     }

//    send stream frame to board card decoder
      par->streamLen = pkt.size;
      par->streamData = pkt.data;
      code = 0;
      for (i = 0; i < ctx->num; ++i) {
        par->ID = dctx[i].pid;
        code += AmsDecodeVideo(par);
        printf("thread:%x pid:%d send data to dps board, return: %d\n", ctx->tid, dctx[i].pid, code);
      }
      fwrite(pkt.data, 1, pkt.size, ctx->fs);
      av_packet_unref(&pkt);
      ++ctx->fid;
      if(code < 0){
          break;
      }
  }

end:
    printf("thread:%x pid:[%d-%d] send all data to board\n", ctx->tid, ctx->sid, ctx->num);
    
    for (i = 0; i < ctx->num; ++i) {
        par->ID = dctx[i].pid;
        code = AmsDecodeStop(par->ID);
    }
  
  // wait decoder over
    while(1) {
       code = 0;
       for (i = 0; i < ctx->num; ++i) {
         printf("thread:%x pid:%d wait decode:[%d  %d]\n", ctx->tid, dctx[i].pid, ctx->fid, dctx[i].fid);
         if (((dctx[i].fid + 1) * (skipFrameNum + 1)) < ctx->fid )
             continue;
         ++code;
       }
       if (code >= ctx->num)
           break;
       sleep(1);
    }
  
    if(par)
       free(par);

    avformat_close_input(&fmt_ctx);
    if (ctx->fs)
        fclose(ctx->fs);
    for (i = 0; i < ctx->num; ++i) {
      if (dctx[i].fyuv)
          fclose(dctx[i].fyuv);
    }

    addexit(1);
    printf("press ctrl + c, thread:%x pid:[%d-%d] is exit\n", ctx->tid, ctx->sid, ctx->num);
    return NULL;
}
  

// ./avdemux_test skipFrameNum decnum url_1 url_2 url_3
// ./avdemux_test 5 4  rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp://admin:fiberhome025@192.17.1.72:554/
int main (int argc, char **argv) {
    thread_param_ctx *tp_ctx = NULL;
    int nthreads = argc - 3, i =0, ret = 0;
    int decnum = 0;
    pthread_mutex_init(&g_mutex, NULL);
    signal(SIGINT, sign_func);
    skipFrameNum = strtol(argv[1], NULL, 0);
    decnum = strtol(argv[2], NULL, 0);
    
    tp_ctx = (thread_param_ctx*)malloc(nthreads * sizeof(thread_param_ctx));
    g_demux_ctx = (demux_ctx*)malloc((decnum * nthreads) * sizeof(demux_ctx));

    printf("process will be execute, work thread num:%d decoder num:%d skipFrameNum:%d \n", nthreads, decnum, skipFrameNum);
    
    ret = AmsDecoderInit(skipFrameNum, DecodeCallback);
    if(ret < 0){
        printf("error when init ams decoder\n");
        return -1;
    }

    for (i = 0; i < nthreads; ++i) {
        //sprintf(tp_ctx[i].url, "%s",argv[i + 2]);
        tp_ctx[i].url = argv[i + 3];
        tp_ctx[i].index = i;
        tp_ctx[i].fid = 0;
        tp_ctx[i].sid = decnum * i;
        tp_ctx[i].num = decnum;                    
        tp_ctx[i].fs = NULL;
        
        pthread_create(&(tp_ctx[i].tid), NULL, demux_thread, &tp_ctx[i]);
    }
    
    // main control thread
    while(1) {
        if (getexit() > 0) {
            printf("press ctrl + c, process will be exit\n");
            break;
        }
         
        printf("threads:%d is running and execute tasks\n", nthreads);
        sleep(5);
    }

    // wait work thread exit
    while(1) {
        if (getexit() > nthreads) {
            printf("wait for thread num:%d exit over\n", nthreads);
            break;
        }
        printf("wait for thread num:%d exit\n", nthreads);
        sleep(1);    
    }
    
    printf("process will releas board card decoder\n");
    sleep(5);
    ret = AmsDecoderRelease();
    if(ret < 0){
        printf("error when release ams decoder\n");
    }

    free(g_demux_ctx);
    free(tp_ctx);
    pthread_mutex_destroy(&g_mutex);
    printf("process will be exit\n");
    return 0;
}

