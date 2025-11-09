#include "../include/NtfsParser.h"
#include <iostream>
#include <cstring>

NtfsParser::NtfsParser() : diskReader(nullptr), bytesPerCluster(0), mftStartSector(0) {
}

bool NtfsParser::ParseBootSector(const std::vector<uint8_t>& buffer) {
    if (buffer.size() < sizeof(NtfsBootSector)) {
        std::wcerr << L"Buffer too small for boot sector" << std::endl;
        return false;
    }
    
    memcpy(&bootSector, buffer.data(), sizeof(NtfsBootSector));
    
    // Verify NTFS signature
    if (memcmp(bootSector.oemId, "NTFS    ", 8) != 0) {
        std::wcerr << L"Not a valid NTFS boot sector (OEM ID mismatch)" << std::endl;
        return false;
    }
    
    // Verify end marker
    if (bootSector.endMarker != 0xAA55) {
        std::wcerr << L"Invalid boot sector end marker" << std::endl;
        return false;
    }
    
    bytesPerCluster = bootSector.bytesPerSector * bootSector.sectorsPerCluster;
    mftStartSector = bootSector.mftCluster * bootSector.sectorsPerCluster;
    
    return true;
}

bool NtfsParser::Initialize(RawDiskReader* reader) {
    diskReader = reader;
    
    if (!diskReader || !diskReader->IsOpen()) {
        std::wcerr << L"Disk reader is not open" << std::endl;
        return false;
    }
    
    std::vector<uint8_t> bootBuffer;
    if (!diskReader->ReadBootSector(bootBuffer)) {
        std::wcerr << L"Failed to read boot sector" << std::endl;
        return false;
    }
    
    if (!ParseBootSector(bootBuffer)) {
        std::wcerr << L"Failed to parse boot sector" << std::endl;
        return false;
    }
    
    return true;
}

bool NtfsParser::ReadMftEntry(uint64_t entryIndex, std::vector<uint8_t>& buffer) {
    if (!diskReader || !diskReader->IsOpen()) {
        return false;
    }
    
    // Calculate MFT entry size
    uint32_t mftEntrySize = 1024; // Default is 1024 bytes
    if (bootSector.clustersPerMftRecord >= 0) {
        mftEntrySize = bootSector.clustersPerMftRecord * bytesPerCluster;
    } else {
        mftEntrySize = 1 << (-bootSector.clustersPerMftRecord);
    }
    
    // Calculate sector offset
    uint64_t entryOffsetBytes = entryIndex * mftEntrySize;
    uint64_t entryStartSector = mftStartSector + (entryOffsetBytes / diskReader->GetBytesPerSector());
    uint32_t sectorCount = (mftEntrySize + diskReader->GetBytesPerSector() - 1) / diskReader->GetBytesPerSector();
    
    return diskReader->ReadSectors(entryStartSector, sectorCount, buffer);
}

bool NtfsParser::ParseMftEntry(const std::vector<uint8_t>& buffer, MftEntryHeader& header) {
    if (buffer.size() < sizeof(MftEntryHeader)) {
        return false;
    }
    
    memcpy(&header, buffer.data(), sizeof(MftEntryHeader));
    
    // Verify FILE signature
    if (memcmp(header.signature, "FILE", 4) != 0) {
        return false;
    }
    
    return true;
}

bool NtfsParser::ExtractFileName(const std::vector<uint8_t>& entryBuffer, std::wstring& fileName) {
    MftEntryHeader header;
    if (!ParseMftEntry(entryBuffer, header)) {
        return false;
    }
    
    uint32_t offset = header.firstAttributeOffset;
    
    while (offset + sizeof(NtfsAttributeHeader) < entryBuffer.size()) {
        NtfsAttributeHeader* attrHeader = (NtfsAttributeHeader*)(entryBuffer.data() + offset);
        
        if (attrHeader->attributeType == ATTR_END || attrHeader->attributeType == 0) {
            break;
        }
        
        if (attrHeader->attributeType == ATTR_FILE_NAME) {
            // FILE_NAME attribute structure
            if (attrHeader->nonResident == 0) {
                ResidentAttributeHeader* resHeader = (ResidentAttributeHeader*)(entryBuffer.data() + offset);
                uint32_t valueOffset = offset + resHeader->valueOffset;
                
                if (valueOffset + 66 < entryBuffer.size()) {
                    // FILE_NAME structure: parent (8), timestamps (32), sizes (16), flags (8), nameLen (1), namespace (1), name
                    uint8_t nameLength = entryBuffer[valueOffset + 64];
                    uint8_t* nameData = (uint8_t*)(entryBuffer.data() + valueOffset + 66);
                    
                    if (valueOffset + 66 + nameLength * 2 <= entryBuffer.size()) {
                        fileName = std::wstring((wchar_t*)nameData, nameLength);
                        return true;
                    }
                }
            }
        }
        
        offset += attrHeader->length;
    }
    
    return false;
}

bool NtfsParser::ExtractFileSize(const std::vector<uint8_t>& entryBuffer, uint64_t& fileSize) {
    MftEntryHeader header;
    if (!ParseMftEntry(entryBuffer, header)) {
        return false;
    }
    
    uint32_t offset = header.firstAttributeOffset;
    
    while (offset + sizeof(NtfsAttributeHeader) < entryBuffer.size()) {
        NtfsAttributeHeader* attrHeader = (NtfsAttributeHeader*)(entryBuffer.data() + offset);
        
        if (attrHeader->attributeType == ATTR_END || attrHeader->attributeType == 0) {
            break;
        }
        
        if (attrHeader->attributeType == ATTR_DATA) {
            if (attrHeader->nonResident == 0) {
                ResidentAttributeHeader* resHeader = (ResidentAttributeHeader*)(entryBuffer.data() + offset);
                fileSize = resHeader->valueLength;
                return true;
            } else {
                NonResidentAttributeHeader* nonResHeader = (NonResidentAttributeHeader*)(entryBuffer.data() + offset);
                fileSize = nonResHeader->realSize;
                return true;
            }
        }
        
        offset += attrHeader->length;
    }
    
    return false;
}

bool NtfsParser::ExtractDataRuns(const std::vector<uint8_t>& entryBuffer, std::vector<DataRun>& dataRuns) {
    MftEntryHeader header;
    if (!ParseMftEntry(entryBuffer, header)) {
        return false;
    }
    
    uint32_t offset = header.firstAttributeOffset;
    
    while (offset + sizeof(NtfsAttributeHeader) < entryBuffer.size()) {
        NtfsAttributeHeader* attrHeader = (NtfsAttributeHeader*)(entryBuffer.data() + offset);
        
        if (attrHeader->attributeType == ATTR_END || attrHeader->attributeType == 0) {
            break;
        }
        
        if (attrHeader->attributeType == ATTR_DATA && attrHeader->nonResident == 1) {
            NonResidentAttributeHeader* nonResHeader = (NonResidentAttributeHeader*)(entryBuffer.data() + offset);
            uint32_t runListOffset = offset + nonResHeader->runListOffset;
            
            // Parse runlist
            int64_t currentLcn = 0;
            uint32_t pos = runListOffset;
            
            while (pos < entryBuffer.size()) {
                uint8_t header = entryBuffer[pos];
                if (header == 0) break;
                
                uint8_t lengthBytes = header & 0x0F;
                uint8_t offsetBytes = (header >> 4) & 0x0F;
                pos++;
                
                if (lengthBytes == 0 || pos + lengthBytes + offsetBytes > entryBuffer.size()) {
                    break;
                }
                
                // Read length
                uint64_t length = 0;
                for (int i = 0; i < lengthBytes; i++) {
                    length |= ((uint64_t)entryBuffer[pos + i]) << (i * 8);
                }
                pos += lengthBytes;
                
                // Read offset (signed)
                int64_t offset = 0;
                for (int i = 0; i < offsetBytes; i++) {
                    offset |= ((int64_t)entryBuffer[pos + i]) << (i * 8);
                }
                // Sign extend if necessary
                if (offsetBytes > 0 && (entryBuffer[pos + offsetBytes - 1] & 0x80)) {
                    for (int i = offsetBytes; i < 8; i++) {
                        offset |= ((int64_t)0xFF) << (i * 8);
                    }
                }
                pos += offsetBytes;
                
                currentLcn += offset;
                
                DataRun run;
                run.clusterOffset = currentLcn;
                run.clusterCount = length;
                dataRuns.push_back(run);
            }
            
            return true;
        }
        
        offset += attrHeader->length;
    }
    
    return false;
}

void NtfsParser::PrintBootSectorInfo(const NtfsBootSector& bs) {
    std::wcout << L"=== NTFS Boot Sector Information ===" << std::endl;
    std::cout << "OEM ID: ";
    for (int i = 0; i < 8; i++) {
        std::cout << bs.oemId[i];
    }
    std::cout << std::endl;
    std::wcout << L"Bytes per Sector: " << bs.bytesPerSector << std::endl;
    std::wcout << L"Sectors per Cluster: " << (int)bs.sectorsPerCluster << std::endl;
    std::wcout << L"Total Sectors: " << bs.totalSectors << std::endl;
    std::wcout << L"MFT Cluster: " << bs.mftCluster << std::endl;
    std::wcout << L"MFT Mirror Cluster: " << bs.mftMirrorCluster << std::endl;
    std::wcout << L"Clusters per MFT Record: " << (int)bs.clustersPerMftRecord << std::endl;
    std::wcout << std::endl;
}
