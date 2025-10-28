//
//  RAIIWrappers.h
//  EndKey
//
//  Created by Refactoring on 2025.
//  Copyright © 2025 Tuyen Mai. All rights reserved.
//

#ifndef RAII_WRAPPERS_H
#define RAII_WRAPPERS_H

#import <CoreFoundation/CoreFoundation.h>
#import <CoreGraphics/CoreGraphics.h>
#import <memory>
#import <type_traits>

namespace EndKey {
namespace RAII {

/**
 * Generic RAII wrapper for Core Foundation objects
 * Provides automatic resource management and move semantics
 */
template<typename T>
class CFWrapper {
private:
    T _object;
    bool _ownsObject;

public:
    // Constructor
    CFWrapper(T obj = nullptr, bool owns = true)
        : _object(obj), _ownsObject(owns) {}

    // Destructor
    ~CFWrapper() {
        if (_ownsObject && _object) {
            CFRelease(_object);
        }
    }

    // Move constructor
    CFWrapper(CFWrapper&& other) noexcept
        : _object(other._object), _ownsObject(other._ownsObject) {
        other._object = nullptr;
        other._ownsObject = false;
    }

    // Move assignment
    CFWrapper& operator=(CFWrapper&& other) noexcept {
        if (this != &other) {
            if (_ownsObject && _object) {
                CFRelease(_object);
            }
            _object = other._object;
            _ownsObject = other._ownsObject;
            other._object = nullptr;
            other._ownsObject = false;
        }
        return *this;
    }

    // Delete copy operations
    CFWrapper(const CFWrapper&) = delete;
    CFWrapper& operator=(const CFWrapper&) = delete;

    // Accessors
    T get() const { return _object; }
    T operator->() const { return _object; }
    operator bool() const { return _object != nullptr; }

    // Reset ownership
    void reset(T obj = nullptr, bool owns = true) {
        if (_ownsObject && _object) {
            CFRelease(_object);
        }
        _object = obj;
        _ownsObject = owns;
    }

    // Release ownership
    T release() {
        T obj = _object;
        _object = nullptr;
        _ownsObject = false;
        return obj;
    }

    // Check if owns object
    bool owns() const { return _ownsObject; }
};

// Specialized wrappers for common Core Foundation types
using CFStringRefWrapper = CFWrapper<CFStringRef>;
using CFArrayRefWrapper = CFWrapper<CFArrayRef>;
using CFDictionaryRefWrapper = CFWrapper<CFDictionaryRef>;
using CFDataRefWrapper = CFWrapper<CFDataRef>;

/**
 * RAII wrapper for CGEvent objects with enhanced functionality
 */
class CGEventWrapper {
private:
    CGEventRef _event;
    bool _ownsEvent;

public:
    CGEventRef _event;
    bool _ownsEvent;

public:
    CGEventWrapper(CGEventRef event = nullptr, bool owns = true)
        : _event(event), _ownsEvent(owns) {}

    ~CGEventWrapper() {
        if (_ownsEvent && _event) {
            CFRelease(_event);
        }
    }

    // Move semantics
    CGEventWrapper(CGEventWrapper&& other) noexcept
        : _event(other._event), _ownsEvent(other._ownsEvent) {
        other._event = nullptr;
        other._ownsEvent = false;
    }

    CGEventWrapper& operator=(CGEventWrapper&& other) noexcept {
        if (this != &other) {
            if (_ownsEvent && _event) {
                CFRelease(_event);
            }
            _event = other._event;
            _ownsEvent = other._ownsEvent;
            other._event = nullptr;
            other._ownsEvent = false;
        }
        return *this;
    }

    // Delete copy
    CGEventWrapper(const CGEventWrapper&) = delete;
    CGEventWrapper& operator=(const CGEventWrapper&) = delete;

    // Accessors and operations
    CGEventRef get() const { return _event; }
    operator bool() const { return _event != nullptr; }

    void post(CGEventTapProxy proxy) const {
        if (_event) {
            CGEventTapPostEvent(proxy, _event);
        }
    }

    void setFlags(CGEventFlags flags) {
        if (_event) {
            CGEventSetFlags(_event, flags);
        }
    }

    CGEventFlags getFlags() const {
        return _event ? CGEventGetFlags(_event) : 0;
    }

    void setUnicodeString(UniChar* chars, UniCharCount length) {
        if (_event) {
            CGEventKeyboardSetUnicodeString(_event, length, chars);
        }
    }

    void reset(CGEventRef event = nullptr, bool owns = true) {
        if (_ownsEvent && _event) {
            CFRelease(_event);
        }
        _event = event;
        _ownsEvent = owns;
    }

    CGEventRef release() {
        CGEventRef event = _event;
        _event = nullptr;
        _ownsEvent = false;
        return event;
    }
};

/**
 * RAII wrapper for CGEventSource objects
 */
class CGEventSourceWrapper {
private:
    CGEventSourceRef _source;
    bool _ownsSource;

public:
    CGEventSourceWrapper(CGEventSourceRef source = nullptr, bool owns = true)
        : _source(source), _ownsSource(owns) {}

    ~CGEventSourceWrapper() {
        if (_ownsSource && _source) {
            CFRelease(_source);
        }
    }

    // Move semantics
    CGEventSourceWrapper(CGEventSourceWrapper&& other) noexcept
        : _source(other._source), _ownsSource(other._ownsSource) {
        other._source = nullptr;
        other._ownsSource = false;
    }

    CGEventSourceWrapper& operator=(CGEventSourceWrapper&& other) noexcept {
        if (this != &other) {
            if (_ownsSource && _source) {
                CFRelease(_source);
            }
            _source = other._source;
            _ownsSource = other._ownsSource;
            other._source = nullptr;
            other._ownsSource = false;
        }
        return *this;
    }

    // Delete copy
    CGEventSourceWrapper(const CGEventSourceWrapper&) = delete;
    CGEventSourceWrapper& operator=(const CGEventSourceWrapper&) = delete;

    // Accessors
    CGEventSourceRef get() const { return _source; }
    operator bool() const { return _source != nullptr; }

    // Factory methods
    static CGEventSourceWrapper createPrivate() {
        return CGEventSourceWrapper(CGEventSourceCreate(kCGEventSourceStatePrivate));
    }

    static CGEventSourceWrapper createCombined() {
        return CGEventSourceWrapper(CGEventSourceCreate(kCGEventSourceStateCombinedSessionState));
    }

    void reset(CGEventSourceRef source = nullptr, bool owns = true) {
        if (_ownsSource && _source) {
            CFRelease(_source);
        }
        _source = source;
        _ownsSource = owns;
    }

    CGEventSourceRef release() {
        CGEventSourceRef source = _source;
        _source = nullptr;
        _ownsSource = false;
        return source;
    }
};

/**
 * Event pool for reusing common events
 */
class EventPool {
private:
    static constexpr size_t POOL_SIZE = 16;
    CGEventWrapper _eventPool[POOL_SIZE];
    std::atomic<size_t> _nextAvailable{0};
    dispatch_queue_t _poolQueue;

public:
    EventPool() : _poolQueue(dispatch_queue_create("com.endkey.eventpool", DISPATCH_QUEUE_CONCURRENT)) {
        // Pre-allocate common events
        initializePool();
    }

    ~EventPool() {
        if (_poolQueue) {
            dispatch_release(_poolQueue);
        }
    }

    // Delete copy/move
    EventPool(const EventPool&) = delete;
    EventPool& operator=(const EventPool&) = delete;
    EventPool(EventPool&&) = delete;
    EventPool& operator=(EventPool&&) = delete;

    CGEventWrapper acquireBackspaceEvent(CGEventSourceRef source) {
        return acquireOrCreate([&]() {
            return CGEventCreateKeyboardEvent(source, 51, true);
        });
    }

    CGEventWrapper acquireKeyEvent(CGEventSourceRef source, CGKeyCode keyCode, bool keyDown) {
        return acquireOrCreate([&]() {
            return CGEventCreateKeyboardEvent(source, keyCode, keyDown);
        });
    }

    void releaseEvent(CGEventWrapper&& event) {
        if (event) {
            dispatch_async(_poolQueue, ^{
                // Reset event to default state for reuse
                event.reset();
            });
        }
    }

private:
    void initializePool() {
        dispatch_async(_poolQueue, ^{
            for (size_t i = 0; i < POOL_SIZE; ++i) {
                _eventPool[i] = CGEventWrapper(nullptr, false);
            }
        });
    }

    template<typename Factory>
    CGEventWrapper acquireOrCreate(Factory&& factory) {
        __block CGEventWrapper result;

        dispatch_sync(_poolQueue, ^{
            // Try to find an available slot
            for (size_t i = 0; i < POOL_SIZE; ++i) {
                if (!_eventPool[i]) {
                    _eventPool[i] = CGEventWrapper(factory());
                    result = std::move(_eventPool[i]);
                    return;
                }
            }
        });

        // If pool is exhausted, create new event
        if (!result) {
            result = CGEventWrapper(factory());
        }

        return result;
    }
};

/**
 * Smart pointer for C++ engine objects
 */
template<typename T>
class EnginePtr {
private:
    T* _ptr;

public:
    EnginePtr(T* ptr = nullptr) : _ptr(ptr) {}

    ~EnginePtr() {
        if (_ptr) {
            // Assuming engine provides cleanup function
            // vKeyCleanup(_ptr); // This would need to be implemented
        }
    }

    // Move semantics
    EnginePtr(EnginePtr&& other) noexcept : _ptr(other._ptr) {
        other._ptr = nullptr;
    }

    EnginePtr& operator=(EnginePtr&& other) noexcept {
        if (this != &other) {
            if (_ptr) {
                // vKeyCleanup(_ptr);
            }
            _ptr = other._ptr;
            other._ptr = nullptr;
        }
        return *this;
    }

    // Delete copy
    EnginePtr(const EnginePtr&) = delete;
    EnginePtr& operator=(const EnginePtr&) = delete;

    T* get() const { return _ptr; }
    T& operator*() const { return *_ptr; }
    T* operator->() const { return _ptr; }
    operator bool() const { return _ptr != nullptr; }

    T* release() {
        T* ptr = _ptr;
        _ptr = nullptr;
        return ptr;
    }

    void reset(T* ptr = nullptr) {
        if (_ptr) {
            // vKeyCleanup(_ptr);
        }
        _ptr = ptr;
    }
};

} // namespace RAII
} // namespace EndKey

#endif /* RAII_WRAPPERS_H */