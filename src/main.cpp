#include "DiskEnumerator.h"
#include "NtfsStructures.h"
#include "RawDiskReader.h"

#include <iostream>
#include <locale>
#include <cstdint>
#include <string>
#include <vector>

#ifdef _WIN32
#include <Windows.h>
#endif

namespace {

void printVolumes(const std::vector<drt::DiskInfo> &volumes) {
    std::wcout << L"\nDanh sách volume phát hiện:\n";
    for (std::size_t i = 0; i < volumes.size(); ++i) {
        const auto &vol = volumes[i];
        std::wcout << i + 1 << L". " << vol.rootPath << L" (" << vol.driveType << L", " << vol.fileSystem << L")\n";
    }
}

void printBootInfo(const drt::NtfsBootSector &boot) {
    std::wcout << L"\nThông tin NTFS Boot Sector:\n";
    std::wcout << L"OEM ID: " << std::wstring(boot.oemId.begin(), boot.oemId.end()) << L"\n";
    std::wcout << L"Bytes/Sector: " << boot.bytesPerSector << L"\n";
    std::wcout << L"Sectors/Cluster: " << static_cast<int>(boot.sectorsPerCluster) << L"\n";
    std::wcout << L"Cluster MFT: " << boot.mftCluster << L"\n";
    std::wcout << L"Cluster MFT Mirror: " << boot.mftMirrorCluster << L"\n";
}

} // namespace

int wmain() {
#ifndef _WIN32
    std::cerr << "DataRecoveryTool hiện chỉ chạy trên Windows." << std::endl;
    return 1;
#else
    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());

    auto volumes = drt::EnumerateVolumes();
    if (volumes.empty()) {
        std::wcerr << L"Không tìm thấy volume nào." << std::endl;
        return 1;
    }

    printVolumes(volumes);

    std::wcout << L"\nChọn volume để đọc Boot Sector (nhập số, 0 để thoát): ";
    std::size_t selection = 0;
    std::wcin >> selection;
    if (!std::wcin || selection == 0 || selection > volumes.size()) {
        std::wcerr << L"Lựa chọn không hợp lệ." << std::endl;
        return 1;
    }

    const auto &chosen = volumes[selection - 1];
    std::wstring volumePath = L"\\\\.\\" + chosen.rootPath.substr(0, 2);

    drt::RawDiskReader reader;
    if (!reader.open(volumePath)) {
        std::wcerr << L"Không mở được volume." << std::endl;
        return 1;
    }

    std::vector<std::uint8_t> buffer;
    if (!reader.readSectors(0, 1, buffer)) {
        std::wcerr << L"Đọc boot sector thất bại." << std::endl;
        return 1;
    }

    drt::NtfsBootSector boot{};
    if (!drt::parseNtfsBootSector(buffer, boot)) {
        std::wcerr << L"Volume không phải NTFS hoặc boot sector không hợp lệ." << std::endl;
        return 1;
    }

    printBootInfo(boot);
    return 0;
#endif
}

#ifndef _WIN32
int main() { return wmain(); }
#endif
