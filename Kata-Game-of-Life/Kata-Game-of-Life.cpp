#include "framework.h"
#include "Kata-Game-of-Life.h"
#include "__Table.h"

auto g_table = WINDOWS_TABLE{ };

LRESULT CALLBACK WndProc(HWND hFrame, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: { g_hWnd = hFrame; g_table.InitializeTable(); } break;
    case WM_COMMAND: {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId) {
            case IDM_ABOUT: { DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hFrame, About); }break;
            case IDM_EXIT: { DestroyWindow(hFrame); } break;
            default: { return DefWindowProc(hFrame, message, wParam, lParam); } 
            }
        }
        break;
    case WM_PAINT: { } break;
    case WM_DESTROY: { PostQuitMessage(0); } break;
    default: { return DefWindowProc(hFrame, message, wParam, lParam); }
    }
    return 0;
}
