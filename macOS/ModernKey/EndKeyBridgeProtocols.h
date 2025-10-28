//
//  EndKeyBridgeProtocols.h
//  EndKey
//
//  Created by Refactoring on 2025.
//  Copyright © 2025 Tuyen Mai. All rights reserved.
//

#ifndef ENDKEY_BRIDGE_PROTOCOLS_H
#define ENDKEY_BRIDGE_PROTOCOLS_H

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import "Engine.h"

@class EndKey::RAII::CGEventWrapper;
@class EndKey::RAII::CGEventSourceWrapper;

namespace EndKey {

// Event types for better type safety
typedef enum {
    EndKeyEventTypeNone = 0,
    EndKeyEventTypeKeyDown,
    EndKeyEventTypeKeyUp,
    EndKeyEventTypeFlagsChanged,
    EndKeyEventTypeMouseDown,
    EndKeyEventTypeMouseUp,
    EndKeyEventTypeMouseDragged
} EndKeyEventType;

// Event processing result
typedef enum {
    EndKeyEventResultPassThrough,  // Let event pass through
    EndKeyEventResultConsumed,     // Event was handled, don't pass through
    EndKeyEventResultError         // Error processing event
} EndKeyEventResult;

// Configuration keys
typedef NSString* EndKeyConfigurationKey NS_STRING_ENUM;

extern EndKeyConfigurationKey const EndKeyConfigCodeTable;
extern EndKeyConfigurationKey const EndKeyConfigCheckSpelling;
extern EndKeyConfigurationKey const EndKeyConfigUseMacro;
extern EndKeyConfigurationKey const EndKeyConfigLanguage;
extern EndKeyConfigurationKey const EndKeyConfigSendKeyStepByStep;
extern EndKeyConfigurationKey const EndKeyConfigTempOffEndKey;
extern EndKeyConfigurationKey const EndKeyConfigOtherLanguage;

// Error types
typedef NSError EndKeyError;
extern NSErrorDomain const EndKeyErrorDomain;

typedef NS_ERROR_ENUM(EndKeyErrorDomain, EndKeyErrorCode) {
    EndKeyErrorInvalidEvent = 1000,
    EndKeyErrorMemoryAllocation,
    EndKeyErrorPermissionDenied,
    EndKeyErrorConfigurationInvalid,
    EndKeyErrorEngineFailure,
    EndKeyErrorSystemIntegration
};

#pragma mark - Input Event Manager Protocol

@protocol EndKeyInputEventManager <NSObject>

/**
 * Handles raw system events and routes them to appropriate handlers
 * @param type The event type
 * @param event The CGEvent object
 * @param proxy The event tap proxy
 * @return Processing result
 */
- (EndKeyEventResult)handleEvent:(EndKeyEventType)type
                          event:(CGEventRef)event
                          proxy:(CGEventTapProxy)proxy;

/**
 * Registers hotkey combinations
 * @param keyCode The key code
 * @param modifiers Modifier flags
 * @param handler Block to execute when hotkey is triggered
 * @return Registration success
 */
- (BOOL)registerHotkey:(CGKeyCode)keyCode
            modifiers:(CGEventFlags)modifiers
               handler:(void(^)(void))handler;

/**
 * Unregisters a specific hotkey
 * @param keyCode The key code to unregister
 * @param modifiers Modifier flags
 */
- (void)unregisterHotkey:(CGKeyCode)keyCode
               modifiers:(CGEventFlags)modifiers;

/**
 * Sets the event processing enabled state
 * @param enabled Whether event processing should be enabled
 */
- (void)setEventProcessingEnabled:(BOOL)enabled;

/**
 * Gets current event processing state
 * @return Whether event processing is enabled
 */
- (BOOL)isEventProcessingEnabled;

@end

#pragma mark - UI Manager Protocol

@protocol EndKeyUIManager <NSObject>

/**
 * Loads configuration from persistent storage
 * @return Success status and error if any
 */
- (BOOL)loadConfiguration:(NSError**)error;

/**
 * Saves current configuration to persistent storage
 * @return Success status and error if any
 */
- (BOOL)saveConfiguration:(NSError**)error;

/**
 * Gets a configuration value
 * @param key Configuration key
 * @return The configuration value
 */
- (id)configurationValueForKey:(EndKeyConfigurationKey)key;

/**
 * Sets a configuration value
 * @param value The value to set
 * @param key Configuration key
 * @return Success status
 */
- (BOOL)setConfigurationValue:(id)value
                       forKey:(EndKeyConfigurationKey)key;

/**
 * Registers for configuration change notifications
 * @param observer Observer object
 * @param key Specific key to observe, nil for all keys
 * @param selector Selector to call on changes
 */
- (void)registerConfigurationObserver:(id)observer
                                 key:(EndKeyConfigurationKey)key
                             selector:(SEL)selector;

/**
 * Unregisters configuration change observer
 * @param observer Observer object
 * @param key Specific key to stop observing
 */
- (void)unregisterConfigurationObserver:(id)observer
                                   key:(EndKeyConfigurationKey)key;

/**
 * Synchronizes configuration with C++ engine
 * @return Success status
 */
- (BOOL)synchronizeWithEngine:(NSError**)error;

@end

#pragma mark - System Integration Protocol

@protocol EndKeySystemIntegration <NSObject>

/**
 * Checks if the application has necessary permissions
 * @return Permission status
 */
- (BOOL)checkPermissions:(NSError**)error;

/**
 * Requests necessary permissions from the user
 * @return Request success
 */
- (BOOL)requestPermissions:(NSError**)error;

/**
 * Gets the current frontmost application identifier
 * @return Bundle identifier or nil if unavailable
 */
- (NSString*)getCurrentFrontmostApplication;

/**
 * Registers for application change notifications
 * @param handler Block to execute when frontmost app changes
 */
- (void)registerApplicationChangeHandler:(void(^)(NSString* bundleIdentifier))handler;

/**
 * Gets the current keyboard input source
 * @return Input source identifier or nil
 */
- (NSString*)getCurrentInputSource;

/**
 * Coordinates with helper applications
 * @return Coordination success
 */
- (BOOL)coordinateWithHelperApps:(NSError**)error;

@end

#pragma mark - Event Sender Protocol

@protocol EndKeyEventSender <NSObject>

/**
 * Sends a Unicode character sequence
 * @param characters Unicode characters to send
 * @param length Number of characters
 * @param proxy Event tap proxy
 * @return Send success
 */
- (BOOL)sendUnicodeCharacters:(const UniChar*)characters
                       length:(UniCharCount)length
                        proxy:(CGEventTapProxy)proxy;

/**
 * Sends a key code with optional modifiers
 * @param keyCode Key code to send
 * @param modifiers Modifier flags
 * @param keyDown Whether this is a key down event
 * @param proxy Event tap proxy
 * @return Send success
 */
- (BOOL)sendKeyCode:(CGKeyCode)keyCode
          modifiers:(CGEventFlags)modifiers
            keyDown:(BOOL)keyDown
              proxy:(CGEventTapProxy)proxy;

/**
 * Sends backspace key
 * @param count Number of backspaces to send
 * @param proxy Event tap proxy
 * @return Send success
 */
- (BOOL)sendBackspace:(NSUInteger)count
                proxy:(CGEventTapProxy)proxy;

/**
 * Sends a sequence of characters efficiently
 * @param data Character data from engine
 * @param count Number of characters
 * @param fromMacro Whether data is from macro expansion
 * @param proxy Event tap proxy
 * @return Send success
 */
- (BOOL)sendCharacterSequence:(const Uint32*)data
                        count:(size_t)count
                    fromMacro:(BOOL)fromMacro
                         proxy:(CGEventTapProxy)proxy;

/**
 * Sets the event source for creating events
 * @param source Event source to use
 */
- (void)setEventSource:(CGEventSourceRef)source;

/**
 * Gets current event source
 * @return Current event source
 */
- (CGEventSourceRef)eventSource;

@end

#pragma mark - Bridge Coordinator Protocol

@protocol EndKeyBridgeCoordinator <NSObject>

/**
 * Initializes the bridge system
 * @return Initialization success
 */
- (BOOL)initializeBridge:(NSError**)error;

/**
 * Shuts down the bridge system
 */
- (void)shutdownBridge;

/**
 * Main event callback - bridges system events to internal handlers
 * @param proxy Event tap proxy
 * @param type Event type
 * @param event Event object
 * @param refcon User data
 * @return Processed event or original event
 */
- (CGEventRef)handleSystemEvent:(CGEventTapProxy)proxy
                          type:(CGEventType)type
                         event:(CGEventRef)event
                        refcon:(void*)refcon;

/**
 * Gets the input event manager
 * @return Input event manager instance
 */
- (id<EndKeyInputEventManager>)inputEventManager;

/**
 * Gets the UI manager
 * @return UI manager instance
 */
- (id<EndKeyUIManager>)uiManager;

/**
 * Gets the system integration manager
 * @return System integration manager instance
 */
- (id<EndKeySystemIntegration>)systemIntegrationManager;

/**
 * Gets the event sender
 * @return Event sender instance
 */
- (id<EndKeyEventSender>)eventSender;

@end

#pragma mark - Notification Names

extern NSNotificationName const EndKeyConfigurationDidChangeNotification;
extern NSNotificationName const EndKeyApplicationDidChangeNotification;
extern NSNotificationName const EndKeyInputSourceDidChangeNotification;
extern NSNotificationName const EndKeyPermissionStatusDidChangeNotification;

#pragma mark - Notification User Info Keys

extern NSString* const EndKeyConfigurationKeyUserInfoKey;
extern NSString* const EndKeyConfigurationOldValueUserInfoKey;
extern NSString* const EndKeyConfigurationNewValueUserInfoKey;
extern NSString* const EndKeyApplicationBundleIdentifierUserInfoKey;
extern NSString* const EndKeyInputSourceIdentifierUserInfoKey;
extern NSString* const EndKeyPermissionGrantedUserInfoKey;

} // namespace EndKey

#endif /* ENDKEY_BRIDGE_PROTOCOLS_H */