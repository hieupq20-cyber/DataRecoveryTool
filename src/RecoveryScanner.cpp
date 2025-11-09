#include "../include/RecoveryScanner.h"
#include <iostream>
#include <iomanip>

RecoveryScanner::RecoveryScanner() : ntfsParser(nullptr), diskReader(nullptr) {
}

bool RecoveryScanner::Initialize(NtfsParser* parser, RawDiskReader* reader) {
    ntfsParser = parser;
    diskReader = reader;
    
    if (!ntfsParser || !diskReader) {
        std::wcerr << L"Invalid parser or reader" << std::endl;
        return false;
    }
    
    return true;
}

bool RecoveryScanner::ScanDeletedFiles(uint64_t maxEntries) {
    deletedFiles.clear();
    
    if (!ntfsParser || !diskReader) {
        std::wcerr << L"Scanner not initialized" << std::endl;
        return false;
    }
    
    std::wcout << L"Scanning for deleted files (max " << maxEntries << L" entries)..." << std::endl;
    
    for (uint64_t i = 0; i < maxEntries; i++) {
        std::vector<uint8_t> entryBuffer;
        
        if (!ntfsParser->ReadMftEntry(i, entryBuffer)) {
            continue;
        }
        
        MftEntryHeader header;
        if (!ntfsParser->ParseMftEntry(entryBuffer, header)) {
            continue;
        }
        
        // Check if entry is deleted (not in use)
        if ((header.flags & MFT_RECORD_IN_USE) == 0) {
            DeletedFileInfo fileInfo;
            fileInfo.mftIndex = i;
            fileInfo.isDirectory = (header.flags & MFT_RECORD_IS_DIRECTORY) != 0;
            
            // Try to extract file name
            if (!ntfsParser->ExtractFileName(entryBuffer, fileInfo.fileName)) {
                continue; // Skip if no filename
            }
            
            // Try to extract file size
            if (!ntfsParser->ExtractFileSize(entryBuffer, fileInfo.fileSize)) {
                fileInfo.fileSize = 0;
            }
            
            // Try to extract data runs
            ntfsParser->ExtractDataRuns(entryBuffer, fileInfo.dataRuns);
            
            // Only add files (not directories) with valid data
            if (!fileInfo.isDirectory && fileInfo.fileSize > 0) {
                deletedFiles.push_back(fileInfo);
            }
        }
        
        // Progress indicator
        if ((i + 1) % 1000 == 0) {
            std::wcout << L"Scanned " << (i + 1) << L" entries, found " 
                      << deletedFiles.size() << L" deleted files...\r" << std::flush;
        }
    }
    
    std::wcout << std::endl;
    std::wcout << L"Scan complete. Found " << deletedFiles.size() << L" deleted files." << std::endl;
    
    return true;
}

void RecoveryScanner::PrintDeletedFiles() const {
    if (deletedFiles.empty()) {
        std::wcout << L"No deleted files found." << std::endl;
        return;
    }
    
    std::wcout << L"\n=== Deleted Files ===" << std::endl;
    std::wcout << L"Index | MFT# | File Name | Size | Data Runs" << std::endl;
    std::wcout << L"------+------+-----------+------+-----------" << std::endl;
    
    for (size_t i = 0; i < deletedFiles.size(); i++) {
        const auto& file = deletedFiles[i];
        
        std::wcout << std::setw(5) << i << L" | ";
        std::wcout << std::setw(4) << file.mftIndex << L" | ";
        std::wcout << std::setw(30) << file.fileName.substr(0, 30) << L" | ";
        
        // Format file size
        if (file.fileSize < 1024) {
            std::wcout << std::setw(8) << file.fileSize << L" B";
        } else if (file.fileSize < 1024 * 1024) {
            std::wcout << std::setw(8) << (file.fileSize / 1024) << L" KB";
        } else if (file.fileSize < 1024 * 1024 * 1024) {
            std::wcout << std::setw(8) << (file.fileSize / (1024 * 1024)) << L" MB";
        } else {
            std::wcout << std::setw(8) << (file.fileSize / (1024 * 1024 * 1024)) << L" GB";
        }
        
        std::wcout << L" | " << file.dataRuns.size() << L" runs";
        std::wcout << std::endl;
    }
    
    std::wcout << L"\nTotal: " << deletedFiles.size() << L" deleted files" << std::endl;
}
