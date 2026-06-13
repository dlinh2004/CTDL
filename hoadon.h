#pragma once
#include "structures.h"
#include "vattu.h"
#include <iostream>

using namespace std;

// Khởi tạo ListHOADON
void initListHOADON(ListHOADON &head) {
    head = NULL;
}

// Tạo node Hóa đơn
NodeHOADON* createNodeHOADON(HOADON hd) {
    NodeHOADON* p = new NodeHOADON;
    if (p == NULL) return NULL;
    p->info = hd;
    p->next = NULL;
    return p;
}

// Thêm hóa đơn vào cuối danh sách
void insertHOADONTail(ListHOADON &head, HOADON hd) {
    NodeHOADON* p = createNodeHOADON(hd);
    if (head == NULL) {
        head = p;
    } else {
        NodeHOADON* cur = head;
        while (cur->next != NULL) {
            cur = cur->next;
        }
        cur->next = p;
    }
}

// Tìm hóa đơn theo mã (Trong toàn bộ danh sách HĐ của một nhân viên)
NodeHOADON* searchHOADON(ListHOADON head, const char* soHD) {
    NodeHOADON* cur = head;
    while (cur != NULL) {
        if (strcmp(cur->info.SoHD, soHD) == 0) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

// KHỞI TẠO VÀ THÊM CHI TIẾT HÓA ĐƠN
void initDscthd(DanhSachCTHD &ds) {
    ds.n = 0;
}

int searchCTHD(DanhSachCTHD &ds, const char* MAVT) {
    for (int i = 0; i < ds.n; i++) {
        if (strcmp(ds.node[i].MAVT, MAVT) == 0) {
            return i;
        }
    }
    return -1;
}

bool insertCTHD(DanhSachCTHD &ds, CT_HOADON cthd) {
    if (ds.n >= 20) return false;
    if (searchCTHD(ds, cthd.MAVT) != -1) return false; // Trùng mã VT trong 1 HĐ (có thể gộp thay vì báo trùng, nhưng yêu cầu ko nói rõ)

    ds.node[ds.n] = cthd;
    ds.n++;
    return true;
}

bool deleteCTHD(DanhSachCTHD &ds, const char* MAVT) {
    int idx = searchCTHD(ds, MAVT);
    if (idx == -1) return false;

    for (int i = idx; i < ds.n - 1; i++) {
        ds.node[i] = ds.node[i + 1];
    }
    ds.n--;
    return true;
}

// Tính trị giá 1 chi tiết hóa đơn
float calculateCTHDTotal(const CT_HOADON& ct) {
    return ct.Soluong * ct.Dongia * (1.0f + ct.VAT / 100.0f);
}

// Tính tổng trị giá của hóa đơn
float calculateHOADONTotal(const HOADON &hd) {
    float total = 0;
    for (int i = 0; i < hd.dscthd.n; i++) {
        total += calculateCTHDTotal(hd.dscthd.node[i]);
    }
    return total;
}

// Hàm so sánh 2 Date (trả về 1 nếu d1 > d2, -1 nếu d1 < d2, 0 nếu d1 == d2)
int compareDate(Date d1, Date d2) {
    if (d1.year > d2.year) return 1;
    if (d1.year < d2.year) return -1;
    if (d1.month > d2.month) return 1;
    if (d1.month < d2.month) return -1;
    if (d1.day > d2.day) return 1;
    if (d1.day < d2.day) return -1;
    return 0;
}

// Cập nhật chi tiết hóa đơn và số lượng tồn kho
// Returns: 1 (success), 0 (vật tư ko tồn tại), -1 (ko đủ tồn kho để xuất), -2 (trùng hoặc đầy)
int lapCTHD(HOADON &hd, CT_HOADON cthd, TreeVatTu &t) {
    if (hd.dscthd.n >= 20) return -2;
    if (searchCTHD(hd.dscthd, cthd.MAVT) != -1) return -2;

    NodeVatTu* pVatTu = searchVatTu(t, cthd.MAVT);
    if (pVatTu == NULL) return 0;

    if (hd.Loai == 'X' || hd.Loai == 'x') {
        if (pVatTu->info.SoLuongTon < cthd.Soluong) {
            return -1;
        }
        pVatTu->info.SoLuongTon -= cthd.Soluong;
    } else if (hd.Loai == 'N' || hd.Loai == 'n') {
        pVatTu->info.SoLuongTon += cthd.Soluong;
    }

    hd.dscthd.node[hd.dscthd.n] = cthd;
    hd.dscthd.n++;
    return 1;
}

// Xóa một chi tiết vật tư đang lập của hóa đơn hiện tại và tự động hoàn trả lại số lượng tồn kho
void xoaCTHDDangLap(HOADON &hd, int idx, TreeVatTu &t) {
    if (idx < 0 || idx >= hd.dscthd.n) return;
    
    CT_HOADON cthd = hd.dscthd.node[idx];
    NodeVatTu* pVatTu = searchVatTu(t, cthd.MAVT);
    if (pVatTu != NULL) {
        if (hd.Loai == 'X' || hd.Loai == 'x') {
            pVatTu->info.SoLuongTon += cthd.Soluong; // Hoàn lại kho nếu là Xuất
        } else if (hd.Loai == 'N' || hd.Loai == 'n') {
            pVatTu->info.SoLuongTon -= cthd.Soluong; // Trừ lại kho nếu là Nhập
        }
    }
    
    // Dồn mảng chi tiết
    for (int i = idx; i < hd.dscthd.n - 1; i++) {
        hd.dscthd.node[i] = hd.dscthd.node[i + 1];
    }
    hd.dscthd.n--;
}

// Hủy bỏ hoàn toàn hóa đơn đang lập và hoàn trả lại toàn bộ tồn kho cho các vật tư đã thêm
void huyHoaDonDangLap(HOADON &hd, TreeVatTu &t) {
    for (int i = 0; i < hd.dscthd.n; i++) {
        CT_HOADON cthd = hd.dscthd.node[i];
        NodeVatTu* pVatTu = searchVatTu(t, cthd.MAVT);
        if (pVatTu != NULL) {
            if (hd.Loai == 'X' || hd.Loai == 'x') {
                pVatTu->info.SoLuongTon += cthd.Soluong; // Hoàn lại kho
            } else if (hd.Loai == 'N' || hd.Loai == 'n') {
                pVatTu->info.SoLuongTon -= cthd.Soluong; // Trừ lại kho
            }
        }
    }
    hd.dscthd.n = 0;
}


// Tìm Hóa đơn trên toàn hệ thống
bool timHoaDonToanHeThong(const DanhSachNhanvien &dsnv, const char* soHD, int &out_idxNV, NodeHOADON* &out_nodeHD) {
    for (int i = 0; i < dsnv.n; i++) {
        NodeHOADON* cur = dsnv.node[i]->dshd;
        while (cur != NULL) {
            if (strcmp(cur->info.SoHD, soHD) == 0) {
                out_idxNV = i;
                out_nodeHD = cur;
                return true;
            }
            cur = cur->next;
        }
    }
    return false;
}

struct ThongKeDoanhThu {
    char MAVT[11];
    char TENVT[51];
    float DoanhThu;
};

// Tính top 10 doanh thu
void thongKeTop10(const DanhSachNhanvien &dsnv, Date tuNgay, Date denNgay, TreeVatTu t, ThongKeDoanhThu top10[10], int &out_count) {
    ThongKeDoanhThu arr[500]; 
    int count = 0;

    for (int i = 0; i < dsnv.n; i++) {
        NodeHOADON* cur = dsnv.node[i]->dshd;
        while (cur != NULL) {
            if ((cur->info.Loai == 'X' || cur->info.Loai == 'x') &&
                compareDate(cur->info.NgayLap, tuNgay) >= 0 &&
                compareDate(cur->info.NgayLap, denNgay) <= 0) {
                
                for (int k = 0; k < cur->info.dscthd.n; k++) {
                    CT_HOADON ct = cur->info.dscthd.node[k];
                    float dt = ct.Soluong * ct.Dongia;
                    
                    int idx = -1;
                    for (int j = 0; j < count; j++) {
                        if (strcmp(arr[j].MAVT, ct.MAVT) == 0) {
                            idx = j;
                            break;
                        }
                    }
                    if (idx != -1) {
                        arr[idx].DoanhThu += dt;
                    } else {
                        strcpy(arr[count].MAVT, ct.MAVT);
                        NodeVatTu* pVT = searchVatTu(t, ct.MAVT);
                        if (pVT != NULL) {
                            strcpy(arr[count].TENVT, pVT->info.TENVT);
                        } else {
                            strcpy(arr[count].TENVT, "Khong xac dinh");
                        }
                        arr[count].DoanhThu = dt;
                        count++;
                    }
                }
            }
            cur = cur->next;
        }
    }

    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (arr[i].DoanhThu < arr[j].DoanhThu) {
                ThongKeDoanhThu temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }

    out_count = (count > 10) ? 10 : count;
    for (int i = 0; i < out_count; i++) {
        top10[i] = arr[i];
    }
}
