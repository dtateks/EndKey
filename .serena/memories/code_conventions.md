# EndKey Code Conventions

## General Guidelines
- **Language**: Tiếng Việt cho comments và user-facing strings, English cho technical documentation
- **Memory Management**: ARC cho Objective-C, manual management cho C++
- **Naming**: Apple naming conventions (camelCase cho variables/methods, PascalCase cho classes)
- **Error Handling**: NSError cho Objective-C, return codes cho C++

## Objective-C/C++ Conventions

### Naming Patterns
- **Variables**: camelCase (vLanguage, vInputType, vCheckSpelling)
- **Methods**: camelCase với descriptive names (handleStringReplace, OnActiveAppChanged)
- **Classes**: PascalCase (ViewController, EndKeyManager, MacroViewController)
- **Constants**: UPPER_SNAKE_CASE hoặc prefix với 'k'
- **Functions**: camelCase với verb prefix (getCharacterCode, utf8ToWideString)

### File Organization
- **Headers**: .h cho Objective-C interfaces, .h cho C++ headers
- **Implementation**: .m cho Objective-C, .mm cho Objective-C++, .cpp cho C++
- **Resource files**: Organized trong appropriate subdirectories
- **Platform-specific**: Separate trong platforms/ directory

### Memory Management
- **Objective-C**: Use ARC, avoid manual retain/release
- **C++**: Smart pointers (std::unique_ptr, std::shared_ptr)
- **Bridge objects**: Careful với cross-language memory boundaries
- **Dealloc**: Implement cleanup cho manual C++ resources

## Vietnamese Input Processing

### Character Encoding
- **Always UTF-8**: Work với UTF-8 strings cho Vietnamese characters
- **Unicode Normalization**: Proper normalization cho diacritics
- **Code Tables**: Support multiple encodings (Unicode, TCVN3, VNI-Windows)

### Input Method Implementation
- **Telex Mode**: Standard Vietnamese typing với tone marks
- **VNI Mode**: Number-based input method
- **Quick Consonants**: Automatic replacements (f→ph, j→gi, w→qu)
- **Smart Detection**: Per-application language switching

### Performance Considerations
- **Real-time Processing**: Optimize cho typing feedback
- **Precomputed Tables**: Fast character lookup
- **Memory Pooling**: Efficient allocation cho frequent operations
- **Event Batching**: Reduced system call overhead

## macOS Integration Patterns

### Event Handling
- **Carbon Events**: Low-level keyboard event interception
- **Cocoa Events**: High-level UI event handling
- **Thread Safety**: Proper synchronization cho cross-thread calls
- **Event Batching**: Batch operations cho performance

### System Integration
- **Permissions**: Accessibility API cho key monitoring
- **Sandboxing**: App Store compliance considerations
- **Code Signing**: Proper certificates cho distribution
- **Launch Services**: Auto-start configuration

### UI Development
- **AppKit**: Native macOS UI components
- **Storyboards**: Interface layout và localization
- **Assets**: Image resources với multiple resolutions
- **Localization**: Vietnamese và English language support

## Cross-Platform Considerations

### Platform Abstraction
- **Headers**: Platform-specific declarations trong platforms/
- **Conditional Compilation**: #ifdef cho platform-specific code
- **Shared Logic**: Core functionality independent of platform
- **Build System**: Xcode cho macOS, tương thích với other platforms

### Data Structures
- **Endian Handling**: Proper byte order conversion
- **Memory Layout**: Cache-friendly organization
- **Serialization**: Cross-platform data format
- **Configuration**: Platform-specific settings

## Testing & Debugging

### Debug Configuration
- **Debug Flags**: IS_DEBUG definition cho debug builds
- **Logging**: Console output cho troubleshooting
- **Breakpoints**: Strategic placement cho complex logic
- **Performance**: Profiling cho optimization opportunities

### Testing Strategy
- **Unit Tests**: Core engine functionality
- **Integration Tests**: Cross-component interaction
- **UI Tests**: User interface validation
- **Performance Tests**: Benchmarking critical paths

## Code Quality

### Documentation
- **Header Comments**: Purpose và usage information
- **Inline Comments**: Complex logic explanation
- **API Documentation**: Interface contracts
- **Architecture Documentation**: High-level design decisions

### Style Consistency
- **Indentation**: 4 spaces, no tabs
- **Line Length**: Reasonable limits cho readability
- **Brace Style**: K&R style cho consistency
- **Whitespace**: Proper spacing cho readability