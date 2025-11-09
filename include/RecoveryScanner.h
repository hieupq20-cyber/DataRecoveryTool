#ifndef RECOVERY_SCANNER_H
#define RECOVERY_SCANNER_H

#include "NtfsStructures.h"
#include "NtfsParser.h"
#include "RawDiskReader.h"
#include <vector>

class RecoveryScanner {
private:
    NtfsParser* ntfsParser;
    RawDiskReader* diskReader;
    std::vector<DeletedFileInfo> deletedFiles;

public:
    RecoveryScanner();
    
    bool Initialize(NtfsParser* parser, RawDiskReader* reader);
    bool ScanDeletedFiles(uint64_t maxEntries = 10000);
    
    const std::vector<DeletedFileInfo>& GetDeletedFiles() const { return deletedFiles; }
    void PrintDeletedFiles() const;
    void ClearResults() { deletedFiles.clear(); }
};

#endif // RECOVERY_SCANNER_H
