# Data Recovery Tool - Technical Documentation

## Overview
This data recovery tool is designed to recover deleted files from NTFS file systems on Windows. It operates in read-only mode and can scan the Master File Table (MFT) to find deleted files that haven't been overwritten yet.

## Architecture

### Core Modules

#### 1. DiskEnumerator
- **Purpose**: Lists all available volumes on the system
- **Key Functions**:
  - `EnumerateVolumes()`: Returns a list of all drives with their properties
  - `PrintAllVolumes()`: Displays formatted volume information
  
#### 2. RawDiskReader
- **Purpose**: Provides low-level disk access
- **Key Functions**:
  - `OpenVolume()`: Opens a volume for raw reading (requires admin privileges)
  - `ReadSectors()`: Reads sectors from disk
  - `ReadBootSector()`: Reads the first sector of the volume
- **Notes**: Uses Windows API CreateFileW with `FILE_FLAG_NO_BUFFERING` for direct disk access

#### 3. NtfsParser
- **Purpose**: Parses NTFS file system structures
- **Key Functions**:
  - `ParseBootSector()`: Validates and extracts NTFS boot sector information
  - `ReadMftEntry()`: Reads a specific MFT entry
  - `ParseMftEntry()`: Parses MFT entry header
  - `ExtractFileName()`: Extracts file name from $FILE_NAME attribute
  - `ExtractFileSize()`: Extracts file size from $DATA attribute
  - `ExtractDataRuns()`: Parses runlist for non-resident data
  
#### 4. RecoveryScanner
- **Purpose**: Scans MFT for deleted files
- **Key Functions**:
  - `ScanDeletedFiles()`: Iterates through MFT entries to find deleted files
  - `PrintDeletedFiles()`: Displays formatted list of deleted files
- **Detection Logic**: Files with MFT flag `inUse == 0` are considered deleted

#### 5. RecoveryEngine
- **Purpose**: Recovers deleted files to disk
- **Key Functions**:
  - `RecoverFile()`: Reads file data using runlist and writes to output file
- **Recovery Process**:
  1. Parse data runs to locate file clusters
  2. Read clusters from disk
  3. Write data to output file on different drive
  4. Verify recovered size matches expected size

## NTFS Structures

### Boot Sector
Contains essential file system parameters:
- Bytes per sector
- Sectors per cluster
- MFT cluster location
- Total sectors

### Master File Table (MFT)
- Database of all files and directories
- Each entry is 1024 bytes (typically)
- Entry 0: $MFT itself
- Entries marked as "not in use" represent deleted files

### MFT Entry Structure
```
[Header (FILE)]
  - Signature: "FILE"
  - Flags: inUse, isDirectory
  - First attribute offset
[Attributes]
  - $STANDARD_INFORMATION (0x10)
  - $FILE_NAME (0x30)
  - $DATA (0x80)
[End marker (0xFFFFFFFF)]
```

### Data Runs (Runlist)
For non-resident files, data runs specify:
- Cluster offset (LCN - Logical Cluster Number)
- Cluster count (length)
- Encoded as variable-length values

## Build Instructions

### Prerequisites
- Windows operating system
- CMake 3.10 or higher
- MSVC compiler (Visual Studio) or MinGW

### Building with CMake

```bash
# Create build directory
mkdir build
cd build

# Generate build files
cmake ..

# Build
cmake --build . --config Release

# Executable will be in build/bin/
```

### Building with Visual Studio
```bash
# Generate Visual Studio solution
cmake -G "Visual Studio 16 2019" ..

# Open DataRecoveryTool.sln in Visual Studio
# Build solution (Ctrl+Shift+B)
```

## Usage

### Running the Tool
1. **Run as Administrator** (required for raw disk access)
2. Launch `DataRecoveryTool.exe`
3. Follow menu options:

### Menu Options

#### 1. List Available Volumes
Displays all drives on the system with:
- Drive letter
- Drive type (Fixed/Removable/etc.)
- File system
- Total/free space

#### 2. Select Volume and Scan
- Enter drive letter (e.g., C)
- Specify max MFT entries to scan (e.g., 10000)
- Tool will:
  - Open volume in read-only mode
  - Parse NTFS boot sector
  - Scan MFT for deleted files
  - Display results

#### 3. View Deleted Files List
Shows all found deleted files with:
- Index number
- MFT entry number
- File name
- File size
- Number of data runs

#### 4. Recover File
- Enter index of file to recover
- Specify output path (on different drive!)
- Tool will attempt recovery and report status

#### 5. Exit
Closes volume and exits

## Safety Features

### Read-Only Operation
- Tool only reads from source volume
- Never writes to source volume
- Minimizes risk of data corruption

### Data Integrity
- Verifies NTFS signatures
- Checks boot sector validity
- Validates MFT entry headers
- Reports partial recoveries

## Limitations

### Known Limitations
1. **NTFS Only**: Currently supports only NTFS file systems
2. **Overwritten Data**: Cannot recover files that have been overwritten
3. **Fragmented Files**: Complex fragmentation may cause issues
4. **Encryption**: Cannot recover encrypted files without keys
5. **Compressed Files**: Limited support for compressed files
6. **Large Files**: Very large files may require significant memory

### Future Enhancements
- FAT32/exFAT support
- File carving (signature-based recovery)
- Graphical user interface (GUI)
- Logging system
- Recovery success prediction
- Multi-threaded scanning
- Better progress reporting

## Error Handling

### Common Errors

**"Failed to open volume"**
- Solution: Run as Administrator
- Cause: Insufficient privileges for raw disk access

**"Not a valid NTFS boot sector"**
- Solution: Verify drive is NTFS formatted
- Cause: Wrong file system or corrupted boot sector

**"Failed to read sectors"**
- Solution: Check disk health
- Cause: Physical disk errors or bad sectors

**"Incomplete read"**
- Solution: Try different sector range
- Cause: Disk read errors

## Advanced Usage

### Scanning Specific MFT Range
Modify `maxEntries` parameter to scan more or fewer entries:
- Small drives: 10,000 - 50,000 entries
- Large drives: 100,000+ entries
- Note: More entries = longer scan time

### Output Path Selection
**Always save to different drive:**
- Source: C:\
- Output: D:\ or E:\ or network drive
- Reason: Prevents overwriting deleted data

### Batch Recovery
To recover multiple files:
1. Scan once
2. Note file indices
3. Recover files one by one

## Technical Details

### Sector and Cluster Calculation
```
bytesPerCluster = bytesPerSector * sectorsPerCluster
mftStartSector = mftCluster * sectorsPerCluster
entryStartSector = mftStartSector + (entryIndex * entrySize / bytesPerSector)
```

### Runlist Parsing
```
Each run encoded as: [header][length][offset]
- header: 4 bits length size, 4 bits offset size
- length: variable-length unsigned int
- offset: variable-length signed int (relative to previous LCN)
```

### Memory Considerations
- MFT entries: ~1 KB each
- Sector buffers: Aligned to sector size
- Large files: Read in chunks (256 sectors = 128 KB)

## References

- Microsoft NTFS Documentation
- Windows Driver Kit (WDK) documentation
- "Windows Internals" by Mark Russinovich
- NTFS.com technical specifications

## License
This tool is provided as-is for educational and data recovery purposes.

## Disclaimer
- Use at your own risk
- No guarantee of successful recovery
- Stop using drive immediately after file deletion
- Consult professional data recovery services for critical data
