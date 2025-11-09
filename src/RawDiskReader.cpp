#include "RawDiskReader.h"

#ifdef _WIN32
#include <Windows.h>
#endif

#include <stdexcept>

namespace drt {

RawDiskReader::RawDiskReader()
#ifdef _WIN32
    : m_handle(INVALID_HANDLE_VALUE)
#endif
{
}

RawDiskReader::~RawDiskReader() { close(); }

bool RawDiskReader::open(const std::wstring &volumePath) {
#ifdef _WIN32
    close();
    m_handle = CreateFileW(volumePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    return m_handle != INVALID_HANDLE_VALUE;
#else
    (void)volumePath;
    throw std::runtime_error("Raw disk reading is only supported on Windows.");
#endif
}

void RawDiskReader::close() {
#ifdef _WIN32
    if (m_handle != nullptr && m_handle != INVALID_HANDLE_VALUE) {
        CloseHandle(static_cast<HANDLE>(m_handle));
        m_handle = INVALID_HANDLE_VALUE;
    }
#endif
}

bool RawDiskReader::readSectors(std::uint64_t startSector, std::uint32_t sectorCount, std::vector<std::uint8_t> &buffer) const {
#ifdef _WIN32
    if (m_handle == nullptr || m_handle == INVALID_HANDLE_VALUE) {
        return false;
    }

    LARGE_INTEGER position;
    position.QuadPart = static_cast<LONGLONG>(startSector) * 512ULL; // default sector size assumption
    if (!SetFilePointerEx(static_cast<HANDLE>(m_handle), position, nullptr, FILE_BEGIN)) {
        return false;
    }

    DWORD bytesToRead = sectorCount * 512U;
    buffer.resize(bytesToRead);

    DWORD bytesRead = 0;
    if (!ReadFile(static_cast<HANDLE>(m_handle), buffer.data(), bytesToRead, &bytesRead, nullptr)) {
        buffer.clear();
        return false;
    }

    buffer.resize(bytesRead);
    return bytesRead == bytesToRead;
#else
    (void)startSector;
    (void)sectorCount;
    (void)buffer;
    throw std::runtime_error("Raw disk reading is only supported on Windows.");
#endif
}

bool RawDiskReader::isOpen() const noexcept {
#ifdef _WIN32
    return m_handle != nullptr && m_handle != INVALID_HANDLE_VALUE;
#else
    return false;
#endif
}

} // namespace drt
