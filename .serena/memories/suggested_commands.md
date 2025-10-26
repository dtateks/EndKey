# Essential Commands for EndKey Development

## Primary Development Commands

### Complete Rebuild (Most Common Development Task)
```bash
# Full rebuild and installation workflow
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

### Quick Build Only (No Installation)
```bash
# Build without installation
xcodebuild -project Sources/EndKey/macOS/EndKey.xcodeproj \
  -target EndKey \
  -configuration Debug \
  CODE_SIGNING_ALLOWED=NO \
  build
```

### Build Specific Targets
```bash
# Build helper app
xcodebuild -project Sources/EndKey/macOS/EndKey.xcodeproj \
  -target EndKeyHelper \
  -configuration Debug \
  CODE_SIGNING_ALLOWED=NO \
  build

# Build update app  
xcodebuild -project Sources/EndKey/macOS/EndKey.xcodeproj \
  -target EndKeyUpdate \
  -configuration Debug \
  CODE_SIGNING_ALLOWED=NO \
  build
```

## Testing & Debugging Commands

### Monitor Application
```bash
# Monitor console logs for debugging
log stream --predicate 'process == "EndKey"'

# Check running processes
ps aux | grep EndKey

# Kill all instances
killall -9 EndKey EndKeyHelper 2>/dev/null
```

### Permissions & System Integration
```bash
# Check accessibility permissions
# System Preferences → Security & Privacy → Accessibility

# Verify code signing
codesign -dv --verbose=4 /Applications/EndKey.app

# Check entitlements
codesign -d --entitlements - /Applications/EndKey.app
```

## Claude-Flow Development Commands

### SPARC Methodology
```bash
# Specification phase
npx claude-flow@alpha sparc spec-pseudocode "Analyze Vietnamese input processing requirements"

# Architecture phase  
npx claude-flow@alpha sparc architect "Design cross-platform engine architecture"

# TDD implementation
npx claude-flow@alpha sparc tdd "Implement Vietnamese tone mark conversion"

# Integration testing
npx claude-flow@alpha sparc integration "Integrate engine with macOS UI"

# Full testing suite
npx claude-flow@alpha sparc test "Vietnamese input method validation"
```

### Agent Coordination
```bash
# Initialize swarm coordination
mcp__claude-flow__swarm_init {topology: "mesh", maxAgents: 5}

# Spawn specialized agents
mcp__claude-flow__agent_spawn {type: "coder"}
mcp__claude-flow__agent_spawn {type: "reviewer"}
mcp__claude-flow__agent_spawn {type: "tester"}

# Orchestrate complex workflows
mcp__claude-flow__task_orchestrate {task: "Implement new Vietnamese input method", priority: "high"}
```

### Memory & Learning
```bash
# Store development insights
npx claude-flow@alpha memory store --key "vietnamese-input-patterns" --value "Telex rules optimization"

# Retrieve prior knowledge  
npx claude-flow@alpha memory get --key "engine-architecture"

# Search memory for patterns
npx claude-flow@alpha memory search --pattern "macro-system"

# Export session knowledge
npx claude-flow@alpha memory export --session "2024-XX-XX"
```

## Performance & Benchmarking

### Performance Analysis
```bash
# Run performance benchmarks
npx claude-flow@alpha benchmark run --type engine --iterations 100

# Memory usage analysis
mcp__claude-flow__memory_usage --action analyze --component engine

# Neural pattern training
mcp__claude-flow__neural_train --pattern_type "coordination" --training_data "session-data.json"

# Performance reporting
mcp__claude-flow__performance_report --timeframe "24h" --format "detailed"
```

### System Diagnostics
```bash
# Check system features
mcp__claude-flow__features_detect --category "all"

# Monitor swarm activity
mcp__claude-flow__swarm_monitor --duration 30 --interval 5

# Agent performance metrics
mcp__claude-flow__agent_metrics --agentId "coder-001"

# Neural status check
mcp__claude-flow__neural_status --modelId "vietnamese-engine"
```

## File & Project Management

### Project Navigation
```bash
# List project structure
find Sources/EndKey -type f -name "*.h" -o -name "*.cpp" -o -name "*.m" -o -name "*.mm"

# Search for specific patterns
grep -r "Vietnamese" Sources/EndKey/engine/
grep -r "EndKeyCallback" Sources/EndKey/macOS/

# Check file dependencies
xcodebuild -project Sources/EndKey/macOS/EndKey.xcodeproj -target EndKey -showBuildSettings
```

### Code Quality
```bash
# Check for compilation warnings
xcodebuild -project Sources/EndKey/macOS/EndKey.xcodeproj -target EndKey build 2>&1 | grep warning

# Analyze code structure
clang++ -Xclang -ast-dump -fsyntax-only Sources/EndKey/engine/Engine.cpp

# Memory leak detection (Valgrind alternative)
leaks --atExit -- /Applications/EndKey.app/Contents/MacOS/EndKey
```

## Git & Version Control

### Common Git Operations
```bash
# Check git status
git status

# View recent commits
git log --oneline -10

# Stage changes
git add Sources/EndKey/engine/
git add Sources/EndKey/macOS/

# Commit changes
git commit -m "Update Vietnamese input processing"

# Push to remote
git push origin main
```

## Utilities & Helper Commands

### System Utilities
```bash
# Check macOS version
sw_vers

# Verify Xcode installation
xcode-select --print-path

# Check available frameworks
ls /System/Library/Frameworks/

# Monitor system resources
top -pid $(pgrep EndKey)
```

### Development Environment
```bash
# Check Node.js and npm (for Claude Flow)
node --version
npm --version

# Verify MCP servers
claude mcp list

# Check Python (if needed for scripts)
python3 --version
```

## Emergency & Recovery

### Application Recovery
```bash
# Force quit stuck application
killall -9 EndKey

# Reset preferences
rm -rf ~/Library/Preferences/com.endkey.EndKey.plist

# Clear application cache
rm -rf ~/Library/Caches/com.endkey.EndKey/

# Reinstall from scratch
rm -rf /Applications/EndKey.app
# Then run complete rebuild workflow
```

### Development Recovery
```bash
# Clean Xcode build cache
rm -rf ~/Library/Developer/Xcode/DerivedData/

# Reset git to clean state
git clean -fd
git reset --hard HEAD

# Rebuild project file
xcodebuild -project Sources/EndKey/macOS/EndKey.xcodeproj clean
```