// atm.h
// File này CHỈ chứa khai báo (definitions) của các lớp.

#ifndef ATM_H
#define ATM_H

#include <string>
#include <vector>

//=============================================
// 1. Định nghĩa lớp Account
//=============================================
class Account {
private:
    std::string pin;
    long long balance; // Dùng long long cho tiền tệ để tránh lỗi làm tròn

public:
    // Constructor
    Account(const std::string& p, long long b);

    // Các hàm thành viên
    bool checkPin(const std::string& inputPin) const;
    bool deposit(long long amount);
    bool withdraw(long long amount); // Dùng long long
    long long getBalance() const;     // Dùng long long
    std::string getPin() const;
};

//=============================================
// 2. Định nghĩa lớp ATM
//=============================================
class ATM {
private:
    std::string filePath;
    std::vector<Account> accounts;
    int activeAccountIndex;
    const int MAX_PIN_ATTEMPTS = 5;

    // Các hàm private trợ giúp
    bool loadAccounts();
    bool saveAccounts();
    void wait_enter(const std::string& message);
    int promptPin();
    long long promptAmount();
    long long promptDepositAmount();
    int promptReceipt();
    void handleWithdrawal(); 
    void handleDeposit();    
public:
    // Constructor
    ATM(const std::string& path);

    // Hàm chạy chính của ATM
    void run();
};

#endif // ATM_H