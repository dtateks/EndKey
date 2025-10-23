//
//  AppDelegate.m
//  EndKeyHelper
//
//  Created by Tuyen on 2/1/19.
//  Copyright © 2019 Tuyen Mai. All rights reserved.
//

#import "AppDelegate.h"

@interface AppDelegate ()

@property (weak) IBOutlet NSWindow *window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification {
    NSArray* runningApps = [[NSWorkspace sharedWorkspace] runningApplications];
    BOOL isEndKeyRunning = NO;
    for (NSRunningApplication* app in runningApps) {
        if ([[app bundleIdentifier] isEqualToString:@"com.tuyenmai.endkey"]) {
            isEndKeyRunning = YES;
            break;
        }
    }

    if (!isEndKeyRunning) {
        NSString* path = [[NSBundle mainBundle] bundlePath];
        for (int i = 0; i < 4; i++)
            path = [path stringByDeletingLastPathComponent];
        [[NSWorkspace sharedWorkspace] launchApplication:path];
    }
}


- (void)applicationWillTerminate:(NSNotification *)aNotification {
    // Insert code here to tear down your application
}


@end
