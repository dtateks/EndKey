# EndKey Code Standards & Development Practices

## Overview

This document defines the coding standards, architectural patterns, and development practices used in the EndKey project. These standards ensure code quality, maintainability, and consistency across the codebase.

## Language-Specific Standards

### C++ Standards (Engine Layer)

#### Version and Compiler
- **C++ Standard**: C++11 minimum (modern features encouraged where beneficial)
- **Compiler**: Clang (Xcode default)
- **Warning Level**: All warnings enabled (`-Wall -Wextra`)
- **C++11 Features**: Use `auto`, range-based for loops, `nullptr`, smart pointers where appropriate

#### Naming Conventions

**Types (Classes, Structs, Enums)**
```cpp
// PascalCase
class VietnameseProcessor { };
struct MacroData { };
enum vKeyEvent { };
```
- Use descriptive, compound names
- Avoid abbreviations unless widely understood
- Suffix interfaces with meaningful names (e.g., `Processor`, `Manager`, `Handler`)

**Variables and Functions**
```cpp
// camelCase with descriptive names
int currentInputType;
void initializeVietnameseEngine();
bool validateInputSequence(const vector<Uint32>& input);
```

**Global Variables**
```cpp
// All global variables prefixed with 'v'
extern int vLanguage;
extern int vInputType;
extern int vUseMacro;
```
- All globals use `v` prefix to indicate global scope
- Must be declared in Engine.h
- Documented with Doxygen-style comments

**Constants and Macros**
```cpp
// UPPER_CASE with descriptive names
#define MAX_BUFF 32
#define IS_CONSONANT(keyCode) !(keyCode == KEY_A || ...)
#define MARK1_MASK 0x80000
```

**File Naming**
- **Headers**: `.h` extension
- **Implementation**: `.cpp` extension
- **Platform-specific**: `platforms/<platform>.h`

#### Code Formatting

**Indentation and Spacing**
- **Indentation**: 4 spaces (no tabs)
- **Line Length**: Target < 120 characters (soft limit)
- **Brace Style**: K&R style (opening brace on same line)

```cpp
void functionName(int parameter) {
    if (condition) {
        doSomething();
    } else {
        doSomethingElse();
    }
}
```

**Spacing Rules**
- Space after commas: `function(a, b, c)`
- Space around operators: `a + b * c`
- No space between function name and parentheses: `function()`
- Space after control flow keywords: `if (condition)`, `while (condition)`

#### Type Definitions

**Custom Types**
```cpp
typedef unsigned char Byte;
typedef unsigned short Uint16;
typedef unsigned int Uint32;
typedef unsigned long int Uint64;
```

**Struct Definitions**
```cpp
struct vKeyHookState {
    Byte code;
    Byte backspaceCount;
    Byte newCharCount;
    Uint32 charData[MAX_BUFF];
};
```

**Enum Usage**
```cpp
enum vKeyEvent {
    Keyboard,
    Mouse
};
```

#### Documentation Standards

**Function Documentation**
```cpp
/**
 * MAIN entry point for each key
 * event: mouse or keyboard event
 * state: additional state for event
 * data: key code
 * isCaps: caplock is on or shift key is pressing
 * otherControlKey: ctrl, option,... is pressing
 */
void vKeyHandleEvent(const vKeyEvent& event,
                     const vKeyEventState& state,
                     const Uint16& data,
                     const Uint8& capsStatus=0,
                     const bool& otherControlKey=false);
```

**Variable Documentation**
```cpp
/*
 * 0: Telex
 * 1: VNI
 */
extern int vInputType;

/**
 * 0: No
 * 1: Yes
 */
extern int vFreeMark;
```

#### Memory Management

**Object Lifetime**
- Prefer stack allocation over heap
- Use RAII principles
- Minimize dynamic allocation in hot paths
- No raw `new`/`delete` in application code

**String Handling**
```cpp
// Use std::string for UTF-8
string wideStringToUtf8(const wstring& str);

// Use std::wstring for wide character strings
wstring utf8ToWideString(const string& str);
```

#### Performance Guidelines

**Avoid Unnecessary Allocations**
- Pre-allocate buffers (use `MAX_BUFF` size)
- Reuse containers when possible
- Avoid `std::string` concatenation in loops
- Use references and const references for parameters

**Efficient Character Processing**
```cpp
// Good: Direct array access
Uint16 getCharacterCode(const Uint32& data) {
    return (Uint16)(data & CHAR_MASK);
}

// Avoid: Unnecessary function calls in hot paths
```

#### Error Handling

**Input Validation**
- Validate all input parameters
- Use assertions in debug builds: `assert(condition)`
- Return early on invalid input

```cpp
void processInput(const vector<Uint32>& input) {
    if (input.empty()) {
        return; // Early return on invalid input
    }
    // Process valid input
}
```

**State Validation**
```cpp
// Check internal state consistency
if (_index > MAX_BUFF) {
    // Handle buffer overflow
    startNewSession();
}
```

### Objective-C Standards (UI Layer)

#### Version and Framework
- **Language**: Objective-C 2.0 with Modern Syntax
- **Runtime**: Modern Objective-C Runtime
- **Memory Management**: Automatic Reference Counting (ARC)
- **Framework**: Cocoa (macOS 10.14+)

#### Naming Conventions

**Classes and Protocols**
```objc
// PascalCase with descriptive prefixes
@interface AppDelegate : NSObject <NSApplicationDelegate>
@interface EndKeyManager : NSObject
@protocol SmartSwitchDelegate <NSObject>
```

**Methods and Properties**
```objc
// camelCase starting with lowercase
- (void)onInputTypeSelectedIndex:(int)index;
- (BOOL)initEventTap;
@property (nonatomic, strong) NSArray *tabviews;
```

**Instance Variables**
```objc
// Suffix with underscore
{
    NSArray *_tabviews;
    NSRect _tabViewRect;
}
```

**Constants**
```objc
// Use const for simple constants
static const NSInteger kMaxBufferSize = 32;

// Use enum for enumerations
typedef NS_ENUM(NSInteger, InputType) {
    InputTypeTelex = 0,
    InputTypeVNI = 1,
    InputTypeSimpleTelex = 2
};

// Use constants for strings
extern NSString *const kDefaultInputMethod;
```

#### Code Formatting

**Indentation and Spacing**
- Indentation: 4 spaces (no tabs)
- Line length: Target < 120 characters
- Brace style: K&R style

```objc
- (void)someMethodWithParameter:(NSString *)param {
    if (condition) {
        [self doSomething];
    } else {
        [self doSomethingElse];
    }
}
```

**Method Declaration and Call Spacing**
```objc
// Colons aligned for multi-parameter methods
- (void)methodWithFirstParam:(Type)first
                secondParam:(Type)second
                 thirdParam:(Type)third;

// Method calls: space before opening bracket
[object methodWithParam:param];
```

#### Documentation Standards

**Method Documentation**
```objc
/**
 * Initialize the event tap for keyboard/mouse interception
 *
 * @return YES if initialization succeeded, NO otherwise
 * @discussion This method creates a CGEventTap and starts listening
 *             to keyboard events on a background thread
 */
+ (BOOL)initEventTap;
```

**Property Documentation**
```objc
/// The main preferences tab view
@property (weak) IBOutlet NSTabView *tabviewPrimary;

/// Array of configured input methods
@property (nonatomic, copy) NSArray *inputMethods;
```

#### Memory Management (ARC)

**Ownership Rules**
- Use weak references for delegates: `@property (weak)`
- Use strong references for owned objects: `@property (strong)`
- Copy NSString properties: `@property (copy)`
- Avoid retain cycles with weak delegates

```objc
// Good: Delegate pattern with weak reference
@interface MyClass : NSObject
@property (weak) id<MyDelegate> delegate;
@end

// Good: Owned objects with strong reference
@interface MyClass : NSObject
@property (strong) NSArray *ownedArray;
@property (copy) NSString *stringValue;
@end
```

**Avoid Retain Cycles**
```objc
// Good: Weak reference to avoid cycle
@interface Handler : NSObject
@property (weak) id<HandlerDelegate> delegate;
@end

// Avoid: Strong reference creates cycle
@interface Handler : NSObject
@property (strong) id<HandlerDelegate> delegate; // Bad
@end
```

#### Best Practices

**Init Methods**
```objc
// Designated initializer
- (instancetype)initWithFrame:(NSRect)frameRect {
    self = [super initWithFrame:frameRect];
    if (self) {
        // Initialization
    }
    return self;
}
```

**Property Accessors**
```objc
// Synthesize is automatic in modern Objective-C
@property (nonatomic, strong) NSString *value;

// Custom accessor if needed
- (NSString *)value {
    return _value ?: @"default";
}
```

**Error Handling**
```objc
// Use NSError by reference
- (BOOL)performOperation:(NSError **)error {
    if (somethingFailed) {
        if (error) {
            *error = [NSError errorWithDomain:@"MyDomain"
                                         code:1
                                     userInfo:nil];
        }
        return NO;
    }
    return YES;
}
```

### Objective-C++ Standards (.mm files)

#### Interoperability

**C++/Objective-C Bridge**
- Use `.mm` extension for mixed files
- Include C++ headers in implementation section only
- Minimize mixing C++ and Objective-C in same function

```objc
// Header - Objective-C only
#import <Cocoa/Cocoa.h>

// Implementation - can include C++
#import "Engine.h"

@interface MyClass : NSObject
@end

@implementation MyClass {
    // C++ objects as instance variables
    VietnameseProcessor *_processor;
}
```

**String Conversion**
```cpp
// Helper functions for string conversion
extern "C" {
    NSString* ConvertUtil(NSString* str);
}
```

## Architectural Patterns

### Module Pattern (Engine)

The engine uses a functional module pattern:

```cpp
// Module interface in header
void initVietnameseModule();
bool processKeyEvent(Uint16 keyCode);
void cleanupModule();

// Module implementation in .cpp
void initVietnameseModule() {
    // Initialize static data
}

bool processKeyEvent(Uint16 keyCode) {
    // Process event
}
```

### Delegation Pattern (UI)

UI components use delegation extensively:

```objc
// Define protocol
@protocol EndKeyManagerDelegate <NSObject>
- (void)eventTapDidFail:(NSError *)error;
@end

// Delegate in header
@interface EndKeyManager : NSObject
@property (weak) id<EndKeyManagerDelegate> delegate;
@end

// Usage
[manager setDelegate:self];
```

### Singleton Pattern (Managers)

Managers use singleton pattern:

```objc
// Singleton pattern
+ (instancetype)sharedManager {
    static id _sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _sharedInstance = [[self alloc] init];
    });
    return _sharedInstance;
}
```

## Project Structure

### Directory Organization

```
project/
├── engine/                   # C++ core
│   ├── *.h                   # Public interfaces
│   ├── *.cpp                 # Implementation
│   └── platforms/            # Platform-specific
├── macOS/ModernKey/          # macOS app
│   ├── *.h                   # Objective-C headers
│   ├── *.m                   # Objective-C implementation
│   ├── *.mm                  # Objective-C++ mixed
│   ├── *.storyboard          # UI definitions
│   └── Resources/            # Assets
```

### File Organization

**Header Files (.h)**
- Public interfaces only
- Minimal implementation
- Well-documented APIs
- Include only necessary headers

**Implementation Files (.cpp, .m, .mm)**
- Complete implementation
- Private helper methods
- Internal constants and types
- Implementation details

## Build Configuration

### Compiler Flags

**C++ Flags**
```
-std=c++11
-Wall
-Wextra
-O3 (Release)
-g (Debug)
```

**Objective-C Flags**
```
-fobjc-arc
-Wall
-Wextra
-O3 (Release)
-g (Debug)
```

**Optimization Settings**
- Debug: No optimization, full debug symbols
- Release: -O3 optimization, stripped debug symbols

### Xcode Project Settings

**Architectures**
- Standard architectures (x86_64, arm64)
- Universal binary for distribution

**Warnings**
- Enable all warnings
- Treat warnings as errors (for production builds)
- Deprecated function warnings enabled

## Code Review Guidelines

### Review Checklist

**Functionality**
- [ ] Code does what it claims to do
- [ ] Edge cases handled correctly
- [ ] No obvious bugs or logic errors
- [ ] Performance considerations addressed

**Readability**
- [ ] Clear, descriptive naming
- [ ] Adequate comments for complex logic
- [ ] Consistent formatting
- [ ] No dead or commented-out code

**Standards Compliance**
- [ ] Follows language-specific standards
- [ ] Uses appropriate patterns
- [ ] Memory management correct
- [ ] Error handling adequate

**Security**
- [ ] Input validation present
- [ ] No buffer overflows
- [ ] No memory leaks
- [ ] Secure string handling

### Review Process

1. **Self-review**: Review your own code before submitting
2. **Automated checks**: Run clang-tidy, static analysis
3. **Peer review**: At least one reviewer for non-trivial changes
4. **Testing**: Verify functionality on target platforms

## Testing Standards

### Unit Testing

**Engine Layer**
- Test pure functions
- Mock dependencies
- Test edge cases
- Benchmark performance-critical paths

**UI Layer**
- Test user interactions
- Verify UI state changes
- Test error handling
- Mock external dependencies

### Manual Testing

**Input Method Testing**
- Test all input methods (Telex, VNI, Simple Telex)
- Verify special cases (diphthongs, tone marks)
- Test with different applications

**Feature Testing**
- Toggle each feature and verify behavior
- Test configuration persistence
- Test error recovery

## Performance Guidelines

### C++ Performance

**Optimization Priorities**
1. Minimize allocations in hot paths
2. Use efficient data structures
3. Cache frequently accessed data
4. Avoid unnecessary copying

**Profiling Points**
- Key event processing latency
- Vietnamese character combination
- Macro lookup performance
- Smart switch application detection

### Objective-C Performance

**UI Responsiveness**
- Perform heavy operations on background threads
- Update UI on main thread only
- Use appropriate collection types
- Avoid view geometry calculations in hot paths

**Memory Efficiency**
- Release unused objects promptly
- Use weak references for delegates
- Minimize retain cycles
- Profile with Instruments

## Security Guidelines

### Input Validation

**All Input Paths**
- Validate key codes are in valid range
- Check buffer sizes before access
- Sanitize file paths
- Validate JSON/Plist data

**String Handling**
- Always specify encoding (UTF-8)
- Avoid buffer overflows
- Use safe string functions
- Validate file content before parsing

### Privilege Management

**macOS Permissions**
- Request minimal required permissions
- Check permission status before operation
- Provide clear user feedback on permission issues
- Gracefully handle permission denial

### Data Protection

**User Data**
- Never transmit user data without consent
- Encrypt sensitive data at rest
- Use macOS Keychain for credentials
- Provide privacy policy

## Documentation Standards

### Code Documentation

**Minimum Requirements**
- Public function documentation (Doxygen format)
- Complex algorithm explanation
- Non-obvious behavior documented
- FIXME/TODO comments for future work

**Style**
```cpp
/**
 * Brief description of function
 *
 * Detailed description of what the function does,
 * how it works, and any important notes.
 *
 * @param param1 Description of first parameter
 * @param param2 Description of second parameter
 * @return Description of return value
 * @throws ExceptionType When this condition occurs
 */
ReturnType functionName(Type param1, Type param2);
```

### API Documentation

**Public Interfaces**
- Document all public APIs
- Include usage examples
- Note any side effects
- Document thread safety

## Version Control

### Commit Messages

**Format**
```
type(scope): brief description

Detailed explanation of why this change was made,
what it does, and any important considerations.

Fixes: #123
```

**Types**
- feat: New feature
- fix: Bug fix
- docs: Documentation changes
- refactor: Code refactoring
- style: Formatting changes
- test: Adding tests
- chore: Maintenance tasks

**Example**
```
feat(engine): add support for Simple Telex 2

Implements additional Simple Telex variant with streamlined
diphthong processing for faster typing.

- Add new input type enum value
- Update Vietnamese.cpp with simplified mappings
- Add tests in test_vietnamese.cpp
- Update documentation

Fixes: #45
```

### Branching Strategy

- **main**: Production-ready code
- **develop**: Integration branch for features
- **feature/***: Feature branches
- **fix/***: Bug fix branches
- **release/***: Release preparation branches

## Continuous Integration

### Build Requirements

- Must compile on Xcode 12+
- Must run on macOS 10.14+ (Mojave)
- No compiler warnings in release builds
- Static analysis clean

### Automated Checks

- clang-format code formatting check
- clang-tidy static analysis
- Unit tests pass
- Memory leak check with Instruments

## Maintenance Guidelines

### Code Health

**Regular Maintenance**
- Remove dead code
- Update comments to match code
- Refactor when patterns emerge
- Update dependencies

**Technical Debt**
- Track in issue tracker
- Prioritize based on impact
- Allocate time in sprint planning
- Monitor complexity metrics

### Backward Compatibility

**Configuration**
- Support older preference files
- Provide migration paths
- Deprecate features gracefully
- Document breaking changes

**Data Format**
- Version data formats
- Handle older versions
- Migration utilities
- User feedback on changes

## Conclusion

These standards ensure the EndKey codebase remains maintainable, performant, and readable. All developers should follow these guidelines when contributing to the project. Regular review of these standards helps maintain code quality as the project evolves.

For questions or suggestions about these standards, please discuss with the development team and update this document accordingly.
