#include <string>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include "structures.h"
#include "vattu.h"
#include "nhanvien.h"
#include "hoadon.h"
#include "docghi.h"
#include "gui.h"
#include "draw.h"

using namespace std;

// Khai báo Font dùng chung cho ứng dụng
Font appFont;

// Hàm tạo danh sách codepoints Tiếng Việt đầy đủ cho Raylib LoadFontEx
int* GetVietnameseCodepoints(int &count) {
    static int codepoints[1000];
    int idx = 0;
    
    // ASCII cơ bản (32 đến 126)
    for (int i = 32; i < 127; i++) codepoints[idx++] = i;
    
    // Latin-1 Supplement (ví dụ: á, à, í, ò, ú,...)
    for (int i = 0x00A0; i <= 0x00FF; i++) codepoints[idx++] = i;
    
    // Latin Extended-A (ví dụ: ă, â, ê, ô, đ,...)
    for (int i = 0x0100; i <= 0x017F; i++) codepoints[idx++] = i;
    
    // Latin Extended-B (ví dụ: ơ, ư,...)
    for (int i = 0x0180; i <= 0x024F; i++) codepoints[idx++] = i;
    
    // Latin Extended Additional (ví dụ: các nguyên âm tiếng Việt kèm dấu thanh phức tạp)
    for (int i = 0x1EA0; i <= 0x1EF9; i++) codepoints[idx++] = i;
    
    count = idx;
    return codepoints;
}

// Các trạng thái màn hình của ứng dụng
enum AppState {
    STATE_MENU,        // Màn hình Dashboard Trang chủ
    STATE_NHAP_VT,      // Màn hình Quản lý Vật tư (Thêm/Xóa/Sửa)
    STATE_NHAP_NV,      // Màn hình Quản lý Nhân viên (Thêm/Xóa/Sửa)
    STATE_LAP_HD,       // Màn hình Lập hóa đơn Nhập/Xuất
    STATE_IN_HD,        // Màn hình Tìm kiếm & In hóa đơn chi tiết
    STATE_THONG_KE,     // Màn hình Thống kê danh sách hóa đơn theo thời gian
    STATE_TOP_VAT_TU    // Màn hình Thống kê Top 10 vật tư doanh thu lớn nhất
};

int main() {
    // Khởi tạo các cấu trúc dữ liệu cốt lõi
    TreeVatTu treeVT;
    DanhSachNhanvien dsNV;
    
    initTreeVatTu(treeVT);
    initDanhSachNhanvien(dsNV);
    
    // Tải dữ liệu từ các file text lên bộ nhớ
    loadData(treeVT, dsNV);

    // Kích thước cửa sổ mặc định
    const int screenWidth = 1024;
    const int screenHeight = 768;

    // Thiết lập cấu hình cửa sổ ứng dụng Raylib (Hỗ trợ thay đổi kích thước và phóng to cửa sổ)
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "QUAN LY KHO VAT TU - GIAO DIEN HIEN DAI");
    
    // Nạp Font Segoe UI Tiếng Việt của Windows với đầy đủ các codepoint tiếng Việt
    int codepointCount = 0;
    int* codepoints = GetVietnameseCodepoints(codepointCount);
    appFont = LoadFontEx("C:\\Windows\\Fonts\\segoeui.ttf", 48, codepoints, codepointCount);
    SetTargetFPS(60); // Khóa khung hình ở 60 FPS để giao diện chạy mượt mà

    int currentState = STATE_MENU;
    UIState ui;

    // Vòng lặp vẽ giao diện chính (Sẽ dừng khi đóng X cửa sổ hoặc chọn tab Thoát)
    while (!WindowShouldClose() && !ui.exitApp) {
        guiDialogActive = ui.showDialog;
        BeginDrawing();
        ClearBackground(COLOR_BG); // Vẽ màu nền nhã nhặn

        // 1. Vẽ Sidebar menu bên trái và bắt sự kiện click chuyển tab
        drawSidebar(currentState, ui);

        // 2. Vẽ nội dung chi tiết tương ứng với từng Tab được chọn bên phải
        switch (currentState) {
            case STATE_MENU:
                drawDashboard(treeVT, dsNV);
                break;
                
            case STATE_NHAP_VT:
                drawVatTuScreen(treeVT, dsNV, ui);
                break;
                
            case STATE_NHAP_NV:
                drawNhanvienScreen(dsNV, treeVT, ui);
                break;
                
            case STATE_LAP_HD:
                drawLapHoaDonScreen(treeVT, dsNV, ui);
                break;
                
            case STATE_IN_HD:
                drawInHoaDonScreen(treeVT, dsNV, ui);
                break;
                
            case STATE_THONG_KE:
                drawThongKeScreen(dsNV, ui);
                break;
                
            case STATE_TOP_VAT_TU:
                drawTopVatTuScreen(treeVT, dsNV, ui);
                break;
                
            default:
                break;
        }

        // 3. Vẽ hộp thoại thông báo lỗi / cảnh báo nổi (Popup Dialog) ở trên cùng nếu có lỗi nhập liệu
        DrawDialog(ui.dialogTitle, ui.dialogMsg, ui.showDialog);

        EndDrawing();
    }

    // Tự động ghi lại toàn bộ dữ liệu xuống file text trước khi thoát chương trình
    saveData(treeVT, dsNV);
    
    // Giải phóng bộ nhớ Font và đóng cửa sổ Raylib
    UnloadFont(appFont);
    CloseWindow();
    return 0;
}
