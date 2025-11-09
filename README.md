# RecorveryDataTool
## Phase 0 – Chuẩn bị & định hướng

 Xác định phạm vi tool

 Hệ điều hành: Windows

 Hệ file ưu tiên: NTFS (sau mở rộng FAT32, exFAT)

 Mục tiêu: khôi phục file bị xóa nhầm (chưa bị ghi đè)

 Chế độ chạy: Console + GUI (giai đoạn đầu làm console cho dễ debug)

 Chuẩn bị môi trường phát triển

 Cài Visual Studio (hoặc MSVC toolchain)

 Tạo project C++: DataRecoveryTool

 Cấu trúc thư mục:

 src/ – mã nguồn chính

 include/ – file header

 docs/ – tài liệu, ghi chú kỹ thuật

 tests/ – test nhỏ, script thử nghiệm

 Ôn/đọc lại kiến thức cần thiết

 C++ (RAII, file I/O, struct, pointer, vector, string)

 Kiến thức cơ bản về:

 Ổ đĩa, sector, cluster, partition (MBR/GPT)

 Cách Windows “xóa file” (thường chỉ bỏ flag, chưa xóa dữ liệu ngay)

 Cấu trúc cơ bản NTFS (Boot Sector, MFT, $Bitmap, Attribute…)

## Phase 1 – Truy cập raw disk (đọc ổ đĩa ở mức thấp)

### Task 1: Liệt kê các volume/partition có trên hệ thống ✅

 Viết module DiskEnumerator:

 Dùng Windows API (ví dụ: GetLogicalDriveStrings, GetDriveType, GetVolumeInformation)

 In danh sách: C:, D:, E:… + loại (Fixed/Removable) + filesystem (NTFS/FAT32…)

### Task 2: Mở volume ở chế độ raw ✅

 Viết module RawDiskReader:

 Dùng CreateFileW(L"\\\\.\\C:", ...) với quyền read-only

 Đảm bảo mở được cả ổ hệ thống (nếu có quyền admin)

 Viết hàm:

bool ReadSectors(HANDLE hVolume, uint64_t startSector, uint32_t sectorCount, std::vector<uint8_t>& buffer);


 Test: đọc Boot Sector (sector 0 của partition) và in ra một vài byte đầu để kiểm tra.

## Phase 2 – Parser hệ file NTFS (đọc cấu trúc logic)

### Task 3: Đọc & phân tích NTFS Boot Sector ✅

 Tạo struct C++ map với NTFS Boot Sector (BPB)

 Đọc các thông số:

 Bytes per sector

 Sectors per cluster

 Cluster index của MFT

 Viết hàm:

bool ParseNtfsBootSector(const std::vector<uint8_t>& buffer, NtfsBootSector& out);


 Kiểm tra OEM ID có là "NTFS " không → xác nhận đúng NTFS.

 Task 4: Xác định vị trí & đọc Master File Table (MFT)

 Từ Boot Sector → tính LBA (sector) của MFT

 Viết hàm:

bool ReadMftEntry(HANDLE hVolume, const NtfsBootSector& bpb, uint64_t entryIndex, std::vector<uint8_t>& buffer);


 Test: đọc vài entry đầu tiên (thường là $MFT, $MFTMirr, $LogFile, …)

 Task 5: Parse 1 MFT Entry

 Tạo struct MftHeader, NtfsAttributeHeader

 Viết code:

 Check signature "FILE"

 Lấy flags: inUse, isDirectory

 Iterate qua các Attribute:

 $STANDARD_INFORMATION

 $FILE_NAME

 $DATA (resident / non-resident)

 Trích xuất:

 Tên file

 Kích thước

 Thời gian tạo/sửa/xóa (nếu có)

 Danh sách runlist (nếu non-resident)

Phase 3 – Tìm & liệt kê file đã xóa

 Task 6: Xác định entry bị xóa

 Viết logic:

 Quét toàn bộ MFT entries (theo count ước lượng từ Boot Sector)

 Với mỗi entry:

 Nếu inUse == false → có khả năng là file đã xóa

 Lưu thông tin vào cấu trúc:

struct DeletedFileInfo {
    uint64_t mftIndex;
    std::wstring fileName;
    uint64_t fileSize;
    bool isDirectory;
    std::vector<DataRun> dataRuns;
};


 Lọc bỏ các entry rỗng / không có $DATA / không có tên.

 Task 7: Xây danh sách file đã xóa để hiển thị

 Tạo module RecoveryScanner:

 Hàm ScanDeletedFiles()

 Trả về danh sách DeletedFileInfo

 Trong bản console:

 In ra bảng:

 Index | Tên file | Kích thước | Đường dẫn (nếu reconstruct được) | Trạng thái (OK/Nguy cơ bị ghi đè)

Phase 4 – Chức năng khôi phục (recovery engine)

 Task 8: Khôi phục file từ runlist

 Viết hàm:

bool RecoverFile(
    HANDLE hVolume,
    const DeletedFileInfo& fileInfo,
    const std::wstring& outputPath);


 Logic:

 Từ DataRun → tính toán sector/cluster vật lý

 Dùng ReadFile/ReadSectors để đọc từng đoạn

 Ghi ra file mới ở ổ khác với đường dẫn outputPath

 Đảm bảo:

 Chỉ đọc, không ghi bất kỳ byte nào lên volume nguồn

 Kiểm tra size đọc đủ với fileSize

 Task 9: Xử lý các case phân mảnh / lỗi

 Nếu runlist phức tạp → log cảnh báo “khả năng hỏng”

 Nếu đọc thiếu sector → thông báo “không khôi phục hoàn chỉnh”

 Thêm mã lỗi/enum để báo kết quả:

enum class RecoveryStatus {
    Success,
    Partial,
    FailedRead,
    InvalidDataRun
};


 Task 10: (Nâng cao sau) Khôi phục theo signature (file carving)

 Xây module scan raw disk theo mẫu đầu file (JPEG, PNG, DOCX…)

 Không cần MFT, dùng pattern matching

 Giai đoạn sau, khi core NTFS đã ổn.

## Phase 5 – Giao diện dòng lệnh (CLI)

 Task 11: Xây menu console

 Màn hình chính:

Chọn volume để scan

Scan file đã xóa

Xem danh sách file đã xóa

Khôi phục file theo số thứ tự

Thoát

 Viết code xử lý input an toàn (tránh crash khi nhập linh tinh)

 In log đơn giản ra console (bước đầu chưa cần logging phức tạp)

## Phase 6 – Giao diện GUI (kết hợp với console)

 Task 12: Chọn framework GUI

 Option:

 Win32 API thuần (khó nhưng không phụ thuộc thư viện ngoài)

 Qt / wxWidgets (dễ làm UI hơn nhưng cần lib ngoài)

 Tối thiểu:

 Window chính hiển thị:

 Combo chọn volume

 Nút “Scan deleted files”

 Bảng hiển thị danh sách file đã xóa

 Nút “Recover selected”

 Chọn thư mục output

 Task 13: Tách core logic & UI

 Đảm bảo các module:

RawDiskReader, NtfsParser, RecoveryScanner, RecoveryEngine

Không phụ thuộc vào console hay GUI

 UI (console/GUI) chỉ gọi hàm từ các module này → dễ bảo trì, test.

## Phase 7 – Logging, an toàn, tài liệu

 Task 14: Thêm hệ thống logging

 Module Logger (log ra file .log):

 Lưu thao tác:

 Volume nào được scan

 File nào được khôi phục, trạng thái

 Lỗi đọc/ghi

 Cho phép bật/tắt debug log qua config.

 Task 15: Viết tài liệu & cảnh báo cho người dùng

 Mô tả:

 Tool chỉ đọc dữ liệu nguồn

 Khuyến nghị:

 Ngừng dùng ổ chứa dữ liệu bị xóa để giảm nguy cơ ghi đè

 Lưu file khôi phục sang ổ khác

 Thêm mục “Known limitations”:

 Không đảm bảo 100% khôi phục

 File bị ghi đè thì không cứu được

## Xây dựng & chạy thử

```
cmake -S . -B build
cmake --build build
```

> ⚠️ Ứng dụng hiện chỉ chạy được trên Windows vì phụ thuộc Windows API.
