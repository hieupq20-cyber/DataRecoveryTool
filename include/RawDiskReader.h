#ifndef RAW_DISK_READER_H
#define RAW_DISK_READER_H

#include <windows.h>
#include <vector>
#include <string>
#include <cstdint>

class RawDiskReader {
private:
    HANDLE hVolume;
    std::wstring volumePath;
    uint32_t bytesPerSector;

public:
    RawDiskReader();
    ~RawDiskReader();

    bool OpenVolume(const std::wstring& driveLetter);
    void CloseVolume();
    bool IsOpen() const;

    bool ReadSectors(uint64_t startSector, uint32_t sectorCount, std::vector<uint8_t>& buffer);
    bool ReadBootSector(std::vector<uint8_t>& buffer);
    
    uint32_t GetBytesPerSector() const { return bytesPerSector; }
};

#endif // RAW_DISK_READER_H
