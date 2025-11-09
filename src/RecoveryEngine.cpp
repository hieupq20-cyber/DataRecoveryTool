#include "../include/RecoveryEngine.h"
#include <iostream>
#include <fstream>
#include <vector>

RecoveryEngine::RecoveryEngine() : ntfsParser(nullptr), diskReader(nullptr) {
}

bool RecoveryEngine::Initialize(NtfsParser* parser, RawDiskReader* reader) {
    ntfsParser = parser;
    diskReader = reader;
    
    if (!ntfsParser || !diskReader) {
        std::wcerr << L"Invalid parser or reader" << std::endl;
        return false;
    }
    
    return true;
}

RecoveryStatus RecoveryEngine::RecoverFile(
    const DeletedFileInfo& fileInfo,
    const std::wstring& outputPath
) {
    if (!ntfsParser || !diskReader) {
        return RecoveryStatus::FailedRead;
    }
    
    if (outputPath.empty()) {
        return RecoveryStatus::InvalidPath;
    }
    
    if (fileInfo.dataRuns.empty()) {
        std::wcerr << L"No data runs available for recovery" << std::endl;
        return RecoveryStatus::InvalidDataRun;
    }
    
    std::wcout << L"Recovering file: " << fileInfo.fileName << std::endl;
    std::wcout << L"Output path: " << outputPath << std::endl;
    
    // Open output file
    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) {
        std::wcerr << L"Failed to create output file" << std::endl;
        return RecoveryStatus::FailedWrite;
    }
    
    uint64_t totalBytesRecovered = 0;
    uint32_t bytesPerCluster = ntfsParser->GetBytesPerCluster();
    uint32_t bytesPerSector = diskReader->GetBytesPerSector();
    uint32_t sectorsPerCluster = bytesPerCluster / bytesPerSector;
    
    // Process each data run
    for (const auto& run : fileInfo.dataRuns) {
        std::wcout << L"Processing run: cluster " << run.clusterOffset 
                  << L", count " << run.clusterCount << std::endl;
        
        // Calculate sector position
        uint64_t startSector = run.clusterOffset * sectorsPerCluster;
        uint32_t sectorCount = run.clusterCount * sectorsPerCluster;
        
        // Read data in chunks
        const uint32_t maxSectorsPerRead = 256; // Read in 128KB chunks (with 512-byte sectors)
        uint32_t sectorsRemaining = sectorCount;
        uint64_t currentSector = startSector;
        
        while (sectorsRemaining > 0) {
            uint32_t sectorsToRead = (sectorsRemaining < maxSectorsPerRead) ? 
                                     sectorsRemaining : maxSectorsPerRead;
            
            std::vector<uint8_t> buffer;
            if (!diskReader->ReadSectors(currentSector, sectorsToRead, buffer)) {
                std::wcerr << L"Failed to read sectors at " << currentSector << std::endl;
                outFile.close();
                return RecoveryStatus::FailedRead;
            }
            
            // Calculate how many bytes to write (don't exceed file size)
            uint64_t bytesToWrite = buffer.size();
            if (totalBytesRecovered + bytesToWrite > fileInfo.fileSize) {
                bytesToWrite = fileInfo.fileSize - totalBytesRecovered;
            }
            
            // Write to output file
            outFile.write(reinterpret_cast<const char*>(buffer.data()), bytesToWrite);
            if (!outFile) {
                std::wcerr << L"Failed to write to output file" << std::endl;
                outFile.close();
                return RecoveryStatus::FailedWrite;
            }
            
            totalBytesRecovered += bytesToWrite;
            currentSector += sectorsToRead;
            sectorsRemaining -= sectorsToRead;
            
            // Check if we've recovered the full file
            if (totalBytesRecovered >= fileInfo.fileSize) {
                break;
            }
        }
        
        if (totalBytesRecovered >= fileInfo.fileSize) {
            break;
        }
    }
    
    outFile.close();
    
    std::wcout << L"Recovered " << totalBytesRecovered << L" / " 
              << fileInfo.fileSize << L" bytes" << std::endl;
    
    if (totalBytesRecovered < fileInfo.fileSize) {
        std::wcout << L"Warning: File was only partially recovered" << std::endl;
        return RecoveryStatus::Partial;
    }
    
    std::wcout << L"File recovered successfully!" << std::endl;
    return RecoveryStatus::Success;
}

std::wstring RecoveryEngine::GetRecoveryStatusString(RecoveryStatus status) {
    switch (status) {
        case RecoveryStatus::Success:
            return L"Success";
        case RecoveryStatus::Partial:
            return L"Partial recovery";
        case RecoveryStatus::FailedRead:
            return L"Failed to read data";
        case RecoveryStatus::InvalidDataRun:
            return L"Invalid data run";
        case RecoveryStatus::FailedWrite:
            return L"Failed to write file";
        case RecoveryStatus::InvalidPath:
            return L"Invalid output path";
        default:
            return L"Unknown status";
    }
}
