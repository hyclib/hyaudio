//
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy audio library
// author:       lipengxiang
// description:  
//

#ifndef __BP_PREHEADER_H_20140702110535__
#define __BP_PREHEADER_H_20140702110535__

#include "../../include/hyaudioPlatform.h"
#include "../../include/hyaudioSystem.h"

#if CN_PLATFORM == CN_PLATFORM_IOS
#include <OpenAL/al.h>
#include <OpenAL/alc.h>

#else
#include "al.h"
#include "alc.h"

#endif

#include "../global.h"


#define IS_AL_NO_ERROR()    (alGetError() == AL_NO_ERROR)

/* Define check OpenAL error */
#ifdef DEBUG
#   define CHECK_AL_ERROR() { ALenum __error = alGetError(); if(__error != AL_NO_ERROR) printf("[! ERROR: OpenAL 0x%04X in %s %d]\r\n", __error, __FUNCTION__, __LINE__); }
#else
#   define CHECK_AL_ERROR()
#endif


namespace hyaudio
{
    inline ALenum _getALFormat( const AudioFormat& format )  { 
        switch (format) {
            case AudioFormat_mono8:
                return AL_FORMAT_MONO8;
            case AudioFormat_mono16:
                return AL_FORMAT_MONO16;
            case AudioFormat_stereo8:
                return AL_FORMAT_STEREO8;
            case AudioFormat_stereo16:
                return AL_FORMAT_STEREO16;
            default:
                return AL_FORMAT_MONO16;
        }
    }
    
    inline bool _isStereoFormat( const AudioFormat& format ) {
        return format >= AudioFormat_stereo8;
    }
}


#endif // end of define __BP_PREHEADER_H_20140702110535__