#pragma once
#include "structures.h"
#include <iostream>

using namespace std;

// Khởi tạo cây
void initTreeVatTu(TreeVatTu &t) {
    t = NULL;
}

// Tạo node vật tư mới
NodeVatTu* createNodeVatTu(VatTu vt) {
    NodeVatTu* p = new NodeVatTu;
    if (p == NULL) return NULL;
    p->info = vt;
    p->left = NULL;
    p->right = NULL;
    return p;
}

// Thêm vật tư vào cây (theo MAVT)
void insertVatTu(TreeVatTu &t, VatTu vt) {
    if (t == NULL) {
        t = createNodeVatTu(vt);
    } else {
        if (strcmp(vt.MAVT, t->info.MAVT) < 0) {
            insertVatTu(t->left, vt);
        } else if (strcmp(vt.MAVT, t->info.MAVT) > 0) {
            insertVatTu(t->right, vt);
        }
    }
}

// Tìm kiếm vật tư theo mã
NodeVatTu* searchVatTu(TreeVatTu t, const char* MAVT) {
    if (t == NULL) return NULL;
    if (strcmp(MAVT, t->info.MAVT) == 0) return t;
    if (strcmp(MAVT, t->info.MAVT) < 0) return searchVatTu(t->left, MAVT);
    return searchVatTu(t->right, MAVT);
}

// Xóa vật tư theo mã
void findAndReplace(TreeVatTu &p, TreeVatTu &q) {
    if (q->left != NULL) {
        findAndReplace(p, q->left);
    } else {
        p->info = q->info;
        p = q;
        q = q->right;
    }
}

bool deleteVatTu(TreeVatTu &t, const char* MAVT) {
    if (t == NULL) return false;
    
    if (strcmp(MAVT, t->info.MAVT) < 0) {
        return deleteVatTu(t->left, MAVT);
    } else if (strcmp(MAVT, t->info.MAVT) > 0) {
        return deleteVatTu(t->right, MAVT);
    } else {
        NodeVatTu* p = t;
        if (t->left == NULL) {
            t = t->right;
        } else if (t->right == NULL) {
            t = t->left;
        } else {
            findAndReplace(p, t->right);
        }
        delete p;
        return true;
    }
}

// Hàm duyệt In-Order để thu thập danh sách vật tư (phục vụ việc chuyển sang mảng và sắp xếp)
void treeToArray(TreeVatTu t, VatTu* arr, int &n) {
    if (t != NULL) {
        treeToArray(t->left, arr, n);
        arr[n++] = t->info;
        treeToArray(t->right, arr, n);
    }
}

// Hàm đếm tổng số node trong cây BST vật tư (để cấp phát động mảng tránh tràn bộ đệm)
int countNodes(TreeVatTu t) {
    if (t == NULL) return 0;
    return 1 + countNodes(t->left) + countNodes(t->right);
}

// Kiểm tra xem vật tư có đang được sử dụng trong hóa đơn nào trên toàn hệ thống không
bool isVatTuReferenced(const DanhSachNhanvien &dsnv, const char* MAVT) {
    for (int i = 0; i < dsnv.n; i++) {
        NodeHOADON* cur = dsnv.node[i]->dshd;
        while (cur != NULL) {
            for (int k = 0; k < cur->info.dscthd.n; k++) {
                if (strcmp(cur->info.dscthd.node[k].MAVT, MAVT) == 0) {
                    return true; // Đang được tham chiếu trong chi tiết hóa đơn
                }
            }
            cur = cur->next;
        }
    }
    return false;
}

// Cập nhật thông tin vật tư (Không cho phép hiệu chỉnh Số lượng tồn ở đây)
bool updateVatTu(TreeVatTu t, const char* MAVT, const char* TENVT, const char* DVT) {
    NodeVatTu* p = searchVatTu(t, MAVT);
    if (p == NULL) return false;
    strcpy(p->info.TENVT, TENVT);
    strcpy(p->info.DVT, DVT);
    return true;
}

// Sắp xếp mảng vật tư theo Tên tăng dần
void sortVatTuByName(VatTu arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (compareNoCase(arr[i].TENVT, arr[j].TENVT) > 0) {
                VatTu temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
    }
}

