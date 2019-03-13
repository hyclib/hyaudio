//
//  AppDelegate.m
//  hyaudioDemo
//
//  Created by brainpoint on 14-7-9.
//  Copyright (c) 2014年 hy. All rights reserved.
//

#import "AppDelegate.h"
#import "hyaudio.h"

hyaudio::IAudioSystem* pSys = NULL;

@implementation AppDelegate

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    self.window = [[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
    // Override point for customization after application launch.
    self.window.backgroundColor = [UIColor whiteColor];
    [self.window makeKeyAndVisible];
    
    pSys = hyaudio::CreateAudioSystem(hyaudio::AudioSystemType_openal);
    hyaudio::IAudio* pAudio = pSys->createAudio(
                                                _T("assets/demo.mp3"),
                                                hyaudio::AudioFormat_mono16,
                                                hyaudio::AudioSampling_undefined);
    if (pAudio)
    {
        pAudio->setLooping(true);
        pAudio->play();
        
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            f32 m = 0.1;
            while (true)
            {
                Thread::Sleep(100);
                pAudio->move(m,0,0);
                
                if (Math::Abs(pAudio->getPosition().x) > 3)
                    m = -m;
            }
        });
    }

    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}

@end
