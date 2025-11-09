#include "../include/RawDiskReader.h"
#include <iostream>

RawDiskReader::RawDiskReader() : hVolume(INVALID_HANDLE_VALUE), bytesPerSector(512) {
}

RawDiskReader::~RawDiskReader() {
    CloseVolume();
}

bool RawDiskReader::OpenVolume(const std::wstring& driveLetter) {
    CloseVolume();
    
    // Format: \\.\C:
    volumePath = L"\\\\.\\";
    if (driveLetter.length() >= 2 && driveLetter[1] == L':') {
        volumePath += driveLetter.substr(0, 2);
    } else {
        volumePath += driveLetter;
        volumePath += L":";
    }
    
    // Open volume with read-only access
    hVolume = CreateFileW(
        volumePath.c_str(),
        GENERIC_READ,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_NO_BUFFERING,
        NULL
    );
    
    if (hVolume == INVALID_HANDLE_VALUE) {
        std::wcerr << L"Failed to open volume: " << volumePath << std::endl;
        std::wcerr << L"Error code: " << GetLastError() << std::endl;
        std::wcerr << L"Note: Administrator privileges may be required." << std::endl;
        return false;
    }
    
    // Get disk geometry to determine sector size
    DISK_GEOMETRY diskGeometry;
    DWORD bytesReturned;
    if (DeviceIoControl(hVolume, IOCTL_DISK_GET_DRIVE_GEOMETRY, NULL, 0,
                       &diskGeometry, sizeof(diskGeometry), &bytesReturned, NULL)) {
        bytesPerSector = diskGeometry.BytesPerSector;
    }
    
    return true;
}

void RawDiskReader::CloseVolume() {
    if (hVolume != INVALID_HANDLE_VALUE) {
        CloseHandle(hVolume);
        hVolume = INVALID_HANDLE_VALUE;
    }
}

bool RawDiskReader::IsOpen() const {
    return hVolume != INVALID_HANDLE_VALUE;
}

bool RawDiskReader::ReadSectors(uint64_t startSector, uint32_t sectorCount, std::vector<uint8_t>& buffer) {
    if (!IsOpen()) {
        std::wcerr << L"Volume is not open" << std::endl;
        return false;
    }
    
    uint64_t offset = startSector * bytesPerSector;
    uint32_t bytesToRead = sectorCount * bytesPerSector;
    
    // Allocate buffer with proper alignment for FILE_FLAG_NO_BUFFERING
    buffer.resize(bytesToRead);
    
    // Set file pointer
    LARGE_INTEGER li;
    li.QuadPart = offset;
    if (!SetFilePointerEx(hVolume, li, NULL, FILE_BEGIN)) {
        std::wcerr << L"Failed to set file pointer. Error: " << GetLastError() << std::endl;
        return false;
    }
    
    // Read data
    DWORD bytesRead = 0;
    if (!ReadFile(hVolume, buffer.data(), bytesToRead, &bytesRead, NULL)) {
        std::wcerr << L"Failed to read sectors. Error: " << GetLastError() << std::endl;
        return false;
    }
    
    if (bytesRead != bytesToRead) {
        std::wcerr << L"Incomplete read: " << bytesRead << L" / " << bytesToRead << std::endl;
        return false;
    }
    
    return true;
}

bool RawDiskReader::ReadBootSector(std::vector<uint8_t>& buffer) {
    return ReadSectors(0, 1, buffer);
}
