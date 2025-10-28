//
//  EventManager.h
//  EndKey
//
//  Created by EndKey Refactoring on 2025.
//  Copyright © 2025 Tuyen Mai. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <Carbon/Carbon.h>
#include "../engine/EngineCore.h"

@interface EventManager : NSObject

// Event handling configuration
@property (nonatomic, assign) BOOL isEnabled;
@property (nonatomic, assign) BOOL eventTapEnabled;
@property (nonatomic, assign) CGEventTapProxy proxy;

// Key event processing
@property (nonatomic, assign) CGEventSourceRef eventSource;
@property (nonatomic, assign) CGEventFlags lastFlags;
@property (nonatomic, assign) CGEventFlags privateFlags;

// Character conversion buffers
@property (nonatomic, assign) Uint16 newCharString[20];
@property (nonatomic, assign) Uint16 newCharSize;
@property (nonatomic, assign) UniChar newChar;
@property (nonatomic, assign) UniChar newCharHi;
@property (nonatomic, assign) CGKeyCode keycode;
@property (nonatomic, assign) CGEventFlags flag;

// Double space period handling
@property (nonatomic, assign) int consecutiveSpaceCount;

// Event state
@property (nonatomic, assign) BOOL willContinueSending;
@property (nonatomic, assign) BOOL willSendControlKey;

// Singleton pattern
+ (instancetype)sharedManager;

// Event tap management
- (BOOL)createEventTap;
- (void)destroyEventTap;
- (void)enableEventTap:(BOOL)enabled;

// Event processing
- (CGEventRef)processKeyEvent:(CGEventRef)event proxy:(CGEventTapProxy)proxy;
- (BOOL)shouldProcessEvent:(CGEventRef)event;
- (CGEventRef)handleKeyDownEvent:(CGEventRef)event;
- (CGEventRef)handleKeyUpEvent:(CGEventRef)event;
- (CGEventRef)handleFlagsChangedEvent:(CGEventRef)event;

// Character conversion and output
- (void)convertCharacter:(Uint32)character;
- (void)sendCharacterEvent:(Uint32)character;
- (void)sendBackspaceEvent:(int)count;
- (void)sendMacroExpansion:(const std::wstring&)macro;

// Application-specific handling
- (BOOL)requiresSpecialHandling:(NSString*)bundleIdentifier;
- (BOOL)supportsUnicodeCompound:(NSString*)bundleIdentifier;
- (void)handleApplicationSpecificLogic:(NSString*)bundleIdentifier;

// Key mapping and validation
- (CGKeyCode)keyCodeFromString:(NSString*)character;
- (BOOL)isValidCharacter:(UniChar)character;
- (BOOL)isControlKey:(CGEventFlags)flags;

// Double space period functionality
- (void)handleDoubleSpacePeriod:(CGEventRef)event;
- (void)resetSpaceCount;

// Engine integration
- (void)setEngineCore:(std::shared_ptr<EngineCore>)engineCore;
- (EngineHookState)processEngineEvent:(const vKeyEvent&)event
                                 state:(const vKeyEventState&)state
                                 data:(const Uint16&)data
                           capsStatus:(const Uint8&)capsStatus
                   otherControlKey:(const bool&)otherControlKey;

// Configuration and state
- (void)loadConfiguration;
- (void)saveConfiguration;
- (void)resetState;

// Cleanup
- (void)cleanup;

@end