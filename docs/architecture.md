# DataRecoveryTool – Kiến trúc giai đoạn đầu

Tài liệu này ghi lại những thành phần đã được triển khai theo kế hoạch trong README.

## Module `DiskEnumerator`
* Chịu trách nhiệm liệt kê các volume có trên hệ thống Windows bằng Windows API.
* Xuất danh sách `DiskInfo` gồm ký tự ổ đĩa, loại ổ và filesystem.

## Module `RawDiskReader`
* Cung cấp API đọc sector ở chế độ raw từ một volume.
* Hiện tại giới hạn trên Windows, đọc theo block 512 byte.

## Module `NtfsStructures`
* Parse NTFS Boot Sector từ buffer 512 byte.
* Trả về các thông tin quan trọng để định vị MFT trong những giai đoạn kế tiếp.

## Ứng dụng console
* Menu đơn giản cho phép người dùng chọn volume và đọc Boot Sector.
* Dùng wide string để hiển thị tiếng Việt chính xác.
