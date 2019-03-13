//
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy audio library
// author:       lipengxiang
// description:  
//


#include "../global.h"
#include "codec.h"

#if CN_PLATFORM == CN_PLATFORM_WINDOWS
#include "codec_win32.hpp"

#elif CN_PLATFORM == CN_PLATFORM_IOS
#include "codec_ios.hpp"

#endif