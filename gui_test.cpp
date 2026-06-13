#include "raylib.h"
#include <iostream>

using namespace std;

Font appFont;

#define DrawText(text, posX, posY, fontSize, color) \
    DrawTextEx(appFont, (text), {(float)(posX), (float)(posY)}, (float)(fontSize), 1.0f, (color))

int main(void) {
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "Quan Ly Nhap Xuat - Raylib Test");
    appFont = LoadFontEx("C:\\Windows\\Fonts\\segoeui.ttf", 32, 0, 0);
    SetTargetFPS(60);

    bool clicked = false;
    
    // Nút bấm ảo
    Rectangle btnBounds = { 300, 200, 200, 50 };

    while (!WindowShouldClose()) {
        // Cập nhật trạng thái
        Vector2 mousePoint = GetMousePosition();
        
        if (CheckCollisionPointRec(mousePoint, btnBounds)) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                clicked = !clicked;
            }
        }

        // Vẽ
        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText("Day la giao dien Raylib test!", 230, 100, 20, DARKGRAY);

            // Vẽ nút
            Color btnColor = CheckCollisionPointRec(mousePoint, btnBounds) ? LIGHTGRAY : GRAY;
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mousePoint, btnBounds)) {
                btnColor = DARKGRAY;
            }
            DrawRectangleRec(btnBounds, btnColor);
            DrawText("Test Button", btnBounds.x + 40, btnBounds.y + 15, 20, BLACK);

            if (clicked) {
                DrawText("Ban da click nut! Raylib hoat dong tot.", 200, 300, 20, LIME);
            }
            
            DrawText("Bam ESC hoac dau X de thoat", 10, 420, 15, GRAY);

        EndDrawing();
    }

    UnloadFont(appFont);
    CloseWindow();
    return 0;
}
