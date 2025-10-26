cmake_minimum_required(VERSION 3.16)
project(EndKeyTests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Sources/EndKey/engine)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../Sources/EndKey/engine/platforms)

# Engine source files for testing
set(ENGINE_SOURCES
  ../Sources/EndKey/engine/Engine.cpp
  ../Sources/EndKey/engine/Vietnamese.cpp
  ../Sources/EndKey/engine/Macro.cpp
  ../Sources/EndKey/engine/ConvertTool.cpp
  ../Sources/EndKey/engine/SmartSwitchKey.cpp
)

# Add test executable
add_executable(EndKeyTests
  test_main.cpp
  vietnamese_input_tests.cpp
  performance_tests.cpp
  integration_tests.cpp
  regression_tests.cpp
  macro_tests.cpp
  code_table_tests.cpp
  ${ENGINE_SOURCES}
)

# Link with GoogleTest
target_link_libraries(EndKeyTests gtest_main)

# Enable code coverage
option(ENABLE_COVERAGE "Enable coverage" ON)
if(ENABLE_COVERAGE)
  target_compile_options(EndKeyTests PRIVATE --coverage)
  target_link_options(EndKeyTests PRIVATE --coverage)
endif()

# Add test discovery
enable_testing()
include(GoogleTest)
gtest_discover_tests(EndKeyTests)