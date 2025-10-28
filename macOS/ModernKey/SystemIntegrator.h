//
//  SystemIntegrator.h
//  EndKey
//
//  Created by EndKey Refactoring on 2025.
//  Copyright © 2025 Tuyen Mai. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>
#include "../engine/EngineCore.h"

@interface SystemIntegrator : NSObject

// Singleton pattern
+ (instancetype)sharedIntegrator;

// Application monitoring
@property (nonatomic, strong) NSString* currentApplication;
@property (nonatomic, strong) NSString* previousApplication;
@property (nonatomic, strong) NSRunningApplication* frontApp;

// Accessibility and permissions
@property (nonatomic, assign) BOOL hasAccessibilityPermissions;
@property (nonatomic, assign) BOOL hasInputMonitoringPermissions;

// Smart switching configuration
@property (nonatomic, assign) BOOL smartSwitchEnabled;
@property (nonatomic, strong) NSMutableDictionary<NSString*, NSNumber*>* applicationLanguages;
@property (nonatomic, strong) NSMutableDictionary<NSString*, NSNumber*>* applicationCodeTables;

// Helper applications
@property (nonatomic, strong) NSTask* helperTask;
@property (nonatomic, strong) NSTask* updateTask;

// Auto-start management
@property (nonatomic, assign) BOOL launchAtLogin;
@property (nonatomic, strong) NSString* launchAgentPath;

// System notification handling
@property (nonatomic, strong) NSNotificationCenter* notificationCenter;

// Application lifecycle
- (void)startApplicationMonitoring;
- (void)stopApplicationMonitoring;
- (void)applicationDidActivate:(NSNotification*)notification;
- (void)applicationDidDeactivate:(NSNotification*)notification;

// Permission management
- (BOOL)checkAccessibilityPermissions;
- (BOOL)checkInputMonitoringPermissions;
- (void)requestAccessibilityPermissions;
- (void)requestInputMonitoringPermissions;
- (void)openAccessibilityPreferences;

// Smart switching logic
- (void)enableSmartSwitching:(BOOL)enabled;
- (void)setApplicationLanguage:(NSString*)bundleIdentifier language:(int)language;
- (void)setApplicationCodeTable:(NSString*)bundleIdentifier codeTable:(int)codeTable;
- (int)getApplicationLanguage:(NSString*)bundleIdentifier;
- (int)getApplicationCodeTable:(NSString*)bundleIdentifier;
- (void)switchLanguageForApplication:(NSString*)bundleIdentifier;

// Application detection
- (NSString*)getCurrentApplicationIdentifier;
- (NSString*)getApplicationDisplayName:(NSString*)bundleIdentifier;
- (BOOL)isSystemApplication:(NSString*)bundleIdentifier;
- (BOOL)isBrowserApplication:(NSString*)bundleIdentifier;
- (BOOL)isDevelopmentApplication:(NSString*)bundleIdentifier;

// Configuration persistence
- (void)loadApplicationSettings;
- (void)saveApplicationSettings;
- (void)resetApplicationSettings;
- (NSString*)getSettingsFilePath;

// Helper application management
- (void)startHelperApplication;
- (void)stopHelperApplication;
- (BOOL)isHelperApplicationRunning;
- (NSString*)getHelperApplicationPath;

// Update management
- (void)checkForUpdates;
- (void)enableAutoUpdate:(BOOL)enabled;
- (NSString*)getCurrentVersion;
- (NSString*)getLatestVersion;

// Auto-start management
- (void)setLaunchAtLogin:(BOOL)enabled;
- (BOOL)isLaunchAtLoginEnabled;
- (void)createLaunchAgent;
- (void)removeLaunchAgent;
- (void)installLaunchAgent;

// System integration utilities
- (void)registerGlobalHotKeys;
- (void)unregisterGlobalHotKeys;
- (BOOL)handleGlobalHotKey:(NSEvent*)event;

// System notifications
- (void)registerForSystemNotifications;
- (void)unregisterForSystemNotifications;
- (void)handleSystemSleep:(NSNotification*)notification;
- (void)handleSystemWake:(NSNotification*)notification;
- (void)handleScreenParametersChanged:(NSNotification*)notification;

// Engine integration
- (void)configureEngineForApplication:(std::shared_ptr<EngineCore>)engineCore
                    bundleIdentifier:(NSString*)bundleIdentifier;
- (void)applyApplicationSpecificSettings:(std::shared_ptr<EngineCore>)engineCore;

// Performance monitoring
- (void)startPerformanceMonitoring;
- (void)stopPerformanceMonitoring;
- (NSDictionary*)getPerformanceMetrics;

// Cleanup and shutdown
- (void)cleanup;
- (void)prepareForShutdown;

@end