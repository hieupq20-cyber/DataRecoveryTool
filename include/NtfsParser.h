#ifndef NTFS_PARSER_H
#define NTFS_PARSER_H

#include "NtfsStructures.h"
#include "RawDiskReader.h"
#include <vector>
#include <string>

class NtfsParser {
private:
    NtfsBootSector bootSector;
    RawDiskReader* diskReader;
    uint32_t bytesPerCluster;
    uint64_t mftStartSector;

public:
    NtfsParser();
    
    bool ParseBootSector(const std::vector<uint8_t>& buffer);
    bool Initialize(RawDiskReader* reader);
    
    bool ReadMftEntry(uint64_t entryIndex, std::vector<uint8_t>& buffer);
    bool ParseMftEntry(const std::vector<uint8_t>& buffer, MftEntryHeader& header);
    
    bool ExtractFileName(const std::vector<uint8_t>& entryBuffer, std::wstring& fileName);
    bool ExtractFileSize(const std::vector<uint8_t>& entryBuffer, uint64_t& fileSize);
    bool ExtractDataRuns(const std::vector<uint8_t>& entryBuffer, std::vector<DataRun>& dataRuns);
    
    const NtfsBootSector& GetBootSector() const { return bootSector; }
    uint32_t GetBytesPerCluster() const { return bytesPerCluster; }
    uint64_t GetMftStartSector() const { return mftStartSector; }
    
    static void PrintBootSectorInfo(const NtfsBootSector& bs);
};

#endif // NTFS_PARSER_H
