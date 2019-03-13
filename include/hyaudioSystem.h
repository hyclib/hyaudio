//
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy audio library
// author:       lipengxiang
// description:  
//

#ifndef __BP_HYAUDIOSYSTEM_H_20140630162702__
#define __BP_HYAUDIOSYSTEM_H_20140630162702__


namespace hyaudio
{
    //----------------------------------------------------------------------------------------------------
    //*	effect
    //----------------------------------------------------------------------------------------------------
    class IAudioEffect
    {
    public:
        virtual void    release() = 0;
    };
     
    //----------------------------------------------------------------------------------------------------
    //*	audio movable interface.
    //----------------------------------------------------------------------------------------------------
    class IAudioMovable
    {
    public:
        //! position.
        virtual const Vec3& getPosition() const = 0;
        virtual void        setPosition( f32 x, f32 y, f32 z ) = 0;
        virtual void        setPosition( const Vec3& v )        { setPosition(v.x, v.y, v.z); }
        virtual void        move( f32 x, f32 y, f32 z )         { const Vec3& pos = getPosition(); setPosition(pos.x+x, pos.y+y, pos.z+z); }
        virtual void        move( const Vec3& v )               { move(v.x, v.y, v.z); }

        //! velocity, use for doppler effect.
        virtual const Vec3& getVelocity() const = 0;
        virtual void        setVelocity( f32 x, f32 y, f32 z ) = 0;

        //! sound direction.
        virtual const Vec3& getDirection() const = 0;
        virtual void        setDirection( f32 x, f32 y, f32 z ) = 0;
        virtual void        setDirection( const Vec3& v )       { setDirection(v.x, v.y, v.z); }
    };

    //----------------------------------------------------------------------------------------------------
    //*	audio
    //*  first use queueFile streamMode == false.
    //*  first use queueStream streamMode == true.
    //----------------------------------------------------------------------------------------------------
    class IAudio : public IAudioMovable
    {
    public:
        //! info.
        virtual bool          isStreamMode() const = 0;
        virtual AudioFormat   getFormat() const = 0;
        virtual AudioSampling getSampling() const = 0;

        //! stream mode can't do this. it will use buffer pool.
        virtual void        queueFile( const tchar* filename ) = 0;

        //! add the stream to audio, if isn't stream mode, will replace the old data.
        virtual void        queueStream( const void* pPCMData, u32 size ) = 0;
        //! clear the process stream, if no call this, it can be looping.
        virtual void        updateStream() = 0;

        //! exceed max distance, listener will not hear.
        virtual f32         getMaxDistance() const = 0;
        virtual void        setMaxDistance( f32 v ) = 0;

        //! looping.
        virtual void        setLooping( bool loop ) = 0;
        virtual bool        isLooping() const = 0;

        //! volume.
        //! max == 1.0f
        virtual void        setVolume( f32 v ) = 0;
        virtual f32         getVolume() const = 0;

        //! controller.
        virtual void        play() = 0;
        virtual void        stop() = 0;
        virtual void        pause() = 0;
    };
     

    //----------------------------------------------------------------------------------------------------
    //*	audio system manager.
    //* IAudioSystem represent the listener.
    //* the coordinate is right-hand system. y-axis is up. x-axis is left.
    //----------------------------------------------------------------------------------------------------
    class IAudioSystem : public IAudioMovable
    {
    public:
        virtual void        release() = 0;

        //! audio object.
        //! if streamMode is false and audioStream is from file, will use bufferPool. The format & sampling will use cache value.
        //! @param format 
        //!     the stereo format have two channel, it will lost 3d spatialization.
        //!     AudioFormat_undefined will determinated by audio in file mode.
        //! @param sampling
        //!     AudioSampling_undefined will determinated by audio in file mode.
        virtual IAudio*     createAudio(
                                        AudioFormat format = AudioFormat_undefined,
                                        AudioSampling sampling = AudioSampling_undefined
                                        ) = 0;
        virtual IAudio*     createAudio(
                                        const tchar* filename,
                                        AudioFormat format = AudioFormat_undefined,
                                        AudioSampling sampling = AudioSampling_undefined
                                        )   { IAudio* pAudio = createAudio(format, sampling); if (pAudio) pAudio->queueFile(filename); return pAudio; }
        virtual void        destoryAudio( IAudio* pAudio ) = 0;

    public:
        //! up.
        virtual const Vec3& getUp() const = 0;
        virtual void        setUp( f32 upx, f32 upy, f32 upz ) = 0;
        virtual void        setUp( const Vec3& up )     { setUp(up.x, up.y, up.z); }

        //! Passing anything between 0.0 and 1.0 will minimize the Doppler effect, and anything greater than 1.0 will maximize the effect.
        virtual void        setDopplerFactor( f32 factor ) = 0;
        virtual f32         getDopplerFactor() const = 0;

        //! sound propagate speed (default is 343.3).
        virtual void        setSpeed( f32 v ) = 0;
        virtual f32         getSpeed() const = 0;

        //! master volume.
        //! max == 1.0f
        virtual void        setVolume( f32 v ) = 0;
        virtual f32         getVolume() const = 0;
    };

} // end namespace hyaudio

#endif // end of define __BP_HYAUDIOSYSTEM_H_20140630162702__