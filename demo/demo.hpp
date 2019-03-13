
#include "hyaudio/include/hyaudio.h"
using namespace hyaudio;

int main()
{
    IAudioSystem* pSys = CreateAudioSystem(AudioSystemType_openal);

    IAudio* pAudio = pSys->createAudio(_T("../../demo/assets/demo.ogg"));
    if (pAudio)
    {
        pAudio->setLooping(true);
        pAudio->play();
    }

    f32 m = 0.01f;
    while (true)
    {
        Sleep(100);
        pAudio->move(m, 0, 0);

        if (Math::Abs(pAudio->getPosition().x) > 0.2)
        {
            print_all_line(_T("%f"), pAudio->getPosition().x);
            m = -m;
        }



//         if (Math::Equals(pAudio->getPosition().x, 0))
//         {
//             pAudio->setPosition(0, 0, 0);
//         }
    }

    system("pause");

    pSys->release();
    return 0;
}