//
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy audio library
// author:       lipengxiang
// description:  
//

#include "../include/hyaudio.h"
#include "global.h"

#include "openal/audioSystemAL.h"

namespace hyaudio
{
    //-----------------------------------------------
    IAudioSystem* CreateAudioSystem( AudioSystemType type )
    {
        switch (type)
        {
        case AudioSystemType_openal:
            {
                AudioSystemAL* pSys = new AudioSystemAL;
                if (!pSys->init())
                {
                    delete pSys;
                    return NULL;
                }
                return pSys;
            } break;
        default:
            return NULL;
        }
    }

} // end namespace hyaudio
