## OpenSpec Workflow - Critical

**⚠️ ALWAYS check for OpenSpec instructions before any task**

When requests involve:
- Planning or proposals (proposal, spec, change, plan)
- New capabilities, breaking changes, architecture shifts
- Performance/security work
- Ambiguous requirements

**Read these first:**
1. `/openspec/AGENTS.md` - OpenSpec complete guide
2. `/openspec/project.md` - Project conventions
3. `openspec list` - Active changes
4. `openspec list --specs` - Existing capabilities

### OpenSpec Commands

```bash
# List work
openspec list                          # Active changes
openspec list --specs                  # All capabilities

# Show details
openspec show [change-id]              # View proposal/spec
openspec show [spec-id] --type spec

# Validate before implementing
openspec validate [change-id] --strict
openspec validate --strict

# Archive after deployment
openspec archive [change-id] --yes     # Skip interactive prompts
```

## Project Overview

EndKey là một bộ gõ tiếng Việt cho macOS được refactoring từ monolithic sang modular architecture sử dụng modern C++. Dự án có 2 phần:

1. **Engine** (C++) - Core Vietnamese typing engine với clean architecture
2. **macOS App** (Objective-C++) - UI và system integration

## Build Commands

### Complete Build Workflow
```bash
# Kill existing processes
killall -9 EndKey EndKeyHelper 2>/dev/null

# Clean build
xcodebuild -project Sources/EndKey/macOS/EndKey.xcodeproj \
  -target EndKey \
  -configuration Debug \
  CODE_SIGNING_ALLOWED=NO \
  clean build

# Install to Applications
cp -af macOS/build/Debug/EndKey.app /Applications/

# Sign app (required for persistent permissions)
codesign --force --deep --sign - /Applications/EndKey.app
```

## Architecture Overview

### Engine Architecture (C++)

#### Clean Architecture Layers

**1. Interface Layer** (`engine/interfaces/`)
- Abstract interfaces cho tất cả components
- Dependency injection cho testability
- Factory pattern cho object creation

Key interfaces:
- `IEngineCore.h` - Main engine coordination
- `IVietnameseProcessor.h` - Vietnamese language processing
- `IMacroProcessor.h` - Text expansion và macros
- `IConfigurationManager.h` - Centralized configuration
- `IInputProcessor.h` - Keyboard event processing

**2. Core Layer** (`engine/core/`)
- Concrete implementations của interfaces
- Modern C++ với RAII pattern
- Smart pointers cho automatic memory management
- Dependency injection pattern

Key components:
- `EngineCore` - Main implementation với dependency injection
- `ConfigurationManager` - Settings management
- `VietnameseProcessor` - Vietnamese character processing
- `MacroProcessor` - Macro expansion
- `MemoryManager` - Memory optimization với LRU cache

**3. Bridge Layer** (`EngineBridge.h/cpp`)
- **100% backward compatibility** với legacy API
- Maps legacy global variables → new configuration system
- Maintains exact same public interface

#### Key Engine Components

- **TypingEngine** - Core input processing pipeline
- **SpellingEngine** - Vietnamese orthography validation
- **VietnameseOptimized** - Efficient character classes (replaces hardcoded maps)
- **ConvertTool** - Unicode/encoding conversions
- **SmartSwitchKey** - Language switching logic

#### Memory & Performance Optimizations

- **Memory Pools** cho frequent allocations
- **LRU Caching** (50K entries cho character lookups)
- **Lazy Loading** cho conversion tables
- **Hot Path Optimization** trong typing pipeline
- **RAII Pattern** - No manual memory management

### macOS App Architecture (Objective-C++)

#### Main Components

- **EndKeyManager** - Core management và event handling
- **InputEventManager** - Global keyboard event monitoring
- **UIManager** - Status window và preferences
- **ModernKey** - Main app interface với Storyboard
- **EndKeyHelper** - Login item cho auto-start

#### Key Files

- `EndKey.mm` - Main entry point
- `InputEventManager.mm` - System-wide event monitoring
- `EndKeyManager.m` - Business logic coordination
- `ViewController.m` - Main UI controller

## Development Patterns

### Memory Management
- Sử dụng **modern C++** với smart pointers (`std::unique_ptr`)
- **RAII pattern** - Không cần manual memory management
- **Memory pools** cho frequent allocations
- **LRU caching** cho character lookups (50K entries)

### Configuration System
- Centralized configuration qua `ConfigurationManager`
- Legacy global variables auto-sync với new system
- Real-time configuration updates

### Code Style
- Modern C++14/17 patterns
- RAII resource management
- Interface-based design for testability
- Dependency injection pattern
- Clean architecture với separation of concerns

## Testing

### Component Testing
Modular architecture enables isolated component testing:
- Mock interfaces cho unit testing
- Test từng component riêng biệt
- Backend compatibility ensures existing functionality works

### Memory Leak Detection
```bash
# Monitor memory usage
leaks --atExit -- /Applications/EndKey.app/Contents/MacOS/EndKey
```

## Debugging

### Common Issues
- **Permissions**: macOS requires accessibility permissions cho keyboard monitoring
- **Code Signing**: App cần được signed cho persistent permissions
- **Memory Leaks**: Use modern C++ patterns để avoid leaks

### Debug Commands
```bash
# Check running processes
ps aux | grep EndKey

# Reset accessibility permissions
sudo tccutil reset Accessibility

# Monitor memory
leaks --atExit -- /Applications/EndKey.app/Contents/MacOS/EndKey
```

### Log Files
- Check Console app for runtime logs
- Look for accessibility permission errors
- Monitor memory pressure warnings

## File Organization

```
engine/
├── interfaces/          # Abstract interfaces
│   ├── IEngineCore.h
│   ├── IVietnameseProcessor.h
│   ├── IMacroProcessor.h
│   ├── IConfigurationManager.h
│   └── IInputProcessor.h
├── core/               # Concrete implementations
│   ├── EngineCore.h/cpp
│   ├── ConfigurationManager.h/cpp
│   ├── VietnameseProcessor.h/cpp
│   ├── MacroProcessor.h/cpp
│   └── MemoryManager.h/cpp
├── platforms/          # Platform-specific code
├── EngineBridge.h/cpp  # Backward compatibility layer
└── [legacy files]      # Original monolithic code

macOS/
├── ModernKey/          # Main app UI
│   ├── EndKey.xcodeproj
│   ├── ViewController.m
│   ├── EndKeyManager.m
│   └── ...
├── EndKeyHelper/       # Background helper
└── build/              # Build output

openspec/
├── project.md          # Project conventions
├── specs/              # Current truth - what IS built
├── changes/            # Proposals - what SHOULD change
└── archive/            # Completed changes
```

## Usage Examples

### Using Modern API (Recommended)
```cpp
#include "core/EngineCore.h"

// Create engine using factory
EndKey::Core::EngineFactory factory;
auto engine = factory.createEngine();

// Initialize
if (!engine->initialize()) {
    return false;
}

// Process events
engine->processKeyEvent(vKeyEvent::Keyboard, vKeyEventState::KeyDown, KEY_A);

// Get configuration
auto config = engine->getConfigurationManager();
config->setInputType(vKeyInputType::vTelex);
```

### Using Legacy API (Unchanged)
```cpp
#include "EngineBridge.h"

// Legacy initialization
void* engineHandle = vKeyInit();

// Process events (same as before)
vKeyHandleEvent(vKeyEvent::Keyboard, vKeyEventState::KeyDown, KEY_A);

// Access legacy variables (still work)
vInputType = 0; // Telex
vLanguage = 1;  // Vietnamese
```

## Backward Compatibility

**100% API Compatibility Guaranteed**
- EngineBridge maintains exact same public interface
- Legacy code works without any changes
- Global variables automatically synchronized với new system
- Drop-in replacement cho existing Engine.cpp

## Development Tips

### When Modifying Engine Code
1. Follow clean architecture patterns
2. Use interfaces for abstraction
3. Apply dependency injection
4. Maintain backward compatibility
5. Add memory management comments

### When Adding New Features
1. Create OpenSpec proposal first
2. Use appropriate interfaces
3. Implement trong core layer
4. Test với mock dependencies
5. Update configuration if needed

### Performance Critical Areas
- **Typing hot path** - Keep in TypingEngine
- **Character lookups** - Use VietnameseOptimized cache
- **Macro expansion** - Leverage trie structure
- **Memory allocation** - Use memory pools

## Important Notes

### Refactoring Benefits
- **3x performance improvement** through optimized data structures
- **Clean architecture** with proper separation of concerns
- **Modern C++ practices** với RAII và smart pointers
- **Zero breaking changes** cho existing applications

### OpenSpec Integration
All new features/capabilities must follow OpenSpec workflow:
1. Check existing specs: `openspec list --specs`
2. Create proposal: Scaffold proposal.md, tasks.md
3. Write deltas: Use ADDED/MODIFIED/REMOVED Requirements
4. Validate: `openspec validate [change-id] --strict`
5. Get approval before implementation
6. Archive after deployment
