#include "DiskEnumerator.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <stdexcept>
#include <cwchar>

namespace drt {

namespace {

#ifdef _WIN32
std::wstring driveTypeToString(UINT type) {
    switch (type) {
    case DRIVE_UNKNOWN:
        return L"Unknown";
    case DRIVE_NO_ROOT_DIR:
        return L"Invalid";
    case DRIVE_REMOVABLE:
        return L"Removable";
    case DRIVE_FIXED:
        return L"Fixed";
    case DRIVE_REMOTE:
        return L"Network";
    case DRIVE_CDROM:
        return L"CD/DVD";
    case DRIVE_RAMDISK:
        return L"RAM Disk";
    default:
        return L"Other";
    }
}
#endif

} // namespace

std::vector<DiskInfo> EnumerateVolumes() {
    std::vector<DiskInfo> result;

#ifdef _WIN32
    DWORD length = GetLogicalDriveStringsW(0, nullptr);
    if (length == 0) {
        return result;
    }

    std::wstring buffer(length, L'\0');
    DWORD written = GetLogicalDriveStringsW(length, buffer.data());
    if (written == 0) {
        return result;
    }

    for (wchar_t *drive = buffer.data(); *drive != L'\0'; drive += wcslen(drive) + 1) {
        DiskInfo info{};
        info.rootPath = drive;
        info.driveType = driveTypeToString(GetDriveTypeW(drive));

        wchar_t fsNameBuffer[MAX_PATH] = {0};
        if (GetVolumeInformationW(drive, nullptr, 0, nullptr, nullptr, nullptr, fsNameBuffer, MAX_PATH)) {
            info.fileSystem = fsNameBuffer;
        }

        result.push_back(std::move(info));
    }
#else
    throw std::runtime_error("Volume enumeration is only supported on Windows.");
#endif

    return result;
}

} // namespace drt
