#pragma once
#include "raylib.h"
#include <string.h>

extern Font appFont;
inline bool guiDialogActive = false;

// Định nghĩa các macro vẽ chữ sử dụng Font Segoe UI chất lượng cao
#define DrawText(text, posX, posY, fontSize, color) \
    DrawTextEx(appFont, (text), {(float)(posX), (float)(posY)}, (float)(fontSize), 1.0f, (color))

#define MeasureText(text, fontSize) \
    ((int)MeasureTextEx(appFont, (text), (float)(fontSize), 1.0f).x)

inline void DrawTextLimit(const char* text, float posX, float posY, int fontSize, Color color, float maxWidth) {
    if (MeasureText(text, fontSize) <= maxWidth) {
        DrawText(text, posX, posY, fontSize, color);
        return;
    }
    
    char buf[256];
    strncpy(buf, text, sizeof(buf) - 1);
    buf[sizeof(buf) - 1] = '\0';
    
    int len = strlen(buf);
    while (len > 0 && MeasureText(TextFormat("%s...", buf), fontSize) > maxWidth) {
        len--;
        buf[len] = '\0';
    }
    
    DrawText(TextFormat("%s...", buf), posX, posY, fontSize, color);
}

// ============================================================================
// HỆ THỐNG MÀU SẮC PREMIUM (Giao diện Hiện đại)
// ============================================================================
#define COLOR_PRIMARY     CLITERAL(Color){ 15, 23, 42, 255 }    // Navy sẫm (#0F172A)
#define COLOR_SECONDARY   CLITERAL(Color){ 30, 41, 59, 255 }    // Slate Grey (#1E293B)
#define COLOR_ACCENT      CLITERAL(Color){ 13, 148, 136, 255 }  // Teal lục lam (#0D9488)
#define COLOR_HOVER       CLITERAL(Color){ 20, 184, 166, 255 }  // Lục lam sáng (#14B8A6)
#define COLOR_BG          CLITERAL(Color){ 241, 245, 249, 255 } // Xám Slate nhạt (#F1F5F9)
#define COLOR_CARD        CLITERAL(Color){ 255, 255, 255, 255 } // Trắng (#FFFFFF)
#define COLOR_BORDER      CLITERAL(Color){ 203, 213, 225, 255 } // Viền xám (#CBD5E1)
#define COLOR_TEXT_MAIN   CLITERAL(Color){ 15, 23, 42, 255 }    // Chữ sẫm (#0F172A)
#define COLOR_TEXT_MUTED  CLITERAL(Color){ 100, 116, 139, 255 } // Chữ mờ (#64748B)

// Hàm vẽ nút bấm hiện đại với hiệu ứng hover và active màu sắc Teal đẹp mắt
bool DrawButton(Rectangle bounds, const char* text, Color baseColor = COLOR_ACCENT) {
    bool clicked = false;
    Vector2 mousePoint = GetMousePosition();
    Color btnColor = baseColor;
    
    bool hover = CheckCollisionPointRec(mousePoint, bounds);
    if (guiDialogActive) {
        hover = false; // Khóa hover và click khi có dialog nổi lên
    }
    
    if (hover) {
        // Làm sáng màu khi hover
        if (baseColor.r == COLOR_ACCENT.r && baseColor.g == COLOR_ACCENT.g) {
            btnColor = COLOR_HOVER;
        } else {
            // Tự động làm sáng màu cơ bản khác
            btnColor = CLITERAL(Color){ (unsigned char)(baseColor.r + 20 > 255 ? 255 : baseColor.r + 20),
                                        (unsigned char)(baseColor.g + 20 > 255 ? 255 : baseColor.g + 20),
                                        (unsigned char)(baseColor.b + 20 > 255 ? 255 : baseColor.b + 20), 255 };
        }
        
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            btnColor = COLOR_PRIMARY;
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            clicked = true;
        }
    }
    
    // Vẽ nút bấm bo góc nhẹ
    DrawRectangleRounded(bounds, 0.15f, 4, btnColor);
    DrawRectangleRoundedLines(bounds, 0.15f, 4, hover ? COLOR_PRIMARY : btnColor);
    
    int textWidth = MeasureText(text, 22);
    DrawText(text, bounds.x + (bounds.width - textWidth) / 2, bounds.y + (bounds.height - 22) / 2, 22, RAYWHITE);
    
    return clicked;
}

// ============================================================================
// HỖ TRỢ DI CHUYỂN CON TRỎ VÀ CHÈN/XÓA TRÊN CHUỖI UTF-8
// ============================================================================
inline int moveCursorLeft(const char* text, int pos) {
    if (pos <= 0) return 0;
    pos--;
    while (pos > 0 && ((text[pos] & 0xC0) == 0x80)) {
        pos--;
    }
    return pos;
}

inline int moveCursorRight(const char* text, int pos) {
    int len = strlen(text);
    if (pos >= len) return len;
    pos++;
    while (pos < len && ((text[pos] & 0xC0) == 0x80)) {
        pos++;
    }
    return pos;
}

inline int deleteCharBeforeCursor(char* text, int pos) {
    if (pos <= 0) return 0;
    int prevPos = moveCursorLeft(text, pos);
    int len = strlen(text);
    memmove(text + prevPos, text + pos, len - pos + 1);
    return prevPos;
}

inline int insertCharAtCursor(char* text, int maxLen, int pos, const char* utf8Char, int byteSize) {
    int len = strlen(text);
    if (len + byteSize >= maxLen) return pos;
    memmove(text + pos + byteSize, text + pos, len - pos + 1);
    memcpy(text + pos, utf8Char, byteSize);
    return pos + byteSize;
}

// Hàm vẽ hộp nhập liệu thông minh (DrawTextBox)
// inputType: 0 - Mọi ký tự (ngoại trừ pipe '|'), 1 - Chỉ số nguyên (0-9), 2 - Số thực (0-9 và '.')
bool DrawTextBox(Rectangle bounds, char* text, int maxLen, bool* isActive, int inputType = 0) {
    bool enterPressed = false;
    Vector2 mousePoint = GetMousePosition();
    
    if (guiDialogActive) {
        *isActive = false; // Hủy kích hoạt nhập liệu khi có dialog nổi lên
    }
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !guiDialogActive) {
        if (CheckCollisionPointRec(mousePoint, bounds)) {
            *isActive = true;
        } else {
            *isActive = false;
        }
    }
    
    // Quản lý vị trí con trỏ nhập liệu cho ô text đang active
    static char* activeTextPtr = nullptr;
    static int cursorPosition = 0;
    
    if (*isActive && !guiDialogActive) {
        if (activeTextPtr != text) {
            activeTextPtr = text;
            cursorPosition = strlen(text);
        }
        int currentLen = strlen(text);
        if (cursorPosition > currentLen) cursorPosition = currentLen;
        if (cursorPosition < 0) cursorPosition = 0;
    } else {
        if (activeTextPtr == text) {
            activeTextPtr = nullptr;
        }
    }
    
    // Vẽ nền textbox
    DrawRectangleRec(bounds, COLOR_CARD);
    DrawRectangleLinesEx(bounds, *isActive ? 2 : 1, *isActive ? COLOR_ACCENT : COLOR_BORDER);
    
    // Vẽ chuỗi ký tự đang có
    DrawText(text, bounds.x + 8, bounds.y + (bounds.height - 22) / 2, 22, COLOR_TEXT_MAIN);
    
    if (*isActive && !guiDialogActive) {
        // Con trỏ nhấp nháy (cursor)
        static int framesCounter = 0;
        framesCounter++;
        
        // Đo chiều rộng chuỗi từ đầu đến vị trí con trỏ để vẽ caret
        char tempBuf[256];
        int posToMeasure = cursorPosition;
        if (posToMeasure > (int)sizeof(tempBuf) - 1) posToMeasure = sizeof(tempBuf) - 1;
        strncpy(tempBuf, text, posToMeasure);
        tempBuf[posToMeasure] = '\0';
        int textWidthBeforeCursor = MeasureText(tempBuf, 22);
        
        if ((framesCounter / 20) % 2 == 0) {
            DrawRectangle(bounds.x + 8 + textWidthBeforeCursor + 2, bounds.y + (bounds.height - 22) / 2, 2, 22, COLOR_TEXT_MAIN);
        }
        
        // 1. Xử lý di chuyển con trỏ (<- và ->)
        static int leftDelayCounter = 0;
        static int rightDelayCounter = 0;
        
        bool moveLeft = IsKeyPressed(KEY_LEFT);
        bool moveRight = IsKeyPressed(KEY_RIGHT);
        
        if (IsKeyDown(KEY_LEFT)) {
            leftDelayCounter++;
            if (leftDelayCounter > 30 && leftDelayCounter % 3 == 0) {
                moveLeft = true;
            }
        } else {
            leftDelayCounter = 0;
        }
        
        if (IsKeyDown(KEY_RIGHT)) {
            rightDelayCounter++;
            if (rightDelayCounter > 30 && rightDelayCounter % 3 == 0) {
                moveRight = true;
            }
        } else {
            rightDelayCounter = 0;
        }
        
        if (moveLeft) {
            cursorPosition = moveCursorLeft(text, cursorPosition);
        }
        if (moveRight) {
            cursorPosition = moveCursorRight(text, cursorPosition);
        }
        
        // 2. Xử lý xóa ký tự (Backspace) TRƯỚC khi nhận ký tự mới
        static int backspaceDelayCounter = 0;
        static int backspaceRepeatCounter = 0;
        
        // Đọc hàng đợi phím bấm để bắt phím Backspace ảo từ bộ gõ tiếng Việt (Unikey/EVKey)
        // và các phím nhấn đơn thông thường
        int k = GetKeyPressed();
        while (k > 0) {
            if (k == KEY_BACKSPACE) {
                cursorPosition = deleteCharBeforeCursor(text, cursorPosition);
            }
            if (k == KEY_ENTER) {
                enterPressed = true;
            }
            k = GetKeyPressed();
        }
        
        // Nhấn giữ phím Backspace vật lý để lặp lại việc xóa liên tục
        if (IsKeyDown(KEY_BACKSPACE)) {
            backspaceDelayCounter++;
            if (backspaceDelayCounter > 30) { // Giữ phím 0.5s để bắt đầu lặp
                backspaceRepeatCounter++;
                if (backspaceRepeatCounter >= 3) { // Xóa mỗi 3 frame khi giữ
                    cursorPosition = deleteCharBeforeCursor(text, cursorPosition);
                    backspaceRepeatCounter = 0;
                }
            }
        } else {
            backspaceDelayCounter = 0;
            backspaceRepeatCounter = 0;
        }

        // 3. Đọc ký tự nhập vào và chèn vào vị trí con trỏ
        int key = GetCharPressed();
        while (key > 0) {
            if ((key >= 32) && (key != '|') && ((int)strlen(text) < maxLen)) {
                bool allowed = false;
                if (inputType == 0) {
                    allowed = true; // Text tự do
                } else if (inputType == 1) { // Chỉ số nguyên
                    if (key >= '0' && key <= '9') allowed = true;
                } else if (inputType == 2) { // Số thực
                    if ((key >= '0' && key <= '9') || key == '.') allowed = true;
                }
                
                if (allowed) {
                    int byteSize = 0;
                    const char* utf8Char = CodepointToUTF8(key, &byteSize);
                    if (utf8Char) {
                        cursorPosition = insertCharAtCursor(text, maxLen, cursorPosition, utf8Char, byteSize);
                    }
                }
            }
            key = GetCharPressed();
        }
        
        if (IsKeyPressed(KEY_ENTER)) enterPressed = true;
    }
    
    return enterPressed;
}

// Vẽ một hộp thoại thông báo lỗi / cảnh báo (Dialog Box) hiện đại
void DrawDialog(const char* title, const char* message, bool &showDialog) {
    if (!showDialog) return;
    
    int screenW = GetScreenWidth();
    int screenH = GetScreenHeight();
    
    // Vẽ lớp phủ tối nền màn hình
    DrawRectangle(0, 0, screenW, screenH, Fade(COLOR_PRIMARY, 0.4f));
    
    // Vẽ hộp thoại ở giữa màn hình (tăng kích thước để chứa chữ to)
    float width = 560;
    float height = 260;
    Rectangle dialogBox = { (screenW - width) / 2.0f, (screenH - height) / 2.0f, width, height };
    
    DrawRectangleRounded(dialogBox, 0.05f, 4, COLOR_CARD);
    DrawRectangleRoundedLines(dialogBox, 0.05f, 4, RED);
    
    // Vẽ tiêu đề Dialog màu đỏ
    DrawText(title, dialogBox.x + 30, dialogBox.y + 30, 30, RED);
    
    // Vẽ nội dung thông báo
    DrawText(message, dialogBox.x + 30, dialogBox.y + 90, 24, COLOR_TEXT_MAIN);
    
    // Vẽ nút đóng Dialog (Tạm tắt guiDialogActive để có thể click được nút này)
    bool prevActive = guiDialogActive;
    guiDialogActive = false;
    
    Rectangle btnOk = { dialogBox.x + (width - 150) / 2.0f, dialogBox.y + 175, 150, 42 };
    if (DrawButton(btnOk, "Đồng ý", RED)) {
        showDialog = false;
    }
    
    guiDialogActive = prevActive;
}
