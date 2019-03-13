//
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy audio library
// author:       lipengxiang
// description:  
//

#ifndef __BP_AUDIO_H_20140702110526__
#define __BP_AUDIO_H_20140702110526__


namespace hyaudio
{
    class AudioAL : public IAudio
    {
    public:
        AudioAL();
        ~AudioAL();

        bool init( AudioFormat format, AudioSampling sampling );

        virtual bool        isStreamMode() const;
        virtual bool        isPlaying() const;
        virtual AudioFormat   getFormat() const    { return _format2; }
        virtual AudioSampling getSampling() const  { return (AudioSampling)_sampling; }
        
        virtual void        queueFile( const tchar* filePath );
        virtual void        queueStream( const void* pPCMData, u32 size );
        virtual void        updateStream();

        virtual const Vec3& getPosition() const { return _pos; }
        virtual void        setPosition( f32 x, f32 y, f32 z );

        virtual const Vec3& getVelocity() const  { return _velo; }
        virtual void        setVelocity( f32 x, f32 y, f32 z );

        virtual const Vec3& getDirection() const  { return _dir; }
        virtual void        setDirection( f32 x, f32 y, f32 z );

        virtual f32         getMaxDistance() const;
        virtual void        setMaxDistance( f32 v );

        virtual void        setLooping( bool loop );
        virtual bool        isLooping() const;

        virtual void        setVolume( f32 v );
        virtual f32         getVolume() const;

        virtual void        play();
        virtual void        stop();
        virtual void        pause();

    protected:
        virtual void        clear();

    private:
        String  _filename;
        ALuint  _alBuffer;
        ALuint  _alSource;
        ALenum  _format;
        AudioFormat _format2;
        ALsizei _sampling;

        Vec3    _pos;
        Vec3    _velo;
        Vec3    _dir;
    };

} // end namespace hyaudio

#endif // end of define __BP_AUDIO_H_20140702110526__