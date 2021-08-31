


#ifndef AVDEMUXING_AVDEMUXING_H
#define AVDEMUXING_AVDEMUXING_H

#include <libavformat/avformat.h>
#include <libavutil/avutil.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file
 * @ingroup libavdemuxing
 * public API header
 */
 
/**
 * Open an input stream and read the header. The codecs are not opened.
 * The stream must be closed with avdemux_close_input().
 *
 * @param pctx Pointer to user-supplied AVFormatContext (allocated by avformat_alloc_context).
 *           May be a pointer to NULL, in which case an AVFormatContext is allocated by this
 *           function and written into ps.
 *           Note that a user-supplied AVFormatContext will be freed on failure.
 * @param url URL of the stream to open.
 * @param fmt If non-NULL, this parameter forces a specific input format.
 *            Otherwise the format is autodetected.
 * @param options  A dictionary filled with AVFormatContext and demuxer-private options.
 *                 On return this parameter will be destroyed and replaced with a dict containing
 *                 options that were not found. May be NULL.
 *
 * @return 0 on success, a negative AVERROR on failure.
 *
 * @note If you want to use custom IO, preallocate the format context and set its pb field.
 */
extern int avdemux_open_input(AVFormatContext **pctx, const char *url, AVInputFormat *fmt, AVDictionary **options);

/**
 * Find the "best" stream in the file.
 * The best stream is determined according to various heuristics as the most
 * likely to be what the user expects.
 * If the decoder parameter is non-NULL, av_find_best_stream will find the
 * default decoder for the stream's codec; streams for which no decoder can
 * be found are ignored.
 *
 * @param ctx                media file handle
 * @param avtype             stream type: video, audio, subtitles, etc.
 * @return  the non-negative stream number in case of success,
 *          AVERROR_STREAM_NOT_FOUND if no stream with the requested type
 *          could be found,
 *          AVERROR_DECODER_NOT_FOUND if streams were found but no decoder
 * @note  If av_find_best_stream returns successfully and decoder_ret is not
 *        NULL, then *decoder_ret is guaranteed to be set to a valid AVCodec.
 */
extern int avdemux_find_streamidx(AVFormatContext *ctx, enum AVMediaType avtype);

/**
 * Return the next frame of a stream, the stream number is streamidx.
 * This function returns what is stored in the file, and does not validate
 * that what is there are valid frames for the decoder. It will split what is
 * stored in the file into frames and return one for each call. It will not
 * omit invalid data between valid frames so as to give the decoder the maximum
 * information possible for decoding.
 *
 * If pkt->buf is NULL, then the packet is valid until the next
 * av_read_frame() or until avformat_close_input(). Otherwise the packet
 * is valid indefinitely. In both cases the packet must be freed with
 * av_packet_unref when it is no longer needed. For video, the packet contains
 * exactly one frame. For audio, it contains an integer number of frames if each
 * frame has a known fixed size (e.g. PCM or ADPCM data). If the audio frames
 * have a variable size (e.g. MPEG audio), then it contains one frame.
 *
 * pkt->pts, pkt->dts and pkt->duration are always set to correct
 * values in AVStream.time_base units (and guessed if the format cannot
 * provide them). pkt->pts can be AV_NOPTS_VALUE if the video format
 * has B-frames, so it is better to rely on pkt->dts if you do not
 * decompress the payload.
 *
 * @return 0 if OK, < 0 on error or end of file, = 1  stream number is not streamidx
 */
extern int avdemux_read_frame(AVFormatContext *ctx, AVPacket *pkt, int streamidx);

/**
 * Close an opened input AVFormatContext. Free it and all its contents
 * and set *s to NULL.
 */
extern void avdemux_close_input(AVFormatContext **pctx);



#ifdef __cplusplus
}
#endif

/**
 * @}
 */

#endif /* AVDEMUXING_AVDEMUXING_H */