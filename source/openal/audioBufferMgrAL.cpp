//
// Copyright(c) 1999-2014 hyrj. All rights reserved.
// hy audio library
// author:       lipengxiang
// description:  
//

#include "preheader.h"
#include "audioBufferMgrAL.h"


IMPLEMENT_INTERFACE_SINGLETON(hyaudio::AudioBufferMgrAL);

namespace hyaudio
{
    //-----------------------------------------------
    AudioBufferMgrAL::AudioBufferMgrAL()
        : _buf(false)
    {
        _buf.reserve(1024*512);
        _frameEvent.set(this, &AudioBufferMgrAL::onDecodeAudio);
    }

    //-----------------------------------------------
    AudioBufferMgrAL::~AudioBufferMgrAL()
    {
        SyncTokenPtr token = _sync.getToken();

        for (MAP_AUDIO::iterator itr = _audios.begin(); itr != _audios.end(); itr++)
        {
            if (itr->second.buffer)
                alDeleteBuffers(1, &itr->second.buffer);
            else
            {
                alDeleteBuffers(itr->second.pBuffers->size(), itr->second.pBuffers->begin());
                delete itr->second.pBuffers;
            }
        }
    }

    //-----------------------------------------------
    bool AudioBufferMgrAL::removeAudio( const String& file )
    {
        SyncTokenPtr token = _sync.getToken();

        MAP_AUDIO::iterator itr = _audios.find(file);
        if (itr != _audios.end())
        {
            if (--itr->second.refc <= 0)
            {
                if (itr->second.buffer)
                    alDeleteBuffers(1, &itr->second.buffer);
                else
                {
                    alDeleteBuffers(itr->second.pBuffers->size(), itr->second.pBuffers->begin());
                    delete itr->second.pBuffers;
                }
                _audios.erase(itr);
            }
            return true;
        }

        return false;
    }

    //-----------------------------------------------
    void AudioBufferMgrAL::getAudio( const String& file, AudioFormat& format, ALsizei& sampling, OUT Array<ALuint>& buffids )
    {
        buffids.clear();
        SyncTokenPtr token = _sync.getToken();
        _pBuffids = &buffids;
        _pBuffids->clear();
        _buf.clear();

        MAP_AUDIO::iterator itr = _audios.find(file);
        if (itr == _audios.end())
        {
            if (Codec::getSingleton().openFile(file.c_str(), format, sampling, _frameEvent))
            {
                Value v;
                if (buffids.size() == 1)
                    v.buffer = buffids[0];
                else
                    v.pBuffers = new Array<ALuint>(buffids);

                _audios.insert(std::make_pair(file, v));
            }
            else
            {
                error_all(_T("open audio file error: %s"), file.c_str());
                alDeleteBuffers(buffids.size(), buffids.begin());
                buffids.clear();
            }
        }
        else
        {
            itr->second.refc++;
            if (itr->second.buffer)
                buffids.push_back(itr->second.buffer);
            else
                buffids = *itr->second.pBuffers;
        }
    }

    //-----------------------------------------------
    bool AudioBufferMgrAL::onDecodeAudio( void* pcm, s32 size, AudioFormat format, s32 sampling, f32 loadedPercent )
    {
        bool finished = loadedPercent >= 1;
        
        void* pData;
        s32   dataSize;
        
        void* pDataReserve = NULL;
        s32   dataSizeReserve = 0;
        
        if (finished && _buf.empty())
        {
            pData = pcm;
            dataSize = size;
        }
        else
        {
            if (size + _buf.size() <= _buf.capacity())
            {
                if (pcm)
                    _buf.add(pcm, size);
            }
            else
            {
                pDataReserve = pcm;
                dataSizeReserve = size;
            }
            
            pData = _buf.begin();
            dataSize = _buf.size();
        } // if..else
        
        ALuint alBuffer;
        alGenBuffers(1, &alBuffer);
        if (0 == alBuffer || !IS_AL_NO_ERROR())
        {
            return false;
        }
        
        alBufferData(alBuffer, _getALFormat(format), pData, dataSize, sampling);
        _buf.clear();
        if (IS_AL_NO_ERROR())
        {
            _pBuffids->push_back(alBuffer);
        }
        else
        {
            return false;
        }
        
        if (pDataReserve)
            _buf.add(pDataReserve, dataSizeReserve);

        return true;
    }

} // end namespace hyaudio
