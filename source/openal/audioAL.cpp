//
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy audio library
// author:       lipengxiang
// description:  
//

#include "preheader.h"
#include "audioAL.h"
#include "audioBufferMgrAL.h"

namespace hyaudio
{


    //-----------------------------------------------
    AudioAL::AudioAL()
        : _alBuffer(0)
        , _alSource(0)
        , _format(AL_FORMAT_MONO16)
        , _sampling(AudioSampling_44kHZ)
        , _pos(0,0,0)
        , _velo(0,0,0)
        , _dir(0,0,0)
    {
    }

    //-----------------------------------------------
    AudioAL::~AudioAL()
    {
        clear();
        
        if (_alSource != 0)
            alDeleteSources(1, &_alSource);
    }
    
    //-----------------------------------------------
    bool AudioAL::isStreamMode() const
    {
        s32 v;
        alGetSourcei(_alSource, AL_SOURCE_TYPE, &v);
        return v == AL_STREAMING;
    }
    
    //-----------------------------------------------
    bool AudioAL::isPlaying() const
    {
        ALint value;
        alGetSourcei(_alSource, AL_SOURCE_STATE, &value);
        return value == AL_PLAYING;
    }

    //-----------------------------------------------
    void AudioAL::clear()
    {
        if (_alBuffer != 0)
        {
            alDeleteBuffers(1, &_alBuffer);
            _alBuffer = 0;
        }

        if (!_filename.empty())
        {
            AudioBufferMgrAL::getSingleton().removeAudio(_filename);
            _filename.clear();
        }

        s32 v;
        alGetSourcei(_alSource, AL_SOURCE_TYPE, &v);
        if (v == AL_STREAMING)
            updateStream();
    }

    //-----------------------------------------------
    bool AudioAL::init( AudioFormat format, AudioSampling sampling )
    {
        assert(!_alSource);

        _format2 = format;
        _format = _getALFormat(format);
        _sampling = sampling;

        // source.
        alGenSources(1, &_alSource);
        if (!IS_AL_NO_ERROR())
        {
            return false;
        }

        alSourcef(_alSource, AL_MIN_GAIN, 0.0f);
        alSourcef(_alSource, AL_MAX_GAIN, 1.0f);
        //setPosition(_pos);
        //setLoop(_loop);
        //setVolume(_volume);

        return IS_AL_NO_ERROR();
    }

    //-----------------------------------------------
    void AudioAL::setPosition( f32 x, f32 y, f32 z )
    {
        _pos.x = x;
        _pos.y = y;
        _pos.z = z;
        alSource3f(_alSource, AL_POSITION, x, y, z);

        CHECK_AL_ERROR();
    }

    //-----------------------------------------------
    void AudioAL::setVelocity( f32 x, f32 y, f32 z )
    {
        _velo.x = x;
        _velo.y = y;
        _velo.z = z;
        alSource3f(_alSource, AL_VELOCITY, x, y, z);
        CHECK_AL_ERROR();
    }

    //-----------------------------------------------
    void AudioAL::setDirection( f32 x, f32 y, f32 z )
    {
        _dir.x = x;
        _dir.y = y;
        _dir.z = z;
        alSource3f(_alSource, AL_DIRECTION, x, y, z);
        CHECK_AL_ERROR();
    }

    //-----------------------------------------------
    void AudioAL::setLooping( bool loop )
    {
        alSourcei(_alSource, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
        CHECK_AL_ERROR();
    }

    //-----------------------------------------------
    bool AudioAL::isLooping() const
    {
        s32 v;
        alGetSourcei(_alSource, AL_LOOPING, &v);
        return v == AL_TRUE;
    }

    //-----------------------------------------------
    void AudioAL::setVolume( f32 v )
    {
        if (v > 1.0f)     v = 1.0f;
        else if (v < 0)   v = 0.0f;

        alSourcef(_alSource, AL_GAIN, v);
        CHECK_AL_ERROR();
    }

    //-----------------------------------------------
    hyc::f32 AudioAL::getVolume() const
    {
        f32 v;
        alGetSourcef(_alSource, AL_GAIN, &v);
        return v;
    }

    //-----------------------------------------------
    void AudioAL::setMaxDistance( f32 v )
    {
        if (v < 0)
            v = 0;
        alSourcef(_alSource, AL_MAX_DISTANCE, v);
        CHECK_AL_ERROR();
    }

    //-----------------------------------------------
    hyc::f32 AudioAL::getMaxDistance() const
    {
        f32 v;
        alGetSourcef(_alSource, AL_MAX_DISTANCE, &v);
        return v;
    }

    //-----------------------------------------------
    void AudioAL::play()
    {
        ALint value;
        alGetSourcei(_alSource, AL_SOURCE_STATE, &value);
        if (value != AL_PLAYING)
        {
            alSourcePlay(_alSource);
        }

        CHECK_AL_ERROR();
    }

    //-----------------------------------------------
    void AudioAL::stop()
    {
        alSourceStop(_alSource);
        alSourceRewind(_alSource);
        CHECK_AL_ERROR();
    }

    //-----------------------------------------------
    void AudioAL::pause()
    {
        alSourcePause(_alSource);
        CHECK_AL_ERROR();
    }

    //-----------------------------------------------
    void AudioAL::queueStream( const void* pPCMData, u32 size )
    {
        assert(pPCMData && size > 0);
        
        s32 v;
        alGetSourcei(_alSource, AL_SOURCE_TYPE, &v);
        
        if (v != AL_STATIC) {
            ALuint alBuffer;
            alGenBuffers(1, &alBuffer);
            if (0 == alBuffer || !IS_AL_NO_ERROR())
            {
                return;
            }
            
            if (_sampling == 0)
                _sampling = AudioSampling_44kHZ;
            
            alBufferData(alBuffer, _format, pPCMData, size, _sampling);
            CHECK_AL_ERROR();
            alSourceQueueBuffers(_alSource, 1, &alBuffer);
            CHECK_AL_ERROR();

            play();
        }
        else
        {
            clear();

            alGenBuffers(1, &_alBuffer);
            if (0 == _alBuffer || !IS_AL_NO_ERROR())
            {
                return;
            }

            if (_sampling == 0)
                _sampling = AudioSampling_44kHZ;
            
            alBufferData(_alBuffer, _format, pPCMData, size, _sampling);
            alSourcei(_alSource, AL_BUFFER, _alBuffer);
            
            play();
        }
    }

    //-----------------------------------------------
    void AudioAL::queueFile( const tchar* filePath )
    {
        s32 v;
        alGetSourcei(_alSource, AL_SOURCE_TYPE, &v);
        
        if (v != AL_STREAMING) {

            if (_filename == filePath)
                return;

            clear();

            _filename = filePath;
            Array<ALuint> arrBufIds;
            AudioBufferMgrAL::getSingleton().getAudio(_filename, _format2, _sampling, arrBufIds);
            _format = _getALFormat(_format2);

            if (arrBufIds.empty())
                return;
            else if (arrBufIds.size() == 1)
            {
                alSourcei(_alSource, AL_BUFFER, arrBufIds[0]);
            }
            else
            {
                alSourceQueueBuffers(_alSource, arrBufIds.size(), arrBufIds.begin());
                CHECK_AL_ERROR();
            }

            play();
        }
#ifdef _DEBUG
        else
        {
            trace(_T("isn't stream mode, can't update queue stream"));
        }
#endif // _DEBUG
    }

    //-----------------------------------------------
    void AudioAL::updateStream()
    {
        s32 v;
        alGetSourcei(_alSource, AL_SOURCE_TYPE, &v);
        
        if (v != AL_STATIC) {

            ALint processed = 0;
            alGetSourcei(_alSource, AL_BUFFERS_PROCESSED, &processed);

            if (processed > 0)
            {
                ALuint* buffers = new ALuint[processed];
                alSourceUnqueueBuffers(_alSource, processed, buffers);
                alDeleteBuffers(processed, buffers);
                delete[] buffers;
            }
        }
#ifdef _DEBUG
        else
        {
            trace(_T("isn't stream mode, can't update queue stream"));
        }
#endif // _DEBUG
    }


} // end namespace hyaudio
