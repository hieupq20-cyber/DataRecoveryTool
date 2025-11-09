#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace drt {

class RawDiskReader {
public:
    RawDiskReader();
    ~RawDiskReader();

    bool open(const std::wstring &volumePath);
    void close();
    bool readSectors(std::uint64_t startSector, std::uint32_t sectorCount, std::vector<std::uint8_t> &buffer) const;
    bool isOpen() const noexcept;

private:
#ifdef _WIN32
    void *m_handle;
#endif
};

} // namespace drt
