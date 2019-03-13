//
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy audio library
// author:       lipengxiang
// description:  
//



#ifdef __cplusplus
extern "C"
{
#endif

#define __STDC_CONSTANT_MACROS
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"

#ifdef __cplusplus
};
#endif


IMPLEMENT_INTERFACE_SINGLETON(hyaudio::Codec);

namespace hyaudio
{
    inline AudioFormat GetFormat( int ffFormat, int channels )
    {
        if (channels == 1)
        {
            switch (ffFormat)
            {
            case AV_SAMPLE_FMT_U8:
                return AudioFormat_mono8;
            case AV_SAMPLE_FMT_U8P:
                return AudioFormat_mono8;
            case AV_SAMPLE_FMT_S16:
                return AudioFormat_mono16;
            case AV_SAMPLE_FMT_S16P:
                return AudioFormat_mono16;
            default:
                return AudioFormat_mono16;
            }
        }
        else
        {
            switch (ffFormat)
            {
            case AV_SAMPLE_FMT_U8:
                return AudioFormat_stereo8;
            case AV_SAMPLE_FMT_U8P:
                return AudioFormat_stereo8;
            case AV_SAMPLE_FMT_S16:
                return AudioFormat_mono16;
            case AV_SAMPLE_FMT_S16P:
                //print_all_line(_T("can't support AV_SAMPLE_FMT_S16P"));
                //return AudioFormat_stereo16;
                return AudioFormat_mono16;
            default:
                return AudioFormat_stereo16;
            }
        }
    }

    static int AudioResampling(
        AVCodecContext* audio_dec_ctx,
        AVFrame * pAudioDecodeFrame,
        int out_sample_fmt,
        int out_channels,
        int out_sample_rate,
        uint8_t* out_buf)
    {
        SwrContext* swr_ctx = NULL;
        int data_size = 0;
        int ret = 0;
        int64_t src_ch_layout = audio_dec_ctx->channel_layout;
        int64_t dst_ch_layout = AV_CH_LAYOUT_STEREO;
        int dst_nb_channels = 0;
        int dst_linesize = 0;
        int src_nb_samples = 0;
        int dst_nb_samples = 0;
        int max_dst_nb_samples = 0;
        uint8_t **dst_data = NULL;
        int resampled_data_size = 0;

        swr_ctx = swr_alloc();
        if (!swr_ctx)
        {
            //printf("swr_alloc error \n");
            return -1;
        }

        src_ch_layout = (audio_dec_ctx->channels ==
            av_get_channel_layout_nb_channels(audio_dec_ctx->channel_layout)) ?
            audio_dec_ctx->channel_layout :
        av_get_default_channel_layout(audio_dec_ctx->channels);

        if (out_channels == 1)
        {
            dst_ch_layout = AV_CH_LAYOUT_MONO;
            //printf("dst_ch_layout: AV_CH_LAYOUT_MONO\n");
        }
        else if (out_channels == 2)
        {
            dst_ch_layout = AV_CH_LAYOUT_STEREO;
            //printf("dst_ch_layout: AV_CH_LAYOUT_STEREO\n");
        }
        else
        {
            dst_ch_layout = AV_CH_LAYOUT_SURROUND;
            //printf("dst_ch_layout: AV_CH_LAYOUT_SURROUND\n");
        }

        if (src_ch_layout <= 0)
        {
            swr_free(&swr_ctx);
            //printf("src_ch_layout error \n");
            return -1;
        }

        src_nb_samples = pAudioDecodeFrame->nb_samples;
        if (src_nb_samples <= 0)
        {
            swr_free(&swr_ctx);
            //printf("src_nb_samples error \n");
            return -1;
        }

        av_opt_set_int(swr_ctx, "in_channel_layout", src_ch_layout, 0);
        av_opt_set_int(swr_ctx, "in_sample_rate", audio_dec_ctx->sample_rate, 0);
        av_opt_set_sample_fmt(swr_ctx, "in_sample_fmt", audio_dec_ctx->sample_fmt, 0);

        av_opt_set_int(swr_ctx, "out_channel_layout", dst_ch_layout, 0);
        av_opt_set_int(swr_ctx, "out_sample_rate", out_sample_rate, 0);
        av_opt_set_sample_fmt(swr_ctx, "out_sample_fmt", (AVSampleFormat)out_sample_fmt, 0);

        if ((ret = swr_init(swr_ctx)) < 0) {
            swr_free(&swr_ctx);
            //printf("Failed to initialize the resampling context\n");
            return -1;
        }

        max_dst_nb_samples = dst_nb_samples = av_rescale_rnd(src_nb_samples,
            out_sample_rate, audio_dec_ctx->sample_rate, AV_ROUND_UP);
        if (max_dst_nb_samples <= 0)
        {
            //printf("av_rescale_rnd error \n");
            swr_free(&swr_ctx);
            return -1;
        }

        dst_nb_channels = av_get_channel_layout_nb_channels(dst_ch_layout);
        ret = av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, dst_nb_channels,
            dst_nb_samples, (AVSampleFormat)out_sample_fmt, 0);
        if (ret < 0)
        {
            //printf("av_samples_alloc_array_and_samples error \n");
            swr_free(&swr_ctx);
            return -1;
        }


        dst_nb_samples = av_rescale_rnd(swr_get_delay(swr_ctx, audio_dec_ctx->sample_rate) +
            src_nb_samples, out_sample_rate, audio_dec_ctx->sample_rate, AV_ROUND_UP);
        if (dst_nb_samples <= 0)
        {
            //printf("av_rescale_rnd error \n");
            swr_free(&swr_ctx);
            return -1;
        }
        if (dst_nb_samples > max_dst_nb_samples)
        {
            av_free(dst_data[0]);
            ret = av_samples_alloc(dst_data, &dst_linesize, dst_nb_channels,
                dst_nb_samples, (AVSampleFormat)out_sample_fmt, 1);
            max_dst_nb_samples = dst_nb_samples;
        }

        if (swr_ctx)
        {
            ret = swr_convert(swr_ctx, dst_data, dst_nb_samples,
                (const uint8_t **)pAudioDecodeFrame->data, pAudioDecodeFrame->nb_samples);
            if (ret < 0)
            {
                //printf("swr_convert error \n");
                swr_free(&swr_ctx);
                return -1;
            }

            resampled_data_size = av_samples_get_buffer_size(&dst_linesize, dst_nb_channels,
                ret, (AVSampleFormat)out_sample_fmt, 1);
            if (resampled_data_size < 0)
            {
                //printf("av_samples_get_buffer_size error \n");
                swr_free(&swr_ctx);
                return -1;
            }
        }
        else
        {
            //printf("swr_ctx null error \n");
            swr_free(&swr_ctx);
            return -1;
        }

        memcpy(out_buf, dst_data[0], resampled_data_size);

        if (dst_data)
        {
            av_freep(&dst_data[0]);
        }
        av_freep(&dst_data);
        dst_data = NULL;

        if (swr_ctx)
        {
            swr_free(&swr_ctx);
        }
        return resampled_data_size;
    }

    //-----------------------------------------------
    Codec::Codec()
    {
        av_register_all();
    }

    //-----------------------------------------------
    Codec::~Codec()
    {
    }

    //-----------------------------------------------
    bool Codec::openFile( const tchar* filename, AudioFormat& format, int& sampling, AudioFrameEvent& frameHandle, AudioInfo* pInfo )
    {
        AVFormatContext* pFormatCtx = NULL;

#if defined(UNICODE) || defined(_UNICODE)
        cString cStr;
        const char* filenamec = StringUtils::StrToStr(filename, cStr);
#else
        const char* filenamec = filename;
#endif

        // open file.
        if (0 != avformat_open_input(&pFormatCtx, filenamec, NULL, NULL))
        {
            error_all(_T("can't open the media file %s"), filename);
            return false;
        }

        // get stream.
        if (0 > avformat_find_stream_info(pFormatCtx, NULL))
        {
            error_all(_T("can't get the media stream %s"), filename);
            avformat_close_input(&pFormatCtx);
            return false;
        }

        // find the first stream
        unsigned int i;
        int audioStream = -1;

        for (i = 0; i < pFormatCtx->nb_streams; i++) {
            if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO && audioStream < 0) {
                audioStream = i;
                break;
            }
        }

        if (audioStream == -1)
        {
            error_all(_T("can't find audio stream %s"), filename);
            avformat_close_input(&pFormatCtx);
            return false;
        }

        AVCodecContext* pAudioCodecCtx = pFormatCtx->streams[audioStream]->codec;

        // find the codec.
        AVCodec* pCodec = avcodec_find_decoder(pAudioCodecCtx->codec_id);
        if (NULL == pCodec)
        {
            error_all(_T("can't find the codec %s"), filename);
            avformat_close_input(&pFormatCtx);
            return false;
        }

        // deal truncated stream.
        if (pCodec->capabilities & CODEC_CAP_TRUNCATED)
            pAudioCodecCtx->flags |= CODEC_FLAG_TRUNCATED;

        // open codec.
        if (0 > avcodec_open2(pAudioCodecCtx, pCodec, NULL))
        {
            error_all(_T("can't open the codec %s"), filename);
            avformat_close_input(&pFormatCtx);
            return false;
        }

        // info.
        if (pInfo)
        {
            pInfo->bitRate = pAudioCodecCtx->bit_rate;
            pInfo->sampleRate = pAudioCodecCtx->sample_rate;
            pInfo->channels = pAudioCodecCtx->channels;
            StringUtils::StrToStr(pAudioCodecCtx->codec->name, pInfo->codecName);
        }
        sampling = pAudioCodecCtx->sample_rate;

        // read frame.
        static AVPacket packet;
        AVFrame*        pFrame = av_frame_alloc();
        int             frame_ptr = 0;
        bool            isBreak = false;
        AudioFormat     af = AudioFormat_mono16;

        while (av_read_frame(pFormatCtx, &packet) >= 0)
        {
            if(packet.stream_index == audioStream)
            {
                if(0 > avcodec_decode_audio4(pAudioCodecCtx, pFrame, &frame_ptr, &packet))
                {
                    isBreak = true;
                    break;
                }

                if (frame_ptr) {
                    //                     int data_size = av_samples_get_buffer_size(pFrame->linesize, pAudioCodecCtx->channels, pFrame->nb_samples, pAudioCodecCtx->sample_fmt, 1);
                    //                     if (data_size < 0)
                    //                     {
                    //                         isBreak = true;
                    //                         break;
                    //                     }
                    //                     else
                    if (pFrame->linesize[0] > 0)
                    {
                        af = GetFormat(pFrame->format, pFrame->channels);
                        if (!frameHandle(pFrame->data[0], pFrame->linesize[0], af, pAudioCodecCtx->sample_rate, 0.1f))
                        {
                            isBreak = true;
                            break;
                        }
                    }
                }
            }
            av_free_packet(&packet);  
        }

        if (!isBreak)
            frameHandle((void*)0, (s32)0, af, pAudioCodecCtx->sample_rate, 1.0f);

        av_frame_free(&pFrame);
        avcodec_close(pAudioCodecCtx);
        avformat_close_input(&pFormatCtx);
        return true;
    }

} // end namespace hyaudio
