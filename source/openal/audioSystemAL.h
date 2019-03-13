//
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy audio library
// author:       lipengxiang
// description:  
//

#ifndef __BP_AUDIOSYSTEMAL_H_20140702145602__
#define __BP_AUDIOSYSTEMAL_H_20140702145602__

#include "preheader.h"
#include "audioAL.h"
#include "audioBufferMgrAL.h"
#include "../codec/codec.h"

namespace hyaudio
{

    //----------------------------------------------------------------------------------------------------
    //*	audio system manager.
    //----------------------------------------------------------------------------------------------------
    class AudioSystemAL : public IAudioSystem, public Singleton<AudioSystemAL>
    {
    public:
        AudioSystemAL();
        virtual ~AudioSystemAL();

        bool    init();

        virtual void        release();
        virtual IAudio*     createAudio( AudioFormat format = AudioFormat_stereo16, AudioSampling sampling = AudioSampling_44kHZ );
        virtual void        destoryAudio( IAudio* pAudio );

    public:
        virtual const Vec3& getPosition() const { return _pos; }
        virtual void        setPosition( f32 x, f32 y, f32 z );

        virtual const Vec3& getVelocity() const  { return _velo; }
        virtual void        setVelocity( f32 x, f32 y, f32 z );

        virtual const Vec3& getUp() const  { return _up; }
        virtual void        setUp( f32 upx, f32 upy, f32 upz );

        virtual const Vec3& getDirection() const    { return _at; }
        virtual void        setDirection( f32 x, f32 y, f32 z );

        // Passing anything between 0.0 and 1.0 will minimize the Doppler effect, and anything greater than 1.0 will maximize the effect.
        virtual void        setDopplerFactor( f32 factor );
        virtual f32         getDopplerFactor() const;

        virtual void        setSpeed( f32 v );
        virtual f32         getSpeed() const;

        virtual void        setVolume( f32 v );
        virtual f32         getVolume() const;

    private:
        AudioBufferMgrAL _bufferMgr;
        Codec            _codec;
        ALCcontext* _alContext;
        ALCdevice*  _alDevice;

        Vec3        _pos;
        Vec3        _velo;
        Vec3        _at;
        Vec3        _up;

        ArrayFind<AudioAL*> _arrAudios;
    };


} // end namespace hyaudio

#endif // end of define __BP_AUDIOSYSTEMAL_H_20140702145602__