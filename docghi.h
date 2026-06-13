#pragma once
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include "structures.h"
#include "vattu.h"
#include "nhanvien.h"
#include "hoadon.h"

using namespace std;

// ================= VẬT TƯ =================

void docVatTu(TreeVatTu &t) {
    ifstream inFile("vattu.txt");
    if (!inFile.is_open()) return;
    
    // Xóa header (nếu có), ở đây tôi giả định ghi thẳng Data
    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;

        VatTu vt;
        // stringstream ko xử lý tốt với khoảng trắng nếu dùng >>, nên dùng custom split cho chắc chắn
        // MAVT|TENVT|DVT|SoLuongTon
        stringstream ss(line);
        string item;
        
        getline(ss, item, '|'); strcpy(vt.MAVT, item.c_str());
        getline(ss, item, '|'); strcpy(vt.TENVT, item.c_str());
        getline(ss, item, '|'); strcpy(vt.DVT, item.c_str());
        getline(ss, item, '|'); vt.SoLuongTon = stof(item);
        
        insertVatTu(t, vt);
    }
    inFile.close();
}

void ghiVatTuGPL(NodeVatTu* t, ofstream &out) {
    if (t != NULL) {
        ghiVatTuGPL(t->left, out);
        out << t->info.MAVT << "|"
            << t->info.TENVT << "|"
            << t->info.DVT << "|"
            << t->info.SoLuongTon << endl;
        ghiVatTuGPL(t->right, out);
    }
}

void ghiVatTu(TreeVatTu t) {
    ofstream outFile("vattu.txt");
    if (!outFile.is_open()) return;
    ghiVatTuGPL(t, outFile);
    outFile.close();
}

// ================= NHÂN VIÊN =================

void docNhanvien(DanhSachNhanvien &ds) {
    ifstream inFile("nhanvien.txt");
    if (!inFile.is_open()) return;

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        Nhanvien nv;
        stringstream ss(line);
        string item;

        getline(ss, item, '|'); nv.MANV = stoi(item);
        getline(ss, item, '|'); strcpy(nv.HO, item.c_str());
        getline(ss, item, '|'); strcpy(nv.TEN, item.c_str());
        getline(ss, item, '|'); strcpy(nv.PHAI, item.c_str());
        nv.dshd = NULL; // Initially no invoices

        insertNhanvien(ds, nv);
    }
    inFile.close();
}

void ghiNhanvien(const DanhSachNhanvien &ds) {
    ofstream outFile("nhanvien.txt");
    if (!outFile.is_open()) return;

    for (int i = 0; i < ds.n; i++) {
        outFile << ds.node[i]->MANV << "|"
                << ds.node[i]->HO << "|"
                << ds.node[i]->TEN << "|"
                << ds.node[i]->PHAI << endl;
    }
    outFile.close();
}

// ================= HÓA ĐƠN =================

void docHOADON(DanhSachNhanvien &dsnv) {
    ifstream inFile("hoadon.txt");
    if (!inFile.is_open()) return;

    string line;
    while (getline(inFile, line)) {
        if (line.empty()) continue;
        
        // MANV|SoHD|Day|Month|Year|Loai|SoLuongCTHD
        stringstream ss(line);
        string item;
        int MANV;
        HOADON hd;
        int ctCount;

        getline(ss, item, '|'); MANV = stoi(item);
        getline(ss, item, '|'); strcpy(hd.SoHD, item.c_str());
        getline(ss, item, '|'); hd.NgayLap.day = stoi(item);
        getline(ss, item, '|'); hd.NgayLap.month = stoi(item);
        getline(ss, item, '|'); hd.NgayLap.year = stoi(item);
        getline(ss, item, '|'); hd.Loai = item[0];
        getline(ss, item, '|'); ctCount = stoi(item);

        hd.dscthd.n = 0; // Init
        
        for (int i = 0; i < ctCount; i++) {
            getline(inFile, line);
            stringstream ssCT(line);
            CT_HOADON cthd;
            
            getline(ssCT, item, '|'); strcpy(cthd.MAVT, item.c_str());
            getline(ssCT, item, '|'); cthd.Soluong = stof(item);
            getline(ssCT, item, '|'); cthd.Dongia = stof(item);
            getline(ssCT, item, '|'); cthd.VAT = stof(item);
            
            insertCTHD(hd.dscthd, cthd);
        }

        // Tìm nhân viên để trỏ vào
        int idx = searchNhanvien(dsnv, MANV);
        if (idx != -1) {
            insertHOADONTail(dsnv.node[idx]->dshd, hd);
        }
    }
    inFile.close();
}

void ghiHOADON(const DanhSachNhanvien &dsnv) {
    ofstream outFile("hoadon.txt");
    if (!outFile.is_open()) return;

    for (int i = 0; i < dsnv.n; i++) {
        NodeHOADON* hd = dsnv.node[i]->dshd;
        while (hd != NULL) {
            outFile << dsnv.node[i]->MANV << "|"
                    << hd->info.SoHD << "|"
                    << hd->info.NgayLap.day << "|"
                    << hd->info.NgayLap.month << "|"
                    << hd->info.NgayLap.year << "|"
                    << hd->info.Loai << "|"
                    << hd->info.dscthd.n << endl;
            
            for (int k = 0; k < hd->info.dscthd.n; k++) {
                outFile << hd->info.dscthd.node[k].MAVT << "|"
                        << hd->info.dscthd.node[k].Soluong << "|"
                        << hd->info.dscthd.node[k].Dongia << "|"
                        << hd->info.dscthd.node[k].VAT << endl;
            }
            hd = hd->next;
        }
    }
    outFile.close();
}

// Gọi hàm trung tâm để đọc/ghi mọi thứ
void loadData(TreeVatTu &t, DanhSachNhanvien &dsnv) {
    docVatTu(t);
    docNhanvien(dsnv);
    docHOADON(dsnv);
}

void saveData(TreeVatTu t, const DanhSachNhanvien &dsnv) {
    ghiVatTu(t);
    ghiNhanvien(dsnv);
    ghiHOADON(dsnv);
}
