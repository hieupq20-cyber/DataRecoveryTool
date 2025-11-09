#include "../include/DiskEnumerator.h"
#include <iostream>
#include <iomanip>

std::vector<VolumeInfo> DiskEnumerator::EnumerateVolumes() {
    std::vector<VolumeInfo> volumes;
    
    // Get all logical drives
    DWORD driveMask = GetLogicalDrives();
    
    for (int i = 0; i < 26; i++) {
        if (driveMask & (1 << i)) {
            wchar_t driveLetter[4] = { L'A' + i, L':', L'\\', L'\0' };
            
            VolumeInfo info;
            info.driveLetter = driveLetter;
            info.driveType = GetDriveTypeW(driveLetter);
            info.driveTypeStr = GetDriveTypeString(info.driveType);
            
            // Get volume information
            wchar_t volumeName[MAX_PATH + 1] = { 0 };
            wchar_t fileSystem[MAX_PATH + 1] = { 0 };
            DWORD serialNumber = 0;
            DWORD maxComponentLen = 0;
            DWORD fileSystemFlags = 0;
            
            if (GetVolumeInformationW(driveLetter, volumeName, MAX_PATH,
                                      &serialNumber, &maxComponentLen,
                                      &fileSystemFlags, fileSystem, MAX_PATH)) {
                info.volumeName = volumeName;
                info.fileSystem = fileSystem;
            }
            
            // Get disk space information
            ULARGE_INTEGER freeBytesAvailable, totalNumberOfBytes, totalNumberOfFreeBytes;
            if (GetDiskFreeSpaceExW(driveLetter, &freeBytesAvailable,
                                   &totalNumberOfBytes, &totalNumberOfFreeBytes)) {
                info.totalSpace = totalNumberOfBytes.QuadPart;
                info.freeSpace = totalNumberOfFreeBytes.QuadPart;
            }
            
            volumes.push_back(info);
        }
    }
    
    return volumes;
}

std::wstring DiskEnumerator::GetDriveTypeString(UINT driveType) {
    switch (driveType) {
        case DRIVE_UNKNOWN:
            return L"Unknown";
        case DRIVE_NO_ROOT_DIR:
            return L"No Root Dir";
        case DRIVE_REMOVABLE:
            return L"Removable";
        case DRIVE_FIXED:
            return L"Fixed";
        case DRIVE_REMOTE:
            return L"Network";
        case DRIVE_CDROM:
            return L"CD-ROM";
        case DRIVE_RAMDISK:
            return L"RAM Disk";
        default:
            return L"Unknown";
    }
}

void DiskEnumerator::PrintVolumeInfo(const VolumeInfo& info) {
    std::wcout << L"Drive: " << info.driveLetter << std::endl;
    std::wcout << L"  Type: " << info.driveTypeStr << std::endl;
    std::wcout << L"  File System: " << info.fileSystem << std::endl;
    std::wcout << L"  Volume Name: " << info.volumeName << std::endl;
    std::wcout << L"  Total Space: " << info.totalSpace / (1024 * 1024 * 1024) << L" GB" << std::endl;
    std::wcout << L"  Free Space: " << info.freeSpace / (1024 * 1024 * 1024) << L" GB" << std::endl;
    std::wcout << std::endl;
}

void DiskEnumerator::PrintAllVolumes() {
    std::vector<VolumeInfo> volumes = EnumerateVolumes();
    
    std::wcout << L"=== Available Volumes ===" << std::endl << std::endl;
    
    for (const auto& volume : volumes) {
        PrintVolumeInfo(volume);
    }
}
