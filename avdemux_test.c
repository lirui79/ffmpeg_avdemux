
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include "avdemuxing.h"
#include "AmsDecoder.h"



pthread_mutex_t g_mutex;
int g_exit = 0;

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
  const char *url;
  const char *name;
	int         pid;				// program id
  pthread_t   tid;
  uint64_t    fid; // frame index
  FILE       *fyuv;
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

static void *demux_thread(void *arg) {
  AVPacket pkt;
  AVFormatContext *fmt_ctx = NULL;
  demux_ctx *ctx = (demux_ctx*) arg;
  int vsidx = -1, asidx = -1;
  int code = -1, index = 0;
  FILE *fp = NULL;
  AVStream *avstream = NULL;
  AVCodecParameters *avcodecpar = NULL;
  char outfname[32] = {0};
  AmsStreamPar *par = NULL;
  
  code = avdemux_open_input(&fmt_ctx, ctx->url, NULL, NULL);
  if (code < 0) {
        fprintf(stderr, "thread:%x pid:%d Could not open source file %s\n", ctx->tid, ctx->pid, ctx->url);
        return NULL;
  }
  
  printf("thread:%x pid:%d avdemux_open_input:%s success\n", ctx->tid, ctx->pid, ctx->url);
  
  vsidx = avdemux_find_streamidx(fmt_ctx, AVMEDIA_TYPE_VIDEO);
  if (vsidx < 0) {
        fprintf(stderr, "thread:%x pid:%d Could not find %s stream in input file '%s'\n", ctx->tid, ctx->pid,
                av_get_media_type_string(AVMEDIA_TYPE_VIDEO),  ctx->url);
        goto end;
  }
  avstream = fmt_ctx->streams[vsidx];
  avcodecpar = avstream->codecpar;
  printf("thread:%x pid:%d avdemux_find_streamidx:%d width:%d height:%d success\n", ctx->tid, ctx->pid, vsidx, avcodecpar->width, avcodecpar->height);
  
  sprintf(outfname, "out_%d_%d_%d.yuv", ctx->pid, avcodecpar->width, avcodecpar->height);

  fp = fopen(ctx->name, "wb");
  if (!fp) {
      fprintf(stderr, "thread:%x pid:%d Could not open destination flow file %s\n", ctx->tid, ctx->pid, ctx->name);
      goto end;
  }
  ctx->fyuv = fopen(outfname, "wb");
  if (!ctx->fyuv) {
      fprintf(stderr, "thread:%x pid:%d Could not open destination yuv file %s\n", ctx->tid, ctx->pid, outfname);
      goto end;
  }

  printf("thread:%x pid:%d open:%s success\n", ctx->tid, ctx->pid, ctx->name);
  av_init_packet(&pkt);
  pkt.data = NULL;
  pkt.size = 0;

  par = malloc(sizeof(AmsStreamPar));
  par->ID = ctx->pid;
  par->codecId = 0;
  par->width = avcodecpar->width;
  par->height = avcodecpar->height;
  par->streamLen = 0;
  par->streamData = NULL;
  code = AmsDecodeStart(par);
  if(code < 0){
      printf("thread:%x pid:%d error when start one decode\n", ctx->tid, ctx->pid);
      goto end;
  }

  while(1) {
     if (getexit() > 0) {
        printf("press ctrl + c, thread:%x pid:%d will be exit\n", ctx->tid, ctx->pid);
        break;
     }

     code = avdemux_read_frame(fmt_ctx, &pkt, vsidx);
     printf("thread:%x pid:%d avdemux_read_frame index:%d  return: %d ", ctx->tid, ctx->pid, index, code);
     if (code < 0) {
        break;
     }
     
     printf(" size:%d\n", pkt.size);
     if (code == 1) {
        av_packet_unref(&pkt);
        continue;
     }

      par->streamLen = pkt.size;
      par->streamData = pkt.data;
      code = AmsDecodeVideo(par);
      printf("thread:%x pid:%d send data to dps board, return: %d\n", ctx->tid, ctx->pid, code);
      fwrite(pkt.data, 1, pkt.size, fp);
      av_packet_unref(&pkt);
      ++index;
      if(code < 0){
          break;
      }
  }

end:
    printf("thread:%x pid:%d send all data to board\n", ctx->tid, ctx->pid);
    code = AmsDecodeStop(par->ID);
  
    while(1) {
       printf("thread:%x pid:%d wait decode:[%d  %d]\n", ctx->tid, ctx->pid, ctx->fid, index);
       if (ctx->fid >= index)
           break;
       sleep(1);
    }
  
    if(par)
       free(par);

    avformat_close_input(&fmt_ctx);
    if (fp)
        fclose(fp);

    if (ctx->fyuv)
        fclose(ctx->fyuv);

    addexit(1);
    printf("press ctrl + c, thread:%x pid:%d is exit\n", ctx->tid, ctx->pid);
    return NULL;
}
  


//./avdemux_test  rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_0.h264 rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_1.h264
int main (int argc, char **argv) {
    int nthreads = argc / 2, i =0, ret = 0;
    pthread_mutex_init(&g_mutex, NULL);
    signal(SIGINT, sign_func);
    g_demux_ctx = malloc(nthreads * sizeof(demux_ctx));

    printf("process will be execute, thread num:%d\n", nthreads);
    
    ret = AmsDecoderInit(0, DecodeCallback);
    if(ret < 0){
        printf("error when init ams decoder\n");
        return -1;
    }

    for (i = 0; i < nthreads; ++i) {
        g_demux_ctx[i].url = argv[2 * i + 1];
        g_demux_ctx[i].name = argv[2 * i + 2];
        g_demux_ctx[i].pid = i;
        g_demux_ctx[i].fid = 0;
        g_demux_ctx[i].fyuv = NULL;
        
        pthread_create(&(g_demux_ctx[i].tid), NULL, demux_thread, &g_demux_ctx[i]);
    }
    
    while(1) {
        if (getexit() > 0) {
            printf("press ctrl + c, process will be exit\n");
            break;
        }
         
        printf("threads:%d is running and execute tasks\n", nthreads);
        sleep(5);
    }

    while(1) {
        if (getexit() > nthreads) {
            printf("wait for thread num:%d exit over\n", nthreads);
            break;
        }
        printf("wait for thread num:%d exit\n", nthreads);
        sleep(1);    
    }
    
    sleep(5);
    ret = AmsDecoderRelease();
    if(ret < 0){
        printf("error when release ams decoder\n");
    }

    free(g_demux_ctx);
    pthread_mutex_destroy(&g_mutex);
    printf("process will be exit\n");
    return 0;
}



//#define DEMUX_TEST
#define DEMUX_MULTI_TEST

#ifdef DEMUX_TEST


//./avdemuxing  rtsp://admin:fiberhome025@192.17.1.72:554/  rtsp_v_1.h264
int main (int argc, char **argv) {
  AVPacket pkt;
  AVFormatContext *fmt_ctx = NULL;
  int vsidx = -1, asidx = -1;
  int code = -1, index = 0;
  FILE *fp = NULL;
  
  code = avdemux_open_input(&fmt_ctx, argv[1], NULL, NULL);
  if (code < 0) {
        fprintf(stderr, "Could not open source file %s\n", argv[1]);
        exit(1);
  }
  
  printf("avdemux_open_input:%s success\n", argv[1]);
  
  vsidx = avdemux_find_streamidx(fmt_ctx, AVMEDIA_TYPE_VIDEO);
  if (vsidx < 0) {
        fprintf(stderr, "Could not find %s stream in input file '%s'\n",
                av_get_media_type_string(AVMEDIA_TYPE_VIDEO),  argv[1]);
        goto end;
  }

  printf("avdemux_find_streamidx:%d success\n", vsidx);
  fp = fopen(argv[2], "wb");
  if (!fp) {
      fprintf(stderr, "Could not open destination file %s\n", argv[2]);
      goto end;
  }

  printf("open:%s success\n", argv[2]);
  av_init_packet(&pkt);
  pkt.data = NULL;
  pkt.size = 0;
    
  while(1) {
     code = avdemux_read_frame(fmt_ctx, &pkt, vsidx);
     printf("avdemux_read_frame index:%d  return: %d ", index, code);
     if (code < 0) {
        break;
     }
     
     printf(" size:%d\n", pkt.size);
     ++index;
     if (code == 1) {
        av_packet_unref(&pkt);
        continue;
     }
     fwrite(pkt.data, 1, pkt.size, fp);
     av_packet_unref(&pkt);
  }
  
end:
    avformat_close_input(&fmt_ctx);
    if (fp)
        fclose(fp);
    return code < 0;
}

#endif