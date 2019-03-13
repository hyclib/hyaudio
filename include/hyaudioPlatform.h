//
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy audio library
// author:       lipengxiang
// description:  
//

#ifndef __BP_HYAUDIOPLATFORM_H_20140630161249__
#define __BP_HYAUDIOPLATFORM_H_20140630161249__

#include "hyc/hyc.h"
#include "hyc/template/geom/geom.hpp"
using namespace hyc;

namespace hyaudio
{
    typedef hyc::geom::Vector3<f32>   Vec3;
    
    //! audio system type.
    enum AudioSystemType {
        AudioSystemType_openal,
    };

    //! audio format.
    enum AudioFormat {
        AudioFormat_undefined,
        AudioFormat_mono8,
        AudioFormat_mono16,
        AudioFormat_stereo8,
        AudioFormat_stereo16,
    };

    //! audio sampling.
    enum AudioSampling {
        AudioSampling_undefined = 0,
        AudioSampling_8kHZ      = 8000,     // people talk.
        AudioSampling_22kHZ     = 22050,    // FM.
        AudioSampling_32kHZ     = 32000,    // miniDV.
        AudioSampling_44kHZ     = 44100,    // CD.
        AudioSampling_48kHZ     = 48000,    // DVD.
    };

}

//----------------------------------------------------------------------------------------------------
//*	导出符号
//----------------------------------------------------------------------------------------------------
#if CN_PLATFORM == CN_PLATFORM_WINDOWS
#   ifdef HYAUDIO_EXPORTS
#       define HYA_API __declspec(dllexport)
#   elif  defined(_LIB) && defined(HYC_COMPILE_STATIC_LIB)
#       define HYA_API
#   else
#       define HYA_API __declspec(dllimport)
#   endif // HYAUDIO_EXPORTS
#else
#   define  HYA_API
#endif // CN_PLATFORM == CN_PLATFORM_WINDOWS



#endif // end of define __BP_HYAUDIOPLATFORM_H_20140630161249__