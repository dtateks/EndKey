//
//  InputEventManager.h
//  EndKey
//
//  Created by Refactoring on 2025.
//  Copyright © 2025 Tuyen Mai. All rights reserved.
//

#ifndef INPUT_EVENT_MANAGER_H
#define INPUT_EVENT_MANAGER_H

#import <Foundation/Foundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import "EndKeyBridgeProtocols.h"
#import "RAIIWrappers.h"

@class EndKeyInputEventManager;

namespace EndKey {

// Hotkey registration structure
struct HotkeyRegistration {
    CGKeyCode keyCode;
    CGEventFlags modifiers;
    void(^handler)(void);
    NSUInteger identifier;
};

// Event handler block types
typedef void(^KeyboardEventHandler)(CGKeyCode keyCode, CGEventFlags flags, CGEventTapProxy proxy);
typedef void(^MouseEventHandler)(CGEventType type, CGEventTapProxy proxy);
typedef void(^FlagsChangedHandler)(CGEventFlags newFlags, CGEventFlags oldFlags, CGEventTapProxy proxy);

/**
 * High-performance input event manager with dispatcher pattern
 * Handles keyboard, mouse, and hotkey events with proper threading
 */
@interface EndKeyInputEventManager : NSObject <EndKeyInputEventManager>

// MARK: - Initialization

/**
 * Initializes the input event manager
 * @return Initialized manager or nil on failure
 */
- (instancetype)init NS_DESIGNATED_INITIALIZER;

/**
 * Unavailable - use init instead
 */
+ (instancetype)new NS_UNAVAILABLE;

// MARK: - Event Processing

/**
 * Main event handler - routes events to appropriate handlers
 * @param type Event type
 * @param event CGEvent object
 * @param proxy Event tap proxy
 * @return Processing result
 */
- (EndKeyEventResult)handleEvent:(EndKeyEventType)type
                          event:(CGEventRef)event
                          proxy:(CGEventTapProxy)proxy;

// MARK: - Handler Registration

/**
 * Registers a keyboard event handler
 * @param handler Block to handle keyboard events
 */
- (void)registerKeyboardEventHandler:(KeyboardEventHandler)handler;

/**
 * Registers a mouse event handler
 * @param handler Block to handle mouse events
 */
- (void)registerMouseEventHandler:(MouseEventHandler)handler;

/**
 * Registers a flags changed handler
 * @param handler Block to handle modifier flag changes
 */
- (void)registerFlagsChangedHandler:(FlagsChangedHandler)handler;

/**
 * Unregisters all keyboard event handlers
 */
- (void)unregisterAllKeyboardEventHandlers;

/**
 * Unregisters all mouse event handlers
 */
- (void)unregisterAllMouseEventHandlers;

/**
 * Unregisters all flags changed handlers
 */
- (void)unregisterAllFlagsChangedHandlers;

// MARK: - Hotkey Management

/**
 * Registers a hotkey with the system
 * @param keyCode Key code
 * @param modifiers Modifier flags
 * @param handler Block to execute when hotkey is triggered
 * @return Hotkey identifier or 0 on failure
 */
- (NSUInteger)registerHotkey:(CGKeyCode)keyCode
                    modifiers:(CGEventFlags)modifiers
                       handler:(void(^)(void))handler;

/**
 * Unregisters a hotkey
 * @param identifier Hotkey identifier returned from registerHotkey:
 * @return Success status
 */
- (BOOL)unregisterHotkey:(NSUInteger)identifier;

/**
 * Unregisters hotkey by key code and modifiers
 * @param keyCode Key code
 * @param modifiers Modifier flags
 * @return Success status
 */
- (BOOL)unregisterHotkey:(CGKeyCode)keyCode
               modifiers:(CGEventFlags)modifiers;

/**
 * Checks if a hotkey is registered
 * @param keyCode Key code
 * @param modifiers Modifier flags
 * @return Registration status
 */
- (BOOL)isHotkeyRegistered:(CGKeyCode)keyCode
                 modifiers:(CGEventFlags)modifiers;

// MARK: - Event Processing Control

/**
 * Enables or disables event processing
 * @param enabled Whether to enable event processing
 */
- (void)setEventProcessingEnabled:(BOOL)enabled;

/**
 * Gets current event processing state
 * @return Whether event processing is enabled
 */
- (BOOL)isEventProcessingEnabled;

/**
 * Pauses event processing temporarily
 * @param duration Duration to pause in seconds
 */
- (void)pauseEventProcessing:(NSTimeInterval)duration;

// MARK: - Event Filtering

/**
 * Adds an event filter
 * @param filter Block that returns YES if event should be processed
 * @return Filter identifier
 */
- (NSUInteger)addEventFilter:(BOOL(^)(EndKeyEventType type, CGEventRef event))filter;

/**
 * Removes an event filter
 * @param identifier Filter identifier
 */
- (void)removeEventFilter:(NSUInteger)identifier;

/**
 * Removes all event filters
 */
- (void)removeAllEventFilters;

// MARK: - Event Statistics

/**
 * Gets statistics about processed events
 * @return Dictionary with event statistics
 */
- (NSDictionary*)getEventStatistics;

/**
 * Resets event statistics
 */
- (void)resetEventStatistics;

// MARK: - Performance Monitoring

/**
 * Enables performance monitoring
 * @param enabled Whether to enable monitoring
 */
- (void)setPerformanceMonitoringEnabled:(BOOL)enabled;

/**
 * Gets performance metrics
 * @return Dictionary with performance metrics
 */
- (NSDictionary*)getPerformanceMetrics;

@end

} // namespace EndKey

#endif /* INPUT_EVENT_MANAGER_H */