# Data Recovery Tool - User Guide

## Introduction
Welcome to the Data Recovery Tool! This utility helps you recover deleted files from NTFS-formatted drives on Windows systems.

## ⚠️ IMPORTANT WARNINGS

### Before You Start
1. **Stop Using the Drive Immediately**
   - When you delete a file, Windows marks it as deleted but doesn't erase the data
   - Using the drive may overwrite the deleted file's data
   - The sooner you attempt recovery, the better the chances

2. **Run as Administrator**
   - This tool requires administrator privileges to access raw disk data
   - Right-click the executable and select "Run as Administrator"

3. **Save to Different Drive**
   - NEVER save recovered files to the same drive
   - Use a different drive letter (if recovering from C:, save to D: or E:)
   - This prevents overwriting the deleted data you're trying to recover

4. **No Guarantees**
   - File recovery is not guaranteed
   - If data has been overwritten, it cannot be recovered
   - Some files may be only partially recovered

## System Requirements
- Windows operating system (Windows 7 or later)
- NTFS-formatted drives
- Administrator privileges
- At least one additional drive for saving recovered files

## Quick Start Guide

### Step 1: Launch the Tool
1. Right-click `DataRecoveryTool.exe`
2. Select "Run as administrator"
3. Click "Yes" on the UAC prompt

### Step 2: List Available Drives
1. Select option `1` from the main menu
2. Review the list of available drives
3. Note the drive letter where your deleted files were located

### Step 3: Scan for Deleted Files
1. Select option `2` from the main menu
2. Enter the drive letter (e.g., `C`)
3. Enter the number of MFT entries to scan:
   - For quick scan: 10,000
   - For thorough scan: 50,000 or more
   - Larger numbers take longer but find more files
4. Wait for the scan to complete
5. Review the list of deleted files found

### Step 4: Review Deleted Files
1. Select option `3` to view the deleted files list
2. Note the index number of files you want to recover
3. Check the file size to ensure it's correct

### Step 5: Recover Files
1. Select option `4` from the main menu
2. Enter the index number of the file to recover
3. Enter the full output path, including filename:
   - Example: `D:\Recovered\document.docx`
   - Make sure the directory exists
   - Use a drive different from the source
4. Wait for recovery to complete
5. Check the recovery status message

### Step 6: Exit
1. Select option `5` to exit the program
2. Verify your recovered files

## Detailed Menu Options

### Option 1: List Available Volumes
**What it does:** Shows all drives on your system

**Output includes:**
- Drive letter (C:, D:, etc.)
- Drive type (Fixed, Removable, Network, etc.)
- File system (NTFS, FAT32, etc.)
- Volume name
- Total space
- Free space

**When to use:** Before scanning, to identify which drive to scan

### Option 2: Select Volume and Scan
**What it does:** Scans a drive's Master File Table for deleted files

**Steps:**
1. Enter drive letter when prompted
2. Enter maximum entries to scan
3. Tool opens the volume
4. Parses NTFS boot sector
5. Scans MFT entries
6. Displays found deleted files

**Scanning tips:**
- More entries = longer scan time
- Typical ranges:
  - Small USB drive: 5,000 - 10,000
  - System drive (C:): 50,000 - 100,000
  - Large data drive: 100,000+

### Option 3: View Deleted Files List
**What it does:** Shows previously scanned deleted files

**Information shown:**
- Index: Number to use for recovery
- MFT#: Master File Table entry number
- File Name: Name of the deleted file
- Size: File size (B, KB, MB, or GB)
- Data Runs: Number of disk fragments

**Tips:**
- Files with 0 data runs may not be recoverable
- Very large files may have many data runs

### Option 4: Recover File by Index
**What it does:** Attempts to recover a specific deleted file

**Process:**
1. Enter index from the deleted files list
2. Specify output path (must be on different drive)
3. Tool reads file data from disk
4. Writes to output file
5. Reports success or failure

**Recovery Status Messages:**
- **Success**: File fully recovered
- **Partial recovery**: Some data recovered, file may be incomplete
- **Failed to read data**: Disk read error
- **Invalid data run**: File structure corrupted
- **Failed to write file**: Output location issue

### Option 5: Exit
Closes the volume and exits the program

## Understanding Results

### Successful Recovery
```
Recovered 1048576 / 1048576 bytes
File recovered successfully!
Recovery status: Success
```
✓ File fully recovered and should be usable

### Partial Recovery
```
Recovered 524288 / 1048576 bytes
Warning: File was only partially recovered
Recovery status: Partial recovery
```
⚠️ File partially recovered - may be corrupted or incomplete

### Failed Recovery
```
Failed to read sectors at 12345
Recovery status: Failed to read data
```
✗ Recovery failed - data may be overwritten or disk error

## Best Practices

### For Best Results
1. **Act Quickly**: Recover files as soon as possible after deletion
2. **Stop Using Drive**: Don't install programs, save files, or browse the drive
3. **One Drive**: Don't recover files to the same drive
4. **Multiple Attempts**: If recovery fails, try scanning with higher entry count
5. **Verify Files**: After recovery, check that files open correctly

### When to Expect Success
- File was recently deleted (same day)
- Drive has not been used much since deletion
- Drive is not nearly full
- File is not highly fragmented

### When Recovery May Fail
- File deleted long ago
- Drive has been used extensively
- Drive was defragmented after deletion
- File was securely deleted (using secure delete tools)
- File was encrypted

## Troubleshooting

### "Failed to open volume"
**Problem**: Cannot access the drive

**Solutions:**
- Run as Administrator
- Close other programs accessing the drive
- Ensure drive letter is correct
- Check if drive is connected (for external drives)

### "Not a valid NTFS boot sector"
**Problem**: Drive is not NTFS or is corrupted

**Solutions:**
- Verify drive is NTFS (use Option 1)
- Try different drive
- Check disk for errors using Windows CHKDSK

### "No deleted files found"
**Problem**: Scan found no recoverable files

**Possible Reasons:**
- Files have been overwritten
- Not enough entries scanned (try higher number)
- Files were on different drive
- Volume is not where files were deleted

### "Failed to create output file"
**Problem**: Cannot write to output location

**Solutions:**
- Check output path is correct
- Ensure directory exists
- Verify sufficient disk space
- Check write permissions
- Don't use the same drive as source

### Recovery Shows 0 Bytes
**Problem**: File recovered but empty

**Possible Reasons:**
- Data has been overwritten
- File had no actual data (was empty)
- Data runs are invalid

## Example Session

```
========================================
   Data Recovery Tool v1.0
   NTFS File Recovery Utility
========================================

=== Main Menu ===
1. List available volumes
2. Select volume and scan for deleted files
3. View deleted files list
4. Recover file by index
5. Exit
Enter choice: 2

Enter drive letter (e.g., C): C
Enter maximum MFT entries to scan (e.g., 10000): 10000

Scanning for deleted files (max 10000 entries)...
Scanned 10000 entries, found 15 deleted files...

=== Deleted Files ===
Index | MFT# | File Name              | Size     | Data Runs
------+------+------------------------+----------+-----------
    0 | 5234 | document.docx          |   45 KB  | 1 runs
    1 | 5892 | photo.jpg              |  2.3 MB  | 1 runs
    2 | 6104 | presentation.pptx      |  8.7 MB  | 3 runs
...

Enter choice: 4

Enter file index to recover (0-14): 0
Enter output file path: D:\Recovered\document.docx

Recovering file: document.docx
Output path: D:\Recovered\document.docx
Processing run: cluster 16384, count 96
Recovered 46080 / 46080 bytes
File recovered successfully!
Recovery status: Success
```

## FAQs

**Q: Can I recover files from formatted drives?**
A: Formatting marks all space as free, making recovery much harder. Quick format may allow some recovery.

**Q: How long does scanning take?**
A: Depends on entries scanned. ~1 second per 1000 entries typically.

**Q: Can I recover files from external drives?**
A: Yes, if they use NTFS file system.

**Q: What file types can be recovered?**
A: Any file type stored on NTFS can potentially be recovered.

**Q: Will this work on network drives?**
A: No, this tool requires local physical drive access.

**Q: Can I recover files from SSD?**
A: Recovery is harder on SSDs due to TRIM command which actually erases deleted data.

## Getting Help

If you encounter issues:
1. Check the troubleshooting section
2. Review the technical documentation
3. Ensure you're following all safety warnings
4. Consider professional data recovery services for critical data

## Remember

🔹 **Prevention is better than recovery**
- Maintain regular backups
- Use cloud storage for important files
- Enable File History in Windows
- Consider version control for documents

🔹 **Recovery is not guaranteed**
- This is a best-effort tool
- For critical data, consult professional services
- Some data loss may be permanent

🔹 **Use responsibly**
- Only recover your own files
- Respect privacy and legal boundaries
- Read-only operation minimizes risks but use carefully

---

Good luck with your file recovery!
