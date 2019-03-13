//
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy audio library
// author:       lipengxiang
// description:  
//

#ifndef __BP_CODEC_H_20140704102350__
#define __BP_CODEC_H_20140704102350__


namespace hyaudio
{
    /**
     * @brief 解析到音频帧时的回调.
     * @return 表明是否继续解析.
     */
    typedef EventSingle<bool(void* pcm, s32 size, AudioFormat format, s32 sampling, f32 loadedPercent)> AudioFrameEvent;

    struct AudioInfo
    {
        int     bitRate;
        int     sampleRate;
        int     channels;
        String  codecName;
    };

    class Codec : public Singleton<Codec>
    {
    public:
        Codec();
        ~Codec();

        bool openFile( const tchar* filename, AudioFormat& format, int& sampling, AudioFrameEvent& frameHandle, AudioInfo* pInfo = NULL );
    };
	

} // end namespace hyaudio

#endif // end of define __BP_CODEC_H_20140704102350__