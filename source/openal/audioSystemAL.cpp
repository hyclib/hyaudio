//
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy audio library
// author:       lipengxiang
// description:  
//

#include "preheader.h"
#include "audioSystemAL.h"
#include "audioAL.h"

IMPLEMENT_INTERFACE_SINGLETON(hyaudio::AudioSystemAL);

namespace hyaudio
{


    //-----------------------------------------------
    AudioSystemAL::AudioSystemAL()
        : _alContext(NULL)
        , _alDevice(NULL)
        , _pos(0,0,0)
        , _velo(0,0,0)
        , _at(0,0,-1)
        , _up(0,1,0)
    {
    }

    //-----------------------------------------------
    AudioSystemAL::~AudioSystemAL()
    {
        assert(!_alContext && !_alDevice);
        assert(_arrAudios.size() == 0);
    }

    //-----------------------------------------------
    bool AudioSystemAL::init()
    {
        assert(!_alContext && !_alDevice);

        _alDevice = alcOpenDevice(NULL);
        if (_alDevice) {
            _alContext = alcCreateContext(_alDevice, NULL);
            if (_alContext) {
                if ( AL_TRUE == alcMakeContextCurrent(_alContext) )
                {
                    return true;
                }
            }
        }

        alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);

        CHECK_AL_ERROR();
        return false;
    }

    //-----------------------------------------------
    void AudioSystemAL::release()
    {
        if (!_alContext)
            return;

        for (u32 i = 0; i < _arrAudios.size(); i++)
        {
            delete _arrAudios[i];
        }
        _arrAudios.clear();

        ALCdevice *device;

        if (!alcMakeContextCurrent (NULL))
        {
            error(_T("ERROR_MAKE_CONTEXT_CURRENT"));
            CHECK_AL_ERROR();

            _alDevice = NULL;
            _alContext = NULL;
            return;
        }

        device = alcGetContextsDevice (_alContext);
        assert(_alDevice == device);

        alcDestroyContext (_alContext);

        if (alcGetError (device) != ALC_NO_ERROR)
        {
            error(_T("ERROR_DESTROY_CONTEXT"));
            CHECK_AL_ERROR();

            _alDevice = NULL;
            _alContext = NULL;
            return;
        }

        if (!alcCloseDevice (device))
        {
            error(_T("ERROR_CLOSE_DEVICE"));
            CHECK_AL_ERROR();

            _alDevice = NULL;
            _alContext = NULL;
            return;
        }

        _alDevice = NULL;
        _alContext = NULL;
        delete this;
    }

    //-----------------------------------------------
    void AudioSystemAL::setPosition( f32 x, f32 y, f32 z )
    {
        _pos.x = x;
        _pos.y = y;
        _pos.z = z;
        alListener3f(AL_POSITION, x, y, z);
        CHECK_AL_ERROR();
    }

    
    //-----------------------------------------------
    void AudioSystemAL::setVelocity( f32 x, f32 y, f32 z )
    {
        _velo.x = x;
        _velo.y = y;
        _velo.z = z;
        alListener3f(AL_VELOCITY, x, y, z);
        CHECK_AL_ERROR();
    }

    //-----------------------------------------------
    void AudioSystemAL::setDopplerFactor( f32 factor )
    {
        if (factor < 0)
            factor = 0;

        alDopplerFactor(factor);
        CHECK_AL_ERROR();
    }

    //-----------------------------------------------
    hyc::f32 AudioSystemAL::getDopplerFactor() const
    {
        return alGetFloat(AL_DOPPLER_FACTOR);
    }

    //-----------------------------------------------
    void AudioSystemAL::setSpeed( f32 v )
    {
        alSpeedOfSound(v);
        CHECK_AL_ERROR();
    }

    //-----------------------------------------------
    hyc::f32 AudioSystemAL::getSpeed() const
    {
        return alGetFloat(AL_SPEED_OF_SOUND);
    }

    //-----------------------------------------------
    void AudioSystemAL::setVolume( f32 v )
    {
        if (v > 1.0f)     v = 1.0f;
        else if (v < 0)   v = 0.0f;

        alListenerf(AL_GAIN, v);
        CHECK_AL_ERROR();
    }

    //-----------------------------------------------
    hyc::f32 AudioSystemAL::getVolume() const
    {
        f32 v;
        alGetListenerf(AL_GAIN, &v);
        return v;
    }

    //-----------------------------------------------
    IAudio* AudioSystemAL::createAudio( AudioFormat format /*= AudioFormat_stereo16*/, AudioSampling sampling /*= AudioSampling_44kHZ */ )
    {
        AudioAL* pAudio = new AudioAL;
        if (!pAudio->init(format, sampling))
        {
            delete pAudio;
            return NULL;
        }

        _arrAudios.push_back(pAudio);
        return pAudio;
    }

    //-----------------------------------------------
    void AudioSystemAL::destoryAudio( IAudio* pAudio )
    {
        if (pAudio)
        {
            _arrAudios.eraseByIndex(_arrAudios.find((AudioAL*)pAudio));
            delete (AudioAL*)pAudio;
        }
    }

    //-----------------------------------------------
    void AudioSystemAL::setDirection( f32 x, f32 y, f32 z )
    {
        _at.x = x;
        _at.y = y;
        _at.z = z;

        f32 ori[] = { _at.x, _at.y, _at.z, _up.x, _up.y, _up.z };
        alListenerfv(AL_ORIENTATION, ori);
        CHECK_AL_ERROR();
    }

    //-----------------------------------------------
    void AudioSystemAL::setUp( f32 upx, f32 upy, f32 upz )
    {
        _up.x = upx;
        _up.y = upy;
        _up.z = upz;

        f32 ori[] = { _at.x, _at.y, _at.z, _up.x, _up.y, _up.z };
        alListenerfv(AL_ORIENTATION, ori);
        CHECK_AL_ERROR();
    }

} // end namespace hyaudio
