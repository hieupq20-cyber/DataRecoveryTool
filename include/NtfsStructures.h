#ifndef NTFS_STRUCTURES_H
#define NTFS_STRUCTURES_H

#include <cstdint>
#include <vector>

#pragma pack(push, 1)

struct NtfsBootSector {
    uint8_t jumpBoot[3];
    char oemId[8];
    uint16_t bytesPerSector;
    uint8_t sectorsPerCluster;
    uint16_t reservedSectors;
    uint8_t zeros1[3];
    uint16_t unused1;
    uint8_t mediaDescriptor;
    uint16_t zeros2;
    uint16_t sectorsPerTrack;
    uint16_t numberOfHeads;
    uint32_t hiddenSectors;
    uint32_t unused2;
    uint32_t unused3;
    uint64_t totalSectors;
    uint64_t mftCluster;
    uint64_t mftMirrorCluster;
    int8_t clustersPerMftRecord;
    uint8_t unused4[3];
    int8_t clustersPerIndexBuffer;
    uint8_t unused5[3];
    uint64_t volumeSerialNumber;
    uint32_t checksum;
    uint8_t bootCode[426];
    uint16_t endMarker;
};

struct MftEntryHeader {
    char signature[4];  // "FILE"
    uint16_t updateSequenceOffset;
    uint16_t updateSequenceSize;
    uint64_t logFileSequenceNumber;
    uint16_t sequenceNumber;
    uint16_t hardLinkCount;
    uint16_t firstAttributeOffset;
    uint16_t flags;  // 0x01 = in use, 0x02 = directory
    uint32_t usedSize;
    uint32_t allocatedSize;
    uint64_t baseRecordReference;
    uint16_t nextAttributeId;
    uint16_t unused;
    uint32_t mftRecordNumber;
};

struct NtfsAttributeHeader {
    uint32_t attributeType;
    uint32_t length;
    uint8_t nonResident;
    uint8_t nameLength;
    uint16_t nameOffset;
    uint16_t flags;
    uint16_t attributeId;
};

struct ResidentAttributeHeader {
    NtfsAttributeHeader common;
    uint32_t valueLength;
    uint16_t valueOffset;
    uint8_t indexedFlag;
    uint8_t padding;
};

struct NonResidentAttributeHeader {
    NtfsAttributeHeader common;
    uint64_t startVcn;
    uint64_t endVcn;
    uint16_t runListOffset;
    uint16_t compressionUnit;
    uint32_t padding;
    uint64_t allocatedSize;
    uint64_t realSize;
    uint64_t initializedSize;
};

#pragma pack(pop)

// Attribute types
enum NtfsAttributeType {
    ATTR_STANDARD_INFORMATION = 0x10,
    ATTR_FILE_NAME = 0x30,
    ATTR_DATA = 0x80,
    ATTR_INDEX_ROOT = 0x90,
    ATTR_INDEX_ALLOCATION = 0xA0,
    ATTR_BITMAP = 0xB0,
    ATTR_END = 0xFFFFFFFF
};

// MFT entry flags
enum MftEntryFlags {
    MFT_RECORD_IN_USE = 0x01,
    MFT_RECORD_IS_DIRECTORY = 0x02
};

struct DataRun {
    uint64_t clusterOffset;
    uint64_t clusterCount;
};

struct DeletedFileInfo {
    uint64_t mftIndex;
    std::wstring fileName;
    uint64_t fileSize;
    bool isDirectory;
    std::vector<DataRun> dataRuns;
    uint64_t creationTime;
    uint64_t modificationTime;
};

#endif // NTFS_STRUCTURES_H
