cmake_minimum_required(VERSION 3.16)
project(DataRecoveryTool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(drt_core
    src/DiskEnumerator.cpp
    src/RawDiskReader.cpp
    src/NtfsStructures.cpp
)

target_include_directories(drt_core PUBLIC include)

add_executable(DataRecoveryTool src/main.cpp)
target_link_libraries(DataRecoveryTool PRIVATE drt_core)

if (MSVC)
    target_compile_definitions(drt_core PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
