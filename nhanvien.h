#pragma once
#include "structures.h"
#include <iostream>

// Khai báo trước các hàm so sánh và sắp xếp để giữ DSNV luôn có thứ tự tăng dần theo tên
struct Nhanvien;
int compareNhanvien(Nhanvien* a, Nhanvien* b);
void sortNhanvien(DanhSachNhanvien &ds);

// Hàm khởi tạo DSNV
void initDanhSachNhanvien(DanhSachNhanvien &ds) {
    ds.n = 0;
    for (int i = 0; i < 500; i++) {
        ds.node[i] = NULL;
    }
}

// Tìm vị trí nhân viên theo mã (trả về index hoặc -1 nếu ko tìm thấy)
int searchNhanvien(const DanhSachNhanvien &ds, int MANV) {
    for (int i = 0; i < ds.n; i++) {
        if (ds.node[i]->MANV == MANV) {
            return i;
        }
    }
    return -1;
}

// Thêm nhân viên
bool insertNhanvien(DanhSachNhanvien &ds, Nhanvien nv) {
    if (ds.n >= 500) return false; // Đầy
    if (searchNhanvien(ds, nv.MANV) != -1) return false; // Trùng mã
    if (strlen(nv.HO) == 0 || strlen(nv.TEN) == 0) return false; // Tên/Họ không được rỗng

    ds.node[ds.n] = new Nhanvien;
    *(ds.node[ds.n]) = nv; // Copy thông tin
    ds.n++;
    
    sortNhanvien(ds); // Sắp xếp lại danh sách nhân viên ngay lập tức để duy trì trật tự
    return true;
}

// Xóa nhân viên theo mã
bool deleteNhanvien(DanhSachNhanvien &ds, int MANV) {
    int idx = searchNhanvien(ds, MANV);
    if (idx == -1) return false; // Không tìm thấy

    // Kiểm tra nhân viên đã lập hóa đơn chưa
    if (ds.node[idx]->dshd != NULL) {
        return false; // Không được xóa nhân viên đã lập hóa đơn
    }

    delete ds.node[idx];
    
    // Dồn mảng
    for (int i = idx; i < ds.n - 1; i++) {
        ds.node[i] = ds.node[i + 1];
    }
    ds.node[ds.n - 1] = NULL;
    ds.n--;
    // Lưu ý: Dồn mảng không làm mất trật tự sắp xếp, nên không cần gọi lại sort
    return true;
}

// Cập nhật thông tin nhân viên theo mã
bool updateNhanvien(DanhSachNhanvien &ds, int MANV, const char* HO, const char* TEN, const char* PHAI) {
    if (strlen(HO) == 0 || strlen(TEN) == 0) return false;

    int idx = searchNhanvien(ds, MANV);
    if (idx == -1) return false;

    strcpy(ds.node[idx]->HO, HO);
    strcpy(ds.node[idx]->TEN, TEN);
    strcpy(ds.node[idx]->PHAI, PHAI);
    
    sortNhanvien(ds); // Sắp xếp lại danh sách sau khi thay đổi Họ/Tên
    return true;
}

// Hàm so sánh dùng để in giảm dần/tăng dần. Sắp xếp theo Tên tăng dần, nếu trùng thì Họ tăng.
int compareNhanvien(Nhanvien* a, Nhanvien* b) {
    int cmpTEN = compareNoCase(a->TEN, b->TEN);
    if (cmpTEN != 0) return cmpTEN;
    return compareNoCase(a->HO, b->HO);
}

// Sắp xếp danh sách nhân viên tăng dần
void sortNhanvien(DanhSachNhanvien &ds) {
    for (int i = 0; i < ds.n - 1; i++) {
        for (int j = i + 1; j < ds.n; j++) {
            if (compareNhanvien(ds.node[i], ds.node[j]) > 0) {
                Nhanvien* temp = ds.node[i];
                ds.node[i] = ds.node[j];
                ds.node[j] = temp;
            }
        }
    }
}

// Lấy danh sách nhân viên đã được sắp xếp (vì mảng gốc đã luôn được sắp xếp, ta chỉ cần copy)
void getSortedNhanvienList(const DanhSachNhanvien &ds, Nhanvien* arr[]) {
    for (int i = 0; i < ds.n; i++) {
        arr[i] = ds.node[i];
    }
}

