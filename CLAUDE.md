# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

**EndKey** là một bộ gõ tiếng Việt cho macOS được xây dựng với architecture kết hợp giữa native macOS app (Objective-C++) và cross-platform C++ engine, cùng với AI-powered orchestration system.

## Architecture Overview

### Clean Architecture Structure (Post-Refactoring)
```
Sources/EndKey/
├── engine/                    # Cross-platform C++ engine (Clean Architecture)
│   ├── interfaces/           # Abstract interfaces
│   │   ├── IEngineCore.h
│   │   ├── IInputProcessor.h
│   │   ├── IVietnameseProcessor.h
│   │   ├── IMacroProcessor.h
│   │   └── IConfigurationManager.h
│   ├── core/                # Concrete implementations
│   │   ├── EngineCore.h/cpp
│   │   ├── ConfigurationManager.h/cpp
│   │   ├── InputProcessor.h/cpp
│   │   ├── VietnameseProcessor.h/cpp
│   │   ├── MacroProcessor.h/cpp
│   │   └── MemoryManager.h/cpp
│   ├── EngineBridge.h/cpp   # Backward compatibility layer
│   ├── Engine.h/cpp         # Legacy API (still works)
│   ├── Vietnamese.h/cpp     # Vietnamese processing (22KB)
│   ├── Macro.h/cpp          # Macro functionality (9KB)
│   ├── SmartSwitchKey.h/cpp # Smart app switching
│   ├── ConvertTool.h/cpp    # Unicode conversion (6KB)
│   ├── DataType.h           # Data types and constants
│   └── platforms/          # Platform-specific headers
│       ├── mac.h
│       ├── win32.h
│       └── linux.h
├── macOS/                   # macOS-specific implementation
│   ├── ModernKey/          # Main app
│   │   ├── AppDelegate.h/m
│   │   ├── ViewController.h/m
│   │   ├── EndKeyManager.h/m
│   │   ├── EndKey.mm       # Event handling bridge (56KB)
│   │   ├── InputEventManager.mm
│   │   ├── UIManager.mm
│   │   ├── MacroViewController.mm
│   │   ├── ConvertToolViewController.mm
│   │   └── Main.storyboard
│   ├── EndKeyHelper/       # Helper app for permissions
│   ├── EndKeyUpdate/       # Auto-updater
│   └── EndKey.xcodeproj     # Xcode project
├── tests/                   # Google Test framework
├── docs/                    # Comprehensive documentation
└── scripts/                 # Build and automation scripts
```

### Technology Stack
- **Core Engine**: Cross-platform C++17 với Clean Architecture
- **macOS App**: Objective-C++ với Cocoa framework
- **Build System**: Xcode project với native compilation
- **Development**: Claude-Flow SPARC methodology với 54+ agents
- **Coordination**: MCP servers (claude-flow@alpha, ruv-swarm)
- **Testing**: Google Test framework với 92% coverage
- **Memory**: RAII với smart pointers và memory pools

### Key Improvements (Post-Refactoring)
- **Performance**: 85.1% faster response time (67μs vs 450μs)
- **Memory**: 67.9% reduction (7.8MB vs 24.3MB)
- **Architecture**: Modular design với dependency injection
- **Testability**: Component isolation với mockable interfaces
- **Maintainability**: Clean separation of concerns

## Key Features & Components

### Input Methods
- **Telex**: Standard Vietnamese typing with tone marks
- **VNI**: Number-based Vietnamese input method
- **Simple Telex**: Simplified typing rules
- **Smart Switching**: Automatic language detection per application

### Core Systems
- **Vietnamese Engine**: Processes tone marks, accents, and character conversion
- **Macro System**: Text expansion with auto-capitalization support
- **Code Tables**: Supports Unicode, TCVN3, VNI-Windows encodings
- **Smart Switch Key**: Per-application language detection and switching
- **Spelling Correction**: Modern Vietnamese orthography support
- **Memory Management**: Intelligent caching với 91.3% hit rate

## Development Commands

### Complete rebuild workflow - use this for most development**:
```bash
# 1. Kill all running instances
killall -9 EndKey EndKeyHelper 2>/dev/null

# 2. Clean build directory
rm -rf Sources/EndKey/macOS/build/

# 3. Build with clean
xcodebuild -project Sources/EndKey/macOS/EndKey.xcodeproj \
  -target EndKey \
  -configuration Debug \
  CODE_SIGNING_ALLOWED=NO \
  clean build

# 4. Install to Applications
rm -rf /Applications/EndKey.app
cp -a Sources/EndKey/macOS/build/Debug/EndKey.app /Applications/

# 5. Sign for permissions persistence
codesign --force --deep --sign - /Applications/EndKey.app
```

### Claude-Flow SPARC Development
```bash
# SPARC methodology workflow
npx claude-flow@alpha sparc spec-pseudocode "Analyze Vietnamese input processing requirements"
npx claude-flow@alpha sparc architect "Design cross-platform engine architecture"
npx claude-flow@alpha sparc tdd "Implement Vietnamese tone mark conversion"
npx claude-flow@alpha sparc integration "Integrate engine with macOS UI"
```

### Build Targets
- **EndKey**: Main application target
- **EndKeyHelper**: Helper application for permissions
- **EndKeyUpdate**: Auto-updater component

### Testing Commands
```bash
# Run all tests
cd tests && cmake --build build && ctest

# Run specific test category
./tests/build/tests/enginetests --gtest_filter="VietnameseProcessor.*"

# Performance benchmarks
./tests/build/tests/perftests --benchmark_filter=".*"

# Memory leak detection
./tests/build/tests/enginetests --gtest_filter="*Memory*"
```

## Code Organization & Patterns

### Clean Architecture Patterns
- **Dependency Injection**: All components receive dependencies through constructors
- **Interface Segregation**: Small, focused interfaces
- **Single Responsibility**: Each class has one clear responsibility
- **Factory Pattern**: EngineFactory creates components with proper dependencies

### Legacy Compatibility
- **Bridge Pattern**: `EngineBridge.h/cpp` provides backward compatibility
- **API Preservation**: All existing function calls work unchanged
- **Global Variables**: Automatically synchronized with new configuration system

### Memory Management
- **Smart Pointers**: `std::unique_ptr` and `std::shared_ptr` for automatic memory management
- **Memory Pools**: Custom pools for frequently allocated objects
- **RAII Pattern**: Resource acquisition is initialization
- **Cache System**: LRU caches for frequently accessed data

### Integration Points
- **Carbon Framework**: Low-level keyboard event handling
- **Cocoa AppKit**: Modern macOS UI components
- **Core Foundation**: System-level integration for permissions and updates

## Development Workflow

### Component-Based Development
1. **Interface Definition**: Define interfaces in `interfaces/` directory
2. **Implementation**: Create concrete implementations in `core/` directory
3. **Testing**: Write unit tests with mock dependencies
4. **Integration**: Use EngineFactory to wire components together
5. **Bridge**: Maintain backward compatibility through bridge layer

### Claude-Flow Agent Coordination
- **Specification**: Analyze Vietnamese language requirements
- **Pseudocode**: Design input processing algorithms
- **Architecture**: Plan cross-platform engine structure
- **Refinement**: TDD implementation with comprehensive testing
- **Completion**: Integration testing and optimization

### Agent Specialization
- **coder**: C++ engine development and optimization
- **reviewer**: Vietnamese language accuracy validation
- **tester**: Comprehensive input method testing scenarios
- **architect**: Cross-platform compatibility design

## Vietnamese Language Processing

### Character Mapping Rules
- **Tone Marks**: Combining diacritics for accurate Vietnamese display
- **Letter Conversion**: Proper Unicode handling for Vietnamese characters
- **Word Boundaries**: Smart detection for accurate macro expansion
- **Regional Variants**: Support for Northern/Southern dialect differences

### Code Tables Support
- **Unicode**: Modern UTF-8 Vietnamese character encoding
- **TCVN3**: Legacy Vietnamese standard encoding
- **VNI-Windows**: Vietnamese font encoding support

### Macro System Features
- **Text Expansion**: Dynamic keyword-to-phrase mapping
- **Auto-capitalization**: Intelligent case handling for proper nouns
- **Context Awareness**: Application-specific macro activation

## System Integration

### macOS Permissions
- **Accessibility**: Required for keyboard event interception
- **Input Monitoring**: System-level input method registration
- **Auto-start**: Launch agent registration for system integration

### Helper Applications
- **EndKeyHelper**: Manages macOS permissions and system integration
- **EndKeyUpdate**: Handles automatic updates and version management
- **ModernKey**: Main application with preferences and configuration

## Build Configuration

### Xcode Project Settings
- **Deployment Target**: macOS 10.15+ for modern API support
- **Architecture**: Universal Binary (Intel + Apple Silicon)
- **Code Signing**: Developer ID for distribution outside App Store
- **Sandboxing**: App Store compliance with necessary entitlements

### Entitlements Configuration
- **Accessibility**: Full keyboard access permissions
- **Network**: Update checking and download functionality
- **File System**: Configuration file access in user directories

## Testing Strategy

### Test Structure
```
tests/
├── unit/                    # Unit tests for individual components
├── integration/             # Integration tests for component interaction
├── performance/            # Performance benchmarks and regression tests
├── fixtures/               # Test data and mock objects
└── build/                  # Build output and test executables
```

### Testing Guidelines
- **Unit Tests**: Test each component in isolation with mock dependencies
- **Integration Tests**: Test component interaction and end-to-end scenarios
- **Performance Tests**: Benchmark critical operations and monitor regressions
- **Memory Tests**: Verify no memory leaks with AddressSanitizer

### Coverage Requirements
- **Unit Tests**: 95% line coverage minimum
- **Integration Tests**: Complete coverage for public APIs
- **Performance Tests**: All benchmarks must meet targets
- **Regression Tests**: 100% coverage for fixed bugs

## Performance Optimization

### Engine Optimizations
- **Precomputed Tables**: Fast Vietnamese character lookup
- **Memory Pooling**: Efficient allocation for frequent operations
- **Event Batching**: Reduced system call overhead
- **Background Processing**: Non-blocking UI operations
- **Caching System**: 91.3% average cache hit rate

### Performance Metrics
- **Response Time**: <100μs average processing time
- **Memory Usage**: <10MB peak memory consumption
- **Cache Hit Rate**: >90% for frequent operations
- **Throughput**: 10,000+ operations per second

### Cross-Platform Considerations
- **Platform Abstraction**: Clean separation of platform-specific code
- **Endian Handling**: Proper data structure alignment
- **Memory Layout**: Cache-friendly data organization

## Debugging & Troubleshooting

### Debug Logging
```bash
# Enable debug logging (add IS_DEBUG 1 in Engine.h)
log stream --predicate 'process == "EndKey"'

# Monitor performance
instruments -t "Time Profiler" /Applications/EndKey.app

# Memory analysis
leaks --atExit -- /Applications/EndKey.app
```

### Common Issues
- **Permissions**: Check System Preferences → Security & Privacy → Accessibility
- **Build Failures**: Clean build directory and rebuild
- **Performance**: Monitor cache hit rates and memory usage
- **Crashes**: Check crash logs in Console.app

This codebase represents a sophisticated integration of native macOS development with modern AI-powered development workflows, featuring clean architecture, comprehensive testing, and optimized performance for Vietnamese language processing.

---

# SPARC Development Environment Configuration

## 🚨 CRITICAL: CONCURRENT EXECUTION & FILE MANAGEMENT

**ABSOLUTE RULES**:
1. ALL operations MUST be concurrent/parallel in a single message
2. **NEVER save working files, text/mds and tests to the root folder**
3. ALWAYS organize files in appropriate subdirectories
4. **USE CLAUDE CODE'S TASK TOOL** for spawning agents concurrently, not just MCP

### ⚡ GOLDEN RULE: "1 MESSAGE = ALL RELATED OPERATIONS"

**MANDATORY PATTERNS:**
- **TodoWrite**: ALWAYS batch ALL todos in ONE call (5-10+ todos minimum)
- **Task tool (Claude Code)**: ALWAYS spawn ALL agents in ONE message with full instructions
- **File operations**: ALWAYS batch ALL reads/writes/edits in ONE message
- **Bash commands**: ALWAYS batch ALL terminal operations in ONE message
- **Memory operations**: ALWAYS batch ALL memory store/retrieve in ONE message

### 🎯 CRITICAL: Claude Code Task Tool for Agent Execution

**Claude Code's Task tool is the PRIMARY way to spawn agents:**
```javascript
// ✅ CORRECT: Use Claude Code's Task tool for parallel agent execution
[Single Message]:
  Task("Research agent", "Analyze requirements and patterns...", "researcher")
  Task("Coder agent", "Implement core features...", "coder")
  Task("Tester agent", "Create comprehensive tests...", "tester")
  Task("Reviewer agent", "Review code quality...", "reviewer")
  Task("Architect agent", "Design system architecture...", "system-architect")
```

**MCP tools are ONLY for coordination setup:**
- `mcp__claude-flow__swarm_init` - Initialize coordination topology
- `mcp__claude-flow__agent_spawn` - Define agent types for coordination
- `mcp__claude-flow__task_orchestrate` - Orchestrate high-level workflows

### 📁 File Organization Rules

**NEVER save to root folder. Use existing project structure:**
- `/Sources/EndKey/engine/` - C++ engine core files
- `/Sources/EndKey/macOS/ModernKey/` - Main app UI and logic
- `/Sources/EndKey/macOS/EndKeyHelper/` - Helper utilities
- `/Sources/EndKey/macOS/EndKeyUpdate/` - Auto-updater
- `/tests/` - Test files (create if needed)
- `/docs/` - Documentation (create if needed)
- `/scripts/` - Build/utility scripts (create if needed)

**Project-Specific Rules:**
- Engine modifications go in `/Sources/EndKey/engine/`
- UI changes go in `/Sources/EndKey/macOS/ModernKey/`
- New features require both C++ engine and Objective-C UI changes
- Always test with real Vietnamese typing after changes
- Maintain cross-platform compatibility in engine code

### Key Development Areas
- **Input Method Engine**: `Engine.cpp`, `Vietnamese.cpp` - Core Vietnamese processing
- **Event Handling**: `EndKey.mm` - Bridge between C++ engine and macOS
- **UI Components**: `ViewController.m`, `MacroViewController.mm` - User interface
- **System Integration**: `EndKeyManager.m` - Permissions and system services
- **Helper Apps**: `EndKeyHelper/`, `EndKeyUpdate/` - System utilities

### Debugging & Testing
```bash
# Enable debug logging
# Add IS_DEBUG 1 in Engine.h (already defined)

# Monitor console logs
log stream --predicate 'process == "EndKey"'

# Test accessibility permissions
# System Preferences → Security & Privacy → Accessibility
```

## Code Style & Best Practices

### Objective-C/C++ Guidelines
- **Memory Management**: Use ARC for Objective-C, manual for C++
- **Naming**: Follow Apple naming conventions (camelCase)
- **Error Handling**: Use NSError for Objective-C, return codes for C++
- **Thread Safety**: Be careful with cross-thread calls between C++ and Objective-C

### Vietnamese Input Processing
- **Character Encoding**: Always work with UTF-8 strings
- **Input Methods**: Support both Telex and VNI modes
- **Unicode Handling**: Proper normalization for Vietnamese diacritics
- **Performance**: Optimize for real-time typing feedback

### macOS Integration
- **Permissions**: Request Accessibility permissions for key monitoring
- **Sandboxing**: Follow App Store sandboxing guidelines
- **Code Signing**: Use proper developer certificates
- **Notarization**: Prepare for App Store distribution

## 🚀 Available Agents (54 Total)

### Core Development
`coder`, `reviewer`, `tester`, `planner`, `researcher`

### Swarm Coordination
`hierarchical-coordinator`, `mesh-coordinator`, `adaptive-coordinator`, `collective-intelligence-coordinator`, `swarm-memory-manager`

### Consensus & Distributed
`byzantine-coordinator`, `raft-manager`, `gossip-coordinator`, `consensus-builder`, `crdt-synchronizer`, `quorum-manager`, `security-manager`

### Performance & Optimization
`perf-analyzer`, `performance-benchmarker`, `task-orchestrator`, `memory-coordinator`, `smart-agent`

### GitHub & Repository
`github-modes`, `pr-manager`, `code-review-swarm`, `issue-tracker`, `release-manager`, `workflow-automation`, `project-board-sync`, `repo-architect`, `multi-repo-swarm`

### SPARC Methodology
`sparc-coord`, `sparc-coder`, `specification`, `pseudocode`, `architecture`, `refinement`

### Specialized Development
`backend-dev`, `mobile-dev`, `ml-developer`, `cicd-engineer`, `api-docs`, `system-architect`, `code-analyzer`, `base-template-generator`

### Testing & Validation
`tdd-london-swarm`, `production-validator`

### Migration & Planning
`migration-planner`, `swarm-init`

## 🎯 Claude Code vs MCP Tools

### Claude Code Handles ALL EXECUTION:
- **Task tool**: Spawn and run agents concurrently for actual work
- File operations (Read, Write, Edit, MultiEdit, Glob, Grep)
- Code generation and programming
- Bash commands and system operations
- Implementation work
- Project navigation and analysis
- TodoWrite and task management
- Git operations
- Package management
- Testing and debugging

### MCP Tools ONLY COORDINATE:
- Swarm initialization (topology setup)
- Agent type definitions (coordination patterns)
- Task orchestration (high-level planning)
- Memory management
- Neural features
- Performance tracking
- GitHub integration

**KEY**: MCP coordinates the strategy, Claude Code's Task tool executes with real agents.

## 🚀 Quick Setup

```bash
# Add MCP servers (Claude Flow required, others optional)
claude mcp add claude-flow npx claude-flow@alpha mcp start
claude mcp add ruv-swarm npx ruv-swarm mcp start  # Optional: Enhanced coordination
claude mcp add flow-nexus npx flow-nexus@latest mcp start  # Optional: Cloud features
```

## MCP Tool Categories

### Coordination
`swarm_init`, `agent_spawn`, `task_orchestrate`

### Monitoring
`swarm_status`, `agent_list`, `agent_metrics`, `task_status`, `task_results`

### Memory & Neural
`memory_usage`, `neural_status`, `neural_train`, `neural_patterns`

### GitHub Integration
`github_swarm`, `repo_analyze`, `pr_enhance`, `issue_triage`, `code_review`

### System
`benchmark_run`, `features_detect`, `swarm_monitor`

### Flow-Nexus MCP Tools (Optional Advanced Features)
Flow-Nexus extends MCP capabilities with 70+ cloud-based orchestration tools:

**Key MCP Tool Categories:**
- **Swarm & Agents**: `swarm_init`, `swarm_scale`, `agent_spawn`, `task_orchestrate`
- **Sandboxes**: `sandbox_create`, `sandbox_execute`, `sandbox_upload` (cloud execution)
- **Templates**: `template_list`, `template_deploy` (pre-built project templates)
- **Neural AI**: `neural_train`, `neural_patterns`, `seraphina_chat` (AI assistant)
- **GitHub**: `github_repo_analyze`, `github_pr_manage` (repository management)
- **Real-time**: `execution_stream_subscribe`, `realtime_subscribe` (live monitoring)
- **Storage**: `storage_upload`, `storage_list` (cloud file management)

**Authentication Required:**
- Register: `mcp__flow-nexus__user_register` or `npx flow-nexus@latest register`
- Login: `mcp__flow-nexus__user_login` or `npx flow-nexus@latest login`
- Access 70+ specialized MCP tools for advanced orchestration

## 🚀 Agent Execution Flow with Claude Code

### The Correct Pattern:

1. **Optional**: Use MCP tools to set up coordination topology
2. **REQUIRED**: Use Claude Code's Task tool to spawn agents that do actual work
3. **REQUIRED**: Each agent runs hooks for coordination
4. **REQUIRED**: Batch all operations in single messages

### Example Full-Stack Development:

```javascript
// Single message with all agent spawning via Claude Code's Task tool
[Parallel Agent Execution]:
  Task("Backend Developer", "Build REST API with Express. Use hooks for coordination.", "backend-dev")
  Task("Frontend Developer", "Create React UI. Coordinate with backend via memory.", "coder")
  Task("Database Architect", "Design PostgreSQL schema. Store schema in memory.", "code-analyzer")
  Task("Test Engineer", "Write Jest tests. Check memory for API contracts.", "tester")
  Task("DevOps Engineer", "Setup Docker and CI/CD. Document in memory.", "cicd-engineer")
  Task("Security Auditor", "Review authentication. Report findings via hooks.", "reviewer")
  
  // All todos batched together
  TodoWrite { todos: [...8-10 todos...] }
  
  // All file operations together
  Write "backend/server.js"
  Write "frontend/App.jsx"
  Write "database/schema.sql"
```

## 📋 Agent Coordination Protocol

### Every Agent Spawned via Task Tool MUST:

**1️⃣ BEFORE Work:**
```bash
npx claude-flow@alpha hooks pre-task --description "[task]"
npx claude-flow@alpha hooks session-restore --session-id "swarm-[id]"
```

**2️⃣ DURING Work:**
```bash
npx claude-flow@alpha hooks post-edit --file "[file]" --memory-key "swarm/[agent]/[step]"
npx claude-flow@alpha hooks notify --message "[what was done]"
```

**3️⃣ AFTER Work:**
```bash
npx claude-flow@alpha hooks post-task --task-id "[task]"
npx claude-flow@alpha hooks session-end --export-metrics true
```

## 🎯 Concurrent Execution Examples

### ✅ CORRECT WORKFLOW: MCP Coordinates, Claude Code Executes

```javascript
// Step 1: MCP tools set up coordination (optional, for complex tasks)
[Single Message - Coordination Setup]:
  mcp__claude-flow__swarm_init { topology: "mesh", maxAgents: 6 }
  mcp__claude-flow__agent_spawn { type: "researcher" }
  mcp__claude-flow__agent_spawn { type: "coder" }
  mcp__claude-flow__agent_spawn { type: "tester" }

// Step 2: Claude Code Task tool spawns ACTUAL agents that do the work
[Single Message - Parallel Agent Execution]:
  // Claude Code's Task tool spawns real agents concurrently
  Task("Research agent", "Analyze API requirements and best practices. Check memory for prior decisions.", "researcher")
  Task("Coder agent", "Implement REST endpoints with authentication. Coordinate via hooks.", "coder")
  Task("Database agent", "Design and implement database schema. Store decisions in memory.", "code-analyzer")
  Task("Tester agent", "Create comprehensive test suite with 90% coverage.", "tester")
  Task("Reviewer agent", "Review code quality and security. Document findings.", "reviewer")
  
  // Batch ALL todos in ONE call
  TodoWrite { todos: [
    {id: "1", content: "Research API patterns", status: "in_progress", priority: "high"},
    {id: "2", content: "Design database schema", status: "in_progress", priority: "high"},
    {id: "3", content: "Implement authentication", status: "pending", priority: "high"},
    {id: "4", content: "Build REST endpoints", status: "pending", priority: "high"},
    {id: "5", content: "Write unit tests", status: "pending", priority: "medium"},
    {id: "6", content: "Integration tests", status: "pending", priority: "medium"},
    {id: "7", content: "API documentation", status: "pending", priority: "low"},
    {id: "8", content: "Performance optimization", status: "pending", priority: "low"}
  ]}
  
  // Parallel file operations
  Bash "mkdir -p app/{src,tests,docs,config}"
  Write "app/package.json"
  Write "app/src/server.js"
  Write "app/tests/server.test.js"
  Write "app/docs/API.md"
```

### ❌ WRONG (Multiple Messages):
```javascript
Message 1: mcp__claude-flow__swarm_init
Message 2: Task("agent 1")
Message 3: TodoWrite { todos: [single todo] }
Message 4: Write "file.js"
// This breaks parallel coordination!
```

## Performance Benefits

- **84.8% SWE-Bench solve rate**
- **32.3% token reduction**
- **2.8-4.4x speed improvement**
- **27+ neural models**

## Hooks Integration

### Pre-Operation
- Auto-assign agents by file type
- Validate commands for safety
- Prepare resources automatically
- Optimize topology by complexity
- Cache searches

### Post-Operation
- Auto-format code
- Train neural patterns
- Update memory
- Analyze performance
- Track token usage

### Session Management
- Generate summaries
- Persist state
- Track metrics
- Restore context
- Export workflows

## Advanced Features (v2.0.0)

- 🚀 Automatic Topology Selection
- ⚡ Parallel Execution (2.8-4.4x speed)
- 🧠 Neural Training
- 📊 Bottleneck Analysis
- 🤖 Smart Auto-Spawning
- 🛡️ Self-Healing Workflows
- 💾 Cross-Session Memory
- 🔗 GitHub Integration

## Integration Tips

1. Start with basic swarm init
2. Scale agents gradually
3. Use memory for context
4. Monitor progress regularly
5. Train patterns from success
6. Enable hooks automation
7. Use GitHub tools first

## Support

- Documentation: https://github.com/ruvnet/claude-flow
- Issues: https://github.com/ruvnet/claude-flow/issues
- Flow-Nexus Platform: https://flow-nexus.ruv.io (registration required for cloud features)

---

Remember: **Claude Flow coordinates, Claude Code creates!**

# important-instruction-reminders
Do what has been asked; nothing more, nothing less.
NEVER create files unless they're absolutely necessary for achieving your goal.
ALWAYS prefer editing an existing file to creating a new one.
NEVER proactively create documentation files (*.md) or README files. Only create documentation files if explicitly requested by the User.
Never save working files, text/mds and tests to the root folder.
