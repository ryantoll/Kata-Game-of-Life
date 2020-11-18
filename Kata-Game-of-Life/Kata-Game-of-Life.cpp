#include "framework.h"
#include "Kata-Game-of-Life.h"
#include "__Table.h"
using namespace std;
auto g_table = WINDOWS_TABLE{ };

LRESULT CALLBACK WndProc(HWND hFrame, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: { g_hWnd = hFrame; g_table.InitializeTable(); } break;
    case WM_COMMAND: {        
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId) {
            /*case WM_CTLCOLORSTATIC: {
                static HBRUSH hbrBkgnd;
                HDC hdcStatic = (HDC)wParam;
                SetTextColor(hdcStatic, RGB(255, 255, 255));
                SetBkColor(hdcStatic, RGB(0, 0, 0));

                if (hbrBkgnd == NULL)
                {
                    hbrBkgnd = CreateSolidBrush(RGB(0, 0, 0));
                }
                return (INT_PTR)hbrBkgnd;
            }*/
            case IDM_ABOUT: { DialogBox(g_hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hFrame, About); }break;
            case IDM_EXIT: { DestroyWindow(hFrame); } break;
            default: { 
                auto id = WINDOWS_TABLE::CELL_ID{ reinterpret_cast<HWND>(lParam) };
                auto text = L"Column: "s + to_wstring(id.Column())
                    + L", Row: "s + to_wstring(id.Row());
                //BeginPaint();
                return DefWindowProc(hFrame, message, wParam, lParam);
            } 
            }
        }
        break;
    case WM_PAINT: { } break;
    case WM_DESTROY: { PostQuitMessage(0); } break;
    default: { return DefWindowProc(hFrame, message, wParam, lParam); }
    }
    return 0;
}
