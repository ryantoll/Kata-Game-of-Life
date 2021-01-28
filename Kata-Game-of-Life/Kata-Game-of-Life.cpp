#include "framework.h"
#include "Kata-Game-of-Life.h"
#include "__Table.h"
using namespace std;
using namespace RYANS_UTILITIES;
using namespace RYANS_UTILITIES::WINDOWS_GUI;
auto g_table = WINDOWS_TABLE{ };
auto allCells = vector<WINDOW>{ };
LIFE_HISTORY history{ };
auto aliveBrush = CreateSolidBrush(RGB(0, 100, 0));
auto deadBrush = CreateSolidBrush(RGB(100, 0, 0));

LRESULT CALLBACK WndProc(HWND hFrame, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: { 
        g_hWnd = hFrame;
        g_table.InitializeTable();
        for (auto position : allPositions) {
            allCells.push_back(WINDOW{ WINDOWS_TABLE::CELL_ID{ position } });
        }
    } break;
    case WM_KEYDOWN: {
        switch (wParam) {
        case VK_RETURN: { 
            history.Advance();
            for (auto cell: allCells) { cell.Redraw();  }
        } break;
        }
    } break;
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
    case WM_DESTROY: { DeleteObject(aliveBrush); PostQuitMessage(0); } break;
    default: { return DefWindowProc(hFrame, message, wParam, lParam); }
    }
    return 0;
}


// This window proceedure is what gives cells much of their UI functionality.
// Features include using arrow keys to move between cells and responding to user focus.
// Without this, cells would only display text on character input.
LRESULT CALLBACK CellWindowProc(HWND hCell, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message)
    {
    case WM_LBUTTONDOWN: {
            auto& frame = history[history.Generation()];
            auto id = WINDOWS_TABLE::CELL_ID{ hCell };
            auto state = frame[id].State();
            //if (state == LIFE_STATE::DEAD) { frame[id].State(LIFE_STATE::ALIVE); }
            //else { frame[id].State(LIFE_STATE::DEAD); }
            frame[id].TogleDeadAlive();
            WINDOW{ id }.Redraw();
            SetFocus(g_hWnd);
        } break;
        case WM_PAINT: {
            const auto& frame = history[history.Generation()];
            auto id = WINDOWS_TABLE::CELL_ID{ hCell };
            auto state = frame.LifeState(CELL_POSITION{ id });
            auto myCell = WINDOW{ id };
            auto rekt = RECT{ };
            GetClientRect(myCell, &rekt);
            auto cellToken = myCell.BeginPaint();
            auto isAlive = EnumHasFlag(state, LIFE_STATE::ALIVE);
            if (isAlive) { FillRect(cellToken, &rekt, aliveBrush); }
            else { FillRect(cellToken, &rekt, deadBrush); }
        } break;
        default: { DefWindowProc(hCell, message, wParam, lParam); }
    }

    return DefWindowProc(hCell, message, wParam, lParam);;
}