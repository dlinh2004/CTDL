#pragma once
#include <vector>
#include <algorithm>
#include <string.h>
#include <ctype.h>
#include "raylib.h"
#include "structures.h"
#include "vattu.h"
#include "nhanvien.h"
#include "hoadon.h"
#include "gui.h"
#include "docghi.h"

// ============================================================================
// CÁC CẤU TRÚC PHỤ TRỢ & QUẢN LÝ TRẠNG THÁI GIAO DIỆN (UI STATE)
// ============================================================================

// Cấu trúc dữ liệu trung gian dùng để chứa thông tin hiển thị trong bảng thống kê hóa đơn
struct ThongKeHDDong {
    char SoHD[21];
    Date NgayLap;
    char Loai;
    char HoTenNV[100];
    float TriGia;
};

// Lớp/Cấu trúc quản lý toàn bộ các biến giao diện, ô nhập liệu và các cờ trạng thái
struct UIState {
    bool exitApp = false; // Cờ thoát chương trình
    
    // Các biến phân trang (Pagination) để tránh tràn bảng
    int pageVT = 0;
    int pageNV = 0;
    int pageTK = 0;
    
    // Dòng đang được lựa chọn trong các bảng danh sách
    int selectedVT = -1;
    int selectedNV = -1;
    int selectedEmployeeIndex = -1; // Nhân viên được chọn để lập hóa đơn
    
    // Các bộ nhớ đệm (buffer) dùng cho các textbox nhập dữ liệu Vật Tư & Nhân Viên
    char text1[100] = ""; bool act1 = false; // MAVT hoặc MANV
    char text2[100] = ""; bool act2 = false; // TENVT hoặc HO
    char text3[100] = ""; bool act3 = false; // DVT hoặc TEN
    char text4[100] = ""; bool act4 = false; // SoLuongTon hoặc PHAI
    
    // Các bộ nhớ đệm dùng cho form thông tin chung Hóa Đơn
    char hdSoHD[50] = ""; bool actSoHD = false;
    char hdDay[10] = ""; bool actDay = false;
    char hdMonth[10] = ""; bool actMonth = false;
    char hdYear[10] = ""; bool actYear = false;
    char hdLoai[10] = ""; bool actLoai = false;
    char hdMANV[50] = ""; bool actMANV = false;
    
    // Các bộ nhớ đệm dùng để nhập chi tiết vật tư hóa đơn (CT_HOADON)
    char ctMAVT[50] = ""; bool actCtMAVT = false;
    char ctSoluong[50] = ""; bool actCtQty = false;
    char ctDongia[50] = ""; bool actCtPrice = false;
    char ctVAT[50] = ""; bool actCtVAT = false;
    
    // Các bộ nhớ đệm dùng cho bộ lọc tìm kiếm & thống kê
    char searchSoHD[50] = ""; bool actSearchSoHD = false;
    char fromDay[10] = "", fromMonth[10] = "", fromYear[10] = "";
    bool actFD = false, actFM = false, actFY = false;
    char toDay[10] = "", toMonth[10] = "", toYear[10] = "";
    bool actTD = false, actTM = false, actTY = false;
    
    // Biến điều khiển Hộp thoại Popup Dialog thông báo lỗi/thành công
    bool showDialog = false;
    char dialogTitle[100] = "";
    char dialogMsg[256] = "";
    
    // Trạng thái hóa đơn đang lập
    bool creatingInvoice = false;
    HOADON tempInvoice; // Hóa đơn tạm thời đang chứa các chi tiết trước khi ghi nhận
    
    // Kết quả tìm kiếm hóa đơn chi tiết
    bool showInvoiceResult = false;
    HOADON foundInvoice;
    char foundEmpName[100] = "";
    
    // Danh sách các hóa đơn đã lọc theo khoảng thời gian
    std::vector<ThongKeHDDong> tkInvoices;
    
    // Hàm xóa sạch các ô nhập dữ liệu khi chuyển tab hoặc ghi nhận dữ liệu xong
    void resetInputs() {
        text1[0] = '\0'; act1 = false;
        text2[0] = '\0'; act2 = false;
        text3[0] = '\0'; act3 = false;
        text4[0] = '\0'; act4 = false;
        
        hdSoHD[0] = '\0'; actSoHD = false;
        hdDay[0] = '\0'; actDay = false;
        hdMonth[0] = '\0'; actMonth = false;
        hdYear[0] = '\0'; actYear = false;
        hdLoai[0] = '\0'; actLoai = false;
        hdMANV[0] = '\0'; actMANV = false;
        
        ctMAVT[0] = '\0'; actCtMAVT = false;
        ctSoluong[0] = '\0'; actCtQty = false;
        ctDongia[0] = '\0'; actCtPrice = false;
        ctVAT[0] = '\0'; actCtVAT = false;
        
        selectedVT = -1;
        selectedNV = -1;
        selectedEmployeeIndex = -1;
        creatingInvoice = false;
        showInvoiceResult = false;
        
        tkInvoices.clear();
    }
};

// Hàm kiểm tra tính hợp lệ của ngày tháng năm
bool isValidDate(int d, int m, int y) {
    if (y < 1900 || y > 2100) return false;
    if (m < 1 || m > 12) return false;
    int daysInMonth[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    // Kiểm tra năm nhuận
    if ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0)) {
        daysInMonth[1] = 29;
    }
    if (d < 1 || d > daysInMonth[m - 1]) return false;
    return true;
}

// ============================================================================
// HÀM VẼ GIAO DIỆN THANH MENU BÊN TRÁI (SIDEBAR)
// ============================================================================

void drawSidebar(int &state, UIState &ui) {
    // Vẽ dải Sidebar bên trái màu Navy đậm rất sang trọng, thích ứng chiều cao cửa sổ
    DrawRectangle(0, 0, 280, GetScreenHeight(), COLOR_SECONDARY);
    
    // Tên ứng dụng (Tăng size lên 26)
    DrawText("QUẢN LÝ KHO VẬT TƯ", 20, 25, 26, COLOR_ACCENT);
    DrawLine(15, 70, 265, 70, COLOR_BORDER);
    
    // Các nhãn nút bấm trên Sidebar
    const char* tabs[] = {
        "Trang Chủ Dashboard",
        "Quản Lý Vật Tư",
        "Quản Lý Nhân Viên",
        "Lập Hóa Đơn (N/X)",
        "In Hóa Đơn Chi Tiết",
        "Thống Kê Hóa Đơn",
        "Top 10 Doanh Thu VT",
        "Thoát & Lưu Dữ Liệu"
    };
    
    // Mã trạng thái tương ứng
    int states[] = {0, 1, 2, 3, 4, 5, 6, 0}; 
    
    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < 8; i++) {
        // Tăng chiều cao nút lên 48 và khoảng cách giữa các nút lên 62
        Rectangle btnRec = { 15, 100 + (float)i * 62, 250, 48 };
        bool isActiveTab = (state == states[i]) && (i != 7); // Không highlight active cho nút Thoát
        
        Color btnCol = isActiveTab ? COLOR_ACCENT : COLOR_SECONDARY;
        
        // Hiệu ứng Hover đổi màu
        if (CheckCollisionPointRec(mouse, btnRec) && !isActiveTab && !guiDialogActive) {
            btnCol = COLOR_PRIMARY;
        }
        
        DrawRectangleRounded(btnRec, 0.15f, 4, btnCol);
        // Tăng size chữ menu từ 18 lên 22 để người lớn tuổi dễ nhìn
        int textW = MeasureText(tabs[i], 22);
        DrawText(tabs[i], btnRec.x + 15, btnRec.y + (btnRec.height - 22)/2, 22, RAYWHITE);
        
        if (CheckCollisionPointRec(mouse, btnRec) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !guiDialogActive) {
            if (i == 7) {
                ui.exitApp = true; // Kích hoạt cờ thoát và lưu file ở main
            } else {
                state = states[i];
                ui.resetInputs(); // Reset toàn bộ input tránh nhiễu dữ liệu tab khác
            }
        }
    }
    
    // Phiên bản chương trình ở góc dưới thích ứng chiều cao cửa sổ
    DrawText("Đồ án CTDL & GT © 2026", 20, GetScreenHeight() - 38, 18, COLOR_TEXT_MUTED);
}

// ============================================================================
// HÀM VẼ GIAO DIỆN TRANG CHỦ (DASHBOARD)
// ============================================================================

void drawDashboard(TreeVatTu t, const DanhSachNhanvien &dsnv) {
    // Thích ứng toàn màn hình
    float contentWidth = GetScreenWidth() - 280;
    float contentHeight = GetScreenHeight();
    DrawRectangle(280, 0, contentWidth, contentHeight, COLOR_BG);
    // Tăng size chữ tiêu đề trang lên 36
    DrawText("TỔNG QUAN HỆ THỐNG", 310, 25, 36, COLOR_PRIMARY);
    
    // Banner chào mừng thích ứng chiều ngang
    Rectangle banner = { 310, 75, contentWidth - 60, 150 };
    DrawRectangleRounded(banner, 0.05f, 4, COLOR_PRIMARY);
    
    // Tự động scale font chữ banner để không bao giờ tràn khung khi thu nhỏ cửa sổ
    const char* bannerTitle = "PHẦN MỀM QUẢN LÝ NHẬP XUẤT KHO VẬT TƯ";
    int titleFontSize = 30;
    while (titleFontSize > 16 && MeasureText(bannerTitle, titleFontSize) > banner.width - 80) {
        titleFontSize--;
    }
    DrawText(bannerTitle, banner.x + 40, banner.y + 35, titleFontSize, RAYWHITE);
    
    const char* bannerDesc = "Ứng dụng quản lý Vật tư (BST), Nhân viên (Mảng con trỏ), Hóa đơn (Danh sách liên kết đơn)";
    int descFontSize = 20;
    while (descFontSize > 12 && MeasureText(bannerDesc, descFontSize) > banner.width - 80) {
        descFontSize--;
    }
    DrawText(bannerDesc, banner.x + 40, banner.y + 90, descFontSize, COLOR_BORDER);
    
    // Thống kê nhanh dữ liệu hiện có
    int countVT = countNodes(t);
    int countNV = dsnv.n;
    int countHD = 0;
    for (int i = 0; i < dsnv.n; i++) {
        NodeHOADON* cur = dsnv.node[i]->dshd;
        while (cur != NULL) {
            countHD++;
            cur = cur->next;
        }
    }
    
    // Tính toán kích thước 3 card thống kê co giãn đều theo màn hình
    float availWidth = contentWidth - 60;
    float cardWidth = (availWidth - 40) / 3.0f;
    float cardHeight = 150; // Tăng lên 150
    float cardY = 255;
    
    // Card 1: Vật tư
    Rectangle card1 = { 310, cardY, cardWidth, cardHeight };
    DrawRectangleRounded(card1, 0.06f, 4, COLOR_CARD);
    DrawRectangleRoundedLines(card1, 0.06f, 4, COLOR_BORDER);
    DrawRectangleRec({ card1.x, card1.y, 8, card1.height }, COLOR_ACCENT);
    DrawText("TỔNG SỐ VẬT TƯ", card1.x + 20, card1.y + 20, 18, COLOR_TEXT_MUTED); // Tăng lên 18
    DrawText(TextFormat("%d", countVT), card1.x + 20, card1.y + 55, 52, COLOR_PRIMARY); // Tăng lên 52
    
    // Card 2: Nhân viên
    Rectangle card2 = { 310 + cardWidth + 20, cardY, cardWidth, cardHeight };
    DrawRectangleRounded(card2, 0.06f, 4, COLOR_CARD);
    DrawRectangleRoundedLines(card2, 0.06f, 4, COLOR_BORDER);
    DrawRectangleRec({ card2.x, card2.y, 8, card2.height }, COLOR_ACCENT);
    DrawText("TỔNG NHÂN VIÊN", card2.x + 20, card2.y + 20, 18, COLOR_TEXT_MUTED);
    DrawText(TextFormat("%d", countNV), card2.x + 20, card2.y + 55, 52, COLOR_PRIMARY);
    
    // Card 3: Hóa đơn
    Rectangle card3 = { 310 + 2 * (cardWidth + 20), cardY, cardWidth, cardHeight };
    DrawRectangleRounded(card3, 0.06f, 4, COLOR_CARD);
    DrawRectangleRoundedLines(card3, 0.06f, 4, COLOR_BORDER);
    DrawRectangleRec({ card3.x, card3.y, 8, card3.height }, COLOR_ACCENT);
    DrawText("HÓA ĐƠN ĐÃ LẬP", card3.x + 20, card3.y + 20, 18, COLOR_TEXT_MUTED);
    DrawText(TextFormat("%d", countHD), card3.x + 20, card3.y + 55, 52, COLOR_PRIMARY);
    
    // Khung hướng dẫn sử dụng phần mềm co giãn cả đứng và ngang
    DrawText("HƯỚNG DẪN SỬ DỤNG NHANH", 310, 435, 24, COLOR_PRIMARY); // Tăng lên 24
    Rectangle guideBox = { 310, 470, availWidth, contentHeight - 470 - 30 };
    DrawRectangleRounded(guideBox, 0.03f, 4, COLOR_CARD);
    DrawRectangleRoundedLines(guideBox, 0.03f, 4, COLOR_BORDER);
    
    // Tăng toàn bộ dòng chữ hướng dẫn lên size 20 và chia dòng ngắn hơn để tránh tràn khi thu nhỏ
    DrawText("- QUẢN LÝ VẬT TƯ: Cập nhật (Thêm/Xóa/Sửa) thông tin vật tư.", 330, 495, 20, COLOR_TEXT_MAIN);
    DrawText("  Số lượng tồn chỉ cho phép nhập khi thêm mới vật tư.", 330, 520, 20, COLOR_TEXT_MAIN);
    
    DrawText("- QUẢN LÝ NHÂN VIÊN: Cập nhật thông tin nhân viên.", 330, 555, 20, COLOR_TEXT_MAIN);
    DrawText("  Danh sách luôn tự động sắp xếp tăng dần theo tên.", 330, 580, 20, COLOR_TEXT_MAIN);
    
    DrawText("- LẬP HÓA ĐƠN: Nhập chi tiết vật tư xuất hoặc nhập kho.", 330, 615, 20, COLOR_TEXT_MAIN);
    DrawText("  Tự động cập nhật số lượng tồn kho và kiểm tra tồn khi xuất.", 330, 640, 20, COLOR_TEXT_MAIN);
    
    DrawText("- IN HÓA ĐƠN & THỐNG KÊ: In hóa đơn chi tiết theo Số HĐ.", 330, 680, 20, COLOR_TEXT_MAIN);
    DrawText("  Thống kê danh sách hóa đơn và Top 10 doanh số theo thời gian.", 330, 715, 20, COLOR_TEXT_MAIN);
}

// ============================================================================
// HÀM VẼ GIAO DIỆN BẢNG VÀ QUẢN LÝ VẬT TƯ (MATERIALS CRUD)
// ============================================================================

void drawTableMaterials(VatTu* arr, int count, UIState &ui, float tableWidth) {
    int startY = 100;
    int rowHeight = 45; // Tăng chiều cao dòng lên 45 để tạo không gian cho chữ to
    
    // Cố định hiển thị 10 vật tư trên mỗi trang để giao diện đồng bộ
    int rowsPerPage = 10;
    
    int totalPages = (count + rowsPerPage - 1) / rowsPerPage;
    if (totalPages == 0) totalPages = 1;
    if (ui.pageVT >= totalPages) ui.pageVT = totalPages - 1;
    
    // Định nghĩa các cột theo tỷ lệ phần trăm của độ rộng bảng
    float col1 = 10;
    float col2 = tableWidth * 0.25f;
    float col3 = tableWidth * 0.65f;
    float col4 = tableWidth * 0.82f;
    
    // Vẽ tiêu đề cột bảng vật tư co giãn theo tableWidth, tăng size chữ lên 20
    Rectangle headerRec = { 310, (float)startY, tableWidth, 40 };
    DrawRectangleRec(headerRec, COLOR_SECONDARY);
    DrawTextLimit("Mã VT", headerRec.x + col1, headerRec.y + 10, 20, RAYWHITE, col2 - col1 - 5);
    DrawTextLimit("Tên Vật Tư", headerRec.x + col2, headerRec.y + 10, 20, RAYWHITE, col3 - col2 - 10);
    DrawTextLimit("ĐVT", headerRec.x + col3, headerRec.y + 10, 20, RAYWHITE, col4 - col3 - 5);
    DrawTextLimit("Số lượng tồn", headerRec.x + col4, headerRec.y + 10, 20, RAYWHITE, tableWidth - col4 - 10);
    
    int startIdx = ui.pageVT * rowsPerPage;
    int endIdx = startIdx + rowsPerPage;
    if (endIdx > count) endIdx = count;
    
    Vector2 mouse = GetMousePosition();
    for (int i = startIdx; i < endIdx; i++) {
        int rowY = startY + 40 + (i - startIdx) * rowHeight;
        Rectangle rowRec = { 310, (float)rowY, tableWidth, (float)rowHeight };
        
        bool isSelected = (ui.selectedVT == i);
        bool isHovered = CheckCollisionPointRec(mouse, rowRec) && !guiDialogActive;
        
        Color bgCol = isSelected ? CLITERAL(Color){ 204, 251, 241, 255 } : (i % 2 == 0 ? COLOR_CARD : COLOR_BG);
        if (isHovered && !isSelected) bgCol = COLOR_BORDER;
        
        DrawRectangleRec(rowRec, bgCol);
        DrawRectangleLinesEx(rowRec, 0.5f, COLOR_BORDER);
        
        // Vẽ chữ hàng dữ liệu size 20 cực rõ nét có giới hạn độ rộng để tránh đè chữ
        DrawTextLimit(arr[i].MAVT, rowRec.x + col1, rowRec.y + 12, 20, COLOR_TEXT_MAIN, col2 - col1 - 5);
        DrawTextLimit(arr[i].TENVT, rowRec.x + col2, rowRec.y + 12, 20, COLOR_TEXT_MAIN, col3 - col2 - 10);
        DrawTextLimit(arr[i].DVT, rowRec.x + col3, rowRec.y + 12, 20, COLOR_TEXT_MAIN, col4 - col3 - 5);
        DrawTextLimit(TextFormat("%.2f", arr[i].SoLuongTon), rowRec.x + col4, rowRec.y + 12, 20, COLOR_TEXT_MAIN, tableWidth - col4 - 10);
        
        if (isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !guiDialogActive) {
            ui.selectedVT = i;
            strcpy(ui.text1, arr[i].MAVT);
            strcpy(ui.text2, arr[i].TENVT);
            strcpy(ui.text3, arr[i].DVT);
            sprintf(ui.text4, "%.2f", arr[i].SoLuongTon);
        }
    }
    
    // Thanh phân trang Y động
    float controlsY = startY + 40 + rowsPerPage * rowHeight + 10;
    
    char pageText[50];
    sprintf(pageText, "Trang %d / %d", ui.pageVT + 1, totalPages);
    DrawText(pageText, 310 + tableWidth/2 - 50, controlsY + 8, 20, COLOR_TEXT_MUTED); // Size 20
    
    if (DrawButton({ 310, controlsY, 110, 36 }, "Trước")) {
        if (ui.pageVT > 0) ui.pageVT--;
    }
    if (DrawButton({ 310 + tableWidth - 110, controlsY, 110, 36 }, "Sau")) {
        if (ui.pageVT < totalPages - 1) ui.pageVT++;
    }
}

void drawVatTuScreen(TreeVatTu &t, const DanhSachNhanvien &dsnv, UIState &ui) {
    float contentWidth = GetScreenWidth() - 280;
    float contentHeight = GetScreenHeight();
    DrawRectangle(280, 0, contentWidth, contentHeight, COLOR_BG);
    DrawText("DANH MỤC VẬT TƯ TRONG KHO", 310, 25, 36, COLOR_PRIMARY); // Size 36
    
    int countVT = countNodes(t);
    VatTu* arrVT = new VatTu[countVT > 0 ? countVT : 1];
    int n = 0;
    treeToArray(t, arrVT, n);
    sortVatTuByName(arrVT, n);
    
    // Tính toán Form cố định bên phải (Rộng hơn chút), Bảng co giãn theo phần còn lại bên trái
    int formWidth = 280; // Tăng lên 280
    int formX = GetScreenWidth() - formWidth - 30;
    int formY = 100;
    
    float tableWidth = formX - 10 - 310;
    
    // Vẽ bảng vật tư
    drawTableMaterials(arrVT, n, ui, tableWidth);
    
    // Vẽ Form bên phải
    DrawRectangleRounded({ (float)formX - 10, (float)formY - 10, (float)formWidth + 20, 540 }, 0.04f, 4, COLOR_CARD);
    DrawRectangleRoundedLines({ (float)formX - 10, (float)formY - 10, (float)formWidth + 20, 540 }, 0.04f, 4, COLOR_BORDER);
    
    DrawText("THÔNG TIN VẬT TƯ", formX, formY, 22, COLOR_PRIMARY); // Size 22
    bool isEditing = (ui.selectedVT != -1);
    
    // Mã VT - Textbox cao 38px cho dễ dùng
    DrawText("Mã vật tư (tối đa 10 kí tự):", formX, formY + 40, 18, COLOR_TEXT_MUTED); // Size 18
    if (isEditing) {
        DrawRectangleRec({ (float)formX, (float)formY + 65, (float)formWidth, 38 }, COLOR_BG);
        DrawRectangleLinesEx({ (float)formX, (float)formY + 65, (float)formWidth, 38 }, 1, COLOR_BORDER);
        DrawText(ui.text1, formX + 8, formY + 65 + (38 - 20)/2, 20, COLOR_TEXT_MUTED);
    } else {
        DrawTextBox({ (float)formX, (float)formY + 65, (float)formWidth, 38 }, ui.text1, 10, &ui.act1, 0);
    }
    
    // Tên VT
    DrawText("Tên vật tư (tối đa 50 kí tự):", formX, formY + 120, 18, COLOR_TEXT_MUTED);
    DrawTextBox({ (float)formX, (float)formY + 145, (float)formWidth, 38 }, ui.text2, 50, &ui.act2, 0);
    
    // ĐVT
    DrawText("Đơn vị tính (tối đa 20 kí tự):", formX, formY + 200, 18, COLOR_TEXT_MUTED);
    DrawTextBox({ (float)formX, (float)formY + 225, (float)formWidth, 38 }, ui.text3, 20, &ui.act3, 0);
    
    // Số lượng tồn
    DrawText("Số lượng tồn:", formX, formY + 280, 18, COLOR_TEXT_MUTED);
    if (isEditing) {
        DrawRectangleRec({ (float)formX, (float)formY + 305, (float)formWidth, 38 }, COLOR_BG);
        DrawRectangleLinesEx({ (float)formX, (float)formY + 305, (float)formWidth, 38 }, 1, COLOR_BORDER);
        DrawText(ui.text4, formX + 8, formY + 305 + (38 - 20)/2, 20, COLOR_TEXT_MUTED);
    } else {
        DrawTextBox({ (float)formX, (float)formY + 305, (float)formWidth, 38 }, ui.text4, 10, &ui.act4, 2);
    }
    
    // Buttons cao 42px
    if (!isEditing) {
        if (DrawButton({ (float)formX, (float)formY + 370, (float)formWidth, 42 }, "Thêm Vật Tư Mới")) {
            trimString(ui.text1);
            trimString(ui.text2);
            trimString(ui.text3);
            trimString(ui.text4);
            formatName(ui.text2); // Tự động định dạng Tên vật tư
            formatName(ui.text3); // Tự động định dạng Đơn vị tính
            if (isOnlySpaces(ui.text1) || isOnlySpaces(ui.text2) || isOnlySpaces(ui.text3)) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Lỗi Thêm Vật Tư");
                strcpy(ui.dialogMsg, "Không được để trống hoặc chỉ nhập\nkhoảng trắng cho Mã, Tên và Đơn vị!");
            } else if (searchVatTu(t, ui.text1) != NULL) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Lỗi Mã Vật Tư");
                strcpy(ui.dialogMsg, "Mã vật tư đã tồn tại trong kho!");
            } else {
                VatTu vt;
                strcpy(vt.MAVT, ui.text1);
                strcpy(vt.TENVT, ui.text2);
                strcpy(vt.DVT, ui.text3);
                vt.SoLuongTon = atof(ui.text4);
                if (vt.SoLuongTon < 0) vt.SoLuongTon = 0;
                
                insertVatTu(t, vt);
                saveData(t, dsnv);
                ui.resetInputs();
            }
        }
    } else {
        if (DrawButton({ (float)formX, (float)formY + 370, (float)formWidth, 42 }, "Lưu Thay Đổi")) {
            trimString(ui.text2);
            trimString(ui.text3);
            formatName(ui.text2); // Tự động định dạng Tên vật tư
            formatName(ui.text3); // Tự động định dạng Đơn vị tính
            if (isOnlySpaces(ui.text2) || isOnlySpaces(ui.text3)) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Lỗi Hiệu Chỉnh");
                strcpy(ui.dialogMsg, "Tên vật tư và Đơn vị tính không được rỗng!");
            } else {
                updateVatTu(t, ui.text1, ui.text2, ui.text3);
                saveData(t, dsnv);
                ui.resetInputs();
            }
        }
        
        if (DrawButton({ (float)formX, (float)formY + 420, (float)formWidth, 42 }, "Xóa Vật Tư", RED)) {
            if (isVatTuReferenced(dsnv, ui.text1)) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Lỗi Ràng Buộc Hóa Đơn");
                strcpy(ui.dialogMsg, "Không được xóa vật tư này vì nó đã\ntồn tại trong các hóa đơn bán lẻ!");
            } else {
                deleteVatTu(t, ui.text1);
                saveData(t, dsnv);
                ui.resetInputs();
            }
        }
        
        if (DrawButton({ (float)formX, (float)formY + 470, (float)formWidth, 42 }, "Bỏ chọn (Thêm mới)", COLOR_TEXT_MUTED)) {
            ui.resetInputs();
        }
    }
    
    delete[] arrVT;
}
void drawTableEmployees(Nhanvien* arr[], int count, UIState &ui, float tableWidth) {
    int startY = 100;
    int rowHeight = 45; // Cao 45px
    
    // Cố định hiển thị 10 nhân viên trên mỗi trang để giao diện đồng bộ
    int rowsPerPage = 10;
    
    int totalPages = (count + rowsPerPage - 1) / rowsPerPage;
    if (totalPages == 0) totalPages = 1;
    if (ui.pageNV >= totalPages) ui.pageNV = totalPages - 1;
    
    // Định nghĩa các cột theo tỷ lệ phần trăm
    float col1 = 10;
    float col2 = tableWidth * 0.25f;
    float col3 = tableWidth * 0.70f;
    float col4 = tableWidth * 0.88f;
    
    // Cột header nhân viên
    Rectangle headerRec = { 310, (float)startY, tableWidth, 40 };
    DrawRectangleRec(headerRec, COLOR_SECONDARY);
    DrawTextLimit("Mã NV", headerRec.x + col1, headerRec.y + 10, 20, RAYWHITE, col2 - col1 - 5);
    DrawTextLimit("Họ", headerRec.x + col2, headerRec.y + 10, 20, RAYWHITE, col3 - col2 - 10);
    DrawTextLimit("Tên", headerRec.x + col3, headerRec.y + 10, 20, RAYWHITE, col4 - col3 - 5);
    DrawTextLimit("Phái", headerRec.x + col4, headerRec.y + 10, 20, RAYWHITE, tableWidth - col4 - 10);
    
    int startIdx = ui.pageNV * rowsPerPage;
    int endIdx = startIdx + rowsPerPage;
    if (endIdx > count) endIdx = count;
    
    Vector2 mouse = GetMousePosition();
    for (int i = startIdx; i < endIdx; i++) {
        int rowY = startY + 40 + (i - startIdx) * rowHeight;
        Rectangle rowRec = { 310, (float)rowY, tableWidth, (float)rowHeight };
        
        bool isSelected = (ui.selectedNV == i);
        bool isHovered = CheckCollisionPointRec(mouse, rowRec) && !guiDialogActive;
        
        Color bgCol = isSelected ? CLITERAL(Color){ 204, 251, 241, 255 } : (i % 2 == 0 ? COLOR_CARD : COLOR_BG);
        if (isHovered && !isSelected) bgCol = COLOR_BORDER;
        
        DrawRectangleRec(rowRec, bgCol);
        DrawRectangleLinesEx(rowRec, 0.5f, COLOR_BORDER);
        
        DrawTextLimit(TextFormat("%d", arr[i]->MANV), rowRec.x + col1, rowRec.y + 12, 20, COLOR_TEXT_MAIN, col2 - col1 - 5);
        DrawTextLimit(arr[i]->HO, rowRec.x + col2, rowRec.y + 12, 20, COLOR_TEXT_MAIN, col3 - col2 - 10);
        DrawTextLimit(arr[i]->TEN, rowRec.x + col3, rowRec.y + 12, 20, COLOR_TEXT_MAIN, col4 - col3 - 5);
        DrawTextLimit(arr[i]->PHAI, rowRec.x + col4, rowRec.y + 12, 20, COLOR_TEXT_MAIN, tableWidth - col4 - 10);
        
        if (isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !guiDialogActive) {
            ui.selectedNV = i;
            sprintf(ui.text1, "%d", arr[i]->MANV);
            strcpy(ui.text2, arr[i]->HO);
            strcpy(ui.text3, arr[i]->TEN);
            strcpy(ui.text4, arr[i]->PHAI);
        }
    }
    
    float controlsY = startY + 40 + rowsPerPage * rowHeight + 10;
    
    char pageText[50];
    sprintf(pageText, "Trang %d / %d", ui.pageNV + 1, totalPages);
    DrawText(pageText, 310 + tableWidth/2 - 50, controlsY + 8, 20, COLOR_TEXT_MUTED);
    
    if (DrawButton({ 310, controlsY, 110, 36 }, "Trước")) {
        if (ui.pageNV > 0) ui.pageNV--;
    }
    if (DrawButton({ 310 + tableWidth - 110, controlsY, 110, 36 }, "Sau")) {
        if (ui.pageNV < totalPages - 1) ui.pageNV++;
    }
}

void drawNhanvienScreen(DanhSachNhanvien &ds, TreeVatTu t, UIState &ui) {
    float contentWidth = GetScreenWidth() - 280;
    float contentHeight = GetScreenHeight();
    DrawRectangle(280, 0, contentWidth, contentHeight, COLOR_BG);
    DrawText("DANH MỤC NHÂN VIÊN CỦA HỆ THỐNG", 310, 25, 36, COLOR_PRIMARY);
    
    Nhanvien* sortedNV[500];
    getSortedNhanvienList(ds, sortedNV);
    
    int formWidth = 280; // Tăng lên 280
    int formX = GetScreenWidth() - formWidth - 30;
    int formY = 100;
    
    float tableWidth = formX - 10 - 310;
    
    // In danh sách nhân viên dạng bảng co giãn
    drawTableEmployees(sortedNV, ds.n, ui, tableWidth);
    
    // Khung form cập nhật bên phải
    DrawRectangleRounded({ (float)formX - 10, (float)formY - 10, (float)formWidth + 20, 540 }, 0.04f, 4, COLOR_CARD);
    DrawRectangleRoundedLines({ (float)formX - 10, (float)formY - 10, (float)formWidth + 20, 540 }, 0.04f, 4, COLOR_BORDER);
    
    DrawText("THÔNG TIN NHÂN VIÊN", formX, formY, 22, COLOR_PRIMARY);
    bool isEditing = (ui.selectedNV != -1);
    
    // Mã NV
    DrawText("Mã nhân viên (chỉ nhập số):", formX, formY + 40, 18, COLOR_TEXT_MUTED);
    if (isEditing) {
        DrawRectangleRec({ (float)formX, (float)formY + 65, (float)formWidth, 38 }, COLOR_BG);
        DrawRectangleLinesEx({ (float)formX, (float)formY + 65, (float)formWidth, 38 }, 1, COLOR_BORDER);
        DrawText(ui.text1, formX + 8, formY + 65 + (38 - 20)/2, 20, COLOR_TEXT_MUTED);
    } else {
        DrawTextBox({ (float)formX, (float)formY + 65, (float)formWidth, 38 }, ui.text1, 8, &ui.act1, 1);
    }
    
    // Họ
    DrawText("Họ và chữ lót (tối đa 50 kí tự):", formX, formY + 120, 18, COLOR_TEXT_MUTED);
    DrawTextBox({ (float)formX, (float)formY + 145, (float)formWidth, 38 }, ui.text2, 50, &ui.act2, 0);
    
    // Tên
    DrawText("Tên nhân viên (tối đa 20 kí tự):", formX, formY + 200, 18, COLOR_TEXT_MUTED);
    DrawTextBox({ (float)formX, (float)formY + 225, (float)formWidth, 38 }, ui.text3, 20, &ui.act3, 0);
    
    // Phái
    DrawText("Phái (Nam hoặc Nữ):", formX, formY + 280, 18, COLOR_TEXT_MUTED);
    DrawTextBox({ (float)formX, (float)formY + 305, (float)formWidth, 38 }, ui.text4, 10, &ui.act4, 0);
    
    if (!isEditing) {
        if (DrawButton({ (float)formX, (float)formY + 370, (float)formWidth, 42 }, "Thêm Nhân Viên")) {
            trimString(ui.text1);
            trimString(ui.text2);
            trimString(ui.text3);
            trimString(ui.text4);
            formatName(ui.text2); // Tự động định dạng Họ
            formatName(ui.text3); // Tự động định dạng Tên
            if (isOnlySpaces(ui.text1) || isOnlySpaces(ui.text2) || isOnlySpaces(ui.text3) || isOnlySpaces(ui.text4)) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Lỗi Thêm Nhân Viên");
                strcpy(ui.dialogMsg, "Không được bỏ trống Mã, Họ, Tên, và Phái!");
            } else if (strcmp(ui.text4, "Nam") != 0 && strcmp(ui.text4, "Nữ") != 0 && strcmp(ui.text4, "Nu") != 0 && strcmp(ui.text4, "nam") != 0 && strcmp(ui.text4, "nữ") != 0 && strcmp(ui.text4, "nu") != 0) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Lỗi Phái Nhân Viên");
                strcpy(ui.dialogMsg, "Phái chỉ chấp nhận ký tự 'Nam' hoặc 'Nữ'!");
            } else {
                Nhanvien nv;
                nv.MANV = atoi(ui.text1);
                strcpy(nv.HO, ui.text2);
                strcpy(nv.TEN, ui.text3);
                
                if (strcmp(ui.text4, "Nam") == 0 || strcmp(ui.text4, "nam") == 0) {
                    strcpy(nv.PHAI, "Nam");
                } else {
                    strcpy(nv.PHAI, "Nữ");
                }
                nv.dshd = NULL;
                
                if (searchNhanvien(ds, nv.MANV) != -1) {
                    ui.showDialog = true;
                    strcpy(ui.dialogTitle, "Lỗi Mã Nhân Viên");
                    strcpy(ui.dialogMsg, "Mã nhân viên đã tồn tại trên hệ thống!");
                } else {
                    insertNhanvien(ds, nv);
                    saveData(t, ds);
                    ui.resetInputs();
                }
            }
        }
    } else {
        if (DrawButton({ (float)formX, (float)formY + 370, (float)formWidth, 42 }, "Lưu Thay Đổi")) {
            trimString(ui.text2);
            trimString(ui.text3);
            trimString(ui.text4);
            formatName(ui.text2); // Tự động định dạng Họ
            formatName(ui.text3); // Tự động định dạng Tên
            if (isOnlySpaces(ui.text2) || isOnlySpaces(ui.text3) || isOnlySpaces(ui.text4)) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Lỗi Cập Nhật");
                strcpy(ui.dialogMsg, "Họ, Tên và Phái không được để trống!");
            } else if (strcmp(ui.text4, "Nam") != 0 && strcmp(ui.text4, "Nữ") != 0 && strcmp(ui.text4, "Nu") != 0 && strcmp(ui.text4, "nam") != 0 && strcmp(ui.text4, "nữ") != 0 && strcmp(ui.text4, "nu") != 0) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Lỗi Phái Nhân Viên");
                strcpy(ui.dialogMsg, "Phái chỉ chấp nhận ký tự 'Nam' hoặc 'Nữ'!");
            } else {
                char cleanPhai[11];
                if (strcmp(ui.text4, "Nam") == 0 || strcmp(ui.text4, "nam") == 0) {
                    strcpy(cleanPhai, "Nam");
                } else {
                    strcpy(cleanPhai, "Nữ");
                }
                updateNhanvien(ds, atoi(ui.text1), ui.text2, ui.text3, cleanPhai);
                saveData(t, ds);
                ui.resetInputs();
            }
        }
        
        if (DrawButton({ (float)formX, (float)formY + 420, (float)formWidth, 42 }, "Xóa Nhân Viên", RED)) {
            int nvId = atoi(ui.text1);
            int idx = searchNhanvien(ds, nvId);
            if (idx != -1 && ds.node[idx]->dshd != NULL) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Lỗi Ràng Buộc Hóa Đơn");
                strcpy(ui.dialogMsg, "Không thể xóa nhân viên này vì họ đã\nphát sinh hóa đơn nhập xuất trong hệ thống!");
            } else {
                deleteNhanvien(ds, nvId);
                saveData(t, ds);
                ui.resetInputs();
            }
        }
        
        if (DrawButton({ (float)formX, (float)formY + 470, (float)formWidth, 42 }, "Bỏ chọn (Thêm mới)", COLOR_TEXT_MUTED)) {
            ui.resetInputs();
        }
    }
}

// ============================================================================
// HÀM VẼ GIAO DIỆN LẬP HÓA ĐƠN (INVOICE CREATION WORKFLOW)
// ============================================================================

void drawLapHoaDonScreen(TreeVatTu &t, DanhSachNhanvien &dsnv, UIState &ui) {
    float contentWidth = GetScreenWidth() - 280;
    float contentHeight = GetScreenHeight();
    DrawRectangle(280, 0, contentWidth, contentHeight, COLOR_BG);
    DrawText("LẬP HÓA ĐƠN NHẬP / XUẤT", 310, 25, 36, COLOR_PRIMARY);
    
    int formWidth = 320;
    int formX = GetScreenWidth() - formWidth - 30;
    int formY = 115;
    
    if (!ui.creatingInvoice) {
        // ---------------- BƯỚC 1: NHẬP THÔNG TIN CHUNG HÓA ĐƠN ----------------
        DrawText("Chọn nhân viên lập hóa đơn từ bảng bên dưới:", 310, 80, 20, COLOR_PRIMARY); // Size 20
        
        int startY = 115;
        int rowHeight = 42; // Tăng lên 42
        // Cố định hiển thị 10 dòng trên mỗi trang để giao diện đồng bộ
        int rowsPerPage = 10;
        
        int totalPages = (dsnv.n + rowsPerPage - 1) / rowsPerPage;
        if (totalPages == 0) totalPages = 1;
        if (ui.pageNV >= totalPages) ui.pageNV = totalPages - 1;
        
        float tableWidth = formX - 10 - 310;
        
        // Định nghĩa các cột theo tỷ lệ
        float col1 = 10;
        float col2 = tableWidth * 0.30f;
        
        Rectangle headerRec = { 310, (float)startY, tableWidth, 36 };
        DrawRectangleRec(headerRec, COLOR_SECONDARY);
        DrawTextLimit("Mã NV", headerRec.x + col1, headerRec.y + 8, 20, RAYWHITE, col2 - col1 - 5);
        DrawTextLimit("Họ Tên Nhân Viên", headerRec.x + col2, headerRec.y + 8, 20, RAYWHITE, tableWidth - col2 - 10);
        
        int startIdx = ui.pageNV * rowsPerPage;
        int endIdx = startIdx + rowsPerPage;
        if (endIdx > dsnv.n) endIdx = dsnv.n;
        
        Vector2 mouse = GetMousePosition();
        for (int i = startIdx; i < endIdx; i++) {
            int rowY = startY + 36 + (i - startIdx) * rowHeight;
            Rectangle rowRec = { 310, (float)rowY, tableWidth, (float)rowHeight };
            
            bool isSelected = (ui.selectedEmployeeIndex == i);
            bool isHovered = CheckCollisionPointRec(mouse, rowRec) && !guiDialogActive;
            
            Color bgCol = isSelected ? CLITERAL(Color){ 204, 251, 241, 255 } : (i % 2 == 0 ? COLOR_CARD : COLOR_BG);
            if (isHovered && !isSelected) bgCol = COLOR_BORDER;
            
            DrawRectangleRec(rowRec, bgCol);
            DrawRectangleLinesEx(rowRec, 0.5f, COLOR_BORDER);
            
            DrawTextLimit(TextFormat("%d", dsnv.node[i]->MANV), rowRec.x + col1, rowRec.y + 10, 20, COLOR_TEXT_MAIN, col2 - col1 - 5);
            DrawTextLimit(TextFormat("%s %s", dsnv.node[i]->HO, dsnv.node[i]->TEN), rowRec.x + col2, rowRec.y + 10, 20, COLOR_TEXT_MAIN, tableWidth - col2 - 10);
            
            if (isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !guiDialogActive) {
                ui.selectedEmployeeIndex = i;
                sprintf(ui.hdMANV, "%d", dsnv.node[i]->MANV);
            }
        }
        
        float controlsY = startY + 36 + rowsPerPage * rowHeight + 10;
        
        char pageText[50];
        sprintf(pageText, "Trang %d / %d", ui.pageNV + 1, totalPages);
        DrawText(pageText, 310 + tableWidth/2 - 50, controlsY + 8, 20, COLOR_TEXT_MUTED);
        
        if (DrawButton({ 310, controlsY, 100, 34 }, "Trước")) {
            if (ui.pageNV > 0) ui.pageNV--;
        }
        if (DrawButton({ 310 + tableWidth - 100, controlsY, 100, 34 }, "Sau")) {
            if (ui.pageNV < totalPages - 1) ui.pageNV++;
        }
        
        // Form nhập hóa đơn mới bên phải
        DrawRectangleRounded({ (float)formX - 10, (float)formY - 10, (float)formWidth + 20, 520 }, 0.03f, 4, COLOR_CARD);
        DrawRectangleRoundedLines({ (float)formX - 10, (float)formY - 10, (float)formWidth + 20, 520 }, 0.03f, 4, COLOR_BORDER);
        
        DrawText("THÔNG TIN HÓA ĐƠN", formX, formY, 22, COLOR_PRIMARY);
        
        DrawText("Nhân viên lập phiếu:", formX, formY + 40, 18, COLOR_TEXT_MUTED);
        if (ui.selectedEmployeeIndex != -1) {
            char nameBuf[128];
            sprintf(nameBuf, "%s %s (MANV: %s)", dsnv.node[ui.selectedEmployeeIndex]->HO, dsnv.node[ui.selectedEmployeeIndex]->TEN, ui.hdMANV);
            DrawText(nameBuf, formX, formY + 65, 20, COLOR_ACCENT); // Size 20
        } else {
            DrawText("Vui lòng click chọn nhân viên lập", formX, formY + 65, 18, RED);
        }
        
        // Số hóa đơn - Textbox cao 38px
        DrawText("Số hóa đơn (tối đa 20 kí tự):", formX, formY + 110, 18, COLOR_TEXT_MUTED);
        DrawTextBox({ (float)formX, (float)formY + 135, (float)formWidth, 38 }, ui.hdSoHD, 20, &ui.actSoHD, 0);
        
        // Ngày lập
        DrawText("Ngày lập hóa đơn (Ngày/Tháng/Năm):", formX, formY + 190, 18, COLOR_TEXT_MUTED);
        DrawTextBox({ (float)formX, (float)formY + 215, 65, 38 }, ui.hdDay, 2, &ui.actDay, 1);
        DrawText("/", formX + 75, formY + 220, 22, COLOR_TEXT_MUTED);
        DrawTextBox({ (float)formX + 92, (float)formY + 215, 65, 38 }, ui.hdMonth, 2, &ui.actMonth, 1);
        DrawText("/", formX + 167, formY + 220, 22, COLOR_TEXT_MUTED);
        DrawTextBox({ (float)formX + 184, (float)formY + 215, 90, 38 }, ui.hdYear, 4, &ui.actYear, 1);
        
        // Loại hóa đơn
        DrawText("Loại hóa đơn (Nhập 'N' hoặc 'X'):", formX, formY + 270, 18, COLOR_TEXT_MUTED);
        DrawTextBox({ (float)formX, (float)formY + 295, 65, 38 }, ui.hdLoai, 1, &ui.actLoai, 0);
        if (strlen(ui.hdLoai) > 0) {
            char tCh = toupper(ui.hdLoai[0]);
            if (tCh == 'N') DrawText("-> PHIẾU NHẬP VẬT TƯ", formX + 80, formY + 301, 18, COLOR_ACCENT);
            else if (tCh == 'X') DrawText("-> PHIẾU XUẤT VẬT TƯ", formX + 80, formY + 301, 18, COLOR_ACCENT);
            else DrawText("-> Loại không hợp lệ!", formX + 80, formY + 301, 18, RED);
        }
        
        if (DrawButton({ (float)formX, (float)formY + 370, (float)formWidth, 42 }, "Bắt đầu thêm chi tiết")) {
            trimString(ui.hdSoHD);
            trimString(ui.hdDay);
            trimString(ui.hdMonth);
            trimString(ui.hdYear);
            trimString(ui.hdLoai);
            
            int d = atoi(ui.hdDay);
            int m = atoi(ui.hdMonth);
            int y = atoi(ui.hdYear);
            
            int tmpNV = -1;
            NodeHOADON* tmpHD = NULL;
            bool dupHD = timHoaDonToanHeThong(dsnv, ui.hdSoHD, tmpNV, tmpHD);
            
            if (ui.selectedEmployeeIndex == -1) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Thiếu Nhân Viên");
                strcpy(ui.dialogMsg, "Vui lòng chọn nhân viên lập phiếu ở bảng trái!");
            } else if (isOnlySpaces(ui.hdSoHD)) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Lỗi Số Hóa Đơn");
                strcpy(ui.dialogMsg, "Số hóa đơn không được rỗng!");
            } else if (dupHD) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Lỗi Trùng Hóa Đơn");
                strcpy(ui.dialogMsg, "Số hóa đơn đã tồn tại trong hệ thống!");
            } else if (!isValidDate(d, m, y)) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Lỗi Ngày Tháng");
                strcpy(ui.dialogMsg, "Ngày lập hóa đơn không hợp lệ!");
            } else if (strlen(ui.hdLoai) == 0 || (toupper(ui.hdLoai[0]) != 'N' && toupper(ui.hdLoai[0]) != 'X')) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Lỗi Loại Hóa Đơn");
                strcpy(ui.dialogMsg, "Loại hóa đơn chỉ nhận ký tự 'N' hoặc 'X'!");
            } else {
                ui.creatingInvoice = true;
                strcpy(ui.tempInvoice.SoHD, ui.hdSoHD);
                ui.tempInvoice.NgayLap = { d, m, y };
                ui.tempInvoice.Loai = toupper(ui.hdLoai[0]);
                ui.tempInvoice.dscthd.n = 0;
                
                ui.ctMAVT[0] = '\0';
                ui.ctSoluong[0] = '\0';
                ui.ctDongia[0] = '\0';
                ui.ctVAT[0] = '\0';
            }
        }
    } else {
        // ---------------- BƯỚC 2: NHẬP VẬT TƯ CHO CHI TIẾT HÓA ĐƠN ----------------
        
        // Vẽ header tóm tắt
        DrawRectangleRounded({ 310, 70, contentWidth - 60, 75 }, 0.05f, 4, COLOR_CARD);
        DrawRectangleRoundedLines({ 310, 70, contentWidth - 60, 75 }, 0.05f, 4, COLOR_BORDER);
        
        char infoBuf[256];
        sprintf(infoBuf, "HĐ: %s  |  Ngày: %02d/%02d/%04d  |  Loại: %s  |  NV: %s %s",
                ui.tempInvoice.SoHD, ui.tempInvoice.NgayLap.day, ui.tempInvoice.NgayLap.month, ui.tempInvoice.NgayLap.year,
                ui.tempInvoice.Loai == 'N' ? "NHẬP" : "XUẤT",
                dsnv.node[ui.selectedEmployeeIndex]->HO, dsnv.node[ui.selectedEmployeeIndex]->TEN);
        DrawText(infoBuf, 330, 80, 20, COLOR_PRIMARY); // Size 20
        DrawText("Nhấp chọn vật tư từ bảng trái để nạp thông tin thêm vào hóa đơn", 330, 110, 18, COLOR_TEXT_MUTED); // Size 18
        
        // Vẽ bảng danh sách vật tư bên trái để click chọn
        int startY = 160;
        int rowHeight = 40;
        // Cố định hiển thị 10 dòng trên mỗi trang để giao diện đồng bộ
        int rowsPerPage = 10;
        
        int countVT = countNodes(t);
        VatTu* arrVT = new VatTu[countVT > 0 ? countVT : 1];
        int n = 0;
        treeToArray(t, arrVT, n);
        sortVatTuByName(arrVT, n);
        
        int totalPages = (n + rowsPerPage - 1) / rowsPerPage;
        if (totalPages == 0) totalPages = 1;
        if (ui.pageVT >= totalPages) ui.pageVT = totalPages - 1;
        
        float tableWidth = formX - 10 - 310;
        
        float col1 = 10;
        float col2 = tableWidth * 0.30f;
        float col3 = tableWidth * 0.75f;
        
        Rectangle headerRec = { 310, (float)startY, tableWidth, 30 };
        DrawRectangleRec(headerRec, COLOR_SECONDARY);
        DrawTextLimit("Mã VT", headerRec.x + col1, headerRec.y + 6, 18, RAYWHITE, col2 - col1 - 5);
        DrawTextLimit("Tên Vật Tư", headerRec.x + col2, headerRec.y + 6, 18, RAYWHITE, col3 - col2 - 10);
        DrawTextLimit("SL Tồn", headerRec.x + col3, headerRec.y + 6, 18, RAYWHITE, tableWidth - col3 - 10);
        
        Vector2 mouse = GetMousePosition();
        for (int i = ui.pageVT * rowsPerPage; i < min(n, (ui.pageVT + 1) * rowsPerPage); i++) {
            int rowY = startY + 30 + (i - ui.pageVT * rowsPerPage) * rowHeight;
            Rectangle rowRec = { 310, (float)rowY, tableWidth, (float)rowHeight };
            
            bool isHovered = CheckCollisionPointRec(mouse, rowRec) && !guiDialogActive;
            Color bgCol = (i % 2 == 0 ? COLOR_CARD : COLOR_BG);
            if (isHovered) bgCol = COLOR_BORDER;
            
            DrawRectangleRec(rowRec, bgCol);
            DrawRectangleLinesEx(rowRec, 0.5f, COLOR_BORDER);
            
            DrawTextLimit(arrVT[i].MAVT, rowRec.x + col1, rowRec.y + 10, 18, COLOR_TEXT_MAIN, col2 - col1 - 5);
            DrawTextLimit(arrVT[i].TENVT, rowRec.x + col2, rowRec.y + 10, 18, COLOR_TEXT_MAIN, col3 - col2 - 10);
            DrawTextLimit(TextFormat("%.2f", arrVT[i].SoLuongTon), rowRec.x + col3, rowRec.y + 10, 18, COLOR_TEXT_MAIN, tableWidth - col3 - 10);
            
            if (isHovered && IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !guiDialogActive) {
                strcpy(ui.ctMAVT, arrVT[i].MAVT);
            }
        }
        
        float controlsY = startY + 30 + rowsPerPage * rowHeight + 10;
        
        char pageText[50];
        sprintf(pageText, "Trang %d / %d", ui.pageVT + 1, totalPages);
        DrawText(pageText, 310 + tableWidth/2 - 50, controlsY + 8, 20, COLOR_TEXT_MUTED);
        
        if (DrawButton({ 310, controlsY, 90, 32 }, "Trước")) {
            if (ui.pageVT > 0) ui.pageVT--;
        }
        if (DrawButton({ 310 + tableWidth - 90, controlsY, 90, 32 }, "Sau")) {
            if (ui.pageVT < totalPages - 1) ui.pageVT++;
        }
        
        delete[] arrVT;
        
        // Form nhập chi tiết vật tư (Bên phải)
        DrawRectangleRounded({ (float)formX - 10, (float)formY - 10, (float)formWidth + 20, 250 }, 0.03f, 4, COLOR_CARD);
        DrawRectangleRoundedLines({ (float)formX - 10, (float)formY - 10, (float)formWidth + 20, 250 }, 0.03f, 4, COLOR_BORDER);
        
        DrawText("THÊM VẬT TƯ VÀO HÓA ĐƠN", formX, formY, 18, COLOR_PRIMARY);
        
        // TextBox MAVT (Tăng lên 35px cao)
        DrawText("Mã vật tư:", formX, formY + 28, 16, COLOR_TEXT_MUTED);
        DrawTextBox({ (float)formX, (float)formY + 48, 140, 35 }, ui.ctMAVT, 10, &ui.actCtMAVT, 0);
        
        // TextBox số lượng
        DrawText("Số lượng:", formX + 170, formY + 28, 16, COLOR_TEXT_MUTED);
        DrawTextBox({ (float)formX + 170, (float)formY + 48, 90, 35 }, ui.ctSoluong, 8, &ui.actCtQty, 2);
        
        // TextBox đơn giá
        DrawText("Đơn giá nhập/xuất (VND):", formX, formY + 95, 16, COLOR_TEXT_MUTED);
        DrawTextBox({ (float)formX, (float)formY + 115, 140, 35 }, ui.ctDongia, 12, &ui.actCtPrice, 2);
        
        // TextBox phần trăm VAT
        DrawText("% thuế VAT:", formX + 170, formY + 95, 16, COLOR_TEXT_MUTED);
        DrawTextBox({ (float)formX + 170, (float)formY + 115, 90, 35 }, ui.ctVAT, 5, &ui.actCtVAT, 2);
        
        if (DrawButton({ (float)formX, (float)formY + 165, (float)formWidth, 38 }, "Thêm vật tư này")) {
            trimString(ui.ctMAVT);
            trimString(ui.ctSoluong);
            trimString(ui.ctDongia);
            trimString(ui.ctVAT);
            NodeVatTu* pVT = searchVatTu(t, ui.ctMAVT);
            float sl = atof(ui.ctSoluong);
            float dg = atof(ui.ctDongia);
            float vat = atof(ui.ctVAT);
            
            if (pVT == NULL) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Lỗi Tìm Vật Tư");
                strcpy(ui.dialogMsg, "Mã vật tư không tồn tại trong danh mục!");
            } else if (sl <= 0 || dg <= 0 || vat < 0) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Lỗi Nhập Số Liệu");
                strcpy(ui.dialogMsg, "Số lượng, Đơn giá phải > 0!\nThuế VAT phải >= 0%!");
            } else if (ui.tempInvoice.dscthd.n >= 20) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Lỗi Số Lượng");
                strcpy(ui.dialogMsg, "Hóa đơn đã đầy! Một hóa đơn chỉ chứa tối đa 20 vật tư!");
            } else if (searchCTHD(ui.tempInvoice.dscthd, ui.ctMAVT) != -1) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Lỗi Vật Tư");
                strcpy(ui.dialogMsg, "Vật tư này đã có trong hóa đơn đang lập!");
            } else {
                CT_HOADON ct;
                strcpy(ct.MAVT, ui.ctMAVT);
                ct.Soluong = sl;
                ct.Dongia = dg;
                ct.VAT = vat;
                
                int res = lapCTHD(ui.tempInvoice, ct, t);
                if (res == -1) {
                    ui.showDialog = true;
                    strcpy(ui.dialogTitle, "Lỗi Tồn Kho");
                    char errStr[128];
                    sprintf(errStr, "Số lượng tồn kho không đủ để xuất!\nTồn kho hiện tại: %.2f", pVT->info.SoLuongTon);
                    strcpy(ui.dialogMsg, errStr);
                } else if (res == 1) {
                    ui.ctMAVT[0] = '\0';
                    ui.ctSoluong[0] = '\0';
                    ui.ctDongia[0] = '\0';
                    ui.ctVAT[0] = '\0';
                }
            }
        }
        
        // Bảng chi tiết hóa đơn nháp bên dưới
        DrawText("CHI TIẾT VẬT TƯ ĐANG LẬP", formX - 10, formY + 255, 18, COLOR_PRIMARY);
        
        int listY = formY + 280;
        Rectangle listHeader = { (float)formX - 10, (float)listY, (float)formWidth + 20, 30 };
        DrawRectangleRec(listHeader, COLOR_SECONDARY);
        DrawTextLimit("Mã VT", listHeader.x + 5, listHeader.y + 6, 16, RAYWHITE, 75);
        DrawTextLimit("SL", listHeader.x + 85, listHeader.y + 6, 16, RAYWHITE, 40);
        DrawTextLimit("Đơn giá", listHeader.x + 130, listHeader.y + 6, 16, RAYWHITE, 85);
        DrawTextLimit("Trị giá", listHeader.x + 220, listHeader.y + 6, 16, RAYWHITE, 75);
        DrawTextLimit("Xóa", listHeader.x + 305, listHeader.y + 6, 16, RAYWHITE, 35);
        
        // Vẽ các chi tiết hóa đơn đang lập (Tăng size chữ hàng lên 16, cao hàng lên 36px)
        for (int i = 0; i < ui.tempInvoice.dscthd.n; i++) {
            int rowY = listY + 30 + i * 36;
            if (rowY > GetScreenHeight() - 100) break; 
            
            Rectangle rowRec = { (float)formX - 10, (float)rowY, (float)formWidth + 20, 36 };
            DrawRectangleRec(rowRec, i % 2 == 0 ? COLOR_CARD : COLOR_BG);
            DrawRectangleLinesEx(rowRec, 0.5f, COLOR_BORDER);
            
            CT_HOADON ctd = ui.tempInvoice.dscthd.node[i];
            DrawTextLimit(ctd.MAVT, rowRec.x + 5, rowRec.y + 9, 16, COLOR_TEXT_MAIN, 75);
            DrawTextLimit(TextFormat("%.1f", ctd.Soluong), rowRec.x + 85, rowRec.y + 9, 16, COLOR_TEXT_MAIN, 40);
            DrawTextLimit(TextFormat("%.0f", ctd.Dongia), rowRec.x + 130, rowRec.y + 9, 16, COLOR_TEXT_MAIN, 85);
            
            float val = calculateCTHDTotal(ctd);
            DrawTextLimit(TextFormat("%.0f", val), rowRec.x + 220, rowRec.y + 9, 16, COLOR_TEXT_MAIN, 75);
            
            Rectangle delBtn = { rowRec.x + 300, rowRec.y + 4, 40, 28 };
            if (DrawButton(delBtn, "X", RED)) {
                xoaCTHDDangLap(ui.tempInvoice, i, t);
            }
        }
        
        float totalVal = calculateHOADONTotal(ui.tempInvoice);
        DrawText(TextFormat("TỔNG TIỀN HĐ: %.0f VND", totalVal), formX - 10, GetScreenHeight() - 90, 24, COLOR_ACCENT); // Size 24
        
        // Nút Ghi và Hủy cao 42px
        float btnY = GetScreenHeight() - 55;
        if (DrawButton({ (float)formX - 10, btnY, 170, 42 }, "GHI HÓA ĐƠN")) {
            if (ui.tempInvoice.dscthd.n == 0) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Hóa Đơn Rỗng");
                strcpy(ui.dialogMsg, "Không thể ghi nhận hóa đơn không chứa vật tư!");
            } else {
                insertHOADONTail(dsnv.node[ui.selectedEmployeeIndex]->dshd, ui.tempInvoice);
                saveData(t, dsnv);
                
                ui.resetInputs();
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Ghi Thành Công");
                strcpy(ui.dialogMsg, "Đã lập hóa đơn thành công và ghi file!");
            }
        }
        
        if (DrawButton({ (float)formX + 170, btnY, 170, 42 }, "HỦY LẬP PHIẾU", RED)) {
            huyHoaDonDangLap(ui.tempInvoice, t);
            ui.resetInputs();
        }
    }
}

// ============================================================================
// HÀM VẼ GIAO DIỆN IN CHI TIẾT HÓA ĐƠN (PRINT VIEW INVOICE)
// ============================================================================

void drawInHoaDonScreen(TreeVatTu t, const DanhSachNhanvien &dsnv, UIState &ui) {
    float contentWidth = GetScreenWidth() - 280;
    float contentHeight = GetScreenHeight();
    DrawRectangle(280, 0, contentWidth, contentHeight, COLOR_BG);
    DrawText("IN CHI TIẾT HÓA ĐƠN", 310, 25, 36, COLOR_PRIMARY);
    
    // Form tìm kiếm Số HĐ
    DrawRectangleRounded({ 310, 70, contentWidth - 60, 75 }, 0.05f, 4, COLOR_CARD);
    DrawRectangleRoundedLines({ 310, 70, contentWidth - 60, 75 }, 0.05f, 4, COLOR_BORDER);
    
    DrawText("Nhập Số Hóa Đơn cần in chi tiết:", 330, 78, 18, COLOR_TEXT_MUTED); // Size 18
    DrawTextBox({ 330, 98, 240, 36 }, ui.searchSoHD, 20, &ui.actSearchSoHD, 0); // Cao 36
    
    if (DrawButton({ 590, 98, 140, 36 }, "Tìm Kiếm")) {
        trimString(ui.searchSoHD);
        int nvIdx = -1;
        NodeHOADON* targetNode = NULL;
        bool found = timHoaDonToanHeThong(dsnv, ui.searchSoHD, nvIdx, targetNode);
        
        if (found) {
            ui.showInvoiceResult = true;
            ui.foundInvoice = targetNode->info;
            sprintf(ui.foundEmpName, "%s %s (MANV: %d)", dsnv.node[nvIdx]->HO, dsnv.node[nvIdx]->TEN, dsnv.node[nvIdx]->MANV);
        } else {
            ui.showInvoiceResult = false;
            ui.showDialog = true;
            strcpy(ui.dialogTitle, "Không Tìm Thấy");
            strcpy(ui.dialogMsg, "Không tìm thấy hóa đơn có số vừa nhập!");
        }
    }
    
    // Vẽ hóa đơn dạng tờ in co giãn chiều ngang và chiều đứng
    if (ui.showInvoiceResult) {
        int billX = 310;
        int billY = 160;
        float billW = contentWidth - 60;
        float billH = contentHeight - 160 - 30;
        
        DrawRectangleRounded({ (float)billX, (float)billY, billW, billH }, 0.02f, 4, COLOR_CARD);
        DrawRectangleRoundedLines({ (float)billX, (float)billY, billW, billH }, 0.02f, 4, COLOR_BORDER);
        
        DrawText("HÓA ĐƠN CHI TIẾT KHO HÀNG", billX + billW/2 - 180, billY + 25, 28, COLOR_PRIMARY); // Size 28
        
        char txt[128];
        sprintf(txt, "Số HĐ: %s", ui.foundInvoice.SoHD);
        DrawText(txt, billX + 40, billY + 70, 20, COLOR_TEXT_MAIN); // Size 20
        
        sprintf(txt, "Ngày lập: %02d/%02d/%04d", ui.foundInvoice.NgayLap.day, ui.foundInvoice.NgayLap.month, ui.foundInvoice.NgayLap.year);
        DrawText(txt, billX + billW - 310, billY + 70, 20, COLOR_TEXT_MAIN);
        
        sprintf(txt, "Nhân viên lập: %s", ui.foundEmpName);
        DrawText(txt, billX + 40, billY + 105, 20, COLOR_TEXT_MAIN);
        
        sprintf(txt, "Loại phiếu: %s", ui.foundInvoice.Loai == 'N' ? "PHIẾU NHẬP" : "PHIẾU XUẤT");
        DrawText(txt, billX + billW - 310, billY + 105, 20, COLOR_TEXT_MAIN);
        
        // Vẽ bảng chi tiết vật tư trong hóa đơn (Tăng size lên 18, chiều cao dòng lên 40)
        int tblY = billY + 150;
        float tblW = billW - 60;
        Rectangle tblHead = { (float)billX + 30, (float)tblY, tblW, 36 };
        DrawRectangleRec(tblHead, COLOR_SECONDARY);
        
        float col1 = 10;
        float col2 = tblW * 0.08f;
        float col3 = tblW * 0.22f;
        float col4 = tblW * 0.52f;
        float col5 = tblW * 0.64f;
        float col6 = tblW * 0.77f;
        float col7 = tblW * 0.87f;
        
        DrawTextLimit("STT", tblHead.x + col1, tblHead.y + 8, 18, RAYWHITE, col2 - col1 - 5);
        DrawTextLimit("Mã VT", tblHead.x + col2, tblHead.y + 8, 18, RAYWHITE, col3 - col2 - 5);
        DrawTextLimit("Tên Vật Tư", tblHead.x + col3, tblHead.y + 8, 18, RAYWHITE, col4 - col3 - 10);
        DrawTextLimit("Số Lượng", tblHead.x + col4, tblHead.y + 8, 18, RAYWHITE, col5 - col4 - 5);
        DrawTextLimit("Đơn Giá", tblHead.x + col5, tblHead.y + 8, 18, RAYWHITE, col6 - col5 - 5);
        DrawTextLimit("Thuế VAT", tblHead.x + col6, tblHead.y + 8, 18, RAYWHITE, col7 - col6 - 5);
        DrawTextLimit("Trị Giá", tblHead.x + col7, tblHead.y + 8, 18, RAYWHITE, tblW - col7 - 10);
        
        for (int i = 0; i < ui.foundInvoice.dscthd.n; i++) {
            int rowY = tblY + 36 + i * 40;
            if (rowY > billY + billH - 70) break; 
            
            Rectangle rowRec = { (float)billX + 30, (float)rowY, tblW, 40 };
            DrawRectangleRec(rowRec, i % 2 == 0 ? COLOR_CARD : COLOR_BG);
            DrawRectangleLinesEx(rowRec, 0.5f, COLOR_BORDER);
            
            CT_HOADON ct = ui.foundInvoice.dscthd.node[i];
            
            char nameVT[100] = "Không tìm thấy vật tư";
            NodeVatTu* pvt = searchVatTu(t, ct.MAVT);
            if (pvt != NULL) {
                strcpy(nameVT, pvt->info.TENVT);
            }
            
            DrawTextLimit(TextFormat("%d", i + 1), rowRec.x + col1, rowRec.y + 10, 18, COLOR_TEXT_MAIN, col2 - col1 - 5);
            DrawTextLimit(ct.MAVT, rowRec.x + col2, rowRec.y + 10, 18, COLOR_TEXT_MAIN, col3 - col2 - 5);
            DrawTextLimit(nameVT, rowRec.x + col3, rowRec.y + 10, 18, COLOR_TEXT_MAIN, col4 - col3 - 10);
            DrawTextLimit(TextFormat("%.1f", ct.Soluong), rowRec.x + col4, rowRec.y + 10, 18, COLOR_TEXT_MAIN, col5 - col4 - 5);
            DrawTextLimit(TextFormat("%.0f", ct.Dongia), rowRec.x + col5, rowRec.y + 10, 18, COLOR_TEXT_MAIN, col6 - col5 - 5);
            DrawTextLimit(TextFormat("%.1f %%", ct.VAT), rowRec.x + col6, rowRec.y + 10, 18, COLOR_TEXT_MAIN, col7 - col6 - 5);
            
            float val = calculateCTHDTotal(ct);
            DrawTextLimit(TextFormat("%.0f", val), rowRec.x + col7, rowRec.y + 10, 18, COLOR_TEXT_MAIN, tblW - col7 - 10);
        }
        
        float total = calculateHOADONTotal(ui.foundInvoice);
        char totalBuf[100];
        sprintf(totalBuf, "TỔNG TRỊ GIÁ HÓA ĐƠN: %.0f VND", total);
        DrawText(totalBuf, billX + 40, billY + billH - 50, 24, COLOR_ACCENT); // Size 24
    } else {
        DrawText("Vui lòng điền Số hóa đơn phía trên và bấm 'Tìm Kiếm' để hiển thị.", 310, 170, 20, COLOR_TEXT_MUTED);
    }
}

// ============================================================================
// HÀM VẼ GIAO DIỆN THỐNG KÊ HÓA ĐƠN THEO KHOẢNG THỜI GIAN (STATE_THONG_KE)
// ============================================================================

void drawThongKeScreen(const DanhSachNhanvien &dsnv, UIState &ui) {
    float contentWidth = GetScreenWidth() - 280;
    float contentHeight = GetScreenHeight();
    DrawRectangle(280, 0, contentWidth, contentHeight, COLOR_BG);
    DrawText("THỐNG KÊ HÓA ĐƠN TRONG KHOẢNG THỜI GIAN", 310, 25, 32, COLOR_PRIMARY);
    
    // Khung lọc ngày co giãn ngang
    float filterBarW = contentWidth - 60;
    DrawRectangleRounded({ 310, 70, filterBarW, 75 }, 0.05f, 4, COLOR_CARD);
    DrawRectangleRoundedLines({ 310, 70, filterBarW, 75 }, 0.05f, 4, COLOR_BORDER);
    
    // Từ ngày
    DrawText("Từ Ngày (D/M/Y):", 330, 78, 18, COLOR_TEXT_MUTED);
    DrawTextBox({ 330, 98, 55, 34 }, ui.fromDay, 2, &ui.actFD, 1);
    DrawText("/", 390, 103, 18, COLOR_TEXT_MUTED);
    DrawTextBox({ 402, 98, 55, 34 }, ui.fromMonth, 2, &ui.actFM, 1);
    DrawText("/", 462, 103, 18, COLOR_TEXT_MUTED);
    DrawTextBox({ 474, 98, 75, 34 }, ui.fromYear, 4, &ui.actFY, 1);
    
    // Đến ngày
    DrawText("Đến Ngày (D/M/Y):", 570, 78, 18, COLOR_TEXT_MUTED);
    DrawTextBox({ 570, 98, 55, 34 }, ui.toDay, 2, &ui.actTD, 1);
    DrawText("/", 630, 103, 18, COLOR_TEXT_MUTED);
    DrawTextBox({ 642, 98, 55, 34 }, ui.toMonth, 2, &ui.actTM, 1);
    DrawText("/", 702, 103, 18, COLOR_TEXT_MUTED);
    DrawTextBox({ 714, 98, 75, 34 }, ui.toYear, 4, &ui.actTY, 1);
    
    float btnX = 310 + filterBarW - 155;
    if (DrawButton({ btnX, 98, 140, 34 }, "Lọc Thống Kê")) {
        trimString(ui.fromDay);
        trimString(ui.fromMonth);
        trimString(ui.fromYear);
        trimString(ui.toDay);
        trimString(ui.toMonth);
        trimString(ui.toYear);
        int fd = atoi(ui.fromDay);
        int fm = atoi(ui.fromMonth);
        int fy = atoi(ui.fromYear);
        int td = atoi(ui.toDay);
        int tm = atoi(ui.toMonth);
        int ty = atoi(ui.toYear);
        
        if (!isValidDate(fd, fm, fy) || !isValidDate(td, tm, ty)) {
            ui.showDialog = true;
            strcpy(ui.dialogTitle, "Lỗi Nhập Ngày");
            strcpy(ui.dialogMsg, "Ngày Từ hoặc Ngày Đến nhập vào không hợp lệ!");
        } else {
            Date fromD = { fd, fm, fy };
            Date toD = { td, tm, ty };
            
            if (compareDate(fromD, toD) > 0) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Lỗi Khoảng Ngày");
                strcpy(ui.dialogMsg, "Ngày Từ phải nhỏ hơn hoặc bằng Ngày Đến!");
            } else {
                ui.tkInvoices.clear();
                for (int i = 0; i < dsnv.n; i++) {
                    NodeHOADON* cur = dsnv.node[i]->dshd;
                    while (cur != NULL) {
                        if (compareDate(cur->info.NgayLap, fromD) >= 0 && compareDate(cur->info.NgayLap, toD) <= 0) {
                            ThongKeHDDong tk;
                            strcpy(tk.SoHD, cur->info.SoHD);
                            tk.NgayLap = cur->info.NgayLap;
                            tk.Loai = cur->info.Loai;
                            sprintf(tk.HoTenNV, "%s %s", dsnv.node[i]->HO, dsnv.node[i]->TEN);
                            tk.TriGia = calculateHOADONTotal(cur->info);
                            ui.tkInvoices.push_back(tk);
                        }
                        cur = cur->next;
                    }
                }
                ui.pageTK = 0;
            }
        }
    }
    
    // In danh sách thống kê hóa đơn
    int startY = 160;
    int rowHeight = 45; // Tăng lên 45
    
    // Cố định hiển thị 10 dòng trên mỗi trang để giao diện đồng bộ
    int rowsPerPage = 10;
    
    int totalItems = ui.tkInvoices.size();
    int totalPages = (totalItems + rowsPerPage - 1) / rowsPerPage;
    if (totalPages == 0) totalPages = 1;
    if (ui.pageTK >= totalPages) ui.pageTK = totalPages - 1;
    
    float tableWidth = contentWidth - 60;
    
    DrawText("BẢNG LIỆT KÊ CÁC HÓA ĐƠN TRONG KHOẢNG THỜI GIAN", 310 + tableWidth/2 - 250, startY, 20, COLOR_PRIMARY);
    if (totalItems > 0) {
        DrawText(TextFormat("Từ ngày: %s/%s/%s   Đến ngày: %s/%s/%s", ui.fromDay, ui.fromMonth, ui.fromYear, ui.toDay, ui.toMonth, ui.toYear), 310 + tableWidth/2 - 160, startY + 25, 18, COLOR_TEXT_MUTED);
    }
    
    Rectangle header = { 310, (float)startY + 50, tableWidth, 36 };
    DrawRectangleRec(header, COLOR_SECONDARY);
    
    float col1 = 10;
    float col2 = tableWidth * 0.20f;
    float col3 = tableWidth * 0.38f;
    float col4 = tableWidth * 0.55f;
    float col5 = tableWidth * 0.80f;
    
    DrawTextLimit("Số HĐ", header.x + col1, header.y + 8, 18, RAYWHITE, col2 - col1 - 5);
    DrawTextLimit("Ngày lập", header.x + col2, header.y + 8, 18, RAYWHITE, col3 - col2 - 5);
    DrawTextLimit("Loại HĐ", header.x + col3, header.y + 8, 18, RAYWHITE, col4 - col3 - 5);
    DrawTextLimit("Họ tên NV lập", header.x + col4, header.y + 8, 18, RAYWHITE, col5 - col4 - 10);
    DrawTextLimit("Trị giá hóa đơn (VND)", header.x + col5, header.y + 8, 18, RAYWHITE, tableWidth - col5 - 10);
    
    int startIdx = ui.pageTK * rowsPerPage;
    int endIdx = startIdx + rowsPerPage;
    if (endIdx > totalItems) endIdx = totalItems;
    
    for (int i = startIdx; i < endIdx; i++) {
        int rowY = startY + 86 + (i - startIdx) * rowHeight;
        Rectangle rowRec = { 310, (float)rowY, tableWidth, (float)rowHeight };
        
        DrawRectangleRec(rowRec, i % 2 == 0 ? COLOR_CARD : COLOR_BG);
        DrawRectangleLinesEx(rowRec, 0.5f, COLOR_BORDER);
        
        ThongKeHDDong r = ui.tkInvoices[i];
        DrawTextLimit(r.SoHD, rowRec.x + col1, rowRec.y + 11, 18, COLOR_TEXT_MAIN, col2 - col1 - 5);
        DrawTextLimit(TextFormat("%02d/%02d/%04d", r.NgayLap.day, r.NgayLap.month, r.NgayLap.year), rowRec.x + col2, rowRec.y + 11, 18, COLOR_TEXT_MAIN, col3 - col2 - 5);
        DrawTextLimit(r.Loai == 'N' ? "Phiếu Nhập" : "Phiếu Xuất", rowRec.x + col3, rowRec.y + 11, 18, COLOR_TEXT_MAIN, col4 - col3 - 5);
        DrawTextLimit(r.HoTenNV, rowRec.x + col4, rowRec.y + 11, 18, COLOR_TEXT_MAIN, col5 - col4 - 10);
        DrawTextLimit(TextFormat("%.0f", r.TriGia), rowRec.x + col5, rowRec.y + 11, 18, COLOR_TEXT_MAIN, tableWidth - col5 - 10);
    }
    
    float controlsY = startY + 86 + rowsPerPage * rowHeight + 10;
    
    char pageText[50];
    sprintf(pageText, "Trang %d / %d", ui.pageTK + 1, totalPages);
    DrawText(pageText, 310 + tableWidth/2 - 50, controlsY + 8, 20, COLOR_TEXT_MUTED);
    
    if (DrawButton({ 310, controlsY, 110, 36 }, "Trước")) {
        if (ui.pageTK > 0) ui.pageTK--;
    }
    if (DrawButton({ 310 + tableWidth - 110, controlsY, 110, 36 }, "Sau")) {
        if (ui.pageTK < totalPages - 1) ui.pageTK++;
    }
}

// ============================================================================
// HÀM VẼ GIAO DIỆN TOP 10 VẬT TƯ DOANH THU CAO NHẤT (STATE_TOP_VAT_TU)
// ============================================================================

void drawTopVatTuScreen(TreeVatTu t, const DanhSachNhanvien &dsnv, UIState &ui) {
    float contentWidth = GetScreenWidth() - 280;
    float contentHeight = GetScreenHeight();
    DrawRectangle(280, 0, contentWidth, contentHeight, COLOR_BG);
    DrawText("10 VẬT TƯ DOANH THU CAO NHẤT", 310, 25, 32, COLOR_PRIMARY);
    
    // Khung lọc ngày co giãn ngang
    float filterBarW = contentWidth - 60;
    DrawRectangleRounded({ 310, 70, filterBarW, 75 }, 0.05f, 4, COLOR_CARD);
    DrawRectangleRoundedLines({ 310, 70, filterBarW, 75 }, 0.05f, 4, COLOR_BORDER);
    
    // Từ ngày
    DrawText("Từ Ngày (D/M/Y):", 330, 78, 18, COLOR_TEXT_MUTED);
    DrawTextBox({ 330, 98, 55, 34 }, ui.fromDay, 2, &ui.actFD, 1);
    DrawText("/", 390, 103, 18, COLOR_TEXT_MUTED);
    DrawTextBox({ 402, 98, 55, 34 }, ui.fromMonth, 2, &ui.actFM, 1);
    DrawText("/", 462, 103, 18, COLOR_TEXT_MUTED);
    DrawTextBox({ 474, 98, 75, 34 }, ui.fromYear, 4, &ui.actFY, 1);
    
    // Đến ngày
    DrawText("Đến Ngày (D/M/Y):", 570, 78, 18, COLOR_TEXT_MUTED);
    DrawTextBox({ 570, 98, 55, 34 }, ui.toDay, 2, &ui.actTD, 1);
    DrawText("/", 630, 103, 18, COLOR_TEXT_MUTED);
    DrawTextBox({ 642, 98, 55, 34 }, ui.toMonth, 2, &ui.actTM, 1);
    DrawText("/", 702, 103, 18, COLOR_TEXT_MUTED);
    DrawTextBox({ 714, 98, 75, 34 }, ui.toYear, 4, &ui.actTY, 1);
    
    static ThongKeDoanhThu top10[10];
    static int topCount = 0;
    static bool hasReport = false;
    
    float btnX = 310 + filterBarW - 155;
    if (DrawButton({ btnX, 98, 140, 34 }, "Thống Kê")) {
        trimString(ui.fromDay);
        trimString(ui.fromMonth);
        trimString(ui.fromYear);
        trimString(ui.toDay);
        trimString(ui.toMonth);
        trimString(ui.toYear);
        int fd = atoi(ui.fromDay);
        int fm = atoi(ui.fromMonth);
        int fy = atoi(ui.fromYear);
        int td = atoi(ui.toDay);
        int tm = atoi(ui.toMonth);
        int ty = atoi(ui.toYear);
        
        if (!isValidDate(fd, fm, fy) || !isValidDate(td, tm, ty)) {
            ui.showDialog = true;
            strcpy(ui.dialogTitle, "Lỗi Nhập Ngày");
            strcpy(ui.dialogMsg, "Ngày Từ hoặc Ngày Đến nhập vào không hợp lệ!");
            hasReport = false;
        } else {
            Date fromD = { fd, fm, fy };
            Date toD = { td, tm, ty };
            
            if (compareDate(fromD, toD) > 0) {
                ui.showDialog = true;
                strcpy(ui.dialogTitle, "Lỗi Khoảng Ngày");
                strcpy(ui.dialogMsg, "Ngày Từ phải nhỏ hơn hoặc bằng Ngày Đến!");
                hasReport = false;
            } else {
                thongKeTop10(dsnv, fromD, toD, t, top10, topCount);
                hasReport = true;
            }
        }
    }
    
    int startY = 160;
    int rowHeight = 45; // Tăng lên 45px
    float tableWidth = contentWidth - 60;
    
    DrawText("BẢNG THỐNG KÊ DOANH THU 10 VẬT TƯ LỚN NHẤT", 310 + tableWidth/2 - 250, startY, 20, COLOR_PRIMARY);
    if (hasReport) {
        DrawText(TextFormat("Từ ngày: %s/%s/%s   Đến ngày: %s/%s/%s", ui.fromDay, ui.fromMonth, ui.fromYear, ui.toDay, ui.toMonth, ui.toYear), 310 + tableWidth/2 - 160, startY + 25, 18, COLOR_TEXT_MUTED);
    }
    
    Rectangle header = { 310, (float)startY + 50, tableWidth, 36 };
    DrawRectangleRec(header, COLOR_SECONDARY);
    
    float col1 = 10;
    float col2 = tableWidth * 0.12f;
    float col3 = tableWidth * 0.32f;
    float col4 = tableWidth * 0.75f;
    
    DrawTextLimit("Hạng", header.x + col1, header.y + 8, 18, RAYWHITE, col2 - col1 - 5);
    DrawTextLimit("Mã Vật Tư", header.x + col2, header.y + 8, 18, RAYWHITE, col3 - col2 - 5);
    DrawTextLimit("Tên Vật Tư", header.x + col3, header.y + 8, 18, RAYWHITE, col4 - col3 - 10);
    DrawTextLimit("Doanh Thu Thu Được (VND)", header.x + col4, header.y + 8, 18, RAYWHITE, tableWidth - col4 - 10);
    
    if (hasReport) {
        if (topCount == 0) {
            DrawText("Không có doanh số xuất kho phát sinh trong khoảng thời gian này.", 330, startY + 110, 20, RED);
        } else {
            for (int i = 0; i < topCount; i++) {
                int rowY = startY + 86 + i * rowHeight;
                Rectangle rowRec = { 310, (float)rowY, tableWidth, (float)rowHeight };
                
                DrawRectangleRec(rowRec, i % 2 == 0 ? COLOR_CARD : COLOR_BG);
                DrawRectangleLinesEx(rowRec, 0.5f, COLOR_BORDER);
                
                DrawTextLimit(TextFormat("%d", i + 1), rowRec.x + col1, rowRec.y + 11, 20, COLOR_TEXT_MAIN, col2 - col1 - 5);
                DrawTextLimit(top10[i].MAVT, rowRec.x + col2, rowRec.y + 11, 20, COLOR_TEXT_MAIN, col3 - col2 - 5);
                DrawTextLimit(top10[i].TENVT, rowRec.x + col3, rowRec.y + 11, 20, COLOR_TEXT_MAIN, col4 - col3 - 10);
                DrawTextLimit(TextFormat("%.0f", top10[i].DoanhThu), rowRec.x + col4, rowRec.y + 11, 20, COLOR_ACCENT, tableWidth - col4 - 10);
            }
        }
    } else {
        DrawText("Điền khoảng thời gian ở trên và bấm 'Thống Kê' để xem báo cáo.", 310, startY + 110, 20, COLOR_TEXT_MUTED);
    }
}
