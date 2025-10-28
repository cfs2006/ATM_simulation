// main.cpp
// File này giữ nguyên, vì nó đã làm tốt nhiệm vụ của mình.

#include <iostream>
#include <string>
#include <fstream>
#include "atm.h"  // Chỉ cần include file header

using namespace std;

int main() {
    string filePath = "C:\\Users\\pun\\Desktop\\ATM_simulation\\mo_phong_DB\\account.json";

    cout << "==== MAY ATM MO PHONG ====" << endl;
    

    // Kiểm tra nhanh xem file có tồn tại không
    ifstream checkFile(filePath);
    if (!checkFile) {
        cerr << "Loi: File '" << filePath << "' khong ton tai hoac khong the mo.\n";
        return 1;
    }
    checkFile.close();

    // Tạo đối tượng ATM
    ATM myATM(filePath);

    // Chạy ATM
    myATM.run();
    
    return 0;
}