#include "NtfsStructures.h"

#include <array>
#include <cstring>

namespace drt {

namespace {
constexpr std::size_t kBootSectorSize = 512;

std::uint64_t readLE64(const std::uint8_t *data) {
    std::uint64_t value = 0;
    for (int i = 7; i >= 0; --i) {
        value = (value << 8) | data[i];
    }
    return value;
}

std::uint32_t readLE32(const std::uint8_t *data) {
    std::uint32_t value = 0;
    for (int i = 3; i >= 0; --i) {
        value = (value << 8) | data[i];
    }
    return value;
}

std::uint16_t readLE16(const std::uint8_t *data) {
    return static_cast<std::uint16_t>(data[0] | (data[1] << 8));
}

} // namespace

bool parseNtfsBootSector(const std::vector<std::uint8_t> &buffer, NtfsBootSector &out) {
    if (buffer.size() < kBootSectorSize) {
        return false;
    }

    out.oemId.assign(reinterpret_cast<const char *>(&buffer[3]), 8);
    out.bytesPerSector = readLE16(&buffer[11]);
    out.sectorsPerCluster = buffer[13];
    out.reservedSectors = readLE16(&buffer[14]);
    out.totalSectors = readLE64(&buffer[40]);
    out.mftCluster = readLE64(&buffer[48]);
    out.mftMirrorCluster = readLE64(&buffer[56]);
    out.clustersPerFileRecord = static_cast<std::int8_t>(buffer[64]);
    out.clustersPerIndexRecord = static_cast<std::int8_t>(buffer[68]);

    return out.oemId.find("NTFS") != std::string::npos;
}

} // namespace drt
