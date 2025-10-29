# Mô phỏng máy ATM bằng C++

Đây là một chương trình console C++ mô phỏng các chức năng cơ bản của một máy ATM. Chương trình cho phép người dùng rút tiền và nạp tiền, với dữ liệu tài khoản được đọc và ghi trực tiếp từ một file JSON.

## Tính năng

* Menu lựa chọn chức năng: **Rút tiền** hoặc **Nạp tiền**.
* Đăng nhập bằng mã PIN (có kiểm tra, khóa thẻ sau 5 lần nhập sai).
* Rút tiền: Kiểm tra số dư khả dụng và xác thực số tiền phải là bội số của 50,000 VND.
* Nạp tiền: Xác thực số tiền nạp vào phải là bội số của 50,000 VND.
* Lưu trữ dữ liệu: Sử dụng file `.json` để mô tả cơ bản DB của ngân hàng. Ở đây nó chỉ đọc và lưu trữ thông tin tài khoản (PIN và số dư). Mọi giao dịch thành công sẽ cập nhật lại file này.
* Mô phỏng xuất tiền, in hóa đơn và chờ người dùng nhấn `Enter` để tiếp tục (mô phỏng hành động lấy của người dùng)

## Yêu cầu

* Một trình biên dịch C++ (ví dụ: g++, Clang, MSVC).
* Thư viện **json.hpp** của Nlohmann (cần đặt file `json.hpp` trong cùng thư mục dự án).

## Hướng dẫn biên dịch

Mở terminal trong thư mục dự án và chạy lệnh sau:
* cd tao_atm

* g++ main.cpp atm.cpp -o main.exe -std=c++17
  
* ./main.exe 

### Lưu ý: không bấm nút RUN trong IDE vì bài này bao gồm 2 file cpp khác nhau, phải xem kĩ là đang cd đến thư mục 'tao_atm' thì mới bắt đầu biên dịch (lệnh cd trong CMD được dùng để thay đổi thư mục hiện tại).
