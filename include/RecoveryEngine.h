#ifndef RECOVERY_ENGINE_H
#define RECOVERY_ENGINE_H

#include "NtfsStructures.h"
#include "NtfsParser.h"
#include "RawDiskReader.h"
#include <string>

enum class RecoveryStatus {
    Success,
    Partial,
    FailedRead,
    InvalidDataRun,
    FailedWrite,
    InvalidPath
};

class RecoveryEngine {
private:
    NtfsParser* ntfsParser;
    RawDiskReader* diskReader;

public:
    RecoveryEngine();
    
    bool Initialize(NtfsParser* parser, RawDiskReader* reader);
    
    RecoveryStatus RecoverFile(
        const DeletedFileInfo& fileInfo,
        const std::wstring& outputPath
    );
    
    static std::wstring GetRecoveryStatusString(RecoveryStatus status);
};

#endif // RECOVERY_ENGINE_H
