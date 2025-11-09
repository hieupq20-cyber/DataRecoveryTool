# Data Recovery Tool

A Windows-based NTFS file recovery utility written in C++. This tool helps recover deleted files from NTFS file systems by scanning the Master File Table (MFT) and restoring files that haven't been overwritten yet.

## ⚠️ Important Warnings

- **Run as Administrator**: Required for raw disk access
- **Stop Using the Drive**: Prevent overwriting deleted files
- **Save to Different Drive**: Never recover to the same drive
- **No Guarantees**: Recovery success depends on whether data has been overwritten
- **Read-Only**: Tool only reads from disk, never writes to source volume

## Features

✅ **NTFS Support**: Works with NTFS file systems (Windows default)  
✅ **Raw Disk Access**: Low-level disk reading for deleted file recovery  
✅ **MFT Scanning**: Scans Master File Table for deleted entries  
✅ **Console Interface**: Simple menu-driven interface  
✅ **Safe Operation**: Read-only access to source drive  
✅ **Detailed Information**: Shows file name, size, and recovery status  

## Quick Start

### Prerequisites
- Windows 7 or later
- Administrator privileges
- NTFS-formatted drive
- CMake 3.10+ and MSVC or MinGW compiler (for building)

### Building

**Option 1: Using build script**
```cmd
build.bat
```

**Option 2: Manual build**
```cmd
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

Executable will be in: `build\bin\Release\DataRecoveryTool.exe`

For detailed build instructions, see [docs/BUILD_INSTRUCTIONS.md](docs/BUILD_INSTRUCTIONS.md)

### Running

1. Right-click `DataRecoveryTool.exe`
2. Select "Run as administrator"
3. Follow the menu prompts

## Usage

### Main Menu Options

1. **List available volumes** - Shows all drives on the system
2. **Select volume and scan** - Scan a drive for deleted files
3. **View deleted files list** - Display found deleted files
4. **Recover file by index** - Recover a specific file
5. **Exit** - Close the program

### Example Session

```
Data Recovery Tool v1.0

=== Main Menu ===
Enter choice: 2

Enter drive letter (e.g., C): C
Enter maximum MFT entries to scan: 10000

Scanning for deleted files...
Found 15 deleted files.

Index | File Name       | Size
------+----------------+--------
    0 | document.docx  | 45 KB
    1 | photo.jpg      | 2.3 MB
    2 | video.mp4      | 125 MB

Enter choice: 4
Enter file index to recover: 0
Enter output path: D:\Recovered\document.docx

Recovering file: document.docx
Recovered 46080 / 46080 bytes
File recovered successfully!
```

For detailed usage instructions, see [docs/USER_GUIDE.md](docs/USER_GUIDE.md)

## Project Structure

```
DataRecoveryTool/
├── src/                    # Source files
│   ├── main.cpp           # Main application
│   ├── DiskEnumerator.cpp # Volume enumeration
│   ├── RawDiskReader.cpp  # Low-level disk access
│   ├── NtfsParser.cpp     # NTFS structure parsing
│   ├── RecoveryScanner.cpp # Deleted file scanning
│   └── RecoveryEngine.cpp # File recovery logic
├── include/               # Header files
│   ├── DiskEnumerator.h
│   ├── RawDiskReader.h
│   ├── NtfsStructures.h
│   ├── NtfsParser.h
│   ├── RecoveryScanner.h
│   └── RecoveryEngine.h
├── docs/                  # Documentation
│   ├── BUILD_INSTRUCTIONS.md
│   ├── TECHNICAL_DOCUMENTATION.md
│   └── USER_GUIDE.md
├── tests/                 # Test files (future)
├── CMakeLists.txt        # CMake build configuration
├── build.bat             # Windows build script
└── README.md             # This file
```

## How It Works

### Technical Overview

1. **Volume Enumeration**: Lists all volumes using Windows API
2. **Raw Disk Access**: Opens volume with `CreateFileW` for direct access
3. **Boot Sector Parsing**: Reads NTFS boot sector to locate MFT
4. **MFT Scanning**: Iterates through MFT entries to find deleted files
5. **File Recovery**: Reads file data using runlist and writes to output

### Key Components

- **DiskEnumerator**: Lists and displays volume information
- **RawDiskReader**: Provides low-level sector reading
- **NtfsParser**: Parses NTFS structures (boot sector, MFT entries)
- **RecoveryScanner**: Scans MFT for deleted files
- **RecoveryEngine**: Recovers files from disk clusters

For technical details, see [docs/TECHNICAL_DOCUMENTATION.md](docs/TECHNICAL_DOCUMENTATION.md)

## Implementation Status

Based on the README.md phases:

### ✅ Completed Phases

- **Phase 0**: Project structure and setup
- **Phase 1**: Raw disk access
  - ✅ Task 1: DiskEnumerator (list volumes)
  - ✅ Task 2: RawDiskReader (raw volume access, ReadSectors)
- **Phase 2**: NTFS Parser
  - ✅ Task 3: NTFS Boot Sector parsing
  - ✅ Task 4: MFT location and reading
  - ✅ Task 5: MFT Entry parsing
- **Phase 3**: Find deleted files
  - ✅ Task 6: Identify deleted entries
  - ✅ Task 7: RecoveryScanner module
- **Phase 4**: Recovery engine
  - ✅ Task 8: File recovery from runlist
  - ✅ Task 9: Error handling and status reporting
- **Phase 5**: CLI Interface
  - ✅ Task 11: Console menu system

### 🔲 Future Enhancements (Not Yet Implemented)

- **Phase 6**: GUI interface (Qt or Win32)
- **Phase 7**: Advanced features
  - Logging system
  - FAT32/exFAT support
  - File carving (signature-based recovery)
  - Timestamp recovery
  - Path reconstruction

## Limitations

- **NTFS Only**: Currently supports only NTFS file systems
- **Overwritten Data**: Cannot recover files that have been overwritten
- **SSD TRIM**: Harder to recover from SSDs due to TRIM command
- **Encryption**: Cannot recover encrypted files without keys
- **Complex Fragmentation**: Highly fragmented files may have issues

## Best Practices

1. **Act Quickly**: Recover files immediately after deletion
2. **Minimize Disk Use**: Stop using the drive after file deletion
3. **Different Drive**: Always save recovered files to a different drive
4. **Regular Backups**: Prevention is better than recovery

## Troubleshooting

| Problem | Solution |
|---------|----------|
| "Failed to open volume" | Run as Administrator |
| "Not a valid NTFS boot sector" | Verify drive is NTFS formatted |
| "No deleted files found" | Increase scan entry count or data may be overwritten |
| "Failed to create output file" | Check output path and permissions |

See [docs/USER_GUIDE.md](docs/USER_GUIDE.md) for more troubleshooting help.

## Documentation

- **[User Guide](docs/USER_GUIDE.md)**: How to use the tool
- **[Technical Documentation](docs/TECHNICAL_DOCUMENTATION.md)**: Architecture and implementation details
- **[Build Instructions](docs/BUILD_INSTRUCTIONS.md)**: How to compile the project

## Development

### Building from Source

See [docs/BUILD_INSTRUCTIONS.md](docs/BUILD_INSTRUCTIONS.md) for detailed instructions.

### Contributing

Future contributions welcome:
- FAT32/exFAT support
- GUI implementation
- File carving algorithms
- Better fragmentation handling
- Logging system
- Unit tests

## License

This project is provided as-is for educational and data recovery purposes.

## Disclaimer

- Use at your own risk
- No guarantee of successful recovery
- For critical data, consider professional data recovery services
- Only use on drives you own or have permission to access

## References

- Microsoft NTFS Documentation
- Windows Driver Kit (WDK)
- "Windows Internals" by Mark Russinovich
- NTFS.com technical specifications

## Author

Data Recovery Tool - Educational C++ project for NTFS file recovery

---

**Remember**: The best data recovery is prevention through regular backups! 💾
