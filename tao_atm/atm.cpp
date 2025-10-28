// atm.cpp
// File này chứa phần triển khai (implementations) của các hàm đã khai báo trong atm.h

#include "atm.h"
#include <iostream>
#include <fstream>
#include <limits>     // Dùng cho numeric_limits
#include <algorithm>  // Dùng cho all_of
#include <cctype>     // Dùng cho ::isdigit
#include "json.hpp"   // Thư viện JSON

// Có thể dùng an toàn trong file .cpp
using namespace std;
using json = nlohmann::json;


//1. Triển khai lớp Account


// Constructor
Account::Account(const string& p, long long b) : pin(p), balance(b) {}

// Kiểm tra PIN
bool Account::checkPin(const string& inputPin) const {
    return pin == inputPin;
}

// Rút tiền
bool Account::withdraw(long long amount) {
    if (amount <= 0) {
        return false;
    }
    if (amount % 50000 != 0) {
        return false;
    }
    if (amount > balance) {
        return false;
    }
    balance -= amount;
    return true;
}

// Gửi tiền
bool Account::deposit(long long amount) {
    if (amount <= 0) {
        cout << "So tien nap vao phai lon hon 0.\n";
        return false;
    }
    if (amount % 50000 != 0) {
        cout << "So tien nap vao phai la boi so cua 50000.\n";
        return false;
    }
    balance += amount; // Cộng tiền vào số dư
    return true;
}

// Lấy số dư
long long Account::getBalance() const {
    return balance;
}

// Lấy PIN (cho việc lưu file)
string Account::getPin() const {
    return pin;
}


// 2. Triển khai lớp ATM


// Constructor
ATM::ATM(const string& path) : filePath(path), activeAccountIndex(-1) {}


// Hàm tải tài khoản từ file JSON
bool ATM::loadAccounts() {
    ifstream file_input(filePath);
    if (!file_input.is_open()) {
        cerr << "Loi: Khong the mo file '" << filePath << "'. Kiem tra lai duong dan.\n";
        return false;
    }

    json data;
    try {
        data = json::parse(file_input);
        if (!data.is_array()) {
            cerr << "Loi: File JSON phai la mot mang (bat dau bang [ ).\n";
            file_input.close();
            return false;
        }

        accounts.clear();
        for (const auto& item : data) {
            if (item.is_object() && item.contains("pin") && item["pin"].is_string()
                && item.contains("balance") && item["balance"].is_number()) {
                // Đọc balance vào kiểu long long
                accounts.emplace_back(item["pin"].get<string>(), item["balance"].get<long long>());
            } else {
                 cerr << "Canh bao: Bo qua muc khong hop le trong file JSON.\n";
            }
        }
    } catch (json::parse_error& e) {
        cerr << "Loi: File JSON bi loi cu phap: " << e.what() << "\n";
        file_input.close();
        return false;
    } catch (json::type_error& e) {
         cerr << "Loi: Du lieu trong file JSON khong dung dinh dang: " << e.what() << "\n";
         file_input.close();
         return false;
    }
    file_input.close();
    return !accounts.empty();
}

// Hàm lưu tài khoản vào file JSON
bool ATM::saveAccounts() {
    ofstream file_output(filePath);
    if (!file_output.is_open()) {
        cerr << "Loi nghiem trong: Khong thể MỞ file de LUU!\n";
        return false;
    }

    json data = json::array();
    for (const auto& acc : accounts) {
        json account_json;
        account_json["pin"] = acc.getPin();
        account_json["balance"] = acc.getBalance();
        data.push_back(account_json);
    }

    try {
        file_output << data.dump(4);
    } catch (json::type_error& e) {
         cerr << "Loi nghiem trong khi ghi file JSON: " << e.what() << "\n";
         file_output.close();
         return false;
    }
    file_output.close();
    return true;
}

// Hàm yêu cầu nhập PIN
int ATM::promptPin() {
    string pinInput;
    int pinAttempts = 0;
    activeAccountIndex = -1;

    cout << "\n--------------------\n";
    cout << "Nhap ma PIN: ";

    while (pinAttempts < MAX_PIN_ATTEMPTS) {
        cin >> pinInput;

        // Xóa bộ đệm đầu vào
        if (cin.fail() || pinInput.length() != 4 || !all_of(pinInput.begin(), pinInput.end(), ::isdigit)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Ma PIN sai. Vui long nhap lai: ";
            continue;
        }

        for (int i = 0; i < accounts.size(); ++i) {
            if (accounts[i].checkPin(pinInput)) {
                activeAccountIndex = i;
                return activeAccountIndex;
            }
        }

        pinAttempts++;
        if (pinAttempts < MAX_PIN_ATTEMPTS) {
            cout << "Ma PIN sai. Vui long nhap lai (" << MAX_PIN_ATTEMPTS - pinAttempts << " lan thu con lai): ";
        } else {
            cout << "The da bi khoa, vui long lien he voi ngan hang\n";
            return -1;
        }
    }
    return -1;
}

//ham đợi nhấn Enter với thông điệp
void ATM::wait_enter(const std::string& message) {
    cout << message << endl;
    

    // Chờ người dùng nhấn Enter (đọc ký tự '\n' mới)
    cin.get();
}

// Hàm yêu cầu nhập số tiền rút
long long ATM::promptAmount() {
    if (activeAccountIndex < 0 || activeAccountIndex >= accounts.size()) {
        cerr << "Loi logic: Chua dang nhap tai khoan.\n";
        return -1; // Lỗi logic
    }

    long long currentBalance = accounts[activeAccountIndex].getBalance();
    cout << "PIN hop le. So du: " << currentBalance << " VND\n";

    // Kiểm tra số dư tối thiểu
    if (currentBalance < 50000) {
        cout << "SO DU KHONG DU DE RUT TIEN. VUI LONG NAP THEM TIEN.\n";
        return -1;
    }

    while (true) {
        cout << "Nhap so tien can rut (VND): ";
        long long amount;  // Dùng long long trực tiếp vì bàn phím ATM chỉ cho nhập số
        cin >> amount;

        // Kiểm tra các điều kiện hợp lệ
        if (amount > currentBalance) {
            cout << "SO DU KHONG DU DE RUT TIEN. So du hien tai: " << currentBalance << " VND\n";
            wait_enter("Xin nhan lai the");
            cout <<"Cam on quy khach\n";
            return -1;
        }
        
        else if (amount % 50000 != 0) {
            cout << "So tien phai la boi so cua 50000. Vui long nhap lai.\n";
            continue;
        }

        return amount; // Số tiền hợp lệ
    }
}

// Hàm yêu cầu nhập số tiền gửi
long long ATM::promptDepositAmount() {
    // Đảm bảo đã đăng nhập
    if (activeAccountIndex < 0 || activeAccountIndex >= accounts.size()) {
        cerr << "Loi logic: Chua dang nhap tai khoan.\n";
        return -1;
    }

    long long currentBalance = accounts[activeAccountIndex].getBalance();
    double amount_input;
    cout << "PIN hop le. So du hien tai: " << currentBalance << " VND\n";

    while (true) {
        cout << "2. Nhap so tien can NAP (VND): "; // Thay đổi lời nhắc
        cin >> amount_input;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Vui long nhap mot con so.\n";
            continue;
        }

        long long amount_ll = static_cast<long long>(amount_input);

        // Kiểm tra
        if (amount_input != static_cast<double>(amount_ll)) {
            cout << "So tien phai la so nguyen. Vui long nhap lai.\n";
        } else if (amount_ll <= 0) {
            cout << "So tien khong hop le (phai lon hon 0). Vui long nhap lai.\n";
        } else if (amount_ll % 50000 != 0) {
             cout << "So tien phai la boi so cua 50000. Vui long nhap lai.\n";
        } else {
            // Không cần kiểm tra số dư khi nạp tiền
            return amount_ll; // Trả về số tiền hợp lệ
        }
    }
}

// Hàm xử lý rút tiền
void ATM::handleWithdrawal() {
    if (promptPin() == -1) {
        return; // PIN sai
    }

    long long amountToWithdraw = promptAmount();
    if (amountToWithdraw < 0) {
        return; // Số tiền không hợp lệ hoặc không đủ
    }

    Account& currentAccount = accounts[activeAccountIndex];
    if (currentAccount.withdraw(amountToWithdraw)) {
        int wantsReceipt = promptReceipt(); // Hàm này đã tự dọn buffer

        if (wantsReceipt == 1) {
            cout << "Dang tao hoa don...\n";
            wait_enter("Vui long lay hoa don (Nhan Enter de tiep tuc)");
            wait_enter("Xin hay nhan tien (Nhan Enter de tiep tuc)");
            cout << "Cam on quy khach\n";
        } else {
            wait_enter("Xin hay nhan tien (Nhan Enter de tiep tuc)");
            cout << "Cam on quy khach\n";
        }

        if (!saveAccounts()) {
             cerr << "CANH BAO: Khong the luu thay doi vao file!\n";
        }
    } else {
        // Hàm withdraw không nên in gì, nên ở đây in
        cout << "Giao dich khong thanh cong.\n";
    }

    activeAccountIndex = -1; // Đăng xuất
    cout << "\n--------------------\n";
}

// Hàm xử lý nạp tiền
void ATM::handleDeposit() {
    if (promptPin() == -1) {
        return; // PIN sai
    }

    long long amountToDeposit = promptDepositAmount();
    if (amountToDeposit < 0) {
        return; // Số tiền không hợp lệ
    }

    Account& currentAccount = accounts[activeAccountIndex];
    if (currentAccount.deposit(amountToDeposit)) {
        cout << "Nap tien thanh cong. So du moi: " << currentAccount.getBalance() << " VND" << endl;

        int wantsReceipt = promptReceipt(); // Dùng chung hàm hỏi hóa đơn

        if (wantsReceipt == 1) {
            cout << "Dang tao hoa don...\n";
            wait_enter("Vui long lay hoa don (Nhan Enter de tiep tuc)");
            cout << "Cam on quy khach\n";
        } else {
            cout << "Cam on quy khach\n";
        }

        if (!saveAccounts()) { // Lưu lại file
             cerr << "CANH BAO: Khong the luu thay doi vao file!\n";
        }
    } else {
        // Trường hợp này chỉ xảy ra nếu code promptDepositAmount bị lỗi
        cout << "Giao dich khong thanh cong.\n";
    }

    activeAccountIndex = -1; // Đăng xuất
    cout << "\n--------------------\n";
}

// Hàm hỏi in hóa đơn
int ATM::promptReceipt() {
    int receiptChoice = -1;
    while (receiptChoice != 0 && receiptChoice != 1) {
        cout << "Ban co muon in hoa don? (1 = Co, 0 = Khong): ";
        cin >> receiptChoice;

        if (cin.fail() || (receiptChoice != 0 && receiptChoice != 1)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Lua chon khong hop le. Vui long nhap 1 hoac 0.\n";
            receiptChoice = -1;
        }
    }
    cout << endl;
    // == DÒNG NÀY ĐỂ DỌN SẠCH BUFFER ==
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return receiptChoice;
}

// Hàm chạy chính của ATM
void ATM::run() {
    // 1. Tải dữ liệu 
    if (!loadAccounts()) {
        cerr << "Khong the bat dau ATM do loi tai du lieu.\n";
        return;
    }

    // 2. Hiển thị menu lựa chọn
    cout << "\n======== CHAO MUNG DEN VOI ATM ========\n";
    cout << "Vui long chon giao dich:\n";
    cout << "1. Rut tien\n";
    cout << "2. Nap tien\n";
    cout << "Lua chon cua ban (1 hoac 2, nhan so khac de thoat): ";

    int choice;
    cin >> choice;

    
    
    // 3. Dọn buffer '\n' sau khi nhập `choice`
    // (Quan trọng, nếu không hàm promptPin tiếp theo có thể bị ảnh hưởng)
    cin.ignore(numeric_limits<streamsize>::max(), '\n');


    // 4. Gọi hàm xử lý tương ứng
    switch (choice) {
        case 1:
            handleWithdrawal();
            break;
        case 2:
            handleDeposit();
            break;
        default:
            cout << "Lua chon khong hop le. Ket thuc giao dich.\n";
            break;
    }
}