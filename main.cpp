#include <graphics.h>
#include <conio.h>
#include "Game.h"
#include "Input.h"
#include <windows.h>

int main() {
    // Get screen resolution
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    
    // Set window size to 1000x600
    int windowWidth = 1280;
    int windowHeight = 768;
    
    // Calculate window position to center it on screen
    int windowX = (screenWidth - windowWidth) / 2;
    int windowY = (screenHeight - windowHeight) / 2;
    
    // Initialize EasyX graphics window
    initgraph(windowWidth, windowHeight);
    HWND hwnd = GetHWnd();
    SetWindowPos(hwnd, NULL, windowX, windowY, windowWidth, windowHeight, SWP_SHOWWINDOW);
    
    // Set background color (sky blue)
    setbkcolor(RGB(135, 206, 235));
    cleardevice();
    
    // Initialize input system
    update_input();
    
    // Create and initialize game instance
    Game game;
    game.init();
    
    // Main game loop
    while (true) {
        // Update input state
        update_input();
        
        // Check for exit
        if (key_is_down(KeyCodeID::KEY_ESCAPE)) {
            break;
        }
        
        // Clear screen with background color
        cleardevice();
        
        // Begin drawing
        BeginBatchDraw();
        
        // Handle input
        game.handleInput();
        
        // Update game state
        game.update(1.0f/60.0f); // Pass fixed timestep for 60 FPS
        
        // Render
        game.render();
        
        // End drawing and flush
        EndBatchDraw();
        FlushBatchDraw();
        
        // Control frame rate
        Sleep(16); // Approximately 60 FPS
    }
    
    // Clean up
    closegraph();
    return 0;
}