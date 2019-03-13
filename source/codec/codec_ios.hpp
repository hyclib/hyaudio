//
//  codec_ios.mm
//  hyaudio
//
//  Created by brainpoint on 14-7-8.
//  Copyright (c) 2014年 hy. All rights reserved.
//

#include <AudioToolbox/AudioToolbox.h>
#include "../global.h"
#include "codec.h"
#include "../openal/preheader.h"

IMPLEMENT_INTERFACE_SINGLETON(hyaudio::Codec);

#define BUF_SIZE    1024*512

#define ERR_MSG(status) if (status!=0)  { \
print_all(_T("audio err: %c"), (char)(status>>24)); \
print_all(_T("%c"), (char)((status<<8)>>24));\
print_all(_T("%c"), (char)((status<<16)>>24));\
print_all_line(_T("%c"), (char)((status<<24)>>24));}


namespace hyaudio {
    
//    struct Info
//    {
//        AudioFrameEvent*    pFrameHandle;
//        AudioFileStreamID   fileStreamId;
//        bool                finish;
//        AudioInfo*          pInfo;
//        bool                result;
//    };
//    
//    void audioFileStream_PacketsProc(
//                                     void *							inClientData,
//                                     UInt32							inNumberBytes,
//                                     UInt32							inNumberPackets,
//                                     const void *					inInputData,
//                                     AudioStreamPacketDescription	*inPacketDescriptions)
//    {
//        Info* pInfo = (Info*)inClientData;
//        pInfo->result = false;
//        
//        UInt32 size;
//        
//        AudioStreamBasicDescription desc;
//        ZeroMemory(&desc, sizeof(desc));
//        size = sizeof(desc);
//        if (0 != AudioFileStreamGetProperty(pInfo->fileStreamId, kAudioFileStreamProperty_DataFormat, &size, &desc)) {
//            return;
//        }
//        
//        if (desc.mChannelsPerFrame > 2) {
//            trace_all(_T("can't deal more then 2 channels audio."));
//            return;
//        }
//        
//        bool mono = (desc.mChannelsPerFrame == 1);
//        
//        
//        AudioFormat af = mono ? AudioFormat_mono16 : AudioFormat_stereo16;
//        
//        // info.
//        if (pInfo->pInfo)
//        {
//            UInt32 bitrate;
//            size = sizeof(bitrate);
//            AudioFileStreamGetProperty(pInfo->fileStreamId, kAudioFileStreamProperty_BitRate, &size, &bitrate);
//            pInfo->pInfo->bitRate = bitrate;
//            
//            pInfo->pInfo->channels = mono ? 1 : 2;
//            pInfo->pInfo->sampleRate = (int)desc.mSampleRate;
//        }
//        
//        u32 sizePerPacket = desc.mBytesPerPacket;
//        
//        if (sizePerPacket != 0)
//        {
//            u8* data = (u8*)inInputData;
//            for (u32 i = 1; i <= inNumberPackets; i++)
//            {
//                (*pInfo->pFrameHandle)(
//                                       data,
//                                       sizePerPacket,
//                                       af,
//                                       (s32)desc.mSampleRate,
//                                       pInfo->finish && i == inNumberPackets);
//                data += sizePerPacket;
//            }
//        }
//        else
//        {
//            size_t bytesPerSample = sizeof (AudioUnitSampleType);
//            AudioStreamBasicDescription desDest = {0};
//            
//            desDest.mFormatID          = kAudioFormatLinearPCM;
//            desDest.mFormatFlags       = kAudioFormatFlagsAudioUnitCanonical;
//            desDest.mBytesPerPacket    = bytesPerSample;
//            desDest.mBytesPerFrame     = bytesPerSample;
//            desDest.mFramesPerPacket   = 1;
//            desDest.mBitsPerChannel    = 8 * bytesPerSample;
//            desDest.mChannelsPerFrame  = 2;           // 2 indicates stereo
//            desDest.mSampleRate        = desc.mSampleRate;
//            
//            
//            bytesPerSample = sizeof (AudioSampleType);
//            AudioStreamBasicDescription desDest2 = {0};
//            
//            desDest2.mFormatID          = kAudioFormatLinearPCM;
//            desDest2.mFormatFlags       = kAudioFormatFlagsCanonical;
//            desDest2.mBytesPerPacket    = bytesPerSample;
//            desDest2.mBytesPerFrame     = bytesPerSample;
//            desDest2.mFramesPerPacket   = 1;
//            desDest2.mBitsPerChannel    = 8 * bytesPerSample;
//            desDest2.mChannelsPerFrame  = 1;           // 2 indicates stereo
//            desDest2.mSampleRate        = desc.mSampleRate;
//            
//            AudioConverterRef converterRef;
//            OSStatus err = AudioConverterNew(&desc, &desDest2, &converterRef);
//            if (0 != err)
//            {
//                ERR_MSG(err);
//                return;
//            }
//            
//            FixArray<char> outBuf(102400);//desDest2.mBytesPerPacket);
//            
//            AudioBufferList outBufList;
//            outBufList.mNumberBuffers = 1;
//            outBufList.mBuffers[0].mNumberChannels = desDest2.mChannelsPerFrame;
//            outBufList.mBuffers[0].mDataByteSize = outBuf.size();
//            outBufList.mBuffers[0].mData = outBuf.begin();
//            
//            for (u32 i = 0; i < inNumberPackets; i++)
//            {
//                AudioBufferList inBufList;
//                inBufList.mNumberBuffers = 1;
//                inBufList.mBuffers[0].mNumberChannels = desc.mChannelsPerFrame;
//                inBufList.mBuffers[0].mDataByteSize = inPacketDescriptions->mDataByteSize;
//                inBufList.mBuffers[0].mData = (u8*)inInputData + inPacketDescriptions->mStartOffset;
//                
//                OSStatus err = AudioConverterConvertComplexBuffer(
//                                                                  converterRef,
//                                                                  inPacketDescriptions->mVariableFramesInPacket ? inPacketDescriptions->mVariableFramesInPacket : desc.mFramesPerPacket,
//                                                                  &inBufList, &outBufList);
//                if (0 != err)
//                {
//                    ERR_MSG(err);
//                    AudioConverterDispose(converterRef);
//                    pInfo->result = false;
//                    return;
//                }
//                
//                inPacketDescriptions++;
//                
////                (*pInfo->pFrameHandle)(
////                                       outBufList,
////                                       sizePerPacket,
////                                       af,
////                                       (s32)desc.mSampleRate,
////                                       pInfo->finish && i == inNumberPackets);
//
//            }
//            
//            AudioConverterDispose(converterRef);
//        } // if..else
//        
//        pInfo->result = true;
//    }
    
    bool GetOpenALAudioData(CFURLRef inFileURL, AudioFormat& format, int& sampling, AudioFrameEvent& frameHandle, AudioInfo* pInfo)
    {
        OSStatus                        err = noErr;
        SInt64                          theFileLengthInFrames = 0;
        AudioStreamBasicDescription     theFileFormat;
        UInt32                          thePropertySize = sizeof(theFileFormat);
        ExtAudioFileRef                 extRef = NULL;
        void*                           theData = NULL;
        AudioStreamBasicDescription     theOutputFormat;
        
        // Open a file with ExtAudioFileOpen()
        err = ExtAudioFileOpenURL(inFileURL, &extRef);
        if(err) { error_all(_T("ExtAudioFileOpenURL FAILED, Error = %ld"), err);
            if (extRef) ExtAudioFileDispose(extRef);
            return false;
        }
        
        // Get the audio data format
        err = ExtAudioFileGetProperty(extRef, kExtAudioFileProperty_FileDataFormat, &thePropertySize, &theFileFormat);
        if(err) { error_all(_T("ExtAudioFileGetProperty(kExtAudioFileProperty_FileDataFormat) FAILED, Error = %ld"), err);
            if (extRef) ExtAudioFileDispose(extRef);
            return false;
        }
        if (theFileFormat.mChannelsPerFrame > 2)  { error_all(_T("Unsupported Format, channel count is greater than stereo"));
            if (extRef) ExtAudioFileDispose(extRef);
            return false;
        }
        
        // Set the client format to 16 bit signed integer (native-endian) data
        // Maintain the channel count and sample rate of the original source format
        
        if (sampling == 0)
        {
            theOutputFormat.mSampleRate = theFileFormat.mSampleRate;
            if (theOutputFormat.mSampleRate > 48000)
                theOutputFormat.mSampleRate = 48000;
            
            sampling = theOutputFormat.mSampleRate;
        }
        else
        {
            theOutputFormat.mSampleRate = sampling;
        }
        
        if (format == AudioFormat_undefined)
        {
            theOutputFormat.mChannelsPerFrame = theFileFormat.mChannelsPerFrame;
            theOutputFormat.mBitsPerChannel = theFileFormat.mBitsPerChannel != 0 ? theFileFormat.mBitsPerChannel : 16;
            if (theOutputFormat.mBitsPerChannel > 16)   theOutputFormat.mBitsPerChannel = 16;
            
            if (theOutputFormat.mChannelsPerFrame == 2)
            {
                if (theOutputFormat.mBitsPerChannel == 16)
                    format = AudioFormat_stereo16;
                else
                    format = AudioFormat_stereo8;
            }
            else
            {
                if (theOutputFormat.mBitsPerChannel == 16)
                    format = AudioFormat_mono16;
                else
                    format = AudioFormat_mono8;
            }
        }
        else
        {
            if (_isStereoFormat(format))
                theOutputFormat.mChannelsPerFrame = 2;
            else
                theOutputFormat.mChannelsPerFrame = 1;
            
            if (AudioFormat_mono16 == format || AudioFormat_stereo16 == format)
                theOutputFormat.mBitsPerChannel = 16;
            else
                theOutputFormat.mBitsPerChannel = 8;
        }
        theOutputFormat.mFormatID = kAudioFormatLinearPCM;
        theOutputFormat.mBytesPerPacket = 2 * theOutputFormat.mChannelsPerFrame;
        theOutputFormat.mFramesPerPacket = 1;
        theOutputFormat.mBytesPerFrame = 2 * theOutputFormat.mChannelsPerFrame;
        theOutputFormat.mFormatFlags = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;

        if (pInfo)
        {
            pInfo->bitRate = 0; // can't read bitRate;
            pInfo->channels = theFileFormat.mChannelsPerFrame;
            pInfo->sampleRate = theFileFormat.mSampleRate;
        }
        
        // Set the desired client (output) data format
        err = ExtAudioFileSetProperty(extRef, kExtAudioFileProperty_ClientDataFormat, sizeof(theOutputFormat), &theOutputFormat);
        if(err) { error_all(_T("ExtAudioFileSetProperty(kExtAudioFileProperty_ClientDataFormat) FAILED, Error = %ld"), err);
            if (extRef) ExtAudioFileDispose(extRef);
            return false;
        }
        
        // Get the total frame count
        thePropertySize = sizeof(theFileLengthInFrames);
        err = ExtAudioFileGetProperty(extRef, kExtAudioFileProperty_FileLengthFrames, &thePropertySize, &theFileLengthInFrames);
        if(err) { error_all(_T("ExtAudioFileGetProperty(kExtAudioFileProperty_FileLengthFrames) FAILED, Error = %ld"), err);
            if (extRef) ExtAudioFileDispose(extRef);
            return false;
        }
        
        AudioFormat outDataFormat = (theOutputFormat.mChannelsPerFrame > 1) ? AudioFormat_stereo16 : AudioFormat_mono16;
        bool r = true;
        
        // Read all the data into memory
        UInt32 theFramesToReadTotal = (UInt32)theFileLengthInFrames;
        //UInt32 dataSize = theFramesToRead * theOutputFormat.mBytesPerFrame;
        UInt32 dataSize = BUF_SIZE < theOutputFormat.mBytesPerFrame ? theOutputFormat.mBytesPerFrame : BUF_SIZE;
        UInt32 theFramesToReadPer = dataSize / theOutputFormat.mBytesPerFrame;  if (theFramesToReadPer <= 0) theFramesToReadPer = 1;
        dataSize = theFramesToReadPer * theOutputFormat.mBytesPerFrame;
        theData = malloc(dataSize);
        
        if (theData)
        {
            AudioBufferList     theDataBuffer;
            theDataBuffer.mNumberBuffers = 1;
            theDataBuffer.mBuffers[0].mDataByteSize = dataSize;
            theDataBuffer.mBuffers[0].mNumberChannels = theOutputFormat.mChannelsPerFrame;
            theDataBuffer.mBuffers[0].mData = theData;
            
            // Read the data into an AudioBufferList
            UInt32 ioNumberFrame;
            UInt32 theFrameReaded = 0;
            while (theFrameReaded < theFramesToReadTotal)
            {
                ioNumberFrame = theFramesToReadPer;
                err = ExtAudioFileRead(extRef, &ioNumberFrame, &theDataBuffer);
                if(err == noErr)
                {
                    // success
                    if (ioNumberFrame == 0)
                    {
                        frameHandle(NULL, 0,
                                    outDataFormat,
                                    theOutputFormat.mSampleRate,
                                    1.0f);
                        break;
                    }
                    
                    theFrameReaded += ioNumberFrame;
                    if (!frameHandle(theData, theDataBuffer.mBuffers[0].mDataByteSize,
                                     outDataFormat,
                                     theOutputFormat.mSampleRate,
                                     theFrameReaded / (f32)theFramesToReadTotal))
                    {
                        r = false;
                        break;
                    }
                }
                else
                {
                    // failure
                    error_all(_T("ExtAudioFileRead FAILED, Error = %ld"), err);
                    r = false;
                    break;
                }
            } // while.
        }
        else
        {
            r = false;
        }
        
        // Dispose the ExtAudioFileRef, it is no longer needed
        if (extRef) ExtAudioFileDispose(extRef);
        if (theData) free(theData);
        
        return r;
    }
    
    //-----------------------------------------------
    Codec::Codec()
    {
    }
    
    //-----------------------------------------------
    Codec::~Codec()
    {
        
    }
    
    //-----------------------------------------------
    bool Codec::openFile( const tchar* filename, AudioFormat& format, int& sampling, AudioFrameEvent& frameHandle, AudioInfo* pInfo )
    {
        bool r = false;
        
        String bundlepath = FileUtils::PathBundle(filename);
        
        //
        // get audio informat.
        //
        CFStringRef         filepath = CFStringCreateWithCString(kCFAllocatorDefault, bundlepath.c_str(), kCFStringEncodingUTF8);
        CFURLRef            fileRef  = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, filepath, kCFURLPOSIXPathStyle, false);
        CFRelease(filepath);
        
        r = GetOpenALAudioData(fileRef, format, sampling, frameHandle, pInfo);
        CFRelease(fileRef);
        
        return r;
        
        
        
        
        
//        OSStatus status;
//        bool r = false;
//        
//        String bundlepath = FileUtils::PathBundle(filename);
//        
//        //
//        // get audio informat.
//        //
//        
//        ExtAudioFileRef     extRef = NULL;
//        CFStringRef         filepath = CFStringCreateWithCString(kCFAllocatorDefault, bundlepath.c_str(), kCFStringEncodingUTF8);
//        CFURLRef            fileRef  = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, filepath, kCFURLPOSIXPathStyle, false);
//        CFRelease(filepath);
//        
//        AudioStreamBasicDescription     theFileFormat;
//        AudioStreamBasicDescription     theOutputFormat;
//        UInt32                          thePropertySize = sizeof(theFileFormat);
//        
//        // Open a file with ExtAudioFileOpen()
//        status = ExtAudioFileOpenURL(fileRef, &extRef);
//        CFRelease(fileRef);
//        
//        if(status) { error_all(_T("ExtAudioFileOpenURL FAILED, Error = %ld"), status);
//            if (extRef) ExtAudioFileDispose(extRef);
//            return r;
//        }
//        
//        // Get the audio data format
//        status = ExtAudioFileGetProperty(extRef, kExtAudioFileProperty_FileDataFormat, &thePropertySize, &theFileFormat);
//        if(status) { error_all(_T("ExtAudioFileGetProperty(kExtAudioFileProperty_FileDataFormat) FAILED, Error = %ld"), status);
//            if (extRef) ExtAudioFileDispose(extRef);
//            return r; }
//        if (theFileFormat.mChannelsPerFrame > 2)  { error_all(_T("Unsupported Format, channel count is greater than stereo"));
//            if (extRef) ExtAudioFileDispose(extRef);
//            return r;}
//        
//        // Set the client format to 16 bit signed integer (native-endian) data
//        // Maintain the channel count and sample rate of the original source format
//        theOutputFormat.mSampleRate = theFileFormat.mSampleRate;
//        theOutputFormat.mChannelsPerFrame = theFileFormat.mChannelsPerFrame;
//        
//        theOutputFormat.mFormatID = kAudioFormatLinearPCM;
//        theOutputFormat.mBytesPerPacket = 2 * theOutputFormat.mChannelsPerFrame;
//        theOutputFormat.mFramesPerPacket = 1;
//        theOutputFormat.mBytesPerFrame = 2 * theOutputFormat.mChannelsPerFrame;
//        theOutputFormat.mBitsPerChannel = 16;
//        theOutputFormat.mFormatFlags = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
//        
//        // close.
//        ExtAudioFileDispose(extRef);
//        
////        // Set the desired client (output) data format
////        status = ExtAudioFileSetProperty(extRef, kExtAudioFileProperty_ClientDataFormat, sizeof(theOutputFormat), &theOutputFormat);
////        if(status) { error_all(_T("ExtAudioFileSetProperty(kExtAudioFileProperty_ClientDataFormat) FAILED, Error = %ld"), status);
////            if (extRef) ExtAudioFileDispose(extRef);
////            return r;
////        }
////
////        // Get the total frame count
////        thePropertySize = sizeof(theFileLengthInFrames);
////        err = ExtAudioFileGetProperty(extRef, kExtAudioFileProperty_FileLengthFrames, &thePropertySize, &theFileLengthInFrames);
////        if(err) { error_all(_T("ExtAudioFileGetProperty(kExtAudioFileProperty_FileLengthFrames) FAILED, Error = %ld"), err);
////            if (extRef) ExtAudioFileDispose(extRef);
////            return NULL;
////        }
//        
//        
//        //
//        // parse stream.
//        //
//        
//        Info info;
//        info.pFrameHandle = &frameHandle;
//        info.pInfo = pInfo;
//        info.finish = false;
//        info.result = false;
//        
//        status = AudioFileStreamOpen(&info, NULL, audioFileStream_PacketsProc, 0, &info.fileStreamId);
//        if (0 != status)
//        {
//            error_all(_T("open filestream error: %ld"), status);
//            return r;
//        }
//        
//        status = AudioFileStreamSetProperty(info.fileStreamId, kAudioFileStreamProperty_DataFormat, sizeof(theOutputFormat), &theOutputFormat);
//        if (0 != status)
//        {
//            ERR_MSG(status);
//            error_all(_T("set property error: %ld"), status);
//            return r;
//        }
//        
//        FileStream fs;
//        if (fs.open(bundlepath.c_str(), FileStream::FM_Binary|FileStream::FM_Read))
//        {
//            r = true;
//            
//            // 分批解析.
//            FixArray<u8>    arrBuf(BUF_SIZE);
//            u32 i = 0;
//            while (i < fs.getByteSize())
//            {
//                s32 size = fs.readByte(arrBuf.begin(), arrBuf.size());
//                if (size <= 0)   break;
//                i += size;
//                
//                if (i >= fs.getByteSize())
//                    info.finish = true;
//                
//                status = AudioFileStreamParseBytes(info.fileStreamId, size, arrBuf.begin(), info.finish ? 0 : kAudioFileStreamParseFlag_Discontinuity);
//                if (0 != status)
//                {
//                    r = false;
//                    break;
//                }
//            }
//            
//            arrBuf.resize(0);
//            
////            // 一次性解析.
////            FixArray<u8>    arrBuf(fs.getByteSize());
////            if (arrBuf.size() == fs.readByte(arrBuf.begin(), arrBuf.size()))
////            {
////                status = AudioFileStreamParseBytes(info.fileStreamId, (u32)arrBuf.size(), arrBuf.begin(), 0);
////                if (0 == status && info.result)
////                {
////                    r = true;
////                }
////            }
////            arrBuf.resize(0);
//        }
//        
//        AudioFileStreamClose(info.fileStreamId);
//        return r && info.result;
    }
    
}