# Change Log

## [1.2] - 2022-08-11

- Thay đổi default check từ DiffChecker (diff command) thành DefaultChecker (compare.cc trên domjudge)
- Cho phép so sánh theo một số tham số mặc định

## [2.0] - 2022-08-10

### Added:

- Thêm hệ số để tính time limit thực tếcho từng ngôi ngữ dựa theo time limit cơ sở
  - C/C++: 1x
  - Java: 3x
  - Python: 5x
- Hỗ trợ tùy chọn chạy code python bằng pypy với option -P/--pypy

### Fixed

- Fix không trả về đúng kết quả TLE/RE