#include "../include/DiskEnumerator.h"
#include "../include/RawDiskReader.h"
#include "../include/NtfsParser.h"
#include "../include/RecoveryScanner.h"
#include "../include/RecoveryEngine.h"
#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <io.h>
#include <fcntl.h>

void PrintBanner() {
    std::wcout << L"========================================" << std::endl;
    std::wcout << L"   Data Recovery Tool v1.0" << std::endl;
    std::wcout << L"   NTFS File Recovery Utility" << std::endl;
    std::wcout << L"========================================" << std::endl;
    std::wcout << std::endl;
    std::wcout << L"WARNING:" << std::endl;
    std::wcout << L"- This tool reads data from disk in read-only mode" << std::endl;
    std::wcout << L"- Stop using the drive to prevent data overwrite" << std::endl;
    std::wcout << L"- Save recovered files to a different drive" << std::endl;
    std::wcout << L"- Administrator privileges may be required" << std::endl;
    std::wcout << std::endl;
}

void PrintMenu() {
    std::wcout << L"\n=== Main Menu ===" << std::endl;
    std::wcout << L"1. List available volumes" << std::endl;
    std::wcout << L"2. Select volume and scan for deleted files" << std::endl;
    std::wcout << L"3. View deleted files list" << std::endl;
    std::wcout << L"4. Recover file by index" << std::endl;
    std::wcout << L"5. Exit" << std::endl;
    std::wcout << L"Enter choice: ";
}

int main() {
    // Enable Unicode console output
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
    
    PrintBanner();
    
    RawDiskReader diskReader;
    NtfsParser ntfsParser;
    RecoveryScanner scanner;
    RecoveryEngine recoveryEngine;
    
    bool volumeSelected = false;
    bool filesScanned = false;
    
    while (true) {
        PrintMenu();
        
        int choice;
        std::wcin >> choice;
        
        if (std::wcin.fail()) {
            std::wcin.clear();
            std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
            std::wcout << L"Invalid input. Please enter a number." << std::endl;
            continue;
        }
        
        std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
        
        switch (choice) {
            case 1: {
                // List available volumes
                DiskEnumerator::PrintAllVolumes();
                break;
            }
            
            case 2: {
                // Select volume and scan
                std::wcout << L"\nEnter drive letter (e.g., C): ";
                std::wstring driveLetter;
                std::wcin >> driveLetter;
                std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
                
                if (driveLetter.empty()) {
                    std::wcout << L"Invalid drive letter." << std::endl;
                    break;
                }
                
                // Close previous volume if open
                if (volumeSelected) {
                    diskReader.CloseVolume();
                }
                
                // Open volume
                if (!diskReader.OpenVolume(driveLetter)) {
                    std::wcout << L"Failed to open volume. Make sure to run as Administrator." << std::endl;
                    volumeSelected = false;
                    break;
                }
                
                volumeSelected = true;
                
                // Initialize NTFS parser
                if (!ntfsParser.Initialize(&diskReader)) {
                    std::wcout << L"Failed to initialize NTFS parser. Is this an NTFS volume?" << std::endl;
                    volumeSelected = false;
                    break;
                }
                
                // Print boot sector info
                NtfsParser::PrintBootSectorInfo(ntfsParser.GetBootSector());
                
                // Initialize scanner
                if (!scanner.Initialize(&ntfsParser, &diskReader)) {
                    std::wcout << L"Failed to initialize scanner." << std::endl;
                    break;
                }
                
                // Scan for deleted files
                std::wcout << L"\nEnter maximum MFT entries to scan (e.g., 10000): ";
                uint64_t maxEntries;
                std::wcin >> maxEntries;
                std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
                
                if (scanner.ScanDeletedFiles(maxEntries)) {
                    filesScanned = true;
                    scanner.PrintDeletedFiles();
                }
                
                break;
            }
            
            case 3: {
                // View deleted files list
                if (!filesScanned) {
                    std::wcout << L"No scan has been performed yet. Please scan first (option 2)." << std::endl;
                } else {
                    scanner.PrintDeletedFiles();
                }
                break;
            }
            
            case 4: {
                // Recover file
                if (!volumeSelected) {
                    std::wcout << L"No volume selected. Please select and scan first (option 2)." << std::endl;
                    break;
                }
                
                if (!filesScanned) {
                    std::wcout << L"No scan has been performed yet. Please scan first (option 2)." << std::endl;
                    break;
                }
                
                const auto& deletedFiles = scanner.GetDeletedFiles();
                if (deletedFiles.empty()) {
                    std::wcout << L"No deleted files found." << std::endl;
                    break;
                }
                
                std::wcout << L"\nEnter file index to recover (0-" << (deletedFiles.size() - 1) << L"): ";
                size_t fileIndex;
                std::wcin >> fileIndex;
                std::wcin.ignore(std::numeric_limits<std::streamsize>::max(), L'\n');
                
                if (fileIndex >= deletedFiles.size()) {
                    std::wcout << L"Invalid file index." << std::endl;
                    break;
                }
                
                std::wcout << L"Enter output file path (e.g., D:\\recovered_file.txt): ";
                std::wstring outputPath;
                std::getline(std::wcin, outputPath);
                
                if (outputPath.empty()) {
                    std::wcout << L"Invalid output path." << std::endl;
                    break;
                }
                
                // Initialize recovery engine
                if (!recoveryEngine.Initialize(&ntfsParser, &diskReader)) {
                    std::wcout << L"Failed to initialize recovery engine." << std::endl;
                    break;
                }
                
                // Recover the file
                RecoveryStatus status = recoveryEngine.RecoverFile(deletedFiles[fileIndex], outputPath);
                std::wcout << L"Recovery status: " << RecoveryEngine::GetRecoveryStatusString(status) << std::endl;
                
                break;
            }
            
            case 5: {
                // Exit
                std::wcout << L"Exiting..." << std::endl;
                diskReader.CloseVolume();
                return 0;
            }
            
            default: {
                std::wcout << L"Invalid choice. Please enter 1-5." << std::endl;
                break;
            }
        }
    }
    
    return 0;
}
