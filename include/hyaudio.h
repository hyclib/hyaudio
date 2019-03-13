//
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy audio library
// author:       lipengxiang
// description:  
//

#ifndef __BP_HYAUDIO_H_20140630161234__
#define __BP_HYAUDIO_H_20140630161234__

#include "hyaudioPlatform.h"
#include "hyaudioSystem.h"

namespace hyaudio
{
    /**
     * @brief create the audio system.
     */
    extern HYA_API IAudioSystem* CreateAudioSystem( AudioSystemType type );

} // end namespace hyaudio

#endif // end of define __BP_HYAUDIO_H_20140630161234__