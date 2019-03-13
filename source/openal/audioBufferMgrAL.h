//
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy audio library
// author:       lipengxiang
// description:  
//

#ifndef __BP_AUDIOBUFFERMGR_H_20140703170021__
#define __BP_AUDIOBUFFERMGR_H_20140703170021__

#include <map>
#include "../codec/codec.h"

namespace hyaudio
{
    class AudioBufferMgrAL : public Singleton<AudioBufferMgrAL>
    {
    public:
        struct Value
        {
            Value() : refc(1), buffer(0), pBuffers(NULL)    {}

            s32   refc;
            ALuint buffer;
            Array<ALuint>* pBuffers;
        };

    public:
        AudioBufferMgrAL();
        ~AudioBufferMgrAL();

        // 返回值表明是否在pool内.
        bool    removeAudio( const String& file );

        void    getAudio( const String& file, AudioFormat& format, ALsizei& sampling, OUT Array<ALuint>& buffids );

    protected:
        bool    onDecodeAudio( void* pcm, s32 size, AudioFormat format, s32 sampling, f32 loadedPercent );
        AudioFrameEvent _frameEvent;

    protected:
        SyncObject      _sync;
        typedef std::map<String, Value>   MAP_AUDIO;
        MAP_AUDIO       _audios;
        Array<ALuint>*  _pBuffids;
        ByteStream      _buf;
    };

} // end namespace hyaudio

#endif // end of define __BP_AUDIOBUFFERMGR_H_20140703170021__