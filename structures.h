#pragma once
#include <iostream>
#include <string>
#include <string.h>
#include <vector>

using namespace std;

// ==========================================
// 1. VẬT TƯ (Material) - Cây nhị phân tìm kiếm
// ==========================================
struct VatTu {
    char MAVT[11];
    char TENVT[51];
    char DVT[21];
    float SoLuongTon; // The doc says: "Số lượng tồn"
};

struct NodeVatTu {
    VatTu info;
    NodeVatTu* left;
    NodeVatTu* right;
};
typedef NodeVatTu* TreeVatTu; // Danh sách Vattu: cây nhị phân tìm kiếm

// ==========================================
// 2. CHI TIẾT HÓA ĐƠN (Invoice Detail) - Danh sách tuyến tính (mảng)
// ==========================================
// The doc says: "Danh sách CT_HOADON : danh sách tuyến tính ( MAVT,  Soluong, Dongia, %VAT)"
struct CT_HOADON {
    char MAVT[11]; // Mã vật tư
    float Soluong; // Số lượng 
    float Dongia;  // Đơn giá
    float VAT;     // % VAT
};

struct DanhSachCTHD {
    int n = 0; // Một hóa đơn có tối đa 20 vật tư.
    CT_HOADON node[20];
};

// ==========================================
// 3. HÓA ĐƠN (Invoice) - Danh sách liên kết đơn
// ==========================================
// The doc says: "Danh sách HOADON : danh sách liên kết đơn(SoHD (C20), Ngày lập hóa đơn, Loai, dscthd)"

struct Date {
    int day, month, year;
};

struct HOADON {
    char SoHD[21];
    Date NgayLap;
    char Loai; // 'N' hoặc 'X'
    DanhSachCTHD dscthd;
};

struct NodeHOADON {
    HOADON info;
    NodeHOADON* next;
};
typedef NodeHOADON* ListHOADON;

// ==========================================
// 4. NHÂN VIÊN (Employee) - Mảng con trỏ
// ==========================================
// The doc says: "Danh sách Nhanvien : danh sách tuyến tính là 1 mảng con trỏ (MANV, HO, TEN, PHAI, con trỏ dshd)"
struct Nhanvien {
    int MANV;
    char HO[51];
    char TEN[21];
    char PHAI[11]; 
    ListHOADON dshd = NULL; // Con trỏ dshd sẽ trỏ đến danh sách các hóa đơn
};

struct DanhSachNhanvien {
    int n = 0; // Danh sách nhân viên có tối đa 500 nhân viên
    Nhanvien* node[500]; // Mảng con trỏ
};

#include <ctype.h>
// Hàm so sánh chuỗi không phân biệt chữ hoa chữ thường dùng chung cho toàn dự án
inline int compareNoCase(const char* s1, const char* s2) {
    while (*s1 && *s2) {
        char c1 = tolower((unsigned char)*s1);
        char c2 = tolower((unsigned char)*s2);
        if (c1 != c2) return c1 - c2;
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

// Hàm kiểm tra chuỗi có trống hoặc chứa toàn khoảng trắng hay không
inline bool isOnlySpaces(const char* str) {
    if (str == NULL || str[0] == '\0') return true;
    while (*str) {
        if (!isspace((unsigned char)*str)) return false;
        str++;
    }
    return true;
}

// Hàm cắt bỏ khoảng trắng thừa ở đầu và cuối chuỗi (Trimming)
inline void trimString(char* str) {
    if (str == NULL || str[0] == '\0') return;
    int start = 0;
    while (str[start] != '\0' && isspace((unsigned char)str[start])) {
        start++;
    }
    if (str[start] == '\0') {
        str[0] = '\0';
        return;
    }
    int end = strlen(str) - 1;
    while (end > start && isspace((unsigned char)str[end])) {
        end--;
    }
    int i;
    for (i = 0; start <= end; i++, start++) {
        str[i] = str[start];
    }
    str[i] = '\0';
}

struct UTF8CharPair {
    const char* upper;
    const char* lower;
};

// Danh sách các cặp ký tự UTF-8 viết hoa - viết thường tiếng Việt
inline const UTF8CharPair vtCharPairs[] = {
    {"À", "à"}, {"Á", "á"}, {"Ả", "ả"}, {"Ã", "ã"}, {"Ạ", "ạ"},
    {"Ă", "ă"}, {"Ằ", "ằ"}, {"Ắ", "ắ"}, {"Ẳ", "ẳ"}, {"Ẵ", "ẵ"}, {"Ặ", "ặ"},
    {"Â", "â"}, {"Ầ", "ầ"}, {"Ấ", "ấ"}, {"Ẩ", "ẩ"}, {"Ẫ", "ẫ"}, {"Ậ", "ậ"},
    {"È", "è"}, {"É", "é"}, {"Ẻ", "ẻ"}, {"Ẽ", "ẽ"}, {"Ẹ", "ẹ"},
    {"Ê", "ê"}, {"Ề", "ề"}, {"Ế", "ế"}, {"Ể", "ể"}, {"Ễ", "ễ"}, {"Ệ", "ệ"},
    {"Ì", "ì"}, {"Í", "í"}, {"Ỉ", "ỉ"}, {"Ĩ", "ĩ"}, {"Ị", "ị"},
    {"Ò", "ò"}, {"Ó", "ó"}, {"Ỏ", "ỏ"}, {"Õ", "õ"}, {"Ọ", "ọ"},
    {"Ô", "ô"}, {"Ồ", "ồ"}, {"Ố", "ố"}, {"Ổ", "ổ"}, {"Ỗ", "ỗ"}, {"Ộ", "ộ"},
    {"Ơ", "ơ"}, {"Ờ", "ờ"}, {"Ớ", "ớ"}, {"Ở", "ở"}, {"Ỡ", "ỡ"}, {"Ợ", "ợ"},
    {"Ù", "ù"}, {"Ú", "ú"}, {"Ủ", "ủ"}, {"Ũ", "ũ"}, {"Ụ", "ụ"},
    {"Ư", "ư"}, {"Ừ", "ừ"}, {"Ứ", "ứ"}, {"Ử", "ử"}, {"Ữ", "ữ"}, {"Ự", "ự"},
    {"Ỳ", "ỳ"}, {"Ý", "ý"}, {"Ỷ", "ỷ"}, {"Ỹ", "ỹ"}, {"Ỵ", "ỵ"},
    {"Đ", "đ"}
};

inline std::string toUpperUTF8(const std::string& ch) {
    if (ch.length() == 1) {
        char c = ch[0];
        if (c >= 'a' && c <= 'z') return std::string(1, c - 'a' + 'A');
        return ch;
    }
    for (const auto& pair : vtCharPairs) {
        if (ch == pair.lower) return pair.upper;
    }
    return ch;
}

inline std::string toLowerUTF8(const std::string& ch) {
    if (ch.length() == 1) {
        char c = ch[0];
        if (c >= 'A' && c <= 'Z') return std::string(1, c - 'A' + 'a');
        return ch;
    }
    for (const auto& pair : vtCharPairs) {
        if (ch == pair.upper) return pair.lower;
    }
    return ch;
}

inline std::vector<std::string> splitIntoUTF8Chars(const std::string& str) {
    std::vector<std::string> chars;
    std::string currentChar;
    for (size_t i = 0; i < str.length(); i++) {
        unsigned char c = str[i];
        if ((c & 0xC0) != 0x80) { // Bắt đầu một ký tự mới
            if (!currentChar.empty()) {
                chars.push_back(currentChar);
            }
            currentChar = std::string(1, c);
        } else {
            currentChar += c;
        }
    }
    if (!currentChar.empty()) {
        chars.push_back(currentChar);
    }
    return chars;
}

inline void formatName(char* text) {
    if (text == NULL || text[0] == '\0') return;
    std::string str(text);
    std::vector<std::string> utf8Chars = splitIntoUTF8Chars(str);
    
    std::string result = "";
    bool isNewWord = true;
    
    for (size_t i = 0; i < utf8Chars.size(); i++) {
        const std::string& ch = utf8Chars[i];
        
        // Kiểm tra xem ký tự có phải khoảng trắng không
        if (ch == " " || ch == "\t" || ch == "\n" || ch == "\r") {
            if (!result.empty() && result.back() != ' ') {
                result += " ";
            }
            isNewWord = true;
        } else {
            if (isNewWord) {
                result += toUpperUTF8(ch);
                isNewWord = false;
            } else {
                result += toLowerUTF8(ch);
            }
        }
    }
    
    // Cắt khoảng trắng thừa ở cuối chuỗi
    while (!result.empty() && result.back() == ' ') {
        result.pop_back();
    }
    
    // Copy lại vào buffer cũ
    strncpy(text, result.c_str(), 99); // Giới hạn kích thước tránh tràn buffer
    text[99] = '\0';
}
