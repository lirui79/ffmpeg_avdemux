

#include "avdemuxing.h"



int avdemux_open_input(AVFormatContext **pctx, const char *url, AVInputFormat *fmt, AVDictionary **options) {
    int code = avformat_open_input(pctx, url, fmt, options);
    if (code < 0) {
        return code;
    }

    code = avformat_find_stream_info(*pctx, NULL);
    if (code < 0) {
        return code;
    }
    return 0;
}


int avdemux_find_streamidx(AVFormatContext *ctx, enum AVMediaType avtype) {
    int streamidx = av_find_best_stream(ctx, avtype, -1, -1, NULL, 0);
    return streamidx;
}


int avdemux_read_frame(AVFormatContext *ctx, AVPacket *pkt, int streamidx) {
    int code = av_read_frame(ctx, pkt);
    if( code < 0) {
       return code;
    }
    
    if (pkt->stream_index != streamidx) {
       return 1;
    }
    
    return 0;
}


void avdemux_close_input(AVFormatContext **pctx) {
     avformat_close_input(pctx);
}



