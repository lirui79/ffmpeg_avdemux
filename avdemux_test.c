
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include "avdemuxing.h"

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
  pthread_t   tid;
}demux_ctx;

static demux_ctx *g_demux_ctx = NULL;

static void *demux_thread(void *arg) {
  AVPacket pkt;
  AVFormatContext *fmt_ctx = NULL;
  demux_ctx *ctx = (demux_ctx*) arg;
  int vsidx = -1, asidx = -1;
  int code = -1, index = 0;
  FILE *fp = NULL;
  
  code = avdemux_open_input(&fmt_ctx, ctx->url, NULL, NULL);
  if (code < 0) {
        fprintf(stderr, "thread:%x Could not open source file %s\n", ctx->tid, ctx->url);
        return NULL;
  }
  
  printf("thread:%x avdemux_open_input:%s success\n", ctx->tid, ctx->url);
  
  vsidx = avdemux_find_streamidx(fmt_ctx, AVMEDIA_TYPE_VIDEO);
  if (vsidx < 0) {
        fprintf(stderr, "thread:%x Could not find %s stream in input file '%s'\n", ctx->tid,
                av_get_media_type_string(AVMEDIA_TYPE_VIDEO),  ctx->url);
        goto end;
  }

  printf("thread:%x avdemux_find_streamidx:%d success\n", ctx->tid, vsidx);
  fp = fopen(ctx->name, "wb");
  if (!fp) {
      fprintf(stderr, "thread:%x Could not open destination file %s\n", ctx->tid, ctx->name);
      goto end;
  }

  printf("thread:%x open:%s success\n", ctx->tid, ctx->name);
  av_init_packet(&pkt);
  pkt.data = NULL;
  pkt.size = 0;
    
  while(1) {
     if (getexit() > 0) {
        printf("press ctrl + c, thread:%x will be exit\n", ctx->tid);
        break;
     }

     code = avdemux_read_frame(fmt_ctx, &pkt, vsidx);
     printf("thread:%x avdemux_read_frame index:%d  return: %d ", ctx->tid, index, code);
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
    addexit(1);
    printf("press ctrl + c, thread:%x is exit\n", ctx->tid);
    return NULL;
}
  


//./avdemux_test  rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_1.h264 rtsp://admin:fiberhome025@192.17.1.72:554/ rtsp_v_2.h264
int main (int argc, char **argv) {
    int nthreads = argc / 2, i =0;
    pthread_mutex_init(&g_mutex, NULL);
    signal(SIGINT, sign_func);
    g_demux_ctx = malloc(nthreads * sizeof(demux_ctx));
    
    printf("process will be execute, thread num:%d\n", nthreads);
    for (i = 0; i < nthreads; ++i) {
        g_demux_ctx[i].url = argv[2 * i + 1];
        g_demux_ctx[i].name = argv[2 * i + 2];
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