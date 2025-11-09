#pragma once

#include <string>
#include <vector>

namespace drt {

struct DiskInfo {
    std::wstring rootPath;
    std::wstring driveType;
    std::wstring fileSystem;
};

std::vector<DiskInfo> EnumerateVolumes();

} // namespace drt
