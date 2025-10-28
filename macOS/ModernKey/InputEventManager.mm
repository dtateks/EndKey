//
//  InputEventManager.mm
//  EndKey
//
//  Created by Refactoring on 2025.
//  Copyright © 2025 Tuyen Mai. All rights reserved.
//

#import "InputEventManager.h"
#import <os/lock.h>
#import <os/signpost.h>

using namespace EndKey;

// Log subsystem for performance monitoring
static os_log_t g_performanceLog;

@interface EndKeyInputEventManager ()

// Event handlers
@property (nonatomic, strong) NSMutableArray<KeyboardEventHandler>* keyboardHandlers;
@property (nonatomic, strong) NSMutableArray<MouseEventHandler>* mouseHandlers;
@property (nonatomic, strong) NSMutableArray<FlagsChangedHandler>* flagsChangedHandlers;

// Hotkey management
@property (nonatomic, strong) NSMutableDictionary<NSString*, HotkeyRegistration*>* hotkeyRegistrations;
@property (nonatomic, assign) NSUInteger nextHotkeyIdentifier;

// Event filtering
@property (nonatomic, strong) NSMutableDictionary<NSNumber*, id<NSCopying>>* eventFilters;
@property (nonatomic, assign) NSUInteger nextFilterIdentifier;

// State and synchronization
@property (nonatomic, assign) BOOL eventProcessingEnabled;
@property (nonatomic, assign) os_unfair_lock eventLock;
@property (nonatomic, strong) dispatch_queue_t eventQueue;
@property (nonatomic, strong) dispatch_queue_t hotkeyQueue;

// Statistics
@property (nonatomic, assign) NSUInteger totalEventsProcessed;
@property (nonatomic, assign) NSUInteger keyboardEventsProcessed;
@property (nonatomic, assign) NSUInteger mouseEventsProcessed;
@property (nonatomic, assign) NSUInteger hotkeysTriggered;
@property (nonatomic, assign) NSTimeInterval lastEventTime;

// Performance monitoring
@property (nonatomic, assign) BOOL performanceMonitoringEnabled;
@property (nonatomic, assign) NSTimeInterval totalProcessingTime;
@property (nonatomic, assign) NSTimeInterval maxProcessingTime;
@property (nonatomic, assign) NSTimeInterval minProcessingTime;

@end

@implementation EndKeyInputEventManager

#pragma mark - Initialization

+ (void)initialize {
    if (self == [EndKeyInputEventManager class]) {
        g_performanceLog = os_log_create("com.endkey.performance", "InputEventManager");
    }
}

- (instancetype)init {
    self = [super init];
    if (self) {
        [self setupEventHandlers];
        [self setupHotkeyManagement];
        [self setupEventFiltering];
        [self setupSynchronization];
        [self setupStatistics];
        [self setupPerformanceMonitoring];
    }
    return self;
}

- (void)dealloc {
    if (_eventLock) {
        os_unfair_lock_destroy(&_eventLock);
    }
    if (_eventQueue) {
        dispatch_release(_eventQueue);
    }
    if (_hotkeyQueue) {
        dispatch_release(_hotkeyQueue);
    }
}

#pragma mark - Setup Methods

- (void)setupEventHandlers {
    _keyboardHandlers = [[NSMutableArray alloc] init];
    _mouseHandlers = [[NSMutableArray alloc] init];
    _flagsChangedHandlers = [[NSMutableArray alloc] init];
}

- (void)setupHotkeyManagement {
    _hotkeyRegistrations = [[NSMutableDictionary alloc] init];
    _nextHotkeyIdentifier = 1;
}

- (void)setupEventFiltering {
    _eventFilters = [[NSMutableDictionary alloc] init];
    _nextFilterIdentifier = 1;
}

- (void)setupSynchronization {
    os_unfair_lock_init(&_eventLock);
    _eventQueue = dispatch_queue_create("com.endkey.events", DISPATCH_QUEUE_CONCURRENT);
    _hotkeyQueue = dispatch_queue_create("com.endkey.hotkeys", DISPATCH_QUEUE_SERIAL);
    _eventProcessingEnabled = YES;
}

- (void)setupStatistics {
    _totalEventsProcessed = 0;
    _keyboardEventsProcessed = 0;
    _mouseEventsProcessed = 0;
    _hotkeysTriggered = 0;
    _lastEventTime = 0;
}

- (void)setupPerformanceMonitoring {
    _performanceMonitoringEnabled = NO;
    _totalProcessingTime = 0;
    _maxProcessingTime = 0;
    _minProcessingTime = INFINITY;
}

#pragma mark - Main Event Handler

- (EndKeyEventResult)handleEvent:(EndKeyEventType)type
                          event:(CGEventRef)event
                          proxy:(CGEventTapProxy)proxy {

    // Quick rejection if processing is disabled
    os_unfair_lock_lock(&_eventLock);
    if (!_eventProcessingEnabled) {
        os_unfair_lock_unlock(&_eventLock);
        return EndKeyEventResultPassThrough;
    }
    os_unfair_lock_unlock(&_eventLock);

    // Performance monitoring
    NSTimeInterval startTime = 0;
    if (_performanceMonitoringEnabled) {
        startTime = [NSDate timeIntervalSinceReferenceDate];
    }

    // Update statistics
    [self updateEventStatistics];

    // Apply event filters
    if (![self shouldProcessEvent:type event:event]) {
        return EndKeyEventResultPassThrough;
    }

    // Route to appropriate handler
    EndKeyEventResult result = [self routeEventToHandler:type event:event proxy:proxy];

    // Update performance metrics
    if (_performanceMonitoringEnabled && startTime > 0) {
        [self updatePerformanceMetrics:startTime];
    }

    return result;
}

- (EndKeyEventResult)routeEventToHandler:(EndKeyEventType)type
                                  event:(CGEventRef)event
                                  proxy:(CGEventTapProxy)proxy {

    switch (type) {
        case EndKeyEventTypeKeyDown:
        case EndKeyEventTypeKeyUp:
            return [self handleKeyboardEvent:type event:event proxy:proxy];

        case EndKeyEventTypeMouseDown:
        case EndKeyEventTypeMouseUp:
        case EndKeyEventTypeMouseDragged:
            return [self handleMouseEvent:type event:event proxy:proxy];

        case EndKeyEventTypeFlagsChanged:
            return [self handleFlagsChangedEvent:event proxy:proxy];

        default:
            return EndKeyEventResultPassThrough;
    }
}

- (EndKeyEventResult)handleKeyboardEvent:(EndKeyEventType)type
                                   event:(CGEventRef)event
                                   proxy:(CGEventTapProxy)proxy {

    CGKeyCode keyCode = (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);
    CGEventFlags flags = CGEventGetFlags(event);

    // Check for hotkey triggers
    if (type == EndKeyEventTypeKeyDown && [self checkAndTriggerHotkey:keyCode modifiers:flags]) {
        return EndKeyEventResultConsumed;
    }

    // Dispatch to keyboard handlers
    dispatch_sync(_eventQueue, ^{
        for (KeyboardEventHandler handler in self.keyboardHandlers) {
            @try {
                handler(keyCode, flags, proxy);
            } @catch (NSException *exception) {
                os_log_error(g_performanceLog, "Exception in keyboard handler: %{public}@", exception.reason);
            }
        }
    });

    return EndKeyEventResultPassThrough;
}

- (EndKeyEventResult)handleMouseEvent:(EndKeyEventType)type
                                event:(CGEventRef)event
                                proxy:(CGEventTapProxy)proxy {

    // Dispatch to mouse handlers
    dispatch_sync(_eventQueue, ^{
        for (MouseEventHandler handler in self.mouseHandlers) {
            @try {
                handler(type, proxy);
            } @catch (NSException *exception) {
                os_log_error(g_performanceLog, "Exception in mouse handler: %{public}@", exception.reason);
            }
        }
    });

    return EndKeyEventResultPassThrough;
}

- (EndKeyEventResult)handleFlagsChangedEvent:(CGEventRef)event
                                       proxy:(CGEventTapProxy)proxy {

    CGEventFlags newFlags = CGEventGetFlags(event);
    CGEventFlags oldFlags = 0; // This would need to be tracked

    // Check for hotkey triggers on flags change
    if ([self checkAndTriggerFlagsHotkey:newFlags]) {
        return EndKeyEventResultConsumed;
    }

    // Dispatch to flags changed handlers
    dispatch_sync(_eventQueue, ^{
        for (FlagsChangedHandler handler in self.flagsChangedHandlers) {
            @try {
                handler(newFlags, oldFlags, proxy);
            } @catch (NSException *exception) {
                os_log_error(g_performanceLog, "Exception in flags changed handler: %{public}@", exception.reason);
            }
        }
    });

    return EndKeyEventResultPassThrough;
}

#pragma mark - Handler Registration

- (void)registerKeyboardEventHandler:(KeyboardEventHandler)handler {
    if (handler) {
        dispatch_barrier_async(_eventQueue, ^{
            [self.keyboardHandlers addObject:[handler copy]];
        });
    }
}

- (void)registerMouseEventHandler:(MouseEventHandler)handler {
    if (handler) {
        dispatch_barrier_async(_eventQueue, ^{
            [self.mouseHandlers addObject:[handler copy]];
        });
    }
}

- (void)registerFlagsChangedHandler:(FlagsChangedHandler)handler {
    if (handler) {
        dispatch_barrier_async(_eventQueue, ^{
            [self.flagsChangedHandlers addObject:[handler copy]];
        });
    }
}

- (void)unregisterAllKeyboardEventHandlers {
    dispatch_barrier_async(_eventQueue, ^{
        [self.keyboardHandlers removeAllObjects];
    });
}

- (void)unregisterAllMouseEventHandlers {
    dispatch_barrier_async(_eventQueue, ^{
        [self.mouseHandlers removeAllObjects];
    });
}

- (void)unregisterAllFlagsChangedHandlers {
    dispatch_barrier_async(_eventQueue, ^{
        [self.flagsChangedHandlers removeAllObjects];
    });
}

#pragma mark - Hotkey Management

- (NSUInteger)registerHotkey:(CGKeyCode)keyCode
                    modifiers:(CGEventFlags)modifiers
                       handler:(void(^)(void))handler {

    __block NSUInteger identifier = 0;

    dispatch_sync(_hotkeyQueue, ^{
        NSString* key = [self hotkeyKeyForKeyCode:keyCode modifiers:modifiers];

        // Check if already registered
        if ([self.hotkeyRegistrations objectForKey:key]) {
            return;
        }

        // Register new hotkey
        identifier = self.nextHotkeyIdentifier++;
        HotkeyRegistration registration = {
            .keyCode = keyCode,
            .modifiers = modifiers,
            .handler = [handler copy],
            .identifier = identifier
        };

        NSValue* registrationValue = [NSValue valueWithBytes:&registration objCType:@typeof(HotkeyRegistration)];
        [self.hotkeyRegistrations setObject:registrationValue forKey:key];
    });

    return identifier;
}

- (BOOL)unregisterHotkey:(NSUInteger)identifier {
    __block BOOL success = NO;

    dispatch_sync(_hotkeyQueue, ^{
        for (NSString* key in self.hotkeyRegistrations.allKeys) {
            NSValue* value = [self.hotkeyRegistrations objectForKey:key];
            HotkeyRegistration registration;
            [value getValue:&registration];

            if (registration.identifier == identifier) {
                [self.hotkeyRegistrations removeObjectForKey:key];
                success = YES;
                break;
            }
        }
    });

    return success;
}

- (BOOL)unregisterHotkey:(CGKeyCode)keyCode
               modifiers:(CGEventFlags)modifiers {

    __block BOOL success = NO;

    dispatch_sync(_hotkeyQueue, ^{
        NSString* key = [self hotkeyKeyForKeyCode:keyCode modifiers:modifiers];
        if ([self.hotkeyRegistrations objectForKey:key]) {
            [self.hotkeyRegistrations removeObjectForKey:key];
            success = YES;
        }
    });

    return success;
}

- (BOOL)isHotkeyRegistered:(CGKeyCode)keyCode
                 modifiers:(CGEventFlags)modifiers {

    __block BOOL registered = NO;

    dispatch_sync(_hotkeyQueue, ^{
        NSString* key = [self hotkeyKeyForKeyCode:keyCode modifiers:modifiers];
        registered = [self.hotkeyRegistrations objectForKey:key] != nil;
    });

    return registered;
}

- (BOOL)checkAndTriggerHotkey:(CGKeyCode)keyCode modifiers:(CGEventFlags)modifiers {
    __block BOOL triggered = NO;

    dispatch_sync(_hotkeyQueue, ^{
        NSString* key = [self hotkeyKeyForKeyCode:keyCode modifiers:modifiers];
        NSValue* value = [self.hotkeyRegistrations objectForKey:key];

        if (value) {
            HotkeyRegistration registration;
            [value getValue:&registration];

            if (registration.handler) {
                dispatch_async(dispatch_get_main_queue(), registration.handler);
                triggered = YES;
                self.hotkeysTriggered++;
            }
        }
    });

    return triggered;
}

- (BOOL)checkAndTriggerFlagsHotkey:(CGEventFlags)flags {
    // This would check for hotkeys that are triggered by modifier flags alone
    // Implementation depends on specific hotkey requirements
    return NO;
}

- (NSString*)hotkeyKeyForKeyCode:(CGKeyCode)keyCode modifiers:(CGEventFlags)modifiers {
    return [NSString stringWithFormat:@"%lu_%lu", (unsigned long)keyCode, (unsigned long)modifiers];
}

#pragma mark - Event Processing Control

- (void)setEventProcessingEnabled:(BOOL)enabled {
    os_unfair_lock_lock(&_eventLock);
    _eventProcessingEnabled = enabled;
    os_unfair_lock_unlock(&_eventLock);
}

- (BOOL)isEventProcessingEnabled {
    os_unfair_lock_lock(&_eventLock);
    BOOL enabled = _eventProcessingEnabled;
    os_unfair_lock_unlock(&_eventLock);
    return enabled;
}

- (void)pauseEventProcessing:(NSTimeInterval)duration {
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(duration * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        [self setEventProcessingEnabled:YES];
    });
    [self setEventProcessingEnabled:NO];
}

#pragma mark - Event Filtering

- (NSUInteger)addEventFilter:(BOOL(^)(EndKeyEventType type, CGEventRef event))filter {
    if (!filter) return 0;

    __block NSUInteger identifier = 0;

    dispatch_barrier_async(_eventQueue, ^{
        identifier = self.nextFilterIdentifier++;
        [self.eventFilters setObject:[filter copy] forKey:@(identifier)];
    });

    return identifier;
}

- (void)removeEventFilter:(NSUInteger)identifier {
    dispatch_barrier_async(_eventQueue, ^{
        [self.eventFilters removeObjectForKey:@(identifier)];
    });
}

- (void)removeAllEventFilters {
    dispatch_barrier_async(_eventQueue, ^{
        [self.eventFilters removeAllObjects];
    });
}

- (BOOL)shouldProcessEvent:(EndKeyEventType)type event:(CGEventRef)event {
    __block BOOL shouldProcess = YES;

    dispatch_sync(_eventQueue, ^{
        for (NSNumber* identifier in self.eventFilters.allKeys) {
            BOOL(^filter)(EndKeyEventType, CGEventRef) = [self.eventFilters objectForKey:identifier];
            if (!filter(type, event)) {
                shouldProcess = NO;
                break;
            }
        }
    });

    return shouldProcess;
}

#pragma mark - Statistics

- (void)updateEventStatistics {
    os_unfair_lock_lock(&_eventLock);
    _totalEventsProcessed++;
    _lastEventTime = [NSDate timeIntervalSinceReferenceDate];
    os_unfair_lock_unlock(&_eventLock);
}

- (NSDictionary*)getEventStatistics {
    os_unfair_lock_lock(&_eventLock);
    NSDictionary* stats = @{
        @"totalEventsProcessed": @(_totalEventsProcessed),
        @"keyboardEventsProcessed": @(_keyboardEventsProcessed),
        @"mouseEventsProcessed": @(_mouseEventsProcessed),
        @"hotkeysTriggered": @(_hotkeysTriggered),
        @"lastEventTime": @(_lastEventTime),
        @"registeredHotkeys": @(_hotkeyRegistrations.count),
        @"registeredFilters": @(_eventFilters.count)
    };
    os_unfair_lock_unlock(&_eventLock);

    return stats;
}

- (void)resetEventStatistics {
    os_unfair_lock_lock(&_eventLock);
    _totalEventsProcessed = 0;
    _keyboardEventsProcessed = 0;
    _mouseEventsProcessed = 0;
    _hotkeysTriggered = 0;
    _lastEventTime = 0;
    os_unfair_lock_unlock(&_eventLock);
}

#pragma mark - Performance Monitoring

- (void)setPerformanceMonitoringEnabled:(BOOL)enabled {
    os_unfair_lock_lock(&_eventLock);
    _performanceMonitoringEnabled = enabled;
    if (!enabled) {
        _totalProcessingTime = 0;
        _maxProcessingTime = 0;
        _minProcessingTime = INFINITY;
    }
    os_unfair_lock_unlock(&_eventLock);
}

- (void)updatePerformanceMetrics:(NSTimeInterval)startTime {
    NSTimeInterval processingTime = [NSDate timeIntervalSinceReferenceDate] - startTime;

    os_unfair_lock_lock(&_eventLock);
    _totalProcessingTime += processingTime;
    _maxProcessingTime = MAX(_maxProcessingTime, processingTime);
    _minProcessingTime = MIN(_minProcessingTime, processingTime);
    os_unfair_lock_unlock(&_eventLock);

    // Log slow events
    if (processingTime > 0.001) { // 1ms threshold
        os_log_debug(g_performanceLog, "Slow event processing: %.3fms", processingTime * 1000);
    }
}

- (NSDictionary*)getPerformanceMetrics {
    os_unfair_lock_lock(&_eventLock);

    NSUInteger totalEvents = _totalEventsProcessed;
    NSTimeInterval avgTime = totalEvents > 0 ? _totalProcessingTime / totalEvents : 0;

    NSDictionary* metrics = @{
        @"performanceMonitoringEnabled": @(_performanceMonitoringEnabled),
        @"totalProcessingTime": @(_totalProcessingTime),
        @"averageProcessingTime": @(avgTime),
        @"maxProcessingTime": @(_maxProcessingTime),
        @"minProcessingTime": @(_minProcessingTime == INFINITY ? 0 : _minProcessingTime),
        @"totalEventsProcessed": @(totalEvents)
    };

    os_unfair_lock_unlock(&_eventLock);

    return metrics;
}

@end