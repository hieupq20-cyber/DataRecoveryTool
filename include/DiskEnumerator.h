#ifndef DISK_ENUMERATOR_H
#define DISK_ENUMERATOR_H

#include <string>
#include <vector>
#include <windows.h>

struct VolumeInfo {
    std::wstring driveLetter;
    std::wstring volumeName;
    std::wstring fileSystem;
    UINT driveType;
    std::wstring driveTypeStr;
    ULONGLONG totalSpace;
    ULONGLONG freeSpace;
};

class DiskEnumerator {
public:
    static std::vector<VolumeInfo> EnumerateVolumes();
    static std::wstring GetDriveTypeString(UINT driveType);
    static void PrintVolumeInfo(const VolumeInfo& info);
    static void PrintAllVolumes();
};

#endif // DISK_ENUMERATOR_H
