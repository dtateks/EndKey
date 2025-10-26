# EndKey Development Workflow

## Claude-Flow SPARC Methodology

### Development Phases
1. **Specification**: Analyze Vietnamese language requirements
2. **Pseudocode**: Design input processing algorithms  
3. **Architecture**: Plan cross-platform engine structure
4. **Refinement**: TDD implementation với comprehensive testing
5. **Completion**: Integration testing và optimization

### Agent Coordination Protocol

#### Before Work (Every Agent MUST run):
```bash
npx claude-flow@alpha hooks pre-task --description "[task description]"
npx claude-flow@alpha hooks session-restore --session-id "swarm-[id]"
```

#### During Work:
```bash
npx claude-flow@alpha hooks post-edit --file "[filename]" --memory-key "swarm/[agent]/[step]"
npx claude-flow@alpha hooks notify --message "[progress update]"
```

#### After Work:
```bash
npx claude-flow@alpha hooks post-task --task-id "[task]"
npx claude-flow@alpha hooks session-end --export-metrics true
```

## File Organization Rules

### Project Structure
- **NEVER save to root folder** - Use existing project structure
- **Engine files**: `/Sources/EndKey/engine/` - C++ engine core
- **UI files**: `/Sources/EndKey/macOS/ModernKey/` - Main app interface
- **Helper files**: `/Sources/EndKey/macOS/EndKeyHelper/` - System utilities
- **Tests**: `/tests/` - Test files (create if needed)
- **Documentation**: `/docs/` - Project docs (create if needed)
- **Scripts**: `/scripts/` - Build/utility scripts (create if needed)

### Critical Rules
1. **Engine modifications** go in C++ engine directory
2. **UI changes** go in ModernKey directory  
3. **New features** require both C++ engine và Objective-C UI changes
4. **Always test** với real Vietnamese typing after changes
5. **Maintain cross-platform compatibility** in engine code

## Key Development Areas

### Core Components
- **Input Method Engine**: `Engine.cpp`, `Vietnamese.cpp` - Core Vietnamese processing
- **Event Handling**: `EndKey.mm` - Bridge between C++ engine và macOS
- **UI Components**: `ViewController.m`, `MacroViewController.mm` - User interface
- **System Integration**: `EndKeyManager.m` - Permissions và system services
- **Helper Apps**: `EndKeyHelper/`, `EndKeyUpdate/` - System utilities

### Common Tasks

#### Adding New Vietnamese Input Features
1. Modify C++ engine (`Vietnamese.cpp`, `Engine.h`)
2. Update Objective-C bridge (`EndKey.mm`)
3. Add UI controls (`ViewController.m`)
4. Test với real Vietnamese text input

#### Modifying Macro System
1. Update C++ macro engine (`Macro.cpp`, `Macro.h`)
2. Modify Objective-C UI (`MacroViewController.mm`)
3. Update storage và persistence
4. Test macro expansion scenarios

#### System Integration Changes
1. Update helper applications (`EndKeyHelper/`)
2. Modify entitlements và permissions
3. Test system integration
4. Verify accessibility permissions

## Testing & Quality Assurance

### Required Testing After Changes
1. **Vietnamese typing test**: Verify input methods work correctly
2. **Macro expansion test**: Check text replacement functionality
3. **Smart switching test**: Verify per-app language detection
4. **System integration test**: Check permissions và system services
5. **Performance test**: Ensure no typing lag

### Debug Commands
```bash
# Enable debug logging (IS_DEBUG 1 already defined in Engine.h)
# Monitor console logs
log stream --predicate 'process == "EndKey"'

# Test accessibility permissions
# System Preferences → Security & Privacy → Accessibility
```

## Build & Deployment

### Development Build
```bash
# Complete rebuild workflow
killall -9 EndKey EndKeyHelper 2>/dev/null
rm -rf Sources/EndKey/macOS/build/
xcodebuild -project Sources/EndKey/macOS/EndKey.xcodeproj \
  -target EndKey \
  -configuration Debug \
  CODE_SIGNING_ALLOWED=NO \
  clean build
rm -rf /Applications/EndKey.app
cp -a Sources/EndKey/macOS/build/Debug/EndKey.app /Applications/
codesign --force --deep --sign - /Applications/EndKey.app
```

### Testing Build
- **Debug Configuration**: Development với debug symbols
- **Code Signing**: Disabled cho development
- **Installation**: Manual copy đến /Applications
- **Verification**: Test real Vietnamese typing

## Performance Optimization

### Engine Optimizations
- **Precomputed Tables**: Fast Vietnamese character lookup
- **Memory Pooling**: Efficient allocation cho frequent operations
- **Event Batching**: Reduced system call overhead
- **Background Processing**: Non-blocking UI operations

### Cross-Platform Considerations
- **Platform Abstraction**: Clean separation of platform-specific code
- **Endian Handling**: Proper data structure alignment
- **Memory Layout**: Cache-friendly data organization

## Memory & Session Management

### Agent Memory System
- **Session-based**: Date-organized development sessions
- **Agent Memory**: Persistent context across Claude Code instances
- **Coordination State**: SQLite databases cho swarm orchestration
- **Knowledge Sharing**: Cross-agent learning via memory patterns

### Memory Operations
- **Store**: Use `npx claude-flow@alpha memory store` để save insights
- **Retrieve**: Use `npx claude-flow@alpha memory get` để access prior knowledge
- **Search**: Use `npx claude-flow@alpha memory search` để find patterns
- **Export**: Use `npx claude-flow@alpha memory export` để backup sessions