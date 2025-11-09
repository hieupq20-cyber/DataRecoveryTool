# Build Instructions for Data Recovery Tool

## Prerequisites

### Required Software
1. **CMake** (3.10 or higher)
   - Download from: https://cmake.org/download/
   - Add to PATH during installation

2. **C++ Compiler** (one of the following):
   - **Visual Studio 2019 or later** (Recommended)
     - Download Community edition: https://visualstudio.microsoft.com/
     - Install "Desktop development with C++" workload
   - **MinGW-w64**
     - Download from: https://www.mingw-w64.org/
     - Or use MSYS2: https://www.msys2.org/

3. **Git** (optional, for cloning repository)
   - Download from: https://git-scm.com/

### System Requirements
- Windows 7 or later
- 64-bit operating system recommended
- At least 100 MB free disk space

## Building the Project

### Option 1: Using Build Script (Easiest)

1. Open Command Prompt or PowerShell
2. Navigate to the project directory:
   ```cmd
   cd path\to\DataRecoveryTool
   ```

3. Run the build script:
   ```cmd
   build.bat
   ```

4. The executable will be created in:
   ```
   build\bin\Release\DataRecoveryTool.exe
   ```

### Option 2: Manual Build with Visual Studio

1. **Generate Visual Studio Solution**
   ```cmd
   mkdir build
   cd build
   cmake .. -G "Visual Studio 16 2019" -A x64
   ```

2. **Open in Visual Studio**
   ```cmd
   start DataRecoveryTool.sln
   ```

3. **Build in Visual Studio**
   - Select "Release" configuration
   - Press F7 or Build → Build Solution
   - Executable: `build\bin\Release\DataRecoveryTool.exe`

### Option 3: Manual Build with MinGW

1. **Generate Makefiles**
   ```cmd
   mkdir build
   cd build
   cmake .. -G "MinGW Makefiles"
   ```

2. **Compile**
   ```cmd
   mingw32-make
   ```

3. **Executable location**
   ```
   build\bin\DataRecoveryTool.exe
   ```

### Option 4: Command Line Build with CMake

1. **Create build directory**
   ```cmd
   mkdir build
   cd build
   ```

2. **Configure**
   ```cmd
   cmake ..
   ```

3. **Build**
   ```cmd
   cmake --build . --config Release
   ```

4. **Executable location**
   ```
   build\bin\Release\DataRecoveryTool.exe
   ```

## Verifying the Build

After building, verify the executable:

```cmd
cd build\bin\Release
DataRecoveryTool.exe
```

You should see the program banner. Note: Some features require Administrator privileges.

## Running the Tool

### Method 1: Right-click (Recommended)
1. Navigate to `build\bin\Release\`
2. Right-click `DataRecoveryTool.exe`
3. Select "Run as administrator"

### Method 2: Command Prompt as Admin
1. Open Command Prompt as Administrator
2. Navigate to executable location
3. Run: `DataRecoveryTool.exe`

## Troubleshooting Build Issues

### CMake Not Found
**Error**: `'cmake' is not recognized as an internal or external command`

**Solution**:
1. Install CMake from https://cmake.org/download/
2. During installation, select "Add CMake to system PATH"
3. Or manually add CMake bin directory to PATH

### Compiler Not Found
**Error**: `No CMAKE_CXX_COMPILER could be found`

**Solution**:
1. Install Visual Studio with C++ workload
2. Or install MinGW-w64
3. Restart Command Prompt after installation

### Visual Studio Version Mismatch
**Error**: Generator "Visual Studio 16 2019" not found

**Solution**:
- For VS 2022: Use `-G "Visual Studio 17 2022"`
- For VS 2017: Use `-G "Visual Studio 15 2017"`
- For MinGW: Use `-G "MinGW Makefiles"`

### Build Failed with Errors
**Error**: Build returns errors during compilation

**Solutions**:
1. Ensure Windows SDK is installed (comes with Visual Studio)
2. Check that you're using a supported compiler
3. Try cleaning and rebuilding:
   ```cmd
   cd build
   cmake --build . --config Release --clean-first
   ```

### Permission Denied
**Error**: Cannot create/write files in build directory

**Solution**:
1. Run Command Prompt as Administrator
2. Or change to a directory where you have write permissions

## Project Structure After Build

```
DataRecoveryTool/
├── build/                  # Build directory (created)
│   ├── bin/
│   │   └── Release/
│   │       └── DataRecoveryTool.exe  # Executable
│   ├── CMakeFiles/        # CMake generated files
│   └── ...
├── docs/                  # Documentation
│   ├── BUILD_INSTRUCTIONS.md
│   ├── TECHNICAL_DOCUMENTATION.md
│   └── USER_GUIDE.md
├── include/               # Header files
│   ├── DiskEnumerator.h
│   ├── RawDiskReader.h
│   ├── NtfsStructures.h
│   ├── NtfsParser.h
│   ├── RecoveryScanner.h
│   └── RecoveryEngine.h
├── src/                   # Source files
│   ├── main.cpp
│   ├── DiskEnumerator.cpp
│   ├── RawDiskReader.cpp
│   ├── NtfsParser.cpp
│   ├── RecoveryScanner.cpp
│   └── RecoveryEngine.cpp
├── tests/                 # Test files (if any)
├── CMakeLists.txt        # CMake configuration
├── build.bat             # Build script
├── .gitignore
└── README.md
```

## Clean Build

To perform a clean build:

```cmd
# Remove build directory
rmdir /s /q build

# Create fresh build
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Building for Different Configurations

### Debug Build
```cmd
cmake --build . --config Debug
```
Executable: `build\bin\Debug\DataRecoveryTool.exe`

### Release Build (Optimized)
```cmd
cmake --build . --config Release
```
Executable: `build\bin\Release\DataRecoveryTool.exe`

## Advanced Build Options

### Static Linking (Single Executable)
Modify CMakeLists.txt to add:
```cmake
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
```

### Enable Warnings
```cmake
if(MSVC)
    target_compile_options(DataRecoveryTool PRIVATE /W4)
else()
    target_compile_options(DataRecoveryTool PRIVATE -Wall -Wextra)
endif()
```

## Distribution

To distribute the tool:

1. Build in Release configuration
2. Copy `DataRecoveryTool.exe` from `build\bin\Release\`
3. Include documentation files from `docs\`
4. Create a README with usage instructions
5. Optionally, compress into a ZIP file

## Next Steps

After successful build:
1. Read `docs/USER_GUIDE.md` for usage instructions
2. Read `docs/TECHNICAL_DOCUMENTATION.md` for technical details
3. Run the tool as Administrator to test functionality

## Support

For build issues:
1. Check this document first
2. Verify all prerequisites are installed
3. Check CMake and compiler versions
4. Review error messages carefully
5. Try clean build if problems persist
