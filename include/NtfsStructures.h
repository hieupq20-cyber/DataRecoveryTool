#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace drt {

struct NtfsBootSector {
    std::string oemId;
    std::uint16_t bytesPerSector = 0;
    std::uint8_t sectorsPerCluster = 0;
    std::uint16_t reservedSectors = 0;
    std::uint64_t totalSectors = 0;
    std::uint64_t mftCluster = 0;
    std::uint64_t mftMirrorCluster = 0;
    std::int8_t clustersPerFileRecord = 0;
    std::int8_t clustersPerIndexRecord = 0;
};

bool parseNtfsBootSector(const std::vector<std::uint8_t> &buffer, NtfsBootSector &out);

} // namespace drt
