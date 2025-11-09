cmake_minimum_required(VERSION 3.10)
project(DataRecoveryTool)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/DiskEnumerator.cpp
    src/RawDiskReader.cpp
    src/NtfsParser.cpp
    src/RecoveryScanner.cpp
    src/RecoveryEngine.cpp
)

# Create executable
add_executable(DataRecoveryTool ${SOURCES})

# For Windows, link required libraries
if(WIN32)
    target_compile_definitions(DataRecoveryTool PRIVATE UNICODE _UNICODE)
endif()

# Installation
install(TARGETS DataRecoveryTool DESTINATION bin)
